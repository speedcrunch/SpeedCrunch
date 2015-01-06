// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2007-2009, 2013, 2014 Helder Correia <helder.pereira.correia@gmail.com>
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

#include <QLatin1String>
#include <QApplication>
#include <QPalette>
#include <QPlainTextEdit>

#define SH(x,r,g,b) (setColorForRole(SyntaxHighlighter::x, QColor(r,g,b)))

void SyntaxHighlighter::setColorScheme(SyntaxHighlighter::ColorScheme id)
{
    switch (id) {
    case SyntaxHighlighter::Sublime:
        SH(Cursor, 100, 200, 255);  SH(Function, 213, 38, 106);   SH(EditorBackground, 29, 30, 24);
        SH(Number, 173, 119, 158);  SH(Operator, 242, 248, 214);
        SH(Parens, 103, 112, 88);   SH(Variable, 64, 181, 238);
        SH(Result, 197, 218, 107);  SH(ScrollBar, 110, 120, 100);
        SH(Comment, 55, 55, 50);    SH(Separator, 197, 218, 107);
        SH(Matched, 163, 126, 219); SH(Background, 39, 40, 34);
        break;
    case SyntaxHighlighter::Terminal:
        SH(Cursor, 140, 100, 140);  SH(Function, 239, 41, 40);    SH(EditorBackground, 38, 0, 26);
        SH(Number, 255, 255, 255);  SH(Operator, 196, 160, 0);
        SH(Parens, 173, 127, 168);  SH(Variable, 74, 154, 7);
        SH(Result, 104, 159, 207);  SH(ScrollBar, 140, 100, 140);
        SH(Comment, 65, 25, 55);    SH(Separator, 100, 80, 123);
        SH(Matched, 100, 80, 123);  SH(Background, 48, 10, 36);
        break;
    case SyntaxHighlighter::SolarizedDark:
        SH(Cursor, 220, 50, 47);    SH(Function, 38, 139, 210);   SH(EditorBackground, 0, 43, 54);
        SH(Number, 131, 148, 150);  SH(Operator, 181, 137, 0);
        SH(Parens, 253, 246, 227);  SH(Variable, 133, 153, 0);
        SH(Result, 253, 246, 227);  SH(ScrollBar, 211, 54, 130);
        SH(Comment, 88, 110, 117);  SH(Separator, 181, 137, 0);
        SH(Matched, 108, 113, 196); SH(Background, 7, 54, 66);
        break;
    case SyntaxHighlighter::SolarizedLight:
        SH(Cursor, 220, 50, 47);    SH(Function, 38, 139, 210);   SH(EditorBackground, 238, 232, 213);
        SH(Number, 101, 123, 131);  SH(Operator, 181, 137, 0);
        SH(Parens, 0, 43, 54);      SH(Variable, 133, 153, 0);
        SH(Result, 7, 54, 66);      SH(ScrollBar, 203, 75, 22);
        SH(Comment, 88, 110, 117);  SH(Separator, 181, 137, 0);
        SH(Matched, 108, 113, 196); SH(Background, 253, 246, 227);
        break;
    case SyntaxHighlighter::Standard:
    default:
        SH(Cursor, 255, 100, 100);  SH(Function, 74, 154, 7);     SH(EditorBackground, 220, 220, 220);
        SH(Number, 40, 40, 40);     SH(Operator, 150, 150, 150);
        SH(Parens, 255, 160, 50);   SH(Variable, 239, 41, 40);
        SH(Result, 0, 100, 200);    SH(ScrollBar, 255, 120, 50);
        SH(Comment, 210, 210, 210); SH(Separator, 100, 80, 123);
        SH(Matched, 100, 80, 123);  SH(Background, 255, 255, 255);
        break;
    }
}

