/* symbols.h: symbols of the parser/lexical analyzer */
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

#ifndef _SYMBOLS_H
#define _SYMBOLS_H

#include <QString>
#include <QVector>
#include "math/hmath.hxx"

class Symbol
{
  public:
    typedef enum { SyntaxElement, Const, Ans, Var, Function, Operator } SyntaxClass;

    Symbol();
    virtual ~Symbol();

    virtual bool isMatching(QString match) const;
    virtual signed char pairType() const;
    QString getKey(QString lookupkey) const;
    SyntaxClass getClass() const { return syntaxClass; };
    int getHelp() const { return helpidx; };

  protected:
    virtual QString getKey() const;
    SyntaxClass syntaxClass;
    int helpidx;
};

class SyntaxSymbol: public Symbol
{
  public:
    SyntaxSymbol(signed char levelchange);
    signed char pairType() const;
  protected:
    signed char parlevel;
};

class AliasSymbol: public Symbol
{
  public:
    Symbol* alias;
    QString key;

    AliasSymbol(QString key, Symbol* symbol);
    bool isMatching(QString match) const;
    signed char pairType() const;
  protected:
    QString getKey() const;
};

class ConstSymbol: public Symbol
{
  public:
    ConstSymbol(HNumber x);
    ConstSymbol(HNumber x, int help);
    HNumber get() const { return value; };

  protected:
    HNumber value;
};

class VarSymbol: public ConstSymbol
{
  public:
    VarSymbol(HNumber x);
    void set(HNumber x) { value = x; };
};

class AnsSymbol: public VarSymbol
{
  public:
    AnsSymbol();
};

class FunctionSymbol: public Symbol
{
  public:
    FunctionSymbol(int help);
    HNumber eval(QVector<HNumber> params);
};

class OperatorSymbol: public FunctionSymbol
{
};

#endif /* _SYMBOLS_H */
