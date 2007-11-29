/* floatconst.h: constants for higher math functions */
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

#ifndef _FLOATCONST_H
#define _FLOATCONST_H

#include "floatnum.h"

#define MAXBERNOULLIIDX 68

#ifdef __cplusplus
extern "C" {
#endif

extern floatstruct c1;
extern floatstruct c2;
extern floatstruct c3;
extern floatstruct c12;
extern floatstruct cMinus1;
extern floatstruct cMinus20;
extern floatstruct c1Div2;
extern floatstruct cLn2;
extern floatstruct cLn3;
extern floatstruct cLn7;
extern floatstruct cLn10;
extern floatstruct cPi;
extern floatstruct cPiDiv2;
extern floatstruct cPiDiv4;
extern floatstruct cSqrtPi;
extern floatstruct c1DivSqrtPi;
extern floatstruct cLnSqrt2PiMinusHalf;
extern floatstruct c2DivSqrtPi;
extern floatstruct cMinus0_4;
extern floatstruct cBernoulliNum[68];
extern floatstruct cBernoulliDen[68];
extern floatstruct cUnsignedBound;

void floatmath_init();
void floatmath_exit();

#ifdef __cplusplus
}
#endif

#endif /* _FLOATCONST_H */
