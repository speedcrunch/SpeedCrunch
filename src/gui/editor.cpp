// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2004, 2005 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2005, 2006 Johan Thelin <e8johan@gmail.com>
// Copyright (C) 2007-2010, 2013, 2014 Helder Correia <helder.pereira.correia@gmail.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#include "gui/editor.h"

#include "core/constants.h"
#include "core/evaluator.h"
#include "core/functions.h"
#include "core/numberformatter.h"
#include "core/settings.h"
#include "gui/syntaxhighlighter.h"

#include <QtCore/QEvent>
#include <QtCore/QMimeData>
#include <QtCore/QTimeLine>
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QFont>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QKeyEvent>
#include <QtGui/QLineEdit>
#include <QtGui/QPainter>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QStyle>
#include <QtGui/QTreeWidget>
#include <QtGui/QWheelEvent>

#include <algorithm>

static void moveCursorToEnd(Editor* editor)
{
    QTextCursor cursor = editor->textCursor();
    cursor.movePosition(QTextCursor::EndOfBlock);
    editor->setTextCursor(cursor);
}

Editor::Editor(QWidget* parent)
    : QPlainTextEdit(parent)
{
    m_evaluator = Evaluator::instance();
    m_currentHistoryIndex = 0;
    m_isAutoCompletionEnabled = true;
    m_completion = new EditorCompletion(this);
    m_constantCompletion = 0;
    m_completionTimer = new QTimer(this);
    m_isAutoCalcEnabled = true;
    m_highlighter = new SyntaxHighlighter(this);
    m_autoCalcTimer = new QTimer(this);
    m_autoCalcSelTimer = new QTimer(this);
    m_matchingTimer = new QTimer(this);
    m_isAnsAvailable = false;
    m_shouldPaintCustomCursor = true;

    setViewportMargins(0, 0, 0, 0);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    setTabChangesFocus(true);
    setWordWrapMode(QTextOption::NoWrap);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCursorWidth(0);

    connect(m_autoCalcTimer, SIGNAL(timeout()), SLOT(autoCalc()));
    connect(m_autoCalcSelTimer, SIGNAL(timeout()), SLOT(autoCalcSelection()));
    connect(m_completion, SIGNAL(selectedCompletion(const QString&)), SLOT(autoComplete(const QString&)));
    connect(m_completionTimer, SIGNAL(timeout()), SLOT(triggerAutoComplete()));
    connect(m_matchingTimer, SIGNAL(timeout()), SLOT(doMatchingPar()));
    connect(this, SIGNAL(selectionChanged()), SLOT(startSelAutoCalcTimer()));
    connect(this, SIGNAL(textChanged()), SLOT(checkAutoCalc()));
    connect(this, SIGNAL(textChanged()), SLOT(checkAutoComplete()));
    connect(this, SIGNAL(textChanged()), SLOT(checkMatching()));

    QFont font;
    font.setPointSize(font.pointSize() + 4);
    font.setBold(true);
    setFont(font);

    adjustSize();
    setFixedHeight(sizeHint().height() + 4);
}

QString Editor::text() const
{
    return toPlainText();
}

void Editor::setText(const QString& text)
{
    setPlainText(text);
}

void Editor::insert(const QString& text)
{
    insertPlainText(text);
}

void Editor::doBackspace()
{
    QTextCursor cursor = textCursor();
    cursor.deletePreviousChar();
    setTextCursor(cursor);
}

int Editor::cursorPosition() const
{
    return textCursor().position();
}

void Editor::setCursorPosition(int position)
{
    QTextCursor cursor = textCursor();
    cursor.setPosition(position);
    setTextCursor(cursor);
}

QSize Editor::sizeHint() const
{
    ensurePolished();
    const QFontMetrics metrics = fontMetrics();
    const int margin = frameWidth() * 2;
    const int height = qMax(metrics.lineSpacing() + 2, 14) + margin;
    const int width = metrics.width('x') * 20 + margin;

    QSize size = QSize(width, height).expandedTo(QApplication::globalStrut());
    return style()->sizeFromContents(QStyle::CT_LineEdit, 0, size, this);
}

QStringList Editor::history() const
{
    return m_history;
}

QStringList Editor::historyResults() const
{
    return m_historyResults;
}

void Editor::setHistory(const QStringList& history)
{
    m_history = history;
    m_currentHistoryIndex = m_history.count();
}

