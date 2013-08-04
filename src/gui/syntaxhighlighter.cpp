// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2007-2009, 2013 Helder Correia <helder.pereira.correia@gmail.com>
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
//

#include "gui/syntaxhighlighter.h"

#include "core/evaluator.h"
#include "core/functions.h"
#include "core/settings.h"
#include "gui/textedit.h"

#include <QtCore/QLatin1String>
#include <QtGui/QApplication>
#include <QtGui/QPalette>

static inline QHash<SyntaxHighlighter::Role, QColor> createColorScheme(SyntaxHighlighter::ColorScheme id)
{
    QHash<SyntaxHighlighter::Role, QColor> result;
    switch (id) {
    case SyntaxHighlighter::Sublime:
         result[SyntaxHighlighter::Background] = QColor(39, 40, 34);
         result[SyntaxHighlighter::Number] = QColor(173, 119, 158);
         result[SyntaxHighlighter::Function] = QColor(213, 38, 106);
         result[SyntaxHighlighter::Operator] = QColor(242, 248, 214);
         result[SyntaxHighlighter::Variable] = QColor(64, 181, 238);
         result[SyntaxHighlighter::Separator] = QColor(197, 218, 107);
         result[SyntaxHighlighter::Parens] = QColor(103, 112, 88);
         result[SyntaxHighlighter::MatchedParens] = QColor(163, 126, 219);
         result[SyntaxHighlighter::Result] = QColor(197, 218, 107);
         break;
    case SyntaxHighlighter::Terminal:
         result[SyntaxHighlighter::Background] = QColor(48, 10, 36);
         result[SyntaxHighlighter::Number] = QColor(255, 255, 255);
         result[SyntaxHighlighter::Function] = QColor(239, 41, 40);
         result[SyntaxHighlighter::Operator] = QColor(196, 160, 0);
         result[SyntaxHighlighter::Variable] = QColor(74, 154, 7);
         result[SyntaxHighlighter::Separator] = QColor(100, 80, 123);
         result[SyntaxHighlighter::Parens] = QColor(173, 127, 168);
         result[SyntaxHighlighter::MatchedParens] = QColor(100, 80, 123);
         result[SyntaxHighlighter::Result] = QColor(104, 159, 207);
         break;
    case SyntaxHighlighter::Standard:
    default:
        result[SyntaxHighlighter::Background] = QColor(255, 255, 255);
        result[SyntaxHighlighter::Number] = QColor(98, 50, 76);
        result[SyntaxHighlighter::Function] = QColor(74, 154, 7);
        result[SyntaxHighlighter::Operator] = QColor(193, 147, 188);
        result[SyntaxHighlighter::Variable] = QColor(239, 41, 40);
        result[SyntaxHighlighter::Separator] = QColor(100, 80, 123);
        result[SyntaxHighlighter::Parens] = QColor(196, 160, 50);
        result[SyntaxHighlighter::MatchedParens] = QColor(100, 80, 123);
        result[SyntaxHighlighter::Result] = QColor(104, 159, 207);
        break;
    }
    return result;
}

SyntaxHighlighter::SyntaxHighlighter(TextEdit* edit)
    : QSyntaxHighlighter(edit)
{
    setDocument(edit->document());
    update();
}

void SyntaxHighlighter::highlightBlock(const QString& text)
{
    if (!Settings::instance()->syntaxHighlighting) {
        setFormat(0, text.length(), colorForRole(Number));
        return;
    }

    if (text.startsWith(QLatin1String("="))) {
        setFormat(0, 1, colorForRole(Operator));
        setFormat(1, text.length(), colorForRole(Result));
        groupDigits(text, 1, text.length() - 1);
        return;
    }

    Tokens tokens = Evaluator::instance()->scan(text);

    for (int i = 0; i < tokens.count(); ++i) {
        const Token& token = tokens.at(i);
        const QString tokenText = token.text().toLower();
        QStringList functionNames = FunctionRepo::instance()->getIdentifiers();
        QColor color;

        switch (token.type()) {
        case Token::stxNumber:
        case Token::stxUnknown:
            color = colorForRole(Number);
            break;

        case Token::stxOperator:
            color = colorForRole(Operator);
            break;

        case Token::stxSep:
            color = colorForRole(Separator);
            break;

        case Token::stxOpenPar:
        case Token::stxClosePar:
            color = colorForRole(Parens);
            break;

        case Token::stxIdentifier:
            color = colorForRole(Variable);
            for (int i = 0; i < functionNames.count(); ++i)
                if (functionNames.at(i).toLower() == tokenText)
                    color = colorForRole(Function);
            break;

        default:
            break;
        };

        if (token.pos() >= 0) {
            setFormat(token.pos(), token.text().length(), color);
            if (token.type() == Token::stxNumber)
                groupDigits(text, token.pos(), token.text().length());
        }
    }
}

