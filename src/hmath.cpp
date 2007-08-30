/* HMath: C++ high precision math routines
   Copyright (C) 2004 Ariya Hidayat <ariya.hidayat@gmail.com>
                 2007 Helder Correia <helder.pereira.correia@gmail.com>
   Last update: May 24, 2007
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

#include "hmath.h"
#include "floatnum.h"
#include "floatconst.h"
#include "floatconvert.h"
#include "floatcommon.h"
#include "floatio.h"
#include "floathmath.h"

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

class HNumberPrivate
{
public:
  floatstruct fnum;
  char format;
};

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
roundResult( floatnum x )
{
  if (!float_isnan(x)) /* avoids setting float_error */
    float_round(x, x, HMATH_WORKING_PREC, TONEAREST);
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
    float_error = FLOAT_UNSTABLE;
  }
}

HNumber::HNumber(): d(0)
{
  h_init();
  d = new HNumberPrivate;
  d->format = 0;
  float_create(&d->fnum);
}

HNumber::HNumber( const HNumber& hn ): d(0)
{
  h_init();
  d = new HNumberPrivate;
  float_create(&d->fnum);
  operator=( hn );
}

HNumber::HNumber( int i ): d(0)
{
  h_init();
  d = new HNumberPrivate;
  d->format = 0;
  float_create(&d->fnum);
  float_setinteger(&d->fnum, i);
}

HNumber::HNumber( const char* str ): d(0)
{
  t_itokens tokens;
  const char* s;

  h_init();
  d = new HNumberPrivate;
  d->format = 0;
  float_create(&d->fnum);
  s = str;
  if (parse(&tokens, &s) == IO_NO_ERROR && *s == 0)
    float_in(&d->fnum, &tokens);
}

