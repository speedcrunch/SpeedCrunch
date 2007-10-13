/* floattrig.c: trigonometry functions, based on floatnum. */
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

#include "floattrig.h"
#include "floatseries.h"
#include "floatconst.h"
#include "floatcommon.h"
#include "floatconfig.h"

/* evaluates arctan x for |x| <= 1
   relative error for a 100 digit result is 6e-100 */
void
_arctanlt1(
  floatnum x,
  int digits)
{
  floatstruct tmp;
  int reductions;

  if (float_iszero(x))
    return;
  float_create(&tmp);
  reductions = 0;
  while(float_getexponent(x) >= -2)
  {
    float_mul(&tmp, x, x, digits);
    float_add(&tmp, &tmp, &c1, digits+2);
    float_sqrt(&tmp, digits);
    float_add(&tmp, &tmp, &c1, digits+1);
    float_div(x, x, &tmp, digits);
    ++reductions;
  }
  arctannear0(x, digits);
  for (;reductions-- > 0;)
    float_add(x, x, x, digits+1);
  float_free(&tmp);
}

/* evaluates arctan x for all x. The result is in the
   range -pi/2 < result < pi/2
   relative error for a 100 digit result is 9e-100 */
void
_arctan(
  floatnum x,
  int digits)
{
  signed char sgn;

  if (float_abscmp(x, &c1) > 0)
  {
    sgn = float_getsign(x);
    float_abs(x);
    float_reciprocal(x, digits);
    _arctanlt1(x, digits);
    float_sub(x, &cPiDiv2, x, digits+1);
    float_setsign(x, sgn);
  }
  else
    _arctanlt1(x, digits);
}

/* evaluates arccos(1+x) for -0.5 <= x <= 0
   arccos(1+x) = arctan(sqrt(-x*(2+x))/(1+x))
   the relative error of a 100 digit result is < 5e-100 */
void
_arccosxplus1lt0_5(
  floatnum x,
  int digits)
{
  floatstruct tmp;

  float_create(&tmp);
  float_add(&tmp, x, &c2, digits+1);
  float_mul(x, x, &tmp, digits+1);
  float_setsign(x, 1);
  float_sqrt(x, digits);
  float_sub(&tmp, &tmp, &c1, digits);
  float_div(x, x, &tmp, digits+1);
  _arctan(x, digits);
  float_free(&tmp);
}

/* evaluates arcsin x for -0.5 <= x <= 0.5
   arcsin x = arctan(x/sqrt(1-x*x))
   the relative error of a 100 digit result is < 5e-100 */
void
_arcsinlt0_5(
  floatnum x,
  int digits)
{
  floatstruct tmp;

  if (2*float_getexponent(x) < -digits)
    return;
  float_create(&tmp);
  float_mul(&tmp, x, x, digits);
  float_sub(&tmp, &c1, &tmp, digits);
  float_sqrt(&tmp, digits);
  float_div(x, x, &tmp, digits+1);
  _arctanlt1(x, digits);
  float_free(&tmp);
}

/* evaluates arccos x for -1 <= x <= 1.
   The result is in the range 0 <= result <= pi.
   The relative error for a 100 digit result is < 5e-100 */
void
_arccos(
  floatnum x,
  int digits)
{
  signed char sgn;

  sgn = float_getsign(x);
  float_abs(x);
  if (float_cmp(x, &c1Div2) > 0)
  {
    float_sub(x, x, &c1, digits+1);
    _arccosxplus1lt0_5(x, digits);
  }
  else
  {
    _arcsinlt0_5(x, digits);
    float_sub(x, &cPiDiv2, x, digits+1);
  }
  if (sgn < 0)
    float_sub(x, &cPi, x, digits+1);
}

/* evaluates arccos (1+x) for -2 <= x <= 0.
   The result is in the range 0 <= x <= pi */
void
_arccosxplus1(
  floatnum x,
  int digits)
{
  if (float_abscmp(x, &c1Div2) <= 0)
    _arccosxplus1lt0_5(x, digits);
  else
  {
    float_add(x, x, &c1, digits);
    _arccos(x, digits);
  }
}

/* evaluates arcsin x for -1 <= x <= 1.
   The result is in the range -pi/2 <= result <= pi/2 
   The relative error for a 100 digit result is < 8e-100 */
void
_arcsin(
  floatnum x,
  int digits)
{
  signed char sgn;

  if (float_abscmp(x, &c1Div2) <= 0)
    _arcsinlt0_5(x, digits);
  else
  {
    sgn = float_getsign(x);
    float_abs(x);
    _arccos(x, digits);
    float_sub(x, &cPiDiv2, x, digits);
    float_setsign(x, sgn);
  }
}

/* evaluates cos x - 1 for x < |pi/4|
   relative error for 100 digit results is < 5e-100 */

