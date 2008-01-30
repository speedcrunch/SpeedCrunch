// HMath: C++ high precision math routines
// Copyright (C) 2004 Ariya Hidayat <ariya.hidayat@gmail.com>
// Copyright (C) 2007-2008 Helder Correia <helder.pereira.correia@gmail.com>
// Copyright (C) 2008 Wolf Lammen
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.


#include "hmath.hxx"

#include "floatcommon.h"
#include "floatconst.h"
#include "floatconvert.h"
#include "floathmath.h"
#include "floatio.h"
#include "floatnum.h"

#include "base/errors.h"

#include <ctype.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define HMATH_WORKING_PREC (DECPRECISION + 3)
#define HMATH_EVAL_PREC (HMATH_WORKING_PREC + 2)

// default scale for fall back in formatting
#define HMATH_MAX_SHOWN 20
#define HMATH_BIN_MAX_SHOWN ((33219*HMATH_MAX_SHOWN)/10000 + 1)
#define HMATH_OCT_MAX_SHOWN ((11073*HMATH_MAX_SHOWN)/10000 + 1)
#define HMATH_HEX_MAX_SHOWN ((8305*HMATH_MAX_SHOWN)/10000 + 1)


/*------------------------   Helper routines  -------------------------*/

void static checkfullcancellation( cfloatnum op1, cfloatnum op2,
                                   floatnum r )
{
  int expr;

  if (float_getlength(op1) != 0
      && float_getlength(op2) != 0
      && float_getlength(r) != 0)
  {
    /* NaN or zero not involved in computation */
    expr = float_getexponent(r);
    if (float_getexponent(op1) - expr >= HMATH_WORKING_PREC - 1
        || float_getexponent(op2) - expr >= HMATH_WORKING_PREC - 1)
      float_setzero(r);
  }
}

static char checkAdd(floatnum dest, cfloatnum s1, cfloatnum s2, int digits)
{
  if (float_add(dest, s1, s2, digits)
      && float_getsign(s1) + float_getsign(s2) == 0)
    checkfullcancellation(s1, s2, dest);
  return float_isnan(dest);
}

static char checkSub(floatnum dest, cfloatnum s1, cfloatnum s2, int digits)
{
  if (float_sub(dest, s1, s2, digits)
      && float_getsign(s1) - float_getsign(s2) == 0)
    checkfullcancellation(s1, s2, dest);
  return float_isnan(dest);
}

static void h_init()
{
  static bool h_initialized = false;
  if( !h_initialized )
  {
    h_initialized = true;
    floatmath_init();
    float_stdconvert();
  }
}

static void
checkpoleorzero( floatnum result, floatnum x )
{
  int expx, expr;

  if (float_getlength(result) == 0 || float_getlength(x) == 0)
    return;
  expx = float_getexponent(x)-HMATH_WORKING_PREC+1;
  expr = float_getexponent(result);
  if (expr <= expx)
    float_setzero(result);
  else if (expr >= -expx)
  {
    float_setnan(result);
    float_seterror(EvalUnstable);
  }
}

static char roundResult( floatnum x )
{
  if (float_isnan(x)) /* avoids setting float_error */
    return 0;
  return float_round(x, x, HMATH_WORKING_PREC, TONEAREST);
}

/*---------------------------   HNumberPrivate   --------------------*/

class HNumberPrivate
{
public:
  HNumberPrivate();
  ~HNumberPrivate();
  floatstruct fnum;
  Error error;
  char format;
};

HNumberPrivate::HNumberPrivate()
  : error(Success), format(0)
{
  h_init();
  float_create(&fnum);
}

HNumberPrivate::~HNumberPrivate()
{
  float_free(&fnum);
}

typedef char (*Float1ArgND)(floatnum x);
typedef char (*Float1Arg)(floatnum x, int digits);
typedef char (*Float2ArgsND)(floatnum result, cfloatnum p1, cfloatnum p2);
typedef char (*Float2Args)(floatnum result, cfloatnum p1, cfloatnum p2, int digits);

static Error checkNaNParam(const HNumberPrivate& v1,
                            const HNumberPrivate* v2 = 0)
{
  if ( !float_isnan(&v1.fnum) && (!v2 || !float_isnan(&v2->fnum)))
    return Success;
  Error error = v1.error;
  if ( error == Success && v2 )
    error = v2->error;
  return error == 0? NoOperand : error;
}

void roundSetError(HNumberPrivate* dest)
{
  dest->error = float_geterror();
  floatnum dfnum = &dest->fnum;
  if (dest->error != Success)
    float_setnan(dfnum);
  if (!float_isnan(dfnum))
    float_round(dfnum, dfnum, HMATH_WORKING_PREC, TONEAREST);
}

void call2Args(HNumberPrivate* dest, HNumberPrivate* n1,
               HNumberPrivate* n2, Float2Args func)
{
  dest->error = checkNaNParam(*n1, n2);
  if (dest->error == Success)
  {
    floatnum dfnum = &dest->fnum;
    func(dfnum, &n1->fnum, &n2->fnum, HMATH_EVAL_PREC);
    roundSetError(dest);
  }
}

void call2ArgsND(HNumberPrivate* dest, HNumberPrivate* n1,
               HNumberPrivate* n2, Float2ArgsND func)
{
  dest->error = checkNaNParam(*n1, n2);
  if (dest->error == Success)
  {
    floatnum dfnum = &dest->fnum;
    func(dfnum, &n1->fnum, &n2->fnum);
    roundSetError(dest);
  }
}

void call1Arg(HNumberPrivate* dest, HNumberPrivate* n, Float1Arg func)
{
  dest->error = checkNaNParam(*n);
  if (dest->error == Success)
  {
    floatnum dfnum = &dest->fnum;
    float_copy(dfnum, &n->fnum, HMATH_EVAL_PREC);
    func(dfnum, HMATH_EVAL_PREC);
    roundSetError(dest);
  }
}

void call1ArgPoleCheck(HNumberPrivate* dest, HNumberPrivate* n, Float1Arg func)
{
  dest->error = checkNaNParam(*n);
  if (dest->error == Success)
  {
    floatnum dfnum = &dest->fnum;
    float_copy(dfnum, &n->fnum, HMATH_EVAL_PREC);
    if (func(dfnum, HMATH_EVAL_PREC))
      checkpoleorzero(dfnum, &n->fnum);
    roundSetError(dest);
  }
}

