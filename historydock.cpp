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

#include "historydock.h"

#include <QListWidget>

class HistoryDockPrivate
{
  public:
    QListWidget* list;
};

HistoryDock::HistoryDock( QWidget* parent ): QDockWidget( tr("History"), parent )
{
  d = new HistoryDockPrivate;

  d->list = new QListWidget( this );
  connect( d->list, SIGNAL( itemDoubleClicked( QListWidgetItem* ) ),
    SLOT( handleItem( QListWidgetItem* ) ) );
  setWidget( d->list );

  setMinimumWidth( 200 );
  setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
}

HistoryDock::~HistoryDock()
{
  delete d;
}

void HistoryDock::clear()
{
  d->list->clear();
}

void HistoryDock::append( const QString& h )
{
  d->list->addItem( h );
  recolor();
}

void HistoryDock::setHistory( const QStringList& h )
{
  d->list->clear();
  d->list->insertItems( 0, h );
  d->list->setCurrentRow( h.count()-1 );
  recolor();
  d->list->scrollToItem( d->list->item(h.count()), QListWidget::PositionAtTop );
}

void HistoryDock::handleItem( QListWidgetItem* item )
{
  emit expressionSelected( item->text() );
}

void HistoryDock::recolor()
{
  int group = 3;
  d->list->setUpdatesEnabled( false );
  for(int i = 0; i < d->list->count(); i++)
  {
    QListWidgetItem* item = d->list->item(i);
    QBrush c = ((int)(i/group))&1 ? palette().base() : palette().alternateBase();
    item->setBackground( c );
  }
  d->list->setUpdatesEnabled( true );
}
