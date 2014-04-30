// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2008-2009 Helder Correia <helder.pereira.correia@gmail.com>
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

#ifndef GUI_HISTORYDOCK_H
#define GUI_HISTORYDOCK_H

#include <QDockWidget>

class HistoryWidget;
class QListWidgetItem;

class HistoryDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit HistoryDock(QWidget *parent = 0);

public slots:
    void retranslateText();

protected:
    void changeEvent(QEvent *);

private:
    Q_DISABLE_COPY(HistoryDock)

    HistoryWidget *m_widget;
};

#endif

