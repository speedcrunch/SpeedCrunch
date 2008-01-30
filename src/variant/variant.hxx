/* variant.hxx: variant class */
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

#include "base/errors.h"
#include "variant/variantdata.hxx"
#include "variant/real.hxx"
#include "variant/variant.hxx"

class VariantBase: public VariantIntf
{
  protected:
    VariantBase(): m_type(vError) {};
    VariantType m_type;
  private:
};

class Variant: public VariantBase
{
  friend class InitVariant;
  public: // constructors
    Variant(Error e){ *this = e; };
    Variant(bool b) { *this = b; };
    Variant() { *this = NoOperand; };
    Variant(VariantData* d) { *this = d; };
    Variant(const Variant& other) { *this = other; };
    Variant(VariantType t, const char* value) { assign(t, value); };
    ~Variant() { *this = Success; };
  public: // assignment
    void operator=(Error);
    void operator=(bool);
    void operator=(VariantData*);
    void operator=(const Variant&);
    bool assign(VariantType, const char*);
    bool assign(const QByteArray&);
    public: // types & conversion
    operator bool() const;
    operator Error() const;
    operator QByteArray() const;
    VariantType type() const { return m_type; };
    static bool isBuiltinType(VariantType);
    Variant embed() const;
    void clear();
  public: // operators & functions
    Variant operator+() const;
    Variant operator-() const;
    Variant operator~() const;
    Variant operator!() const;
    Variant operator+(const Variant&) const;
    Variant operator-(const Variant&) const;
    Variant operator*(const Variant& other) const;
    Variant operator/(const Variant& other) const;
    Variant operator%(const Variant& other) const;
    Variant idiv(const Variant& other) const;
    Variant raise(const Variant& exp) const;
    Variant operator&(const Variant& other) const;
    Variant operator|(const Variant& other) const;
    Variant operator^(const Variant& other) const;
    Variant operator==(const Variant& other) const;
    Variant operator!=(const Variant& other) const;
    Variant operator>(const Variant& other) const;
    Variant operator>=(const Variant& other) const;
    Variant operator<(const Variant& other) const;
    Variant operator<=(const Variant& other) const;
  protected:
    typedef Variant (VariantData::*Method1)() const;
    typedef Variant (VariantData::*Method2)(const Variant& other) const;

    // retype sets val to 0, defaultType creates a default value
    void defaultType(VariantType);
    void retype(VariantType);
    Variant overloadfct(Method1) const;
    Variant overloadfct(Method2, const Variant& other) const;
  private:
    union{
      Error error;
      bool boolval;
      VariantData* val;
    };
    static void initClass();
    static VariantData* create();

    Variant call2(Method2, Method2, const Variant& other) const;
  // support of LongReal
  public:
    operator cfloatnum() const;
    // this frees x
    Variant(floatnum x, Error e = Success) { move(x, e); };
    void move(floatnum x, Error e = Success);
};

#endif /*_VARIANT_H*/
