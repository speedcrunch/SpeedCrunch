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
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>
#include <QTreeWidget>
#include <QVBoxLayout>

class FunctionsDockPrivate
{
  public:
    QStringList functionNames;
    QStringList functionDesc;
    QTreeWidget* list;
    QLineEdit* filter;
    QTimer* filterTimer;
};

FunctionsDock::FunctionsDock( QWidget* parent ): QDockWidget( tr("Functions"), parent )
{
  d = new FunctionsDockPrivate;

  QLabel* label = new QLabel( this );
  label->setText( tr("Search") );

  d->filter = new QLineEdit( this );
  connect( d->filter, SIGNAL( textChanged( const QString& ) ), SLOT( triggerFilter() ) );

  QWidget* searchBox = new QWidget( this );
  QHBoxLayout* searchLayout = new QHBoxLayout;
  searchBox->setLayout( searchLayout );
  searchLayout->addWidget( label );
  searchLayout->addWidget( d->filter );
  searchLayout->setMargin( 0 );

  d->list = new QTreeWidget( this );
  d->list->setColumnCount( 2 );
  d->list->setRootIsDecorated( false );
  d->list->header()->hide();
  d->list->setEditTriggers( QTreeWidget::NoEditTriggers );
  d->list->setSelectionBehavior( QTreeWidget::SelectRows );
  connect( d->list, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ),
    SLOT( handleItem( QTreeWidgetItem* ) ) );

  QWidget* widget = new QWidget( this );
  QVBoxLayout* layout = new QVBoxLayout;
  widget->setLayout( layout );
  setWidget( widget );
  layout->addWidget( searchBox );
  layout->addWidget( d->list );

  d->filterTimer = new QTimer( this );
  d->filterTimer->setInterval( 500 );
  d->filterTimer->setSingleShot( true );
  connect( d->filterTimer, SIGNAL( timeout() ), SLOT( filter() ) );

  setMinimumWidth( 200 );
  setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

  QStringList functionNames = FunctionRepository::self()->functionNames();
  for( int i = 0; i < functionNames.count(); i++ )
  {
    Function* f = FunctionRepository::self()->function( functionNames[i] );
    if( f )
    {
      d->functionNames << f->name();
      d->functionDesc << f->description();
    }
  }

  filter();
}

FunctionsDock::~FunctionsDock()
{
  d->filterTimer->stop();
  delete d;
}

void FunctionsDock::triggerFilter()
{
  d->filterTimer->stop();
  d->filterTimer->start();
}

void FunctionsDock::filter()
{
  QString term = d->filter->text();

  d->list->setUpdatesEnabled(false);

  d->list->clear();
  for( int k = 0; k < d->functionNames.count(); k++ )
  {
      QStringList str;
      str << d->functionNames[k];
      str << d->functionDesc[k];
      if( term.isEmpty() )
        new QTreeWidgetItem( d->list, str );
      else
      {
        if( str[0].contains(term, Qt::CaseInsensitive) ||
            str[1].contains(term, Qt::CaseInsensitive) )
          new QTreeWidgetItem( d->list, str );
      }
  }

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

void FunctionsDock::handleItem( QTreeWidgetItem* item )
{
  d->list->clearSelection();
  emit functionSelected( item->text(0) );
}

