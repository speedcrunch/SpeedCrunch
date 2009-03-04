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

#include "gui/variableswidget.hxx"

#include "3rdparty/flickcharm/flickcharm.h"
#include "core/evaluator.hxx"
#include "core/settings.hxx"
#include "math/hmath.hxx"

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QTimer>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>

struct VariablesWidget::Private
{
    QTimer * filterTimer;
    FlickCharm flickCharm;
    bool insertAllItems;
    QLabel * noMatchLabel;
    QLabel * searchLabel;
    QLineEdit * searchFilter;
    QTreeWidget * variables;

    static QString formatValue( const HNumber & value );
};

QString VariablesWidget::Private::formatValue( const HNumber & value )
{
    char * str = HMath::format( value, 'g' );
    QString s = QString::fromLatin1( str );
    if ( Settings::instance()->radixCharacter() != '.' )
        s.replace( '.', Settings::instance()->radixCharacter() );
    free( str );
    return s;
}

VariablesWidget::VariablesWidget( bool hideHeaders, bool insertAllItems, QWidget * parent )
    : QWidget( parent ), d( new VariablesWidget::Private )
{
    d->filterTimer = new QTimer( this );
    d->insertAllItems = insertAllItems;
    d->variables = new QTreeWidget( this );
    d->noMatchLabel = new QLabel( d->variables );
    d->flickCharm.activateOn( d->variables );
    d->searchFilter = new QLineEdit( this );
    d->searchLabel = new QLabel( this );

    d->filterTimer->setInterval( 500 );
    d->filterTimer->setSingleShot( true );

    d->variables->setAlternatingRowColors( true );
    d->variables->setAutoScroll( true );
    d->variables->setColumnCount( 2 );
    d->variables->setEditTriggers( QTreeWidget::NoEditTriggers );
    d->variables->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    d->variables->setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );
    d->variables->setRootIsDecorated( false );
    d->variables->setSelectionBehavior( QTreeWidget::SelectRows );
    d->variables->setCursor( QCursor(Qt::PointingHandCursor) );

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
    layout->addWidget( d->variables );
    setLayout( layout );

    retranslateText();

    if ( hideHeaders )
        d->variables->header()->hide();

    connect( d->filterTimer, SIGNAL(timeout()), SLOT(filter()) );
    connect( d->searchFilter, SIGNAL(textChanged(const QString &)),
             SLOT(triggerFilter()) );
    connect( d->variables, SIGNAL(itemActivated(QTreeWidgetItem *, int)),
             SLOT( catchItemActivated( QTreeWidgetItem *, int ) ) );
    connect( d->variables, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
             SLOT(catchItemDoubleClicked(QTreeWidgetItem *, int)) );
}

VariablesWidget::~VariablesWidget()
{
    d->filterTimer->stop();
}

void VariablesWidget::fillTable()
{
    setUpdatesEnabled( false );

    d->filterTimer->stop();
    d->variables->clear();
    QString term = d->searchFilter->text();
    QVector<Variable> variables = Evaluator::instance()->variables();

    for ( int k = 0; k < variables.count(); k++ )
    {
        if ( ! d->insertAllItems
             && (variables.at(k).name.toUpper() == "ANS"
                 || variables.at(k).name.toUpper() == "PHI"
                 || variables.at(k).name.toUpper() == "PI") )
            continue;

        QStringList str;
        str << variables.at(k).name
            << VariablesWidget::Private::formatValue( variables.at(k).value );

        if ( term.isEmpty()
             || str.at(0).contains(term, Qt::CaseInsensitive)
             || str.at(1).contains(term, Qt::CaseInsensitive) )
        {
            QTreeWidgetItem * item = new QTreeWidgetItem( d->variables, str );
            item->setTextAlignment( 0, Qt::AlignLeft | Qt::AlignVCenter );
            item->setTextAlignment( 1, Qt::AlignLeft | Qt::AlignVCenter );
        }
    }

    d->variables->resizeColumnToContents( 0 );
    d->variables->resizeColumnToContents( 1 );

    if ( d->variables->topLevelItemCount() > 0 || ! d->insertAllItems ) {
        d->noMatchLabel->hide();
        d->variables->sortItems( 0, Qt::AscendingOrder );
    } else {
        d->noMatchLabel->setGeometry( d->variables->geometry() );
        d->noMatchLabel->show();
        d->noMatchLabel->raise();
    }

    d->searchFilter->setFocus();
    setUpdatesEnabled( true );
}

void VariablesWidget::retranslateText()
{
    QStringList titles;
    titles << tr( "Name"  );
    titles << tr( "Value" );
    d->variables->setHeaderLabels( titles );

    d->searchLabel->setText( tr("Search") );
    d->noMatchLabel->setText( tr("No match found") );

    filter();
}

QList<QTreeWidgetItem *> VariablesWidget::selectedItems() const
{
    return d->variables->selectedItems();
}

QTreeWidgetItem * VariablesWidget::currentItem() const
{
    return d->variables->currentItem();
}

void VariablesWidget::filter()
{
    fillTable();
}

void VariablesWidget::catchItemActivated( QTreeWidgetItem * item, int column )
{
    emit itemActivated( item, column );
}

void VariablesWidget::catchItemDoubleClicked( QTreeWidgetItem * item, int column )
{
    emit itemDoubleClicked( item, column );
}

void VariablesWidget::clearSelection( QTreeWidgetItem * item )
{
    d->variables->clearSelection();
    emit itemActivated( item, 0 );
}

void VariablesWidget::triggerFilter()
{
    d->filterTimer->stop();
    d->filterTimer->start();
}

