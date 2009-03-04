// This file is part of the SpeedCrunch project
// Copyright (C) 2005 Ariya Hidayat <ariya@kde.org>
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


#include "deletevardlg.hxx"

#include "gui/variableswidget.hxx"

#include "core/evaluator.hxx"

#include <QPushButton>
#include <QTreeWidget>
#include <QVBoxLayout>


struct DeleteVariableDlg::Private
{
  VariablesWidget * list;

  QPushButton * deleteButton;
  QPushButton * closeButton;
};


DeleteVariableDlg::DeleteVariableDlg( QWidget * parent )
  : QDialog( parent ), d( new DeleteVariableDlg::Private )
{
  setWindowTitle( tr( "Delete Variable" ) );
  setModal( true );

  QVBoxLayout * layout = new QVBoxLayout;
  setLayout( layout );

  d->list = new VariablesWidget( VariablesWidget::ShowUserDefinedOnly, this );

  d->deleteButton = new QPushButton( this );
  d->deleteButton->setText( tr( "&Delete" ) );
  d->deleteButton->setDefault( true );

  d->closeButton = new QPushButton( this );
  d->closeButton->setText( tr( "Close" ) );

  QWidget * box = new QWidget( this );
  QHBoxLayout* boxLayout = new QHBoxLayout;
  boxLayout->setMargin( 0 );
  box->setLayout( boxLayout );

  boxLayout->addWidget( d->deleteButton );
  boxLayout->addItem( new QSpacerItem( 50, 0, QSizePolicy::MinimumExpanding,
                                       QSizePolicy::Minimum ) );
  boxLayout->addWidget( d->closeButton );

  layout->addWidget( d->list );
  layout->addWidget( box );

  connect( d->deleteButton, SIGNAL( clicked() ), this, SLOT( deleteVar() ) );
  connect( d->closeButton,  SIGNAL( clicked() ), this, SLOT( reject()    ) );

  d->list->setLayoutDirection( Qt::LeftToRight );

  updateList();
  adjustSize();
}


DeleteVariableDlg::~DeleteVariableDlg()
{
}


void DeleteVariableDlg::updateList()
{
  d->list->fillTable();
}


void DeleteVariableDlg::deleteVar()
{
  if ( d->list->selectedItems().count() > 0 )
  {
    QTreeWidgetItem * item = d->list->selectedItems().at(0);
    Evaluator::instance()->remove( item->text( 0 ) );
    delete item;
  }
}