void Editor::setHistoryResults(const QStringList& results)
{
    m_historyResults = results;
}

void Editor::appendHistory(const QStringList& expressions, const QStringList& results)
{
    m_history += expressions;
    m_historyResults += results;
    m_currentHistoryIndex = m_history.count();
}

void Editor::clearHistory()
{
    m_history.clear();
    m_historyResults.clear();
    m_currentHistoryIndex = 0;
}

bool Editor::isAutoCompletionEnabled() const
{
    return m_isAutoCompletionEnabled;
}

void Editor::setAutoCompletionEnabled(bool enable)
{
    m_isAutoCompletionEnabled = enable;
}

bool Editor::isAutoCalcEnabled() const
{
    return m_isAutoCalcEnabled;
}

void Editor::setAutoCalcEnabled(bool enable)
{
    m_isAutoCalcEnabled = enable;
}

void Editor::appendHistory(const QString& expression, const QString& result)
{
    if (expression.isEmpty() || result.isEmpty())
        return;

    m_history.append(expression);
    m_historyResults.append(result);
    m_currentHistoryIndex = m_history.count();
}

void Editor::checkAutoComplete()
{
    if (!m_isAutoCompletionEnabled)
        return;

    m_completionTimer->stop();
    m_completionTimer->setSingleShot(true);
    m_completionTimer->start(500);
}

void Editor::checkMatching()
{
    if (!Settings::instance()->syntaxHighlighting)
        return;

    m_matchingTimer->stop();
    m_matchingTimer->setSingleShot(true);
    m_matchingTimer->start(200);
}

void Editor::checkAutoCalc()
{
    if (!m_isAutoCalcEnabled)
        return;

    m_autoCalcTimer->stop();
    m_autoCalcTimer->setSingleShot(true);
    m_autoCalcTimer->start(1);

    emit autoCalcDisabled();
}

void Editor::startSelAutoCalcTimer()
{
    if (!m_isAutoCalcEnabled)
        return;

    m_autoCalcSelTimer->stop();
    m_autoCalcSelTimer->setSingleShot(true);
    m_autoCalcSelTimer->start(1);
}

void Editor::doMatchingPar()
{
    // Clear previous.
    setExtraSelections(QList<QTextEdit::ExtraSelection>());

    if (!Settings::instance()->syntaxHighlighting)
        return;

    doMatchingLeft();
    doMatchingRight();
}

void Editor::doMatchingLeft()
{
    // Tokenize the expression.
    const int currentPosition = textCursor().position();

    // Check for right par.
    QString subtext = text().left(currentPosition);
    Tokens tokens = m_evaluator->scan(subtext);
    if (!tokens.valid() || tokens.count() < 1)
        return;
    Token lastToken = tokens.at(tokens.count() - 1);

    // Right par?
    if (lastToken.type() == Token::stxClosePar && lastToken.pos() == currentPosition - 1) {
        // Find the matching left par.
        unsigned par = 1;
        int matchPosition = -1;

        for (int i = tokens.count() - 2; i >= 0 && par > 0; --i) {
            Token matchToken = tokens.at(i);
            switch (matchToken.type()) {
                case Token::stxOpenPar : --par; break;
                case Token::stxClosePar: ++par; break;
                default:;
            }
            matchPosition = matchToken.pos();
        }

        if (par == 0) {
            QTextEdit::ExtraSelection hilite1;
            hilite1.cursor = textCursor();
            hilite1.cursor.setPosition(matchPosition);
            hilite1.cursor.setPosition(matchPosition + 1, QTextCursor::KeepAnchor);
            hilite1.format.setBackground(m_highlighter->colorForRole(SyntaxHighlighter::Matched));

            QTextEdit::ExtraSelection hilite2;
            hilite2.cursor = textCursor();
            hilite2.cursor.setPosition(lastToken.pos());
            hilite2.cursor.setPosition(lastToken.pos() + 1, QTextCursor::KeepAnchor);
            hilite2.format.setBackground(m_highlighter->colorForRole(SyntaxHighlighter::Matched));

            QList<QTextEdit::ExtraSelection> extras;
            extras << hilite1;
            extras << hilite2;
            setExtraSelections(extras);
        }
    }
}