char
_cosminus1ltPiDiv4(
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
  if (!cosminus1near0(x, digits) && reductions == 0)
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

/* evaluate sin x for |x| <= pi/4,
   using |sin x| = sqrt((1-cos x)*(2 + cos x-1)) 
   relative error for 100 digit results is < 6e-100*/
void
_sinltPiDiv4(
  floatnum x,
  int digits)
{
  floatstruct tmp;
  signed char sgn;

  if (2*float_getexponent(x)+2 < -digits)
    /* for small x: sin x approx.== x */
    return;
  float_create(&tmp);
  sgn = float_getsign(x);
  _cosminus1ltPiDiv4(x, digits);
  float_add(&tmp, x, &c2, digits+1);
  float_mul(x, x, &tmp, digits+1);
  float_abs(x);
  float_sqrt(x, digits);
  float_setsign(x, sgn);
  float_free(&tmp);
}

/* evaluates tan x for |x| <= pi/4.
   The relative error for a 100 digit result is < 6e-100 */

void
_tanltPiDiv4(
  floatnum x,
  int digits)
{
  floatstruct tmp;
  signed char sgn;

  if (2*float_getexponent(x)+2 < -digits)
    /* for small x: tan x approx.== x */
    return;
  float_create(&tmp);
  sgn = float_getsign(x);
  _cosminus1ltPiDiv4(x, digits);
  float_add(&tmp, x, &c2, digits+1);
  float_mul(x, x, &tmp, digits+1);
  float_abs(x);
  float_sqrt(x, digits);
  float_sub(&tmp, &tmp, &c1, digits);
  float_div(x, x, &tmp, digits+1);
  float_setsign(x, sgn);
  float_free(&tmp);
}

/* evaluates cos x for |x| <= pi */

void
_cos(
  floatnum x,
  int digits)
{
  signed char sgn;

  float_abs(x);
  sgn = 1;
  if (float_cmp(x, &cPiDiv2) > 0)
  {
    sgn = -1;
    float_sub(x, &cPi, x, digits+1);
  }
  if (float_cmp(x, &cPiDiv4) <= 0)
  {
    if (2*float_getexponent(x)+2 < - digits)
      float_setzero(x);
    else
      _cosminus1ltPiDiv4(x, digits);
    float_add(x, x, &c1, digits);
  }
  else
  {
    float_sub(x, &cPiDiv2, x, digits+1);
    _sinltPiDiv4(x, digits);
  }
  float_setsign(x, sgn);
}

/* evaluates cos x - 1 for |x| <= pi.
   This function may underflow, which
   is indicated by the return value 0 */
char
_cosminus1(
  floatnum x,
  int digits)
{
  float_abs(x);
  if (float_cmp(x, &cPiDiv4) <= 0)
    return _cosminus1ltPiDiv4(x, digits);
  _cos(x, digits);
  float_sub(x, x, &c1, digits);
  return 1;
}

/* evaluates sin x for |x| <= pi. */
void
_sin(
  floatnum x,
  int digits)
{
  signed char sgn;

  sgn = float_getsign(x);
  float_abs(x);
  if (float_cmp(x, &cPiDiv2) > 0)
    float_sub(x, &cPi, x, digits+1);
  if (float_cmp(x, &cPiDiv4) <= 0)
    _sinltPiDiv4(x, digits);
  else
  {
    float_sub(x, &cPiDiv2, x, digits+1);
    if (2*float_getexponent(x)+2 < - digits)
      float_setzero(x);
    else
      _cosminus1ltPiDiv4(x, digits);
    float_add(x, x, &c1, digits);
  }
  float_setsign(x, sgn);
}

/* evaluates tan x for |x| <= pi.
   A return value of 0 indicates
   that x = +/- pi/2 within
   small tolerances, so that tan x
   cannot be reliable computed */
char
_tan(
  floatnum x,
  int digits)
{
  signed char sgn;

  sgn = float_getsign(x);
  float_abs(x);
  if (float_cmp(x, &cPiDiv2) > 0)
  {
    float_sub(x, &cPi, x, digits+1);
    sgn = -sgn;
  }
  if (float_cmp(x, &cPiDiv4) <= 0)
    _tanltPiDiv4(x, digits);
  else
  {
    float_sub(x, &cPiDiv2, x, digits+1);
    if (float_iszero(x) || float_getexponent(x) < -digits)
      return 0;
    _tanltPiDiv4(x, digits);
    float_reciprocal(x, digits);
  }
  float_setsign(x, sgn);
  return 1;
}

char
_trigreduce(
  floatnum x,
  int digits)
{
  floatstruct tmp;
  int expx;
  signed char sgn;
  char odd;

  if (float_abscmp(x, &cPi) <= 0)
    return 1;
  expx = float_getexponent(x);
  if (expx > float_getlength(&cPi) - MATHPRECISION)
    return 0;
  float_create(&tmp);
  sgn = float_getsign(x);
  float_abs(x);
  float_div(&tmp, x, &cPi, expx + 1);
  float_int(&tmp);
  odd = float_isodd(&tmp);
  float_mul(&tmp, &tmp, &cPi, digits + float_getexponent(&tmp) + 1);
  float_sub(x, x, &tmp, digits+1);
  if (float_cmp(x, &cPi) > 0)
  {
    float_sub(x, x, &cPi, digits+1);
    odd = !odd;
  }
  if (float_getsign(x) < 0)
  {
    float_add(x, x, &cPi, digits+1);
    odd = !odd;
  }
  if (odd)
    float_sub(x, x, &cPi, digits+1);
  if (sgn < 0)
    float_neg(x);
  float_free(&tmp);
  return 1;
}

void
_sinpix(
  floatnum x,
  int digits)
{
  char odd;

  odd = float_isodd(x);
  float_frac(x);
  float_mul(x, &cPi, x, digits+1);
  _sin(x, digits);
  if (odd)
    float_neg(x);
}
