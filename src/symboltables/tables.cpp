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
  { " ,", group },
  { " p", scale },
  { " e", decscale },
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
};
static const int cnt2 = sizeof(CParSymbols)/sizeof(struct CParSymbol);

struct CVFctSymbol
{
  const char* key;
  Vfct fct;
  const char* paramtypes;
  int mincount;
  int maxcount;
} CVFctSymbols[] =
{
  { " escape", Tables::escape, "t", 1, 1 },
  { " def", Tables::define, "ttt", 2, 3 },
  { " undef", Tables::undefine, "ti", 1, 2 },
  { " overload", Tables::overload, "tt", 2, 2 },
};
static const int cnt3 = sizeof(CVFctSymbols)/sizeof(struct CVFctSymbol);

struct CNFct1Symbol
{
  const char* key;
  Nfct1 fct;
} CNFct1Symbols[] =
{
  { " sin", HMath::sin },
};
static const int cnt4 = sizeof(CNFct1Symbols)/sizeof(struct CNFct1Symbol);

struct CTagSymbol
{
  const char* key;
  char base;
  bool cmpl;
} CTagSymbols[] =
{
  { " x", 16, false },
  { " xs", 16, true },
  { " d", 10, false },
  { " o",  8, false },
  { " os",  8, true },
  { " b",  2, false },
  { " bs",  2, true },
};
static const int cnt5 = sizeof(CTagSymbols)/sizeof(struct CTagSymbol);

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
    struct CVFctSymbol* ps = CVFctSymbols + i;
    TypeList paramType(ps->paramtypes);
    fcts.vfct = ps->fct;
    builtinTable().addFunctionSymbol(ps->key, paramType, fcts,
                                     ps->mincount, ps->maxcount);
  }
  fcts.clear();
  for (int i = -1; ++i < cnt4; )
  {
    struct CNFct1Symbol* ps = CNFct1Symbols + i;
    TypeList paramType("n");
    fcts.nfct1 = ps->fct;
    builtinTable().addFunctionSymbol(ps->key, paramType, fcts, 1, 1);
  }
  for (int i = -1; ++i < cnt5; )
  {
    struct CTagSymbol* ps = CTagSymbols + i;
    builtinTable().addTagSymbol(ps->key, ps->base, ps->cmpl);
  }
}

Tables::SearchResult Tables::doLookup(const QString& key, bool exact,
                                      int index, int lastindex)
{
  SearchResult result;
  result.count = 0;
  result.keyused = 0;
  result.table = 0;
  int itemsize;
  if ( index < 0 )
    // default: search all tables
    index = tableList.size() - 1;
  if (lastindex < 0)
    // a negative lastindex is a relative offset to index
    lastindex = index + lastindex + 1;
  for (; index >= lastindex && result.keyused < key.size(); --index)
  {
    Table& tbl = tableList[index];
    Table::const_iterator item = tbl.lookup(key, exact);
    if ( item != tbl.constEnd() 
         && (itemsize = item.key().size()) > result.keyused )
    {
      result.pt = item++;
      result.count = 1;
      while (item != tbl.constEnd() && item.key() == result.pt.key())
      {
        ++result.count;
        ++item;
      }
      result.table = &tbl;
      result.keyused = itemsize;
    }
  }
  return result;
}

Tables::SearchResult Tables::builtinLookup(const QString& key, bool exact)
{
  SearchResult r = self().doLookup(' ' + key, exact, builtinSymbols, -1);
  if (r.keyused > 0)
    --r.keyused;
  return r;
}

Tables::SearchResult Tables::lookup(const QString& key, bool exact)
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

bool Tables::keysContainChar(QChar c)
{
  // do not search the table with closing symbols
  for (int i = 0; ++i < self().tableList.size();)
    if (self().tableList[i].keysContainChar(c))
      return true;
  return false;
}

Variant Tables::escape(const ParamList& params)
{
  Settings::self()->escape = (const QString&)params.at(0);
  return 0;
}

Variant Tables::define(const ParamList& params)
{
  PSymbol symbol = builtinLookup(params.at(0)).pt.value();
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
    ok = globalTable().addOpenSymbol(key, symbol->type(), params.at(2));
  }
  else
  {
    if (params.size() != 2)
      return SYMBOLS_INVALID_PARAMCOUNT;
    ok = globalTable().cloneSymbol(key, symbol);
  }
  return ok? 0 : SYMBOLS_CLONE_ERROR;
}

Variant Tables::overload(const ParamList& params)
{
  PSymbol symbol = builtinLookup(params.at(0)).pt.value();
  if (!symbol)
    return TABLE_SYMBOL_NOT_FOUND;
  return globalTable().cloneSymbol(params.at(1), symbol, true)? 0 : SYMBOLS_CLONE_ERROR;
}

Variant Tables::undefine(const ParamList& params)
{
  int firstindex =  -1;
  if (params.size() == 2)
  {
    HNumber p2 = (const HNumber&)params.at(1);
    if (p2.isNan())
      return FLOAT_NANOPERAND;
    if (p2 != 0 && p2 != 1)
      return FLOAT_OUTOFDOMAIN;
    if (p2 == 1)
      firstindex = globalSymbols;
  }
  QString key = params.at(0);
  SearchResult sr = Tables::self().doLookup(key, true,
                                            firstindex, globalSymbols);
  if (sr.count == 0)
    return TABLE_SYMBOL_NOT_FOUND;
  sr.table->removeSymbols(key);
  return 0;
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

bool Table::overloadSymbol(const QString& key, PSymbol symbol)
{
  if (!symbol)
    return false;
  insertMulti(key, symbol);
  return true;
}

bool Table::cloneSymbol(const QString& key, PSymbol symbol, bool overload)
{
  symbol = symbol->clone(this);
  if (overload)
    return overloadSymbol(key, symbol);
  return addSymbol(key, symbol);
}

void Table::removeSymbols(const QString& key)
{
  iterator i = find(key);
  for (; i != end() && i.key() == key; ++i )
    checkDelete(i.value());
  remove(key);
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

bool Table::addTagSymbol(const QString& key, char base, bool complement)
{
  return addSymbol(key, new TagSymbol(this, base, complement));
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

bool Table::keysContainChar(QChar c)
{
  const_iterator item = constBegin();
  while (item != constEnd())
    if ((item++).key().contains(c))
      return true;
  return false;
}
