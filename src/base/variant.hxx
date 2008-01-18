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
{
  vEmpty,
  vError,
  vBoolean,
  vInteger,
  vReal,
  vText,
} VariantType;

class Variant;

class Data
{
  public:
    // virtual constructor
    static Data* create(VariantType);
    virtual ~Data() { };
    virtual void release() = 0;
    virtual Data* clone() const = 0;
    virtual operator bool() const { return true; };
    virtual Data* retype(VariantType) const { return 0; };
    virtual Data* retype(Data*) const { return 0; };
    virtual Data* retype(bool) const { return 0; };
    virtual Variant operator+() const;
    virtual Variant operator-() const;
    virtual Variant operator~() const;
    virtual Variant operator!() const;
    virtual Variant operator+(const Variant other) const;
    virtual Variant operator-(const Variant other) const;
    virtual Variant operator*(const Variant other) const;
    virtual Variant operator/(const Variant other) const;
    virtual Variant operator%(const Variant other) const;
    virtual Variant operator&(const Variant other) const;
    virtual Variant operator|(const Variant other) const;
    virtual Variant operator^(const Variant other) const;
    virtual Variant operator==(const Variant other) const;
    virtual Variant operator!=(const Variant other) const;
    virtual Variant operator>=(const Variant other) const;
    virtual Variant operator<=(const Variant other) const;
    virtual Variant operator>(const Variant other) const;
    virtual Variant operator<(const Variant other) const;
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
    Variant(const Variant& other) { *this = other; };
    void operator=(Error error);
    void operator=(bool val);
    void operator=(const Variant& other);
    operator Error() const;
    operator bool() const;
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
    Variant operator==(const Variant other) const;
    Variant operator!=(const Variant other) const;
    Variant operator>=(const Variant other) const;
    Variant operator<=(const Variant other) const;
    Variant operator>(const Variant other) const;
    Variant operator<(const Variant other) const;
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
    bool setType(VariantType newType, Data* newval);
    Data* convert(VariantType newtype);
};

#endif /* _VARIANT_H */
