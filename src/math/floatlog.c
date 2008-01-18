/* floatlog.c: logarithm and friends, based on floatnum. */
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

#include "floatlog.h"
#include "floatconst.h"
#include "floatcommon.h"
#include "floatseries.h"

typedef struct
{
  char c2;
  char c3;
  char c5;
  char c7;
  char c10;
} _lincomb;

static _lincomb _lincombtbl[] =
{
  {0,3,3,2,5},{12,4,0,2,7},{4,0,0,0,1},{0,2,2,1,3},{13,3,0,1,6}, /* -.40 */
  {7,5,0,2,6},{10,1,0,2,5},{4,3,0,3,5},{1,6,0,0,3},{4,2,0,0,2},  /* -.35 */
  {0,4,2,1,4},{1,0,0,1,1},{0,9,0,1,5},{3,5,0,1,4},{6,1,0,1,3},   /* -.30 */
  {0,3,0,2,3},{8,6,0,1,6},{11,2,0,1,5},{0,6,2,1,5},{22,1,0,0,7}, /* -.25 */
  {9,5,0,0,5},{0,0,2,2,3},{0,5,1,0,3},{2,1,0,0,1},{0,3,4,1,5},   /* -.20 */
  {0,1,8,0,6},{0,3,3,3,6},{5,6,0,2,6},{8,2,0,2,5},{0,13,0,1,7},  /* -.15 */
  {12,3,0,0,5},{0,7,1,0,4},{0,2,1,4,5},{0,7,0,2,5},{3,3,0,2,4},  /* -.10 */
  {20,0,0,0,6},{7,4,0,0,4},{10,0,0,0,3},{8,4,0,2,6},{0,0,0,0,0}, /* -.05 */
  {0,0,0,0,0},{0,9,1,0,5},{13,5,0,2,8},{9,3,0,1,5},{0,7,4,1,7},  /* 0.00 */
  {10,3,0,3,7},{0,1,5,0,4},{0,12,2,1,8},{10,2,0,0,4},{8,6,0,2,7},/* 0.05 */
  {8,1,0,6,8},{7,0,0,1,3},{5,4,0,3,6},{22,1,0,1,8},{2,7,0,0,4},  /* 0.10 */
  {2,2,0,4,5},{0,0,2,3,4},{0,5,1,1,4},{0,3,5,0,5},{14,6,0,1,8},  /* 0.15 */
  {10,4,0,0,5},{3,1,0,3,4},{4,6,0,1,5},{0,4,0,0,2},{14,0,0,2,6}, /* 0.20 */
  {0,13,1,0,7},{8,2,0,3,6},{4,0,0,2,3},{2,4,0,4,6},{0,2,2,3,5},  /* 0.25 */
  {0,0,6,2,6},{0,5,5,0,6},{23,2,0,0,8},{0,1,2,0,2},{17,4,0,1,8}, /* 0.30 */
  {0,10,3,0,7},{11,6,0,2,8},{4,3,0,5,7},{14,2,0,2,7},{10,0,0,1,4},/* 0.35 */
  {8,4,0,3,7},{1,1,0,6,6},{11,0,0,3,6},{12,5,0,1,7},{2,1,0,8,8}, /* 0.40 */
  {15,1,0,1,6},{0,7,5,0,7},{6,2,0,6,8},{2,0,0,5,5},{1,14,0,1,8}, /* 0.45 */
  {0,12,3,0,8},{20,2,0,1,8},{0,8,0,0,4},{14,4,0,2,8},{3,4,0,0,3},/* 0.50 */
  {0,1,4,3,6},{6,0,0,0,2},{4,4,0,2,5},{0,2,0,1,2},{8,0,1,2,5},   /* 0.55 */
  {8,5,0,0,5},{15,3,0,1,7},{0,9,5,0,8},{0,0,3,2,4},{0,5,2,0,4},  /* 0.60 */
  {5,3,0,1,4},{0,0,2,4,5},{13,6,0,0,7},{3,2,0,7,8},{16,2,0,0,6}, /* 0.65 */
  {0,1,9,0,7},{3,6,0,0,4},{10,4,0,1,6},{0,0,0,8,7},{13,0,0,1,5}, /* 0.70 */
  {0,6,7,0,8},{0,4,0,1,3},{0,2,4,0,4},{3,0,0,1,2},{15,5,0,1,8},  /* 0.75 */
  {8,2,0,4,7},{2,9,0,1,6},{4,0,0,3,4},{2,4,0,5,7},{12,3,0,2,7},  /* 0.80 */
  {1,3,0,0,2},{0,0,6,3,7},{6,5,0,3,7},{16,4,0,0,7},{0,3,9,0,8},  /* 0.85 */
  {0,1,2,1,3},{10,6,0,1,7},{0,2,0,8,8},{0,10,3,1,8},{0,1,1,3,4}, /* 0.90 */
  {9,0,0,0,3},{4,3,0,6,8},{0,4,4,0,5},{8,9,0,0,7},{10,0,0,2,5}   /* 0.95 */
};

