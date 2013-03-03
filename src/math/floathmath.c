/* floathmath.c: higher mathematical functions, based on floatnum. */
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

#include "floathmath.h"
#include "floatconst.h"
#include "floatcommon.h"
#include "floatlog.h"
#include "floatexp.h"
#include "floattrig.h"
#include "floatpower.h"
#include "floatipower.h"
#include "floatgamma.h"
#include "floaterf.h"
#include "floatlogic.h"

static char
_cvtlogic(
  t_longint* lx,
  cfloatnum x)
{
  if (float_isnan(x))
  {
    float_seterror(NoOperand);
    return 0;
  }
  if (_floatnum2logic(lx, x))
    return 1;
  float_seterror(OutOfLogicRange);
  return 0;
}

char
float_lnxplus1(
  floatnum x,
  int digits)
{
  if (!chckmathparam(x, digits))
    return 0;
  if (float_getsign(x) < 0 && float_getexponent(x) >= 0)
    return _seterror(x, OutOfDomain);
  _lnxplus1(x, digits);
  return 1;
}

char
float_ln(floatnum x, int digits)
{
  if (!chckmathparam(x, digits))
    return 0;
  if (float_getsign(x) <= 0)
    return _seterror(x, OutOfDomain);
  _ln(x, digits);
  return 1;
}

char
float_artanh(
  floatnum x,
  int digits)
{
  if (!chckmathparam(x, digits))
    return 0;
  if (float_getexponent(x) >= 0)
    return _seterror(x, OutOfDomain);
  _artanh(x, digits);
  return 1;
}

char
float_artanhxplus1(
  floatnum x,
  int digits)
{
  if (!chckmathparam(x, digits))
    return 0;
  if (float_getsign(x) >= 0 || float_abscmp(x, &c2) >= 0)
    return _seterror(x, OutOfDomain);
  if (float_cmp(x, &c1Div2) < 0)
  {
    float_neg(x);
    _artanh1minusx(x, digits);
  }
  else
  {
    float_sub(x, &c1, x, digits+1);
    _artanh(x, digits);
  }
  return 1;
}

char
float_arsinh(
  floatnum x,
  int digits)
{
  if (!chckmathparam(x, digits))
    return 0;
  _arsinh(x, digits);
  return 1;
}

char
float_arcoshxplus1(
  floatnum x,
  int digits)
{
  if (!chckmathparam(x, digits))
    return 0;
  if (float_getsign(x) < 0)
    return _seterror(x, OutOfDomain);
  _arcoshxplus1(x, digits);
  return 1;
}

char
float_arcosh(
  floatnum x,
  int digits)
{
  if (!chckmathparam(x, digits))
    return 0;
  float_sub(x, x, &c1, digits+1);
  return float_arcoshxplus1(x, digits);
}

char
float_log(
  floatnum x,
  int digits)
{
  floatstruct tmp;
  int expx;

  if (!chckmathparam(x, digits))
    return 0;
  if (float_getsign(x) <= 0)
    return _seterror(x, OutOfDomain);
  float_create(&tmp);
  expx = float_getexponent(x);
  float_setexponent(x, 0);
  _ln(x, digits);
  float_div(x, x, &cLn10, digits);
  float_setinteger(&tmp, expx);
  float_add(x, x, &tmp, digits);
  float_free(&tmp);
  return 1;
}

char
float_lg(
  floatnum x,
  int digits)
{
  if (!chckmathparam(x, digits))
    return 0;
  if (float_getsign(x) <= 0)
    return _seterror(x, OutOfDomain);
  _ln(x, digits);
  float_div(x, x, &cLn2, digits);
  return 1;
}

char
float_exp(
  floatnum x,
  int digits)
{
  signed char sgn;

  if (!chckmathparam(x, digits))
    return 0;
  sgn = float_getsign(x);
  if (_exp(x, digits))
    return 1;
  if (sgn < 0)
    return _seterror(x, Underflow);
  return _seterror(x, Overflow);
}

char
float_expminus1(
  floatnum x,
  int digits)
{
  if (!chckmathparam(x, digits))
    return 0;
  if (_expminus1(x, digits))
    return 1;
  return _seterror(x, Overflow);
}

char
float_cosh(
  floatnum x,
  int digits)
{
  int expx;

  if (!chckmathparam(x, digits))
    return 0;
  expx = float_getexponent(x);
  if (2*expx+2 <= -digits || !_coshminus1(x, digits+2*expx))
  {
    if (expx > 0)
      return _seterror(x, Overflow);
    float_setzero(x);
  }
  return float_add(x, x, &c1, digits);
}

char
float_coshminus1(
  floatnum x,
  int digits)
{
  int expx;

  if (!chckmathparam(x, digits))
    return 0;
  expx = float_getexponent(x);
  if (_coshminus1(x, digits))
    return 1;
  if (expx < 0)
    return _seterror(x, Underflow);
  return _seterror(x, Overflow);
}

