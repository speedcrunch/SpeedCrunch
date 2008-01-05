/* tables.cpp: container objects for symbols
   Copyright (C) 2007, 2008 Wolf Lammen ookami1 <at> gmx <dot> de

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
#include "math/hmath.hxx"

struct CSyntaxSymbol
{
  const char* key;
  SymType symtype;
} CSyntaxSymbols[] =
{
  { " .", dot },
  { " =", assign },
  { " ;", separator },
  { " posscale", signPlus },
  { " negscale", signMinus },
};
static const int cnt1 = sizeof(CSyntaxSymbols)/sizeof(struct CSyntaxSymbol);

struct CParSymbol
{
  const char* key;
  const char* closeKey;
  SymType symtype;
} CParSymbols[] =
{
  { " (", " )", openPar },
  { " \"", " \"", quote },
  { " '", " ", quote },
};
static const int cnt2 = sizeof(CParSymbols)/sizeof(struct CParSymbol);

struct CVFct1Symbol
{
  const char* key;
  Vfct fct;
  VariantType paramtype;
} CVFct1Symbols[] =
{
  { " escape", Tables::escape, TText },
};
static const int cnt3 = sizeof(CVFct1Symbols)/sizeof(struct CVFct1Symbol);

struct CVFctSymbol
{
  const char* key;
  Vfct fct;
  int mincount;
  int maxcount;
} CVFctSymbols[] =
{
  { " def", Tables::define, 2, 3 },
};
static const int cnt4 = sizeof(CVFctSymbols)/sizeof(struct CVFctSymbol);

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
  FctList fcts;
  for (int i = -1; ++i < cnt1; )
  {
    struct CSyntaxSymbol* ps = CSyntaxSymbols + i;
    builtinTable().addSyntaxSymbol(ps->key, ps->symtype);
  }
  for (int i = -1; ++i < cnt2; )
  {
    struct CParSymbol* ps = CParSymbols + i;
    builtinTable().addOpenSymbol(ps->key, ps->symtype, ps->closeKey);
  }
  fcts.clear();
  for (int i = -1; ++i < cnt3; )
  {
    struct CVFct1Symbol* ps = CVFct1Symbols + i;
    TypeList paramType;
    paramType.appendType(ps->paramtype);
    fcts.vfct = ps->fct;
    builtinTable().addFunctionSymbol(ps->key, paramType, fcts, 1, 1);
  }
  fcts.clear();
  for (int i = -1; ++i < cnt4; )
  {
    struct CVFctSymbol* ps = CVFctSymbols + i;
    TypeList paramType;
    fcts.vfct = ps->fct;
    builtinTable().addFunctionSymbol(ps->key, paramType, fcts,
                                     ps->mincount, ps->maxcount);
  }
}

SearchResult Tables::doLookup(const QString& key, bool exact, int index,
                              int lastindex)
{
  SearchResult result;
  result.symbol = 0;
  result.keyused = 0;
  int itemsize;
  if ( index < 0 )
    // default: search all tables
    index = tableList.size() - 1;
  if (lastindex < 0)
    // a negative lastindex is a relative offset to index
    lastindex = index + lastindex + 1;
  Table::const_iterator item;
  for (; index >= lastindex && result.keyused < key.size(); --index)
  {
    const Table& tbl = tableList.at(index);
    item = tbl.lookup(key, exact);
    if ( item != tbl.constEnd() 
         && (itemsize = item.key().size()) > result.keyused )
    {
      result.symbol = item.value();
      result.keyused = itemsize;
    }
  }
  return result;
}

SearchResult Tables::builtinLookup(const QString& key, bool exact)
{
  SearchResult r = self().doLookup(' ' + key, exact, builtinSymbols, -1);
  if (r.keyused > 0)
    --r.keyused;
  return r;
}

SearchResult Tables::lookup(const QString& key, bool exact)
{
  return self().doLookup(key, exact);
}

void Tables::addCloseSymbol(const QString& key, PSymbol symbol)
{
  closeTable().insertMulti(key, symbol);
}

void Tables::removeCloseSymbol(PSymbol symbol)
{
  closeTable().removeSymbol(symbol);
}

Variant Tables::escape(const ParamList& params)
{
  Settings::self()->escape = (const QString&)params.at(0);
  return 0;
}

Variant Tables::define(const ParamList& params)
{
  TypeList t;
  t.appendType(TText, 2);
  if (!t.match(params))
    return SYMBOLS_INVALID_PARAMTYPE;
  PSymbol symbol = builtinLookup(params.at(0)).symbol;
  if (!symbol)
    return TABLE_SYMBOL_NOT_FOUND;
  QString key = (const QString&)params.at(1);
  if (globalTable().contains(key))
    return TABLE_KEY_EXISTS;
  bool ok;
  if (dynamic_cast<OpenSymbol*>(symbol))
  {
    if (params.size() != 3)
      return TABLE_MISSING_CLOSE;
    if (params.at(2).type() != TText)
      return SYMBOLS_INVALID_PARAMTYPE;
    ok = globalTable().addOpenSymbol(key, symbol->type(), params.at(2));
  }
  else
    ok = globalTable().cloneSymbol(key, symbol);
  return ok? 0 : SYMBOLS_CLONE_ERROR;
}

Table::~Table()
{
  clear();
}

void Table::checkDelete(PSymbol symbol)
{
  if (symbol->owner() == this)
    delete symbol;
}

bool Table::addSymbol(const QString& key, PSymbol symbol)
{
  if(!symbol)
    return false;
  if (contains(key))
  {
    checkDelete(symbol);
    return false;
  }
  insert(key, symbol);
  return true;
}

bool Table::cloneSymbol(const QString& key, PSymbol symbol)
{
  return addSymbol(key, symbol->clone(this));
}

void Table::removeSymbol(PSymbol symbol)
{
  iterator i = find(key(symbol));
  while ( i.value() != symbol )
    ++i;
  erase(i);
  checkDelete(symbol);
}

bool Table::addSyntaxSymbol(const QString& key, SymType aType)
{
  return addSymbol(key, new SyntaxSymbol(this, aType));
}

bool Table::addOpenSymbol(const QString& key, SymType aType, const QString& close)
{
  return addSymbol(key, new OpenSymbol(this, aType, close));
}

bool Table::addFunctionSymbol(const QString& key, const TypeList& t,
               const FctList& f, int minParamCount, int maxParamCount)
{
  return addSymbol(key, new FunctionSymbol(this, t, f, minParamCount, maxParamCount));
}

void Table::clear()
{
  Table::const_iterator i = constBegin();
  for (; i != constEnd(); ++i)
    checkDelete(i.value());
  QMap<QString, PSymbol>::clear();
}

Table::const_iterator Table::lookup(const QString& key, bool exact) const
{
  if ( exact )
    return constFind(key);
  if ( size() != 0 )
  {
    Table::const_iterator item = lowerBound(key);
    if ( item != constEnd() && item.key() == key )
      return item;
    --item;
    if ( key.startsWith(item.key()) )
      return item;
  }
  return constEnd();
}
