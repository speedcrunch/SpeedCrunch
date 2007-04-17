/* This file is part of the SpeedCrunch project
   Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "functionsdock.h"
#include "functions.h"
#include "settings.h"

#include <QHeaderView>
#include <QTableWidget>

class FunctionsDockPrivate
{
  public:
    QTableWidget* list;
};

FunctionsDock::FunctionsDock( QWidget* parent ): QDockWidget( tr("Functions"), parent )
{
  d = new FunctionsDockPrivate;

  d->list = new QTableWidget( this );
  d->list->setColumnCount( 3 );
  d->list->setAlternatingRowColors( true );
  d->list->verticalHeader()->hide();
  d->list->horizontalHeader()->hide();
  d->list->horizontalHeader()->setStretchLastSection( true );
  d->list->setShowGrid( false );
  d->list->setContentsMargins( 0, 0, 0, 0 );
  d->list->setEditTriggers( QTableWidget::NoEditTriggers );
  d->list->setSelectionBehavior( QTableWidget::SelectRows );
  //d->list->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  connect( d->list, SIGNAL( itemDoubleClicked( QTableWidgetItem* ) ),
    SLOT( handleItem( QTableWidgetItem* ) ) );
  setWidget( d->list );

  setMinimumWidth( 200 );
  setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

  QStringList functionNames = FunctionRepository::self()->functionNames();
  d->list->setRowCount( functionNames.count() );
  int k = 0;
  for( int i = 0; i < functionNames.count(); i++ )
  {
    Function* f = FunctionRepository::self()->function( functionNames[i] );
    if( f )
    {
      d->list->setItem( k, 0, new QTableWidgetItem( f->name() ) );
      d->list->setItem( k, 1, new QTableWidgetItem( f->description() ) );
      k++;
    }
  }
  d->list->setRowCount( k );
  d->list->sortItems( 0 );
  d->list->resizeColumnsToContents();
  d->list->resizeRowsToContents();
}

FunctionsDock::~FunctionsDock()
{
  delete d;
}

void FunctionsDock::handleItem( QTableWidgetItem* item )
{
  emit functionSelected( item->text() );
}
