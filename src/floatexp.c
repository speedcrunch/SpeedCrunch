/* floatexp.c: exponential function and friends, based on floatnum. */
/*
    Copyright (C) 2007 Wolf Lammen.

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
#include "floatconst.h"
#include "floatcommon.h"
#include "floatseries.h"
#include "floatexp.h"

/* uses the addition theorem
   cosh(2x)-1 == 2*(cosh x - 1)*(cosh x + 1)
   to reduce the argument to the range |x| < 0.01.
   Starting with x == 1, you need 7 reduction steps
   to achieve the desired magnitude.
   The relative error is < 8e-100 for a 100 digit result.
   The return value is 0, if the result underflows.
   |x| < 1, otherwise the final process, where
   the reductions are unwinded, becomes too
   unstable */
char
_coshminus1lt1(
  floatnum x,
  int digits)
{
  floatstruct tmp;
  int reductions;

  if (float_iszero(x))
    return 1;
  float_abs(x);
  reductions = 0;
  while(float_getexponent(x) >= -2)
  {
    float_mul(x, x, &c1Div2, digits+1);
    ++reductions;
  }
  if (!coshminus1near0(x, digits) && reductions == 0)
    return !float_iszero(x);
  float_create(&tmp);
  for(; reductions-- > 0;)
  {
    float_mul(&tmp, x, x, digits);
    float_add(x, x, x, digits+2);
    float_add(x, x, &tmp, digits+2);
    float_add(x, x, x, digits+2);
  }
  float_free(&tmp);
  return 1;
}

/* sinh x == sqrt((cosh x - 1) * (cosh x + 1)) */
static void
_sinhfromcoshminus1(
  floatnum x,
  int digits)
{
  floatstruct tmp;

  float_create(&tmp);
  float_add(&tmp, x, &c2, digits);
  float_mul(x, &tmp, x, digits+1);
  float_sqrt(x, digits+1);
  float_free(&tmp);
}

/* sinh x for |x| < 1. Derived from cosh x - 1.
   The relative error is < 8e-100 for a 100 digit result */
void
_sinhlt1(
  floatnum x,
  int digits)
{
  signed char sgn;
  if (float_getexponent(x) < -digits)
    /* for very small x: sinh(x) approx. == x. */
    return;
  sgn = float_getsign(x);
  _coshminus1lt1(x, digits);
  _sinhfromcoshminus1(x, digits);
  float_setsign(x, sgn);
}

/* evaluates exp(x) - 1. This value can be obtained
   by exp(x) - 1 == sinh(x) + cosh(x) - 1
   relative error < 8e-100 for a 100 digit result */
void
_expminus1lt1(
  floatnum x,
  int digits)
{
  floatstruct tmp;
  signed char sgn;

  if (float_getexponent(x) < -digits || float_iszero(x))
    /* for very small x: exp(x)-1 approx.== x */
    return;
  float_create(&tmp);
  sgn = float_getsign(x);
  _coshminus1lt1(x, digits);
  float_copy(&tmp, x, EXACT);
  _sinhfromcoshminus1(x, digits);
  float_setsign(x, sgn);
  float_add(x, x, &tmp, digits+1);
  float_free(&tmp);
}

/* exp(x) for 0 <= x < ln 10
   relative error < 5e-100 */
void
_expltln10(
  floatnum x,
  int digits)
{
  int expx;
  int factor;
  char sgnf;

  expx = float_getexponent(x);
  factor = 1;
  if (expx >= -1)
  {
    sgnf = leadingdigits(x, 2 + expx);
    if (sgnf > 4)
    {
      if (sgnf < 9)
      {
        factor = 2;
        float_sub(x, x, &cLn2, digits+1);
      }
      else if (sgnf < 14)
      {
        factor = 3;
        float_sub(x, x, &cLn3, digits+1);
      }
      else if (sgnf < 21)
      {
        factor = 7;
        float_sub(x, x, &cLn7, digits+1);
      }
      else
      {
        factor = 10;
        float_sub(x, x, &cLn10, digits+1);
      }
    }
  }
  _expminus1lt1(x, digits);
  float_add(x, x, &c1, digits+1);
  if (factor != 1)
    float_muli(x, x, factor, digits+1);
}

/* exp(x) for all x. Underflow or overflow is
   indicated by the return value (0, if error)
   relative error for 100 digit results is 5e-100 */
