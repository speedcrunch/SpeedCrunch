/* symbols.c: symbols of the parser/lexical analyzer */
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

Symbol::Symbol()
{
  syntaxClass = SyntaxElement;
  helpidx = 0;
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

signed char Symbol::pairType() const
{
  return 0;
}

SyntaxSymbol::SyntaxSymbol(signed char levelchange)
  : parlevel(levelchange)
{
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
  helpidx = alias->getHelp();
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

ConstSymbol::ConstSymbol(HNumber x, int help)
  : value(x)
{
  syntaxClass = Const;
  helpidx = help;
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

FunctionSymbol::FunctionSymbol(int help)
{
  syntaxClass = Function;
  helpidx = help;
}

HNumber FunctionSymbol::eval(QVector<HNumber>)
{
  return HNumber();
}