char
float_sinh(
  floatnum x,
  int digits)
{
  if (!chckmathparam(x, digits))
    return 0;
  if (_sinh(x, digits))
    return 1;
  return _seterror(x, Overflow);
}

char
float_tanh(
  floatnum x,
  int digits)
{
  signed char sgn;

  if (!chckmathparam(x, digits))
    return 0;
  sgn = float_getsign(x);
  float_abs(x);
  if (float_cmp(x, &c1Div2) >= 0)
    _tanhgt0_5(x, digits);
  else
    _tanhlt0_5(x, digits);
  float_setsign(x, sgn);
  return 1;
}

char
float_tanhminus1(
  floatnum x,
  int digits)
{
  if (!chckmathparam(x, digits))
    return 0;
  if (float_cmp(x, &c1Div2) >= 0)
    return _tanhminus1gt0(x, digits)? 1 : _seterror(x, Underflow);
  if (!float_iszero(x))
  {
    if (float_abscmp(x, &c1Div2) <= 0)
      _tanhlt0_5(x, digits);
    else
    {
      float_setsign(x, 1);
      _tanhgt0_5(x, digits);
      float_setsign(x, -1);
    }
  }
  return float_sub(x, x, &c1, digits);
}

char
float_arctan(
  floatnum x,
  int digits)
{
  if (!chckmathparam(x, digits))
    return 0;
  _arctan(x, digits);
  return 1;
}

char
float_arcsin(
  floatnum x,
  int digits)
{
  if (!chckmathparam(x, digits))
    return 0;
  if (float_abscmp(x, &c1) > 0)
    return _seterror(x, OutOfDomain);
  _arcsin(x, digits);
  return 1;
}

char
float_arccos(
  floatnum x,
  int digits)
{
  if (!chckmathparam(x, digits))
    return 0;
  if (float_abscmp(x, &c1) > 0)
    return _seterror(x, OutOfDomain);
  _arccos(x, digits);
  return 1;
}

char
float_arccosxplus1(
  floatnum x,
  int digits)
{
  if (!chckmathparam(x, digits))
    return 0;
  if (float_getsign(x) > 0 || float_abscmp(x, &c2) > 0)
    return _seterror(x, OutOfDomain);
  _arccosxplus1(x, digits);
  return 1;
}

char
float_cos(
  floatnum x,
  int digits)
{
  if (!chckmathparam(x, digits))
    return 0;
  if (float_getexponent(x) >= DECPRECISION - 1 || !_trigreduce(x, digits))
    return _seterror(x, EvalUnstable);
  _cos(x, digits);
  return 1;
}

char
float_cosminus1(
  floatnum x,
  int digits)
{
  if (!chckmathparam(x, digits))
    return 0;
  if (!_trigreduce(x, digits))
    return _seterror(x, EvalUnstable);
  return _cosminus1(x, digits)? 1 : _seterror(x, Underflow);
}

char
float_sin(
  floatnum x,
  int digits)
{
  if (!chckmathparam(x, digits))
    return 0;
  if (float_getexponent(x) >= DECPRECISION - 1 || !_trigreduce(x, digits))
    return _seterror(x, EvalUnstable);
  _sin(x, digits);
  return 1;
}

char
float_tan(
  floatnum x,
  int digits)
{
  if (!chckmathparam(x, digits))
    return 0;
  return float_getexponent(x) >= DECPRECISION - 1
         || !_trigreduce(x, digits)
         || !_tan(x, digits)? _seterror(x, EvalUnstable) : 1;
}

char
float_raisei(
  floatnum power,
  cfloatnum base,
  int exponent,
  int digits)
{
  if (digits <= 0 || digits > maxdigits)
    return _seterror(power, InvalidPrecision);
  if (float_isnan(base))
    return _seterror(power, NoOperand);
  if (float_iszero(base))
  {
    if (exponent == 0)
      return _seterror(power, OutOfDomain);
    if (exponent < 0)
      return _seterror(power, ZeroDivide);
    return _setzero(power);
  }
  digits += 14;
  if (digits > maxdigits)
    digits = maxdigits;
  float_copy(power, base, digits);
  if (!_raisei(power, exponent, digits)
      || !float_isvalidexp(float_getexponent(power)))
  {
    if (float_getexponent(base) < 0)
      return _seterror(power, Underflow);
    return _seterror(power, Overflow);
  }
  return 1;
}

char
float_raise(
  floatnum power,
  cfloatnum base,
  cfloatnum exponent,
  int digits)
{
  signed char sgn;

  if (float_isnan(exponent) || float_isnan(base))
    return _seterror(power, NoOperand);
  if (digits <= 0 || digits > MATHPRECISION)
    return _seterror(power, InvalidPrecision);
  if (float_iszero(base))
  {
    switch(float_getsign(exponent))
    {
    case 0:
      return _seterror(power, OutOfDomain);
    case -1:
      return _seterror(power, ZeroDivide);
    }
    return _setzero(power);
  }
  sgn = float_getsign(base);
  if (sgn < 0)
  {
    if (!float_isinteger(exponent))
      return _seterror(power, OutOfDomain);
    if ((float_getdigit(exponent, float_getexponent(exponent)) & 1) == 0)
      sgn = 1;
  }
  float_copy(power, base, digits+1);
  float_abs(power);
  if (!_raise(power, exponent, digits))
  {
    float_seterror(Overflow);
    if (float_getexponent(base) * float_getsign(exponent) < 0)
      float_seterror(Underflow);
    return _setnan(power);
  }
  float_setsign(power, sgn);
  return 1;
}

