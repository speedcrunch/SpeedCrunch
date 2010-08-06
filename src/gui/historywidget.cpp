// This file is part of the SpeedCrunch project
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

#include "gui/historywidget.h"

#include "thirdparty/flickcharm/flickcharm.h"

#include <QtCore/QEvent>
#include <QtGui/QListWidget>
#include <QtGui/QVBoxLayout>

struct HistoryWidget::Private
{
    FlickCharm flickCharm;
    QListWidget * list;
};

HistoryWidget::HistoryWidget( QWidget * parent )
    : QWidget( parent ), d( new HistoryWidget::Private )
{
    d->list = new QListWidget( this );
    d->list->setAlternatingRowColors( true );
    d->list->setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );
    d->list->setVerticalScrollMode( QAbstractItemView::ScrollPerPixel );
    d->list->setCursor( QCursor(Qt::PointingHandCursor) );
    d->flickCharm.activateOn( d->list );

    QVBoxLayout * layout = new QVBoxLayout;
    layout->setMargin( 3 );
    layout->addWidget( d->list );
    setLayout( layout );

    connect( d->list, SIGNAL(itemActivated(QListWidgetItem *)),
             SLOT(handleItem(QListWidgetItem *)) );
}

HistoryWidget::~HistoryWidget()
{
}

void HistoryWidget::clear()
{
    d->list->clear();
}

void HistoryWidget::append( const QString & h )
{
    d->list->addItem( h );
    d->list->clearSelection();
    d->list->scrollToBottom();
}

void HistoryWidget::appendHistory( const QStringList & h )
{
    d->list->insertItems( 0, h );
    d->list->setCurrentRow( h.count() - 1 );
    d->list->scrollToItem( d->list->item( h.count() ), QListWidget::PositionAtTop );
    d->list->clearSelection();
}

void HistoryWidget::setHistory( const QStringList & h )
{
    d->list->clear();
    appendHistory( h );
}

void HistoryWidget::handleItem( QListWidgetItem * item )
{
    d->list->clearSelection();
    emit expressionSelected( item->text() );
}

void HistoryWidget::changeEvent( QEvent * e )
{
    if ( e->type() == QEvent::LanguageChange )
        setLayoutDirection( Qt::LeftToRight );
    else
        QWidget::changeEvent( e );
}

