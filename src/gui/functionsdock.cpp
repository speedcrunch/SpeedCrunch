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

#include "gui/functionsdock.hxx"
#include "gui/functionswidget.hxx"

#include "core/functions.hxx"

#include <QtCore/QEvent>
#include <QtCore/QTimer>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>

FunctionsDock::FunctionsDock( QWidget * parent )
  : QDockWidget( parent ),
    m_functionWidget( new FunctionsWidget(this) )
{
  connect( m_functionWidget, SIGNAL( itemActivated( QTreeWidgetItem *, int ) ),
          this, SLOT( handleItem( QTreeWidgetItem * ) ) );

  setWidget( m_functionWidget );

  setMinimumWidth( 200 );
  setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  setWindowIcon( QIcon() );

  retranslateText();
}

FunctionsDock::~FunctionsDock()
{
}

void FunctionsDock::updateList()
{
  m_functionWidget->fillTable();
}

void FunctionsDock::retranslateText()
{
  setWindowTitle( tr( "Functions" ) );
  m_functionWidget->setLayoutDirection( Qt::LeftToRight );
  m_functionWidget->retranslateText();
}

void FunctionsDock::handleItem( QTreeWidgetItem * item )
{
  emit functionSelected( item->text( 0 ) );
}

void FunctionsDock::changeEvent( QEvent * e )
{
  if ( e->type() == QEvent::LayoutDirectionChange )
    retranslateText();
  else
    QDockWidget::changeEvent( e );
}

