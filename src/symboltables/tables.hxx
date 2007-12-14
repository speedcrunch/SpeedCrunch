/* tables.hxx containers for symbols
   Copyright (C) 2007 Wolf Lammen ookami1 <at> gmx <dot> de

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

#ifndef _TABLES_H
#define _TABLES_H

#include <QString>
#include <QMap>
#include "symboltables/symbols.hxx"
#include "symboltables/help.h"

class Table: protected QMap<QString, Symbol*>
{
  public:
    ~Table();
    bool put(QString key, Symbol* symbol);
    bool putAlias(QString key, QString alias);
    bool remove(QString key);
    Symbol* get(QString key);
    void clear();

    // a FI-LO queue 
    static void addTable(Table* newTable);
    static void deleteTable();
    static void init();
    static void exit();
  private:
    Table* next;
    static Table* head;
};

#endif /* _TABLES_H */
