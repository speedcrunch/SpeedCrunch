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

#include "insertvardlg.h"
#include "evaluator.h"
#include "hmath.h"
#include "settings.h"

#include <QDialog.h>
#include <QTreeWidget>
#include <QPushButton>
#include <QTimer>
#include <QStringList>

class InsertVariableDlg::Private
{
public:
  Evaluator* eval;
};

InsertVariableDlg::InsertVariableDlg( Evaluator* e, QWidget* parent, const char* name ):
QDialog( parent, name )
{
  d = new Private;
  d->eval = e;

  ui.setupUi( this );

  QStringList headers;

  headers << tr("Name") << tr("Value");
  ui.treeWidget->setHeaderLabels( headers );

  ui.okButton->setText( tr("&Insert") );
  ui.okButton->setDefault( true );

  connect( ui.okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
  // FIXME: cannot get the return pressed event to trigger propertly
  connect( ui.treeWidget, SIGNAL( returnPressed() ), this, SLOT( accept() ) );
  connect( ui.treeWidget, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ),
    this, SLOT( accept() ) );

  setCaption( tr("Insert Variable") );
  QTimer::singleShot( 0, this, SLOT( initUI() ) );
}

InsertVariableDlg::~InsertVariableDlg()
{
  delete d;
}

void InsertVariableDlg::updateList()
{
  ui.treeWidget->clear();
  QVector<Variable> vars = d->eval->variables();
  for( unsigned i = 0; i < vars.count(); i++ )
  {
    char* ss = HMath::format( vars[i].value, 'g' );
    QStringList itemTexts;

    itemTexts << vars[i].name << QString( ss );
    QTreeWidgetItem *item = new QTreeWidgetItem( ui.treeWidget, itemTexts );
    item->setTextAlignment( 1, Qt::AlignRight );
    free( ss );
  }

  ui.treeWidget->setFocus();
}

QString InsertVariableDlg::variableName() const
{
  QTreeWidgetItem* item = ui.treeWidget->currentItem();
  return item ? item->text(0) : QString();
}


void InsertVariableDlg::initUI()
{
  updateList();
  adjustSize();
}
