/* floaterf.c: normal distribution integrals erf and the like */
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

#include "floaterf.h"
#include "floatconst.h"
#include "floatcommon.h"
#include "floatexp.h"

static int
_guess_logerfc(
  floatnum x)
{
  int result;

  result = float_asinteger(x);
  result = (result * result * 7) >> 4;
  return -result;
}

char
_erf(
  floatnum x,
  int digits)
{
  int workprec;
  signed char sign;

  sign = float_getsign(x);
  if (sign < 0)
    float_abs(x);
  if (float_cmp(x, &c1Div2) > 0)
  {
    workprec = digits + _guess_logerfc(x);
    if (workprec < 0)
      float_setzero(x);
    else if (!_erfc(x, workprec))
      return 0;
    float_sub(x, &c1, x, digits);
  }
  else
  {
    erfnear0(x, digits);
    float_mul(x, x, &c2DivSqrtPi, digits+1);
  }
  float_setsign(x, sign);
  return 1;
}

char
_erfc(
  floatnum x,
  int digits)
{
  floatstruct tmp, t2, t3;
  int expx, prec;
  char result;

  expx = float_getexponent(x);
  if (expx < -digits || float_iszero(x))
  {
    float_copy(x, &c1, EXACT);
    return 1;
  }
  if (float_cmp(x, &c1Div2) <= 0)
  {
    _erf(x, digits + expx);
    float_sub(x, &c1, x, digits);
    return 1;
  }
  float_create(&tmp);
  if (expx < digits && _guess_logerfc(x) > -digits)
  {
    float_create(&t2);
    float_create(&t3);
    float_mul(&t2, x, x, digits+1);
    float_copy(&tmp, &t2, EXACT);
    erfcsum(&tmp, digits);
    float_add(&tmp, &tmp, &tmp, digits+1);
    float_copy(&t3, &t2, EXACT);
    float_reciprocal(&t2, digits);
    float_add(&tmp, &tmp, &t2, digits);
    float_neg(&t3);
    _exp(&t3, digits+1);
    float_mul(&t3, &t3, &tmp, digits);
    float_mul(&tmp, &erfcalpha, x, digits);
    float_mul(&t3, &tmp, &t3, digits);
    float_mul(&t3, &c1DivPi, &t3, digits);
    /* quick estimate to find the right working precision */
    float_div(&tmp, x, &erfcalpha, 4);
    float_mul(&tmp, &tmp, &c2Pi, 4);
    float_div(&tmp, &tmp, &cLn10, 4);
    prec = digits - float_getexponent(&t3) - float_asinteger(&tmp) + 1;
    /* add correction term */
    result = prec <= digits;
    if (result && prec > 0)
    {
      float_div(&tmp, x, &erfcalpha, prec+2);
      float_mul(&tmp, &tmp, &c2Pi, prec+2);
      _exp(&tmp, prec);
      float_sub(&tmp, &c1, &tmp, prec);
      float_div(&tmp, &c2, &tmp, prec);
      float_add(&t3, &t3, &tmp, digits);
    }
    float_free(&t2);
    float_move(x, &t3);
  }
  else
    result = float_mul(&tmp, x, x, digits+1)
        && _exp(&tmp, digits+1)
        && float_mul(&tmp, &tmp, x, digits+1)
        && float_div(&tmp, &c1DivSqrtPi, &tmp, digits)
        && erfcbigx(x, digits)
        && float_mul(x, x, &tmp, digits+1);
  float_free(&tmp);
  if (!result)
    float_setnan(x);
  return result;
}