void Editor::doMatchingRight()
{
    // Tokenize the expression.
    const int currentPosition = textCursor().position();

    // Check for left par.
    QString subtext = text().right(text().length() - currentPosition);
    Tokens tokens = m_evaluator->scan(subtext);
    if (!tokens.valid() || tokens.count() < 1)
        return;
    Token firstToken = tokens.at(0);

    // Left par?
    if (firstToken.type() == Token::stxOpenPar && firstToken.pos() == 0) {
        // Find the matching right par.
        unsigned par = 1;
        int k = 0;
        Token matchToken;
        int matchPosition = -1;

        for (k = 1; k < tokens.count() && par > 0; ++k) {
            const Token matchToken = tokens.at(k);
            switch (matchToken.type()) {
                case Token::stxOpenPar : ++par; break;
                case Token::stxClosePar: --par; break;
                default:;
            }
            matchPosition = matchToken.pos();
        }

        if (par == 0) {
            QTextEdit::ExtraSelection hilite1;
            hilite1.cursor = textCursor();
            hilite1.cursor.setPosition(currentPosition+matchPosition);
            hilite1.cursor.setPosition(currentPosition+matchPosition + 1, QTextCursor::KeepAnchor);
            hilite1.format.setBackground(m_highlighter->colorForRole(SyntaxHighlighter::Matched));

            QTextEdit::ExtraSelection hilite2;
            hilite2.cursor = textCursor();
            hilite2.cursor.setPosition(currentPosition+firstToken.pos());
            hilite2.cursor.setPosition(currentPosition+firstToken.pos() + 1, QTextCursor::KeepAnchor);
            hilite2.format.setBackground(m_highlighter->colorForRole(SyntaxHighlighter::Matched));

            QList<QTextEdit::ExtraSelection> extras;
            extras << hilite1;
            extras << hilite2;
            setExtraSelections(extras);
        }
    }
}

void Editor::triggerAutoComplete()
{
    if (!m_isAutoCompletionEnabled)
        return;

    // Tokenize the expression (this is very fast).
    const int currentPosition = textCursor().position();
    QString subtext = text().left(currentPosition);
    const Tokens tokens = m_evaluator->scan(subtext);
    if (!tokens.valid() || tokens.count() < 1)
        return;

    Token lastToken = tokens.at(tokens.count()-1);

    // Last token must be an identifier.
    if (!lastToken.isIdentifier())
        return;
    const QString id = lastToken.text();
    if (id.length() < 1)
        return;

    // No space after identifier.
    if (lastToken.pos() + id.length() < subtext.length())
        return;

    // Find matches in function names.
    const QStringList fnames = FunctionRepo::instance()->getIdentifiers();
    QStringList choices;
    for (int i = 0; i < fnames.count(); ++i) {
        if (fnames.at(i).startsWith(id, Qt::CaseInsensitive)) {
            QString str = fnames.at(i);
            Function* f = FunctionRepo::instance()->find(str);
            if (f)
                str.append(':').append(f->name());
            choices.append(str);
        }
    }
    choices.sort();

    // Find matches in variables names.
    QStringList vchoices;
    QList<Evaluator::Variable> variables = m_evaluator->getVariables();
    for (int i = 0; i < variables.count(); ++i)
        if (variables.at(i).name.startsWith(id, Qt::CaseInsensitive))
            vchoices.append(QString("%1: %2").arg(variables.at(i).name)
                .arg(NumberFormatter::format(variables.at(i).value)));
    vchoices.sort();
    choices += vchoices;

    // No match, don't bother with completion.
    if (!choices.count())
        return;

    // Single perfect match, no need to give choices.
    if (choices.count() == 1)
        if (choices.at(0).toLower() == id.toLower())
            return;

    // One match, complete it for the user.
    if (choices.count() == 1) {
        QString str = choices.at(0).split(':').at(0);
        str = str.remove(0, id.length());
        const int currentPosition = textCursor().position();
        if (textCursor().selectionStart() == textCursor().selectionEnd()) {
            blockSignals(true);
            QTextCursor cursor = textCursor();
            cursor.clearSelection();
            cursor.insertText(str);
            cursor.setPosition(currentPosition);
            cursor.setPosition(currentPosition+str.length(), QTextCursor::KeepAnchor);
            setTextCursor(cursor);
            blockSignals(false);
        }
        return;
    }

    // Present the user with completion choices.
    m_completion->showCompletion(choices);
}

