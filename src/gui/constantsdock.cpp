// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2007-2009 Helder Correia <helder.pereira.correia@gmail.com>
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


#include "gui/constantsdock.hxx"

#include "core/constants.hxx"
#include "core/settings.hxx"

#include <QtCore/QEvent>
#include <QtCore/QTimer>
#include <QtGui/QComboBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>

struct ConstantsDock::Private
{
  Constants *   constants;
  Settings *    settings;
  QComboBox *   category;
  QLineEdit *   filter;
  QTimer *      filterTimer;
  QTreeWidget * list;
  QLabel *      label;
  QLabel *      categoryLabel;
  QLabel *      noMatchLabel;
};

ConstantsDock::ConstantsDock( QWidget * parent )
  : QDockWidget( parent ), d( new ConstantsDock::Private )
{
  d->constants = Constants::instance();
  d->settings = Settings::instance();

  d->categoryLabel = new QLabel( this );

  d->category = new QComboBox( this );
  d->category->setEditable( false );
  d->category->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );

  connect( d->category, SIGNAL( activated( int ) ), SLOT( filter() ) );

  QWidget     * categoryBox    = new QWidget( this );
  QHBoxLayout * categoryLayout = new QHBoxLayout;
  categoryBox->setLayout( categoryLayout );
  categoryLayout->addWidget( d->categoryLabel );
  categoryLayout->addWidget( d->category );
  categoryLayout->setMargin( 0 );

  d->label = new QLabel( this );

  d->filter = new QLineEdit( this );
  d->filter->setMinimumWidth( fontMetrics().width('X') * 10 );

  connect( d->filter, SIGNAL( textChanged( const QString & ) ),
           this, SLOT( triggerFilter() ) );

  QWidget *     searchBox    = new QWidget( this );
  QHBoxLayout * searchLayout = new QHBoxLayout;
  searchBox->setLayout( searchLayout );
  searchLayout->addWidget( d->label );
  searchLayout->addWidget( d->filter );
  searchLayout->setMargin( 0 );

  d->list = new QTreeWidget( this );
  d->list->setAutoScroll( true );
  d->list->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
  d->list->setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );
  d->list->setColumnCount( 5 );
  d->list->setColumnHidden( 3, true );
  d->list->setRootIsDecorated( false );
  d->list->header()->hide();
  d->list->setMouseTracking( true );
  d->list->setEditTriggers( QTreeWidget::NoEditTriggers );
  d->list->setSelectionBehavior( QTreeWidget::SelectRows );
  d->list->setAlternatingRowColors( true );

  connect( d->list, SIGNAL( itemActivated( QTreeWidgetItem *, int ) ),
           this, SLOT( handleItem( QTreeWidgetItem * ) ) );

  QWidget *     widget = new QWidget( this );
  QVBoxLayout * layout = new QVBoxLayout;
  widget->setLayout( layout );
  setWidget( widget );
  layout->setMargin( 3 );
  layout->addWidget( categoryBox );
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

  retranslateText();
}

ConstantsDock::~ConstantsDock()
{
  d->filterTimer->stop();
}

void ConstantsDock::handleRadixCharacterChange()
{
  updateList();
}

void ConstantsDock::retranslateText()
{
  setWindowTitle( tr( "Constants" ) );
  d->categoryLabel->setText( tr( "Category" ) );
  d->label->setText( tr( "Search" ) );
  updateList();
}

void ConstantsDock::filter()
{
  Constants * c = d->constants;
  QString term = d->filter->text();

  d->filterTimer->stop();
  setUpdatesEnabled( false );

  QString chosenCategory = d->category->currentText();

  d->list->clear();
  for ( int k = 0; k < c->constantList().count(); k++ )
  {
    QStringList str;
    str << c->constantList().at( k ).name;
    QString radCh = (d->settings->radixCharacter() != '.' ?
        QString( c->constantList().at( k ).value ).replace( '.', d->settings->radixCharacter() )
      : c->constantList().at( k ).value );
    if ( layoutDirection() == Qt::RightToLeft )
    {
      str << c->constantList().at( k ).unit + QChar( 0x200e ); // Unicode LRM
      str << radCh;
    }
    else
    {
      str << radCh;
      str << c->constantList().at( k ).unit;
    }
    str << c->constantList().at( k ).name.toUpper();
    str << QString( "" );

    bool include = (chosenCategory == tr( "All" )) ?
      true : c->constantList().at( k ).categories.contains( chosenCategory );

    if ( ! include )
      continue;

    QTreeWidgetItem * item = 0;
    if ( term.isEmpty() )
      item = new QTreeWidgetItem( d->list, str );
    else
    {
      if ( str.at(0).contains( term, Qt::CaseInsensitive ) )
        item = new QTreeWidgetItem( d->list, str );
    }
    if ( item )
    {
      QString tip;
      tip += QString( QChar( 0x200E ) );
      tip += QString( "<b>%1</b><br>%2" )
                      .arg( c->constantList().at( k ).name,
                            c->constantList().at( k ).value );
      tip += QString( QChar( 0x200E ) );
      if ( ! c->constantList().at( k ).unit.isEmpty() )
        tip.append( " " ).append( c->constantList().at( k ).unit );
      if ( d->settings->radixCharacter() != '.' )
        tip.replace( '.', d->settings->radixCharacter());
      tip += QString( QChar( 0x200E ) );
      item->setToolTip( 0, tip );
      item->setToolTip( 1, tip );
      item->setToolTip( 2, tip );

      if ( layoutDirection() == Qt::RightToLeft )
      {
        item->setTextAlignment( 1, Qt::AlignRight );
        item->setTextAlignment( 2, Qt::AlignLeft  );
      }
      else
      {
        item->setTextAlignment( 1, Qt::AlignLeft );
        item->setTextAlignment( 2, Qt::AlignLeft );
      }
    }
  }

  d->list->resizeColumnToContents( 0 );
  d->list->resizeColumnToContents( 1 );
  d->list->resizeColumnToContents( 2 );

  if ( d->list->topLevelItemCount() > 0 ) {
    d->noMatchLabel->hide();
    d->list->sortItems( 0, Qt::AscendingOrder );
  } else {
    d->noMatchLabel->setGeometry( d->list->geometry() );
    d->noMatchLabel->show();
    d->noMatchLabel->raise();
  }

  setUpdatesEnabled( true );
}

void ConstantsDock::handleItem( QTreeWidgetItem * item )
{
  Constants * c = d->constants;
  for ( int k = 0; k < c->constantList().count(); k++ )
    if ( c->constantList().at( k ).name == item->text( 0 ) )
      emit constantSelected( c->constantList().at( k ).value );
}

void ConstantsDock::triggerFilter()
{
  d->filterTimer->stop();
  d->filterTimer->start();
}

void ConstantsDock::updateList()
{
  Constants * c = d->constants;
  d->category->clear();
  d->category->addItems( c->categoryList() );
  d->category->insertItem( 0, tr( "All" ) );
  d->category->setCurrentIndex( 0 );
  filter();
}

void ConstantsDock::changeEvent( QEvent * e )
{
  if ( e->type() == QEvent::LayoutDirectionChange )
    retranslateText();
  else
    QDockWidget::changeEvent( e );
}

