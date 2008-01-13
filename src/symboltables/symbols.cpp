/* symbols.cpp: symbols of the parser/lexical analyzer */
/*
    Copyright (C) 2007 Wolf Lammen.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License , or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING.  If not, write to:

      The Free Software Foundation, Inc.
      59 Temple Place, Suite 330
      Boston, MA 02111-1307 USA.


    You may contact the author by:
       e-mail:  ookami1 <at> gmx <dot> de
       mail:  Wolf Lammen
              Oertzweg 45
              22307 Hamburg
              Germany

*************************************************************************/

#include "symboltables/symbols.hxx"
#include "symboltables/tables.hxx"
#include "main/errors.h"

Variant::Variant()
{
  m_type = TEmpty;
}

void Variant::operator=(const HNumber& val)
{
  HNumber::operator=(val);
  text.clear();
  if ( HNumber::error() == 0 )
    m_type = TNumeric;
  else
    m_type = TError;
}

void Variant::operator=(int error)
{
  *this = HNumber::nan(error);
  if (error == 0)
    m_type = TEmpty;
}

void Variant::operator=(const QString& str)
{
  *this = 0;
  text = str;
  m_type = TText;
}

int Variant::match(VariantType t) const
{
  switch (t)
  {
    case TEmpty: return 0;
    case TInteger:
      if (type() == TNumeric && isInteger())
        return 0; // fall through
    case TNumeric:
      if (type() == TError || (type() == TNumeric && isNan()))
        return FLOAT_NANOPERAND; // fall through
    default:
      if (type() != t)
        return SYMBOLS_TYPE_MISMATCH;
  }
  return 0;
}

bool Variant::isNum() const
{
  switch (match(TNumeric))
  {
    case 0:
    case FLOAT_NANOPERAND: return true;
    default: return false;
  }
}

void TypeList::appendType(VariantType t, int cnt)
{
  while (--cnt >= 0)
    append(t);
}

bool ParamList::allNums() const
{
  for (int i = 0; i < size(); ++i)
    if (!at(i).isNum())
      return false;
  return true;
}

bool ParamList::isType(int index, VariantType t) const
{
  return index <= size() && at(index).match(t) == 0;
}

bool ParamList::isNum(int index) const
{
  return index <= size() && at(index).isNum();
}

TypeCheck ParamList::match(const TypeList& types) const
{
  TypeCheck result;
  result.error = 0;
  int limit = qMin(types.size(), size());
  for (result.index = -1; ++result.index < limit && result.error == 0;)
    result.error = at(result.index).match(types.at(result.index));
  return result;
}

TypeList::TypeList(const char* s)
{
  for (const char* p = s; *p != 0; ++p)
  {
    VariantType t;
    switch(*p)
    {
      case 't': t = TText; break;
      case 'e': t = TError; break;
      case 'i': t = TInteger; break;
      case 'n': t = TNumeric; break;
      default : t = TEmpty;
    }
    appendType(t);
  }
}

bool FctList::match(const ParamList& params) const
{
  if (vfct) return true;
  int sz = params.size();
  return params.allNums()
         && (nfct
             || (nfct0 && sz == 0)
             || (nfct1 && sz == 1)
             || (nfct2 && sz == 2)
             || (nfct3 && sz == 3)
             || (nfct4 && sz == 4));
}

void FctList::clear()
{
  nfct0 = 0;
  nfct1 = 0;
  nfct2 = 0;
  nfct3 = 0;
  nfct4 = 0;
  nfct  = 0;
  vfct  = 0;
}

Variant FctList::eval(const ParamList& params) const
{
  if (params.allNums())
    switch (params.size())
    {
      case 0:
        if (nfct0)
          return nfct0();
      case 1:
        if (nfct1)
          return nfct1((const HNumber&)params.at(0));
      case 2:
        if (nfct2)
          return nfct2((const HNumber&)params.at(0),
                      (const HNumber&)params.at(1));
      case 3:
        if (nfct3)
          return nfct3((const HNumber&)params.at(0),
                      (const HNumber&)params.at(1),
                      (const HNumber&)params.at(2));
      case 4:
        if (nfct4)
          return nfct4((const HNumber&)params.at(0),
                      (const HNumber&)params.at(1),
                      (const HNumber&)params.at(2),
                      (const HNumber&)params.at(3));
      default:
        if (nfct)
        {
          HNumberList hnumbers;
          for (int i = 0; i < params.size(); ++i)
            hnumbers.append((const HNumber*)params.at(i));
          return nfct(hnumbers);
        }
    }
    if (vfct)
      return vfct(params);
    return SYMBOLS_TYPE_MISMATCH;
}

Symbol::~Symbol()
{
}

SymType Symbol::type() const
{
  return unassigned;
}

PSymbol Symbol::clone(void* aOwner) const
{
  return 0;
}

