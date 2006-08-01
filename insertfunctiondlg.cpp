/* This file is part of the SpeedCrunch project
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
#include <QTreeWidget>
#include <QPushButton>
#include <QTimer>
#include <QStringList>

InsertFunctionDlg::InsertFunctionDlg( QWidget* parent, const char* name ):
QDialog( parent, name )
{
  ui.setupUi( this );

  QStringList headers;
  headers << tr("Name") << tr("Description");
  ui.treeWidget->setHeaderLabels( headers );

  ui.okButton->setText( tr("&Insert") );

  connect( ui.okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( ui.treeWidget, SIGNAL( returnPressed() ), this, SLOT( accept() ) );
  connect( ui.treeWidget, SIGNAL( doubleClicked( QTreeWidgetItem*, int ) ),
    this, SLOT( accept() ) );

  QStringList functionNames = FunctionRepository::self()->functionNames();
  for( unsigned i = 0; i < functionNames.count(); i++ )
  {
    Function* f = FunctionRepository::self()->function( functionNames[i] );
    if( f )
    {
      QStringList itemTexts;
      itemTexts << f->name() << f->description();
      new QTreeWidgetItem( ui.treeWidget, itemTexts );
    }
  }

  ui.treeWidget->setFocus();
  setCaption( tr("Insert Function") );

  QTimer::singleShot( 0, this, SLOT( initUI() ) );
}

InsertFunctionDlg::~InsertFunctionDlg()
{
}

QString InsertFunctionDlg::functionName() const
{
  QTreeWidgetItem* item = ui.treeWidget->currentItem();
  return item ? item->text(0).lower() : QString();
}

void InsertFunctionDlg::initUI()
{
  adjustSize();
}
