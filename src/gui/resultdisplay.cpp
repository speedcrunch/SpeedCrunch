// This file is part of the SpeedCrunch project
// Copyright (C) 2009, 2011, 2013 Helder Correia <helder.pereira.correia@gmail.com>
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
#include "core/settings.h"
#include "gui/syntaxhighlighter.h"
#include "math/hmath.h"
#include "math/floatconfig.h"

#include <QtCore/QLatin1String>
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QPainter>
#include <QtGui/QScrollBar>

#define OVERLAY_GRADIENT_HEIGHT 50

static QString formatNumber(const HNumber&);

class FadeOverlay : public QWidget {
public:
    explicit FadeOverlay(QWidget*);
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
    if (value.isNan())
        return;

    ++m_count;

    appendPlainText(expression);
    appendPlainText(QLatin1String("= ") + formatNumber(value));
    appendPlainText(QLatin1String(""));

    ensureCursorVisible();

    // TODO: Refactor, this only serves to save a session.
    m_expressions.append(expression);
    const char format = value.format() != 0 ? value.format() : 'e';
    char* str = HMath::format(value, format, DECPRECISION);
    m_results.append(str);
    free(str);
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

        if (!result.isNan())
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

void ResultDisplay::scrollLineUp()
{
    QScrollBar* bar = verticalScrollBar();
    bar->setValue(bar->value() - 1);
}

void ResultDisplay::scrollLineDown()
{
    QScrollBar* bar = verticalScrollBar();
    bar->setValue(bar->value() + 1);
}

void ResultDisplay::scrollPageUp()
{
    QScrollBar* bar = verticalScrollBar();
    bar->setValue(bar->value() - linesPerPage());
}

void ResultDisplay::scrollPageDown()
{
    QScrollBar* bar = verticalScrollBar();
    bar->setValue(bar->value() + linesPerPage());
}

void ResultDisplay::zoomIn()
{
    QFont newFont = font();
    const int newSize = newFont.pointSize() + 1;
    if (newSize > 64)
        return;
    newFont.setPointSize(newSize);
    setFont(newFont);
}

void ResultDisplay::zoomOut()
{
    QFont newFont = font();
    const int newSize = newFont.pointSize() - 1;
    if (newSize < 4)
        return;
    newFont.setPointSize(newSize);
    setFont(newFont);
}

void ResultDisplay::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() == Qt::ShiftModifier) {
        if (event->delta() > 0)
            zoomIn();
        else if (event->delta() < 0)
            zoomOut();
        event->accept();
        return;
    }

    QPlainTextEdit::wheelEvent(event);
}

void ResultDisplay::resizeEvent(QResizeEvent* event)
{
    QPlainTextEdit::resizeEvent(event);
    m_fadeOverlay->resize(event->size());
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
        "   background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 0.4 %2, stop: 0.6 %2, stop: 1 %1);"
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
    ).arg(m_highlighter->colorForRole(SyntaxHighlighter::DisplayBackground).name())
     .arg(m_highlighter->colorForRole(SyntaxHighlighter::DisplayScrollBar).name()));
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
    ResultDisplay* widget = qobject_cast<ResultDisplay*>(parent());
    QColor backgroundColor = widget->m_highlighter->colorForRole(SyntaxHighlighter::DisplayBackground);

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

static QString formatNumber(const HNumber& value)
{
    Settings* settings = Settings::instance();
    const char format = value.format() != 0 ? value.format() : settings->resultFormat;
    char* str = HMath::format(value, format, settings->resultPrecision);
    QString s = QString::fromLatin1(str);
    free(str);
    if (settings->radixCharacter() != '.')
        s.replace('.', settings->radixCharacter());
    return s;
}