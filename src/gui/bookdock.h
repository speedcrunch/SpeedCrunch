/*
  This file is part of the SpeedCrunch project
  Copyright (C) 2007 Petri Damstén <damu@iki.fi>
  Copyright (C) 2008-2013 Helder Correia <helder.pereira.correia@gmail.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#ifndef GUI_BOOKDOCK_H
#define GUI_BOOKDOCK_H

#include <QtGui/QDockWidget>

class QHBoxLayout;
class QPushButton;
class QTextBrowser;
class QUrl;

class BookDock : public QDockWidget {
    Q_OBJECT

public:
    BookDock(const QString& path, const QString& indexPage, QWidget* parent = 0);

signals:
    void expressionSelected(const QString& expression);

public slots:
    void openPage(const QUrl&);
    void retranslateText();

protected slots:
    virtual void changeEvent(QEvent* event);

private slots:
    void handleAnchorClick(const QUrl& link);
private:
    BookDock();
    Q_DISABLE_COPY(BookDock);

    QString m_currentPage;
    QString m_path;
    QTextBrowser* m_browser;
};

#endif // GUI_BOOKDOCK_H