/* HMath: C++ high precision math routines
   Copyright (C) 2004 Ariya Hidayat <ariya.hidayat@gmail.com>
                 2007 Helder Correia <helder.pereira.correia@gmail.com>
                 2008 Wolf Lammen
   Last update: January 01, 2008
   adapted to floatnum Wolf Lammen July 2007

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
 */

#include <math/hmath.hxx>
#include <math/floatnum.h>
#include <math/floatconst.h>
#include <math/floatconvert.h>
#include <math/floatcommon.h>
#include "math/floatio.h"
#include <math/floathmath.h>
#include "main/errors.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sstream>

#define HMATH_WORKING_PREC (DECPRECISION + 3)
#define HMATH_EVAL_PREC (HMATH_WORKING_PREC + 2)

// default scale for fall back in formatting
#define HMATH_MAX_SHOWN 20
#define HMATH_BIN_MAX_SHOWN ((33219*HMATH_MAX_SHOWN)/10000 + 1)
#define HMATH_OCT_MAX_SHOWN ((11073*HMATH_MAX_SHOWN)/10000 + 1)
#define HMATH_HEX_MAX_SHOWN ((8305*HMATH_MAX_SHOWN)/10000 + 1)

/*------------------------   Helper routines  -------------------------*/

void static checkfullcancellation( floatnum op1, floatnum op2,
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
        || float_getexponent(op1) - expr >= HMATH_WORKING_PREC - 1)
      float_setzero(r);
  }
}

static char checkAdd(floatnum dest, floatnum s1, floatnum s2, int digits)
{
  if (float_add(dest, s1, s2, digits)
      && float_getsign(s1) + float_getsign(s2) == 0)
    checkfullcancellation(s1, s2, dest);
  return float_isnan(dest);
}

static char checkSub(floatnum dest, floatnum s1, floatnum s2, int digits)
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
    float_seterror(FLOAT_UNSTABLE);
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
  int error;
  char format;
};

HNumberPrivate::HNumberPrivate()
  : error(0), format(0)
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
typedef char (*Float2ArgsND)(floatnum result, floatnum p1, floatnum p2);
typedef char (*Float2Args)(floatnum result, floatnum p1, floatnum p2, int digits);

static int checkNaNParam(const HNumberPrivate& v1,
                         const HNumberPrivate* v2 = 0)
{
  if ( !float_isnan(&v1.fnum) && (!v2 || !float_isnan(&v2->fnum)))
    return 0;
  int error = v1.error;
  if ( error == 0 && v2 )
    error = v2->error;
  if ( error == 0 )
    error = FLOAT_NANOPERAND;
  return error;
}

void roundSetError(HNumberPrivate* dest)
{
  dest->error = float_geterror();
  floatnum dfnum = &dest->fnum;
  if (dest->error != 0)
    float_setnan(dfnum);
  if (!float_isnan(dfnum))
    float_round(dfnum, dfnum, HMATH_WORKING_PREC, TONEAREST);
}

void call2Args(HNumberPrivate* dest, HNumberPrivate* n1,
               HNumberPrivate* n2, Float2Args func)
{
  dest->error = checkNaNParam(*n1, n2);
  if (dest->error == 0)
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
  if (dest->error == 0)
  {
    floatnum dfnum = &dest->fnum;
    func(dfnum, &n1->fnum, &n2->fnum);
    roundSetError(dest);
  }
}

void call1Arg(HNumberPrivate* dest, HNumberPrivate* n, Float1Arg func)
{
  dest->error = checkNaNParam(*n);
  if (dest->error == 0)
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
  if (dest->error == 0)
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
  if (dest->error == 0)
  {
    floatnum dfnum = &dest->fnum;
    float_copy(dfnum, &n->fnum, HMATH_EVAL_PREC);
    func(dfnum);
    roundSetError(dest);
  }
}

char modwrap(floatnum result, floatnum p1, floatnum p2, int digits)
{
  char ok;
  floatstruct tmp;
  float_create(&tmp);
  ok = float_divmod(&tmp, result, p1, p2, INTQUOT);
  float_free(&tmp);
  return ok;
}

char idivwrap(floatnum result, floatnum p1, floatnum p2)
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

HNumber::HNumber()
{
  d = new HNumberPrivate;
}

HNumber::HNumber( const HNumber& hn )
{
  d = new HNumberPrivate;
  operator=( hn );
}

HNumber::HNumber( int i )
{
  d = new HNumberPrivate;
  float_setinteger(&d->fnum, i);
}

