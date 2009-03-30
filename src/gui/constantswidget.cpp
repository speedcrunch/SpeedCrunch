// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2007-2009 Helder Correia <helder.pereira.correia@gmail.com>
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

#include "gui/constantswidget.hxx"

#include "3rdparty/flickcharm/flickcharm.h"
#include "core/constants.hxx"
#include "core/settings.hxx"

#include <QtCore/QEvent>
#include <QtCore/QTimer>
#include <QtGui/QComboBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>

#include <algorithm>

struct ConstantsWidget::Private
{
    QComboBox * category;
    QLabel * categoryLabel;
    QLineEdit * filter;
    QTimer * filterTimer;
    FlickCharm flickCharm;
    QLabel * label;
    QTreeWidget * list;
    QLabel * noMatchLabel;
};

ConstantsWidget::ConstantsWidget( QWidget * parent )
  : QWidget( parent ), d( new ConstantsWidget::Private )
{
    d->categoryLabel = new QLabel( this );
    d->category = new QComboBox( this );
    d->category->setEditable( false );
    d->category->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );

    connect( d->category, SIGNAL(activated(int)), SLOT(filter()) );

    QWidget * categoryBox = new QWidget( this );
    QHBoxLayout * categoryLayout = new QHBoxLayout;
    categoryBox->setLayout( categoryLayout );
    categoryLayout->addWidget( d->categoryLabel );
    categoryLayout->addWidget( d->category );
    categoryLayout->setMargin( 0 );

    d->label = new QLabel( this );

    d->filter = new QLineEdit( this );
    d->filter->setMinimumWidth( fontMetrics().width('X') * 10 );

    connect( d->filter, SIGNAL(textChanged(const QString &)), SLOT(triggerFilter()) );

    QWidget * searchBox = new QWidget( this );
    QHBoxLayout * searchLayout = new QHBoxLayout;
    searchBox->setLayout( searchLayout );
    searchLayout->addWidget( d->label );
    searchLayout->addWidget( d->filter );
    searchLayout->setMargin( 0 );

    d->list = new QTreeWidget( this );
    d->list->setAutoScroll( true );
    d->list->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    d->list->setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );
    d->list->setVerticalScrollMode( QAbstractItemView::ScrollPerPixel );
    d->list->setColumnCount( 3 );
    d->list->setRootIsDecorated( false );
    d->list->setMouseTracking( true );
    d->list->setEditTriggers( QTreeWidget::NoEditTriggers );
    d->list->setSelectionBehavior( QTreeWidget::SelectRows );
    d->list->setAlternatingRowColors( true );
    d->list->setCursor( QCursor(Qt::PointingHandCursor) );
    d->flickCharm.activateOn( d->list );

    connect( d->list, SIGNAL(itemActivated(QTreeWidgetItem *, int)),
             SLOT(handleItem(QTreeWidgetItem *)) );

    QVBoxLayout * layout = new QVBoxLayout;
    setLayout( layout );
    layout->setMargin( 3 );
    layout->addWidget( categoryBox );
    layout->addWidget( searchBox );
    layout->addWidget( d->list );

    d->filterTimer = new QTimer( this );
    d->filterTimer->setInterval( 500 );
    d->filterTimer->setSingleShot( true );
    connect( d->filterTimer, SIGNAL(timeout()), SLOT(filter()) );

    d->noMatchLabel = new QLabel( this );
    d->noMatchLabel->setAlignment( Qt::AlignCenter );
    d->noMatchLabel->adjustSize();
    d->noMatchLabel->hide();

    retranslateText();
}

ConstantsWidget::~ConstantsWidget()
{
    d->filterTimer->stop();
}

void ConstantsWidget::handleRadixCharacterChange()
{
    updateList();
}

