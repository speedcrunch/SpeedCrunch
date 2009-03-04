// This file is part of the SpeedCrunch project
// Copyright (C) 2009 Andreas Scherer <andreas_coder@freenet.de>
// Copyright (C) 2009 Helder Correia <helder.pereira.correia@gmail.com>
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

#include "gui/functionswidget.hxx"

#include "3rdparty/flickcharm/flickcharm.h"
#include "core/functions.hxx"
#include "core/settings.hxx"

#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>

struct FunctionsWidget::Private
{
    QTimer * filterTimer;
    FlickCharm flickCharm;
    QTreeWidget * functions;
    bool insertAllItems;
    QLabel * noMatchLabel;
    QLabel * searchLabel;
    QLineEdit * searchFilter;
};

FunctionsWidget::FunctionsWidget( QWidget * parent )
    : QWidget( parent ), d( new FunctionsWidget::Private )
{
    d->filterTimer = new QTimer( this );
    d->functions = new QTreeWidget( this );
    d->noMatchLabel = new QLabel( d->functions );
    d->flickCharm.activateOn( d->functions );
    d->searchFilter = new QLineEdit( this );
    d->searchLabel = new QLabel( this );

    d->filterTimer->setInterval( 500 );
    d->filterTimer->setSingleShot( true );

    d->functions->setAutoScroll( true );
    d->functions->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    d->functions->setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );
    d->functions->setColumnCount( 2 );
    d->functions->setAlternatingRowColors( true );
    d->functions->setRootIsDecorated( false );
    d->functions->setEditTriggers( QTreeWidget::NoEditTriggers );
    d->functions->setSelectionBehavior( QTreeWidget::SelectRows );
    d->functions->setCursor( QCursor(Qt::PointingHandCursor) );

    d->noMatchLabel->setAlignment( Qt::AlignCenter );
    d->noMatchLabel->adjustSize();
    d->noMatchLabel->hide();

    QWidget * searchBox = new QWidget( this );
    QHBoxLayout * searchLayout = new QHBoxLayout;
    searchLayout->addWidget( d->searchLabel );
    searchLayout->addWidget( d->searchFilter );
    searchLayout->setMargin( 0 );
    searchBox->setLayout( searchLayout );

    QVBoxLayout * layout = new QVBoxLayout;
    layout->setMargin( 3 );
    layout->addWidget( searchBox );
    layout->addWidget( d->functions );
    setLayout( layout );

    retranslateText();

    connect( d->filterTimer, SIGNAL(timeout()), SLOT(fillTable()) );
    connect( d->functions, SIGNAL(itemActivated(QTreeWidgetItem *, int)),
             SLOT(catchItemActivated(QTreeWidgetItem *, int)) );
    connect( d->functions, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
             SLOT(catchItemDoubleClicked(QTreeWidgetItem *, int)) );
    connect( d->searchFilter, SIGNAL(textChanged(const QString &)), SLOT(triggerFilter()) );
}

FunctionsWidget::~FunctionsWidget()
{
    d->filterTimer->stop();
}

void FunctionsWidget::fillTable()
{
    setUpdatesEnabled( false );

    d->filterTimer->stop();
    d->functions->clear();
    QString term = d->searchFilter->text();
    QStringList functionNames = Functions::instance()->functionNames();

    for ( int k = 0; k < functionNames.count(); k++ )
    {
        Function * f = Functions::instance()->function( functionNames.at(k) );
        if ( ! f )
            continue;

        QStringList str;
        str << f->name() << f->description();

        if ( term.isEmpty()
             || str.at(0).contains(term, Qt::CaseInsensitive)
             || str.at(1).contains(term, Qt::CaseInsensitive) )
        {
            QTreeWidgetItem * item = new QTreeWidgetItem( d->functions, str );
            item->setTextAlignment( 0, Qt::AlignLeft | Qt::AlignVCenter );
            item->setTextAlignment( 1, Qt::AlignLeft | Qt::AlignVCenter );
        }
    }

    d->functions->resizeColumnToContents( 0 );
    d->functions->resizeColumnToContents( 1 );

    if ( d->functions->topLevelItemCount() > 0 ) {
        d->noMatchLabel->hide();
        d->functions->sortItems( 0, Qt::AscendingOrder );
    } else {
        d->noMatchLabel->setGeometry( d->functions->geometry() );
        d->noMatchLabel->show();
        d->noMatchLabel->raise();
    }

  d->searchFilter->setFocus();
  setUpdatesEnabled( true );
}

void FunctionsWidget::retranslateText()
{
    QStringList titles;
    titles << tr( "Name" );
    titles << tr( "Description" );
    d->functions->setHeaderLabels( titles );

    d->searchLabel->setText( tr("Search") );
    d->noMatchLabel->setText( tr("No match found") );

    fillTable();
}

QList<QTreeWidgetItem *> FunctionsWidget::selectedItems() const
{
    return d->functions->selectedItems();
}

QTreeWidgetItem * FunctionsWidget::currentItem() const
{
    return d->functions->currentItem();
}

void FunctionsWidget::catchItemActivated( QTreeWidgetItem * item, int column )
{
    emit itemActivated( item, column );
}

void FunctionsWidget::catchItemDoubleClicked( QTreeWidgetItem * item, int column )
{
    emit itemDoubleClicked( item, column );
}

void FunctionsWidget::clearSelection( QTreeWidgetItem * item )
{
    d->functions->clearSelection();
    emit itemActivated( item, 0 );
}

void FunctionsWidget::triggerFilter()
{
    d->filterTimer->stop();
    d->filterTimer->start();
}