HNumber::HNumber( const char* str )
{
  t_itokens tokens;

  d = new HNumberPrivate;
  if ((d->error = parse(&tokens, &str)) == IO_NO_ERROR && *str == 0)
    d->error = float_in(&d->fnum, &tokens);
  float_geterror();
}

HNumber::~HNumber()
{
  delete d;
}

int HNumber::error() const
{
  return d->error;
}

bool HNumber::isNan() const
{
  return float_isnan(&d->fnum) != 0;
}

bool HNumber::isZero() const
{
  return float_iszero(&d->fnum) != 0;
}

bool HNumber::isPositive() const
{
  return float_getsign(&d->fnum) > 0;
}

bool HNumber::isNegative() const
{
  return float_getsign(&d->fnum) < 0;
}

bool HNumber::isInteger() const
{
  return float_isinteger(&d->fnum) != 0;
}

char HNumber::format() const
{
   return d->format;
}

void HNumber::setFormat(char c) const
{
   d->format = float_isnan(&d->fnum)?0:c;
}

HNumber HNumber::nan(int error)
{
  HNumber result;
  result.d->error = error;
  return result;
}

int HNumber::toInt() const
{
  return float_asinteger(&d->fnum);
}

HNumber& HNumber::operator=( const HNumber& hn )
{
  d->format = hn.format();
  d->error = hn.error();
  float_copy(&d->fnum, &hn.d->fnum, EXACT);
  return *this;
}

HNumber HNumber::operator+( const HNumber& num ) const
{
  HNumber result;
  call2Args(result.d, d, num.d, checkAdd);
  return result;
}

HNumber& HNumber::operator+=( const HNumber& num )
{
  return operator=( *this + num );
}

HNumber HNumber::operator-( const HNumber& num ) const
{
  HNumber result;
  call2Args(result.d, d, num.d, checkSub);
  return result;
}

HNumber& HNumber::operator-=( const HNumber& num )
{
  return operator=( *this - num );
}

HNumber HNumber::operator*( const HNumber& num ) const
{
  HNumber result;
  call2Args(result.d, d, num.d, float_mul);
  return result;
}

HNumber& HNumber::operator*=( const HNumber& num )
{
  return operator=( *this * num );
}

HNumber HNumber::operator/( const HNumber& num ) const
{
  HNumber result;
  call2Args(result.d, d, num.d, float_div);
  return result;
}

HNumber& HNumber::operator/=( const HNumber& num )
{
  return operator=( *this / num );
}

HNumber HNumber::operator%( const HNumber& num ) const
{
  HNumber result;
  call2Args(result.d, d, num.d, modwrap);
  if (result.error() == FLOAT_INVALIDPARAM)
    result.d->error = HMATH_TOO_EXPENSIVE;
  return result;
}

HNumber HNumber::idiv( const HNumber& dividend, const HNumber& divisor)
{
  HNumber result;
  call2ArgsND(result.d, dividend.d, divisor.d, idivwrap);
  if (result.error() == FLOAT_INVALIDPARAM)
    result.d->error = HMATH_INTEGER_OVERFLOW;
  return result;
}

bool HNumber::operator>( const HNumber& n ) const
{
  return HMath::compare( *this, n ) > 0;
}

bool HNumber::operator<( const HNumber& n ) const
{
  return HMath::compare( *this, n ) < 0;
}

bool HNumber::operator>=( const HNumber& n ) const
{
  return HMath::compare( *this, n ) >= 0;
}

bool HNumber::operator<=( const HNumber& n ) const
{
  return HMath::compare( *this, n ) <= 0;
}

bool HNumber::operator==( const HNumber& n ) const
{
  return HMath::compare( *this, n ) == 0;
}

bool HNumber::operator!=( const HNumber& n ) const
{
  return HMath::compare( *this, n ) != 0;
}

HNumber HNumber::operator&( const HNumber& num ) const
{
  HNumber result;
  call2ArgsND(result.d, d, num.d, float_and);
  return result;
}

HNumber& HNumber::operator&=( const HNumber& num )
{
  return operator=( *this & num );
}

HNumber HNumber::operator|( const HNumber& num ) const
{
  HNumber result;
  call2ArgsND(result.d, d, num.d, float_or);
  return result;
}

HNumber& HNumber::operator|=( const HNumber& num )
{
  return operator=( *this | num );
}

HNumber HNumber::operator^( const HNumber& num ) const
{
  HNumber result;
  call2ArgsND(result.d, d, num.d, float_xor);
  return result;
}

HNumber& HNumber::operator^=( const HNumber& num )
{
  return operator=( *this ^ num );
}

