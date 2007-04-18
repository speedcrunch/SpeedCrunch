/* This file is part of the SpeedCrunch project
   Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2006 Johan Thelin <e8johan@gmail.com>

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

#include "insertfunctiondlg.h"
#include "functions.h"
#include "settings.h"

#include <QDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QStringList>
#include <QTimer>
#include <QTreeWidget>
#include <QVBoxLayout>

class InsertFunctionDlgPrivate
{
public:
  QTreeWidget* list;
  QPushButton* insertButton;
  QPushButton* cancelButton;
};

InsertFunctionDlg::InsertFunctionDlg( QWidget* parent, const char* name ):
QDialog( parent, name )
{
  d = new InsertFunctionDlgPrivate;

  setWindowTitle( tr("Insert Function") );
  setCaption( tr("Insert Function") );
  setModal( true );

  QVBoxLayout* layout = new QVBoxLayout;
  setLayout( layout );

  d->list = new QTreeWidget( this );
  d->list->setColumnCount( 2 );
  d->list->setAlternatingRowColors( true );
  d->list->setRootIsDecorated( false );
  d->list->setEditTriggers( QTreeWidget::NoEditTriggers );
  d->list->setSelectionBehavior( QTreeWidget::SelectRows );

  QStringList titles;
  titles << tr("Name");
  titles << tr("Description");
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
  boxLayout->addItem( new QSpacerItem( 50, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum ) );
  boxLayout->addWidget( d->cancelButton );

  layout->addWidget( d->list );
  layout->addWidget( box );

  connect( d->insertButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( d->cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( d->list, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( accept() ) );

  QTimer::singleShot( 0, this, SLOT( initUI() ) );
}

InsertFunctionDlg::~InsertFunctionDlg()
{
  delete d;
}

QString InsertFunctionDlg::functionName() const
{
  QTreeWidgetItem* item = d->list->currentItem();
  return item ? item->text(0).lower() : QString();
}

void InsertFunctionDlg::initUI()
{
  QStringList functionNames = FunctionRepository::self()->functionNames();
  int k = 0;
  for( int i = 0; i < functionNames.count(); i++ )
  {
    Function* f = FunctionRepository::self()->function( functionNames[i] );
    if( f )
    {
      QStringList str;
      str << f->name();
      str << f->description();
      new QTreeWidgetItem( d->list, str );
      k++;
    }
  }

  d->list->sortItems( 0, Qt::AscendingOrder );
  d->list->setFocus();
  d->list->setCurrentItem( d->list->itemAt( 0, 0 ) );

  adjustSize();
}