void Editor::autoComplete(const QString& item)
{
    if (!m_isAutoCompletionEnabled || item.isEmpty())
        return;

    const int currentPosition = textCursor().position();
    const QString subtext = text().left(currentPosition);
    const Tokens tokens = m_evaluator->scan(subtext);
    if (!tokens.valid() || tokens.count() < 1)
        return;

    const Token lastToken = tokens.at(tokens.count() - 1);
    if (!lastToken.isIdentifier())
        return;

    const QStringList str = item.split(':');

    blockSignals(true);
    QTextCursor cursor = textCursor();
    cursor.setPosition(lastToken.pos());
    cursor.setPosition(lastToken.pos() + lastToken.text().length(), QTextCursor::KeepAnchor);
    setTextCursor(cursor);
    insert(str.at(0));
    blockSignals(false);
}

void Editor::insertFromMimeData(const QMimeData* source)
{
    QStringList expressions = source->text().split('\n', QString::SkipEmptyParts);
    if (expressions.size() == 1) {
        QPlainTextEdit::insertFromMimeData(source);
        return;
    }

    for (int i = 0; i < expressions.size(); ++i) {
        insert(expressions.at(i));
        evaluate();
    }
}

void Editor::autoCalc()
{
    if (!m_isAutoCalcEnabled)
        return;

    const QString str = m_evaluator->autoFix(text());
    if (str.isEmpty())
        return;

    // Very short (just one token) and still no calculation, then skip.
    if (!m_isAnsAvailable) {
        const Tokens tokens = m_evaluator->scan(text());
        if (tokens.count() < 2)
            return;
    }

    // Too short even after autofix? Do not bother either.
    const Tokens tokens = m_evaluator->scan(str);
    if (tokens.count() < 2)
        return;

    // Same reason as above, do not update "ans".
    m_evaluator->setExpression(str);
    const HNumber num = m_evaluator->evalNoAssign();

    if (m_evaluator->error().isEmpty()) {
        const QString message = tr("Current result: <b>%1</b>").arg(NumberFormatter::format(num));
        emit autoCalcEnabled(message);
    } else
        emit autoCalcEnabled(m_evaluator->error());
}

void Editor::autoCalcSelection()
{
    if (!m_isAutoCalcEnabled)
        return;

    const QString str = m_evaluator->autoFix(textCursor().selectedText());
    if (str.isEmpty())
        return;

    // Very short (just one token) and still no calculation, then skip.
    if (!m_isAnsAvailable) {
        const Tokens tokens = m_evaluator->scan(text());
        if (tokens.count() < 2)
            return;
    }

    // Too short even after autofix? Don't bother either.
    const Tokens tokens = m_evaluator->scan(str);
    if (tokens.count() < 2)
        return;

    // Same reason as above, do not update "ans".
    m_evaluator->setExpression(str);
    const HNumber num = m_evaluator->evalNoAssign();

    if (m_evaluator->error().isEmpty()) {
        const QString message = tr("Selection result: <b>%1</b>").arg(NumberFormatter::format(num));
        emit autoCalcEnabled(message);
    } else
        emit autoCalcEnabled(m_evaluator->error());
}

void Editor::insertConstant(const QString& constant)
{
    QString formattedConstant = constant;
    if (Settings::instance()->radixCharacter() == ',')
        formattedConstant.replace('.', ',');
    if (!constant.isNull())
        insert(formattedConstant);
    if (m_constantCompletion) {
        disconnect(m_constantCompletion);
        m_constantCompletion->deleteLater();
        m_constantCompletion = 0;
    }
}

void Editor::cancelConstantCompletion()
{
    if (m_constantCompletion) {
        disconnect(m_constantCompletion);
        m_constantCompletion->deleteLater();
        m_constantCompletion = 0;
    }
}

void Editor::evaluate()
{
    triggerEnter();
}

void Editor::paintEvent(QPaintEvent* event)
{
    QPlainTextEdit::paintEvent(event);

    if (!m_shouldPaintCustomCursor) {
        m_shouldPaintCustomCursor = true;
        return;
    }
    m_shouldPaintCustomCursor = false;

    QRect cursor = cursorRect();
    cursor.setLeft(cursor.left() - 1);
    cursor.setRight(cursor.right() + 1);

    QPainter painter(viewport());
    painter.fillRect(cursor, m_highlighter->colorForRole(SyntaxHighlighter::Cursor));
}

