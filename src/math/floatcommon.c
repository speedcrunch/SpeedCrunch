/* floatcommon.c: convenience functions, based on floatnum. */
/*
    Copyright (C) 2007 - 2009 Wolf Lammen.

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

#include "floatcommon.h"
#include "floatconst.h"
#include "floatlong.h"
#include <string.h>
#include <math.h>

#define MSB (1 << (sizeof(unsigned)*8 - 1))
#define LOGMSB ((301*(sizeof(unsigned)*8-1))/1000)

static char
_chckparam1(
  cfloatnum x,
  int digits,
  int limit,
  int specialval)
{
  if (float_isnan(x))
  {
    float_seterror(NoOperand);
    return 0;
  }
  if ((digits <= 0 || digits > limit) && digits != specialval)
  {
    float_seterror(InvalidPrecision);
    return 0;
  }
  return 1;
}

static char
_chckparam(
  floatnum x,
  int digits,
  int limit,
  int specialval)
{
  if (!_chckparam1(x, digits, limit, specialval))
    return _setnan(x);
  return 1;
}

char
chckmathparam(
  floatnum x,
  int digits)
{
  return _chckparam(x, digits, MATHPRECISION, 1);
}

int
logexp(
  cfloatnum x)
{
  int expx, result;

  expx = float_getexponent(x);
  if (expx < 0)
    expx = -expx;
  result = -1;
  while (expx != 0)
  {
    expx <<= 1;
    ++result;
  }
  return result;
}

void
float_setasciiz(
  floatnum x,
  const char* asciiz)
{
  float_setscientific(x, asciiz, NULLTERMINATED);
}

char
float_divi(
  floatnum quotient,
  cfloatnum dividend,
  int divisor,
  int digits)
{
  floatstruct tmp;
  int result, expx;

  if (!_chckparam1(dividend, digits, maxdigits, INTQUOT))
    return _setnan(quotient);
  if (digits != INTQUOT && (divisor == 1 || divisor == -1))
    return float_muli(quotient, dividend, divisor, digits);
  if (divisor == 10 || divisor == -10)
  {
    expx = float_getexponent(dividend)-1;
    if (expx < -float_getrange() - 1)
      return _seterror(quotient, Underflow);
  }
  float_create(&tmp);
  float_setinteger(&tmp, divisor);
  result = float_div(quotient, dividend, &tmp, digits);
  float_free(&tmp);
  return result;
}

char
float_addi(
  floatnum sum,
  cfloatnum summand1,
  int summand2,
  int digits)
{
  floatstruct tmp;
  int result;

  if (!_chckparam1(summand1, digits, maxdigits, EXACT))
    return _setnan(sum);
  if (summand2 == 0)
    return float_copy(sum, summand1, digits);
  float_create(&tmp);
  float_setinteger(&tmp, summand2);
  result = float_add(sum, summand1, &tmp, digits);
  float_free(&tmp);
  return result;
}

char
float_muli(
  floatnum product,
  cfloatnum factor1,
  int factor2,
  int digits)
{
  floatstruct tmp;
  int result;
  int expx;

  if (!_chckparam1(factor1, digits, maxdigits, EXACT))
    return _setnan(product);
  switch(factor2)
  {
  case 0:
    return _setzero(product);
  case 1:
  case -1:
  case 10:
  case -10:
    expx = float_getexponent(factor1);
    if (factor2 != 1 && factor2 != -1
        && ++expx > float_getrange())
      return _seterror(product, Overflow);
    result = float_copy(product, factor1, digits);
    if (factor2 < 0)
      float_neg(product);
    float_setexponent(product, expx);
    return result;
  case 2:
  case -2:
    result = float_add(product, factor1, factor1, digits);
    if (factor2 < 0)
      float_neg(product);
    return result;
  }
  float_create(&tmp);
  float_setinteger(&tmp, factor2);
  result = float_mul(product, factor1, &tmp, digits);
  float_free(&tmp);
  return result;
}

int
leadingdigits(
  cfloatnum x,
  int digits)
{
  int i;
  unsigned tmp, ovfl;
  char buf[LOGMSB+1];
  const unsigned msb = MSB;

  if (digits <= 0 || digits > (int)LOGMSB+1 || float_isnan(x) || float_iszero(x))
    return 0;
  memset(buf, '0', digits);
  float_getsignificand(buf, digits, x);
  tmp = 0;
  for(i = 0; i < digits; ++i)
  {
    ovfl = 10;
    if (_longmul(&tmp, &ovfl))
    {
      ovfl = buf[i] - '0';
      _longadd(&tmp, &ovfl);
    }
    if (ovfl != 0)
      return 0;
  }
  if (float_getsign(x) < 0)
  {
    if (tmp > msb)
      return 0;
    if (tmp == msb)
      return (int)tmp;
    return -(int)tmp;
  }
  if (tmp >= msb)
    return 0;
  return (int)tmp;
}

int
float_abscmp(
  floatnum x,
  floatnum y)
{
  signed char sx, sy;
  int result;

  sx = float_getsign(x);
  sy = float_getsign(y);
  float_abs(x);
  float_abs(y);
  result = float_cmp(x, y);
  float_setsign(x, sx);
  float_setsign(y, sy);
  return result;
}

int
float_relcmp(
  floatnum x,
  floatnum y,
  int digits)
{
  /* do not simply use float_sub, because of overflow/underflow */
  floatstruct tmp;
  int result;
  int expx, expy, expdiff;

  result = float_cmp(x, y);
  if (result == 0 || float_getlength(x) == 0
     || float_getlength(y) == 0
     || float_getsign(x) != float_getsign(y))
    return result;
  expx = float_getexponent(x);
  expy = float_getexponent(y);
  expdiff = expx - expy;
  if (expdiff >= 2 || expdiff < -2)
    return result;
  float_create(&tmp);
  if (result > 0)
  float_setexponent(x, 0);
  float_setexponent(y, expy - expx);
  float_sub(&tmp, x, y, 2);
  if ((result * float_getsign(x)) > 0)
    float_div(&tmp, &tmp, x, 2);
  else
    float_div(&tmp, &tmp, y, 2);
  if (float_getexponent(&tmp) < -digits)
    result = 0;
  float_setexponent(x, expx);
  float_setexponent(y, expy);
  float_free(&tmp);
  return result;
}

