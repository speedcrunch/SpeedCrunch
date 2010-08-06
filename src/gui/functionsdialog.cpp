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

#include "gui/functionsdialog.h"

#include "gui/functionswidget.h"

#include <QtGui/QPushButton>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>

FunctionsDialog::FunctionsDialog( QWidget * parent )
    : QDialog( parent ), m_widget( new FunctionsWidget(this) )
{
    setWindowTitle( tr("Insert Function") );

    QVBoxLayout * layout = new QVBoxLayout;
    layout->addWidget( m_widget );
    setLayout( layout );

    connect( m_widget, SIGNAL(functionSelected(const QString &)), SLOT(accept()) );

    adjustSize();
}

QString FunctionsDialog::selectedFunctionName() const
{
    const QTreeWidgetItem * item = m_widget->currentItem();
    return item ? item->text( 0 ).toLower() : QString();
}

FunctionsDialog::~FunctionsDialog()
{
}

