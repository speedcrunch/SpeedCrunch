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

#include "gui/variableswidget.hxx"

#include "core/settings.hxx"
#include "core/evaluator.hxx"

#include <QLabel>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QTimer>
#include <QTreeWidget>

VariablesWidget::VariablesWidget( Evaluator * eval, bool hideHeaders, QWidget * parent )
  : QWidget( parent ),
    m_variables( new QTreeWidget( this ) ),
    m_searchLabel( new QLabel( this ) ),
    m_searchFilter( new QLineEdit( this ) ),
    m_filterTimer( new QTimer( this ) ),
    m_noMatchLabel( new QLabel( m_variables ) ),
    m_evaluator( eval )
{
  connect( m_variables, SIGNAL( itemActivated( QTreeWidgetItem *, int ) ),
          SLOT( catchItemActivated( QTreeWidgetItem *, int ) ) );
  connect( m_variables, SIGNAL( itemDoubleClicked( QTreeWidgetItem *, int ) ),
          SLOT( catchItemDoubleClicked( QTreeWidgetItem *, int ) ) );

  connect( m_searchFilter, SIGNAL( textChanged( const QString& ) ),
          SLOT( triggerFilter() ) );

  m_filterTimer->setInterval( 500 );
  m_filterTimer->setSingleShot( true );
  connect( m_filterTimer, SIGNAL( timeout() ), SLOT( filter() ) );

  m_variables->setAutoScroll( true );
  m_variables->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
  m_variables->setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );
  m_variables->setColumnCount( 2 );
  m_variables->setAlternatingRowColors( true );
  m_variables->setRootIsDecorated( false );
  m_variables->setEditTriggers( QTreeWidget::NoEditTriggers );
  m_variables->setSelectionBehavior( QTreeWidget::SelectRows );

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
  layout->addWidget( m_variables );
  setLayout( layout );

  retranslateText();

  if ( hideHeaders ) m_variables->header()->hide();
}

VariablesWidget::~VariablesWidget()
{
  m_filterTimer->stop();
}

QString VariablesWidget::formatValue( const HNumber & value )
{
  char * str = HMath::format( value, 'g' );
  QString s = QString::fromLatin1( str );
  if ( Settings::instance()->radixCharacter() != '.' )
    s.replace( '.', Settings::instance()->radixCharacter() );
  free( str );
  return s;
}

void VariablesWidget::fillTable( bool insertAll )
{
  setUpdatesEnabled( false );

  m_filterTimer->stop();

  QString term = m_searchFilter->text();

  m_variables->clear();

  QVector<Variable> variables = m_evaluator->variables();
  for ( int k = 0; k < variables.count(); k++ )
  {
      if ( ! insertAll &&
               ( variables.at(k).name.toUpper() == "ANS"
              || variables.at(k).name.toUpper() == "PHI"
              || variables.at(k).name.toUpper() == "PI" ) )
        continue;

      QStringList str;
      str << variables.at(k).name
          << formatValue( variables.at(k).value );

      if ( term.isEmpty()
              || str.at(0).contains( term, Qt::CaseInsensitive )
              || str.at(1).contains( term, Qt::CaseInsensitive ) )
      {
        QTreeWidgetItem * item = new QTreeWidgetItem( m_variables, str );
        item->setTextAlignment( 0, Qt::AlignLeft | Qt::AlignVCenter );
        item->setTextAlignment( 1, Qt::AlignLeft | Qt::AlignVCenter );
      }
  }

  m_variables->resizeColumnToContents( 0 );
  m_variables->resizeColumnToContents( 1 );

  if ( m_variables->topLevelItemCount() > 0 || ! insertAll )
  {
    m_noMatchLabel->hide();
    m_variables->sortItems( 0, Qt::AscendingOrder );

    int group = 3;
    if ( m_variables->topLevelItemCount() >= 2 * group )
      for ( int i = 0; i < m_variables->topLevelItemCount(); i++ )
      {
        QTreeWidgetItem * item = m_variables->topLevelItem(i);
        QBrush c = (((int)(i / group)) & 1) ? palette().base()
                                            : palette().alternateBase();
        item->setBackground( 0, c );
        item->setBackground( 1, c );
      }
  }
  else
  {
    m_noMatchLabel->setGeometry( m_variables->geometry() );
    m_noMatchLabel->show();
    m_noMatchLabel->raise();
  }

  setUpdatesEnabled( true );
}

void VariablesWidget::retranslateText()
{
  QStringList titles;
  titles << tr( "Name"  );
  titles << tr( "Value" );
  m_variables->setHeaderLabels( titles );

  m_searchLabel->setText( tr( "Search" ) );
  m_noMatchLabel->setText( tr( "No match found" ) );

  filter();
}

QList< QTreeWidgetItem * > VariablesWidget::selectedItems() const
{
  return m_variables->selectedItems();
}

QTreeWidgetItem * VariablesWidget::currentItem() const
{
  return m_variables->currentItem();
}

// public slot
void VariablesWidget::filter()
{
  fillTable();
}

// protected slots
//
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
  m_variables->clearSelection();
  emit itemActivated( item, 0 );
}

void VariablesWidget::triggerFilter()
{
  m_filterTimer->stop();
  m_filterTimer->start();
}