void Editor::historyBack()
{
    if (!m_history.count())
        return;
    if (!m_currentHistoryIndex)
        return;

    if (m_currentHistoryIndex == m_history.count())
        m_savedCurrentEditor = toPlainText();
    --m_currentHistoryIndex;
    setText(m_history.at(m_currentHistoryIndex));
    moveCursorToEnd(this);
    ensureCursorVisible();
}

void Editor::historyForward()
{
    if (!m_history.count())
        return;
    if (m_currentHistoryIndex == m_history.count())
        return;

    m_currentHistoryIndex++;
    if (m_currentHistoryIndex == m_history.count())
        setText(m_savedCurrentEditor);
    else
        setText(m_history.at(m_currentHistoryIndex));
    moveCursorToEnd(this);
    ensureCursorVisible();
}

void Editor::triggerEnter()
{
    m_completionTimer->stop();
    m_matchingTimer->stop();
    m_autoCalcTimer->stop();
    m_autoCalcSelTimer->stop();
    m_currentHistoryIndex = m_history.count();
    emit returnPressed();
}

void Editor::focusOutEvent(QFocusEvent* event)
{
    m_shouldPaintCustomCursor = false;
    QPlainTextEdit::focusOutEvent(event);
}

void Editor::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Enter) {
        QTimer::singleShot(0, this, SLOT(triggerEnter()));
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_Return) {
        QTimer::singleShot(0, this, SLOT(triggerEnter()));
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_Up) {
        if (event->modifiers() & Qt::ShiftModifier)
            emit shiftUpPressed();
        else
            historyBack();
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_Down) {
        if (event->modifiers() & Qt::ShiftModifier)
            emit shiftDownPressed();
        else
            historyForward();
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_PageUp) {
        if (event->modifiers() & Qt::ShiftModifier)
            emit shiftPageUpPressed();
        else if (event->modifiers() & Qt::ControlModifier)
            emit controlPageUpPressed();
        else
            emit pageUpPressed();
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_PageDown) {
        if (event->modifiers() & Qt::ShiftModifier)
            emit shiftPageDownPressed();
        else if (event->modifiers() & Qt::ControlModifier)
            emit controlPageDownPressed();
        else
            emit pageDownPressed();
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_Left
        || event->key() == Qt::Key_Right
        || event->key() == Qt::Key_Home
        || event->key() == Qt::Key_End)
    {
        checkMatching();
    }

    if (event->key() == Qt::Key_Space
        && event->modifiers() == Qt::ControlModifier
        && !m_constantCompletion)
    {
        m_constantCompletion = new ConstantCompletion(this);
        connect(m_constantCompletion, SIGNAL(selectedCompletion(const QString&)), SLOT(insertConstant(const QString&)));
        connect(m_constantCompletion, SIGNAL(canceledCompletion()), SLOT(cancelConstantCompletion()));
        m_constantCompletion->showCompletion();
        event->accept();
        return;
    }

    if (event->matches(QKeySequence::Copy)) {
        emit copySequencePressed();
        event->accept();
        return;
    }

    QPlainTextEdit::keyPressEvent(event);
}

void Editor::wheelEvent(QWheelEvent* event)
{
    if (event->delta() > 0)
        historyBack();
    else if (event->delta() < 0)
        historyForward();
    event->accept();
}

void Editor::rehighlight()
{
    m_highlighter->update();
    setStyleSheet(QString(
        "QPlainTextEdit {"
        "   background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %2, stop: 1 %1);"
        "}"
    ).arg(m_highlighter->colorForRole(SyntaxHighlighter::Background).name())
     .arg(m_highlighter->colorForRole(SyntaxHighlighter::EditorFade).name())
    );
}

void Editor::setAnsAvailable(bool available)
{
    m_isAnsAvailable = available;
}

void Editor::stopAutoCalc()
{
    m_autoCalcTimer->stop();
    m_autoCalcSelTimer->stop();
    emit autoCalcDisabled();
}

void Editor::stopAutoComplete()
{
    m_completionTimer->stop();
    m_completion->selectItem(QString());
    m_completion->doneCompletion();
    setFocus();
}

