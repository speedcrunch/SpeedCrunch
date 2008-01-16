/* symbols.hxx: symbols of the parser/lexical analyzer */
/*
    Copyright (C) 2007, 2008 Wolf Lammen.

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
#include <QLinkedList>

typedef enum
{
  unassigned = 0,
  functionSym = 'F',
  operatorSym = 'O',
  constSym = 'C',
  ansSym = 'A',
  varSym = 'V',
  tagSym = 'T',
  openSym = '(',
  closeSym = ')',
  dot = '.',
  assign = '=',
  separator = ';',
  signPlus = 'P',
  signMinus = 'M',
  quote = '"',
  group = ',',
  scale = 'S',
  decscale = 'E',
} SymType;

typedef enum
{
  TEmpty,
  TError,
  TInteger,
  TNumeric,
  TText,
} VariantType;

class Variant: protected HNumber
// FIXME do not subclass from HNumber
{
  public:
    Variant();
    Variant(const HNumber& val) { *this = val; };
    Variant(int error) { *this = error; };
    Variant(const QString& val) { *this = val; };
    void operator=(const HNumber& val);
    void operator=(int error);
    void operator=(const QString& str);
    operator const HNumber& () const { return *this; };
    operator const HNumber* () const { return this; };
    operator int () const { return error(); };
    operator const QString& () const { return text; };
    int matchType(VariantType) const;
    bool isNum() const;
    VariantType type() const { return m_type; };
  private:
    VariantType m_type;
    QString text;
};

class TypeList: public QList<VariantType>
{
  public:
    TypeList(){};
    TypeList(const char* desc);
    void appendType(VariantType t, int count = 1);
};

typedef struct
{
  int index;
  int error;
} TypeCheck;

class ParamList: public QList<Variant>
{
  public:
    bool allNums() const;
    bool isType(int index, VariantType) const;
    bool isNum(int index) const;
    TypeCheck match(const TypeList&) const;
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
    void clear();
};

class Symbol;
class TagSymbol;
class FunctionSymbol;
class OperatorSymbol;
class OpenSymbol;
class ConstSymbol;
class VarSymbol;

typedef Symbol* PSymbol;

class Symbol
{
  public:
    Symbol(void* aOwner) : m_owner(aOwner) {};
    virtual ~Symbol();
    virtual SymType type() const;
    virtual PSymbol clone(void* aOwner) const;
    void* owner() const { return m_owner; };
    const TagSymbol* asTag() const;
    const FunctionSymbol* asFunc() const;
    const OpenSymbol* asOpen() const;
    OperatorSymbol* asOp();
    const OperatorSymbol* asOp() const;
    const ConstSymbol* asConst() const;
    const VarSymbol* asVar() const;
  private:
    void* m_owner;
};

class SyntaxSymbol: public Symbol
{
  public:
    SyntaxSymbol(void* aOwner, SymType aType);
    SymType type() const;
    PSymbol clone(void* aOwner) const;
  private:
    SymType m_type;
};

class CloseSymbol: public Symbol
{
  public:
    CloseSymbol(void* aOwner, SymType open);
    SymType openType() const { return m_opentype; };
    SymType type() const;
    PSymbol clone(void* aOwner) const;
  private:
    SymType m_opentype;
};

class OpenSymbol: public SyntaxSymbol
{
  public:
    OpenSymbol(void* aOwner, SymType aType, const QString& end);
    ~OpenSymbol();
    const QString& closeToken() const { return m_end; };
    PSymbol clone(void* aOwner) const;
    PSymbol setCloseSymbolKey(const QString& key);
  private:
    QString m_end;
    CloseSymbol* closeSymbol;
};

class TagSymbol: public Symbol
{
  public:
    TagSymbol(void* aOwner, char aBase, bool iscomplement);
    char base() const { return m_base; };
    bool complement() const { return m_compl; };
    SymType type() const;
    PSymbol clone(void* aOwner) const;
  private:
    char m_base;
    bool m_compl;
};

class FunctionSymbol: public Symbol
{
  public:
    SymType type() const;
    FunctionSymbol(void* aOwner, const TypeList&, const FctList&,
                   int minCount = 0, int maxCount = -1);
    bool match(const ParamList& params) const;
    Variant eval(const ParamList& params) const;
    PSymbol clone(void* aOwner) const;
  protected:
    int minParamCount;
    int maxParamCount;
    FctList fcts;
    TypeList types;
  private:
    bool checkCount(const ParamList& params) const;
};

class OperatorSymbol: public FunctionSymbol
{
  public:
    OperatorSymbol(void* aOwner, const TypeList&, const FctList&, int prec);
    bool isUnary() const;
    SymType type() const;
    int precedence() const { return m_prec; };
    PSymbol setPrecedence(int prec) { m_prec = prec; return this;};
    PSymbol clone(void* aOwner) const;
  protected:
    int m_prec;
};

class VarSymbolIntf: public Symbol
{
  public:
    VarSymbolIntf(void* aOwner): Symbol(aOwner){};
    virtual const Variant& value() const = 0;
};

class ConstSymbol: public VarSymbolIntf
{
  public:
    ConstSymbol(void* aOwner, const Variant& val);
    SymType type() const;
    const Variant& value() const { return m_value; };
    PSymbol clone(void* aOwner) const;
  protected:
    const Variant& m_value;
};

class AnsSymbol: public VarSymbolIntf
{
  public:
    AnsSymbol(void* aOwner);
    SymType type() const;
    PSymbol clone(void* aOwner) const;
    void setAns(const Variant& val) { ansVar = val; };
    const Variant& value() const { return ansVar; };
  protected:
    static Variant ansVar;
};

class VarSymbol: public VarSymbolIntf
{
  public:
    VarSymbol(void* aOwner);
    ~VarSymbol();
    SymType type() const;
    PSymbol clone(void* aOwner) const;
    const Variant& value() const { return m_var->v; };
    Variant* leftVal() const { return &m_var->v; };
  private:
    typedef struct
    {
      unsigned refcount;
      Variant v;
    } VariableData;
    typedef QLinkedList<VariableData>::iterator iterator;

    VarSymbol(void* aOwner, iterator);
    static QLinkedList<VariableData> variables;
    static iterator newVar();
    iterator m_var;
};

#endif /* _SYMBOLS_H */
