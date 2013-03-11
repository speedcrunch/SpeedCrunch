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
        return;
    }

    Tokens tokens = Evaluator::instance()->scan(text);

    for (int i = 0; i < tokens.count(); ++i) {
        const Token& token = tokens.at(i);
        const QString text = token.text().toLower();
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
                if (functionNames.at(i).toLower() == text)
                    color = colorForRole(Function);
            break;

        default:
            break;
        };

        if (token.pos() >= 0)
            setFormat(token.pos(), token.text().length(), color);
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