EditorCompletion::EditorCompletion(Editor* editor)
    : QObject(editor)
{
    m_editor = editor;

    m_popup = new QTreeWidget();
    m_popup->setFrameShape(QFrame::NoFrame);
    m_popup->setColumnCount(2);
    m_popup->setRootIsDecorated(false);
    m_popup->header()->hide();
    m_popup->setEditTriggers(QTreeWidget::NoEditTriggers);
    m_popup->setSelectionBehavior(QTreeWidget::SelectRows);
    m_popup->setMouseTracking(true);
    m_popup->installEventFilter(this);

    connect(m_popup, SIGNAL(itemClicked(QTreeWidgetItem*, int)), SLOT(doneCompletion()));

    m_popup->hide();
    m_popup->setParent(0, Qt::Popup);
    m_popup->setFocusPolicy(Qt::NoFocus);
    m_popup->setFocusProxy(editor);
    m_popup->setFrameStyle(QFrame::Box | QFrame::Plain);
}

EditorCompletion::~EditorCompletion()
{
    delete m_popup;
}

bool EditorCompletion::eventFilter(QObject* object, QEvent* event)
{
    if (object == m_popup) {

        if (event->type() == QEvent::KeyPress) {
            QKeyEvent* keyEvent = (QKeyEvent*)event;

            if (keyEvent->key() == Qt::Key_Enter
                || keyEvent->key() == Qt::Key_Return
                || keyEvent->key() == Qt::Key_Tab)
            {
                doneCompletion();
                return true;
            }

            if (keyEvent->key() == Qt::Key_Up
                || keyEvent->key() == Qt::Key_Down
                || keyEvent->key() == Qt::Key_Home
                || keyEvent->key() == Qt::Key_End
                || keyEvent->key() == Qt::Key_PageUp
                || keyEvent->key() == Qt::Key_PageDown)
            {
                return false;
            }

            m_popup->hide();
            m_editor->setFocus();
            if (keyEvent->key() != Qt::Key_Escape)
                QApplication::sendEvent(m_editor, event);
            return true;
        }

        if (event->type() == QEvent::MouseButtonPress) {
            m_popup->hide();
            m_editor->setFocus();
            return true;
        }
    }

    return false;
}

void EditorCompletion::doneCompletion()
{
    m_popup->hide();
    m_editor->setFocus();
    QTreeWidgetItem* item = m_popup->currentItem();
    emit selectedCompletion(item ? item->text(0) : QString());
}

void EditorCompletion::showCompletion(const QStringList& choices)
{
    if (!choices.count())
        return;

    m_popup->setUpdatesEnabled(false);
    m_popup->clear();
    for (int i = 0; i < choices.count(); ++i) {
        QTreeWidgetItem* item = new QTreeWidgetItem(m_popup, choices.at(i).split(':'));
        if (item && m_editor->layoutDirection() == Qt::RightToLeft)
            item->setTextAlignment(0, Qt::AlignRight);
    }
    m_popup->sortItems(0, Qt::AscendingOrder);
    m_popup->sortItems(1, Qt::AscendingOrder);
    m_popup->setCurrentItem(m_popup->topLevelItem(0));
    m_popup->adjustSize();
    m_popup->setUpdatesEnabled(true);

    // Size of the pop-up.
    m_popup->adjustSize();
    const int height = m_popup->sizeHintForRow(0) * qMin(7, choices.count()) + 3;
    const int width = m_popup->width();
    m_popup->resize(width, height);

    // Position, reference is editor's cursor position in global coord.
    QFontMetrics metrics(m_editor->font());
    QTextCursor cursor = m_editor->textCursor();
    cursor.movePosition(QTextCursor::StartOfWord);
    const int pixelsOffset = metrics.width(m_editor->text(), cursor.position());
    QPoint position = m_editor->mapToGlobal(QPoint(pixelsOffset, m_editor->height()));

    // If popup is partially invisible, move to other position.
    const QRect screen = QApplication::desktop()->availableGeometry(m_editor);
    if (position.y() + height > screen.y() + screen.height())
        position.setY(position.y() - height - m_editor->height());
    if (position.x() + width > screen.x() + screen.width())
        position.setX(screen.x() + screen.width() - width);

    m_popup->move(position);
    m_popup->setFocus();
    m_popup->show();
}

void EditorCompletion::selectItem(const QString& item)
{
    if (item.isNull()) {
        m_popup->setCurrentItem(0);
        return;
    }

    QList<QTreeWidgetItem*> targets = m_popup->findItems(item, Qt::MatchExactly);
    if (targets.count() > 0)
        m_popup->setCurrentItem(targets.at(0));
}