/* artanh x and ln(x+1) are closely related. Evaluate the logarithm
   using the artanh series, which converges twice as fast as the
   logarithm series.
   With 100 digits, if -0.0198 <= x <= 0.02, the relative error is
   less than 5.0e-100, or at most 1 unit in the 100th place */
void
_lnxplus1near0(
  floatnum x,
  int digits)
{
  floatstruct tmp;

  float_create(&tmp);
  float_add(&tmp, &c2, x, digits);
  float_div(x, x, &tmp, digits);
  artanhnear0(x, digits);
  float_add(x, x, x, EXACT);
  float_free(&tmp);
}

/* helpers */
static void
_addcoef(
  floatnum x,
  int coef,
  floatnum cnst,
  int digits)
{
  floatstruct tmp;

  if (coef != 0)
  {
    float_create(&tmp);
    float_muli(&tmp, cnst, coef, digits);
    float_add(x, x, &tmp, digits);
    float_free(&tmp);
  }
}

static int
_factor(
  int idx)
{
  int factor;
  char i;

  factor = 1;
  for (i = _lincombtbl[idx].c2; --i >= 0;)
    factor *= 2;
  for (i = _lincombtbl[idx].c3; --i >= 0;)
    factor *= 3;
  for (i = _lincombtbl[idx].c5; --i >= 0;)
    factor *= 5;
  for (i = _lincombtbl[idx].c7; --i >= 0;)
    factor *= 7;
  return factor;
}

static void
_lnguess(
  floatnum dest,
  int digits,
  int idx)
{
  _addcoef(dest, _lincombtbl[idx].c2-_lincombtbl[idx].c5, &cLn2, digits);
  _addcoef(dest, _lincombtbl[idx].c3, &cLn3, digits);
  _addcoef(dest, _lincombtbl[idx].c7, &cLn7, digits);
  _addcoef(dest, _lincombtbl[idx].c5-_lincombtbl[idx].c10, &cLn10, digits);
}

/* reduces x using a special factor whose primes factors
   are 2, 3, 5 and 7 only. x is multiplied by this
   factor, yielding a value near a power of ten.
   Then x is divided by this power of ten.
   The logarithm of this factor is returned in
   lnguess. Valid for -0.4 <= x < 1. Relative error < 5e-100 for
   100 digit result.
   This algorithm reduces x to a value < 0.01, which
   is appropriate small for submitting to a series evaluation */
void
_lnreduce(
  floatnum x,
  floatnum lnguess,
  int digits)
{
  floatstruct tmp1, tmp2;
  int idx;
  int expx;
  signed char pos;

  float_setzero(lnguess);
  expx = float_getexponent(x);
  if (expx < -2)
    return;
  float_create(&tmp1);
  float_create(&tmp2);

  idx = leadingdigits(x, 3 + expx) + 39;
  if (idx < 0)
    idx = 0;
  pos = idx >= 40? 1 : 0;
  idx += pos;
  float_setinteger(&tmp1, _factor(idx));
  float_setexponent(&tmp1, -pos);
  float_sub(&tmp1, &tmp1, &c1, EXACT);
  float_mul(&tmp2, x, &tmp1, digits);
  float_add(&tmp1, &tmp1, &tmp2, digits+1);
  float_add(x, x, &tmp1, digits);
  _lnguess(lnguess, digits+4, idx);
  float_free(&tmp1);
  float_free(&tmp2);
  return;
}

