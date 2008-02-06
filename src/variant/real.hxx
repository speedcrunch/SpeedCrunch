/* real.hxx: real numbers */
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

#ifndef _REAL_H
#define _REAL_H

#include "math/floatnum.h"

#include "variant/variantdata.hxx"
#include "variant/variant.hxx"
#include <QString>

class LongReal: public VariantData
{
  friend class InitVariant;
  public: // creating & destructing
    LongReal();
    ~LongReal();
    void release();
    VariantData* clone() const;
  public: // type, casting, assignment
    VariantType type() const { return vLongReal; };
    operator cfloatnum() const { return &val; };
    bool move(floatnum x);
    static cfloatnum NaN();
  public: // operators
    Variant operator+() const;
    Variant operator-() const;
    Variant operator+(const Variant& other) const;
    Variant operator-(const Variant& other) const;
    Variant operator*(const Variant& other) const;
    Variant operator/(const Variant& other) const;
    Variant operator%(const Variant& other) const;
    Variant idiv(const Variant& other) const;
    Variant operator==(const Variant& other) const;
    Variant operator!=(const Variant& other) const;
    Variant operator>(const Variant& other) const;
    Variant operator>=(const Variant& other) const;
    Variant operator<(const Variant& other) const;
    Variant operator<=(const Variant& other) const;
    Variant swapSub(const Variant& other) const;
    Variant swapDiv(const Variant& other) const;
    Variant swapIdiv(const Variant& other) const;
    Variant swapMod(const Variant& other) const;
  public: // info
    bool isNaN() const;
    bool isZero() const;
  public: // precision handling
    enum { precQuery = -1, precDefault = 0 };
    static int evalPrec();
    static int workPrec();
    static int precision(int newprec = precQuery);
  public: // conversion, IO
    typedef enum
    {
      FixPoint,
      Scientific,
      Engineering,
      Complement2,
//      IEEE754,
    } FmtMode;
    typedef enum
    {
      Plus,
      Minus,
      Compl2,
      None,
    } Sign;
    typedef struct
    {
      QString intpart;
      QString fracpart;
      QString scale;
      Sign signSignificand;
      Sign signScale;
      char baseSignificand;
      char baseScale;
      Error error;
    } BasicIO;
    bool assign(const char*);
    operator QByteArray() const;
    BasicIO convert(int prec = -1, FmtMode mode = Scientific,
                    char base = 10, char scalebase = 10) const;
    static Variant convert(const BasicIO&);
  private:
    static void initClass();
    static VariantData* create();
    mutable unsigned refcount;
    floatstruct val;
  private: // generic function calls, helpers
    typedef char (*Fct2)(floatnum result, cfloatnum op1,
                         cfloatnum op2, int digits);
    typedef char (*Fct2ND)(floatnum result, cfloatnum op1,
                           cfloatnum op2);
    Variant call2(const Variant& other, Fct2, bool swap = false) const;
    Variant call2ND(const Variant& other, Fct2ND, bool swap = false) const;
    Variant callCmp(const Variant& other, char mask) const;
};

class RealFormat: public FormatIntf
{
  public:
    RealFormat();
    virtual QString format(const VariantData&);
    void setMode(LongReal::FmtMode, int digits = -1,
                 char base = 10, char scalebase = 10, int prec = -1);
    void setGroupChars(QChar dot = '.', QChar group = ',', int grouplg = 0);
  protected:
    virtual QString getSignificandPrefix(LongReal::BasicIO&);
    virtual QString getSignificandSuffix(LongReal::BasicIO&);
    virtual QString getScalePrefix(LongReal::BasicIO&);
    virtual QString getScaleSuffix(LongReal::BasicIO&);
    virtual QString formatNaN();
    virtual QString formatZero();
    virtual QString formatInt(LongReal::BasicIO&);
    virtual QString formatFrac(LongReal::BasicIO&);
    virtual QString formatScale(LongReal::BasicIO&);
  protected:
    // format type
    LongReal::FmtMode mode;
    // radix to what the significand is shown
    char base;
    // radix to what the scale is shown
    char scalebase;
    // allowed error (2*10^(-precision)) in radix conversion
    int  precision;
    // shown digits of the significand
    int  digits;
    // minimum length of the significand
    int  minSignificandLg;
    // minimum length of the scale
    int  minScaleLg;
    // preferred position of the dot
    int  dotpos;
    // length of a digit group
    int  grouplg;
    // dot character
    QChar dot;
    // character for digit grouping
    QChar groupchar;
    // padding character for the significand to achieve minimum length
    char pad;
    // padding character for the scale to achieve minimum length
    char scalePad;
    // truncate the significand instead of round it
    bool truncSignificand;
    // show the scale, even if it is zero
    bool showZeroScale;
    // show the sign, even if the significand is not negative
    bool showPlus;
    // suppress the plus if the scale is not negative
    bool suppressScalePlus;
    // shows the radix tag of the significand
    bool showRadix;
    // suppress the radix tag of the scale
    bool suppressScaleRadix;
    // shows a leading zero before the dot if the integer part is zero
    bool showLeadingZero;
    // suppress trailing zeros in the fractional part
    bool suppressTrailingZero;
    // shows a trailing dot, if the fractional part is zero
    bool showTrailingDot;
    // do not group the digits in the scale
    bool suppressScaleGroups;
    // use lower case hex digits
    bool lowerCaseHexDigit;
    // pad the scale to the right
    bool scaleLeftAlign;
    bool groupSeq() { return groupchar != 0 && grouplg > 0; };
};

#endif /*_REAL_H*/
