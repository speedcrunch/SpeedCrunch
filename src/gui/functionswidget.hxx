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

#ifndef GUI_FUNCTIONSWIDGET_HXX
#define GUI_FUNCTIONSWIDGET_HXX

#include <QtGui/QWidget>

#include <memory>

class QTreeWidgetItem;

class FunctionsWidget : public QWidget
{
    Q_OBJECT

public:
    FunctionsWidget( bool hideHeaders, QWidget * parent = 0 );
    ~FunctionsWidget();

    QTreeWidgetItem * currentItem() const;
    void fillTable();
    void retranslateText();
    QList<QTreeWidgetItem *> selectedItems() const;

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

private:
    struct Private;
    const std::auto_ptr<Private> d;

    FunctionsWidget( const FunctionsWidget & );
    FunctionsWidget & operator=( const FunctionsWidget & );
};

#endif

