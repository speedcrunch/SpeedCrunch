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
#include "math/floathmath.h"

#include "variant/variantbase.hxx"
#include "variant/variant.hxx"
#include <QString>

typedef enum
{
  FixPoint,
  Scientific,
  Engineering,
  Complement2,
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
  Sign signSignificand;
  char baseSignificand;
  unsigned scale;
  char baseScale;
  Sign signScale;
  Error error;
} RawFloatIO;

class LongReal: public VariantRefData
{
  friend class InitVariant;
  public: // creating & destructing
    LongReal() { float_create(&val); };
    ~LongReal() { float_free(&val); };
    static LongReal* create(floatnum);
  public: // type, casting, assignment
    VariantType type() const { return vtLongReal; };
    operator cfloatnum() const { return &val; };
    bool move(floatnum x);
    static cfloatnum NaN();
  public: // info
    bool isNaN() const;
    bool isZero() const;
  public: // precision handling
    typedef enum { PrecQuery = -1, PrecDefault = 0 };
    typedef enum { EvalExact, EvalRelaxed, EvalQuery } EvalMode;

    static int evalPrec();
    static int workPrec();
    static int precision(int newprec = PrecQuery);
    static EvalMode evalMode(EvalMode newMode = EvalQuery);
  public: // conversion, IO
    RawFloatIO convert(int digits, FmtMode mode = Scientific,
                    char base = 10, char scalebase = 10) const;
    static Variant convert(const RawFloatIO&);
    void xmlWrite(QDomDocument&, QDomNode&) const;
    bool xmlRead(QDomNode&);
  private:
    floatstruct val;
    static void initClass();
    static VariantType vtLongReal;
    static VariantData* create();
  private: // generic function calls, helpers
};

class RealFormat: public FormatIntf
{
  public:
    typedef enum
    {
      fShowZeroScale = 1,
      fShowPlus = 2,
      fShowScalePlus = 4,
      fShowRadix = 8,
      fShowScaleRadix = 0x10,
      fShowLeadingZero = 0x20,
      fShowScaleLeadingZero = 0x40,
      fShowTrailingZero = 0x80,
      fShowTrailingDot = 0x100,
      fLowerCaseDigit = 0x200,
    };
  public:
    RealFormat();
    QString format(const Variant&);
    void setMode(FmtMode, int digits = -1,
                 char base = 10, char scalebase = 10);
    void setGroupChars(QChar dot = '.', QChar group = ',', int grouplg = 0);
    void setMinLengths(int newMinInt = 0, int newMinFrac = 0,
                       int newMinScale = 0);
    void setFlags(unsigned flags);
  protected:
    virtual QString getSignificandPrefix(RawFloatIO&);
    virtual QString getSignificandSuffix(RawFloatIO&);
    virtual QString getScalePrefix(RawFloatIO&);
    virtual QString getScaleSuffix(RawFloatIO&);
    virtual QString formatNaN();
    virtual QString formatZero();
    virtual QString formatInt(RawFloatIO&);
    virtual QString formatFrac(RawFloatIO&);
    virtual QString formatScale(RawFloatIO&);
  protected:
    // format type
    FmtMode mode;
    // radix to what the significand is shown
    char base;
    // radix to what the scale is shown
    char scalebase;
    // shown digits of the significand after the dot
    int  digits;
    // minimum length of the integer part
    int  minIntLg;
    // minimum length of the fraction part
    int  minFracLg;
    // minimum length of the scale
    int  minScaleLg;
    // length of a digit group
    int  grouplg;
    // dot character
    QChar dot;
    // character for digit grouping
    QChar groupchar;
    // show the scale, even if it is zero
    bool showZeroScale;
    // show the sign, even if the significand is not negative
    bool showPlus;
    // show the plus sign if the scale is not negative
    bool showScalePlus;
    // shows the radix tag of the significand
    bool showRadix;
    // suppress the radix tag of the scale
    bool showScaleRadix;
    // shows leading zeros to pad the integer part to the minimum size
    bool showLeadingZero;
    // shows leading zeros to pad the scale to the minimum size
    bool showScaleLeadingZero;
    // show trailing zeros in the fractional part
    bool showTrailingZero;
    // shows a trailing dot, if the fractional part is zero
    bool showTrailingDot;
    // use lower case hex digits
    bool lowerCaseHexDigit;
    // helpers
    bool useGrouping() { return groupchar != 0 && grouplg > 0; };
    bool useScale(const RawFloatIO&);
    QString getPrefix(Sign, char base, bool isCompl);
};

#endif /*_REAL_H*/
