// This file is part of the SpeedCrunch project
// Copyright (C) 2009 Andreas Scherer <andreas_coder@freenet.de>
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

#include "core/functions.hxx"
#include "core/settings.hxx"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>
#include <QTreeWidget>

FunctionsWidget::FunctionsWidget( bool hideHeaders, QWidget * parent )
  : QWidget( parent ),
    m_functions( new QTreeWidget( this ) ),
    m_searchLabel( new QLabel( this ) ),
    m_searchFilter( new QLineEdit( this ) ),
    m_filterTimer( new QTimer( this ) ),
    m_noMatchLabel( new QLabel( m_functions ) )
{
  connect( m_functions, SIGNAL( itemActivated( QTreeWidgetItem *, int ) ),
          SLOT( catchItemActivated( QTreeWidgetItem *, int ) ) );
  connect( m_functions, SIGNAL( itemDoubleClicked( QTreeWidgetItem *, int ) ),
          SLOT( catchItemDoubleClicked( QTreeWidgetItem *, int ) ) );

  connect( m_searchFilter, SIGNAL( textChanged( const QString& ) ),
          SLOT( triggerFilter() ) );

  m_filterTimer->setInterval( 500 );
  m_filterTimer->setSingleShot( true );
  connect( m_filterTimer, SIGNAL( timeout() ), SLOT( filter() ) );

  m_functions->setAutoScroll( true );
  m_functions->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
  m_functions->setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );
  m_functions->setColumnCount( 2 );
  m_functions->setAlternatingRowColors( true );
  m_functions->setRootIsDecorated( false );
  m_functions->setEditTriggers( QTreeWidget::NoEditTriggers );
  m_functions->setSelectionBehavior( QTreeWidget::SelectRows );

  m_noMatchLabel->setAlignment( Qt::AlignCenter );
  m_noMatchLabel->adjustSize();
  m_noMatchLabel->hide();

  QWidget * searchBox = new QWidget( this );
  QHBoxLayout * searchLayout = new QHBoxLayout;
  searchLayout->addWidget( m_searchLabel );
  searchLayout->addWidget( m_searchFilter );
  searchLayout->setMargin( 0 );
  searchBox->setLayout( searchLayout );

  QVBoxLayout* layout = new QVBoxLayout;
  layout->setMargin( 3 );
  layout->addWidget( searchBox );
  layout->addWidget( m_functions );
  setLayout( layout );

  retranslateText();

  if ( hideHeaders ) m_functions->header()->hide();
}

FunctionsWidget::~FunctionsWidget()
{
  m_filterTimer->stop();
}

void FunctionsWidget::fillTable()
{
  setUpdatesEnabled( false );

  m_filterTimer->stop();

  QString term = m_searchFilter->text();

  m_functions->clear();

  QStringList functionNames = Functions::instance()->functionNames();
  for ( int k = 0; k < functionNames.count(); k++ )
  {
      Function * f = Functions::instance()->function( functionNames.at(k) );
      if ( ! f ) continue;

      QStringList str;
      str << f->name()
          << f->description();

      if ( term.isEmpty()
              || str.at(0).contains( term, Qt::CaseInsensitive )
              || str.at(1).contains( term, Qt::CaseInsensitive ) )
      {
        QTreeWidgetItem * item = new QTreeWidgetItem( m_functions, str );
        item->setTextAlignment( 0, Qt::AlignLeft | Qt::AlignVCenter );
        item->setTextAlignment( 1, Qt::AlignLeft | Qt::AlignVCenter );
      }
  }

  m_functions->resizeColumnToContents( 0 );
  m_functions->resizeColumnToContents( 1 );

  if ( m_functions->topLevelItemCount() > 0 )
  {
    m_noMatchLabel->hide();
    m_functions->sortItems( 0, Qt::AscendingOrder );

    int group = 3;
    if ( m_functions->topLevelItemCount() >= 2 * group )
      for ( int i = 0; i < m_functions->topLevelItemCount(); i++ )
      {
        QTreeWidgetItem * item = m_functions->topLevelItem(i);
        QBrush c = (((int)(i / group)) & 1) ? palette().base()
                                            : palette().alternateBase();
        item->setBackground( 0, c );
        item->setBackground( 1, c );
      }
  }
  else
  {
    m_noMatchLabel->setGeometry( m_functions->geometry() );
    m_noMatchLabel->show();
    m_noMatchLabel->raise();
  }

  m_searchFilter->setFocus();

  setUpdatesEnabled( true );
}

void FunctionsWidget::retranslateText()
{
  QStringList titles;
  titles << tr( "Name"  );
  titles << tr( "Description" );
  m_functions->setHeaderLabels( titles );

  m_searchLabel->setText( tr( "Search" ) );
  m_noMatchLabel->setText( tr( "No match found" ) );

  filter();
}

QList< QTreeWidgetItem * > FunctionsWidget::selectedItems() const
{
  return m_functions->selectedItems();
}

QTreeWidgetItem * FunctionsWidget::currentItem() const
{
  return m_functions->currentItem();
}

// public slot
void FunctionsWidget::filter()
{
  fillTable();
}

// protected slots
//
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
  m_functions->clearSelection();
  emit itemActivated( item, 0 );
}

void FunctionsWidget::triggerFilter()
{
  m_filterTimer->stop();
  m_filterTimer->start();
}

