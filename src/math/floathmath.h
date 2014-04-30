/* floathmath.h: higher mathematical functions, based on floatnum. */
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

#ifndef FLOATHMATH_H
#define FLOATHMATH_H

#include "floatnum.h"

#ifdef __cplusplus
extern "C" {
#endif

/* evaluates ln(1+x) for x > -1. This function is especially useful
   for small x, where its computation is more precise than that of
   float_ln.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: NaNOperand
           InvalidPrecision (digits > MATHPRECISION)
           OutOfDomain */
char float_lnxplus1(floatnum x, int digits);

/* evaluates ln x (the logarithm to base e) for x > 0.
   Near x == 0, this function yields better results than float_lnxplus1.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: NaNOperand
           InvalidPrecision (digits > MATHPRECISION)
           OutOfDomain */
char float_ln(floatnum x, int digits);

/* evaluates lg x (the logarithm to base 10) for x > 0.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: NaNOperand
           InvalidPrecision (digits > MATHPRECISION)
           OutOfDomain */
char float_lg(floatnum x, int digits);

/* evaluates lb x (the logarithm to base 2) for x > 0.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: NaNOperand
           InvalidPrecision (digits > MATHPRECISION)
           OutOfDomain */
char float_lb(floatnum x, int digits);

/* evaluates arsinh x, the inverse function of sinh.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: NaNOperand
           InvalidPrecision (digits > MATHPRECISION) */
char float_arsinh(floatnum x, int digits);

/* evaluates arcosh x, the inverse function of cosh, for
   x >= 1.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: NaNOperand
           InvalidPrecision (digits > MATHPRECISION)
           OutOfDomain */
char float_arcosh(floatnum x, int digits);

/* evaluates artanh x, the inverse function of tanh, for
   |x| < 1.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: NaNOperand
           InvalidPrecision (digits > MATHPRECISION)
           OutOfDomain */
char float_artanh(floatnum x, int digits);

/* evaluates artanh (1+x), for -2 < x <= 0. This function is
   especially useful, if you want to compute values near the
   pole at x == 1.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: NaNOperand
           InvalidPrecision (digits > MATHPRECISION)
           OutOfDomain */
char float_artanhxplus1(floatnum x, int digits);

/* evaluates arcosh (1+x), for x >= 0. This function is
   especially useful, if you want to compute values near the
   singularity of arcosh at x == 1.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: NaNOperand
           InvalidPrecision (digits > MATHPRECISION)
           OutOfDomain */
char float_arcoshxplus1(floatnum x, int digits);

/* evaluates exp(x)
   In case of an error, x is set to NaN and 0 is returned.
   Errors: Underflow
           Overflow
           NaNOperand
           InvalidPrecision (digits > MATHPRECISION) */
char float_exp(floatnum x, int digits);

/* evaluates exp(x)-1. Use this in the neighbourhood of x == 0.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: Overflow
           NaNOperand
           InvalidPrecision (digits > MATHPRECISION) */
char float_expminus1(floatnum x, int digits);

/* evaluates cosh(x).
   In case of an error, x is set to NaN and 0 is returned.
   Errors: Overflow
           NaNOperand
           InvalidPrecision (digits > MATHPRECISION) */
char float_cosh(floatnum x, int digits);

/* evaluates cosh(x) - 1. Yields better results in the
   neighbourhood of x == 0 than float_cosh.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: Underflow
           Overflow
           NaNOperand
           InvalidPrecision (digits > MATHPRECISION) */
char float_coshminus1(floatnum x, int digits);

/* evaluates sinh(x).
   In case of an error, x is set to NaN and 0 is returned.
   Errors: Overflow
           NaNOperand
           InvalidPrecision (digits > MATHPRECISION) */
char float_sinh(floatnum x, int digits);

/* evaluates tanh(x).
   In case of an error, x is set to NaN and 0 is returned.
   Errors: NaNOperand
           InvalidPrecision (digits > MATHPRECISION) */
char float_tanh(floatnum x, int digits);

/* evaluates tanh(x)-1. Yields better results for large x > 0,
   when tanh x approx.== 1, than float_tanh.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: Underflow
           NaNOperand
           InvalidPrecision (digits > MATHPRECISION) */
char float_tanhminus1(floatnum x, int digits);

/* evaluates 10^x. No optimization is applied for integer
   exponents.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: Underflow
           Overflow
           NaNOperand
           InvalidPrecision (digits > MATHPRECISION) */
char float_power10(floatnum x, int digits);

/* evaluates arctan x, yielding a result -pi/2 < result < pi/2.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: NaNOperand
           InvalidPrecision (digits > MATHPRECISION) */
char float_arctan(floatnum x, int digits);

/* evaluates arcsin x for -1 <= x <= 1, yielding a result
   -pi/2 <= result <= pi/2.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: OutOfDomain
           NaNOperand
           InvalidPrecision (digits > MATHPRECISION) */
char float_arcsin(floatnum x, int digits);

/* evaluates arccos x for -1 <= x <= 1, yielding a result
   0 <= result <= pi.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: OutOfDomain
           NaNOperand
           InvalidPrecision (digits > MATHPRECISION) */
char float_arccos(floatnum x, int digits);
/* evaluates arccos (1+x) for -2 <= x <= 0, yielding a result
   0 <= result <= pi. This function is more precise in the
   neighbourhood of x == 0 than float_arccos.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: Underflow
           Overflow
           NaNOperand
           InvalidPrecision (digits > MATHPRECISION) */
char float_arccosxplus1(floatnum x, int digits);

/* evaluates tan x. For extreme large x, the periodicity
   of tan is not recognized any more, and a FLOAT_UNSTABLE
   error is reported. The same holds, if x is too near to
   a pole of tan, more precise, if |x-0.5*n*pi| < 1e-<digits>
   for some n.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: EvalUnstable
           NaNOperand
           InvalidPrecision (digits > MATHPRECISION) */
char float_tan(floatnum x, int digits);

/* evaluates sin x. For extreme large x, the periodicity
   of sin is not recognized any more, and a FLOAT_UNSTABLE
   error is reported.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: EvalUnstable
           NaNOperand
           InvalidPrecision (digits > MATHPRECISION) */
char float_sin(floatnum x, int digits);

/* evaluates cos x. For extreme large x, the periodicity
   of sin is not recognized any more, and a FLOAT_UNSTABLE
   error is reported.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: EvalUnstable
           NaNOperand
           InvalidPrecision (digits > MATHPRECISION) */
char float_cos(floatnum x, int digits);

/* evaluates cos x - 1. In the neighbourhood of x==0, when
   cos x approx.== 1, this function yields better results
   than float_cos.
   For extreme large x, the periodicity
   of sin is not recognized any more, and a FLOAT_UNSTABLE
   error is reported.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: Undeflow
           EvalUnstable
           NaNOperand
           InvalidPrecision (digits > MATHPRECISION) */
char float_cosminus1(floatnum x, int digits);

/* evaluates base^exponent
   In case of an error, x is set to NaN and 0 is returned.
   Errors: Underflow
           Overflow
           OutOfDomain (base <= 0 and most exponents)
           ZeroDivide  ( base == 0 and exponent negative)
           NaNOperand
           InvalidPrecision (digits > maxprecision-14) */
char float_raisei(floatnum power, cfloatnum base,
                 int exponent, int digits);

/* evaluates base^exponent
   In case of an error, x is set to NaN and 0 is returned.
   Errors: Underflow
           Overflow
           OutOfDomain (base <= 0 and most exponents)
           ZeroDivide  (base == 0 and exponent negative)
           NaNOperand
           InvalidPrecision (digits > MATHPRECISION) */
char float_raise(floatnum power, cfloatnum base,
                 cfloatnum exponent, int digits);

/* evaluates Gamma(x) = (x-1)!
   In case of an error, x is set to NaN and 0 is returned.
   Errors: Underflow
           Overflow
           ZeroDivide  (for integers <= 0)
           NaNOperand
           InvalidPrecision (digits > MATHPRECISION) */
char float_gamma(floatnum x, int digits);

/* evaluates ln(Gamma(x)) = ln((x-1)!)
   In case of an error, x is set to NaN and 0 is returned.
   Errors: Overflow
           OutOfDomain  (for x <= 0)
           NaNOperand
           InvalidPrecision (digits > MATHPRECISION) */
char float_lngamma(floatnum x, int digits);

/* evaluates x!
   In case of an error, x is set to NaN and 0 is returned.
   Errors: Underflow
           Overflow
           ZeroDivide  (for integers < 0)
           NaNOperand
           InvalidPrecision (digits > MATHPRECISION) */
char float_factorial(floatnum x, int digits);

/* evaluates gamma(x+delta)/gamma(x). If delta is a positive integer, this
   is the Pochhammer symbol x*(x+1)*...*(x+delta-1). The poles of the
   gamma function are handled appropriately.
   Errors: Underflow
           Overflow
           ZeroDivide  (not annihilated pole in the nominator)
           NaNOperand
           InvalidPrecision (digits > MATHPRECISION) */
char float_pochhammer(floatnum x, cfloatnum delta, int digits);

/* evaluates erf(x).
           NaNOperand
           InvalidPrecision (digits > MATHPRECISION) */
char float_erf(floatnum x, int digits);

/* evaluates erfc(x).
           Underflow
           NaNOperand
           InvalidPrecision (digits > MATHPRECISION) */
char float_erfc(floatnum x, int digits);

/* cuts off the fraction part of <x> and complements the bits
   in the 2's complement representation of <x> then. The
   corresponding integer of the result is stored back in <x>.
   In case of an error, <x> is set to NaN and 0 is returned.
   Errors: OutOfLogicRange
           NaNOperand */
char float_not(floatnum x);

/* uses the integer parts of <x> and <y> and builds their
   2's complement representation. The resulting strings
   are and-ed and the corresponding integer is stored
   in dest.
   In case of an error, <dest> is set to NaN and 0 is returned.
   Errors: OutOfLogicRange
           NaNOperand */
char float_and(floatnum dest, cfloatnum x, cfloatnum y);

/* uses the integer parts of x and y and builds their
   2's complement representation. The resulting strings
   are or-ed and the corresponding integer is stored
   in dest.
   In case of an error, <dest> is set to NaN and 0 is returned.
   Errors: OutOfLogicRange
           NaNOperand */
char float_or(floatnum dest, cfloatnum x, cfloatnum y);

/* uses the integer parts of <x> and <y> and builds their
   2's complement representation. The resulting strings
   are xor-ed and the corresponding integer is stored
   in dest.
   In case of an error, <dest> is set to NaN and 0 is returned.
   Errors: OutOfLogicRange
           NaNOperand */
char float_xor(floatnum dest, cfloatnum x, cfloatnum y);

/* uses the integer part of <x> and builds its 2's complement
   representation as LOGICRANGE bits. The resulting bitstring
   is then shifted y times to the left. Shifted out bits are dropped,
   to the right 0 bits are fed in.
   The corresponding integer of the resulting bitstring is stored
   in <dest>.
   <y> has to be an integer or an error is reported. If it is negative,
   this operation turns into a shift right.
   In case of an error, <dest> is set to NaN and 0 is returned.
   Errors: OutOfDomain (y not an integer)
           OutOfLogicRange
           NaNOperand */
char float_shl(floatnum dest, cfloatnum x, cfloatnum y);

/* uses the integer part of <x> and builds its 2's complement
   representation as LOGICRANGE bits. The resulting bitstring
   is then shifted y times to the right. Shifted out bits are dropped,
   to the left the sign bit is duplicated.
   The corresponding integer of the resulting bitstring is stored
   in <dest>.
   <y> has to be an integer or an error is reported. If it is negative,
   this operation turns into a shift left.
   In case of an error, <dest> is set to NaN and 0 is returned.
   Errors: OutOfDomain (y not an integer)
           OutOfLogicRange
           NaNOperand */
char float_shr(floatnum dest, cfloatnum x, cfloatnum y);

#ifdef __cplusplus
}
#endif

#endif /* FLOATLOG_H */
