/* floatpower.c: power operation, based on floatnum. */
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

#include "floatpower.h"
#include "floatipower.h"
#include "floatcommon.h"
#include "floatlog.h"
#include "floatexp.h"

char
_raise(
  floatnum x,
  floatnum exponent,
  int digits)
{
  int iexp;
  int extra;

  extra = float_getexponent(exponent);
  if (float_isinteger(exponent))
  {
    iexp = leadingdigits(exponent, float_getexponent(exponent) + 1);
    if (float_iszero(exponent) || iexp !=  0)
      return _raisei(x, iexp, digits+extra);
  }
  _ln(x, digits+extra);
  if (!float_mul(x, x, exponent, digits+extra))
    return 0;
  return _exp(x, digits);
}
