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

bool FctList::matchParams(const ParamList& params)
{
  for (int i = 0; i < params.size(); ++i)
    if (!params.at(i).isNum())
      return false;
  return true;
}

Variant FctList::eval(const ParamList& params)
{
  if (!matchParams(params))
    return SYMBOLS_INVALID_PARAMTYPE;
  switch (params.size())
  {
    case 0:
      if (nfct0)
        return nfct0();
      break;
    case 1:
      if (nfct1)
        return nfct1((const HNumber&)params.at(0));
      break;
    case 2:
      if (nfct2)
        return nfct2((const HNumber&)params.at(0),
                     (const HNumber&)params.at(1));
      break;
    case 3:
      if (nfct3)
        return nfct3((const HNumber&)params.at(0),
                     (const HNumber&)params.at(1),
                     (const HNumber&)params.at(2));
      break;
    case 4:
      if (nfct4)
        return nfct4((const HNumber&)params.at(0),
                     (const HNumber&)params.at(1),
                     (const HNumber&)params.at(2),
                     (const HNumber&)params.at(3));
    default: break;
  }
  HNumberList hnumbers;
  for (int i = 0; i < params.size(); ++i)
    hnumbers.append((const HNumber*)params.at(i));
  return nfct(hnumbers);
}


Symbol::~Symbol()
{
}

int Symbol::type()
{
  return UNKNOWNTOKEN;
}

OpenSymbol::OpenSymbol(int aType, const QString& end)
  : SyntaxSymbol(aType)
{
  m_end = end;
  closeSymbol = new SyntaxSymbol(-aType);
  Tables::addCloseSymbol(end, closeSymbol);
}

OpenSymbol::~OpenSymbol()
{
  // the tables own all symbols, so do not free it here
  Tables::removeCloseSymbol(closeSymbol);
}

SyntaxSymbol::SyntaxSymbol(int aType)
  : m_type(aType)
{
}

int SyntaxSymbol::type()
{
  return m_type;
}

FunctionSymbol::FunctionSymbol(const FctList& list, int minCount, int maxCount)
{
  if (maxCount < 0)
    maxCount = minCount;
  maxParamCount = maxCount;
  minParamCount = minCount;
  fcts = list;
}

bool FunctionSymbol::checkCount(const ParamList& params)
{
  return params.size() >= minParamCount
      && params.size() <= maxParamCount;
}

int FunctionSymbol::type()
{
  return 'f';
}

bool FunctionSymbol::matchParams(const ParamList& params)
{
  return checkCount(params) && fcts.matchParams(params);
}

OperatorSymbol::OperatorSymbol(const FctList& list, int paramCount, int prec)
  : FunctionSymbol(list, paramCount, paramCount), precedence(prec)
{
}

int OperatorSymbol::type()
{
  return 'o';
}

Variant FunctionSymbol::eval(const ParamList& params)
{
  if (!checkCount(params))
    return SYMBOLS_INVALID_PARAMCOUNT;
  return fcts.eval(params);
}

int ConstSymbol::type()
{
  return 'c';
}

ConstSymbol::ConstSymbol(const Variant& val)
{
  m_value = val;
}

int AnsSymbol::type()
{
  return 'a';
}

int VarSymbol::type()
{
  return 'v';
}