void call1ArgND(HNumberPrivate* dest, HNumberPrivate* n, Float1ArgND func)
{
  dest->error = checkNaNParam(*n);
  if (dest->error == Success)
  {
    floatnum dfnum = &dest->fnum;
    float_copy(dfnum, &n->fnum, HMATH_EVAL_PREC);
    func(dfnum);
    roundSetError(dest);
  }
}

static char modwrap(floatnum result, cfloatnum p1, cfloatnum p2, int digits)
{
  char ok;
  floatstruct tmp;
  float_create(&tmp);
  ok = float_divmod(&tmp, result, p1, p2, INTQUOT);
  float_free(&tmp);
  return ok;
}

char idivwrap(floatnum result, cfloatnum p1, cfloatnum p2)
{
  char ok;
  floatstruct tmp;
  int save = float_setprecision(DECPRECISION);
  float_create(&tmp);
  ok = float_divmod(result, &tmp, p1, p2, INTQUOT);
  float_free(&tmp);
  float_setprecision(save);
  return ok;
}

/*--------------------------   HNumber   -------------------*/

/**
 * Creates a new number.
 */
HNumber::HNumber()
{
  d = new HNumberPrivate;
}

/**
 * Copies from another number.
 */
HNumber::HNumber( const HNumber& hn )
{
  d = new HNumberPrivate;
  operator=( hn );
}

/**
 * Creates a new number from an integer value.
 */
HNumber::HNumber( int i )
{
  d = new HNumberPrivate;
  float_setinteger(&d->fnum, i);
}

/**
 * Creates a new number from a string.
 */
HNumber::HNumber( const char* str )
{
  t_itokens tokens;

  d = new HNumberPrivate;
  if ((d->error = parse(&tokens, &str)) == Success && *str == 0)
    d->error = float_in(&d->fnum, &tokens);
  float_geterror();
}

/**
 * Destroys this number.
 */
HNumber::~HNumber()
{
  delete d;
}

/**
 * Returns the error code kept with a NaN.
 */
Error HNumber::error() const
{
  return d->error;
}

/**
 * Returns true if this number is Not a Number (NaN).
 */
bool HNumber::isNan() const
{
  return float_isnan(&d->fnum) != 0;
}

/**
 * Returns true if this number is zero.
 */
bool HNumber::isZero() const
{
  return float_iszero(&d->fnum) != 0;
}

/**
 * Returns true if this number is more than zero.
 */
bool HNumber::isPositive() const
{
  return float_getsign(&d->fnum) > 0;
}

/**
 * Returns true if this number is less than zero.
 */
bool HNumber::isNegative() const
{
  return float_getsign(&d->fnum) < 0;
}

/**
 * Returns true if this number is integer.
 */
bool HNumber::isInteger() const
{
  return float_isinteger(&d->fnum) != 0;
}

/**
 * Returns the preferred format (base/precision), default is 0
 */
char HNumber::format() const
{
   return d->format;
}

/**
 * Sets the preferred format (base/precision), default is 0
 */
void HNumber::setFormat(char c)
{
   d->format = float_isnan(&d->fnum)?0:c;
}

/**
 * Returns a NaN (Not a Number) with error set to
 * passed parameter.
 */
HNumber HNumber::nan(Error error)
{
  HNumber result;
  result.d->error = error;
  return result;
}

/**
 * Returns the number as an int.
 * It is meant to convert small (integer) numbers only and no
 * checking is done whatsoever.
 */
int HNumber::toInt() const
{
  return float_asinteger(&d->fnum);
}

/**
 * Assigns from another number.
 */
HNumber& HNumber::operator=( const HNumber & hn )
{
  d->format = hn.format();
  d->error = hn.error();
  float_copy(&d->fnum, &hn.d->fnum, EXACT);
  return *this;
}

/**
 * Adds another number.
 */
HNumber HNumber::operator+( const HNumber & num ) const
{
  HNumber result;
  call2Args(result.d, d, num.d, checkAdd);
  return result;
}

/**
 * Adds another number.
 */
HNumber& HNumber::operator+=( const HNumber & num )
{
  return operator=( *this + num );
}

/**
 * Subtract from another number.
 */
HNumber operator-( const HNumber & n1, const HNumber & n2 )
{
  HNumber result;
  call2Args(result.d, n1.d, n2.d, checkSub);
  return result;
}

/**
 * Subtract from another number.
 */
HNumber& HNumber::operator-=( const HNumber & num )
{
  return operator=( *this - num );
}

/**
 * Multiplies with another number.
 */
HNumber HNumber::operator*( const HNumber & num ) const
{
  HNumber result;
  call2Args(result.d, d, num.d, float_mul);
  return result;
}

/**
 * Multiplies with another number.
 */
HNumber& HNumber::operator*=( const HNumber & num )
{
  return operator=( *this * num );
}

/**
 * Divides with another number.
 */
HNumber HNumber::operator/( const HNumber & num ) const
{
  HNumber result;
  call2Args(result.d, d, num.d, float_div);
  return result;
}

/**
 * Divides with another number.
 */
HNumber& HNumber::operator/=( const HNumber & num )
{
  return operator=( *this / num );
}

/**
 * Modulo (rest of integer division)
 */
HNumber HNumber::operator%( const HNumber & num ) const
{
  HNumber result;
  call2Args(result.d, d, num.d, modwrap);
  return result;
}

/**
 * Performs an integer divide
 */
HNumber HNumber::idiv( const HNumber & dividend, const HNumber& divisor)
{
  HNumber result;
  call2ArgsND(result.d, dividend.d, divisor.d, idivwrap);
  if (result.error() == TooExpensive)
    result.d->error = Overflow;
  return result;
}

/**
 * Returns true if this number is greater than n.
 */
bool HNumber::operator>( const HNumber & n ) const
{
  return HMath::compare( *this, n ) > 0;
}

/**
 * Returns true if this number is less than n.
 */
bool HNumber::operator<( const HNumber & n ) const
{
  return HMath::compare( *this, n ) < 0;
}

/**
 * Returns true if this number is greater than or equal to n.
 */
bool HNumber::operator>=( const HNumber & n ) const
{
  return HMath::compare( *this, n ) >= 0;
}

