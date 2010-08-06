/* real.h: real numbers */
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

#include "math/floathmath.h"

#include "variant/variantbase.h"

typedef enum
{
  // try to keep the size of the fractional part fixed
  FixPoint,
  // try to keep the overall length of the output fixed
  FixSize,
  // scientific format with normalized significand
  Scientific,
  // scientific format with biased exponent
  IEEE754,
  // scientific format with exponent adjusted to be a multiple of 3
  Engineering,
  // two's complement
  Complement2,
  FmtLastMode = Complement2,
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
  Sign signScale;
  Error error;
} RawFloatIO;

class LongReal: public VariantData
{
  friend class InitVariant;
  public: // creating & destructing
    LongReal() { float_create(&val); };
    ~LongReal() { float_free(&val); };

    // post: sets the argument to NaN
    static LongReal* create(floatnum);
  public: // type, casting, assignment
    VariantType type() const { return vtLongReal; };
    operator cfloatnum() const { return &val; };
    operator double() const;
    void operator = (double);
    bool move(floatnum x);
    static cfloatnum NaN();
  public: // info
    bool isNaN() const;
    bool isZero() const;
  public: // precision handling
    enum { PrecQuery = -1, PrecDefault = 0 };
    enum EvalMode { EvalExact, EvalRelaxed, EvalQuery };

    static int evalPrec();
    static int workPrec();
    static int precision(int newprec = PrecQuery);
    static EvalMode evalMode(EvalMode newMode = EvalQuery);
  public: // conversion, IO
    RawFloatIO convert(int digits, FmtMode mode = Scientific,
                       char base = 10) const;
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

#endif /*_REAL_H*/
