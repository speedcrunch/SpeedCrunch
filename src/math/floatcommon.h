/* floatcommon.h: header file for convenience functions, based on floatnum. */
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

#ifndef FLOATCOMMON_H
# define FLOATCOMMON_H
#include "floatnum.h"

#ifdef __cplusplus
extern "C" {
#endif

/* helper, checks parameters. Sets float_error to NaNOperand
   or InvalidPrecision and sets <x> to NaN, if the parameters do
   not meet the requirements of routines for higher mathematical
   functions, and returns 0 in this case */
char chckmathparam(floatnum x, int digits);

/* helper, determines, how many decimal digits the exponent of <x> has.
   If the exponent is 0, -1 is returned */
int logexp(cfloatnum x);

/* helper, returns the <digits> first decimal digits and the sign of a
   significand, encoded in an integer. */
int leadingdigits(cfloatnum x, int digits);

/* convenience wrapper for float_setscientific, setting the last
   parameter to NULLTERMINATED */
void float_setasciiz(floatnum x, const char* asciiz);

/* convenience wrapper for float_add, adds a signed integer to <summand1>
   and places the result in <sum> */
char float_addi(floatnum sum, cfloatnum summand1,
               int summand2, int digits);

/* convenience wrapper for float_mul, multiplies a signed integer with
   <factor1> and places the result in <product> */
char float_muli(floatnum product, cfloatnum factor1,
               int factor2, int digits);

/* convenience wrapper for float_div, divides <dividend> by a signed integer
   and places the result in <quotient> */
char float_divi(floatnum quotient, cfloatnum dividend,
               int divisor, int digits);

/* convenience wrapper for float_cmp: compares the absolute value of
   both operands */
int float_abscmp(floatnum x, floatnum y);

/* convenience wrapper for float_div, returns 1/<x> */
char float_reciprocal(floatnum x, int digits);

/* compares two numbers in a normal fashion, but returns equal, if their
   relative difference is less than 1e-<digits>, i.e.
   |(x-y)/max(x,y)| < 1e-<digits> */
int float_relcmp(floatnum x, floatnum y, int digits);

/* returns whether x is an integer */
char float_isinteger(cfloatnum x);

/* returns the integer part of x as integer. If x exceeds the
   integer range, 0 is returned */
int float_asinteger(cfloatnum x);

/* rounds x in TONEAREST mode. If x overflows, the rounding is reverted.
   Does not report errors */
void float_checkedround(floatnum x, int digits);

/* a fast way to multiply with a power of ten, does not set float_error
   on overflow or NaN, returns silently NaN instead*/
void float_addexp(floatnum x, int smd);

/* returns 0, if the integer part of x is even */
char float_isodd(floatnum x);

/* an extension of float_int: you can choose the round mode
   errors: FLOAT_NANOPERAND
           FLOAT_OVERFLOW (if EXP_MAX is really small) */
char float_roundtoint(floatnum x, roundmode mode);

float float_asfloat(cfloatnum x);

void float_setfloat(floatnum dest, float x);

float aprxsqrt(float x);
float aprxln(float x);
float aprxlog10(float x);
float aprxlog2(float x);
float aprxlog2fn(cfloatnum x);

#ifdef __cplusplus
}
#endif

#endif /* FLOATCOMMON_H */
