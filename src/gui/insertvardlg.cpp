// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2006 Johan Thelin <e8johan@gmail.com>
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


#include "insertvardlg.hxx"

#include "core/evaluator.hxx"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLocale>
#include <QPushButton>
#include <QStringList>
#include <QTimer>
#include <QTreeWidget>
#include <QVBoxLayout>


struct InsertVariableDlg::Private
{
  char          radixChar;
  QPushButton * cancelButton;
  Evaluator *   eval;
  QPushButton * insertButton;
  QTreeWidget * list;

  QString formatValue( const HNumber & value );
};


QString InsertVariableDlg::Private::formatValue( const HNumber & value )
{
  char * str = HMath::format( value, 'g' );
  QString s;
  if ( radixChar == '.' )
    s = QString::fromLatin1( str );
  else
    s = QString::fromLatin1( str ).replace( '.', ',' );
  free( str );
  return s;
}


// public

InsertVariableDlg::InsertVariableDlg( Evaluator * eval, char radixChar,
                                      QWidget * parent )
  : QDialog( parent ), d( new InsertVariableDlg::Private )
{
  setWindowTitle( tr( "Insert Variable" ) );
  setModal( true );

  d->eval = eval;

  if ( radixChar == 'C' )
    d->radixChar = QLocale().decimalPoint().toAscii();
  else
    d->radixChar = radixChar;

  QVBoxLayout* layout = new QVBoxLayout;
  setLayout( layout );

  d->list = new QTreeWidget( this );
  d->list->setColumnCount( 2 );
  d->list->setAlternatingRowColors( true );
  d->list->setRootIsDecorated( false );
  d->list->setEditTriggers( QTreeWidget::NoEditTriggers );
  d->list->setSelectionBehavior( QTreeWidget::SelectRows );

  QStringList titles;
  titles << tr( "Name"  );
  titles << tr( "Value" );
  d->list->setHeaderLabels( titles );

  d->insertButton = new QPushButton( this );
  d->insertButton->setText( tr("&Insert") );
  d->insertButton->setDefault( true );

  d->cancelButton = new QPushButton( this );
  d->cancelButton->setText( tr("Cancel") );

  QWidget* box = new QWidget( this );
  QHBoxLayout* boxLayout = new QHBoxLayout;
  boxLayout->setMargin( 0 );
  box->setLayout( boxLayout );

  boxLayout->addWidget( d->insertButton );
  boxLayout->addItem( new QSpacerItem( 50, 0, QSizePolicy::MinimumExpanding,
                                       QSizePolicy::Minimum ) );
  boxLayout->addWidget( d->cancelButton );

  layout->addWidget( d->list );
  layout->addWidget( box );

  connect( d->insertButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( d->cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( d->list, SIGNAL( itemActivated( QTreeWidgetItem *, int ) ),
           this, SLOT(accept()) );
  connect( d->list, SIGNAL( itemDoubleClicked( QTreeWidgetItem *, int ) ),
           this, SLOT(accept()) );

  d->list->setLayoutDirection( Qt::LeftToRight );

  updateList();
  adjustSize();
}


void InsertVariableDlg::updateList()
{
  d->list->setUpdatesEnabled( false );

  d->list->clear();
  QVector<Variable> variables = d->eval->variables();
  for ( int k = 0; k < variables.count(); k++ )
  {
      QStringList str;
      str << variables.at(k).name;
      str << d->formatValue( variables.at(k).value );

      QTreeWidgetItem * item = 0;
      item = new QTreeWidgetItem( d->list, str );
      item->setTextAlignment( 0, Qt::AlignLeft | Qt::AlignVCenter );
      item->setTextAlignment( 1, Qt::AlignLeft | Qt::AlignVCenter );
  }
  d->list->sortItems( 0, Qt::AscendingOrder );

  d->list->setUpdatesEnabled( true );
}


QString InsertVariableDlg::variableName() const
{
  QTreeWidgetItem* item = d->list->currentItem();
  return item ? item->text(0) : QString();
}


InsertVariableDlg::~InsertVariableDlg()
{
  delete d;
}


// public slots

void InsertVariableDlg::setRadixChar( char c )
{
  if ( c == 'C' )
    c = QLocale().decimalPoint().toAscii();
  if ( d->radixChar != c )
  {
    d->radixChar = c;
    updateList();
  }
}
