/* floaterf.c: normal distribution integrals erf and the like */
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

#include "floaterf.h"
#include "floatconst.h"
#include "floatcommon.h"
#include "floatexp.h"
#include "math.h"

/*
  The Taylor expansion of sqrt(pi)*erf(x)/2 around x = 0.
  converges only for small |x| < 1 sufficiently.
  erf(x) = SUM[i>=0] (x^(2*i+1)/(i! * (2*i+1)))

  relative error for 100-digit evaluation: < 3e-100
*/

char
erfseries(floatnum x,
          int digits)
{
  floatstruct xsqr, smd, pwr;
  int i, workprec, expx;

  expx = float_getexponent(x);
  workprec = digits + 2*expx + 2;
  if (workprec <= 0 || float_iszero(x))
    /* for tiny arguments approx. == x */
    return 1;
  float_create(&xsqr);
  float_create(&smd);
  float_create(&pwr);
  float_mul(&xsqr, x, x, workprec + 1);
  workprec = digits + float_getexponent(&xsqr) + 1;
  float_copy(&pwr, x, workprec + 1);
  i = 1;
  while (workprec > 0)
  {
    float_mul(&pwr, &pwr, &xsqr, workprec + 1);
    float_divi(&pwr, &pwr, -i, workprec + 1);
    float_divi(&smd, &pwr, 2 * i++ + 1, workprec);
    float_add(x, x, &smd, digits + 3);
    workprec = digits + float_getexponent(&smd) + expx + 2;
  }
  float_free(&pwr);
  float_free(&smd);
  float_free(&xsqr);
  return 1;
}

/* the asymptotic expansion of erfc, the bigger x is, the better.
   returns sum( (2*i+1)! /i! / x^(2*i)
   Relative error for x >= 16 and 100-digit evaluation is less than
   9e-100 */

char
erfcasymptotic(floatnum x,
               int digits)
{
  floatstruct smd, fct;
  int i, workprec, newprec;

  float_create(&smd);
  float_create(&fct);
  workprec = digits - 2 * float_getexponent(x) + 1;
  if (workprec <= 0)
  {
    float_copy(x, &c1, EXACT);
    return 1;
  }
  float_mul(&fct, x, x, digits + 1);
  float_div(&fct, &c1Div2, &fct, digits);
  float_neg(&fct);
  float_copy(&smd, &c1, EXACT);
  float_setzero(x);
  newprec = digits;
  workprec = newprec;
  i = 1;
  while (newprec > 0 && newprec <= workprec)
  {
    workprec = newprec;
    float_add(x, x, &smd, digits + 4);
    float_muli(&smd, &smd, i, workprec + 1);
    float_mul(&smd, &smd, &fct, workprec + 2);
    newprec = digits + float_getexponent(&smd) + 1;
    i += 2;
  }
  float_free(&fct);
  float_free(&smd);
  return newprec <= workprec;
}

/* this algorithm is based on a paper from Crandall, who in turn attributes
   to Chiarella and Reichel.
   Found this in a paper from Borwein, Bailey and Girgensohn, and added
   minor improvements such as the adaptive working precision.
   There is a restriction with this algorithm not mentioned in the paper:
   x must not be too large, because the correcting term 2/(1-exp(2*pi*x/alpha))
   becomes dominant and renders the result incorrect for large x. Fortunately,
   the valid range seems to overlap with the range of the asymptotic formula.

   Picks a fixed alpha suitable for the desired precision and evaluates the sum
   f(t, alpha) = Sum[k>0](exp(-k*k*alpha*alpha)/(k*k*alpha*alpha + t)
   f(t, alpha) is used in the evaluation of erfc(sqrt(t))

   alpha is dependent on the desired precision; For a precision of p
   places, alpha should be < pi/sqrt(p*ln 10). Unfortunately, the
   smaller alpha is, the worse is the convergence rate, so alpha is
   usually approximately its upper limit.

   relative error for 100-digit evaluation < 5e-100 */

