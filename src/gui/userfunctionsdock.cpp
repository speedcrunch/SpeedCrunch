// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2008, 2009, 2011 Helder Correia <helder.pereira.correia@gmail.com>
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

#include "gui/userfunctionsdock.h"

#include "gui/userfunctionlistwidget.h"

#include <QtCore/QEvent>
#include <QtCore/QTimer>
#include <QtGui/QIcon>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>

UserFunctionsDock::UserFunctionsDock(QWidget*  parent)
    : QDockWidget(parent)
    , m_userFunctionsWidget(new UserFunctionListWidget(this))
{
    connect(m_userFunctionsWidget, SIGNAL(itemActivated(const QString&)), SIGNAL(userFunctionSelected(const QString&)));

    setWidget(m_userFunctionsWidget);

    setMinimumWidth(200);
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    setWindowIcon(QIcon());

    retranslateText();
}

void UserFunctionsDock::updateList()
{
    m_userFunctionsWidget->fillTable();
}

void UserFunctionsDock::handleRadixCharacterChange()
{
    m_userFunctionsWidget->fillTable();
}

void UserFunctionsDock::retranslateText()
{
    setWindowTitle(tr("User Functions"));
}

void UserFunctionsDock::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange)
        retranslateText();
    else
        QDockWidget::changeEvent(e);
}