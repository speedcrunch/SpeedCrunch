// This file is part of the SpeedCrunch project
// Copyright (C) 2009, 2011 Helder Correia <helder.pereira.correia@gmail.com>
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
#include "gui/textedit.h"
#include "math/hmath.h"
#include "math/floatconfig.h"

#include <QtCore/QLatin1String>
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QScrollBar>

static QString formatNumber(const HNumber &);

ResultDisplay::ResultDisplay(QWidget *parent)
    : TextEdit(parent)
    , m_count(0)
    , m_customAppearance(false)
    , m_customBackgroundColor(QColor(0, 0, 0))
    , m_customBackgroundAlternateColor(QColor(21,  21,  21))
    , m_customTextColor(QColor(255, 255, 255))
    , m_highlighter(new SyntaxHighlighter(this))
{
    m_highlighter->rehighlight();
    setBackgroundRole(QPalette::Base);
    setLayoutDirection(Qt::LeftToRight);
    setMinimumWidth(150);
    setReadOnly(true);
    setTabChangesFocus(true);
    setWordWrapMode(QTextOption::WrapAnywhere);

    QFont font;
    font.setBold(true);
    setFont(font);
}

void ResultDisplay::append(const QString &expr, const HNumber &value)
{
    if (value.isNan())
        return;

    ++m_count;

    appendPlainText(expr);
    appendPlainText(QLatin1String("= ") + formatNumber(value));
    appendPlainText(QLatin1String(""));

    ensureCursorVisible();

    // REFACTOR: this only serves to save a session, nonsense.
    m_expressions.append(expr);
    const char format = value.format() != 0 ? value.format() : 'e';
    char *str = HMath::format(value, format, DECPRECISION);
    m_results.append(str);
    free(str);
}

void ResultDisplay::appendHistory(const QStringList &expressions, const QStringList &results)
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

bool ResultDisplay::customAppearance() const
{
    return m_customAppearance;
}

QColor ResultDisplay::customBackgroundColor() const
{
    return m_customBackgroundColor;
}

QColor ResultDisplay::customBackgroundAlternateColor() const
{
    return m_customBackgroundAlternateColor;
}

QColor ResultDisplay::customTextColor() const
{
    return m_customTextColor;
}

void ResultDisplay::setCustomAppearance(bool custom)
{
    m_customAppearance = custom;
    QColor bgcolor = QApplication::palette().base().color();
    if (custom)
        bgcolor = customBackgroundColor();

    QPalette pal = palette();
    pal.setColor(QPalette::Background, bgcolor);
    setPalette(pal);

    refresh();
}

void ResultDisplay::setCustomBackgroundColor(const QColor &base, const QColor &alternate)
{
    m_customBackgroundColor = base;
    m_customBackgroundAlternateColor = alternate;
    if (m_customAppearance) {
        QPalette pal = palette();
        pal.setColor(QPalette::Base, base);
        setPalette(pal);
    }
    refresh();
}

void ResultDisplay::setCustomTextColor(const QColor &c)
{
    m_customTextColor = c;
    refresh();
}

void ResultDisplay::highlight()
{
    m_highlighter->rehighlight();
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

static QString formatNumber(const HNumber &value)
{
    Settings *settings = Settings::instance();
    const char format = value.format() != 0 ? value.format() : settings->resultFormat;
    char * str = HMath::format(value, format, settings->resultPrecision);
    QString s = QString::fromLatin1(str);
    free(str);
    if (settings->radixCharacter() != '.')
        s.replace('.', settings->radixCharacter());
    return s;
}