/* for -0.4 <= x < 1.0 */
void
_lnxplus1lt1(
  floatnum x,
  int digits)
{
  floatstruct lnfactor;

  float_create(&lnfactor);
  _lnreduce(x, &lnfactor, digits);
  _lnxplus1near0(x, digits);
  float_sub(x, x, &lnfactor, digits+1);
  float_free(&lnfactor);
}

/* the general purpose routine evaluating ln(x) for all
   positive arguments. It uses multiplication and division to
   reduce the argument to a value near 1. The factors are
   always small, so these operations do not take much time.
   --
   One often sees an argument reduction by taking the
   square root several times in succession. But the square root is
   slow in number.c, so we avoid it best.
   --
   Super-fast algorithm like the AGM based ones, do not
   pay off for limited lengths because of the same reason: They
   require taking square roots several times. A test showed,
   a hundred digits calculation using AGM is 8 times slower than
   the algorithm here. For extreme precision, of course, AGM will be
   superior.
   The relative error seems to be less than 7e-101 for 100-digits
   computations */
void
_ln(
  floatnum x,
  int digits)
{
  floatstruct tmp;
  int coef10;
  char coef3;
  char dgt;

  float_create(&tmp);
  coef10 = float_getexponent(x);

  /* reducing the significand to 0.6 <= x < 2
     by simple multiplication */
  dgt = leadingdigits(x, 1);
  coef3 = 0;
  if (dgt == 1)
    float_setexponent(x, 0);
  else
  {
    ++coef10;
    float_setexponent(x, - 1);
    coef3 = (dgt < 6);
    if (coef3)
      float_mul(x, x, &c3, digits+1);
  }
  float_sub(x, x, &c1, digits+1);
  _lnxplus1lt1(x, digits);
  if (coef10 != 0)
  {
    float_muli(&tmp, &cLn10, coef10, digits+1);
    float_add(x, x, &tmp, digits+1);
  }
  if (coef3)
    float_sub(x, x, &cLn3, digits);
  float_free(&tmp);
}

void
_lnxplus1(
  floatnum x,
  int digits)
{
  if (float_cmp(x, &cMinus0_4) >= 0 && float_cmp(x, &c1) < 0)
    _lnxplus1lt1(x, digits);
  else
  {
    float_add(x, x, &c1, digits+1);
    _ln(x, digits);
  }
}

/* The artanh function has a pole at x == 1.
   For values close to 1 the general purpose
   evaluation is too unstable. We use a dedicated
   algorithm here to get around the problems.
   Avoid using this function for values of x > 0.5,
   _artanh is the better choice then.
   Values near the other pole of artanh at -1 can
   be derived from this function using
   artanh(-1+x) = -artanh(1-x).
   For x < 0.5 and a 100-digit computation, the maximum
   relative error is in the order of 1e-99 */
void
_artanh1minusx(
  floatnum x,
  int digits)
{
  floatstruct tmp;

  float_create(&tmp);
  float_sub(&tmp, &c2, x, digits);
  float_div(x, &tmp, x, digits);
  _ln(x, digits);
  float_mul(x, x, &c1Div2, digits);
  float_free(&tmp);
}

/* designed for |x| <= 0.5. The evaluation is not
   symmetric with respect to 0, i.e. it might be
   -artanh x != artanh -x. The difference
   is in the order of the last digit, of course.
   Evaluation of positive values yield a slightly
   better relative error than that of negative
   values.
   The maximum relative error is the maximum of
   that of artanhnear0 and 1e-99  */

void
_artanhlt0_5(
  floatnum x,
  int digits)
{
  floatstruct tmp;

  if (float_getexponent(x) < -2 || float_iszero(x))
    artanhnear0(x, digits);
  else
  {
    float_create(&tmp);
    float_sub(&tmp, &c1, x, digits+1);
    float_add(x, x, x, digits);
    float_div(x, x, &tmp, digits);
    _lnxplus1(x, digits);
    float_mul(x, x, &c1Div2, digits);
    float_free(&tmp);
  }
}

