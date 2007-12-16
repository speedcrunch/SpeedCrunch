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
  int idx;
  const char* value;
  constfunc func;
} initvar[] = {
  { "__pi",     symPi,  "", HMath::pi  },
  { "__phi",    symPhi, "", HMath::phi },
  { "__au",     symAU,  "149597870691" },
};

struct _initsyntax
{
  const char* name;
  int idx;
  signed char parchange;
} initsyntax[] = {
  { "__sep", symSep, 0 },
  { "__open", symOpenPar, 1 },
  { "__close", symClosePar, -1 },
};

struct _initfunc
{
  const char* name;
  int idx;
  int minparam;
  int maxparam;
  FuncList f;
} initfunc[] = {
  { "__sqrt"     , symSqrt,      1, 1, { 0, HMath::sqrt } },
  { "__npr"      , symnPr,       2, 2, { 0, 0, HMath::nPr } },
  { "__hypermean", symHyperMean, 3, 3, { 0, 0, 0, HMath::hypergeometricMean } },
};

struct _initop
{
  const char* name;
  int idx;
  bool postfix;
  Func1 unaryOp;
  Func2 binaryOp;
} initop[] = {
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
  BuiltinTable* builtin;
  HNumber x;
  struct _initfunc funcdesc;
  Func fnct;
  FunctionSymbol* fsymbol;
  OperatorSymbol* osymbol;
  int opflags;

  while(head)
    deleteTable();
  builtin = new BuiltinTable;
  addTable(builtin);
  for (unsigned i = 0; i < sizeof(initsyntax); ++i)
    builtin->put(initsyntax[i].name,
                 new SyntaxSymbol(initsyntax[i].idx, initsyntax[i].parchange));
  for (unsigned i = 0; i < sizeof(initvar); ++i)
  {
    x = initvar[i].value == 0? initvar[i].func() : HNumber(initvar[i].value);
    builtin->put(initvar[i].name, new ConstSymbol(x));
  }
  for (unsigned i = 0; i < sizeof(initfunc); ++i)
  {
    funcdesc = initfunc[i];
    fsymbol = new FunctionSymbol(funcdesc.idx,
                                 funcdesc.minparam,
                                 funcdesc.maxparam);
    for (int i = 0; i < fctinterfaces; ++i)
    {
      switch(i)
      {
        case 0: fnct.f0 = funcdesc.f.f0; break;
        case 1: fnct.f1 = funcdesc.f.f1; break;
        case 2: fnct.f2 = funcdesc.f.f2; break;
        case 3: fnct.f3 = funcdesc.f.f3; break;
        case 4: fnct.f4 = funcdesc.f.f4; break;
        case fctinterfaces - 1: fnct.fl = funcdesc.f.fl; break;
        default: fnct.f0 = 0;
      }
      if (fnct.f0)
        fsymbol->addFunc(i, fnct);
    }
    builtin->put(funcdesc.name, fsymbol);
  }
  for (unsigned i = 0; i < sizeof(initop); ++i)
  {
    opflags = 0;
    if (initop[i].unaryOp)
      opflags += unaryOp;
    if (initop[i].binaryOp)
      opflags += binaryOp;
    osymbol = new OperatorSymbol(initop[i].idx, opflags, initop[i].postfix);
    fnct.f1 = initop[i].unaryOp;
    if ((opflags & unaryOp) != 0)
      osymbol->addFunc(1, fnct);
    fnct.f2 = initop[i].binaryOp;
    if ((opflags & binaryOp) != 0)
      osymbol->addFunc(2, fnct);
    builtin->put(initop[i].name, osymbol);
  }
  builtin->put("__ans", new AnsSymbol);
}

BuiltinTable::BuiltinTable()
  : destructing(false)
{
}

BuiltinTable::~BuiltinTable()
{
  destructing = true;
  clear();
}

void BuiltinTable::clear()
{
  if (destructing)
    Table::clear();
}

bool BuiltinTable::remove(QString key)
{
  if (destructing)
    return Table::remove(key);
  return false;
}