char
_exp(
  floatnum x,
  int digits)
{
  floatstruct exp, tmp;
  int expx, extra;
  char ok;

  if (float_iszero(x))
  {
    float_copy(x, &c1, EXACT);
    return 1;
  }
  expx = float_getexponent(x);
  if (expx >= (int)(BITS_IN_EXP >> 1))
    /* clear overflow or underflow */
    return 0;

  float_create(&exp);
  float_create(&tmp);
  float_setzero(&exp);

  if (expx >= 0)
  {
    float_div(&exp, x, &cLn10, expx+1);
    float_int(&exp);
    extra = float_getexponent(&exp)+1;
    float_mul(&tmp, &exp, &cLn10, digits+extra);
    float_sub(x, x, &tmp, digits+extra);
    if (float_cmp(x, &cLn10) >= 0)
    {
      float_add(&exp, &exp, &c1, EXACT);
      float_sub(x, x, &cLn10, digits);
    }
  }
  if (float_getsign(x) < 0)
  {
    float_sub(&exp, &exp, &c1, EXACT);
    float_add(x, x, &cLn10, digits);
  }
  /* when we get here 0 <= x < ln 10 */
  _expltln10(x, digits);
  /* just in case rounding leads to a value >= 10 */
  expx = float_getexponent(x);
  if (expx != 0)
    float_addi(&exp, &exp, expx, EXACT);

  ok = 1;
  if (!float_iszero(&exp))
  {
    expx = float_asinteger(&exp);
    ok = expx != 0;
    float_setexponent(x, expx);
  }
  float_free(&exp);
  float_free(&tmp);
  return ok && !float_isnan(x);
}

static char
_0_5exp(
  floatnum x,
  int digits)
{
  float_sub(x, x, &cLn2, digits + (3*logexp(x)/10)+1);
  return _exp(x, digits);
}

/* exp(x)-1 for all x. Overflow is
   indicated by the return value (0, if error)
   relative error for 100 digit results is 8e-100 */
char
_expminus1(
  floatnum x,
  int digits)
{
  int expr;

  if (float_abscmp(x, &c1Div2) < 0)
  {
    _expminus1lt1(x, digits);
    return 1;
  }
  if (float_getsign(x) < 0)
  {
    expr = (2*float_getexponent(x)/5);
    if (expr >= digits)
      float_setinteger(x, -1);
    else
    {
      _exp(x, digits-expr);
      float_sub(x, x, &c1, digits);
    }
    return 1;
  }
  if (!_exp(x, digits))
    return 0;
  float_sub(x, x, &c1, digits);
  return 1;
}

static void
_addreciproc(
  floatnum x,
  int digits,
  signed char sgn)
{
  floatstruct tmp;
  int expx;

  expx = float_getexponent(x);
  if (2*expx < digits)
  {
    float_create(&tmp);
    float_muli(&tmp, x, 4, digits-2*expx);
    float_reciprocal(&tmp, digits-2*expx);
    float_setsign(&tmp, sgn);
    float_add(x, x, &tmp, digits+1);
    float_free(&tmp);
  }
}

/* cosh(x)-1 for all x. Underflow or overflow is
   indicated by the return value (0, if error)
   relative error for 100 digit results is 6e-100 */

char
_coshminus1(
  floatnum x,
  int digits)
{
  if (float_getexponent(x) < 0 || float_iszero(x))
    return _coshminus1lt1(x, digits);
  if(!_0_5exp(x, digits))
    return 0;
  _addreciproc(x, digits, 1);
  float_sub(x, x, &c1, digits);
  return 1;
}

/* sinh(x) for all x. Overflow is
   indicated by the return value (0, if error)
   relative error for 100 digit results is < 8e-100 */
char
_sinh(
  floatnum x,
  int digits)
{
  if (float_getexponent(x) < 0 || float_iszero(x))
    _sinhlt1(x, digits);
  else
  {
    if(!_0_5exp(x, digits))
      return 0;
    _addreciproc(x, digits, -1);
  }
  return 1;
}

/* tanh(x) for |x| <= 0.5.
   relative error for 100 digit results is < 7e-100 */
void
_tanhlt0_5(
  floatnum x,
  int digits)
{
  floatstruct tmp;
  signed char sgn;

  float_create(&tmp);
  sgn = float_getsign(x);
  float_abs(x);
  float_add(x, x, x, digits+1);
  _expminus1lt1(x, digits);
  float_add(&tmp, x, &c2, digits);
  float_div(x, x, &tmp, digits);
  float_setsign(x, sgn);
  float_free(&tmp);
}

/* tanh(x)-1 for x > 0.
   relative error for 100 digit results is < 9e-100 */
char
_tanhminus1gt0(
  floatnum x,
  int digits)
{
  if (float_add(x, x, x, digits+1) && _0_5exp(x, digits))
  {
    float_add(x, x, &c1Div2, digits+1);
    float_reciprocal(x, digits+1);
    float_setsign(x, -1);
    return 1;
  }
  return 0;
}

void
_tanhgt0_5(
  floatnum x,
  int digits)
{
  int expx;

  expx = float_getexponent(x);
  if (5*expx >= digits)
    float_copy(x, &c1, EXACT);
  else
  {
    _tanhminus1gt0(x, digits - 5*expx);
    float_add(x, x, &c1, digits);
  }
}

char
_power10(
  floatnum x,
  int digits)
{
  int exp;

  if (float_isinteger(x))
  {
    exp = float_asinteger(x);
    float_copy(x, &c1, EXACT);
    if (float_iszero(x))
      return 1;
    float_setexponent(x, exp);
    return exp != 0 && !float_isnan(x);
  }
  return float_mul(x, x, &cLn10, digits+2) && _exp(x, digits);
}