/**
 * Returns true if this number is less than or equal to n.
 */
bool HNumber::operator<=( const HNumber & n ) const
{
  return HMath::compare( *this, n ) <= 0;
}

/**
 * Returns true if this number is equal to n.
 */
bool HNumber::operator==( const HNumber & n ) const
{
  return HMath::compare( *this, n ) == 0;
}

/**
 * Returns true if this number is not equal to n.
 */
bool HNumber::operator!=( const HNumber & n ) const
{
  return HMath::compare( *this, n ) != 0;
}

/**
 * Bitwise ANDs the integral parts of both operands.
 * Yields NaN, if any operand exeeds the logic range
 */
HNumber HNumber::operator&( const HNumber & num ) const
{
  HNumber result;
  call2ArgsND(result.d, d, num.d, float_and);
  return result;
}

/**
 * Bitwise ANDs the integral parts of both operands.
 * Yields NaN, if any operand exeeds the logic range
 */
HNumber& HNumber::operator&=( const HNumber & num )
{
  return operator=( *this & num );
}

/**
 * Bitwise ORs the integral parts of both operands.
 * Yields NaN, if any operand exeeds the logic range
 */
HNumber HNumber::operator|( const HNumber & num ) const
{
  HNumber result;
  call2ArgsND(result.d, d, num.d, float_or);
  return result;
}

/**
 * Bitwise ORs the integral parts of both operands.
 * Yields NaN, if any operand exeeds the logic range
 */
HNumber& HNumber::operator|=( const HNumber & num )
{
  return operator=( *this | num );
}

/**
 * Bitwise XORs the integral parts of both operands.
 * Yields NaN, if any operand exeeds the logic range
 */
HNumber HNumber::operator^( const HNumber & num ) const
{
  HNumber result;
  call2ArgsND(result.d, d, num.d, float_xor);
  return result;
}

/**
 * Bitwise XORs the integral parts of both operands.
 * Yields NaN, if any operand exeeds the logic range
 */
HNumber& HNumber::operator^=( const HNumber& num )
{
  return operator=( *this ^ num );
}

/**
 * Bitwise NOTs the integral part *this.
 * Yields NaN, if *this exeeds the logic range
 */
HNumber HNumber::operator~() const
{
  HNumber result;
  call1ArgND(result.d, d, float_not);
  return result;
}


/**
 * Changes the sign.
 */
HNumber operator-( const HNumber & x )
{
  HNumber result;
  call1ArgND(result.d, x.d, float_neg);
  return result;
}

/**
 * Shifts the integral part of <*this> to the left by
 * the parameters value's bits. Zeros are shifted in
 * to the right, shifted out bits are dropped.
 * Yields NaN, if the operand exeeds the logic range,
 * or the shift count is not a non-negative integer.
 */
HNumber HNumber::operator<<( const HNumber & num ) const
{
  HNumber result;
  call2ArgsND(result.d, d, num.d, float_shl);
  return result;
}

/**
 * Shifts the integral part of <*this> to the right by
 * the parameters value's bits. The most significand
 * bit is duplicated to the left, shifted out bits are dropped
 * (signed or arithmethic shift right).
 * Yields NaN, if the operand exeeds the logic range,
 * or the shift count is not a non-negative integer.
 */
HNumber HNumber::operator>>( const HNumber & num ) const
{
  HNumber result;
  call2ArgsND(result.d, d, num.d, float_shr);
  return result;
}

static char*
_doFormat(
  floatnum x,
  signed char base,
  signed char expbase,
  char outmode,
  int prec,
  unsigned flags)
{
  floatstruct tmp;
  t_otokens tokens;
  int sz;
  char* str;
  char intbuf[BINPRECISION+1];
  char fracbuf[BINPRECISION+1];
  char expbuf[BITS_IN_EXP+1];

  str = NULL;
  float_create(&tmp);
  switch (base)
  {
  case 2:
    sz = BINPRECISION+1;
    break;
  case 8:
    sz = OCTPRECISION+1;
    break;
  case 10:
    sz = DECPRECISION+1;
    break;
  case 16:
    sz = HEXPRECISION+1;
    break;
  }
  tokens.intpart.sz = sz;
  tokens.intpart.buf = intbuf;
  tokens.fracpart.sz = sz;
  tokens.fracpart.buf = fracbuf;
  tokens.exp.sz = sizeof(expbuf);
  tokens.exp.buf = expbuf;
  float_copy(&tmp, x, DECPRECISION + 2);
  if (float_out(&tokens, &tmp, prec, base, expbase, outmode) == Success)
  {
    sz = cattokens(NULL, -1, &tokens, flags);
    str = (char*)malloc( sz );
    cattokens(str, sz, &tokens, flags);
  }
  float_free(&tmp);
  return str;
}


/**
 * Formats the given number as string, using specified decimal digits.
 * Note that the returned string must be freed.
 */
char * HMath::formatFixed( const HNumber & hn, int prec )
{
  unsigned flags;
  int scale;
  char* result;

  scale = float_getlength(&hn.d->fnum) - float_getexponent(&hn.d->fnum) - 1;
  if (scale < 0)
    scale = 0;
  flags = IO_FLAG_SUPPRESS_PLUS + IO_FLAG_SUPPRESS_DOT;
  if( prec < 0 )
  {
    flags |= IO_FLAG_SUPPRESS_TRL_ZERO;
    prec = HMATH_MAX_SHOWN;
    if( scale < HMATH_MAX_SHOWN )
      prec = scale;
  }
  result = _doFormat(&hn.d->fnum, 10, 10, IO_MODE_FIXPOINT, prec,
                     flags);
  if (!result)
    result = _doFormat(&hn.d->fnum, 10, 10, IO_MODE_SCIENTIFIC,
                       HMATH_MAX_SHOWN, flags);
  return result;
}


/**
 * Formats the given number as string, in scientific format.
 * Note that the returned string must be freed.
 */
char * HMath::formatScientific( const HNumber & hn, int prec )
{
  unsigned flags;

  flags = IO_FLAG_SUPPRESS_PLUS + IO_FLAG_SUPPRESS_DOT
      + IO_FLAG_SUPPRESS_EXPPLUS;
  if( prec < 0 )
  {
    flags |= IO_FLAG_SUPPRESS_TRL_ZERO;
    prec = HMATH_MAX_SHOWN;
  }
  return _doFormat(&hn.d->fnum, 10, 10, IO_MODE_SCIENTIFIC, prec,
                   flags);
}

