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

#ifndef GUI_SYNTAXHIGHLIGHTER_H
#define GUI_SYNTAXHIGHLIGHTER_H

#include <QtGui/QSyntaxHighlighter>

class QPlainTextEdit;

class SyntaxHighlighter : public QSyntaxHighlighter {
public:
    enum ColorScheme {
        Standard = 0,
        Sublime = 1,
        Terminal = 2
    };

    enum Role {
        DisplayBackground,
        DisplayScrollBar,
        EditorCursor,
        EditorFadeAway,
        Number,
        Function,
        Operator,
        Variable,
        Separator,
        Parens,
        MatchedParens,
        Result
    };

    explicit SyntaxHighlighter(QPlainTextEdit*);

    QColor colorForRole(Role role) const { return m_colorScheme[role]; }
    void setColorForRole(Role role, const QColor& color) { m_colorScheme[role] = color; }

    void update();

    virtual void highlightBlock(const QString&);

private:
    Q_DISABLE_COPY(SyntaxHighlighter);
    SyntaxHighlighter();
    SyntaxHighlighter(QObject*);
    SyntaxHighlighter(QTextDocument*);
    void groupDigits(const QString& text, int pos, int length);

    QHash<Role, QColor> m_colorScheme;
};

#endif