// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2008, 2009, 2010, 2011 Helder Correia <helder.pereira.correia@gmail.com>
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

#ifndef GUI_CONSTANTSWIDGET_H
#define GUI_CONSTANTSWIDGET_H

#include <QtGui/QWidget>

class QComboBox;
class QLabel;
class QLineEdit;
class QTreeWidget;
class QTreeWidgetItem;

class ConstantsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConstantsWidget(QWidget *parent = 0);
    ~ConstantsWidget();

signals:
    void constantSelected(const QString &);

public slots:
    void handleRadixCharacterChange();

protected slots:
    void filter();
    void handleItem(QTreeWidgetItem *);
    void retranslateText();
    void triggerFilter();
    void updateList();

protected:
    virtual void changeEvent(QEvent *);

private:
    Q_DISABLE_COPY(ConstantsWidget)

    QComboBox *m_category;
    QLabel *m_categoryLabel;
    QLineEdit *m_filter;
    QTimer *m_filterTimer;
    QLabel *m_label;
    QTreeWidget *m_list;
    QLabel *m_noMatchLabel;
};

#endif