ConstantCompletion::ConstantCompletion(Editor* editor)
    : QObject(editor)
{
    m_editor = editor;

    m_popup = new QFrame;
    m_popup->setParent(0, Qt::Popup);
    m_popup->setFocusPolicy(Qt::NoFocus);
    m_popup->setFocusProxy(editor);
    m_popup->setFrameStyle(QFrame::Box | QFrame::Plain);

    m_categoryWidget = new QTreeWidget(m_popup);
    m_categoryWidget->setFrameShape(QFrame::NoFrame);
    m_categoryWidget->setColumnCount(1);
    m_categoryWidget->setRootIsDecorated(false);
    m_categoryWidget->header()->hide();
    m_categoryWidget->setEditTriggers(QTreeWidget::NoEditTriggers);
    m_categoryWidget->setSelectionBehavior(QTreeWidget::SelectRows);
    m_categoryWidget->setMouseTracking(true);
    m_categoryWidget->installEventFilter(this);
    m_categoryWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    connect(m_categoryWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), SLOT(showConstants()));

    m_constantWidget = new QTreeWidget(m_popup);
    m_constantWidget->setFrameShape(QFrame::NoFrame);
    m_constantWidget->setColumnCount(2);
    m_constantWidget->setColumnHidden(1, true);
    m_constantWidget->setRootIsDecorated(false);
    m_constantWidget->header()->hide();
    m_constantWidget->setEditTriggers(QTreeWidget::NoEditTriggers);
    m_constantWidget->setSelectionBehavior(QTreeWidget::SelectRows);
    m_constantWidget->setMouseTracking(true);
    m_constantWidget->installEventFilter(this);
    m_constantWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    connect(m_constantWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), SLOT(doneCompletion()));

    m_slider = new QTimeLine(250, m_popup);
    m_slider->setCurveShape(QTimeLine::EaseInCurve);
    connect(m_slider, SIGNAL(frameChanged(int)), SLOT(setHorizontalPosition(int)));

    const Constants* constants = Constants::instance();
    m_constantList = constants->list();

    // Populate categories.
    QStringList categoryList;
    categoryList << tr("All");
    QTreeWidgetItem* all = new QTreeWidgetItem(m_categoryWidget, categoryList);
    for (int i = 0; i < constants->categories().count(); ++i) {
        categoryList.clear();
        categoryList << constants->categories().at(i);
        new QTreeWidgetItem(m_categoryWidget, categoryList);
    }
    m_categoryWidget->setCurrentItem(all);

    // Populate constants.
    m_lastCategory = tr("All");
    for (int i = 0; i < constants->list().count(); ++i) {
        QStringList names;
        names << constants->list().at(i).name;
        names << constants->list().at(i).name.toUpper();
        new QTreeWidgetItem(m_constantWidget, names);
    }
    m_constantWidget->sortItems(0, Qt::AscendingOrder);

    // Find size, the biggest between both.
    m_constantWidget->resizeColumnToContents(0);
    m_categoryWidget->resizeColumnToContents(0);
    int width = qMax(m_constantWidget->width(), m_categoryWidget->width());
    const int constantsHeight = m_constantWidget->sizeHintForRow(0) * qMin(7, m_constantList.count()) + 3;
    const int categoriesHeight = m_categoryWidget->sizeHintForRow(0) * qMin(7, constants->categories().count()) + 3;
    const int height = qMax(constantsHeight, categoriesHeight);
    width += 200; // Extra space (FIXME: scrollbar size?).

    // Adjust dimensions.
    m_popup->resize(width, height);
    m_constantWidget->resize(width, height);
    m_categoryWidget->resize(width, height);
}

ConstantCompletion::~ConstantCompletion()
{
    delete m_popup;
    m_editor->setFocus();
}

void ConstantCompletion::showCategory()
{
    m_slider->setFrameRange(m_popup->width(), 0);
    m_slider->stop();
    m_slider->start();
    m_categoryWidget->setFocus();
}