SyntaxHighlighter::SyntaxHighlighter(QPlainTextEdit* edit)
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

    // NOTE: 4 NBSP chars are used to visually align more closely with the case when we use the 
    // equals sign, ie: this '4' may change.
    // TODO/NOTE: In the case of no equals sign, this would still run colorForRole on the first NBSP char.
    // TODO: Make this configurable and consider abstracting away the management of color positions.
    if (text.startsWith(QLatin1String("=")) || text.left(4).simplified().isEmpty() ) {
        setFormat(0, 1, colorForRole(Operator));
        setFormat(1, text.length(), colorForRole(Result));
        if (Settings::instance()->digitGrouping > 0)
            groupDigits(text, 1, text.length() - 1);
        return;
    }

    int questionMarkIndex = text.indexOf('?');
    if (questionMarkIndex != -1)
        setFormat(questionMarkIndex, text.length(), colorForRole(Comment));

    Tokens tokens = Evaluator::instance()->scan(text, Evaluator::NoAutoFix);

    for (int i = 0; i < tokens.count(); ++i) {
        const Token& token = tokens.at(i);
        const QString tokenText = token.text().toLower();
        QStringList functionNames = FunctionRepo::instance()->getIdentifiers();
        QColor color;

        switch (token.type()) {
        case Token::stxNumber:
        case Token::stxUnknown:
            color = colorForRole(Number);
            // TODO: color thousand separators differently? It might help troubleshooting issues
            //       in non-strict mode where more than 95k characters are valid separators.
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
            if (Evaluator::instance()->hasUserFunction(token.text())
                || functionNames.contains(tokenText, Qt::CaseInsensitive))
                color = colorForRole(Function);
            break;

        default:
            break;
        };

        Q_ASSERT(token.pos() >= 0); // Why would pos() ever return a negative value?

        // The token text might be a modified version of the displayed text,
        // so we must not rely on it to find out the number of character to highlight.
        int end = (questionMarkIndex != -1) ? questionMarkIndex : text.length();
        if (i + 1 < tokens.size())
        {
            const Token& nextToken = tokens.at(i + 1);

            Q_ASSERT(nextToken.pos() >= 0);

            end = nextToken.pos();
        }

        setFormat(token.pos(), end - token.pos(), color);
        if (token.type() == Token::stxNumber && Settings::instance()->digitGrouping > 0)
            groupDigits(text, token.pos(), end - token.pos());
    }
}

void SyntaxHighlighter::update()
{
    ColorScheme id = static_cast<ColorScheme>(Settings::instance()->colorScheme);
    setColorScheme(id);

    QColor backgroundColor = colorForRole(Background);
    QWidget* parentWidget = static_cast<QWidget*>(parent());
    QPalette pal = parentWidget->palette();
    pal.setColor(QPalette::Active, QPalette::Base, backgroundColor);
    pal.setColor(QPalette::Inactive, QPalette::Base, backgroundColor);
    parentWidget->setPalette(pal);

    rehighlight();
}

void SyntaxHighlighter::formatDigitsGroup(const QString& text, int start, int end, bool invert, int size)
{
    Q_ASSERT(start <= end);
    Q_ASSERT(size > 0);

    qreal spacing = 100; // Size of the space between groups (100 means no space).
    spacing += 40 * Settings::instance()->digitGrouping;
    Evaluator *evaluator = Evaluator::instance();
    int inc = !invert ? -1 : 1;
    if(!invert)
    {
        int tmp = start;
        start = end - 1;
        end = tmp - 1;

        // Skip the first digit so that we add the spacing to the first digit of the next group.
        while (start != end && evaluator->isSeparatorChar(text[start].unicode()))
            --start;
        if (start == end)
            return; // Bug ?
        --start;
    }

    for (int count = 0 ; start != end ; start += inc)
    {
        // When there are separators in the number, we must not count them as part of the group.
        if (!evaluator->isSeparatorChar(text[start].unicode()))
        {
            ++count;
            if (count == size)
            {
                // Only change the letter spacing from the format and keep the other properties.
                QTextCharFormat fmt = format(start);
                fmt.setFontLetterSpacing(spacing);
                setFormat(start, 1, fmt);
                count = 0; // Reset
                // TODO: if the next character is a separator, do not add spacing?
            }
        }
    }
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

    int s = -1; // Index of the first digit (most significant).
    bool invertGroup = false; // If true, group digits from the most significant digit.
    int groupSize = 3; // Number of digits to group (depends on the radix).
    int allowedChars = DEC_CHAR; // Allowed characters for the radix of the current number being parsed.
    Evaluator *evaluator = Evaluator::instance();

    int endPos = pos + length;
    if (endPos > text.length())
        endPos = text.length();
    for (int i = pos; i < endPos; ++i) {
        ushort c = text[i].unicode();
        bool isNumber = c < 128 && (charType[c] & allowedChars);

        if (s >= 0) {
            if (!isNumber && !evaluator->isSeparatorChar(c)) {
                // End of current number found, start grouping the digits.
                formatDigitsGroup(text, s, i, invertGroup, groupSize);
                s = -1; // Reset.
            }
        } else {
            if (isNumber) // Start of number found.
                s = i;
        }

        if (!isNumber) {
            if (evaluator->isRadixChar(c)) {
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
                } else if (!evaluator->isSeparatorChar(c)){
                    groupSize = 3;
                    allowedChars = DEC_CHAR;
                }
            }
        }
    }

    // Group the last digits if the string finishes with the number.
    if (s >= 0) {
        formatDigitsGroup(text, s, endPos, invertGroup, groupSize);
    }
}
