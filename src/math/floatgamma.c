/* floatgamma.c: Gamma function, based on floatnum. */
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

#include "floatconfig.h"
#include "floatgamma.h"
#include "floatconst.h"
#include "floatcommon.h"
#include "floatlog.h"
#include "floatexp.h"
#include "floattrig.h"
#include "floatseries.h"

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
  case 1: /* fall through */
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

#if 0
static char
_lnpochhammerbigx(
  floatnum x,
  floatnum delta,
  int digits)
{
  floatstruct tmp1, tmp2, tmp3;
  char result;

  if (float_getexponent(delta) < -digits)
  {
    _ln(x, 5);
    return float_mul(x, x, delta, 5);
  }
  float_create(&tmp1);
  float_create(&tmp2);
  float_div(&tmp1, delta, x, digits);
  _lnxplus1(&tmp1, digits);
  float_sub(&tmp2, x, &c1Div2, digits+2);
  float_mul(&tmp2, &tmp2, &tmp1, digits);
  if (float_getexponent(x) < digits)
  {
    float_create(&tmp3);
    float_add(&tmp3, x, delta, digits+2);
    lngammaseries(&tmp3, digits);
    float_add(&tmp2, &tmp3, &tmp2, digits+1);
    float_clone(&tmp3, x, digits+2);
    lngammaseries(&tmp3, digits);
    float_sub(&tmp2, &tmp2, &tmp3, digits);
    float_free(&tmp3);
  }
  _ln(x, digits);
  float_add(x, x, &tmp1, digits+2);
  float_sub(x, x, &c1, digits+2);
  result = float_mul(x, x, delta, digits+2)
           && float_add(x, x, &tmp1, digits+2);
  float_free(&tmp1);
  float_free(&tmp2);
  return result;
}

static int
_shiftarg(
  floatnum x,
  floatnum revfactor,
  int digits)
{
  int ofs;

  ofs = _ofs(x, digits);
  if (ofs > 0)
  {
    float_clone(revfactor, x, digits+2);
    _pochhammer_su(revfactor, ofs+1, digits);
    addi(x, x, ofs+1, digits+2);
  }
  else
    float_clone(revfactor, &c1, EXACT);
  return ofs;
}

static char
_pochhammergt0deltalt1(
  floatnum x,
  floatnum delta,
  int digits)
{
  floatstruct tmp1, tmp2;
  int ofs;
  char result;

  float_create(&tmp1);
  float_create(&tmp2);
  ofs = _shiftarg(x, &tmp1, digits);
  float_add(&tmp2, x, delta, digits+2);
  _pochhammer_su(&tmp2, ofs, digits);
  result = _lnpochhammerbigx(x, delta, digits);
  if (result)
    float_div(delta, &tmp1, &tmp2, digits);
  float_free(&tmp2);
  float_free(&tmp1);
  return result;
}

#endif

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
    lngammaasymptotic(x, digits);
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
  {
    float_setzero(x);
    return 1;
  }
  float_create(&factor);
  result = _lngamma_prim(x, &factor, &infinity, digits)
           && infinity == 0;
  if (result)
  {
    float_abs(&factor);
    _ln(&factor, digits + 1);
    result = float_sub(x, x, &factor, digits+1);
  }
  if (infinity != 0)
    float_seterror(FLOAT_ZERODIVIDE);
  float_free(&factor);
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
    if (infinity != 0)
      float_seterror(FLOAT_ZERODIVIDE);
    float_free(&tmp);
    if (!result)
      float_setnan(x);
    return result;
  }
  else
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
    return _gammagtminus20(x, digits);
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
  if (n >= 0)
    return _pochhammer_su(x, n, digits);
  return float_addi(x, x, n, digits)
         && _pochhammer_su(x, -n, digits)
         && float_reciprocal(x, digits);
}

static char
_pochhammer_g(
  floatnum x,
  floatnum n,
  int digits)
{
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
  {
    float_seterror(FLOAT_ZERODIVIDE);
    float_setnan(x);
  }
  if (result)
  {
    if (inf2 < 0)
      float_setzero(x);
    else
      result = float_div(&factor1, &factor1, &factor2, digits+1);
  }
  if (result && inf2 == 0)
    result = float_sub(x, &tmp, x, digits+1)
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
  floatnum n,
  int digits)
{
  /* pre: n is an integer */
  int ni;
  char result;

  if (float_iszero(n))
    return float_copy(x, &c1, EXACT);
  if (float_isinteger(x))
  {
    result = -1;
    float_neg(n);
    if (float_getsign(x) <= 0 && float_cmp(x, n) > 0)
    {
      /* x and x+n have opposite signs, meaning 0 is
         among the factors */
      float_setzero(x);
      result = 1;
    }
    else if (float_getsign(x) >= 0 && float_cmp(x, n) <= 0)
    {
      /* x and x+n have opposite signs, meaning at one point
      you have to divide by 0 */
      float_seterror(FLOAT_ZERODIVIDE);
      float_setnan(x);
      result = 0;
    }
    float_neg(n);
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
  floatnum n,
  int digits)
{
  if (float_isinteger(n))
    return _pochhammer_i(x, n, digits);
  return _pochhammer_g(x, n, digits);
}