void ConstantCompletion::showConstants()
{
    m_slider->setFrameRange(0, m_popup->width());
    m_slider->stop();
    m_slider->start();
    m_constantWidget->setFocus();

    QString chosenCategory;
    if (m_categoryWidget->currentItem())
        chosenCategory = m_categoryWidget->currentItem()->text(0);

    if (m_lastCategory == chosenCategory)
        return;

    m_constantWidget->clear();

    for (int i = 0; i < m_constantList.count(); ++i) {
        QStringList names;
        names << m_constantList.at(i).name;
        names << m_constantList.at(i).name.toUpper();

        const bool include = (chosenCategory == tr("All")) ?
            true : (m_constantList.at(i).category == chosenCategory);

        if (!include)
            continue;

        new QTreeWidgetItem(m_constantWidget, names);
    }

    m_constantWidget->sortItems(0, Qt::AscendingOrder);
    m_constantWidget->setCurrentItem(m_constantWidget->itemAt(0, 0));
    m_lastCategory = chosenCategory;
}

bool ConstantCompletion::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::Hide) {
        emit canceledCompletion();
        return true;
    }

    if (object == m_constantWidget) {

        if (event->type() == QEvent::KeyPress) {
            QKeyEvent* keyEvent = (QKeyEvent*)event;

            if (keyEvent->key() == Qt::Key_Enter
                || keyEvent->key() == Qt::Key_Return
                || keyEvent->key() == Qt::Key_Tab)
            {
                doneCompletion();
                return true;
            }

            if (keyEvent->key() == Qt::Key_Left) {
                showCategory();
                return true;
            }

            if (keyEvent->key() == Qt::Key_Right
                || keyEvent->key() == Qt::Key_Up
                || keyEvent->key() == Qt::Key_Down
                || keyEvent->key() == Qt::Key_Home
                || keyEvent->key() == Qt::Key_End
                || keyEvent->key() == Qt::Key_PageUp
                || keyEvent->key() == Qt::Key_PageDown)
            {
                return false;
            }

            if (keyEvent->key() != Qt::Key_Escape)
                QApplication::sendEvent(m_editor, event);
            emit canceledCompletion();
            return true;
        }
    }

    if (object == m_categoryWidget) {

        if (event->type() == QEvent::KeyPress) {
            QKeyEvent* keyEvent = (QKeyEvent*)event;

            if (keyEvent->key() == Qt::Key_Enter
                || keyEvent->key() == Qt::Key_Return
                || keyEvent->key() == Qt::Key_Right)
            {
                showConstants();
                return true;
            }

            if (keyEvent->key() == Qt::Key_Up
                || keyEvent->key() == Qt::Key_Down
                || keyEvent->key() == Qt::Key_Home
                || keyEvent->key() == Qt::Key_End
                || keyEvent->key() == Qt::Key_PageUp
                || keyEvent->key() == Qt::Key_PageDown)
            {
                return false;
            }

            if (keyEvent->key() != Qt::Key_Escape)
                QApplication::sendEvent(m_editor, event);
            emit canceledCompletion();
            return true;
        }
    }

    return false;
}

void ConstantCompletion::doneCompletion()
{
    m_editor->setFocus();
    const QTreeWidgetItem* item = m_constantWidget->currentItem();
    emit selectedCompletion(item ?
        std::find_if(m_constantList.begin(), m_constantList.end(),
            constant_name_is(item->text(0)))->value
        : QString());
}

void ConstantCompletion::showCompletion()
{
    // Position, reference is editor's cursor position in global coord.
    QFontMetrics metrics(m_editor->font());
    const int currentPosition = m_editor->textCursor().position();
    const int pixelsOffset = metrics.width(m_editor->text(), currentPosition);
    QPoint position = m_editor->mapToGlobal(QPoint(pixelsOffset, m_editor->height()));

    const int height = m_popup->height();
    const int width = m_popup->width();

    // If popup is partially invisible, move to other position.
    const QRect screen = QApplication::desktop()->availableGeometry(m_editor);
    if (position.y() + height > screen.y() + screen.height())
        position.setY(position.y() - height - m_editor->height());
    if (position.x() + width > screen.x() + screen.width())
        position.setX(screen.x() + screen.width() - width);

    // Start with category.
    m_categoryWidget->setFocus();
    setHorizontalPosition(0);

    m_popup->move(position);
    m_popup->show();
}

void ConstantCompletion::setHorizontalPosition(int x)
{
    m_categoryWidget->move(-x, 0);
    m_constantWidget->move(m_popup->width() - x, 0);
}