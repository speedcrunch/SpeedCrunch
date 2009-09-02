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

#include "gui/variablesdock.hxx"
#include "gui/variableswidget.hxx"

#include <QtCore/QEvent>
#include <QtCore/QTimer>
#include <QtGui/QIcon>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>

struct VariablesDock::Private
{
    VariablesWidget * variablesWidget;
};

VariablesDock::VariablesDock( QWidget * parent )
    : QDockWidget( parent ), d( new VariablesDock::Private )
{
    d->variablesWidget = new VariablesWidget( VariablesWidget::ShowUser, this );

    connect( d->variablesWidget, SIGNAL(itemActivated(QTreeWidgetItem *, int) ),
             SLOT(handleItem(QTreeWidgetItem *)) );

    setWidget( d->variablesWidget );

    setMinimumWidth( 200 );
    setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    setWindowIcon( QIcon() );

    retranslateText();
}

VariablesDock::~VariablesDock()
{
}

void VariablesDock::updateList()
{
    d->variablesWidget->fillTable();
}

void VariablesDock::handleRadixCharacterChange()
{
    d->variablesWidget->fillTable();
}

void VariablesDock::retranslateText()
{
    setWindowTitle( tr("Variables") );
}

void VariablesDock::handleItem( QTreeWidgetItem * item )
{
    emit variableSelected( item->text(0) );
}

void VariablesDock::changeEvent( QEvent * e )
{
    if ( e->type() == QEvent::LanguageChange )
        retranslateText();
    else
        QDockWidget::changeEvent( e );
}

