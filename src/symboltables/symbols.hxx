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

typedef enum
{
  unknownSym = 'X',
  functionSym = 'F',
  operatorSym = 'O',
  constSym = 'C',
  ansSym = 'A',
  varSym = 'V',
  referenceSym = 'R',
  closeSym = ')',
  dot = '.',
  assign = '=',
  separator = ';',
  signPlus = 'P',
  signMinus = 'M',
  openPar = '(',
  quote = '"',
} SymType;

typedef enum
{
  TEmpty,
  TError,
  TNumeric,
  TText,
} VariantType;

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
    bool isText() const { return type() == TText; };
    VariantType type() const { return m_type; };
  private:
    VariantType m_type;
    QString text;
};

class ParamList: public QList<Variant>
{
  public:
    bool allNums() const;
};

class TypeList: protected QList<VariantType>
{
  public:
    void appendType(VariantType t) { QList<VariantType>::append(t); };
    bool match(const ParamList&) const;
};

typedef HNumber (*Nfct0)();
typedef HNumber (*Nfct1)(const HNumber& p1);
typedef HNumber (*Nfct2)(const HNumber& p1, const HNumber& p2);
typedef HNumber (*Nfct3)(const HNumber& p1, const HNumber& p2,
                         const HNumber& p3);
typedef HNumber (*Nfct4)(const HNumber& p1, const HNumber& p2,
                         const HNumber& p3, const HNumber& p4);
typedef HNumber (*Nfct) (const HNumberList& param);
typedef Variant (*Vfct) (const ParamList& params);

class FctList
{
  public:
    Nfct0 nfct0;
    Nfct1 nfct1;
    Nfct2 nfct2;
    Nfct3 nfct3;
    Nfct4 nfct4;
    Nfct  nfct;
    Vfct  vfct;
    Variant eval(const ParamList& params) const;
    bool match(const ParamList& params) const;
};

class Symbol
{
  public:
    virtual SymType type() const;
    virtual ~Symbol();
};

typedef Symbol* PSymbol;

class SyntaxSymbol: public Symbol
{
  public:
    SymType type() const;
    SyntaxSymbol(SymType aType);
  private:
    SymType m_type;
};

class CloseSymbol: public Symbol
{
  public:
    CloseSymbol(SymType open) { m_openType = open; };
    SymType openType() const { return m_openType; };
    SymType type() const;
  private:
    SymType m_openType;
};

class OpenSymbol: public SyntaxSymbol
{
  public:
    OpenSymbol(SymType aType, const QString& end);
    ~OpenSymbol();
    const QString& closeToken() const { return m_end; };
  private:
    QString m_end;
    CloseSymbol* closeSymbol;
};

class FunctionSymbol: public Symbol
{
  public:
    SymType type() const;
    FunctionSymbol(const TypeList&, const FctList&, int minCount = 0, int maxCount = -1);
    bool match(const ParamList& params) const;
    Variant eval(const ParamList& params) const;
  protected:
    int minParamCount;
    int maxParamCount;
  private:
    bool checkCount(const ParamList& params) const;
    FctList fcts;
    TypeList types;
};

class OperatorSymbol: public FunctionSymbol
{
  public:
    OperatorSymbol(const TypeList&, const FctList&, int paramCount, int prec);
    SymType type() const;
    bool isUnary() const { return minParamCount == 1; };
    int precedence;
};

class ConstSymbol: public Symbol
{
  public:
    ConstSymbol(const Variant& val);
    SymType type() const;
    const Variant& value() const { return m_value; };
  protected:
    ConstSymbol(){};
    Variant m_value;
};

class AnsSymbol: public ConstSymbol
{
  public:
    AnsSymbol() {};
    SymType type() const;
    void setAns(const Variant& val) { m_value = val; };
};

class VarSymbol: public ConstSymbol
{
  public:
    VarSymbol() {};
    SymType type() const;
    void operator=(const Variant& val) { m_value = val; };
};

class ReferenceSymbol: public Symbol
{
  public:
    ReferenceSymbol();
    SymType type() const;
    const Symbol* operator[](int index) const;
    void operator=(const Symbol& other);
  private:
    static const unsigned maxindex = 3;
    const Symbol* symbols[maxindex];
};

#endif /* _SYMBOLS_H */
