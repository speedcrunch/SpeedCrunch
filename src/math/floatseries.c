/* floatseries.c: basic series, based on floatnum. */
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

#include "floatseries.h"
#include "floatconst.h"
#include "floatcommon.h"
#include "floatexp.h"

/* Though all these serieses can be used with arguments |x| < 1 or
   even more, the computation time increases rapidly with x.
   Tests show, that for 100 digit results, it is best to limit x
   to |x| < 0.01..0.02, and use reduction formulas for greater x */

/* the Taylor series of arctan/arctanh x at x == 0. For small
   |x| < 0.01 this series converges very fast, yielding 4 or
   more digits of the result with every summand. The working
   precision is adjusted, so that the relative error for
   100-digit arguments is around 5.0e-100. This means, the error
   is 1 in the 100-th place (or less) */
void
arctanseries(
  floatnum x,
  int digits,
  char alternating)
{
  int expx;
  int expsqrx;
  int pwrsz;
  int addsz;
  int i;
  floatstruct xsqr;
  floatstruct pwr;
  floatstruct smd;
  floatstruct sum;

  /* upper limit of log(x) and log(result) */
  expx = float_getexponent(x)+1;

  /* the summands of the series from the second on are
     bounded by x^(2*i-1)/3. So the summation yields a
     result bounded by (x^3/(1-x*x))/3.
     For x < sqrt(1/3) approx.= 0.5, this is less than 0.5*x^3.
     We need to sum up only, if the first <digits> places
     of the result (roughly x) are touched. Ignoring the effect of
     a possile carry, this is only the case, if
     x*x >= 2*10^(-digits) > 10^(-digits)
     Example: for x = 9e-51, a 100-digits result covers
     the decimal places from 1e-51 to 1e-150. x^3/3
     is roughly 3e-151, and so is the sum of the series.
     So we can ignore the sum, but we couldn't for x = 9e-50 */
  if (float_iszero(x) || 2*expx < -digits)
    /* for very tiny arguments arctan/arctanh x is approx.== x */
    return;
  float_create(&xsqr);
  float_create(&pwr);
  float_create(&smd);
  float_create(&sum);

  /* we adapt the working precision to the decreasing
     summands, saving time when multiplying. Unfortunately,
     there is no error bound given for the operations of
     bc_num. Tests show, that the last digit in an incomplete
     multiplication is usually not correct up to 5 ULP's. */
  pwrsz = digits + 2*expx + 1;
  /* the precision of the addition must not decrease, of course */
  addsz = pwrsz;
  i = 3;
  float_mul(&xsqr, x, x, pwrsz);
  float_setsign(&xsqr, alternating? -1 : 1);
  expsqrx = float_getexponent(&xsqr);
  float_copy(&pwr, x, pwrsz);
  float_setzero(&sum);

  for(; pwrsz > 0; )
  {
    /* x^i */
    float_mul(&pwr, &pwr, &xsqr, pwrsz+1);
    /* x^i/i */
    float_divi(&smd, &pwr, i, pwrsz);
    /* The addition virtually does not introduce errors */
    float_add(&sum, &sum, &smd, addsz);
    /* reduce the working precision according to the decreasing powers */
    pwrsz = digits - expx + float_getexponent(&smd) + expsqrx + 3;
    i += 2;
  }
  /* add the first summand */
  float_add(x, x, &sum, digits+1);

  float_free(&xsqr);
  float_free(&pwr);
  float_free(&smd);
  float_free(&sum);
}

/* series expansion of cos/cosh - 1 used for small x,
   |x| <= 0.01.
   The function returns 0, if an underflow occurs.
   The relative error seems to be less than 5e-100 for
   a 100-digit calculation with |x| < 0.01 */
