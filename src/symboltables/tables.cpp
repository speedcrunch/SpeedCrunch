/* tables.cpp: container objects for symbols
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

#include "symboltables/tables.hxx"

struct CSyntaxSymbol
{
  const char* key;
  int tokenValue;
} CSyntaxSymbols[] =
{
  { " .", '.' },
  { " =", '=' },
  { " ;", ';' },
};
static const int cnt1 = sizeof(CSyntaxSymbols)/sizeof(struct CSyntaxSymbol);

struct CParSymbol
{
  const char* key;
  const char* closeKey;
  int tokenValue;
} CParSymbols[] =
{
  { " (", " )", '(' },
  { " \"", " \"", '"' },
};
static const int cnt2 = sizeof(CParSymbols)/sizeof(struct CParSymbol);

Tables* Tables::tables = 0;

Tables& Tables::self()
{
  if ( !tables )
  {
    tables = new Tables;
    tables->init();
  }
  return *tables;
}

Tables::Tables()
{
  Table builtin, close, global;

  tableList.append(close);
  tableList.append(builtin);
  tableList.append(global);
}

void Tables::init()
{
  for (int i = -1; ++i < cnt1; )
  {
    struct CSyntaxSymbol* ps = CSyntaxSymbols + i;
    builtinTable().insert(ps->key, new SyntaxSymbol(ps->tokenValue));
  }
  for (int i = -1; ++i < cnt2; )
  {
    struct CParSymbol* ps = CParSymbols + i;
    builtinTable().insert(ps->key, new OpenSymbol(ps->tokenValue, ps->closeKey));
  }
}

PSymbol Tables::doLookup(const QString& key, bool exact, int tblindex)
{
  PSymbol result = 0;
  int matchsize = 0;
  int itemsize;
  if ( tblindex < 0 )
    // default: search all tables
    tblindex = tableList.size() - 1;
  Table::const_iterator item;
  for (; tblindex >= 0 && matchsize < key.size(); --tblindex)
  {
    const Table& tbl = tableList.at(tblindex);
    item = tbl.lookup(key, exact);
    if ( item != tbl.constEnd() 
         && (itemsize = item.key().size()) > matchsize )
    {
      result = item.value();
      matchsize = itemsize;
    }
  }
  return result;
}

PSymbol Tables::builtinLookup(const QString& key, bool exact)
{
  return self().doLookup(' ' + key, exact, builtinSymbols);
}

PSymbol Tables::lookup(const QString& key, bool exact)
{
  return self().doLookup(key, exact);
}

void Tables::addCloseSymbol(const QString& key, PSymbol symbol)
{
  closeTable().insertMulti(key, symbol);
}

void Tables::removeCloseSymbol(PSymbol symbol)
{
  QString key = closeTable().key(symbol);
  Table::iterator i = closeTable().find(key);
  while ( i.value() != symbol )
    ++i;
  closeTable().erase(i);
  delete symbol;
}

Table::~Table()
{
  clear();
}

void Table::clear()
{
  Table::const_iterator i = constBegin();
  for (; i != constEnd(); ++i)
    delete i.value();
  QMap<QString, PSymbol>::clear();
}

Table::const_iterator Table::lookup(const QString& key, bool exact) const
{
  if ( exact )
    return constFind(key);
  int resultsz = 0;
  Table::const_iterator result = constEnd();
  Table::const_iterator item = lowerBound(key.left(1));
  for (; item != constEnd() && key.startsWith(item.key()); ++item)
  {
    if ( key == item.key() )
      return item;
    result = item;
    resultsz = result.key().size();
  }
  return result;
}
