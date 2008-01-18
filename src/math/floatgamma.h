/* floatgamma.h: Gamma function, based on floatnum. */
/*
    Copyright (C) 2007, 2008 Wolf Lammen.

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

#ifndef _FLOATGAMMA_H
# define _FLOATGAMMA_H

#include "floatnum.h"

#ifdef __cplusplus
extern "C" {
#endif

char _lngamma(floatnum x, int digits);
char _gamma(floatnum x, int digits);
char _gammaint(floatnum integer, int digits);
char _gamma0_5(floatnum x, int digits);
char _pochhammer(floatnum x, cfloatnum n, int digits);

#ifdef __cplusplus
}
#endif

#endif /* _FLOATGAMMA_H */
