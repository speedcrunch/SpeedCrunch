// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2008, 2009, 2010, 2011 Helder Correia <helder.pereira.correia@gmail.com>
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

#ifndef GUI_FORMATSWIDGET_H
#define GUI_FORMATSWIDGET_H

#include <QWidget>
#include <QList>
#include <QPlainTextEdit>
#include <QAction>

#include "math/hmath.h"

class QPlainTextEdit;
class QGridLayout;
class QAction;
class FormatLabel;
class QSize;
class QFont;

class HNumber;
class SyntaxHighlighter;

//------------------------------------------------------------------------------

class FormatLabel : public QPlainTextEdit
{
    Q_OBJECT

public:

    FormatLabel(QWidget *parent, char& format);
    virtual QSize sizeHint() const;

public slots:
    void updateNumber(const HNumber& number);
    void rehighlight();
    void handleRadixCharacterChange();
    void setAltFormat(bool altFormat);

protected:
    virtual void changeEvent(QEvent *e);
    virtual void focusOutEvent(QFocusEvent* e);

private:
    SyntaxHighlighter *m_highlighter;
    char& format;
    HNumber number;
};

//------------------------------------------------------------------------------


class FormatsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FormatsWidget(QWidget* parent = 0);

signals:
    void constantSelected(const QString&);

public slots:
    void updateNumber(const HNumber&);
    void increaseFontPointSize();
    void decreaseFontPointSize();
    void setNumberFont(const QFont&);

    void rehighlight();
    void handleRadixCharacterChange();

protected slots:
    void retranslateText();

protected:
    virtual void changeEvent(QEvent *);

private:
    Q_DISABLE_COPY(FormatsWidget)

    QGridLayout *layout;
    QList<FormatLabel*> m_fmtLbls;

    QAction     *m_actDec;
    FormatLabel *m_fmtDec;

    QAction     *m_actHex;
    FormatLabel *m_fmtHex;

    QAction     *m_actOct;
    FormatLabel *m_fmtOct;

    QAction     *m_actBin;
    FormatLabel *m_fmtBin;

    QAction     *m_actExp;
    FormatLabel *m_fmtExp;

    void createLblFmt(QAction*&, FormatLabel*&, char&);
};

#endif // GUI_FORMATSWIDGET_H