/**
 * Formats the given number as string, in engineering notation.
 * Note that the returned string must be freed.
 */
char * HMath::formatEngineering( const HNumber & hn, int prec )
{
  unsigned flags;

  flags = IO_FLAG_SUPPRESS_PLUS + IO_FLAG_SUPPRESS_EXPPLUS;
  if( prec <= 1 )
  {
    flags |= IO_FLAG_SUPPRESS_TRL_ZERO + IO_FLAG_SUPPRESS_DOT;
    prec = HMATH_MAX_SHOWN;
  }
  return _doFormat(&hn.d->fnum, 10, 10, IO_MODE_ENG, prec,
                   flags);
}


/**
 * Formats the given number as string, using specified decimal digits.
 * Note that the returned string must be freed.
 */
char * HMath::formatGeneral( const HNumber & hn, int prec )
{
  // find the exponent and the factor
  int expd = float_getexponent(&hn.d->fnum);

  char* str;
  if( expd > 5 )
    str = formatScientific( hn, prec );
  else if( expd < -4 )
    str = formatScientific( hn, prec );
  else if ( (expd < 0) && (prec>0) && (expd < -prec) )
    str = formatScientific( hn, prec );
  else
    str = formatFixed( hn, prec );

  return str;
}

char* formathexfp( floatnum x, char base,
                   char expbase, int scale )
{
  char* result;
  int tmpscale;

#if 0

  // SpeedCrunch 0.8 behaviour

  floatstruct tmp;
  int exp;

  float_create(&tmp);
  exp = float_getexponent(x);
  float_copy(&tmp, x, exp <= 0? 1 : exp + 1);
  float_int(&tmp);
  result = _doFormat(&tmp, base, expbase, IO_MODE_FIXPOINT,
                     0, IO_FLAG_SUPPRESS_PLUS
                     + IO_FLAG_SUPPRESS_DOT + IO_FLAG_SHOW_BASE);
  float_free(&tmp);

#else
  tmpscale = scale;
  if (float_isinteger(x))
    tmpscale = 0;
  result = _doFormat(x, base, expbase, IO_MODE_FIXPOINT,
                     tmpscale, IO_FLAG_SUPPRESS_PLUS
                     + IO_FLAG_SUPPRESS_DOT + IO_FLAG_SHOW_BASE);
#endif
  if (result != NULL)
    return result;

  return _doFormat(x, base, expbase, IO_MODE_SCIENTIFIC,
                    scale,
                    IO_FLAG_SUPPRESS_PLUS + IO_FLAG_SUPPRESS_DOT
                    + IO_FLAG_SHOW_BASE + IO_FLAG_SHOW_EXPBASE);
}


/**
 * Formats the given number as string, using hexadecimal digits.
 * Note that the returned string must be freed.
 */
char * HMath::formatHexadec( const HNumber & hn )
{
  return formathexfp (&hn.d->fnum, 16, 10, HMATH_HEX_MAX_SHOWN);
}


/**
 * Formats the given number as string, using octal digits.
 * Note that the returned string must be freed.
 */
char * HMath::formatOctal( const HNumber & hn )
{
  return formathexfp (&hn.d->fnum, 8, 10, HMATH_OCT_MAX_SHOWN);
}


/**
 * Formats the given number as string, using binary digits.
 * Note that the returned string must be freed.
 */
char * HMath::formatBinary( const HNumber & hn )
{
  return formathexfp (&hn.d->fnum, 2, 10, HMATH_BIN_MAX_SHOWN);
}


/**
 * Formats the given number as string, using specified decimal digits.
 * Note that the returned string must be freed.
 */
char* HMath::format( const HNumber& hn, char format, int prec )
{
  if( format=='g' )
    return formatGeneral( hn, prec );
  else if( format=='f' )
    return formatFixed( hn, prec );
  else if( format=='e' )
    return formatScientific( hn, prec );
  else if( format=='n' )
    return formatEngineering( hn, prec );
  else if( format=='h' )
    return formatHexadec( hn );
  else if( format=='o' )
    return formatOctal( hn );
  else if( format=='b' )
    return formatBinary( hn );

  // fallback to 'g'
  return formatGeneral( hn, prec );
}


/**
 * Converts radians to degrees.
 */
HNumber HMath::rad2deg( const HNumber & angle )
{
  return angle * (HNumber(180) / HMath::pi());
}


/**
 * Converts degrees to radians.
 */
HNumber HMath::deg2rad( const HNumber & angle )
{
  return angle * (HMath::pi() / HNumber(180));
}


/**
 * Returns the constant pi.
 */
HNumber HMath::pi()
{
  HNumber value;

  float_copy(&value.d->fnum, &cPi, HMATH_EVAL_PREC);
  return value;
}


/**
 * Returns the constant phi (golden number).
 */
HNumber HMath::phi()
{
  return HNumber("1.61803398874989484820458683436563811772030917980576"
                   "28621354486227052604628189024497072072041893911374"
                   "84754088075386891752");
}


/**
 * Returns -1, 0, 1 if n1 is less than, equal to, or more than n2.
 */
int HMath::compare( const HNumber & n1, const HNumber & n2 )
{
  int result = float_relcmp(&n1.d->fnum, &n2.d->fnum, HMATH_EVAL_PREC-1);
  float_geterror(); // clears error, if one operand was a NaN
  return result;
}


/**
 * Returns the maximum of two numbers.
 */
HNumber HMath::max( const HNumber & n1, const HNumber & n2 )
{
  switch ( float_cmp(&n1.d->fnum, &n2.d->fnum) )
  {
    case 0:
    case 1:  return n1;
    case -1: return n2;
    default: return HNumber::nan(checkNaNParam(*n1.d, n2.d));
  }
}


/**
 * Returns the minimum of two numbers.
 */
HNumber HMath::min( const HNumber & n1, const HNumber & n2 )
{
  switch ( float_cmp(&n1.d->fnum, &n2.d->fnum) )
  {
    case 0:
    case 1:  return n2;
    case -1: return n1;
    default: return HNumber::nan(checkNaNParam(*n1.d, n2.d));
  }
}


