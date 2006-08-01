/* This file is part of the SpeedCrunch project
   Copyright (C) 2005 Ariya Hidayat <ariya@kde.org>

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

#include "deletevardlg.h"
#include "evaluator.h"
#include "hmath.h"
#include "settings.h"

#include <QDialog>
#include <QTreeWidget>
#include <QPushButton>
#include <QTimer>
#include <QStringList>

class DeleteVariableDlg::Private
{
public:
  Evaluator* eval;
};

DeleteVariableDlg::DeleteVariableDlg( Evaluator* e, QWidget* parent, const char* name ):
QDialog( parent, name )
{
  d = new Private;
  d->eval = e;

  ui.setupUi( this );

  QStringList headerTexts;
  headerTexts << tr("Name") << tr("Value");
  ui.treeWidget->setHeaderLabels( headerTexts );

  ui.okButton->setText( tr("&Delete") );
  ui.cancelButton->setText( tr("Close") );

  connect( ui.okButton, SIGNAL( clicked() ), this, SLOT( deleteVar() ) );
  connect( ui.cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );

  QTimer::singleShot( 0, this, SLOT( initUI() ) );
  setCaption( tr("Delete Variable") );
}

DeleteVariableDlg::~DeleteVariableDlg()
{
  delete d;
}

void DeleteVariableDlg::updateList()
{
  ui.treeWidget->clear();
  QVector<Variable> vars = d->eval->variables();
  for( unsigned i = 0; i < vars.count(); i++ )
  {
    QString var = vars[i].name;
    if( var.upper() == QString("ANS") ) continue;
    if( var.upper() == QString("PI") ) continue;
    char* ss = HMath::format( vars[i].value, 'g' );
    QStringList itemTexts;
    itemTexts << vars[i].name << QString( ss );
    QTreeWidgetItem *item = new QTreeWidgetItem( ui.treeWidget, itemTexts );
    item->setTextAlignment( 1, Qt::AlignRight );
    free( ss );
  }

  ui.treeWidget->setFocus();
}

void DeleteVariableDlg::initUI()
{
  updateList();
  adjustSize();
}

void DeleteVariableDlg::deleteVar()
{
  if( ui.treeWidget->selectedItems().count() > 0 )
  {
    QTreeWidgetItem *item = ui.treeWidget->selectedItems()[0];
    d->eval->remove( item->text(0) );
    delete item;
  }
}
