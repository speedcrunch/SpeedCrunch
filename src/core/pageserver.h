// This file is part of the SpeedCrunch project
// Copyright (C) 2014 Helder Correia <helder.pereira.correia@gmail.com>
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

#ifndef CORE_PAGESERVER_H
#define CORE_PAGESERVER_H

#include <QHash>
#include <QObject>
#include <QString>

class PageServer : public QObject {
public:
    explicit PageServer(QObject* parent = 0) : QObject(parent) { }
    QString getPageContent(const QString& id);
    QString getCurrentPageContent();

protected:
    typedef QString (*PageMaker)();
    void addPage(const QString& id, PageMaker maker) { m_toc[id] = maker; }
    virtual void createPages() = 0;

private:
    Q_DISABLE_COPY(PageServer)
    QHash<QString, PageMaker> m_toc;
    QString m_currentPageID;
};

#endif // CORE_PAGESERVER_H
