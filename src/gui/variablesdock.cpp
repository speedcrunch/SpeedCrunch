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
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>
#include <QVBoxLayout>

struct VariablesDock::Private
{
  VariableTable * list;

  QLineEdit *   filter;
  QTimer *      filterTimer;
  QLabel *      label;
};

VariablesDock::VariablesDock( Evaluator * eval, QWidget * parent )
  : QDockWidget( parent ), d( new VariablesDock::Private )
{
  d->list = new VariableTable( eval, true, this );
  connect( d->list, SIGNAL( itemActivated( QTreeWidgetItem *, int ) ),
           this, SLOT( handleItem( QTreeWidgetItem * ) ) );

  d->label = new QLabel( this );

  d->filter = new QLineEdit( this );
  connect( d->filter, SIGNAL( textChanged( const QString & ) ),
           this, SLOT( triggerFilter() ) );

  QWidget     * searchBox    = new QWidget( this );
  QHBoxLayout * searchLayout = new QHBoxLayout;
  searchBox->setLayout( searchLayout );
  searchLayout->addWidget( d->label );
  searchLayout->addWidget( d->filter );
  searchLayout->setMargin( 0 );

  QWidget *     widget = new QWidget( this );
  QVBoxLayout * layout = new QVBoxLayout;
  widget->setLayout( layout );
  setWidget( widget );
  layout->setMargin( 3 );
  layout->addWidget( searchBox );
  layout->addWidget( d->list );

  d->filterTimer = new QTimer( this );
  d->filterTimer->setInterval( 500 );
  d->filterTimer->setSingleShot( true );
  connect( d->filterTimer, SIGNAL( timeout() ), SLOT( filter() ) );

  setMinimumWidth( 200 );
  setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  setWindowIcon( QIcon() );

  retranslateText();
}

void VariablesDock::updateList()
{
  filter();
}

VariablesDock::~VariablesDock()
{
  d->filterTimer->stop();
}

void VariablesDock::handleRadixCharacterChange()
{
  filter();
}

void VariablesDock::retranslateText()
{
  setWindowTitle( tr( "Variables" ) );
  d->label->setText( tr( "Search" ) );
  d->list->setLayoutDirection( Qt::LeftToRight );
  filter();
}

void VariablesDock::filter()
{
  d->filterTimer->stop();

  d->list->fillTable( d->filter->text() );
}

void VariablesDock::handleItem( QTreeWidgetItem * item )
{
  d->list->clearSelection();
  emit variableSelected( item->text( 0 ) );
}

void VariablesDock::triggerFilter()
{
  d->filterTimer->stop();
  d->filterTimer->start();
}

void VariablesDock::changeEvent( QEvent * e )
{
  if ( e->type() == QEvent::LayoutDirectionChange )
    retranslateText();
  else
    QDockWidget::changeEvent( e );
}

