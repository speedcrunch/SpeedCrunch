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
#include "floatconst.h"

typedef HNumber(*constfunc)();
struct _initvar
{
  const char* name;
  constfunc func;
  int help;
} initvar[] = {
  { "__pi__", HMath::pi, helpPi},
  { "__phi__",HMath::phi, helpPhi},
};

struct _initconst
{
  const char* name;
  const char* value;
  int help;
} initconst[] = {
  { "__au__", "149597870691" },
};

struct _initsyntax
{
  const char* name;
  signed char parchange;
} initsyntax[] = {
  { "__sep__", 0 },
  { "__open__", 1 },
  { "__close__", -1 },
};

Table* Table::head = 0;

Table::~Table()
{
  clear();
}

bool Table::remove(QString key)
{
  if (!contains(key))
    return false;
  delete value(key);
  QMap<QString, Symbol*>::remove(key);
  return true;
}

void Table::clear()
{
  while(!isEmpty())
    remove(begin().key());
}

Symbol* Table::get(QString key)
{
  if (!contains(key))
    return next? next->get(key) : 0;
  return (*this)[key];
}

bool Table::put(QString key, Symbol* symbol)
{
  if (contains(key))
  {
    delete symbol;
    return 0;
  }
  (*this)[key] = symbol;
  return 1;
}

bool Table::putAlias(QString key, QString alias)
{
  Symbol* symbol = next->get(alias);
  QString symkey = symbol->getKey(alias);
  if (!symbol)
    return 0;
  return put(symkey, new AliasSymbol(symkey, symbol));
}

void Table::addTable(Table* newTable)
{
  newTable->next = head;
  head = newTable;
}

void Table::deleteTable()
{
  Table* tmp = head;

  head = tmp->next;
  delete tmp;
}

void Table::init()
{
  Table* builtin;

  while(head)
    deleteTable();
  builtin = new Table;
  addTable(builtin);
  for (unsigned i = 0; i < sizeof(initvar); ++i)
    builtin->put(initvar[i].name, new ConstSymbol(initvar[i].func()));
  for (unsigned i = 0; i < sizeof(initsyntax); ++i)
    builtin->put(initsyntax[i].name, new SyntaxSymbol(initsyntax[i].parchange));
  for (unsigned i = 0; i < sizeof(initconst); ++i)
    builtin->put(initsyntax[i].name, new SyntaxSymbol(initsyntax[i].parchange));
  builtin->put("__ans__", new AnsSymbol);
}