char
float_power10(
  floatnum x,
  int digits)
{
  signed char sign;

  if (!chckmathparam(x, digits))
    return 0;
  sign = float_getsign(x);
  if (_power10(x, digits))
    return 1;
  return sign > 0? _seterror(x, Overflow) : _seterror(x, Underflow);
}

char
float_gamma(
  floatnum x,
  int digits)
{
  signed char sign;
  char result;

  if (!chckmathparam(x, digits))
    return 0;
  sign = float_getsign(x);
  if (float_isinteger(x))
  {
    if (sign <= 0)
      return _seterror(x, ZeroDivide);
    result = _gammaint(x, digits);
  }
  else if (float_getlength(x) - float_getexponent(x) == 2
           && float_getdigit(x, float_getlength(x) - 1) == 5)
    result = _gamma0_5(x, digits);
  else
    result = _gamma(x, digits);
  if (!result)
  {
    if (sign < 0)
      float_seterror(Underflow);
    else
      float_seterror(Overflow);
    float_setnan(x);
  }
  return result;
}

char
float_lngamma(
  floatnum x,
  int digits)
{
  if (!x)
    return _seterror(x, OutOfDomain);
  return chckmathparam(x, digits) && _lngamma(x, digits)?
          1 : _setnan(x);
}

char
float_factorial(
  floatnum x,
  int digits)
{
  if (!float_isnan(x))
    float_add(x, x, &c1, digits);
  return float_gamma(x, digits);
}

char
float_pochhammer(
  floatnum x,
  cfloatnum delta,
  int digits)
{
  if (!chckmathparam(x, digits))
    return 0;
  return float_isnan(delta)?
         _seterror(x, NoOperand)
         : _pochhammer(x, delta, digits);
}

char
float_erf(floatnum x, int digits)
{
  return chckmathparam(x, digits)? _erf(x, digits) : 0;
}

char
float_erfc(floatnum x, int digits)
{
  return chckmathparam(x, digits)? _erfc(x, digits) : 0;
}

char
float_not(
  floatnum x)
{
  t_longint lx;

  if(!_cvtlogic(&lx, x))
    return _setnan(x);
  _not(&lx);
  _logic2floatnum(x, &lx);
  return 1;
}

char
float_and(
  floatnum dest,
  cfloatnum x,
  cfloatnum y)
{
  t_longint lx, ly;

  if(!_cvtlogic(&lx, x) || !_cvtlogic(&ly, y))
    return _setnan(dest);
  _and(&lx, &ly);
  _logic2floatnum(dest, &lx);
  return 1;
}

char
float_or(
  floatnum dest,
  cfloatnum x,
  cfloatnum y)
{
  t_longint lx, ly;

  if(!_cvtlogic(&lx, x) || !_cvtlogic(&ly, y))
    return _setnan(dest);
  _or(&lx, &ly);
  _logic2floatnum(dest, &lx);
  return 1;
}

char
float_xor(
  floatnum dest,
  cfloatnum x,
  cfloatnum y)
{
  t_longint lx, ly;

  if(!_cvtlogic(&lx, x) || !_cvtlogic(&ly, y))
    return _setnan(dest);
  _xor(&lx, &ly);
  _logic2floatnum(dest, &lx);
  return 1;
}

char
_doshift(
  floatnum dest,
  cfloatnum x,
  cfloatnum shift,
  char right)
{
  int ishift;
  t_longint lx;

  if (float_isnan(shift))
    return _seterror(dest, NoOperand);
  if (!float_isinteger(shift))
    return _seterror(dest, OutOfDomain);
  if(!_cvtlogic(&lx, x))
    return 0;
  if (float_iszero(shift))
  {
    float_copy(dest, x, EXACT);
    return 1;
  }
  ishift = float_asinteger(shift);
  if (ishift == 0)
    ishift = (3*LOGICRANGE) * float_getsign(shift);
  if (!right)
    ishift = -ishift;
  if (ishift > 0)
    _shr(&lx, ishift);
  else
    _shl(&lx, -ishift);
  _logic2floatnum(dest, &lx);
  return 1;
}

char
float_shr(
  floatnum dest,
  cfloatnum x,
  cfloatnum shift)
{
  return _doshift(dest, x, shift, 1);
}

char
float_shl(
  floatnum dest,
  cfloatnum x,
  cfloatnum shift)
{
  return _doshift(dest, x, shift, 0);
}