HNumber HNumber::operator~() const
{
  HNumber result;
  call1ArgND(result.d, d, float_not);
  return result;
}

HNumber HNumber::operator-() const
{
  HNumber result;
  call1ArgND(result.d, d, float_neg);
  return result;
}

HNumber HNumber::operator<<( const HNumber& num ) const
{
  HNumber result;
  call2ArgsND(result.d, d, num.d, float_shl);
  return result;
}

HNumber HNumber::operator>>( const HNumber& num ) const
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
  if (float_out(&tokens, &tmp, prec, base, expbase, outmode))
  {
    sz = cattokens(NULL, -1, &tokens, flags);
    str = (char*)malloc( sz );
    cattokens(str, sz, &tokens, flags);
  }
  float_free(&tmp);
  return str;
}

// format number with fixed number of decimal digits
char* HMath::formatFixed( const HNumber& hn, int prec )
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

// format number with exponential
char* HMath::formatScientific( const HNumber& hn, int prec )
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

// format number in engineering notation
char* HMath::formatEngineering( const HNumber& hn, int prec )
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

char* HMath::formatGeneral( const HNumber& hn, int prec )
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

char* HMath::formatHexadec( const HNumber& hn )
{
  return formathexfp (&hn.d->fnum, 16, 10, HMATH_HEX_MAX_SHOWN);
}

char* HMath::formatOctal( const HNumber& hn )
{
  return formathexfp (&hn.d->fnum, 8, 10, HMATH_OCT_MAX_SHOWN);
}

char* HMath::formatBinary( const HNumber& hn )
{
  return formathexfp (&hn.d->fnum, 2, 10, HMATH_BIN_MAX_SHOWN);
}

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

HNumber HMath::rad2deg( const HNumber& angle )
{
  return angle * (HNumber(180) / HMath::pi());
}

HNumber HMath::deg2rad( const HNumber& angle )
{
  return angle * (HMath::pi() / HNumber(180));
}

HNumber HMath::pi()
{
  HNumber value;

  float_copy(&value.d->fnum, &cPi, HMATH_EVAL_PREC);
  return value;
}

HNumber HMath::phi()
{
  return HNumber("1.61803398874989484820458683436563811772030917980576"
                   "28621354486227052604628189024497072072041893911374"
                   "84754088075386891752");
}

int HMath::compare( const HNumber& n1, const HNumber& n2 )
{
  int result = float_relcmp(&n1.d->fnum, &n2.d->fnum, HMATH_EVAL_PREC-1);
  float_geterror(); // clears error, if one operand was a NaN
  return result;
}

HNumber HMath::max( const HNumber& n1, const HNumber& n2 )
{
  switch ( float_cmp(&n1.d->fnum, &n2.d->fnum) )
  {
    case 0:
    case 1:  return n1;
    case -1: return n2;
    default: return HNumber::nan(checkNaNParam(*n1.d, n2.d));
  }
}

HNumber HMath::min( const HNumber& n1, const HNumber& n2 )
{
  switch ( float_cmp(&n1.d->fnum, &n2.d->fnum) )
  {
    case 0:
    case 1:  return n2;
    case -1: return n1;
    default: return HNumber::nan(checkNaNParam(*n1.d, n2.d));
  }
}

HNumber HMath::abs( const HNumber& n )
{
  HNumber result;
  call1ArgND(result.d, n.d, float_abs);
  return result;
}

HNumber HMath::round( const HNumber& n, int prec )
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

HNumber HMath::trunc( const HNumber& n, int prec )
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

HNumber HMath::integer( const HNumber& n )
{
  HNumber result;
  call1ArgND(result.d, n.d, float_int);
  return result;
}

HNumber HMath::frac( const HNumber& n )
{
  HNumber result;
  call1ArgND(result.d, n.d, float_frac);
  return result;
}

HNumber HMath::floor( const HNumber& n )
{
  if (n.isNan())
    return HNumber::nan(checkNaNParam(*n.d));
  HNumber r(n);
  float_roundtoint(&r.d->fnum, TOMINUSINFINITY);
  return r;
}

HNumber HMath::ceil( const HNumber& n )
{
  if (n.isNan())
    return HNumber::nan(checkNaNParam(*n.d));
  HNumber r(n);
  float_roundtoint(&r.d->fnum, TOPLUSINFINITY);
  return r;
}