/* the general purpose routine for evaluating artanh.
   The evaluation is symmetric with respect to 0, i.e.
   it is always -artanh(x) == artanh -x.
   Valid for |x| < 1, but unstable for |x| approx. == 1 */
void
_artanh(
  floatnum x,
  int digits)
{
  signed char sgn;

  sgn = float_getsign(x);
  float_abs(x);
  if (float_cmp(x, &c1Div2) <= 0)
    _artanhlt0_5(x, digits);
  else
  {
    float_sub(x, &c1, x, digits+1);
    _artanh1minusx(x, digits);
  }
  float_setsign(x, sgn);
}

/* evaluates ln(2*x), the asymptotic function of arsinh and
   arcosh for large x */
static void
_ln2x(
  floatnum x,
  int digits)
{
  _ln(x, digits);
  float_add(x, &cLn2, x, digits);
}

/* valid for all x. The relative error is less than 1e-99.
   The evaluation is symmetric with respect to 0:
   arsinh -x == - arsinh x */
void
_arsinh(
  floatnum x,
  int digits)
{
  floatstruct tmp;
  int expxsqr;
  signed char sgn;

  expxsqr = 2*float_getexponent(x)+2;
  /* for extreme small x, sqrt(1+x*x) is approx == 1 + x*x/2 - x^4/8,
     so arsinh x == ln(1 + x + x*x/2 - x^4/8 +...)
           approx == x - x*x*x/6 + ...
     If we approximate arsinh x by x, the relative error is
     roughly |x*x/6|. This is less than acceptable 10^(-<digits>), if
     log(x*x) < -<digits> */
  if (expxsqr < -digits || float_iszero(x))
    return;
  float_create(&tmp);

  /* arsinh -x = -arsinh x, so we can derive the
     arsinh for negative arguments from that
     of positive arguments */
  sgn = float_getsign(x);
  float_abs(x);
  if (expxsqr-2 > digits)
    /* for very large x, use the asymptotic formula ln(2*x) */
    _ln2x(x, digits);
  else
  {
    float_mul(&tmp, x, x, digits + (expxsqr>=0? 0 : expxsqr));
    float_add(&tmp, &tmp, &c1, digits+1);
    float_sqrt(&tmp, digits);
    if (float_getexponent(x) < 0)
    {
      /* for small x, use arsinh x == artanh (x/sqrt(1+x*x)).
         Stable for x < 1, but not for large x*/
      float_div(x, x, &tmp, digits);
      _artanh(x, digits+1);
    }
    else
    {
      /* arsinh x = ln(x+sqrt(1+x*x)), stable for x >= 1, but
         not for small x */
      float_add(x, x, &tmp, digits);
      _ln(x, digits);
    }
  }
  float_setsign(x, sgn);
  float_free(&tmp);
}

/* arcosh(x+1), x >= 0, is the stable variant of
   arcosh(x), x >= 1.
   The relative error is less than 1e-99. */
void
_arcoshxplus1(
  floatnum x,
  int digits)
{
  floatstruct tmp;

  float_create(&tmp);
  if (2*float_getexponent(x) > digits)
  {
    /* for very large x, use the asymptotic formula ln(2*(x+1)) */
    float_add(x, x, &c1, digits+1);
    _ln2x(x, digits);
  }
  else
  {
    /* arcosh(x+1) = ln(1+(x+sqrt(x*(x+2)))), stable
       for all positive x, except for extreme large x, where
       x*(x+2) might overflow */

    /* get sinh(arcosh (1+x)) = sqrt(x*(x+2)) */
    float_add(&tmp, x, &c2, digits);
    float_mul(&tmp, x, &tmp, digits);
    float_sqrt(&tmp, digits);

    float_add(x, x, &tmp, digits);
    _lnxplus1(x, digits);
  }
  float_free(&tmp);
}