char
cosminus1series(
  floatnum x,
  int digits,
  char alternating)
{
  floatstruct sum, smd;
  int expsqrx, pwrsz, addsz, i;

  expsqrx = 2 * float_getexponent(x);
  float_setexponent(x, 0);
  float_mul(x, x, x, digits+1);
  float_mul(x, x, &c1Div2, digits+1);
  float_setsign(x, alternating? -1 : 1);
  expsqrx += float_getexponent(x);
  if (float_iszero(x) || expsqrx < EXPMIN)
  {
    /* underflow */
    float_setzero(x);
    return expsqrx == 0;
  }
  float_setexponent(x, expsqrx);
  pwrsz = digits + expsqrx + 2;
  if (pwrsz <= 0)
    /* for very small x, cos/cosh(x) - 1 = (-/+)0.5*x*x */
    return 1;
  addsz = pwrsz;
  float_create(&sum);
  float_create(&smd);
  float_copy(&smd, x, pwrsz);
  float_setzero(&sum);
  i = 2;
  while (pwrsz > 0)
  {
    float_mul(&smd, &smd, x, pwrsz+1);
    float_divi(&smd, &smd, i*(2*i-1), pwrsz);
    float_add(&sum, &sum, &smd, addsz);
    ++i;
    pwrsz = digits + float_getexponent(&smd);
  }
  float_add(x, x, &sum, digits+1);
  float_free(&sum);
  float_free(&smd);
  return 1;
}

/* asymptotic series for ln Gamma(x)
   for x >= 77 and a 100 digit computation, the
   relative error is < 9e-100 */

char
lngammaasymptotic(
  floatnum x,
  int digits)
{
  floatstruct recsqr;
  floatstruct sum;
  floatstruct smd;
  floatstruct pwr;
  int i, workprec;

  if (float_getexponent(x) >= digits)
  {
    /* if x is very big, ln(gamma(x)) is
       dominated by x*ln x and this
       asymptotic series does not contribute
       anything substantial to the final
       result */
    float_setzero(x);
    return 1;
  }
  float_create(&recsqr);
  float_create(&sum);
  float_create(&smd);
  float_create(&pwr);

  float_copy(&pwr, &c1, EXACT); 
  float_setzero(&sum);
  float_div(&smd, &c1, &c12, digits+1);
  workprec = digits - 2*float_getexponent(x)+3;
  i = 1;
  if (workprec > 0)
  {
    float_mul(&recsqr, x, x, workprec);
    float_reciprocal(&recsqr, workprec);
    while (float_getexponent(&smd) > -digits-1
          && ++i <= MAXBERNOULLIIDX)
    {
      workprec = digits + float_getexponent(&smd) + 3;
      float_add(&sum, &sum, &smd, digits+1);
      float_mul(&pwr, &recsqr, &pwr, workprec);
      float_muli(&smd, &cBernoulliDen[i-1], 2*i*(2*i-1), workprec);
      float_div(&smd, &pwr, &smd, workprec);
      float_mul(&smd, &smd, &cBernoulliNum[i-1], workprec);
    }
  }
  else
    /* sum reduces to the first summand*/
    float_move(&sum, &smd);
  if (i > MAXBERNOULLIIDX)
      /* x was not big enough for the asymptotic
    series to converge sufficiently */
    float_setnan(x);
  else
    float_div(x, &sum, x, digits);
  float_free(&pwr);
  float_free(&smd);
  float_free(&sum);
  float_free(&recsqr);
  return i <= MAXBERNOULLIIDX;
}

/*
  The Taylor expansion of sqrt(pi)*erf(x)/2 around x = 0.
  converges only for small |x| < 1 sufficiently.
  erf(x) = SUM[i>=0] (x^(2*i+1)/(i! * (2*i+1)))
*/

char
erfseries(
  floatnum x,
  int digits)
{
  floatstruct xsqr, smd, pwr;
  int i, workprec, expsqr, expx;

  expx = float_getexponent(x);
  expsqr = 2 * expx + 2;
  if (-expsqr > digits || float_iszero(x))
    /* for tiny arguments approx. == x */
    return 1;
  float_create(&xsqr);
  float_create(&smd);
  float_create(&pwr);
  float_mul(&xsqr, x, x, digits + expsqr);
  workprec = digits + float_getexponent(&xsqr) + 1;
  float_copy(&pwr, x, workprec);
  i = 1;
  while (workprec > 0)
  {
    float_mul(&pwr, &pwr, &xsqr, workprec);
    float_divi(&pwr, &pwr, -i, workprec);
    float_divi(&smd, &pwr, 2 * i++ + 1, workprec);
    float_add(x, x, &smd, digits+2);
    workprec = digits + float_getexponent(&smd) - expx;
  }
  float_free(&pwr);
  float_free(&smd);
  float_free(&xsqr);
  return 1;
}

