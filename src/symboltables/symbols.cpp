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
  operator=(HNumber::nan(error));
  if (error == 0)
    m_type = TEmpty;
}

bool Variant::isNum() const
{
  return m_type == TNumeric || m_type == TError || !isNan();
}

bool ParamList::allNums() const
{
  for (int i = 0; i < size(); ++i)
    if (!at(i).isNum())
      return false;
  return true;
}

bool TypeList::match(const ParamList& params) const
{
  if (params.size() < size()) return false;
  for (int i = 0; i < size();)
    if (at(i) != params.at(i).type())
      switch (at(i))
      {
        case TEmpty: continue;
        case TNumeric: if (params.at(i).isNum()) continue; // fall through
        default: return false;
      };
  return true;
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
    return SYMBOLS_INVALID_PARAMTYPE;
}

Symbol::~Symbol()
{
}

SymType Symbol::type() const
{
  return unknownSym;
}

SyntaxSymbol::SyntaxSymbol(SymType aType)
  : m_type(aType)
{
}

SymType SyntaxSymbol::type() const
{
  return m_type;
}

SymType CloseSymbol::type() const
{
  return closeSym;
}

OpenSymbol::OpenSymbol(SymType aType, const QString& end)
  : SyntaxSymbol(aType)
{
  m_end = end;
  closeSymbol = new CloseSymbol(aType);
  Tables::addCloseSymbol(end, closeSymbol);
}

OpenSymbol::~OpenSymbol()
{
  // the tables own all symbols, so do not free it here
  Tables::removeCloseSymbol(closeSymbol);
}

FunctionSymbol::FunctionSymbol(const TypeList& tlist, const FctList& flist,
                               int minCount, int maxCount)
{
  if (maxCount < 0)
    maxCount = minCount;
  maxParamCount = maxCount;
  minParamCount = minCount;
  fcts = flist;
  types = tlist;
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
  return checkCount(params)
         && fcts.match(params)
         && types.match(params);
}

Variant FunctionSymbol::eval(const ParamList& params) const
{
  if (!checkCount(params))
    return SYMBOLS_INVALID_PARAMCOUNT;
  if (!types.match(params))
    return SYMBOLS_INVALID_PARAMTYPE;
  return fcts.eval(params);
}

OperatorSymbol::OperatorSymbol(const TypeList& tlist, const FctList& flist,
                               int paramCount, int prec)
  : FunctionSymbol(tlist, flist, paramCount, paramCount), precedence(prec)
{
}

SymType OperatorSymbol::type() const
{
  return operatorSym;
}

SymType ConstSymbol::type() const
{
  return constSym;
}

ConstSymbol::ConstSymbol(const Variant& val)
{
  m_value = val;
}

SymType AnsSymbol::type() const
{
  return ansSym;
}

SymType VarSymbol::type() const
{
  return varSym;
}

SymType ReferenceSymbol::type() const
{
  return referenceSym;
}

ReferenceSymbol::ReferenceSymbol()
{
  for (unsigned i = 0; i < sizeof(symbols); ++i)
    symbols[i] = 0;
}

const Symbol* ReferenceSymbol::operator[](int index) const
{
  if (index < 0 || (unsigned)index >= sizeof(symbols)/sizeof(symbols[0]))
    return 0;
  return symbols[index];
}

void ReferenceSymbol::operator=(const Symbol& other)
{
  unsigned firstFree = 0;
  while (firstFree < maxindex && symbols[firstFree])
    ++firstFree;
  if (firstFree >= maxindex) return;
  if (other.type() == referenceSym)
    for (int i = 0; firstFree < maxindex; ++firstFree, ++i)
      symbols[firstFree] = (static_cast<const ReferenceSymbol&>(other))[i];
  else
    symbols[firstFree] = &other;
}
