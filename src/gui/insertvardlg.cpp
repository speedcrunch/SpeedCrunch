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

#include "gui/variabletable.hxx"

#include "core/evaluator.hxx"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLocale>
#include <QPushButton>
#include <QStringList>
#include <QTimer>
#include <QVBoxLayout>


struct InsertVariableDlg::Private
{
  VariableTable * list;

  QPushButton * insertButton;
  QPushButton * cancelButton;
};


// public

InsertVariableDlg::InsertVariableDlg( Evaluator * eval, QWidget * parent )
  : QDialog( parent ), d( new InsertVariableDlg::Private )
{
  setWindowTitle( tr( "Insert Variable" ) );
  setModal( true );

  QVBoxLayout* layout = new QVBoxLayout;
  setLayout( layout );

  d->list = new VariableTable( eval, false, this );

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
  d->list->fillTable();
}


QString InsertVariableDlg::variableName() const
{
  QTreeWidgetItem* item = d->list->currentItem();
  return item ? item->text(0) : QString();
}


InsertVariableDlg::~InsertVariableDlg()
{
}
