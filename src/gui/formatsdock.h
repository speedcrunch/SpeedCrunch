// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2007, 2008, 2009, 2010 Helder Correia <helder.pereira.correia@gmail.com>
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

#ifndef GUI_FORMATSDOCK_H
#define GUI_FORMATSDOCK_H

#include <QDockWidget>

class FormatsWidget;
class HNumber;

class FormatsDock : public QDockWidget
{
    Q_OBJECT

public:
    FormatsDock(QWidget *parent = 0);

public slots:
    void updateNumber(const HNumber&);
    void setNumberFont(const QFont&);

protected:
    virtual void changeEvent(QEvent *);
    void retranslateText();

private:
    Q_DISABLE_COPY(FormatsDock)

    FormatsWidget *m_widget;
};

#endif // GUI_FORMATSDOCK_H

