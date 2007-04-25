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
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
 */

#include "constantsdock.h"

#include "evaluator.h"
#include "functions.h"
#include "settings.h"

#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>
#include <QTreeWidget>
#include <QVBoxLayout>


class Constant
{
public:
  QString name;
  QString value;
  QStringList categories;
};

class ConstantsDockPrivate
{
  public:
    QList<Constant> constants;
    QTreeWidget* list;
    QLineEdit* filter;
    QTimer* filterTimer;
    QLabel* noMatchLabel;
};

ConstantsDock::ConstantsDock( QWidget* parent ): QDockWidget( tr("Constants"), parent )
{
  d = new ConstantsDockPrivate;

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
  layout->setMargin( 3 );
  layout->addWidget( searchBox );
  layout->addWidget( d->list );

  d->filterTimer = new QTimer( this );
  d->filterTimer->setInterval( 500 );
  d->filterTimer->setSingleShot( true );
  connect( d->filterTimer, SIGNAL( timeout() ), SLOT( filter() ) );

  d->noMatchLabel = new QLabel( this );
  d->noMatchLabel->setText( tr("No match found") );
  d->noMatchLabel->setAlignment( Qt::AlignCenter );
  d->noMatchLabel->adjustSize();
  d->noMatchLabel->hide();

  setMinimumWidth( 200 );
  setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  setWindowIcon( QIcon() ); // no icon
}

ConstantsDock::~ConstantsDock()
{
  d->filterTimer->stop();
  delete d;
}

void ConstantsDock::triggerFilter()
{
  d->filterTimer->stop();
  d->filterTimer->start();
}

void ConstantsDock::filter()
{
  QString term = d->filter->text();

  d->filterTimer->stop();
  setUpdatesEnabled(false);

  d->list->clear();
  for( int k = 0; k < d->constants.count(); k++ )
  {
      QStringList str;
      str << d->constants[k].name;

      QTreeWidgetItem* item = 0;
      if( term.isEmpty() )
        item = new QTreeWidgetItem( d->list, str );
      else
      {
        if( str[0].contains(term, Qt::CaseInsensitive) )
          item = new QTreeWidgetItem( d->list, str );
      }
  }

  if( d->list->topLevelItemCount() > 0 )
  {
    d->noMatchLabel->hide();
    d->list->sortItems( 0, Qt::AscendingOrder );

    int group = 3;
    if( d->list->topLevelItemCount() >= 2*group )
      for(int i = 0; i < d->list->topLevelItemCount(); i++)
      {
        QTreeWidgetItem* item = d->list->topLevelItem(i);
        QBrush c = ((int)(i/group))&1 ? palette().base() : palette().alternateBase();
        item->setBackground( 0, c );
        item->setBackground( 1, c );
      }
  }
  else
  {
    d->noMatchLabel->setGeometry( d->list->geometry() );
    d->noMatchLabel->show();
    d->noMatchLabel->raise();
  }


  setUpdatesEnabled(true);
}

void ConstantsDock::handleItem( QTreeWidgetItem* item )
{
  d->list->clearSelection();
  emit constantSelected( item->text(0) );
}

