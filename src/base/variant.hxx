/* variant.hxx: general value type used throughout SpeedCrunch*/
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

#include "math/floatnum.h"
#include "main/errors.h"
#include <QString>
#include <QList>

typedef enum
// the order is important, because it controls the retyping.
// later types know how to convert from earlier ones
{
  vBoolean,  // has to be the first
  vText,
  vInteger,
  vReal,     // real numbers
  vEmpty,    // has to be the last but one
  vError     // sentinel value, has to be the last
} VariantType;

class Variant;

class Data
{
  public:
    typedef Data*(*Constructor)();
    typedef Constructor Constructors[int(vError)];
    static Constructors constructors;
    // virtual constructor
    static Data* create(VariantType);
    virtual ~Data() { };
    virtual VariantType type() const { return vEmpty; };
    virtual void release() { delete this; };
    virtual Data* clone() = 0;
    virtual operator bool() const { return true; };
    virtual Data* retype(VariantType) const { return 0; };
    virtual Data* retype(Data*) const { return 0; };
    virtual Data* retype(bool) const { return 0; };
    virtual Variant operator+() const;
    virtual Variant operator-() const;
    virtual Variant operator~() const;
    virtual Variant operator!() const;
    virtual Variant operator+(const Variant& other) const;
    virtual Variant operator-(const Variant& other) const;
    virtual Variant operator*(const Variant& other) const;
    virtual Variant operator/(const Variant& other) const;
    virtual Variant operator%(const Variant& other) const;
    virtual Variant operator&(const Variant& other) const;
    virtual Variant operator|(const Variant& other) const;
    virtual Variant operator^(const Variant& other) const;
    virtual Variant operator<<(const Variant& other) const;
    virtual Variant operator>>(const Variant& other) const;
    virtual Variant operator==(const Variant& other) const;
    virtual Variant operator!=(const Variant& other) const;
    virtual Variant operator>=(const Variant& other) const;
    virtual Variant operator<=(const Variant& other) const;
    virtual Variant operator>(const Variant& other) const;
    virtual Variant operator<(const Variant& other) const;
    virtual Variant swapadd(const Variant& other) const;
    virtual Variant swapsub(const Variant& other) const;
    virtual Variant swapmul(const Variant& other) const;
    virtual Variant swapdiv(const Variant& other) const;
    virtual Variant swapmod(const Variant& other) const;
    virtual Variant swapand(const Variant& other) const;
    virtual Variant swapor(const Variant& other) const;
    virtual Variant swapxor(const Variant& other) const;
    virtual Variant swapshl(const Variant& other) const;
    virtual Variant swapshr(const Variant& other) const;
    virtual Variant swapeq(const Variant& other) const;
    virtual Variant swapne(const Variant& other) const;
    virtual Variant swapge(const Variant& other) const;
    virtual Variant swaple(const Variant& other) const;
    virtual Variant swapgt(const Variant& other) const;
    virtual Variant swapls(const Variant& other) const;
    virtual bool isZero() { return false; };
    virtual bool isNaN() { return true; };
    virtual bool isInteger() { return false; };
};

class VariantBase
{
  public:
    VariantType type() const { return m_type; };
  protected:
    VariantBase() : m_type(vEmpty) {};
    VariantType m_type;
};

class Variant: public VariantBase
{
  public:
    Variant() {};
    Variant(bool val) { *this = val; };
    Variant(Error error) { *this = error; };
    Variant(Data* d) { *this = d; };
    Variant(const Variant& other) { *this = other; };
    void operator=(Error error);
    void operator=(bool val);
    void operator=(const Variant& other);
    void operator=(Data* other);
    operator Error() const;
    operator bool() const;
    operator const Data*() const;
    Variant as(VariantType);
    // in place retyping, result is true on success
    bool retype(VariantType);
    Variant operator+() const;
    Variant operator-() const;
    Variant operator~() const;
    Variant operator!() const;
    Variant operator+(const Variant&) const;
    Variant operator-(const Variant&) const;
    Variant operator*(const Variant&) const;
    Variant operator/(const Variant&) const;
    Variant operator%(const Variant&) const;
    Variant operator&(const Variant&) const;
    Variant operator|(const Variant&) const;
    Variant operator^(const Variant&) const;
    Variant operator<<(const Variant&) const;
    Variant operator>>(const Variant&) const;
    Variant operator==(const Variant&) const;
    Variant operator!=(const Variant&) const;
    Variant operator>=(const Variant&) const;
    Variant operator<=(const Variant&) const;
    Variant operator>(const Variant&) const;
    Variant operator<(const Variant&) const;
    void operator+=(const Variant& other) { *this = *this + other; };
    void operator-=(const Variant& other) { *this = *this - other; };
    void operator*=(const Variant& other) { *this = *this * other; };
    void operator/=(const Variant& other) { *this = *this / other; };
    bool isZero();
    bool isNaN();
    bool isInteger();
  protected:
    union
    {
      bool boolval;
      Error error;
      Data* val;
    };
    Variant(VariantType aType) { *this = aType; };
    void operator=(VariantType aType);
    void clear();
    bool builtinType() const;
  private:
    Error checkError(const Variant& other, bool checkBool = true) const;
    bool setType(Data* newval);
    Data* convert(VariantType newtype);

  // support of Real
  public:
    typedef enum
    {
      Plus,
      Minus,
      Compl2,
    } Sign;
    typedef enum
    {
      Scientific,
      Fixpoint,
      Engineering,
      Complement2,
    } Format;
    typedef struct
    {
      QString intpart;
      QString fracpart;
      QString scale;
      Sign signSignificand;
      Sign signScale;
      signed char baseSignificand;
      signed char baseScale;
    } LowLevelIO;

    // 0: default, -1: query; returns old value
    static int precision(int newprec = -1);
    void operator=(int);
    void operator=(const char*);
    // this frees x
    void operator=(floatnum x);
    void operator=(const LowLevelIO&);
    operator cfloatnum() const;
    Variant(int x) { *this = x; };
    Variant(const char* s) { *this = s; };
    Variant(const LowLevelIO& io) { *this = io; };
    LowLevelIO format(Format, int scale,
                      char base = 10, char expbase = 10) const;
};

#endif /* _VARIANT_H */
