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

#ifndef GUI_USERFUNCTIONLISTWIDGET_H
#define GUI_USERFUNCTIONLISTWIDGET_H

#include <QtCore/QList>
#include <QWidget>

class QEvent;
class QKeyEvent;
class QLabel;
class QLineEdit;
class QTimer;
class QTreeWidget;
class QTreeWidgetItem;

class UserFunctionListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UserFunctionListWidget(QWidget* parent = 0);
    ~UserFunctionListWidget();

    QTreeWidgetItem* currentItem() const;
    QString getUserFunctionName(const QTreeWidgetItem *);

signals:
    void itemActivated(const QString&);
    void itemDeleted(const QString&);

public slots:
    void fillTable();
    void retranslateText();

protected slots:
    void activateItem();
    void deleteItem();
    void deleteAllItems();
    void triggerFilter();

protected:
    void changeEvent(QEvent*);
    void keyPressEvent(QKeyEvent*);

private:
    Q_DISABLE_COPY(UserFunctionListWidget)

    QTimer* m_filterTimer;
    QTreeWidget* m_userFunctions;
    QAction* m_insertAction;
    QAction* m_deleteAction;
    QAction* m_deleteAllAction;
    QLabel* m_noMatchLabel;
    QLineEdit* m_searchFilter;
    QLabel* m_searchLabel;
};

#endif