char
erfcsum(floatnum x, /* should be the square of the parameter to erfc */
        int digits)
{
  int i;
  int workprec = 0;
  floatstruct sum, smd;
  floatnum Ei;

  if (digits > erfcdigits)
  {
    /* cannot re-use last evaluation's intermediate results */
    for (i = MAXERFCIDX; --i >= 0;)
      /* clear all exp(-k*k*alpha*alpha) to indicate their absence */
      float_free(&erfccoeff[i]);
    /* current precision */
    erfcdigits = digits;
    /* create new alpha appropriate for the desired precision
    This alpha need not be high precision, any alpha near the
    one evaluated here would do */
    float_setfloat(&erfcalpha, M_PI / aprxsqrt((digits + 4) * M_LN10));
    float_round(&erfcalpha, &erfcalpha, 3, TONEAREST);

    float_mul(&erfcalphasqr, &erfcalpha, &erfcalpha, EXACT);
    /* the exp(-k*k*alpha*alpha) are later evaluated iteratively.
    Initiate the iteration here */
    float_copy(&erfct2, &erfcalphasqr, EXACT);
    float_neg(&erfct2);
    _exp(&erfct2, digits + 3); /* exp(-alpha*alpha) */
    float_copy(erfccoeff, &erfct2, EXACT); /* start value */
    float_mul(&erfct3, &erfct2, &erfct2, digits + 3); /* exp(-2*alpha*alpha) */
  }
  float_create(&sum);
  float_create(&smd);
  float_setzero(&sum);
  for (i = 0; ++i < MAXERFCIDX;)
  {
    Ei = &erfccoeff[i-1];
    if (float_isnan(Ei))
    {
      /* if exp(-i*i*alpha*alpha) is not available, evaluate it from
      the coefficient of the last summand */
      float_mul(&erfct2, &erfct2, &erfct3, workprec + 3);
      float_mul(Ei, &erfct2, &erfccoeff[i-2], workprec + 3);
    }
    /* Ei finally decays rapidly. save some time by adjusting the
    working precision */
    workprec = digits + float_getexponent(Ei) + 1;
    if (workprec <= 0)
      break;
    /* evaluate the summand exp(-i*i*alpha*alpha)/(i*i*alpha*alpha+x) */
    float_muli(&smd, &erfcalphasqr, i*i, workprec);
    float_add(&smd, x, &smd, workprec + 2);
    float_div(&smd, Ei, &smd, workprec + 1);
    /* add summand to the series */
    float_add(&sum, &sum, &smd, digits + 3);
  }
  float_move(x, &sum);
  float_free(&smd);
  return 1;
}

/* checks the quality of the asymptotic series for erfc.
   If the ratio of two subsequent summands from the series
   (the convergence rate) should not fall below `ratio'
   for a desired result precision (represented by digits), the number
   of summands n must not be greater than
    n <= (`digits'*ln 10 + 0.5 * ln 2)/(1 - ln `ratio')
   and the parameter x has to fullfil
    x >= sqrt(n/`ratio')
   `ratio' must be a value < 1, If you pick a value close to
   1, you finally have to add quite a lot of summands from the
   series (in low precision), that affect a few digits at the low
   end of the result only. On the other hand, choosing a good
   convergence rate pushes the validity range of the series towards
   larger x.
   Here, the convergence rate is chosen to be 0.5, meaning that the
   addition of a summand from the series at least halfs the magnitude
   of the tail of the series.
   The evaluation is carried out in low precision using interger
   arithmetic rather than floating point data.
   For a 100 digit result the lower boundary of the range of the
   asymptotic series (truncated when the convergence rate falls below 0.5)
   is x > approx. 16.5.
   The above formulas estimate the limit x slightly too small, especially
   when `digits' is small. So, to compensate for that, r should be at least
   <= 0.92 */
static char
_asymptotic_good(
  floatnum x,
  int digits)
{
  /* all constants scaled by 10000 */
  /* 1/ratio */
#define RATIO 20000
  /* (1 - ln ratio) */
#define C_RATIO 16931
  /* ln 10 */
#define LN10 23026
  /* 0.5*ln 2 */
#define LN2DIV2 3466

  int n, ix;

  if (!float_isvalidexp(float_getexponent(x) + 2)
      || (digits == 1 && float_cmp(x, &c2) >= 0))
    return 1;
  /* 10000 * n/ratio */
  n = RATIO*((digits * LN10 + LN2DIV2) / C_RATIO);
  float_addexp(x, 2);
  ix = float_asinteger(x);
  float_addexp(x, -2);
  return ix == 0 || ix >= 0x10000 || ix * ix >= n;
}

