/* floatgamma.c: Gamma function, based on floatnum. */
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

#include "floatgamma.h"
#include "floatconst.h"
#include "floatcommon.h"
#include "floatlog.h"
#include "floatexp.h"
#include "floattrig.h"

/* asymptotic series of the Binet function
   for x >= 77 and a 100 digit computation, the
   relative error is < 9e-100.
   the series converges, if x and digits comply to
     100 >= digits >= 2
     x >= sqrt((digits*ln 10 + 0.5*ln 2)/1.0033).
   As a special case, for digits == 1, convergence is guaranteed,
   if x >= 1.8. */

char
binetasymptotic(floatnum x,
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
    dominated by x*ln x and the Binet function
    does not contribute anything substantial to
    the final result */
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

/* returns the number of summands needed in the asymptotic
   series to guarantee <digits> precision. Each extra summand
   yields roughly extra 1.8 digits. This is derived under the
   assumption, that the costs of an extra factor in the rising
   pochhammer symbol are about the same than those of an extra
   summand in the series */
static int
_findorder(
  int digits)
{
  return (5*digits + 5)/9;
}

/* returns how big x has to be to let the asymptotic series
   converge to at least <digits> precision. Derived from
   an estimation of the Bernouilli number inserted in the
   formula of a summand. */
static int
_minx(
  int digits)
{
  return (4657*_findorder(digits)-2750)/5000;
}

/* returns how much x has to be increased to let the
   asymptotic series converge to <digits> places */
static int
_ofs(
  floatnum x,
  int digits)
{
  int result;

  if (float_getexponent(x) >= 8)
    return 0;
  result = _minx(digits) - float_asinteger(x);
  return result <= 0? 0 : result;
}

/* evaluates the rising pochhammer symbol x*(x+1)*...*(x+n-1)
   (n >= 0) by multiplying. This can be expensive when n is large,
   so better restrict n to something sane like n <= 100.
   su stands for "small" and "unsigned" n */
static char
_pochhammer_su(
  floatnum x,
  int n,
  int digits)
{
  floatstruct factor;
  char result;

  /* the rising pochhammer symbol is computed recursively,
  observing that
  pochhammer(x, n) == pochhammer(x, p) * pochhammer(x+p, n-p).
  p is choosen as floor(n/2), so both factors are somehow
  "balanced". This pays off, if x has just a few digits, since
  only some late multiplications are full scale then and
  Karatsuba boosting yields best results, because both factors
  are always almost the same size. */

  result = 1;
  switch (n)
  {
  case 0:
    float_copy(x, &c1, EXACT);
  case 1:
    break;
  default:
    float_create(&factor);
    float_addi(&factor, x, n >> 1, digits+2);
    result = _pochhammer_su(x, n >> 1, digits)
        && _pochhammer_su(&factor, n - (n >> 1), digits)
        && float_mul(x, x, &factor, digits+2);
    float_free(&factor);
  }
  return result;
}

/* evaluates ln(Gamma(x)) for all those x big
   enough to let the asymptotic series converge directly.
   Returns 0, if the result overflows
   relative error for a 100 gigit calculation < 5e-100 */
static char
_lngammabigx(
  floatnum x,
  int digits)
{
  floatstruct tmp1, tmp2;
  char result;

  result = 0;
  float_create(&tmp1);
  float_create(&tmp2);
  /* compute (ln x-1) * (x-0.5) - 0.5 + ln(sqrt(2*pi)) */
  float_copy(&tmp2, x, digits+1);
  _ln(&tmp2, digits+1);
  float_sub(&tmp2, &tmp2, &c1, digits+2);
  float_sub(&tmp1, x, &c1Div2, digits+2);
  if (float_mul(&tmp1, &tmp1, &tmp2, digits+2))
  {
    /* no overflow */
    binetasymptotic(x, digits);
    float_add(x, &tmp1, x, digits+3);
    float_add(x, x, &cLnSqrt2PiMinusHalf, digits+3);
    result = 1;
  }
  float_free(&tmp2);
  float_free(&tmp1);
  return result;
}

static char
_lngamma_prim_xgt0(
  floatnum x,
  floatnum revfactor,
  int digits)
{
  int ofs;

  ofs = _ofs(x, digits);
  float_copy(revfactor, x, digits+1);
  _pochhammer_su(revfactor, ofs, digits);
  float_addi(x, x, ofs, digits+2);
  return _lngammabigx(x, digits);
}

static char
_lngamma_prim(
  floatnum x,
  floatnum revfactor,
  int* infinity,
  int digits)
{
  floatstruct tmp;
  char result;
  char odd;

  *infinity = 0;
  if (float_getsign(x) > 0)
    return _lngamma_prim_xgt0(x, revfactor, digits);
  float_copy(revfactor, x, digits + 2);
  float_sub(x, &c1, x, digits+2);
  float_create(&tmp);
  result = _lngamma_prim_xgt0(x, &tmp, digits);
  if (result)
  {
    float_neg(x);
    odd = float_isodd(revfactor);
    _sinpix(revfactor, digits);
    if (float_iszero(revfactor))
    {
      *infinity = 1;
      float_setinteger(revfactor, odd? -1 : 1);
    }
    else
      float_mul(&tmp, &tmp, &cPi, digits+2);
    float_div(revfactor, revfactor, &tmp, digits+2);
  }
  float_free(&tmp);
  return result;
}

char
_lngamma(
  floatnum x,
  int digits)
{
  floatstruct factor;
  int infinity;
  char result;

  if (float_cmp(x, &c1) == 0 || float_cmp(x, &c2) == 0)
    return _setzero(x);
  float_create(&factor);
  result = _lngamma_prim(x, &factor, &infinity, digits)
           && infinity == 0;
  if (result)
  {
    float_abs(&factor);
    _ln(&factor, digits + 1);
    result = float_sub(x, x, &factor, digits+1);
  }
  float_free(&factor);
  if (infinity != 0)
    return _seterror(x, ZeroDivide);
  if (!result)
    float_setnan(x);
  return result;
}

char
_gammagtminus20(
  floatnum x,
  int digits)
{
  floatstruct factor;
  int ofs;
  char result;

  float_create(&factor);
  ofs = _ofs(x, digits+1);
  float_copy(&factor, x, digits+1);
  _pochhammer_su(&factor, ofs, digits);
  float_addi(x, x, ofs, digits+2);
  result = _lngammabigx(x, digits)
           && _exp(x, digits)
           && float_div(x, x, &factor, digits+1);
  float_free(&factor);
  if (!result)
    float_setnan(x);
  return result;
}

char
_gamma(
  floatnum x,
  int digits)
{
  floatstruct tmp;
  int infinity;
  char result;

  if (float_cmp(&cMinus20, x) > 0)
  {
    float_create(&tmp);
    result = _lngamma_prim(x, &tmp, &infinity, digits)
             && infinity == 0
             && _exp(x, digits)
             && float_div(x, x, &tmp, digits + 1);
    float_free(&tmp);
    if (infinity != 0)
      return _seterror(x, ZeroDivide);
    if (!result)
      float_setnan(x);
    return result;
  }
  return _gammagtminus20(x, digits);
}

char
_gammaint(
  floatnum integer,
  int digits)
{
  int ofs;

  if (float_getexponent(integer) >=2)
    return _gammagtminus20(integer, digits);
  ofs = float_asinteger(integer);
  float_copy(integer, &c1, EXACT);
  return _pochhammer_su(integer, ofs-1, digits);
}

char
_gamma0_5(
  floatnum x,
  int digits)
{
  floatstruct tmp;
  int ofs;

  if (float_getexponent(x) >= 2)
    return _gamma(x, digits);
  float_create(&tmp);
  float_sub(&tmp, x, &c1Div2, EXACT);
  ofs = float_asinteger(&tmp);
  float_free(&tmp);
  if (ofs >= 0)
  {
    float_copy(x, &c1Div2, EXACT);
    if(!_pochhammer_su(x, ofs, digits))
      return 0;
    return float_mul(x, x, &cSqrtPi, digits);
  }
  if(!_pochhammer_su(x, -ofs, digits))
    return 0;
  return float_div(x, &cSqrtPi, x, digits);
}

static char
_pochhammer_si(
  floatnum x,
  int n,
  int digits)
{
  /* this extends the rising Pochhammer symbol to negative integer offsets
     following the formula pochhammer(x,n-1) = pochhammer(x,n)/(x-n+1) */

  if (n >= 0)
    return _pochhammer_su(x, n, digits);
  return float_addi(x, x, n, digits)
         && _pochhammer_su(x, -n, digits)
         && float_reciprocal(x, digits);
}

static char
_pochhammer_g(
  floatnum x,
  cfloatnum n,
  int digits)
{
  /* this generalizes the rising Pochhammer symbol using the
     formula pochhammer(x,n) = Gamma(x+1)/Gamma(x-n+1) */
  floatstruct tmp, factor1, factor2;
  int inf1, inf2;
  char result;

  float_create(&tmp);
  float_create(&factor1);
  float_create(&factor2);
  inf2 = 0;
  float_add(&tmp, x, n, digits+1);
  result = _lngamma_prim(x, &factor1, &inf1, digits)
           && _lngamma_prim(&tmp, &factor2, &inf2, digits)
           && (inf2 -= inf1) <= 0;
  if (inf2 > 0)
    float_seterror(ZeroDivide);
  if (result && inf2 < 0)
    float_setzero(x);
  if (result && inf2 == 0)
    result = float_div(&factor1, &factor1, &factor2, digits+1)
             && float_sub(x, &tmp, x, digits+1)
             && _exp(x, digits)
             && float_mul(x, x, &factor1, digits+1);
  float_free(&tmp);
  float_free(&factor2);
  float_free(&factor1);
  if (!result)
    float_setnan(x);
  return result;
}

static char
_pochhammer_i(
  floatnum x,
  cfloatnum n,
  int digits)
{
  /* do not use the expensive Gamma function when a few
     multiplications do the same */
  /* pre: n is an integer */
  int ni;
  signed char result;

  if (float_iszero(n))
    return float_copy(x, &c1, EXACT);
  if (float_isinteger(x))
  {
    result = -1;
    float_neg((floatnum)n);
    if (float_getsign(x) <= 0 && float_cmp(x, n) > 0)
      /* x and x+n have opposite signs, meaning 0 is
         among the factors */
      result = _setzero(x);
    else if (float_getsign(x) > 0 && float_cmp(x, n) <= 0)
      /* x and x+n have opposite signs, meaning at one point
      you have to divide by 0 */
      result = _seterror(x, ZeroDivide);
    float_neg((floatnum)n);
    if (result >= 0)
      return result;
  }
  if (float_getexponent(x) < EXPMAX/100)
  {
    ni = float_asinteger(n);
    if (ni != 0 && ni < 50 && ni > -50)
      return _pochhammer_si(x, ni, digits+2);
  }
  return _pochhammer_g(x, n, digits);
}

char
_pochhammer(
  floatnum x,
  cfloatnum n,
  int digits)
{
  if (float_isinteger(n))
    return _pochhammer_i(x, n, digits);
  return _pochhammer_g(x, n, digits);
}