/**
 * Returns the absolute value of n.
 */
HNumber HMath::abs( const HNumber & n )
{
  HNumber result;
  call1ArgND(result.d, n.d, float_abs);
  return result;
}


/**
 * Rounds n to the specified decimal digits.
 */
HNumber HMath::round( const HNumber & n, int prec )
{
  if (n.isNan())
    return HNumber::nan(checkNaNParam(*n.d));
  int exp;
  floatnum rnum;
  HNumber result(n);
  rnum = &result.d->fnum;
  exp = float_getexponent(rnum);
  /* avoid exponent overflow later */
  if (prec > HMATH_WORKING_PREC && exp > 0)
    prec = HMATH_WORKING_PREC;
  if (prec < 0 && -exp-1 > prec)
    float_setzero(rnum);
  else if (exp + prec < HMATH_WORKING_PREC)
  {
    float_addexp(rnum, prec);
    float_roundtoint(rnum, TONEAREST);
    float_addexp(rnum, -prec);
  }
  return result;
}


/**
 * Truncates n to the specified decimal digits.
 */
HNumber HMath::trunc( const HNumber & n, int prec )
{
  if (n.isNan())
    return HNumber::nan(checkNaNParam(*n.d));
  int exp;
  floatnum rnum;
  HNumber result(n);
  rnum = &result.d->fnum;
  exp = float_getexponent(rnum);
  /* avoid exponent overflow later on */
  if (prec > HMATH_WORKING_PREC && exp > 0)
    prec = HMATH_WORKING_PREC;
  if (prec < 0 && -exp-1 > prec)
    float_setzero(rnum);
  else if (exp + prec < HMATH_WORKING_PREC)
  {
    float_addexp(rnum, prec);
    float_roundtoint(rnum, TOZERO);
    float_addexp(rnum, -prec);
  }
  return result;
}


/**
 * Returns the integer part of n.
 */
HNumber HMath::integer( const HNumber & n )
{
  HNumber result;
  call1ArgND(result.d, n.d, float_int);
  return result;
}


/**
 * Returns the fraction part of n.
 */
HNumber HMath::frac( const HNumber & n )
{
  HNumber result;
  call1ArgND(result.d, n.d, float_frac);
  return result;
}


/**
 * Returns the floor of n.
 */
HNumber HMath::floor( const HNumber & n )
{
  if (n.isNan())
    return HNumber::nan(checkNaNParam(*n.d));
  HNumber r(n);
  float_roundtoint(&r.d->fnum, TOMINUSINFINITY);
  return r;
}


/**
 * Returns the ceiling of n.
 */
HNumber HMath::ceil( const HNumber & n )
{
  if (n.isNan())
    return HNumber::nan(checkNaNParam(*n.d));
  HNumber r(n);
  float_roundtoint(&r.d->fnum, TOPLUSINFINITY);
  return r;
}


/**
 * Returns the greatest common divisor of n1 and n2.
 */
HNumber HMath::gcd( const HNumber & n1, const HNumber & n2 )
{
  if( !n1.isInteger() || !n2.isInteger() )
  {
    Error error = checkNaNParam(*n1.d, n2.d);
    if (error != Success)
      return HNumber::nan(error);
    return HNumber::nan(TypeMismatch);
  }

  HNumber a = abs( n1 );
  HNumber b = abs( n2 );

  if ( a == 0 )
    return b;
  if ( b == 0 )
    return a;

  // run Euclidean algorithm
  while ( true )
  {
    a = a % b;

    if ( a == 0 )
      return b;

    b = b % a;

    if ( b == 0 )
      return a;
  }
}


/**
 * Returns the square root of n. If n is negative, returns NaN.
 */
HNumber HMath::sqrt( const HNumber & n )
{
  HNumber result;
  call1Arg(result.d, n.d, float_sqrt);
  return result;
}


/**
 * Returns the cube root of n.
 */
HNumber HMath::cbrt( const HNumber & n )
{
  int expn, digits;
  floatstruct a, q;
  floatnum rnum;
  signed char sign;

  if (n.isNan())
    return HNumber::nan(checkNaNParam(*n.d));
  if( n.isZero() )
    return n;
  HNumber r;
  rnum = &r.d->fnum;

  // iterations to approximate result
  // X[i+1] = (2/3)X[i] + n / (3 * X[i]^2))
  // initial guess = sqrt( n )
  // r = X[i], q = X[i+1], a = n

  float_create(&a);
  float_create(&q);
  float_copy(&a, &n.d->fnum, HMATH_EVAL_PREC);
  sign = float_getsign(&a);
  float_abs(&a);
  expn = float_getexponent(&a);
  float_setexponent(&a, expn % 3);
  expn /= 3;

  digits = 0;
  float_copy(&q, &a, 2);
  float_sqrt(&q, 2);
  while (!float_iszero(rnum) && digits < HMATH_EVAL_PREC/2 + 1)
  {
    digits = 4 * digits + 2;
    if (digits > HMATH_EVAL_PREC+2)
      digits = HMATH_EVAL_PREC+2;
    float_move(rnum, &q);
    float_mul(&q, rnum, rnum, digits);
    float_div(&q, &a, &q, digits);
    float_add(&q, &q, rnum, digits);
    float_add(&q, &q, rnum, digits);
    float_div(&q, &q, &c3, digits);
    float_sub(rnum, rnum, &q, 3);
    digits = float_getexponent(&q) - float_getexponent(rnum);
  }
  float_move(rnum, &q);
  float_free(&q);
  float_free(&a);
  float_setsign(rnum, sign);
  float_addexp(rnum, expn);

  roundResult(&r.d->fnum);
  return r;
}


/**
 * Raises n1 to an integer n.
 */
HNumber HMath::raise( const HNumber & n1, int n )
{
  if (n1.isNan())
    return HNumber::nan(checkNaNParam(*n1.d));
  HNumber r;
  float_raisei(&r.d->fnum, &n1.d->fnum, n, HMATH_EVAL_PREC);
  roundSetError(r.d);
  return r;
}


/**
 * Raises n1 to n2.
 */
HNumber HMath::raise( const HNumber & n1, const HNumber & n2  )
{
  HNumber result;
  call2Args(result.d, n1.d, n2.d, float_raise);
  return result;
}


/**
 * Returns e raised to x.
 */