static int _logexpxsqr(int exp)
{
  if (exp < 0)
    exp = 0;
  if (exp >= 0x1000)
    exp = 0x1000;
  return ((exp * exp * 73) >> 5);
}

char
_erf(
  floatnum x,
  int digits)
{
  int workprec;
  signed char sign;

  sign = float_getsign(x);
  float_abs(x);
  if (float_cmp(x, &c1Div2) > 0)
  {
    workprec = digits - _logexpxsqr(float_getexponent(x));
    if (workprec < 0 || !_erfc(x, workprec))
      float_setzero(x);
    float_sub(x, &c1, x, digits + 1);
  }
  else
  {
    erfnear0(x, digits);
    float_mul(x, x, &c2DivSqrtPi, digits + 2);
  }
  float_setsign(x, sign);
  return 1;
}

char
_erfc(
  floatnum x,
  int digits)
{
  floatstruct tmp, t2, t3;
  int expx, prec;
  char result;

  if (float_cmp(x, &c1Div2) <= 0)
  {
    /* use erfc(x) = 1 - erf(x) for small or negative x */
    prec = digits; /* default for negative x, result is approx. 1 */
    expx = float_getexponent(x);
    if (expx < 0)
    {
      /* |x| < 1, but not 0 */
      prec = expx + digits + 2;
      if (prec <= 0)
      {
        float_copy(x, &c1, EXACT);
        return 1;
      }
    }
    _erf(x, prec);
    float_sub(x, &c1, x, digits + 1);
    return 1;
  }
  float_create(&tmp);
  if (_asymptotic_good(x, digits))
  {
    if (float_mul(&tmp, x, x, digits + 5)
        && _exp(&tmp, digits + 3)
        && float_mul(&tmp, &tmp, x, digits + 3)
        && float_div(&tmp, &c1DivSqrtPi, &tmp, digits + 3))
    {
      if (!erfcbigx(x, digits))
        result = _seterror(x, EvalUnstable);
      else
        result = float_mul(x, x, &tmp, digits + 4);
    }
    else
      result = _seterror(x, Underflow);
  }
  else
  {
    result = 1;
    float_create(&t2);
    float_create(&t3);
    float_mul(&t2, x, x, digits + 2);
    float_copy(&tmp, &t2, EXACT);
    erfcsum(&tmp, digits);
    float_add(&tmp, &tmp, &tmp, digits + 1);
    float_copy(&t3, &t2, EXACT);
    float_reciprocal(&t2, digits + 1);
    float_add(&tmp, &tmp, &t2, digits + 2);
    float_neg(&t3);
    _exp(&t3, digits + 2);
    float_mul(&t3, &t3, &tmp, digits + 2);
    float_mul(&tmp, &erfcalpha, x, digits + 2);
    float_mul(&t3, &tmp, &t3, digits + 3);
    float_mul(&t3, &c1DivPi, &t3, digits + 2);
    /* quick estimate to find the right working precision */
    float_div(&tmp, x, &erfcalpha, 4);
    float_mul(&tmp, &tmp, &c2Pi, 4);
    float_div(&tmp, &tmp, &cLn10, 4);
    prec = digits - float_getexponent(&t3) - float_asinteger(&tmp) + 1;
    /* add correction term */
    if (prec > 0)
    {
      float_div(&tmp, x, &erfcalpha, prec + 3);
      float_mul(&tmp, &tmp, &c2Pi, prec + 4);
      _exp(&tmp, prec);
      float_sub(&tmp, &c1, &tmp, prec);
      float_div(&tmp, &c2, &tmp, prec);
      float_add(&t3, &t3, &tmp, digits + 1);
    }
    float_free(&t2);
    float_move(x, &t3);
  }
  float_free(&tmp);
  return result;
}
