/* symbols.hxx: symbols of the parser/lexical analyzer */
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

const int unaryOp = 1;
const int binaryOp = 2;
const int fctinterfaces = 6;

typedef HNumber (*Func0)();
typedef HNumber (*Func1)(const HNumber&);
typedef HNumber (*Func2)(const HNumber&, const HNumber&);
typedef HNumber (*Func3)(const HNumber&, const HNumber&, const HNumber&);
typedef HNumber (*Func4)(const HNumber&, const HNumber&, const HNumber&, const HNumber&);
typedef HNumber (*Funcl)(const QVector<HNumber>&);

typedef union
{
  Func0 f0;
  Func1 f1;
  Func2 f2;
  Func3 f3;
  Func4 f4;
  Funcl fl;
} Func;

typedef struct
{
  Func0 f0;
  Func1 f1;
  Func2 f2;
  Func3 f3;
  Func4 f4;
  Funcl fl;
} FuncList;

typedef Func FuncArray[fctinterfaces];

struct FunctionResult
{
  HNumber value;
  QString msg;
};

class Symbol
{
  public:
    typedef enum { SyntaxElement, Const, Ans, Var, Function, Operator } SyntaxClass;

    Symbol();
    virtual ~Symbol();

    virtual bool isMatching(QString match) const;
    virtual bool isPostfix() const;
    virtual signed char pairType() const;
    QString getKey(QString lookupkey) const;
    SyntaxClass getClass() const { return syntaxClass; };
    int getSymidx() const { return symidx; };

  protected:
    virtual QString getKey() const;
    SyntaxClass syntaxClass;
    int symidx;
};

class SyntaxSymbol: public Symbol
{
  public:
    SyntaxSymbol(int idx, signed char levelchange);
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
    ConstSymbol(HNumber x, int idx);
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
    FunctionSymbol(int idx, int minParam, int maxParam);
    FunctionResult eval(const QVector<HNumber>& params);
    void addFunc(int paramCount, Func aFunc);
  protected:
    FuncArray func;
    int minParamCount;
    int maxParamCount;
};

class OperatorSymbol: public FunctionSymbol
{
  public:
    OperatorSymbol(int idx, int opflags, bool postfixop);
    bool isPostfix() const;
  protected:
    bool postfix;
};

#endif /* _SYMBOLS_H */
