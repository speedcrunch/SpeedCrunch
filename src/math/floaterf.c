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
  if (float_cmp(x, &c1Div2) > 0)
  {
    /* for the time being support for small x only */
    float_setnan(x);
    return 0;
  }
  erfnear0(x, digits);
  float_mul(x, x, &c2DivSqrtPi, digits+1);
  return 1;
}

char
_erfc(
  floatnum x,
  int digits)
{
  floatstruct tmp;
  int expx;
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
  result = 0;
  if (expx > digits || _guess_logerfc(x) < -digits)
  {
    float_create(&tmp);
    result = float_mul(&tmp, x, x, digits+1)
             && _exp(&tmp, digits+1)
             && float_mul(&tmp, &tmp, x, digits+1)
             && float_div(&tmp, &c1DivSqrtPi, &tmp, digits)
             && erfcbigx(x, digits)
             && float_mul(x, x, &tmp, digits+1);
    float_free(&tmp);
  }
  if (!result)
    float_setnan(x);
  return result;
}
