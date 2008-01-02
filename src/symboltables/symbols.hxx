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

#include "math/hmath.hxx"
#include "base/functions.hxx" // FIXME this should not be necessary
#include <QString>
#include <QList>
#include <QByteArray>

typedef enum
{
  TEmpty,
  TError,
  TNumeric,
  TText,
} BuiltinType;

class Variant: protected HNumber
// FIXME do not subclass from HNumber
{
  public:
    Variant();
    Variant(const HNumber& val) { operator=(val); };
    Variant(int error) { operator=(error); };
    void operator=(const HNumber& val);
    void operator=(int error);
    operator const HNumber& () const { return *this; };
    operator const HNumber* () const { return this; };
    operator int () const { return error(); };
    bool isNum() const;
    BuiltinType type() const { return m_type; };
  private:
    BuiltinType m_type;
    QString text;
};

typedef QList<Variant> ParamList;

typedef HNumber (*Nfct0)();
typedef HNumber (*Nfct1)(const HNumber& p1);
typedef HNumber (*Nfct2)(const HNumber& p1, const HNumber& p2);
typedef HNumber (*Nfct3)(const HNumber& p1, const HNumber& p2,
                         const HNumber& p3);
typedef HNumber (*Nfct4)(const HNumber& p1, const HNumber& p2,
                         const HNumber& p3, const HNumber& p4);
typedef HNumber (*Nfct) (const HNumberList& param);

class FctList
{
  public:
    Nfct0 nfct0;
    Nfct1 nfct1;
    Nfct2 nfct2;
    Nfct3 nfct3;
    Nfct4 nfct4;
    Nfct  nfct;
    bool matchParams(const ParamList& params);
    Variant eval(const ParamList& params);
};

class Symbol
{
  public:
    virtual int type();
    virtual ~Symbol();
};

typedef Symbol* PSymbol;

class SyntaxSymbol: public Symbol
{
  public:
    int type();
    SyntaxSymbol(int aType);
  private:
    int m_type;
};

class OpenSymbol: public SyntaxSymbol
{
  public:
    OpenSymbol(int aType, const QString& end);
    ~OpenSymbol();
    const QString& closeToken() const { return m_end; };
  private:
    QString m_end;
    SyntaxSymbol* closeSymbol;
};

class FunctionSymbol: public Symbol
{
  public:
    int type();
    FunctionSymbol(const FctList&, int minCount = 0, int maxCount = -1);
    bool matchParams(const ParamList& params);
    Variant eval(const ParamList& params);
  protected:
    int minParamCount;
    int maxParamCount;
  private:
    bool checkCount(const ParamList& params);
    FctList fcts;
};

class OperatorSymbol: public FunctionSymbol
{
  public:
    OperatorSymbol(const FctList&, int paramCount, int prec);
    int type();
    bool isUnary() { return minParamCount == 1; };
    int precedence;
};

class ConstSymbol: public Symbol
{
  public:
    ConstSymbol(const Variant& val);
    int type();
    Variant value() { return m_value; };
  protected:
    ConstSymbol(){};
    Variant m_value;
};

class AnsSymbol: public ConstSymbol
{
  public:
    AnsSymbol() {};
    int type();
    void setAns(const Variant& val) { m_value = val; };
};

class VarSymbol: public ConstSymbol
{
  public:
    VarSymbol() {};
    int type();
    void operator=(const Variant& val) { m_value = val; };
};

#endif /* _SYMBOLS_H */