HNumber HMath::gcd( const HNumber& n1, const HNumber& n2 )
{
  if( !n1.isInteger() || !n2.isInteger() )
  {
    int error = checkNaNParam(*n1.d, n2.d);
    if (error != 0)
      return HNumber::nan(error);
    return HNumber::nan(HMATH_INTEGER_REQUIRED);
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

HNumber HMath::sqrt( const HNumber& n )
{
  HNumber result;
  call1Arg(result.d, n.d, float_sqrt);
  return result;
}

HNumber HMath::cbrt( const HNumber& n )
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

HNumber HMath::raise( const HNumber& n1, int n )
{
  if (n1.isNan())
    return HNumber::nan(checkNaNParam(*n1.d));
  HNumber r;
  float_raisei(&r.d->fnum, &n1.d->fnum, n, HMATH_EVAL_PREC);
  roundSetError(r.d);
  return r;
}

HNumber HMath::raise( const HNumber& n1, const HNumber& n2  )
{
  HNumber result;
  call2Args(result.d, n1.d, n2.d, float_raise);
  return result;
}

HNumber HMath::exp( const HNumber& x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_exp);
  return result;
};

HNumber HMath::ln( const HNumber& x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_ln);
  return result;

}

HNumber HMath::log( const HNumber& x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_log);
  return result;
}

HNumber HMath::lg( const HNumber& x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_lg);
  return result;
}

HNumber HMath::sin( const HNumber& x )
{
  HNumber result;
  call1ArgPoleCheck(result.d, x.d, float_sin);
  return result;
}


HNumber HMath::cos( const HNumber& x )
{
  HNumber result;
  call1ArgPoleCheck(result.d, x.d, float_cos);
  return result;
}

HNumber HMath::tan( const HNumber& x )
{
  HNumber result;
  call1ArgPoleCheck(result.d, x.d, float_tan);
  return result;
}

HNumber HMath::cot( const HNumber& x )
{
  return cos(x) / sin(x);
}

HNumber HMath::sec( const HNumber& x )
{
  return HNumber(1) / cos(x);
}

HNumber HMath::csc( const HNumber& x )
{
  return HNumber(1) / sin(x);
}

HNumber HMath::atan( const HNumber& x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_arctan);
  return result;
};

HNumber HMath::asin( const HNumber& x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_arcsin);
  return result;
};

HNumber HMath::acos( const HNumber& x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_arccos);
  return result;
};

HNumber HMath::sinh( const HNumber& x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_sinh);
  return result;
}

HNumber HMath::cosh( const HNumber& x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_cosh);
  return result;
}

HNumber HMath::tanh( const HNumber& x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_tanh);
  return result;
}

HNumber HMath::arsinh( const HNumber& x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_arsinh);
  return result;
}

HNumber HMath::arcosh( const HNumber& x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_arcosh);
  return result;
}

HNumber HMath::artanh( const HNumber& x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_artanh);
  return result;
}

HNumber HMath::Gamma( const HNumber& x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_gamma);
  return result;
}

HNumber HMath::lnGamma( const HNumber& x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_lngamma);
  return result;
}

HNumber HMath::sign( const HNumber& x )
{
  if (x.isNan())
    return HNumber::nan(checkNaNParam(*x.d));

  return float_getsign(&x.d->fnum);
}

