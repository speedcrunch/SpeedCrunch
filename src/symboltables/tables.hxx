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

#include "symboltables/symbols.hxx"
#include <QMap>
#include <QString>
#include <QList>
#include <QStringList>

class Table: private QMap<QString, PSymbol>
{
  friend class Tables;
  public:
    ~Table();
  private:
    void clear();
    Table::const_iterator lookup(const QString& key, bool exact = true) const;
};

class Tables
{
  public:
    static PSymbol builtinLookup(const QString& key, bool exact = true);
    static PSymbol lookup(const QString& key, bool exact = true);
    static void addCloseSymbol(const QString& key, PSymbol symbol);
    static void removeCloseSymbol(PSymbol symbol);
    static Variant escape(const ParamList& params);
  private:

    enum
    {
      closeSymbols,
      builtinSymbols,
      globalSymbols,
    };

    static Tables& self();
    static Table& closeTable() { return self().tableList[closeSymbols]; };
    static Table& builtinTable() { return self().tableList[builtinSymbols]; };
    PSymbol doLookup(const QString& key, bool exact, int firstTable = -1);
    QList<Table> tableList;
    static Tables* tables;
    Tables( const Tables& );
    Tables& operator=( const Tables& );
    Tables();
    void init();
};

#endif /* _TABLES_H */