char
float_reciprocal(
  floatnum x,
  int digits)
{
  return float_div(x, &c1, x, digits);
}

char
float_isinteger(
  cfloatnum x)
{
  return !float_isnan(x)
         && float_getlength(x) <= float_getexponent(x) + 1;
}

int
float_asinteger(
  cfloatnum x)
{
  return leadingdigits(x, float_getexponent(x)+1);
}

void
float_checkedround(
  floatnum x,
  int digits)
{
  floatstruct tmp;
  int saveerr;

  saveerr = float_geterror();
  float_create(&tmp);
  if (float_round(&tmp, x, digits, TONEAREST))
    float_move(x, &tmp);
  float_free(&tmp);
  float_geterror();
  float_seterror(saveerr);
}

void
float_addexp(
  floatnum x,
  int smd)
{
  float_setexponent(x, float_getexponent(x) + smd);
}

char
float_isodd(
  floatnum x)
{
  return (float_getdigit(x, float_getexponent(x)) & 1) != 0;
}

char
float_roundtoint(
  floatnum x,
  roundmode mode)
{
  signed char value = 0;
  signed char sign;
  char digit;

  if (float_isnan(x))
    return float_int(x); /* sets float_error */
  if (float_getexponent(x) >= 0)
    return float_round(x, x, float_getexponent(x) + 1, mode);
  sign = float_getsign(x);
  switch (mode)
  {
  case TONEAREST:
    digit = float_getdigit(x, 0);
    if (digit < 5 || (digit == 5 && float_getlength(x) == 1))
      value = 0;
    else
      value = sign;
    break;
  case TOINFINITY:
    value = sign;
    break;
  case TOPLUSINFINITY:
    value = sign > 0? 1 : 0;
    break;
  case TOMINUSINFINITY:
    value = sign > 0? 0 : -1;
    break;
  case TOZERO:
    value = 0;
    break;
  }
  switch (value)
  {
  case 0:
    float_setzero(x);
    break;
  case 1:
    float_copy(x, &c1, EXACT);
    break;
  case -1:
    float_copy(x, &cMinus1, EXACT);
    break;
  }
  return 1;
}

static float _ipwr(float x, int exp){
  int e = exp < 0? -exp : exp;
  double pwr = x;
  if ((e & 1) == 0)
    x = 1;
  while (e >>= 1){
    pwr *= pwr;
    if ((exp & 1) != 0)
      x *= pwr;
  }
  return exp < 0? 1/x : x;
}

/* returns x as a float. Only the first 6 digits
   contribute to the result. The exponent has to
   be in the valid range of a float */

float float_asfloat(cfloatnum x){
  return leadingdigits(x, 6)/100000.0 * _ipwr(10, float_getexponent(x));
}

void float_setfloat(floatnum dest, float x){
  int exp = aprxlog10(x);
  // use two assignments to avoid overflow
  x *= _ipwr(10, -exp);
  x *= 100000000;

  float_setinteger(dest, (int)x);
  float_addexp(dest, exp - 8);
}

/* Somehow math.h cannot always be included with the full set of
   ISO C99 math functions enabled. So use the approximations below.
   These functions are used to get first guess start values for
   iterative algorithms, or to estimate round off errors, or to find
   the approximative size of a summand. They need not be
   accurate to more than, say, 0.1% */

float aprxsqrt(float x){
  int exp, i;
  float x2 = 2 * frexp(x, &exp) - 1;
  float result = (0.5 - 0.125 * x2) * x2 + 1;
  x2 += 1;
  for (i = 0; ++i <= 2;)
    result = 0.5 * (result + x2 / result);
  if ((exp & 1) == 0)
    result *= M_SQRT2;
  return result * _ipwr(2, (exp - 1) >> 1);
}

float aprxln(float x){
  /* The evaluation of approxlog(x) is based
  on an approximation suggested by Abramowitz, Stegun,
  Handbook of mathematical functions.
  The returned logarithm is valid to
  5 (decimal) digits after the decimal point. */
  int exp;

  x = 2 * frexpf(fabs(x), &exp) - 1;
  return ((((0.03215845 * x
         - 0.13606275) * x
         + 0.28947478) * x
         - 0.49190896) * x
         + 0.99949556) * x
         + (exp - 1) * M_LN2;
}

float aprxlog2(float x){
  return aprxln(x) * M_LOG2E;
}

float aprxlog10(float x){
  return aprxln(x) * M_LOG10E;
}

float aprxlog10fn(cfloatnum x){
 return float_getexponent(x)
        + aprxlog10(leadingdigits(x, 5)) - 4;
}

float aprxlngamma(float x){
  return (x-0.5) * aprxln(x) - x + 0.9189385332f;
}
