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
#include "floathmath.h"

/* many ideas are from a paper of Serge Winitzki,
"Computing the incomplete Gamma function to arbitrary precision"
*/

/* The critical line of the lower incomplete gamma function lowgamma(a, z)
   is composed of all non-positive real 'a' along with z == 0. It cannot be
   continued on this line. Near this line lowgamma behaves approximately as
   1. z^a/a, a not close to a negative integer
   2. (z^a/a) + (-1)^k*z^h/h/k!, where -k is a negative integer very close to a
                                 and h = a + k very small
   If h << z the second summand in 2. becomes dominant, otherwise z^a/a is
   a rough estimate of lowgamma(a, z), diverging for fixed non-positive 'a*
   and z -> 0.
   Since z^a is true complex for real z < 0, lowgamma cannot be continued
   in the real number domain accross the critical line, except for 'a' a
   negative integer.

   reglowgammanearpole computes
     f(x, k, h) = x^(k-h) * [lowgamma(-k+h, x) - (-1)^k * x^h / h / k!]
   where k is an integer >= 0. It should only be used for |h| <= 0.5 and |x|
   not much bigger than 2.
   f is useful for computing both the lower and upper incomplete gamma
   function for values a, x close to the critical line. The term
   (-1)^k * x^h/(k!*h) subtracts the pole Gamma(-k), and, in addition,
   lowgamma(a, x) is regularized by the factor x^(k-h), in order to eliminate
   the singularity introduced by the factor z^a/a.
   f is real-valued even for negative x, where lowgamma is true complex.
   f is not continuous at a = (-2k-1)/2, k integer > 0, where the subtracted
   pole changes */
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

/* computes x^a (withExp = 0) or exp(-x)*x^a (withExp = 1)
   x^a[*exp(-x)] is the regulizing factor of the lower incomplete gamma function */
static char
regulizingfactor(floatnum x, cfloatnum a, int digits, char withExp){
  char result;
  floatstruct tmp;
  float_create(&tmp);
  result = float_raise(&tmp, x, a, digits);
  if (result && withExp){
    float_neg(x);
    result = float_exp(x, digits)
             && float_mul(&tmp, &tmp, x, digits);
  }
  float_move(x, &tmp); // frees tmp as a side-effect
  // float_free(&tmp);
  return result;
}

/* adds (-1)^k * x^h / (h * k!) to lowgamma, accounting for the
   pole of the gamma function, h!= 0 */
static char
addgammapole(floatnum lowgamma, cfloatnum x, int k, cfloatnum h, int digits){
  // estimate the size of this summand, often it will not contribute
  // to the result
  char result = 1;
  float fh = float_getexponent(h) >= -37? float_asfloat(h) : 0;
  float xx;
  float fexp = aprxlog10fn(x);
  fexp *= fh;
  fexp -= aprxlog10fn(h);
  xx = aprxlngamma(k+1);
  xx *= 0.434294481903f;
  fexp += xx;
  fexp +=1;
/*  float fexp = (aprxlog2fn(x)*fh - aprxlog2fn(h)) * 0.301029995663981f
               - aprxlngamma(k+1) * 0.434294481903f + 1;*/
  if (fexp > EXPMIN){
    int exp = fexp;
    int explowgamma = float_getexponent(lowgamma);
    int workprec = digits + 3;
    if (exp < explowgamma-3)
      workprec = digits - explowgamma - exp -3;
    if (workprec > 0){
      floatstruct pwr;
      floatstruct fct;
      float_create(&pwr);
      float_create(&fct);
      float_setinteger(&fct, k);
      result = float_raise(&pwr, x, h, workprec)
               && float_factorial(&fct, workprec)
               && float_div(&pwr, &pwr, &fct, workprec)
               && float_div(&pwr, &pwr, h, workprec);
      if ((k & 1) != 0)
        float_neg(&pwr);
      result = result && float_add(lowgamma, lowgamma, &pwr, digits+2);
      float_free(&fct);
      float_free(&pwr);
    }
  }
  return result;
}

/* for 0.5 > a and ln Gamma(-a) / ln 10 < 2 * digits */
static char
lowgammanearpole(floatnum x, cfloatnum a, int digits){
  int k = float_getexponent(a);
  char result;
  floatstruct h;
  floatstruct lowgamma;
  floatstruct factor;
  float_create(&h);
  float_create(&lowgamma);
  float_create(&factor);
  float_sub(&h, a, &c1Div2, k < -1? 2 : k+3);
  k = -float_asinteger(&h);
  float_addi(&h, a, k, digits+2);
  float_copy(&lowgamma, x, digits+2);
  float_copy(&factor, x, digits+3);
  reglowgammanearpole(&lowgamma, k, &h, digits);
  result = regulizingfactor(&factor, a, digits, 0)
           && float_mul(&lowgamma, &lowgamma, &factor, digits+2)
           && addgammapole(&lowgamma, x, k, &h, digits);
  float_move(x, &lowgamma); // frees lowgamma
  float_free(&lowgamma);
  float_free(&h);
  return result;
}

void
testincgamma(floatnum x, cfloatnum a, int digits){
  lowgammanearpole(x, a, digits);
}
