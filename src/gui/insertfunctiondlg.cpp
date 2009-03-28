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

#include "gui/insertfunctiondlg.hxx"

#include "gui/functionswidget.hxx"

#include <QtGui/QPushButton>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>

struct InsertFunctionDlg::Private
{
    FunctionsWidget * list;
};

InsertFunctionDlg::InsertFunctionDlg( QWidget * parent )
    : QDialog( parent ), d( new InsertFunctionDlg::Private )
{
    setWindowTitle( tr("Insert Function") );
    setModal( true );

    QVBoxLayout * layout = new QVBoxLayout;
    setLayout( layout );

    d->list = new FunctionsWidget( this );
    layout->addWidget( d->list );

    connect( d->list, SIGNAL(functionSelected(const QString &)), SLOT(accept()) );

    adjustSize();
}

QString InsertFunctionDlg::selectedFunctionName() const
{
    const QTreeWidgetItem * item = d->list->currentItem();
    return item ? item->text( 0 ).toLower() : QString();
}

InsertFunctionDlg::~InsertFunctionDlg()
{
}

