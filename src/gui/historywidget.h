// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2008, 2009, 2011 Helder Correia <helder.pereira.correia@gmail.com>
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

#ifndef GUI_HISTORYWIDGET_H
#define GUI_HISTORYWIDGET_H

#include <QWidget>

class QListWidget;
class QListWidgetItem;

class HistoryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HistoryWidget(QWidget *parent = 0);

public slots:
    void append(const QString &);
    void appendHistory(const QStringList &);
    void clear();
    void setHistory(const QStringList &);

signals:
    void expressionSelected(const QString &);

protected slots:
    void handleItem(QListWidgetItem *);

protected:
    void changeEvent(QEvent *);

private:
    Q_DISABLE_COPY(HistoryWidget)

    QListWidget *m_list;
};

#endif