HNumber HMath::exp( const HNumber & x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_exp);
  return result;
};


/**
 * Returns the natural logarithm of x.
 * If x is non positive, returns NaN.
 */
HNumber HMath::ln( const HNumber & x )
{
  HNumber result;
  call1ArgPoleCheck(result.d, x.d, float_ln);
  return result;

}


/**
 * Returns the base-10 logarithm of x.
 * If x is non positive, returns NaN.
 */
HNumber HMath::log( const HNumber & x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_log);
  return result;
}


/**
 * Returns the base-2 logarithm of x.
 * If x is non positive, returns NaN.
 */
HNumber HMath::lg( const HNumber & x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_lg);
  return result;
}

/**
 * Returns the sine of x. Note that x must be in radians.
 */
HNumber HMath::sin( const HNumber & x )
{
  HNumber result;
  call1ArgPoleCheck(result.d, x.d, float_sin);
  return result;
}


/**
 * Returns the cosine of x. Note that x must be in radians.
 */
HNumber HMath::cos( const HNumber & x )
{
  HNumber result;
  call1ArgPoleCheck(result.d, x.d, float_cos);
  return result;
}


/**
 * Returns the tangent of x. Note that x must be in radians.
 */
HNumber HMath::tan( const HNumber & x )
{
  HNumber result;
  call1ArgPoleCheck(result.d, x.d, float_tan);
  return result;
}


/**
 * Returns the cotangent of x. Note that x must be in radians.
 */
HNumber HMath::cot( const HNumber & x )
{
  return cos(x) / sin(x);
}


/**
 * Returns the secant of x. Note that x must be in radians.
 */
HNumber HMath::sec( const HNumber & x )
{
  return HNumber(1) / cos(x);
}


/**
 * Returns the cosecant of x. Note that x must be in radians.
 */
HNumber HMath::csc( const HNumber & x )
{
  return HNumber(1) / sin(x);
}


/**
 * Returns the arc tangent of x.
 */
HNumber HMath::atan( const HNumber & x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_arctan);
  return result;
};


/**
 * Returns the arc sine of x.
 */
HNumber HMath::asin( const HNumber & x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_arcsin);
  return result;
};


/**
 * Returns the arc cosine of x.
 */
HNumber HMath::acos( const HNumber & x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_arccos);
  return result;
};


/**
 * Returns the hyperbolic sine of x.
 */
HNumber HMath::sinh( const HNumber & x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_sinh);
  return result;
}


/**
 * Returns the hyperbolic cosine of x.
 */
HNumber HMath::cosh( const HNumber & x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_cosh);
  return result;
}


/**
 * Returns the hyperbolic tangent of x.
 */
HNumber HMath::tanh( const HNumber & x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_tanh);
  return result;
}


/**
 * Returns the area hyperbolic sine of x.
 */
HNumber HMath::arsinh( const HNumber & x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_arsinh);
  return result;
}


/**
 * Returns the area hyperbolic cosine of x.
 */
HNumber HMath::arcosh( const HNumber & x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_arcosh);
  return result;
}


/**
 * Returns the area hyperbolic tangent of x.
 */
HNumber HMath::artanh( const HNumber & x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_artanh);
  return result;
}


/**
 * Returns the Gamma function.
 */
HNumber HMath::gamma( const HNumber & x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_gamma);
  return result;
}


/**
 * Returns ln(abs(Gamma(x))).
 */
HNumber HMath::lnGamma( const HNumber & x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_lngamma);
  return result;
}


/**
 * Returns the sign of x.
 */
HNumber HMath::sign( const HNumber & x )
{
  if (x.isNan())
    return HNumber::nan(checkNaNParam(*x.d));

  return float_getsign(&x.d->fnum);
}


/**
 * Returns the binomial coefficient of n and k.
 */
HNumber HMath::nCr( const HNumber & n, const HNumber & r )
{
  floatstruct fn, fr;
  floatnum rnum;

  if( !n.isInteger() || !r.isInteger() )
  {
    Error error = checkNaNParam(*n.d, r.d);
    if (error != 0)
      return HNumber::nan(error);
    return HNumber::nan(TypeMismatch);
  }

  if (r > 50 && n-r > 50)
  {
    HNumber result(n);
    rnum = &result.d->fnum;

    float_create(&fn);
    float_create(&fr);
    float_copy(&fr, &r.d->fnum, HMATH_EVAL_PREC);
    float_copy(&fn, rnum, EXACT);
    float_sub(rnum, rnum, &fr, HMATH_EVAL_PREC)
    && float_add(&fn, &fn, &c1, HMATH_EVAL_PREC)
    && float_add(&fr, &fr, &c1, HMATH_EVAL_PREC)
    && float_add(rnum, rnum, &c1, HMATH_EVAL_PREC)
    && float_lngamma(&fn, HMATH_EVAL_PREC)
    && float_lngamma(&fr, HMATH_EVAL_PREC)
    && float_lngamma(rnum, HMATH_EVAL_PREC)
    && float_add(rnum, rnum, &fr, HMATH_EVAL_PREC)
    && float_sub(rnum, &fn, rnum, HMATH_EVAL_PREC)
    && float_exp(rnum, HMATH_EVAL_PREC);
    float_free(&fn);
    float_free(&fr);
    roundSetError(result.d);
    return result;
  }
  if (r > n/2)
    return factorial(n, r+1) / factorial((n-r), 1);
  else
    return factorial(n, (n-r+1)) / factorial(r, 1);
}


/**
 * Returns the permutation of n elements chosen r elements.
 */
HNumber HMath::nPr( const HNumber & n, const HNumber & r )
{
  return factorial(n, (n-r+1));
}


/**
 * Returns the falling Pochhammer symbol x*(x-1)*..*base.
 * For base == 1, this is the usual factorial x!, what this
 * function is named after.
 * This function has been extended using the Gamma function,
 * so that actually Gamma(x+1)/Gamma(base) is computed, a
 * value that equals the falling Pochhammer symbol, when
 * x - base is an integer, but allows other differences as well.
 */
