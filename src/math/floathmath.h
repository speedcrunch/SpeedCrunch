/* floathmath.h: higher mathematical functions, based on floatnum. */
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

#ifndef _FLOATHMATH_H
#define _FLOATHMATH_H

#include "floatnum.h"

#ifdef __cplusplus
extern "C" {
#endif 

/* evaluates ln(1+x) for x > -1. This function is especially useful
   for small x, where its computation is more precise than that of
   float_ln.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10)
           FLOAT_OUTOFDOMAIN */
char float_lnxplus1(floatnum x, int digits);

/* evaluates ln x for x > 0. Near x == 0, this function yields better
   results than float_lnxplus1.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10)
           FLOAT_OUTOFDOMAIN */
char float_ln(floatnum x, int digits);

/* evaluates log x (the logarithm to base 10) for x > 0.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10)
           FLOAT_OUTOFDOMAIN */
char float_log(floatnum x, int digits);

/* evaluates lg x (the logarithm to base 2) for x > 0.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10)
           FLOAT_OUTOFDOMAIN */
char float_lg(floatnum x, int digits);

/* evaluates arsinh x, the inverse function of sinh.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10) */
char float_arsinh(floatnum x, int digits);

/* evaluates arcosh x, the inverse function of cosh, for
   x >= 1.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10)
           FLOAT_OUTOFDOMAIN */
char float_arcosh(floatnum x, int digits);

/* evaluates artanh x, the inverse function of tanh, for
   |x| < 1.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10)
           FLOAT_OUTOFDOMAIN */
char float_artanh(floatnum x, int digits);

/* evaluates artanh (1+x), for -2 < x <= 0. This function is
   especially useful, if you want to compute values near the
   pole at x == 1.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10)
           FLOAT_OUTOFDOMAIN */
char float_artanhxplus1(floatnum x, int digits);

/* evaluates arcosh (1+x), for x >= 0. This function is
   especially useful, if you want to compute values near the
   singularity of cosh at x == 1.
   This function may underflow for very small x.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_UNDERFLOW
           FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10)
           FLOAT_OUTOFDOMAIN */
char float_arcoshxplus1(floatnum x, int digits);

/* evaluates exp(x)
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_UNDERFLOW
           FLOAT_OVERFLOW
           FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10) */
char float_exp(floatnum x, int digits);

/* evaluates exp(x)-1. Use this in the neighbourhood of x == 0.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_OVERFLOW
           FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10) */
char float_expminus1(floatnum x, int digits);

/* evaluates cosh(x).
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_OVERFLOW
           FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10) */
char float_cosh(floatnum x, int digits);

/* evaluates cosh(x) - 1. Yields better results in the
   neighbourhood of x == 0.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_UNDERFLOW
           FLOAT_OVERFLOW
           FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10) */
char float_coshminus1(floatnum x, int digits);

/* evaluates sinh(x).
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_OVERFLOW
           FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10) */
char float_sinh(floatnum x, int digits);

/* evaluates tanh(x).
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10) */
char float_tanh(floatnum x, int digits);

/* evaluates tanh(x)-1. Yields better results for large x > 0,
   when tanh x approx.== 1, than float_tanh.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_UNDERFLOW
           FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10) */
char float_tanhminus1(floatnum x, int digits);

/* evaluates 10^x. 
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_UNDERFLOW
           FLOAT_OVERFLOW
           FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10) */
char float_power10(floatnum x, int digits);

/* evaluates arctan x, yielding a result -pi/2 < result < pi/2.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10) */
char float_arctan(floatnum x, int digits);

/* evaluates arcsin x for -1 <= x <= 1, yielding a result
   -pi/2 <= result <= pi/2.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_OUTOFDOMAIN
           FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10) */
char float_arcsin(floatnum x, int digits);

/* evaluates arccos x for -1 <= x <= 1, yielding a result
   0 <= result <= pi.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_OUTOFDOMAIN
           FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10) */
char float_arccos(floatnum x, int digits);
/* evaluates arccos (1+x) for -2 <= x <= 0, yielding a result
   0 <= result <= pi. This function is more precise in the
   neighbourhood of x == 0 than float_arccos.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_UNDERFLOW
           FLOAT_OUTOFDOMAIN
           FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10) */
char float_arccosxplus1(floatnum x, int digits);

/* evaluates tan x. For extreme large x, the periodicity
   of tan is not recognized any more, and a FLOAT_UNSTABLE
   error is reported. The same holds, if x is too near to
   a pole of tan, more precise, if |x-0.5*n*pi| < 1e-<digits>
   for some n.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_UNSTABLE
           FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10) */
char float_tan(floatnum x, int digits);

/* evaluates sin x. For extreme large x, the periodicity
   of sin is not recognized any more, and a FLOAT_UNSTABLE
   error is reported.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_UNSTABLE
           FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10) */
char float_sin(floatnum x, int digits);

/* evaluates cos x. For extreme large x, the periodicity
   of sin is not recognized any more, and a FLOAT_UNSTABLE
   error is reported.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_UNSTABLE
           FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10) */
char float_cos(floatnum x, int digits);

/* evaluates cos x - 1. In the neighbourhood of x==0, when
   cos x approx.== 1, this function yields better results
   than float_cos.
   For extreme large x, the periodicity
   of sin is not recognized any more, and a FLOAT_UNSTABLE
   error is reported.
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_UNDERFLOW
           FLOAT_UNSTABLE
           FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10) */
char float_cosminus1(floatnum x, int digits);

/* evaluates base^exponent
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_UNDERFLOW
           FLOAT_OVERFLOW
           FLOAT_OUTOFDOMAIN (base <= 0 and most exponents)
           FLOAT_ZERODIVIDE  ( base == 0 and exponent negative)
           FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10) */
char float_raisei(floatnum power, floatnum base,
                 int exponent, int digits);

/* evaluates base^exponent
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_UNDERFLOW
           FLOAT_OVERFLOW
           FLOAT_OUTOFDOMAIN (base <= 0 and most exponents)
           FLOAT_ZERODIVIDE  ( base == 0 and exponent negative)
           FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10) */
char float_raise(floatnum power, floatnum base,
                floatnum exponent, int digits);

/* evaluates Gamma(x) = (x-1)!
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_UNDERFLOW
           FLOAT_OVERFLOW
           FLOAT_ZERODIVIDE  ( for integers <= 0)
           FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10) */
char float_gamma(floatnum x, int digits);

/* evaluates ln(abs(Gamma(x))) = ln(abs((x-1)!))
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_OVERFLOW
           FLOAT_OUTOFDOMAIN  ( for x <= 0)
           FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10) */
char float_lngamma(floatnum x, int digits);

/* evaluates x!
   In case of an error, x is set to NaN and 0 is returned.
   Errors: FLOAT_UNDERFLOW
           FLOAT_OVERFLOW
           FLOAT_ZERODIVIDE  ( for integers < 0)
           FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10) */
char float_factorial(floatnum x, int digits);

/* evaluates gamma(x+delta)/gamma(x). If delta is a positive integer, this
   is the Pochhammer symbol x*(x+1)*...*(x+delta-1). The poles of the
   gamma function are handled appropriately.
   Errors: FLOAT_UNDERFLOW
           FLOAT_OVERFLOW
           FLOAT_OUTOFDOMAIN (base <= 0 and most exponents)
           FLOAT_ZERODIVIDE  (not annihilated pole in the nominator)
           FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10) */
char float_pochhammer(floatnum x, floatnum delta, int digits);

/* currently supports small x only!!
   evaluates erf(x).
           FLOAT_OUTOFDOMAIN (currently |x| < 0.5)
           FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10) */
char float_erf(floatnum x, int digits);

/* currently supports small x only!!
   evaluates erfc(x).
           FLOAT_OUTOFDOMAIN (currently |x| < 0.5)
           FLOAT_UNDERFLOW
           FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM (digits > MAXSCALE - 10) */
char float_erfc(floatnum x, int digits);

/* cuts off the fraction part of <x> and complements the bits
   in the 2's complement representation of <x> then. The
   corresponding integer of the result is stored back in <x>.
   In case of an error, <x> is set to NaN and 0 is returned.
   Errors: FLOAT_OUTOFDOMAIN (x exceeding integer range)
           FLOAT_NANOPERAND */
char float_not(floatnum x);

/* uses the integer parts of <x> and <y> and builds their
   2's complement representation. The resulting strings
   are and-ed and the corresponding integer is stored
   in dest.
   In case of an error, <dest> is set to NaN and 0 is returned.
   Errors: FLOAT_OUTOFDOMAIN (x, y exceeding integer range)
           FLOAT_NANOPERAND */
char float_and(floatnum dest, floatnum x, floatnum y);

/* uses the integer parts of x and y and builds their
   2's complement representation. The resulting strings
   are or-ed and the corresponding integer is stored
   in dest.
   In case of an error, <dest> is set to NaN and 0 is returned.
   Errors: FLOAT_OUTOFDOMAIN (x, y exceeding integer range)
           FLOAT_NANOPERAND */
char float_or(floatnum dest, floatnum x, floatnum y);

/* uses the integer parts of <x> and <y> and builds their
   2's complement representation. The resulting strings
   are xor-ed and the corresponding integer is stored
   in dest.
   In case of an error, <dest> is set to NaN and 0 is returned.
   Errors: FLOAT_OUTOFDOMAIN (x, y exceeding integer range)
           FLOAT_NANOPERAND */
char float_xor(floatnum dest, floatnum x, floatnum y);

/* uses the integer part of <x> and builds its 2's complement
   representation as LOGICRANGE bits. The resulting bitstring
   is then shifted y times to the left. Shifted out bits are dropped,
   to the right 0 bits are fed in.
   The corresponding integer of the resulting bitstring is stored
   in <dest>.
   <y> has to be a non-negative integer or an error is reported.
   In case of an error, <dest> is set to NaN and 0 is returned.
   Errors: FLOAT_OUTOFDOMAIN
           FLOAT_NANOPERAND */
char float_shl(floatnum dest, floatnum x, floatnum y);

/* uses the integer part of <x> and builds its 2's complement
   representation as LOGICRANGE bits. The resulting bitstring
   is then shifted y times to the right. Shifted out bits are dropped,
   to the left the sign bit is duplicated.
   The corresponding integer of the resulting bitstring is stored
   in <dest>.
   <y> has to be a non-negative integer or an error is reported.
   In case of an error, <dest> is set to NaN and 0 is returned.
   Errors: FLOAT_OUTOFDOMAIN
           FLOAT_NANOPERAND */
char float_shr(floatnum dest, floatnum x, floatnum y);

#ifdef __cplusplus
}
#endif

#endif /* _FLOATLOG_H */
