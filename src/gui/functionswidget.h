// This file is part of the SpeedCrunch project
// Copyright (C) 2009 Andreas Scherer <andreas_coder@freenet.de>
// Copyright (C) 2009, 2011, 2013 Helder Correia <helder.pereira.correia@gmail.com>
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

#ifndef GUI_FUNCTIONSWIDGET_H
#define GUI_FUNCTIONSWIDGET_H

#include <QtCore/QList>
#include <QtGui/QWidget>

class QEvent;
class QLabel;
class QLineEdit;
class QTreeWidget;
class QTreeWidgetItem;

class FunctionsWidget : public QWidget {
    Q_OBJECT

public:
    explicit FunctionsWidget(QWidget* parent = 0);
    ~FunctionsWidget();

    const QTreeWidgetItem* currentItem() const;
    QList<QTreeWidgetItem*> selectedItems() const;

signals:
    void functionSelected(const QString&);

protected slots:
    void handleItemActivated(QTreeWidgetItem*, int);
    virtual void changeEvent(QEvent*);
    void clearSelection(QTreeWidgetItem*);
    void fillTable();
    void retranslateText();
    void triggerFilter();

private:
    Q_DISABLE_COPY(FunctionsWidget);

    QTimer* m_filterTimer;
    QTreeWidget* m_functions;
    bool m_insertAllItems;
    QLabel* m_noMatchLabel;
    QLineEdit* m_searchFilter;
    QLabel* m_searchLabel;
};

#endif // GUI_FUNCTIONSWIDGET_H