HNumber HMath::factorial( const HNumber & x, const HNumber & base )
{
  floatstruct tmp;

  if (float_cmp(&c1, &base.d->fnum) == 0)
  {
    HNumber result;
    call1Arg(result.d, x.d, float_factorial);
    return result;
  }
  float_create(&tmp);
  HNumber r(base);
  float_sub(&tmp, &x.d->fnum, &base.d->fnum, HMATH_EVAL_PREC)
  && float_add(&tmp, &tmp, &c1, HMATH_EVAL_PREC)
  && float_pochhammer(&r.d->fnum, &tmp, HMATH_EVAL_PREC);
  roundSetError(r.d);
  float_free(&tmp);
  return r;
}


/**
 * Calculates the binomial discrete distribution probability mass function:
 * \f[X{\sim}B(n,p)\f]
 * \f[\Pr(X=k|n,p)={n\choose k}p^{k}(1-p)^{n-k}\f]
 *
 * \param[in] k the number of probed exact successes
 * \param[in] n the number of trials
 * \param[in] p the probability of success in a single trial
 *
 * \return the probability of exactly \p k successes, otherwise \p NaN if the
 * function is not defined for the specified parameters.
 */
HNumber HMath::binomialPmf( const HNumber & k, const HNumber & n, const HNumber & p )
{
  if ( k.isNan() || ! k.isInteger() || k < 0 || k > n
         || n.isNan() || ! n.isInteger() || n < 0
           || p.isNan() || p < 0 || p > 1 )
    return HNumber::nan();

  return HMath::nCr( n, k ) * HMath::raise( p, k ) *
           HMath::raise( HNumber(1)-p, n-k );
}


/**
 * Calculates the binomial cumulative distribution function:
 * \f[X{\sim}B(n,p)\f]
 * \f[\Pr(X \leq k|n,p)=\sum_{i=0}^{k}{n\choose i}p^{i}(1-p)^{n-i}\f]
 *
 * \param[in] k the maximum number of probed successes
 * \param[in] n the number of trials
 * \param[in] p the probability of success in a single trial
 *
 * \return the probability of up to \p k successes, otherwise \p NaN if the
 * function is not defined for the specified parameters.
 */
HNumber HMath::binomialCdf( const HNumber & k, const HNumber & n, const HNumber & p )
{
  // FIXME use the regularized incomplete Beta function to avoid
  // the potentially very expensive loop
  if ( k.isNan() || ! k.isInteger() || k < 0 || k > n
         || n.isNan() || ! n.isInteger() || n < 0
           || p.isNan() || p < 0 || p > 1 )
    return HNumber::nan();

  HNumber one = HNumber(1);
  HNumber pcompl = one - p;
  HNumber summand = HMath::raise( pcompl, n );
  HNumber result( summand );
  for ( HNumber i( 0 ); i < k; )
  {
    summand *= p * (n-i);
    i += one;
    summand /= (pcompl * i);
    result += summand;
  }
  return result;
}


/**
 * Calculates the expected value of a binomially distributed random variable:
 * \f[X{\sim}B(n,p)\f]
 * \f[E(X)=np\f]
 *
 * \param[in] n the number of trials
 * \param[in] p the probability of success in a single trial
 *
 * \return the expected value of the variable, otherwise \p NaN if the
 * function is not defined for the specified parameters.
 */
HNumber HMath::binomialMean( const HNumber & n, const HNumber & p )
{
  if ( n.isNan() || ! n.isInteger() || n < 0
         || p.isNan() || p < 0 || p > 1 )
    return HNumber::nan();

  return n * p;
}


/**
 * Calculates the variance of a binomially distributed random variable:
 * \f[X{\sim}B(n,p)\f]
 * \f[Var(X)=np(1-p)\f]
 *
 * \param[in] n the number of trials
 * \param[in] p the probability of success in a single trial
 *
 * \return the variance of the variable, otherwise \p NaN if the
 * function is not defined for the specified parameters.
 */
HNumber HMath::binomialVariance( const HNumber & n, const HNumber & p )
{
  if ( n.isNan() || ! n.isInteger() || n < 0
         || p.isNan() || p < 0 || p > 1 )
    return HNumber::nan();

  return n * p * ( HNumber(1) - p );
}


/**
 * Calculates the hypergeometric discrete distribution probability mass
 * function:
 * \f[X{\sim}H(N,M,n)\f]
 * \f[\Pr(X=k|N,M,n)=\frac{{M\choose k}{N-M\choose n-k}}{{N\choose n}}\f]
 *
 * \param[in] k the number of probed exact successes
 * \param[in] N the number of total elements
 * \param[in] M the number of success elements
 * \param[in] n the number of selected elements
 *
 * \return the probability of exactly \p k successes, otherwise \p NaN if the
 * function is not defined for the specified parameters.
 */
HNumber HMath::hypergeometricPmf( const HNumber & k, const HNumber & N,
                                  const HNumber & M, const HNumber & n )
{
  if ( k.isNan() || ! k.isInteger() || k < max( 0, M+n-N ) || k > min( M, n )
         || N.isNan() || ! N.isInteger() || N < 0
           || M.isNan() || ! M.isInteger() || M < 0 || M > N
             || n.isNan() || ! n.isInteger() || n < 0 || n > N )
    return HNumber::nan();

  return HMath::nCr( M, k ) * HMath::nCr( N-M, n-k ) / HMath::nCr( N, n );
}


/**
 * Calculates the hypergeometric cumulative distribution function:
 * \f[X{\sim}H(N,M,n)\f]
 * \f[\Pr(X\leq k|N,M,n)=
 *   \sum_{i=0}^{k}\frac{{M\choose k}{N-M\choose n-k}}{{N\choose n}}\f]
 *
 * \param[in] k the maximum number of probed successes
 * \param[in] N the number of total elements
 * \param[in] M the number of success elements
 * \param[in] n the number of selected elements
 *
 * \return the probability of up to \p k successes, otherwise \p NaN if the
 * function is not defined for the specified parameters.
 */
HNumber HMath::hypergeometricCdf( const HNumber & k, const HNumber & N,
                                  const HNumber & M, const HNumber & n )
{
  if ( k.isNan() || ! k.isInteger() || k < max( 0, M+n-N ) || k > min( M, n )
         || N.isNan() || ! N.isInteger() || N < 0
           || M.isNan() || ! M.isInteger() || M < 0 || M > N
             || n.isNan() || ! n.isInteger() || n < 0 || n > N )
    return HNumber::nan();

  HNumber result( 0 );
  for ( HNumber i( 0 ); i <= k; i += 1 )
    result += HMath::nCr( M, i ) * HMath::nCr( N-M, n-i ) / HMath::nCr( N, n );
  return result;
}


