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

#ifndef GUI_FUNCTIONSWIDGET_HXX
#define GUI_FUNCTIONSWIDGET_HXX

#include <QWidget>

class QLabel;
class QLineEdit;
class QString;
class QTimer;
class QTreeWidget;
class QTreeWidgetItem;

template<class T>
class QList;

class HNumber;

class FunctionsWidget : public QWidget
{
  Q_OBJECT

  QTreeWidget * m_functions;
  QLabel      * m_searchLabel;
  QLineEdit   * m_searchFilter;
  QTimer      * m_filterTimer;
  QLabel      * m_noMatchLabel;

  bool m_insertAllItems;

public:
  FunctionsWidget( bool hideHeaders, QWidget * parent = 0 );
  ~FunctionsWidget();

  void fillTable();
  QTreeWidgetItem* currentItem() const;
  QList< QTreeWidgetItem* > selectedItems() const;

  void retranslateText();

signals:
  void itemActivated( QTreeWidgetItem *, int );
  void itemDoubleClicked( QTreeWidgetItem *, int );

public slots:
  void filter();

protected slots:
  void catchItemActivated( QTreeWidgetItem *, int );
  void catchItemDoubleClicked( QTreeWidgetItem *, int );
  void clearSelection( QTreeWidgetItem * );
  void triggerFilter();
};

#endif

