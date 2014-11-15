// This file is part of the SpeedCrunch project
// Copyright (C) 2009, 2011, 2013, 2014 Helder Correia <helder.pereira.correia@gmail.com>
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

#include "gui/resultdisplay.h"

#include "core/functions.h"
#include "core/numberformatter.h"
#include "core/settings.h"
#include "gui/syntaxhighlighter.h"
#include "math/hmath.h"
#include "math/floatconfig.h"

#include <QLatin1String>
#include <QApplication>
#include <QClipboard>
#include <QPainter>
#include <QScrollBar>

#define OVERLAY_GRADIENT_HEIGHT 50

class FadeOverlay : public QWidget {
public:
    explicit FadeOverlay(QWidget*);
    void updateGradients();
protected:
    virtual void paintEvent(QPaintEvent*);
    virtual void resizeEvent(QResizeEvent*);
private:
    QImage m_bottomGradientImage;
    QImage m_topGradientImage;
};

ResultDisplay::ResultDisplay(QWidget* parent)
    : QPlainTextEdit(parent)
    , m_count(0)
    , m_highlighter(new SyntaxHighlighter(this))
    , m_fadeOverlay(new FadeOverlay(this))
    , m_scrolledLines(0)
    , m_scrollDirection(0)
    , m_isScrollingPageOnly(false)
{
    setViewportMargins(0, 0, 0, 0);
    setBackgroundRole(QPalette::Base);
    setLayoutDirection(Qt::LeftToRight);
    setMinimumWidth(150);
    setReadOnly(true);
    setFocusPolicy(Qt::NoFocus);
    setWordWrapMode(QTextOption::WrapAnywhere);
}

void ResultDisplay::append(const QString& expression, const HNumber& value)
{
    ++m_count;

    appendPlainText(expression);
    if (!value.isNan())
        appendPlainText(QLatin1String("= ") + NumberFormatter::format(value));
    appendPlainText(QLatin1String(""));

    // TODO: Refactor, this only serves to save a session.
    m_expressions.append(expression);
    if (value.isNan()) {
        m_results.append("");
    } else {
        const char format = value.format() != 0 ? value.format() : 'e';
        char* str = HMath::format(value, format, DECPRECISION);
        m_results.append(str);
        free(str);
    }
}

void ResultDisplay::appendHistory(const QStringList& expressions, const QStringList& results)
{
    const int count = expressions.count();
    for (int i = 0 ; i < count; ++i) {
        QString str = results.at(i);
        str.replace(',', '.');
        HNumber result(str.toLatin1().constData());

        if (str.indexOf('b') == 1)
            result.setFormat('b');
        else if (str.indexOf('o') == 1)
            result.setFormat('o');
        else if (str.indexOf('x') == 1)
            result.setFormat('h');

        append(expressions.at(i), result);
    }
}

int ResultDisplay::count() const
{
    return m_count;
}

void ResultDisplay::rehighlight()
{
    m_highlighter->update();
    updateScrollBarStyleSheet();
    m_fadeOverlay->updateGradients();
}

void ResultDisplay::clear()
{
    m_count = 0;
    m_expressions.clear();
    m_results.clear();
    setPlainText(QLatin1String(""));
}

void ResultDisplay::refresh()
{
    const QStringList expressions = m_expressions;
    const QStringList results = m_results;
    clear();
    appendHistory(expressions, results);
}

void ResultDisplay::scrollLines(int numberOfLines)
{
    QScrollBar* bar = verticalScrollBar();
    bar->setValue(bar->value() + numberOfLines);
}

void ResultDisplay::scrollLineUp()
{
    if (m_scrollDirection != 0) {
        stopActiveScrollingAnimation();
        return;
    }

    scrollLines(-1);
}

void ResultDisplay::scrollLineDown()
{
    if (m_scrollDirection != 0) {
        stopActiveScrollingAnimation();
        return;
    }

    scrollLines(1);
}

void ResultDisplay::scrollToDirection(int direction)
{
    m_scrolledLines = 0;
    bool mustStartTimer = (m_scrollDirection == 0);
    m_scrollDirection = direction;
    if (mustStartTimer)
        m_scrollTimer.start(16, this);
}

void ResultDisplay::scrollPageUp()
{
    if (verticalScrollBar()->value() == 0)
        return;

    m_isScrollingPageOnly = true;
    scrollToDirection(-1);
}

void ResultDisplay::scrollPageDown()
{
    if (verticalScrollBar()->value() == verticalScrollBar()->maximum())
        return;

    m_isScrollingPageOnly = true;
    scrollToDirection(1);
}

void ResultDisplay::scrollToTop()
{
    if (verticalScrollBar()->value() == 0)
        return;

    m_isScrollingPageOnly = false;
    scrollToDirection(-1);
}

void ResultDisplay::scrollToBottom()
{
    if (verticalScrollBar()->value() == verticalScrollBar()->maximum())
        return;

    m_isScrollingPageOnly = false;
    scrollToDirection(1);
}

