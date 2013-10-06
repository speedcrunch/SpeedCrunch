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

#ifndef GUI_RESULTDISPLAY_H
#define GUI_RESULTDISPLAY_H

#include <QtGui/QPlainTextEdit>

class HNumber;
class SyntaxHighlighter;

class ResultDisplay : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit ResultDisplay(QWidget* parent = 0);

    void append(const QString& expr, const HNumber& value);
    void appendHistory(const QStringList& expressions, const QStringList& results);
    int count() const;

signals:
    void textCopied(const QString&);
    void textSelected(const QString&);

public slots:
    void clear();
    void rehighlight();
    void refresh();

private:
    Q_DISABLE_COPY(ResultDisplay);

    int m_count;
    QStringList m_expressions;
    SyntaxHighlighter* m_highlighter;
    QStringList m_results;
};

#endif