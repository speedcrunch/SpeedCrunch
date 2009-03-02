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
#include "gui/variabletable.hxx"

#include <QEvent>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>
#include <QTreeWidget>
#include <QVBoxLayout>

VariablesDock::VariablesDock( Evaluator * eval, QWidget * parent )
  : QDockWidget( parent ),
    m_variableTable( new VariableTable( eval, true, this ) )
{
  connect( m_variableTable, SIGNAL( itemActivated( QTreeWidgetItem *, int ) ),
           this, SLOT( handleItem( QTreeWidgetItem * ) ) );

  setWidget( m_variableTable );

  setMinimumWidth( 200 );
  setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  setWindowIcon( QIcon() );

  retranslateText();
}

void VariablesDock::updateList()
{
  m_variableTable->filter();
}

VariablesDock::~VariablesDock()
{
}

void VariablesDock::handleRadixCharacterChange()
{
  m_variableTable->filter();
}

void VariablesDock::retranslateText()
{
  setWindowTitle( tr( "Variables" ) );
  m_variableTable->setLayoutDirection( Qt::LeftToRight );
  m_variableTable->retranslateText();
}

void VariablesDock::handleItem( QTreeWidgetItem * item )
{
  emit variableSelected( item->text( 0 ) );
}

void VariablesDock::changeEvent( QEvent * e )
{
  if ( e->type() == QEvent::LayoutDirectionChange )
    retranslateText();
  else
    QDockWidget::changeEvent( e );
}

