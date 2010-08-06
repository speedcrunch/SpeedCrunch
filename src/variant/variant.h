/* variant.h: variant class */
/*
    Copyright (C) 2008 Wolf Lammen.

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

#ifndef _VARIANT_H
#define _VARIANT_H

#include "variant/real.h"
//#include "variant/vstring.h"

class VariantBase: public VariantIntf
// takes care of builtin data types
{
  public:
    VariantBase() { setup(); };
    VariantType type() const;
    operator bool() const;
    operator Error() const;
    bool operator = (bool);
    Error operator = (Error);
    operator const VariantData*() const;
    void xmlWrite(QDomDocument&, QDomNode&) const;
    bool xmlRead(QDomNode&);
  protected:
    bool isBuiltIn() const { return m_type != btExtern; };
    bool toBuiltInType(const char* typeName);
    void operator = (VariantData*);
    VariantData* variantData() { return val; };
    const VariantData* variantData() const { return val; };
    void operator = (const VariantBase& other);
    virtual void teardown() = 0;
    static QByteArray xmlTypeAttr(const QDomNode&);
    static void initClass();
  private:
    enum
    {
      btError,
      btBool,
      btExtern
    } m_type;
    union
    {
      Error error;
      bool boolval;
      VariantData* val;
    };
    bool isBool() const { return m_type == btBool; };
    bool isError() const { return m_type == btError; };
    void setup();
    static VariantData* builtInConstructor() { return 0; };
    static VariantType vtBool;
    static VariantType vtError;
};

class Variant: public VariantBase
{
  friend class InitVariant;
  public: // constructors
    Variant() { };
    Variant(bool v) { *this = v; };
    Variant(Error e) { *this = e; };
    Variant(VariantData* vd) { *this = vd; };
    Variant(const Variant& other) { *this = other; };
    ~Variant() { teardown(); };
  public: // assignment
    Variant& operator = (const Variant& other);
    void operator = (VariantData*);
  public: // IO
    void xmlWrite(QDomDocument&, QDomNode&) const;
    bool xmlRead(QDomNode&);
    QByteArray toUtf8() const;
    static Variant fromUtf8(const char* utf8, const char* type = 0);
    static const char* xmlTagName;
  public: // types & conversion
    VariantType type() const;
    bool is(const char* name) const { return typeName(type()) == name; };
  public: // operators & functions
  protected:
    void teardown();
  private:
    static QDomElement createEmptyElement(QDomDocument&, const char* type);
    static void initClass();
  public: // support of LongReal
    Variant(double d) { *this = d; };
    Variant(floatnum, Error = Success);
    operator cfloatnum() const;
    operator double() const;
    double operator = (double);
  public: // support of Strings
    Variant(QString, Error = Success);
    const QString& operator = (const QString& s);
    operator QString() const;
};

#endif /*_VARIANT_H*/
