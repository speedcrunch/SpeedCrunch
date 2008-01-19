/* numdata.hxx: number classes for use with Variant */
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

#ifndef _NUMDATA_H
#define _NUMDATA_H

#include "base/variant.hxx"
#include "floatnum.h"
#include <QString>

class Floatnum: public Data
{
  public:
    Floatnum();
    ~Floatnum() { float_free(&val); };
    void release();
    Data* clone();
    operator bool() const;
    operator cfloatnum() const { return &val; };
    Data* retype(bool) const;
    Variant operator-() const;
    Variant operator~() const;
    Variant operator+(const Variant& other) const;
    Variant operator-(const Variant& other) const;
    Variant operator*(const Variant& other) const;
    Variant operator/(const Variant& other) const;
    Variant operator%(const Variant& other) const;
    Variant operator&(const Variant& other) const;
    Variant operator|(const Variant& other) const;
    Variant operator^(const Variant& other) const;
    Variant operator>>(const Variant& other) const;
    Variant operator<<(const Variant& other) const;
    Variant operator==(const Variant& other) const;
    Variant operator>(const Variant& other) const;
    Variant operator<(const Variant& other) const;
    Variant swapsub(const Variant& other) const;
    Variant swapdiv(const Variant& other) const;
    Variant swapmod(const Variant& other) const;
    Variant swapshl(const Variant& other) const;
    Variant swapshr(const Variant& other) const;
    bool isZero();
    bool isNaN();
    bool isInteger();
    // 0: default, -1: query; returns old value
    static int precision(int newprec = -1);
    void operator=(int);
    void operator=(const char*);
    // this frees x
    void operator=(floatnum x);
    void operator=(const Variant::LowLevelIO&);
    Variant::LowLevelIO format(Variant::Format, int scale,
                               char base = 10, char expbase = 10) const;
    static cfloatnum NaN() { return &NaNval; };
  protected:
    static bool isFloatnumType(VariantType);
    Variant boolresult(bool result) const;
    Variant realresult(floatnum result) const;
    static cfloatnum asfloatnum(const Variant&);
    Floatnum* createSameType(floatnum val = 0) const;
    static int workprec;
    static int evalprec;
  private:
    typedef char (*Float1ArgND)(floatnum x);
    typedef char (*Float2Args)(floatnum result, cfloatnum p1, cfloatnum p2, int digits);
    typedef char (*Float2ArgsND)(floatnum result, cfloatnum p1, cfloatnum p2);

    int refcount;
    floatstruct val;
    static floatstruct NaNval;

    Variant call2Args(Variant x2, Float2Args func, bool swap = false) const;
    Variant call2ArgsND(Variant x2, Float2ArgsND func, bool swap = false) const;
    Variant call1ArgND(Float1ArgND func) const;
    static void checkfullcancellation(cfloatnum op1, cfloatnum op2, floatnum r);
    static char checkAdd(floatnum dest, cfloatnum s1, cfloatnum s2, int digits);
    static char checkSub(floatnum dest, cfloatnum s1, cfloatnum s2, int digits);
};

class Real: public Floatnum
{
  public:
    VariantType type() { return vReal; };
    static Data* createReal() { return new Real; };
};

#endif /* _NUMDATA_H */
