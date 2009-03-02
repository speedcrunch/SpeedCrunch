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

#include "gui/variabletable.hxx"

#include "core/settings.hxx"
#include "core/evaluator.hxx"
#include "gui/mainwindow.hxx"

#include <QLabel>
#include <QHeaderView>

VariableTable::VariableTable( Evaluator * eval, bool hideHeaders, QWidget * parent )
  : QTreeWidget( parent ),
    m_noMatchLabel( new QLabel( tr( "No match found" ), this ) ),
    m_evaluator( eval )
{
  m_noMatchLabel->setAlignment( Qt::AlignCenter );
  m_noMatchLabel->adjustSize();
  m_noMatchLabel->hide();

  setAutoScroll( true );
  setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
  setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );

  setColumnCount( 2 );
  setAlternatingRowColors( true );
  setRootIsDecorated( false );
  setEditTriggers( QTreeWidget::NoEditTriggers );
  setSelectionBehavior( QTreeWidget::SelectRows );

  QStringList titles;
  titles << tr( "Name"  );
  titles << tr( "Value" );
  setHeaderLabels( titles );
  if ( hideHeaders ) header()->hide();
}

VariableTable::~VariableTable()
{
}

QString VariableTable::formatValue( const HNumber & value )
{
  char * str = HMath::format( value, 'g' );
  QString s = QString::fromLatin1( str );
  if ( Settings::instance()->radixCharacter() != '.' )
    s.replace( '.', Settings::instance()->radixCharacter() );
  free( str );
  return s;
}

void VariableTable::fillTable( QString term, bool insertAll )
{
  setUpdatesEnabled( false );

  clear();

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
        QTreeWidgetItem * item = new QTreeWidgetItem( this, str );
        item->setTextAlignment( 0, Qt::AlignLeft | Qt::AlignVCenter );
        item->setTextAlignment( 1, Qt::AlignLeft | Qt::AlignVCenter );
      }
  }

  resizeColumnToContents( 0 );
  resizeColumnToContents( 1 );

  if ( topLevelItemCount() > 0 )
  {
    m_noMatchLabel->hide();
    sortItems( 0, Qt::AscendingOrder );

    int group = 3;
    if ( topLevelItemCount() >= 2 * group )
      for ( int i = 0; i < topLevelItemCount(); i++ )
      {
        QTreeWidgetItem * item = topLevelItem(i);
        QBrush c = (((int)(i / group)) & 1) ? palette().base()
                                            : palette().alternateBase();
        item->setBackground( 0, c );
        item->setBackground( 1, c );
      }
  }
  else
  {
    m_noMatchLabel->setGeometry( geometry() );
    m_noMatchLabel->show();
    m_noMatchLabel->raise();
  }

  setUpdatesEnabled( true );
};