HNumber HMath::nCr( const HNumber& n, const HNumber& r )
{
  floatstruct fn, fr;
  floatnum rnum;

  if( !n.isInteger() || !r.isInteger() )
  {
    int error = checkNaNParam(*n.d, r.d);
    if (error != 0)
      return HNumber::nan(error);
    return HNumber::nan(HMATH_INTEGER_REQUIRED);
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

HNumber HMath::nPr( const HNumber& n, const HNumber& r )
{
/*  if( !n.isInteger() || !r.isInteger() )
  {
    int error = checkNaNParam(*n.d, r.d);
    if (error != 0)
      return HNumber::nan(error);
    return HNumber::nan(HMATH_INTEGER_REQUIRED);
  }*/
  return factorial(n, (n-r+1));
}

HNumber HMath::factorial( const HNumber& x, const HNumber& base )
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

HNumber HMath::binomialPmf( const HNumber & k,
                            const HNumber & n,
                            const HNumber & p )
{
  if ( k.isNan() || ! k.isInteger() || k < 0 || k > n
         || n.isNan() || ! n.isInteger() || n < 0
           || p.isNan() || p < 0 || p > 1 )
    return HNumber::nan();

  return HMath::nCr( n, k ) * HMath::raise( p, k ) *
           HMath::raise( HNumber(1)-p, n-k );
}

HNumber HMath::binomialCdf( const HNumber & k,
                            const HNumber & n,
                            const HNumber & p )
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

HNumber HMath::binomialMean( const HNumber & n,
                             const HNumber & p )
{
  if ( n.isNan() || ! n.isInteger() || n < 0
         || p.isNan() || p < 0 || p > 1 )
    return HNumber::nan();

  return n * p;
}

HNumber HMath::binomialVariance( const HNumber & n,
                                 const HNumber & p )
{
  if ( n.isNan() || ! n.isInteger() || n < 0
         || p.isNan() || p < 0 || p > 1 )
    return HNumber::nan();

  return n * p * ( HNumber(1) - p );
}

HNumber HMath::hypergeometricPmf( const HNumber & k,
                                  const HNumber & N,
                                  const HNumber & M,
                                  const HNumber & n )
{
  if ( k.isNan() || ! k.isInteger() || k < max( 0, M+n-N ) || k > min( M, n )
         || N.isNan() || ! N.isInteger() || N < 0
           || M.isNan() || ! M.isInteger() || M < 0 || M > N
             || n.isNan() || ! n.isInteger() || n < 0 || n > N )
    return HNumber::nan();

  return HMath::nCr( M, k ) * HMath::nCr( N-M, n-k ) / HMath::nCr( N, n );
}

HNumber HMath::hypergeometricCdf( const HNumber & k,
                                  const HNumber & N,
                                  const HNumber & M,
                                  const HNumber & n )
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

HNumber HMath::hypergeometricMean( const HNumber & N,
                                   const HNumber & M,
                                   const HNumber & n )
{
  if ( N.isNan() || ! N.isInteger() || N < 0
         || M.isNan() || ! M.isInteger() || M < 0 || M > N
           || n.isNan() || ! n.isInteger() || n < 0 || n > N )
    return HNumber::nan();

  return n * M / N;
}

HNumber HMath::hypergeometricVariance( const HNumber & N,
                                       const HNumber & M,
                                       const HNumber & n )
{
  if ( N.isNan() || ! N.isInteger() || N < 0
         || M.isNan() || ! M.isInteger() || M < 0 || M > N
           || n.isNan() || ! n.isInteger() || n < 0 || n > N )
    return HNumber::nan();

  return (n * (M/N) * (HNumber(1) - M/N) * (N-n)) / (N - HNumber(1));
}

HNumber HMath::poissonPmf( const HNumber & k,
                           const HNumber & l )
{
  if ( k.isNan() || ! k.isInteger() || k < 0
         || l.isNan() || l < 0 )
    return HNumber::nan();

  return exp( -l ) * raise( l, k ) / factorial( k );
}

HNumber HMath::poissonCdf( const HNumber & k,
                           const HNumber & l )
{
  if ( k.isNan() || ! k.isInteger() || k < 0
         || l.isNan() || l < 0 )
    return HNumber::nan();

  HNumber result( 0 );
  for ( HNumber i( 0 ); i <= k; i += 1 )
    result += exp( -l ) * raise( l, i ) / factorial( i );

  return result;
}

HNumber HMath::poissonMean( const HNumber & l )
{
  if ( l.isNan() || l < 0 )
    return HNumber::nan();

  return l;
}

HNumber HMath::poissonVariance( const HNumber & l )
{
  if ( l.isNan() || l < 0 )
    return HNumber::nan();

  return l;
}

HNumber HMath::erf( const HNumber & x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_erf);
  return result;
}

HNumber HMath::erfc( const HNumber & x )
{
  HNumber result;
  call1Arg(result.d, x.d, float_erfc);
  return result;
}

HNumber HMath::mask ( const HNumber & val, const HNumber & bits )
{
  if ( val.isNan() || bits == 0 || bits >= LOGICRANGE || ! bits.isInteger() )
    return HNumber::nan();
  return val & ~(HNumber(-1) << HNumber(bits));
}

HNumber HMath::sgnext( const HNumber & val, const HNumber & bits )
{
  if ( val.isNan() || bits == 0 || bits >= LOGICRANGE || ! bits.isInteger() )
    return HNumber::nan();
  HNumber ofs;
  ofs = HNumber(LOGICRANGE) - bits;
  return (val << ofs) >> ofs;
}

HNumber HMath::ashr( const HNumber & val, const HNumber & bits )
{
  if ( val.isNan() || bits <= -LOGICRANGE || bits >= LOGICRANGE
       || ! bits.isInteger() )
    return HNumber::nan();
  if (bits >= 0)
    return val >> bits;
  return val << -bits;
}

void HMath::finalize()
{
}

std::ostream& operator<<( std::ostream& s, const HNumber& n )
{
  char* str = HMath::formatFixed( n );
  s << str;
  delete[] str;
  return s;
}