void SyntaxHighlighter::update()
{
    ColorScheme id = static_cast<ColorScheme>(Settings::instance()->colorScheme);
    m_colorScheme = createColorScheme(id);

    QColor backgroundColor = colorForRole(Background);
    QWidget* parentWidget = static_cast<QWidget*>(parent());
    QPalette pal = parentWidget->palette();
    pal.setColor(QPalette::Active, QPalette::Base, backgroundColor);
    pal.setColor(QPalette::Inactive, QPalette::Base, backgroundColor);
    parentWidget->setPalette(pal);

    rehighlight();
}

void SyntaxHighlighter::groupDigits(const QString& text, int pos, int length)
{
    // Used to find out which characters belong to which radixes.
    static int charType[128] = { 0 };
    static const int BIN_CHAR = (1 << 0);
    static const int OCT_CHAR = (1 << 1);
    static const int DEC_CHAR = (1 << 2);
    static const int HEX_CHAR = (1 << 3);

    if (charType['0'] == 0) { // Initialize the table on first call (not thread-safe!).
        charType['0'] = HEX_CHAR | DEC_CHAR | OCT_CHAR | BIN_CHAR;
        charType['1'] = HEX_CHAR | DEC_CHAR | OCT_CHAR | BIN_CHAR;
        charType['2'] = HEX_CHAR | DEC_CHAR | OCT_CHAR;
        charType['3'] = HEX_CHAR | DEC_CHAR | OCT_CHAR;
        charType['4'] = HEX_CHAR | DEC_CHAR | OCT_CHAR;
        charType['5'] = HEX_CHAR | DEC_CHAR | OCT_CHAR;
        charType['6'] = HEX_CHAR | DEC_CHAR | OCT_CHAR;
        charType['7'] = HEX_CHAR | DEC_CHAR | OCT_CHAR;
        charType['8'] = HEX_CHAR | DEC_CHAR;
        charType['9'] = HEX_CHAR | DEC_CHAR;
        charType['a'] = HEX_CHAR;
        charType['b'] = HEX_CHAR;
        charType['c'] = HEX_CHAR;
        charType['d'] = HEX_CHAR;
        charType['e'] = HEX_CHAR;
        charType['f'] = HEX_CHAR;
        charType['A'] = HEX_CHAR;
        charType['B'] = HEX_CHAR;
        charType['C'] = HEX_CHAR;
        charType['D'] = HEX_CHAR;
        charType['E'] = HEX_CHAR;
        charType['F'] = HEX_CHAR;
    }

    qreal groupSpacing = 140; // Size of the space between groups (100 means no space).
    int s = -1; // Index of the first digit (most significant).
    bool invertGroup = false; // If true, group digits from the most significant digit.
    int groupSize = 3; // Number of digits to group (depends on the radix).
    int allowedChars = DEC_CHAR; // Allowed characters for the radix of the current number being parsed.

    int endPos = pos + length;
    if (endPos > text.length())
        endPos = text.length();
    for (int i = pos; i < endPos; ++i) {
        ushort c = text[i].unicode();
        bool isNumber = c < 256 && (charType[c] & allowedChars);

        if (s >= 0) {
            if (!isNumber) {
                // End of current number found, start grouping the digits.
                if (invertGroup)
                    for (s += groupSize - 1; s < i ; s += groupSize) {
                        // Only change the letter spacing from the format and keep the other properties.
                        QTextCharFormat fmt = format(s);
                        fmt.setFontLetterSpacing(groupSpacing);
                        setFormat(s, 1, fmt);
                    }
                else
                    for (int e = i - (groupSize + 1); e >= s ; e -= groupSize) {
                        QTextCharFormat fmt = format(e);
                        fmt.setFontLetterSpacing(groupSpacing);
                        setFormat(e, 1, fmt);
                    }
                s = -1; // Reset.
            }
        } else {
            if (isNumber) // Start of number found.
                s = i;
        }

        if (!isNumber) {
            if (c == '.') {
                // Invert the grouping for the fractional part.
                invertGroup = true;
            } else {
                // Look for a radix prefix.
                invertGroup = false;
                if (i > 0 && text[i - 1] == '0') {
                    if (c == 'x') {
                        groupSize = 4;
                        allowedChars = HEX_CHAR;
                    } else if (c == 'o') {
                        groupSize = 3;
                        allowedChars = OCT_CHAR;
                    } else if (c == 'b') {
                        groupSize = 4;
                        allowedChars = BIN_CHAR;
                    } else {
                        groupSize = 3;
                        allowedChars = DEC_CHAR;
                    }
                } else {
                    groupSize = 3;
                    allowedChars = DEC_CHAR;
                }
            }
        }
    }

    // Group the last digits if the string finishes with the number.
    if (s >= 0) {
        if (invertGroup)
            for (s += groupSize - 1; s < endPos ; s += groupSize) {
                QTextCharFormat fmt = format(s);
                fmt.setFontLetterSpacing(groupSpacing);
                setFormat(s, 1, fmt);
            }
        else
            for (int e = endPos - (groupSize + 1); e >= s ; e -= groupSize) {
                QTextCharFormat fmt = format(e);
                fmt.setFontLetterSpacing(groupSpacing);
                setFormat(e, 1, fmt);
            }
    }
}
