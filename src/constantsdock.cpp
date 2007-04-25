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

#include <QComboBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QTimer>
#include <QTreeWidget>
#include <QVBoxLayout>


class Constant
{
public:
  QString name;
  QString value;
  QString unit;
  QStringList categories;

  Constant( const QString& n, const QString& v, const QString& u, const QStringList& cat ):
    name( n ), value( v ), unit( u ), categories( cat ) {}

  Constant( const QString& n, const QString& v, const QString& u, const QString& cat ):
    name( n ), value( v ), unit( u ) { categories << cat; }

  Constant( const QString& n, const QString& v, const QString& u, const QString& cat1,   const QString& cat2):
    name( n ), value( v ), unit( u ) {  categories << cat1; categories << cat2; }

};

class ConstantsDockPrivate
{
  public:
    QList<Constant> constants;
    QComboBox* category;
    QTreeWidget* list;
    QLineEdit* filter;
    QTimer* filterTimer;
    QLabel* noMatchLabel;
};

ConstantsDock::ConstantsDock( QWidget* parent ): QDockWidget( tr("Constants"), parent )
{
  d = new ConstantsDockPrivate;

  QLabel* categorylabel = new QLabel( this );
  categorylabel->setText( tr("Category") );

  d->category = new QComboBox( this );
  d->category->setEditable( false );
  d->category->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
  connect( d->category, SIGNAL( activated( int ) ), SLOT( filter() ) );

  QWidget* categoryBox = new QWidget( this );
  QHBoxLayout* categoryLayout = new QHBoxLayout;
  categoryBox->setLayout( categoryLayout );
  categoryLayout->addWidget( categorylabel );
  categoryLayout->addWidget( d->category );
  categoryLayout->setMargin( 0 );

  QLabel* label = new QLabel( this );
  label->setText( tr("Search") );

  d->filter = new QLineEdit( this );
  d->filter->setMinimumWidth( fontMetrics().width('X')*10 );
  connect( d->filter, SIGNAL( textChanged( const QString& ) ), SLOT( triggerFilter() ) );

  QWidget* searchBox = new QWidget( this );
  QHBoxLayout* searchLayout = new QHBoxLayout;
  searchBox->setLayout( searchLayout );
  searchLayout->addWidget( label );
  searchLayout->addWidget( d->filter );
  searchLayout->setMargin( 0 );

  d->list = new QTreeWidget( this );
  d->list->setColumnCount( 1 );
  d->list->setRootIsDecorated( false );
  d->list->header()->hide();
  d->list->setMouseTracking( true );
  d->list->setEditTriggers( QTreeWidget::NoEditTriggers );
  d->list->setSelectionBehavior( QTreeWidget::SelectRows );
  connect( d->list, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ),
    SLOT( handleItem( QTreeWidgetItem* ) ) );

  QWidget* widget = new QWidget( this );
  QVBoxLayout* layout = new QVBoxLayout;
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

  // http://en.wikipedia.org/wiki/Physical_constant#Table_of_atomic_and_nuclear_constants
  d->constants += Constant( tr("Bohr radius"), "0.5291772108e-10", "m", tr("Atomic & Nuclear") );
  d->constants += Constant( tr("Fermi coupling constant"), "1.16639e-5", "Ge/V^2", tr("Atomic & Nuclear") );
  d->constants += Constant( tr("fine-structure constant"), "7.297352568e-3", QString(), tr("Atomic & Nuclear") );
  d->constants += Constant( tr("Hartree energy"), "4.35974417e-18", "J", tr("Atomic & Nuclear") );
  d->constants += Constant( tr("quantum of circulation"), "3.636947550e-4", "m^2/s", tr("Atomic & Nuclear") );
  d->constants += Constant( tr("Rydberg constant"), "10973731.568525", "/m", tr("Atomic & Nuclear") );
  d->constants += Constant( tr("Thomson cross section"), "0.665245873e-28", "m^2", tr("Atomic & Nuclear") );
  d->constants += Constant( tr("weak mixing angle"), "0.22215", QString(), tr("Atomic & Nuclear") );

  // http://www.astronomynotes.com/tables/tablesa.htm
  d->constants += Constant( tr("astronomical unit"), "149597870.691", "km", tr("Astronomy") );
  d->constants += Constant( tr("light year"), "9.460536207e12", "km", tr("Astronomy") );
  d->constants += Constant( tr("parsec"), "3.08567802e13", "km", tr("Astronomy") );
  d->constants += Constant( tr("sidereal year"), "365.2564", "days", tr("Astronomy") );
  d->constants += Constant( tr("tropical year"), "365.2422", "days", tr("Astronomy") );
  d->constants += Constant( tr("Gregorian year"), "365.2425", "days", tr("Astronomy") );
  d->constants += Constant( tr("Earth mass"), "5.9736e24", "kg", tr("Astronomy") );
  d->constants += Constant( tr("Sun mass"), "1.9891e30", "kg", tr("Astronomy") );
  d->constants += Constant( tr("mean Earth radius"), "6371", "km", tr("Astronomy") );
  d->constants += Constant( tr("Sun radius"), "6.96265e5", "km", tr("Astronomy") );
  d->constants += Constant( tr("Sun luminosity"), "3.827e26", "W", tr("Astronomy") );

  QStringList categoryList;
  for( int k = 0; k < d->constants.count(); k++ )
  {
    QStringList cats = d->constants[k].categories;
    for( int i = 0; i < cats.count(); i++ )
      if( !categoryList.contains( cats[i] ) )
        categoryList += cats[i];
  }
  categoryList.sort();
  d->category->addItems( categoryList );
  d->category->insertItem( 0, tr("All") );
  d->category->setCurrentIndex( 0 );

  filter();
  adjustSize();
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

  QString chosenCategory = d->category->currentText();

  d->list->clear();
  for( int k = 0; k < d->constants.count(); k++ )
  {
      QStringList str;
      str << d->constants[k].name;

      bool include = (chosenCategory == tr("All")) ? true:
        d->constants[k].categories.contains( chosenCategory );

      if( !include )
        continue;

      QTreeWidgetItem* item = 0;
      if( term.isEmpty() )
        item = new QTreeWidgetItem( d->list, str );
      else
      {
        if( str[0].contains(term, Qt::CaseInsensitive) )
          item = new QTreeWidgetItem( d->list, str );
      }
      if( item )
      {
        QString tip = QString("<b>%1</b><br> %2").arg(d->constants[k].name, d->constants[k].value );
        if( !d->constants[k].unit.isEmpty() )
          tip.append( " " ).append( d->constants[k].unit );
        item->setToolTip( 0, tip );
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
  for( int k = 0; k < d->constants.count(); k++ )
    if( d->constants[k].name == item->text(0) )
      emit constantSelected( d->constants[k].value );
}