const OpenSymbol* Symbol::asOpen() const
{
  return dynamic_cast<const OpenSymbol*>(this);
}

const TagSymbol* Symbol::asTag() const
{
  return dynamic_cast<const TagSymbol*>(this);
}

const FunctionSymbol* Symbol::asFunc() const
{
  return dynamic_cast<const FunctionSymbol*>(this);
}

const OperatorSymbol* Symbol::asOp() const
{
  return dynamic_cast<const OperatorSymbol*>(this);
}

OperatorSymbol* Symbol::asOp()
{
  return dynamic_cast<OperatorSymbol*>(this);
}

SyntaxSymbol::SyntaxSymbol(void* aOwner, SymType aType)
  : Symbol(aOwner), m_type(aType)
{
}

SymType SyntaxSymbol::type() const
{
  return m_type;
}

PSymbol SyntaxSymbol::clone(void* aOwner) const
{
  return new SyntaxSymbol(aOwner, type());
}

CloseSymbol::CloseSymbol(void* aOwner, SymType open)
  : Symbol(aOwner), m_opentype(open)
{
}

TagSymbol::TagSymbol(void* aOwner, char aBase, bool iscomplement)
  : Symbol(aOwner), m_base(aBase), m_compl(iscomplement)
{
}

SymType TagSymbol::type() const
{
  return tagSym;
}

PSymbol TagSymbol::clone(void* aOwner) const
{
  return new TagSymbol(aOwner, base(), complement());
}

PSymbol CloseSymbol::clone(void* aOwner) const
{
  return 0;
}

SymType CloseSymbol::type() const
{
  return closeSym;
}

OpenSymbol::OpenSymbol(void* aOwner, SymType aType, const QString& end)
  : SyntaxSymbol(aOwner, aType), closeSymbol(0)
{
  setCloseSymbolKey(end);
}

OpenSymbol::~OpenSymbol()
{
  setCloseSymbolKey(QString());
}

PSymbol OpenSymbol::setCloseSymbolKey(const QString& key)
{
  if (closeSymbol)
  {
    Tables::removeCloseSymbol(closeSymbol);
    if (closeSymbol->owner() == this)
      delete closeSymbol;
  }
  m_end = key;
  if (!key.isEmpty())
  {
    closeSymbol = new CloseSymbol(this, type());
    Tables::addCloseSymbol(key, this);
  }
  return this;
}

PSymbol OpenSymbol::clone(void* aOwner) const
{
  return new OpenSymbol(aOwner, type(), closeToken());
}

FunctionSymbol::FunctionSymbol(void* aOwner, const TypeList& tlist,
      const FctList& flist, int minCount, int maxCount)
  : Symbol(aOwner)
{
  if (maxCount < 0)
    maxCount = minCount;
  maxParamCount = maxCount;
  minParamCount = minCount;
  fcts = flist;
  types = tlist;
}

PSymbol FunctionSymbol::clone(void* aOwner) const
{
  return new FunctionSymbol(aOwner, types, fcts, minParamCount, maxParamCount);
}

bool FunctionSymbol::checkCount(const ParamList& params) const
{
  return params.size() >= minParamCount
      && params.size() <= maxParamCount;
}

SymType FunctionSymbol::type() const
{
  return functionSym;
}

bool FunctionSymbol::match(const ParamList& params) const
{
  if (!checkCount(params)
      || !fcts.match(params))
    return false;
  switch (params.match(types).error)
  {
    case 0:
    case FLOAT_NANOPERAND: return true;
    default: return false;
  }
}

Variant FunctionSymbol::eval(const ParamList& params) const
{
  if (!checkCount(params))
    return SYMBOLS_INVALID_PARAMCOUNT;
  TypeCheck tc = params.match(types);
  if (tc.error != 0)
    return tc.error;
  return fcts.eval(params);
}

OperatorSymbol::OperatorSymbol(void* aOwner, const TypeList& tlist,
                               const FctList& flist, int prec)
  : FunctionSymbol(aOwner, tlist, flist, tlist.size(), tlist.size()),
    m_prec(prec)
{
}

PSymbol OperatorSymbol::clone(void* aOwner) const
{
  return new OperatorSymbol(aOwner, types, fcts, m_prec);
}

bool OperatorSymbol::isUnary() const
{
  return minParamCount == 1;
}

SymType OperatorSymbol::type() const
{
  return operatorSym;
}

SymType AnsSymbol::type() const
{
  return ansSym;
}

SymType VarSymbol::type() const
{
  return varSym;
}

ConstSymbol::ConstSymbol(void* aOwner)
  : Symbol(aOwner)
{
}

ConstSymbol::ConstSymbol(void* aOwner, const Variant& val)
  : Symbol(aOwner)
{
  m_value = val;
}

SymType ConstSymbol::type() const
{
  return constSym;
}
