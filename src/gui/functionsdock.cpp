// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2008 Helder Correia <helder.pereira.correia@gmail.com>
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

#include "core/functions.hxx"

#include <QEvent>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>
#include <QTreeWidget>
#include <QVBoxLayout>

struct FunctionsDock::Private
{
  const Functions * functions;
  QLineEdit *       filter;
  QTimer *          filterTimer;
  QStringList       functionDesc;
  QStringList       functionNames;
  QTreeWidget *     list;
  QLabel *          label;
  QLabel *          noMatchLabel;
};

FunctionsDock::FunctionsDock( const Functions * f, QWidget * parent )
  : QDockWidget( parent ), d( new FunctionsDock::Private )
{
  d->functions = f;
  d->label = new QLabel( this );
  d->filter = new QLineEdit( this );

  connect( d->filter, SIGNAL( textChanged( const QString & ) ),
           SLOT( triggerFilter()) );

  QWidget * searchBox = new QWidget( this );
  QHBoxLayout * searchLayout = new QHBoxLayout;
  searchBox->setLayout( searchLayout );
  searchLayout->addWidget( d->label );
  searchLayout->addWidget( d->filter );
  searchLayout->setMargin( 0 );

  d->list = new QTreeWidget( this );
  d->list->setAutoScroll( true );
  d->list->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
  d->list->setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );
  d->list->setColumnCount( 3 );
  d->list->setRootIsDecorated( false );
  d->list->header()->hide();
  d->list->setEditTriggers( QTreeWidget::NoEditTriggers );
  d->list->setSelectionBehavior( QTreeWidget::SelectRows );

  connect( d->list, SIGNAL( itemActivated( QTreeWidgetItem *, int ) ),
           this, SLOT( handleItem( QTreeWidgetItem * ) ) );

  QWidget * widget = new QWidget( this );
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

  d->noMatchLabel = new QLabel( this );
  d->noMatchLabel->setAlignment( Qt::AlignCenter );
  d->noMatchLabel->adjustSize();
  d->noMatchLabel->hide();

  setMinimumWidth( 200 );
  setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  setWindowIcon( QIcon() );

  retranslateText();
}

FunctionsDock::~FunctionsDock()
{
  d->filterTimer->stop();
  delete d;
}

const Functions * FunctionsDock::functions() const
{
  return d->functions;
}

void FunctionsDock::retranslateText()
{
  setWindowTitle( tr( "Functions" ) );
  d->label->setText( tr( "Search" ) );
  d->noMatchLabel->setText( tr( "No match found" ) );

  d->functionNames.clear();
  d->functionDesc.clear();
  QStringList functionNames = d->functions->functionNames();
  for ( int i = 0; i < functionNames.count(); i++ )
  {
    Function * f = d->functions->function( functionNames[i] );
    if ( f )
    {
      d->functionNames << f->name();
      d->functionDesc  << f->description();
    }
  }

  filter();
}

void FunctionsDock::filter()
{
  QString term = d->filter->text();

  setUpdatesEnabled( false );

  d->list->clear();
  for ( int k = 0; k < d->functionNames.count(); k++ )
  {
      QStringList str;
      str << d->functionNames[k];
      str << d->functionDesc[k];
      str << QString( "" );

      QTreeWidgetItem * item = 0;

      if ( term.isEmpty() )
      {
        item = new QTreeWidgetItem( d->list, str );
      }
      else
      {
        if (    str[0].contains( term, Qt::CaseInsensitive )
             || str[1].contains( term, Qt::CaseInsensitive ) )
          item = new QTreeWidgetItem( d->list, str );
      }

      if ( item && layoutDirection() == Qt::RightToLeft )
      {
        item->setTextAlignment( 0, Qt::AlignRight | Qt::AlignVCenter );
        item->setTextAlignment( 1, Qt::AlignLeft  | Qt::AlignVCenter );
      }
  }

  d->list->resizeColumnToContents( 0 );
  d->list->resizeColumnToContents( 1 );
  d->list->resizeColumnToContents( 2 );

  if ( d->list->topLevelItemCount() > 0 )
  {
    d->noMatchLabel->hide();
    d->list->sortItems( 0, Qt::AscendingOrder );

    int group = 3;
    if ( d->list->topLevelItemCount() >= 2 * group )
      for ( int i = 0; i < d->list->topLevelItemCount(); i++ )
      {
        QTreeWidgetItem * item = d->list->topLevelItem( i );
        QBrush c = ((int)(i / group)) & 1 ? palette().base() : palette().alternateBase();
        item->setBackground( 0, c );
        item->setBackground( 1, c );
        item->setBackground( 2, c );
      }
  }
  else
  {
    d->noMatchLabel->setGeometry( d->list->geometry() );
    d->noMatchLabel->show();
    d->noMatchLabel->raise();
  }

  setUpdatesEnabled( true );
}

void FunctionsDock::handleItem( QTreeWidgetItem * item )
{
  d->list->clearSelection();
  emit functionSelected( item->text( 0 ) );
}

void FunctionsDock::triggerFilter()
{
  d->filterTimer->stop();
  d->filterTimer->start();
}

void FunctionsDock::changeEvent( QEvent * e )
{
  if ( e->type() == QEvent::LayoutDirectionChange )
    retranslateText();
  else
    QDockWidget::changeEvent( e );
}

