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

#include "gui/historydock.h"

#include "gui/historywidget.h"

#include <QtCore/QEvent>
#include <QtGui/QIcon>

HistoryDock::HistoryDock(QWidget *parent)
    : QDockWidget(parent),
    m_widget(new HistoryWidget(this))
{
    setWidget(m_widget);
    retranslateText();
}

void HistoryDock::retranslateText()
{
    setWindowTitle(tr("History"));
}

void HistoryDock::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange)
        retranslateText();
    else
        QDockWidget::changeEvent(e);
}

