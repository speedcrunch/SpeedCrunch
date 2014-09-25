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

class QGridLayout;
class QLabel;
class FormatLabel;
class QSize;
class QFont;

class HNumber;

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

    QLabel      *m_lblDec;
    FormatLabel *m_fmtDec;

    QLabel      *m_lblHex;
    FormatLabel *m_fmtHex;

    QLabel      *m_lblOct;
    FormatLabel *m_fmtOct;

    QLabel      *m_lblBin;
    FormatLabel *m_fmtBin;

    QLabel      *m_lblEng;
    FormatLabel *m_fmtEng;

    QLabel      *m_lblSci;
    FormatLabel *m_fmtSci;

    void createLblFmt(QLabel*&, FormatLabel*&, char format);
};

#endif // GUI_FORMATSWIDGET_H