/* the asymptotic expansion of erfc, the bigger x the better */

char
erfcasymptotic(
  floatnum x,
  int digits)
{
  floatstruct smd, fct;
  int i, workprec, newprec;

  float_create(&smd);
  float_create(&fct);
  workprec = digits - 2 * float_getexponent(x);
  if (workprec < 0)
  {
    float_copy(x, &c1, EXACT);
    return 1;
  }
  float_mul(&fct, x, x, workprec);
  float_div(&fct, &c1Div2, &fct, workprec);
  float_neg(&fct);
  float_copy(&smd, &c1, EXACT);
  float_setzero(x);
  workprec = digits;
  newprec = digits;
  i = 1;
  while (newprec > 0 && newprec <= workprec)
  {
    workprec = newprec;
    float_add(x, x, &smd, digits);
    float_muli(&smd, &smd, i, workprec);
    float_mul(&smd, &smd, &fct, workprec);
    newprec = digits + float_getexponent(&smd);
    i += 2;
  }
  float_free(&fct);
  float_free(&smd);
  return newprec <= workprec;
}

static int erfcdigits = 0;

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
   f(t, alpha) is used in the evaluation of erfc(sqrt(t)) */

char
erfcsum(
  floatnum x, /* should be the square of the parameter to erfc */
  int digits)
{
  int steps, i, workprec;
  floatstruct sum, smd;
  floatnum Ei;

  steps = (1501 * digits) >> 11;
  if (steps > MAXERFCIDX)
  {
    /* not enough internal buffer for such a high precision */
    float_setnan(x);
    return 0;
  }
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
    float_muli(&erfcalpha, &cLn10, digits, 3);
    float_sqrt(&erfcalpha, 3);
    float_div(&erfcalpha, &cPi, &erfcalpha, 3);
    float_mul(&erfcalphasqr, &erfcalpha, &erfcalpha, digits);
    /* the exp(-k*k*alpha*alpha) are later evaluated iteratively.
       Initiate the iteration here */
    float_copy(&erfct2, &erfcalphasqr, EXACT);
    float_neg(&erfct2);
    _exp(&erfct2, digits); /* exp(-alpha*alpha) */
    float_copy(erfccoeff, &erfct2, EXACT); /* start value */
    float_mul(&erfct3, &erfct2, &erfct2, digits); /* exp(-2*alpha*alpha) */
  }
  float_create(&sum);
  float_create(&smd);
  float_setzero(&sum);
  workprec = digits + float_getexponent(erfccoeff) + 1;
  for (i = 0; ++i <= steps && workprec > 0;)
  {
    Ei = &erfccoeff[i-1];
    if (float_isnan(Ei))
    {
      /* if exp(-i*i*alpha*alpha) is not available, evaluate it from
         that of the last summand */
      float_mul(&erfct2, &erfct2, &erfct3, workprec);
      float_mul(Ei, &erfct2, &erfccoeff[i-2], workprec);
    }
    /* Ei finally decays rapidly. save some time by adjusting the
       working precision */
    workprec = digits + float_getexponent(Ei) + 1;
    /* evaluate the summand exp(-i*i*alpha*alpha)/(i*i*alpha*alpha+x) */
    float_muli(&smd, &erfcalphasqr, i*i, workprec);
    float_add(&smd, x, &smd, workprec);
    float_div(&smd, Ei, &smd, workprec);
    /* add summand to the series */
    float_add(&sum, &sum, &smd, digits + 1);
  }
  float_move(x, &sum);
  float_free(&smd);
  return 1;
}
