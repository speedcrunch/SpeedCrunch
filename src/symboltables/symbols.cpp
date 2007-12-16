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

const int maxfctidx = fctinterfaces - 1;

Symbol::Symbol()
{
  syntaxClass = SyntaxElement;
  symidx = 0;
}

Symbol::~Symbol()
{
}

QString Symbol::getKey(QString lookupkey) const
{
  QString result = getKey();
  return result.isEmpty()? lookupkey : result;
}

QString Symbol::getKey() const
{
  return QString();
}

bool Symbol::isMatching(QString match) const
{
  return false;
}

bool Symbol::isPostfix() const
{
  return false;
}

signed char Symbol::pairType() const
{
  return 0;
}

SyntaxSymbol::SyntaxSymbol(int idx, signed char levelchange)
  : parlevel(levelchange)
{
  symidx = idx;
}

signed char SyntaxSymbol::pairType() const
{
  return parlevel;
}

AliasSymbol::AliasSymbol(QString aliaskey, Symbol* symbol)
  : alias(symbol)
{
  syntaxClass = symbol->getClass();
  key = alias->getKey(aliaskey);
  symidx = alias->getSymidx();
}

QString AliasSymbol::getKey() const
{
  return key;
}

bool AliasSymbol::isMatching(QString match) const
{
  return alias->isMatching(match);
}

signed char AliasSymbol::pairType() const
{
  return alias->pairType();
}

ConstSymbol::ConstSymbol(HNumber x)
  : value(x)
{
  syntaxClass = Const;
}

ConstSymbol::ConstSymbol(HNumber x, int idx)
  : value(x)
{
  syntaxClass = Const;
  symidx = idx;
}

VarSymbol::VarSymbol(HNumber x)
  : ConstSymbol(x)
{
  syntaxClass = Var;
}

AnsSymbol::AnsSymbol()
  : VarSymbol(HNumber())
{
  syntaxClass = Ans;
}

FunctionSymbol::FunctionSymbol(int idx, int minParam, int maxParam)
  : minParamCount(minParam), maxParamCount(maxParam)
{
  syntaxClass = Function;
  symidx = idx;
  for (int i = 0; i < fctinterfaces; ++i)
    func[i].f0 = 0;
}

void FunctionSymbol::addFunc(int paramCount, Func aFunc)
{
  if (paramCount < maxfctidx-1)
    func[paramCount] = aFunc;
  else if (paramCount == -1)
    func[maxfctidx] = aFunc;
}

FunctionResult FunctionSymbol::eval(const QVector<HNumber>& params)
{
  FunctionResult result;
  int fctidx;

  HMath::getError();
  fctidx = params.size();
  if (fctidx < minParamCount || fctidx > maxParamCount)
    fctidx = -1;
  if (fctidx > maxfctidx)
    fctidx = maxfctidx;
  if (fctidx >= 0 && !func[fctidx].f0)
    fctidx = -1;
  switch(fctidx)
  {
    case 0: result.value = func[0].f0(); break;
    case 1: result.value = func[1].f1(params[0]); break;
    case 2: result.value = func[2].f2(params[0], params[1]); break;
    case 3: result.value = func[3].f3(params[0], params[1], params[2]); break;
    case 4: result.value = func[4].f4(params[0], params[1], params[2], params[3]); break;
    case maxfctidx: result.value = func[maxfctidx].fl(params); break;
    default:;
  }
  if (result.value.isNan())
    if (fctidx < 0)
      result.msg = "invalid number of parameters";
    else
      result.msg = HMath::getError();
  return result;
}

OperatorSymbol::OperatorSymbol(int idx, int opflags, bool postfixop)
  : FunctionSymbol(idx, 1, 2),postfix(postfixop)
{
  if ((opflags & unaryOp) == 0)
    minParamCount = 2;
  if ((opflags & binaryOp) == 0)
    maxParamCount = 1;
}

bool OperatorSymbol::isPostfix() const
{
  return postfix;
}