HNumber::~HNumber()
{
  float_free(&d->fnum);
  delete d;
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

HNumber HNumber::nan()
{
  HNumber n;
  return n;
}

int HNumber::toInt()
{
  return float_asinteger(&d->fnum);
}

HNumber& HNumber::operator=( const HNumber& hn )
{
  d->format = hn.format();
  float_copy(&d->fnum, &hn.d->fnum, EXACT);
  return *this;
}

bool static checkfullcancellation( floatnum op1, floatnum op2,
                                   floatnum r )
{
  int expr;

  if (float_getlength(op1) + float_getlength(op2) + float_getlength(r) == 0)
    /* NaN or zero involved in computation */
    return 0;
  expr = float_getexponent(r);
  return float_getexponent(op1) - expr >= HMATH_WORKING_PREC - 1
      || float_getexponent(op1) - expr >= HMATH_WORKING_PREC - 1;
}

HNumber HNumber::operator+( const HNumber& num ) const
{
  floatnum op1, op2, r;
  HNumber result;
  op1 = &d->fnum;
  op2 = &num.d->fnum;
  r = &result.d->fnum;
  float_add(r, op1, op2, HMATH_EVAL_PREC);
  if (float_getsign(op1) + float_getsign(op2) == 0
      && checkfullcancellation(op1, op2, r))
    /* underflow due to cancellation */
    float_setzero(r);
  else
    roundResult(&result.d->fnum);
  return result;
}

HNumber& HNumber::operator+=( const HNumber& num )
{
  HNumber n = HNumber(*this) + num;
  operator=( n );
  return *this;
}

HNumber HNumber::operator-( const HNumber& num ) const
{
  floatnum op1, op2, r;
  HNumber result;
  op1 = &d->fnum;
  op2 = &num.d->fnum;
  r = &result.d->fnum;
  float_sub(r, op1, op2, HMATH_EVAL_PREC);
  if (float_getsign(op1) - float_getsign(op2) == 0
      && checkfullcancellation(op1, op2, r))
    /* underflow due to cancellation */
    float_setzero(r);
  else
    roundResult(r);
  return result;
}

HNumber& HNumber::operator-=( const HNumber& num )
{
  HNumber n = HNumber(*this) - num;
  operator=( n );
  return *this;
}

HNumber HNumber::operator*( const HNumber& num ) const
{
  HNumber result;
  float_mul(&result.d->fnum, &d->fnum, &num.d->fnum, HMATH_EVAL_PREC);
  roundResult(&result.d->fnum);
  return result;
}

HNumber& HNumber::operator*=( const HNumber& num )
{
  HNumber n = HNumber(*this) * num;
  operator=( n );
  return *this;
}

HNumber HNumber::operator/( const HNumber& num ) const
{
  HNumber result;
  float_div(&result.d->fnum, &d->fnum, &num.d->fnum, HMATH_EVAL_PREC);
  roundResult(&result.d->fnum);
  return result;
}

HNumber& HNumber::operator/=( const HNumber& num )
{
  HNumber n = HNumber(*this) / num;
  operator=( n );
  return *this;
}

HNumber HNumber::operator%( const HNumber& num ) const
{
  floatstruct tmp;

  HNumber result;
  float_create(&tmp);
  float_divmod(&tmp, &result.d->fnum, &d->fnum, &num.d->fnum, INTQUOT);
  float_free(&tmp);
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
  float_and(&result.d->fnum, &d->fnum, &num.d->fnum);
  return result;
}

HNumber& HNumber::operator&=( const HNumber& num )
{
  HNumber n = HNumber(*this) & num;
  operator=( n );
  return *this;
}

HNumber HNumber::operator|( const HNumber& num ) const
{
  HNumber result;
  float_or(&result.d->fnum, &d->fnum, &num.d->fnum);
  return result;
}

HNumber& HNumber::operator|=( const HNumber& num )
{
  HNumber n = HNumber(*this) | num;
  operator=( n );
  return *this;
}

HNumber HNumber::operator^( const HNumber& num ) const
{
  HNumber result;
  float_xor(&result.d->fnum, &d->fnum, &num.d->fnum);
  return result;
}

HNumber& HNumber::operator^=( const HNumber& num )
{
  HNumber n = HNumber(*this) ^ num;
  operator=( n );
  return *this;
}

HNumber HNumber::operator~() const
{
  HNumber result = HNumber(*this);
  float_not(&result.d->fnum);
  return result;
}

HNumber HNumber::operator<<( const HNumber& num ) const
{
  HNumber result;
  float_shl(&result.d->fnum, &d->fnum, &num.d->fnum);
  return result;
}

HNumber HNumber::operator>>( const HNumber& num ) const
{
  HNumber result;
  float_shr(&result.d->fnum, &d->fnum, &num.d->fnum);
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

#if 1

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
  // demo version
  result = _doFormat(x, base, expbase, IO_MODE_FIXPOINT,
                     scale, IO_FLAG_SUPPRESS_PLUS
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

HNumber HMath::add( const HNumber& n1, const HNumber& n2 )
{
  HNumber result = n1 + n2;
  return result;
}

HNumber HMath::sub( const HNumber& n1, const HNumber& n2 )
{
  HNumber result = n1 - n2;
  return result;
}

HNumber HMath::mul( const HNumber& n1, const HNumber& n2 )
{
  HNumber result = n1 * n2;
  return result;
}

HNumber HMath::div( const HNumber& n1, const HNumber& n2 )
{
  HNumber result = n1 / n2;
  return result;
}

int HMath::compare( const HNumber& n1, const HNumber& n2 )
{
  return float_relcmp(&n1.d->fnum, &n2.d->fnum, HMATH_EVAL_PREC-1);
}

HNumber HMath::max( const HNumber& n1, const HNumber& n2 )
{
  if ( n1.isNan() || n2.isNan() )
    return HNumber::nan();

  if ( n1 >= n2 )
    return n1;
  else
    return n2;
}

HNumber HMath::min( const HNumber& n1, const HNumber& n2 )
{
  if ( n1.isNan() || n2.isNan() )
    return HNumber::nan();

  if ( n1 <= n2 )
    return n1;
  else
    return n2;
}

HNumber HMath::abs( const HNumber& n )
{
  HNumber r( n );
  float_abs(&r.d->fnum);
  return r;
}

HNumber HMath::negate( const HNumber& n )
{
  HNumber result( n );
  float_neg(&result.d->fnum);
  return result;
}

HNumber HMath::round( const HNumber& n, int prec )
{
  int exp;
  floatnum rnum;
  HNumber result(n);

  rnum = &result.d->fnum;
  exp = float_getexponent(rnum);
  if (exp + prec < HMATH_WORKING_PREC)
  {
    float_addexp(rnum, prec);
    float_roundtoint(rnum, TONEAREST);
    float_addexp(rnum, -prec);
  }
  return result;
}

HNumber HMath::trunc( const HNumber& n, int prec )
{
  int exp;
  floatnum rnum;
  HNumber result(n);

  rnum = &result.d->fnum;
  exp = float_getexponent(rnum);
  if (exp + prec < HMATH_WORKING_PREC)
  {
    float_addexp(rnum, prec);
    float_roundtoint(rnum, TOZERO);
    float_addexp(rnum, -prec);
  }
  return result;
}

HNumber HMath::integer( const HNumber& n )
{
  HNumber result(n);
  float_int(&result.d->fnum);
  return result;
}

HNumber HMath::frac( const HNumber& n )
{
  HNumber result(n);
  float_frac(&result.d->fnum);
  return result;
}

HNumber HMath::floor( const HNumber& n )
{
  HNumber r(n);

  float_roundtoint(&r.d->fnum, TOMINUSINFINITY);
  return r;
}

HNumber HMath::ceil( const HNumber& n )
{
  HNumber r(n);

  float_roundtoint(&r.d->fnum, TOPLUSINFINITY);
  return r;
}

HNumber HMath::gcd( const HNumber& n1, const HNumber& n2 )
{
  if( n1.isNan() || n2.isNan() )
    return HNumber::nan();

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
  HNumber r(n);
  float_sqrt(&r.d->fnum, HMATH_EVAL_PREC);
  roundResult(&r.d->fnum);
  return r;
}

HNumber HMath::cbrt( const HNumber& n )
{
  int expn, digits;
  floatstruct a, q;
  floatnum rnum;
  signed char sign;

  if( n.isNan() )
    return HNumber::nan();
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
  HNumber r;
  float_raisei(&r.d->fnum, &n1.d->fnum, n, HMATH_EVAL_PREC);
  roundResult(&r.d->fnum);
  return r;
}

HNumber HMath::raise( const HNumber& n1, const HNumber& n2  )
{
  HNumber r;
  float_raise(&r.d->fnum, &n1.d->fnum, &n2.d->fnum, HMATH_EVAL_PREC);
  roundResult(&r.d->fnum);
  return r;
}

HNumber HMath::exp( const HNumber& x )
{
  HNumber r(x);
  float_exp(&r.d->fnum, HMATH_EVAL_PREC);
  roundResult(&r.d->fnum);
  return r;
};

HNumber HMath::ln( const HNumber& x )
{
  floatnum rnum;

  HNumber r(x);
  rnum = &r.d->fnum;
  float_ln(rnum, HMATH_EVAL_PREC);
  if (float_getexponent(rnum) < -HMATH_WORKING_PREC + 3)
    checkpoleorzero(rnum, &x.d->fnum);
  roundResult(rnum);
  return r;
}

HNumber HMath::log( const HNumber& x )
{
  floatnum rnum;

  HNumber r(x);
  rnum = &r.d->fnum;
  float_log(rnum, HMATH_EVAL_PREC);
  if (float_getexponent(rnum) < -HMATH_WORKING_PREC + 3)
    checkpoleorzero(rnum, &x.d->fnum);
  roundResult(rnum);
  return r;
}

HNumber HMath::lg( const HNumber& x )
{
  floatnum rnum;

  HNumber r(x);
  rnum = &r.d->fnum;
  float_lg(rnum, HMATH_EVAL_PREC);
  if (float_getexponent(rnum) < -HMATH_WORKING_PREC + 3)
    checkpoleorzero(rnum, &x.d->fnum);
  roundResult(rnum);
  return r;
}

HNumber HMath::sin( const HNumber& x )
{
  HNumber r(x);
  float_sin(&r.d->fnum, HMATH_EVAL_PREC);
  checkpoleorzero(&r.d->fnum, &x.d->fnum);
  roundResult(&r.d->fnum);
  return r;
}


HNumber HMath::cos( const HNumber& x )
{
  HNumber r(x);
  float_cos(&r.d->fnum, HMATH_EVAL_PREC);
  checkpoleorzero(&r.d->fnum, &x.d->fnum);
  roundResult(&r.d->fnum);
  return r;
}

HNumber HMath::tan( const HNumber& x )
{
  HNumber r(x);
  float_tan(&r.d->fnum, HMATH_EVAL_PREC);
  checkpoleorzero(&r.d->fnum, &x.d->fnum);
  roundResult(&r.d->fnum);
  return r;
}

HNumber HMath::cot( const HNumber& x )
{
  return HMath::cos(x) / HMath::sin(x);
}

HNumber HMath::sec( const HNumber& x )
{
  return HNumber(1) / HMath::cos(x);
}

HNumber HMath::csc( const HNumber& x )
{
  return HNumber(1) / HMath::sin(x);
}

HNumber HMath::atan( const HNumber& x )
{
  HNumber r(x);
  float_arctan(&r.d->fnum, HMATH_EVAL_PREC);
  roundResult(&r.d->fnum);
  return r;
};

HNumber HMath::asin( const HNumber& x )
{
  HNumber r(x);
  float_arcsin(&r.d->fnum, HMATH_EVAL_PREC);
  roundResult(&r.d->fnum);
  return r;
};

HNumber HMath::acos( const HNumber& x )
{
  HNumber r(x);
  float_arccos(&r.d->fnum, HMATH_EVAL_PREC);
  roundResult(&r.d->fnum);
  return r;
};

HNumber HMath::sinh( const HNumber& x )
{
  HNumber r(x);
  float_sinh(&r.d->fnum, HMATH_EVAL_PREC);
  roundResult(&r.d->fnum);
  return r;
}

HNumber HMath::cosh( const HNumber& x )
{
  HNumber r(x);
  float_cosh(&r.d->fnum, HMATH_EVAL_PREC);
  roundResult(&r.d->fnum);
  return r;
}

HNumber HMath::tanh( const HNumber& x )
{
  HNumber r(x);
  float_tanh(&r.d->fnum, HMATH_EVAL_PREC);
  roundResult(&r.d->fnum);
  return r;
}

HNumber HMath::arsinh( const HNumber& x )
{
  HNumber r(x);
  float_arsinh(&r.d->fnum, HMATH_EVAL_PREC);
  roundResult(&r.d->fnum);
  return r;
}

HNumber HMath::arcosh( const HNumber& x )
{
  HNumber r(x);
  float_arcosh(&r.d->fnum, HMATH_EVAL_PREC);
  roundResult(&r.d->fnum);
  return r;
}

HNumber HMath::artanh( const HNumber& x )
{
  HNumber r(x);
  float_artanh(&r.d->fnum, HMATH_EVAL_PREC);
  roundResult(&r.d->fnum);
  return r;
}

HNumber HMath::Gamma( const HNumber& x )
{
  HNumber r(x);
  float_gamma(&r.d->fnum, HMATH_EVAL_PREC);
  roundResult(&r.d->fnum);
  return r;
}

HNumber HMath::lnGamma( const HNumber& x )
{
  HNumber r(x);
  float_lngamma(&r.d->fnum, HMATH_EVAL_PREC);
  roundResult(&r.d->fnum);
  return r;
}

HNumber HMath::sign( const HNumber& x )
{
  if( x.isNan() )
    return HNumber::nan();

  HNumber result( float_getsign(&x.d->fnum) );
  return result;
}

HNumber HMath::nCr( const HNumber& n, const HNumber& r )
{
  floatstruct fn, fr;
  floatnum rnum;

  if( !n.isInteger() || !r.isInteger() || r > n || r < 0)
    return HNumber::nan();

  if (r > 50 && n-r > 50)
  {
    HNumber result(n);
    rnum = &result.d->fnum;

    float_create(&fn);
    float_create(&fr);
    float_copy(&fr, &r.d->fnum, HMATH_EVAL_PREC);
    float_copy(&fn, rnum, EXACT);
    float_sub(rnum, rnum, &fr, HMATH_EVAL_PREC);
    float_add(&fn, &fn, &c1, HMATH_EVAL_PREC);
    float_add(&fr, &fr, &c1, HMATH_EVAL_PREC);
    float_add(rnum, rnum, &c1, HMATH_EVAL_PREC);
    float_lngamma(&fn, HMATH_EVAL_PREC);
    float_lngamma(&fr, HMATH_EVAL_PREC);
    float_lngamma(rnum, HMATH_EVAL_PREC);
    float_add(rnum, rnum, &fr, HMATH_EVAL_PREC);
    float_sub(rnum, &fn, rnum, HMATH_EVAL_PREC);
    float_exp(rnum, HMATH_EVAL_PREC);
    float_free(&fn);
    float_free(&fr);
    return result;
  }
  if (r > n/2)
    return factorial(n, r+1) / factorial((n-r), 1);
  else
    return factorial(n, (n-r+1)) / factorial(r, 1);
}

HNumber HMath::nPr( const HNumber& n, const HNumber& r )
{
  if (!n.isInteger() || !r.isInteger() || r > n || r < 0)
    return HNumber::nan();
  return factorial(n, (n-r+1));
}

HNumber HMath::factorial( const HNumber& x, const HNumber& base )
{
  floatstruct tmp;

  if (float_cmp(&c1, &base.d->fnum) == 0)
  {
    HNumber r(x);
    float_factorial(&r.d->fnum, HMATH_EVAL_PREC);
    roundResult(&r.d->fnum);
    return r;
  }
  HNumber r(base);
  float_create(&tmp);
  float_sub(&tmp, &x.d->fnum, &base.d->fnum, HMATH_EVAL_PREC);
  float_add(&tmp, &tmp, &c1, HMATH_EVAL_PREC);
  float_pochhammer(&r.d->fnum, &tmp, HMATH_EVAL_PREC);
  roundResult(&r.d->fnum);
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
  if ( k.isNan() || ! k.isInteger() || k < 0 || k > n
         || n.isNan() || ! n.isInteger() || n < 0
           || p.isNan() || p < 0 || p > 1 )
    return HNumber::nan();

  HNumber result( 0 );
  for ( HNumber i( 0 ); i <= k; i += 1 )
    result += HMath::nCr( n, i ) * HMath::raise( p, i )
                * HMath::raise( HNumber(1)-p, n-i );
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

  return exp( l*(-1) ) * raise( l, k ) / factorial( k );
}

HNumber HMath::poissonCdf( const HNumber & k,
                           const HNumber & l )
{
  if ( k.isNan() || ! k.isInteger() || k < 0
         || l.isNan() || l < 0 )
    return HNumber::nan();

  HNumber result( 0 );
  for ( HNumber i( 0 ); i <= k; i += 1 )
    result += exp( l*(-1) ) * raise( l, i ) / factorial( i );

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
