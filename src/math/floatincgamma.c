/* floatincgamma.h: incomplete gamma function */
/*
    Copyright (C) 2009 Wolf Lammen.

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

#include "floatincgamma.h"
#include "floatconst.h"
#include "floatcommon.h"

/* many ideas are from a paper of Serge Winitzki,
"Computing the incomplete Gamma function to arbitrary precision"
*/

/* reglowgammanearpole computes
     f(x, k, h) = x^(k-h) * [lowgamma(-k+h, x) - (-1)^k * x^h / h / k!]
   where k is an integer >= 0. It should only be used for |h| <= 0.5 and |x|
   not much bigger than 2.
   f is useful for computing both the lower and upper incomplete gamma
   function for 'a' close to a non-positive integer and x near 0. The term
   (-1)^k * x^h/(k!*h) subtracts the pole Gamma(-k), and, in addition,
   lowgamma(a, x) is regularized by the factor x^(k-h), in order to eliminate
   its singularity at x == 0 for a <= 0. */
static void
reglowgammanearpole(floatnum x, int k, cfloatnum h, int digits){
  floatstruct tmp;
  float_create(&tmp);
  if (float_iszero(x) || float_getexponent(x) < -digits-3){
    // tiny x, return the first element of the series: k > 0? 1/(h-k) : -x/(1+h)
    if (k > 0){
      float_addi(x, h, -k, digits+2);
      float_reciprocal(x, digits+1);
    }
    else{
      float_sub(&tmp, &cMinus1, h, digits+2);
      float_div(x, x, &tmp, digits+1);
    }
  }
  else{
    // evaluate the series
    int workprec = digits+5;
    int i = 0;
    int expx = float_getexponent(x);
    floatstruct summand;
    floatstruct sum;
    float_create(&summand);
    float_create(&sum);

    float_copy(&tmp, &c1, EXACT);
    float_setzero(&sum);
    while (workprec > 0){
      if (i != k){
        float_addi(&summand, h, i-k, workprec);
        float_div(&summand, &tmp, &summand, workprec);
        float_add(&sum, &sum, &summand, digits+5);
      }
      workprec = digits - float_getexponent(&sum)
                 + float_getexponent(&summand) + expx+5;
      if (workprec > 0){
        float_mul(&tmp, &tmp, x, workprec);
        float_divi(&tmp, &tmp, -++i, workprec);
      }
    }
    float_move(x, &sum); // frees sum as a side-effect
    // float_free(&sum);
    float_free(&summand);
  }
  float_free(&tmp);
}