/**
 * Calculates the expected value of a hypergeometrically distributed random
 * variable:
 * \f[X{\sim}H(N,M,n)\f]
 * \f[E(X)=n\frac{M}{N}\f]
 *
 * \param[in] N the number of total elements
 * \param[in] M the number of success elements
 * \param[in] n the number of selected elements
 *
 * \return the expected value of the variable, otherwise \p NaN if the
 * function is not defined for the specified parameter.
 */
HNumber HMath::hypergeometricMean( const HNumber & N, const HNumber & M, const HNumber & n )
{
  if ( N.isNan() || ! N.isInteger() || N < 0
         || M.isNan() || ! M.isInteger() || M < 0 || M > N
           || n.isNan() || ! n.isInteger() || n < 0 || n > N )
    return HNumber::nan();

  return n * M / N;
}


/**
 *
 * Calculates the variance of a hypergeometrically distributed random variable:
 * \f[X{\sim}H(N,M,n)\f]
 * \f[Var(X)=\frac{n\frac{M}{N}(1-\frac{M}{N})(N-n)}{N-1}\f]
 *
 * \param[in] N the number of total elements
 * \param[in] M the number of success elements
 * \param[in] n the number of selected elements
 *
 * \return the variance of the variable, otherwise \p NaN if the function is
 * not defined for the specified parameter.
 */
HNumber HMath::hypergeometricVariance( const HNumber & N, const HNumber & M, const HNumber & n )
{
  if ( N.isNan() || ! N.isInteger() || N < 0
         || M.isNan() || ! M.isInteger() || M < 0 || M > N
           || n.isNan() || ! n.isInteger() || n < 0 || n > N )
    return HNumber::nan();

  return (n * (M/N) * (HNumber(1) - M/N) * (N-n)) / (N - HNumber(1));
}


/**
 *
 * Calculates the poissonian discrete distribution probability mass function:
 * \f[X{\sim}P(\lambda)\f]
 * \f[\Pr(X=k|\lambda)=\frac{e^{-\lambda}\lambda^k}{k!}\f]
 *
 * \param[in] k the number of event occurrences
 * \param[in] l the expected number of occurrences that occur in an interval
 *
 * \return the probability of exactly \p k event occurrences, otherwise \p NaN
 * if the function is not defined for the specified parameters.
 */
HNumber HMath::poissonPmf( const HNumber & k, const HNumber & l )
{
  if ( k.isNan() || ! k.isInteger() || k < 0
         || l.isNan() || l < 0 )
    return HNumber::nan();

  return exp( -l ) * raise( l, k ) / factorial( k );
}


/**
 * Calculates the poissonian cumulative distribution function:
 * \f[X{\sim}P(\lambda)\f]
 * \f[\Pr(X\leq k|\lambda)=\sum_{i=0}^{k}\frac{e^{-\lambda}\lambda^k}{k!}\f]
 *
 * \param[in] k the maximum number of event occurrences
 * \param[in] l the expected number of occurrences that occur in an interval
 *
 * \return the probability of up to \p k event occurrences, otherwise \p NaN
 * if the function is not defined for the specified parameters.
 */
HNumber HMath::poissonCdf( const HNumber & k, const HNumber & l )
{
  if ( k.isNan() || ! k.isInteger() || k < 0
         || l.isNan() || l < 0 )
    return HNumber::nan();

  HNumber result( 0 );
  for ( HNumber i( 0 ); i <= k; i += 1 )
    result += exp( -l ) * raise( l, i ) / factorial( i );

  return result;
}


/**
 * Calculates the expected value of a Poisson distributed random variable:
 * \f[X{\sim}P(\lambda)\f]
 * \f[E(X)=\lambda\f]
 *
 * \param[in] l the expected number of occurrences that occur in an interval
 *
 * \return the expected value of the variable, otherwise \p NaN if the
 * function is not defined for the specified parameter.
 */
HNumber HMath::poissonMean( const HNumber & l )
{
  if ( l.isNan() || l < 0 )
    return HNumber::nan();

  return l;
}


/**
 * Calculates the variance of a Poisson distributed random variable:
 * \f[X{\sim}P(\lambda)\f]
 * \f[Var(X)=\lambda\f]
 *
 * \param[in] l the expected number of occurrences that occur in an interval
 *
 * \return the variance of the variable, otherwise \p NaN if the function is
 * not defined for the specified parameter.
 */
HNumber HMath::poissonVariance( const HNumber & l )
{
  if ( l.isNan() || l < 0 )
    return HNumber::nan();

  return l;
}


/**
 * Returns the erf function (related to normal distribution).
 */
HNumber HMath::erf( const HNumber & x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_erf);
  return result;
}


/**
 * Returns the complementary erf function (related to normal distribution).
 */
HNumber HMath::erfc( const HNumber & x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_erfc);
  return result;
}


/**
 * Restricts a logic value to a given bit size.
 */
HNumber HMath::mask ( const HNumber & val, const HNumber & bits )
{
  if ( val.isNan() || bits == 0 || bits >= LOGICRANGE || ! bits.isInteger() )
    return HNumber::nan();
  return val & ~(HNumber(-1) << HNumber(bits));
}


/**
 * sign-extends an unsigned value
 */
HNumber HMath::sgnext( const HNumber & val, const HNumber & bits )
{
  if ( val.isNan() || bits == 0 || bits >= LOGICRANGE || ! bits.isInteger() )
    return HNumber::nan();
  HNumber ofs;
  ofs = HNumber(LOGICRANGE) - bits;
  return (val << ofs) >> ofs;
}


/**
 * For bits >= 0 does an arithmetic shift right, for bits < 0 a shift left.
 */
HNumber HMath::ashr( const HNumber & val, const HNumber & bits )
{
  if ( val.isNan() || bits <= -LOGICRANGE || bits >= LOGICRANGE
       || ! bits.isInteger() )
    return HNumber::nan();
  if (bits >= 0)
    return val >> bits;
  return val << -bits;
}


std::ostream& operator<<( std::ostream& s, const HNumber& n )
{
  char* str = HMath::formatFixed( n );
  s << str;
  delete[] str;
  return s;
}
