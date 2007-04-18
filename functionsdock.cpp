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
#include <QTreeWidget>

class FunctionsDockPrivate
{
  public:
    QTreeWidget* list;
};

FunctionsDock::FunctionsDock( QWidget* parent ): QDockWidget( tr("Functions"), parent )
{
  d = new FunctionsDockPrivate;

  d->list = new QTreeWidget( this );
  d->list->setColumnCount( 2 );
  d->list->setRootIsDecorated( false );
  d->list->header()->hide();
  d->list->setEditTriggers( QTreeWidget::NoEditTriggers );
  d->list->setSelectionBehavior( QTreeWidget::SelectRows );
  connect( d->list, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ),
    SLOT( handleItem( QTreeWidgetItem* ) ) );
  setWidget( d->list );

  setMinimumWidth( 200 );
  setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

  QStringList functionNames = FunctionRepository::self()->functionNames();
  int k = 0;
  for( int i = 0; i < functionNames.count(); i++ )
  {
    Function* f = FunctionRepository::self()->function( functionNames[i] );
    if( f )
    {
      QStringList str;
      str << f->name();
      str << f->description();
      new QTreeWidgetItem( d->list, str );
      k++;
    }
  }

  d->list->setUpdatesEnabled(false);
  d->list->sortItems( 0, Qt::AscendingOrder );

  int group = 3;
  for(int i = 0; i < d->list->topLevelItemCount(); i++)
  {
    QTreeWidgetItem* item = d->list->topLevelItem(i);
    QBrush c = ((int)(i/group))&1 ? palette().base() : palette().alternateBase();
    item->setBackground( 0, c );
    item->setBackground( 1, c );
  }
  d->list->setUpdatesEnabled(true);
}

FunctionsDock::~FunctionsDock()
{
  delete d;
}

void FunctionsDock::handleItem( QTreeWidgetItem* item )
{
  emit functionSelected( item->text(0) );
}
