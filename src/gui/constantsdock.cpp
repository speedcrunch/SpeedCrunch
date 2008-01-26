// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2007-2008 Helder Correia <helder.pereira.correia@gmail.com>
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


#include <base/constants.hxx>
#include <base/settings.hxx>
#include <gui/constantsdock.hxx>

#include <QComboBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QLocale>
#include <QTimer>
#include <QTreeWidget>
#include <QVBoxLayout>


struct ConstantsDockPrivate
{
  QChar radixChar;

  QComboBox   * category;
  QLineEdit   * filter;
  QTimer      * filterTimer;
  QTreeWidget * list;
  QLabel      * noMatchLabel;
};


// public

ConstantsDock::ConstantsDock( QWidget * parent )
  : QDockWidget( tr("Constants"), parent ), d( new ConstantsDockPrivate )
{
  QLabel * categorylabel = new QLabel( this );
  categorylabel->setText( tr("Category") );

  d->category = new QComboBox( this );
  d->category->setEditable( false );
  d->category->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
  connect( d->category, SIGNAL(activated( int )), SLOT(filter()) );

  QWidget     * categoryBox    = new QWidget( this );
  QHBoxLayout * categoryLayout = new QHBoxLayout;
  categoryBox->setLayout( categoryLayout );
  categoryLayout->addWidget( categorylabel );
  categoryLayout->addWidget( d->category );
  categoryLayout->setMargin( 0 );

  QLabel * label = new QLabel( this );
  label->setText( tr("Search") );

  d->filter = new QLineEdit( this );
  d->filter->setMinimumWidth( fontMetrics().width('X') * 10 );
  connect( d->filter, SIGNAL(textChanged( const QString& )), SLOT(triggerFilter()) );

  QWidget     * searchBox    = new QWidget( this );
  QHBoxLayout * searchLayout = new QHBoxLayout;
  searchBox->setLayout( searchLayout );
  searchLayout->addWidget( label );
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
  connect( d->list, SIGNAL(itemDoubleClicked( QTreeWidgetItem*, int )), SLOT(handleItem( QTreeWidgetItem* )) );

  QWidget     * widget = new QWidget( this );
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
  connect( d->filterTimer, SIGNAL(timeout()), SLOT(filter()) );

  d->noMatchLabel = new QLabel( this );
  d->noMatchLabel->setText( tr("No match found") );
  d->noMatchLabel->setAlignment( Qt::AlignCenter );
  d->noMatchLabel->adjustSize();
  d->noMatchLabel->hide();

  setMinimumWidth( 200 );
  setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  setWindowIcon( QIcon() ); // no icon

  updateList();
}


ConstantsDock::~ConstantsDock()
{
  d->filterTimer->stop();
  delete d;
}


QChar ConstantsDock::radixChar() const
{
  return d->radixChar;
}


// public slots

void ConstantsDock::setRadixChar( QChar c )
{
  if ( d->radixChar != c )
  {
    d->radixChar = c;
    updateList();
  }
}


// protected slots

void ConstantsDock::filter()
{
  Constants * c    = Constants::self();
  QString     term = d->filter->text();

  d->filterTimer->stop();
  setUpdatesEnabled(false);

  QString chosenCategory = d->category->currentText();

  d->list->clear();
  for ( int k = 0; k < c->constantList.count(); k++ )
  {
      QStringList str;
      str << c->constantList[k].name;
      if ( QLocale().language() == QLocale::Hebrew )
      {
	str << c->constantList[k].unit;
	str << (d->radixChar == '.' ?
                  c->constantList[k].value.replace( ',', '.' )
                    : c->constantList[k].value.replace( '.', ',' ) );
      }
      else
      {
	str << (d->radixChar == '.' ?
                  c->constantList[k].value.replace( ',', '.' )
                    : c->constantList[k].value.replace( '.', ',' ));
	str << c->constantList[k].unit;
      }
      str << c->constantList[k].name.toUpper();
      str << QString( "" );

      bool include = (chosenCategory == tr("All")) ? true:
        c->constantList[k].categories.contains( chosenCategory );

      if ( ! include )
        continue;

      QTreeWidgetItem * item = 0;
      if ( term.isEmpty() )
        item = new QTreeWidgetItem( d->list, str );
      else
      {
        if ( str[0].contains( term, Qt::CaseInsensitive ) )
          item = new QTreeWidgetItem( d->list, str );
      }
      if ( item )
      {
        QString tip = QString("<b>%1</b><br>%2").arg( c->constantList[k].name, c->constantList[k].value );
        if ( !c->constantList[k].unit.isEmpty() )
          tip.append( " " ).append( c->constantList[k].unit );
        item->setToolTip( 0, tip );
        item->setToolTip( 1, tip );
        item->setToolTip( 2, tip );

	item->setTextAlignment( 1, Qt::AlignRight );
	item->setTextAlignment( 2, Qt::AlignLeft  );
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
        item->setBackground( 4, c );
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


void ConstantsDock::handleItem( QTreeWidgetItem * item )
{
  Constants * c = Constants::self();
  for ( int k = 0; k < c->constantList.count(); k++ )
    if ( c->constantList[k].name == item->text( 0 ) )
      emit constantSelected( c->constantList[k].value );
}


void ConstantsDock::triggerFilter()
{
  d->filterTimer->stop();
  d->filterTimer->start();
}


void ConstantsDock::updateList()
{
  Constants * c = Constants::self();

  d->category->clear();
  d->category->addItems( c->categoryList );
  d->category->insertItem( 0, tr("All") );
  d->category->setCurrentIndex( 0 );

  filter();
}