void ResultDisplay::increaseFontPointSize()
{
    QFont newFont = font();
    const int newSize = newFont.pointSize() + 1;
    if (newSize > 96)
        return;
    newFont.setPointSize(newSize);
    setFont(newFont);
}

void ResultDisplay::decreaseFontPointSize()
{
    QFont newFont = font();
    const int newSize = newFont.pointSize() - 1;
    if (newSize < 8)
        return;
    newFont.setPointSize(newSize);
    setFont(newFont);
}

void ResultDisplay::mouseDoubleClickEvent(QMouseEvent*)
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    setTextCursor(cursor);
    QString text = cursor.selectedText();
    QString resultMarker = QLatin1String("= ");
    if (text.startsWith(resultMarker))
        text.remove(resultMarker);
    emit expressionSelected(text);
}

void ResultDisplay::timerEvent(QTimerEvent* event)
{
    if (event->timerId() != m_scrollTimer.timerId()) {
        QWidget::timerEvent(event);
        return;
    }

    if (m_isScrollingPageOnly)
        pageScrollEvent();
    else
        fullContentScrollEvent();
}

void ResultDisplay::pageScrollEvent()
{
    if (m_scrolledLines >= linesPerPage()) {
        stopActiveScrollingAnimation();
        return;
    }

    scrollLines(m_scrollDirection * 2);
    m_scrolledLines += 2;
}

void ResultDisplay::fullContentScrollEvent()
{
    QScrollBar* bar = verticalScrollBar();
    int value = bar->value();
    bool shouldStop = (m_scrollDirection == -1 && value <= 0) || (m_scrollDirection == 1 && value >= bar->maximum());

    if (shouldStop && m_scrollDirection != 0) {
        stopActiveScrollingAnimation();
        return;
    }

    scrollLines(m_scrollDirection * 10);
}

void ResultDisplay::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() == (Qt::ShiftModifier | Qt::ControlModifier)) {
        if (event->delta() > 0)
            emit shiftControlWheelUp();
        else
            emit shiftControlWheelDown();
    } else if (event->modifiers() == Qt::ShiftModifier) {
        if (event->delta() > 0)
            emit shiftWheelUp();
        else
            emit shiftWheelDown();
    } else {
        QPlainTextEdit::wheelEvent(event);
        return;
    }

    event->accept();
}

void ResultDisplay::resizeEvent(QResizeEvent* event)
{
    QPlainTextEdit::resizeEvent(event);
    m_fadeOverlay->resize(event->size());
}

void ResultDisplay::stopActiveScrollingAnimation()
{
    m_scrollTimer.stop();
    m_scrolledLines = 0;
    m_scrollDirection = 0;
}

void ResultDisplay::updateScrollBarStyleSheet()
{
    verticalScrollBar()->setStyleSheet(QString(
        "QScrollBar:vertical {"
        "   border: 0;"
        "   margin: 0 0 0 0;"
        "   background: %1;"
        "   width: 5px;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: %2;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "   border: 0;"
        "   width: 0;"
        "   height: 0;"
        "   background: %1;"
        "}"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
        "   background: %1;"
        "}"
    ).arg(m_highlighter->colorForRole(SyntaxHighlighter::Background).name())
     .arg(m_highlighter->colorForRole(SyntaxHighlighter::ScrollBar).name()));
}

FadeOverlay::FadeOverlay(QWidget* parent) : QWidget(parent)
{
    setPalette(Qt::transparent);
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void FadeOverlay::paintEvent(QPaintEvent*)
{
    ResultDisplay* widget = qobject_cast<ResultDisplay*>(parent());
    int scrollBarValue = widget->verticalScrollBar()->value();
    bool shouldPaintTopGradient = (scrollBarValue > 0);
    bool shouldPaintBottomGradient = scrollBarValue < widget->verticalScrollBar()->maximum();

    QPainter painter(this);
    if (shouldPaintTopGradient)
        painter.drawImage(0, 0, m_topGradientImage);
    if (shouldPaintBottomGradient)
        painter.drawImage(0, size().height() - OVERLAY_GRADIENT_HEIGHT, m_bottomGradientImage);
}

void FadeOverlay::resizeEvent(QResizeEvent*)
{
    updateGradients();
}

void FadeOverlay::updateGradients()
{
    ResultDisplay* widget = qobject_cast<ResultDisplay*>(parent());
    QColor backgroundColor = widget->m_highlighter->colorForRole(SyntaxHighlighter::Background);

    QLinearGradient gradient = QLinearGradient(0, 0, 0, OVERLAY_GRADIENT_HEIGHT);
    gradient.setColorAt(0, backgroundColor);
    backgroundColor.setAlpha(0);
    gradient.setColorAt(1, backgroundColor);

    m_topGradientImage = QImage(size().width(), OVERLAY_GRADIENT_HEIGHT, QImage::Format_ARGB32_Premultiplied);
    m_topGradientImage.fill(0);
    QPainter painter(&m_topGradientImage);
    painter.fillRect(QRect(0, 0, size().width(), OVERLAY_GRADIENT_HEIGHT), gradient);
    m_bottomGradientImage = m_topGradientImage.mirrored();
}
