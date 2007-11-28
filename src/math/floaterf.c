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