void ConstantsWidget::retranslateText()
{
    d->categoryLabel->setText( tr("Category") );
    d->label->setText( tr("Search") );
    d->noMatchLabel->setText( tr("No match found") );

    QStringList titles;
    const QString name = tr( "Name" );
    const QString value = tr( "Value" );
    const QString unit = tr( "Unit" );
    if ( layoutDirection() == Qt::LeftToRight )
        titles << name << value << unit;
    else
        titles << name << unit << value;
    d->list->setHeaderLabels( titles );

    updateList();
}

void ConstantsWidget::filter()
{
    const QList<Constant> & clist = Constants::instance()->list();
    const char radixChar = Settings::instance()->radixCharacter();
    QString term = d->filter->text();

    d->filterTimer->stop();
    setUpdatesEnabled( false );

    QString chosenCategory = d->category->currentText();

    d->list->clear();
    for ( int k = 0; k < clist.count(); k++ ) {
        QStringList str;
        str << clist.at( k ).name;

        QString radCh = (radixChar != '.') ?
            QString( clist.at(k).value ).replace( '.', radixChar )
            : clist.at(k).value;

        if ( layoutDirection() == Qt::RightToLeft ) {
            str << clist.at( k ).unit + QChar( 0x200e ); // Unicode LRM
            str << radCh;
        } else {
            str << radCh;
            str << clist.at( k ).unit;
        }

        str << clist.at( k ).name.toUpper();
        str << QString( "" );

        bool include = (chosenCategory == tr( "All" )) ?
            true : (clist.at( k ).category == chosenCategory);

        if ( ! include )
            continue;

        QTreeWidgetItem * item = 0;
        if ( term.isEmpty() )
            item = new QTreeWidgetItem( d->list, str );
        else
            if ( str.at(0).contains(term, Qt::CaseInsensitive) )
                item = new QTreeWidgetItem( d->list, str );
        if ( item ) {
            QString tip;
            tip += QString( QChar(0x200E) );
            tip += QString( "<b>%1</b><br>%2" ).arg( clist.at(k).name, clist.at(k).value );
            tip += QString( QChar(0x200E) );
            if ( ! clist.at( k ).unit.isEmpty() )
                tip.append( " " ).append( clist.at(k).unit );
            if ( radixChar != '.' )
                tip.replace( '.', radixChar );
            tip += QString( QChar(0x200E) );
            item->setToolTip( 0, tip );
            item->setToolTip( 1, tip );
            item->setToolTip( 2, tip );

            if ( layoutDirection() == Qt::RightToLeft ) {
                item->setTextAlignment( 1, Qt::AlignRight );
                item->setTextAlignment( 2, Qt::AlignLeft );
            } else {
                item->setTextAlignment( 1, Qt::AlignLeft );
                item->setTextAlignment( 2, Qt::AlignLeft );
            }
        }
    }

    d->list->resizeColumnToContents( 0 );
    d->list->resizeColumnToContents( 1 );
    d->list->resizeColumnToContents( 2 );

    if ( d->list->topLevelItemCount() > 0 ) {
        d->noMatchLabel->hide();
        d->list->sortItems( 0, Qt::AscendingOrder );
    } else {
        d->noMatchLabel->setGeometry( d->list->geometry() );
        d->noMatchLabel->show();
        d->noMatchLabel->raise();
    }

    setUpdatesEnabled( true );
}

void ConstantsWidget::handleItem( QTreeWidgetItem * item )
{
    const QList<Constant> & c = Constants::instance()->list();
    emit constantSelected( std::find_if(c.begin(), c.end(),
                           constant_name_is(item->text(0)))->value );
}

void ConstantsWidget::triggerFilter()
{
    d->filterTimer->stop();
    d->filterTimer->start();
}

void ConstantsWidget::updateList()
{
    d->category->clear();
    d->category->addItems( Constants::instance()->categories() );
    d->category->insertItem( 0, tr("All") );
    d->category->setCurrentIndex( 0 );
    filter();
}

void ConstantsWidget::changeEvent( QEvent * e )
{
    if ( e->type() == QEvent::LanguageChange ) {
        Constants::instance()->retranslateText();
        retranslateText();
    }
    else
        QWidget::changeEvent( e );
}

