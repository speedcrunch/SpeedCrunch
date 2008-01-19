/* numdata.cpp: number classes for use with Variant */
/*
    Copyright (C) 2008 Wolf Lammen.

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

#include "math/numdata.hxx"
#include "math/floathmath.h"
#include "math/floatcommon.h"
#include "math/floatconvert.h"
#include <QByteArray>

#define WORKING_PREC (DECPRECISION + 3)
#define EVAL_PREC (WORKING_PREC + 2)

class InitReal
{
  private:
    InitReal();
    static InitReal instance;
};

InitReal InitReal::instance;

InitReal::InitReal()
{
  Data::constructors[vReal] = Real::createReal;
  float_create(const_cast<floatnum>(Floatnum::NaN()));
}

floatstruct Floatnum::NaNval;

int Floatnum::precision(int newprec)
{
  int result = workprec - 3;
  if (newprec == 0 || newprec > DECPRECISION)
    newprec = DECPRECISION;
  if (newprec > 0)
  {
    workprec = newprec + 3;
    evalprec = newprec + 5;
  }
  return result;
}

void Floatnum::checkfullcancellation(cfloatnum op1, cfloatnum op2,
                                     floatnum r)
{
  int expr;

  if (float_getlength(op1) != 0
      && float_getlength(op2) != 0
      && float_getlength(r) != 0)
  {
    /* NaN or zero not involved in computation */
    expr = float_getexponent(r);
    if (float_getexponent(op1) - expr >= workprec - 1
        && float_getexponent(op2) - expr >= workprec - 1)
      float_setzero(r);
  }
}

char Floatnum::checkAdd(floatnum dest, cfloatnum s1, cfloatnum s2, int digits)
{
  if (float_add(dest, s1, s2, digits)
      && float_getsign(s1) + float_getsign(s2) == 0)
    checkfullcancellation(s1, s2, dest);
  return float_isnan(dest);
}

char Floatnum::checkSub(floatnum dest, cfloatnum s1, cfloatnum s2, int digits)
{
  if (float_sub(dest, s1, s2, digits)
      && float_getsign(s1) - float_getsign(s2) == 0)
    checkfullcancellation(s1, s2, dest);
  return float_isnan(dest);
}

static char modwrap(floatnum result, cfloatnum p1, cfloatnum p2, int digits)
{
  char ok;
  floatstruct tmp;
  float_create(&tmp);
  ok = float_divmod(&tmp, result, p1, p2, INTQUOT);
  float_free(&tmp);
  return ok;
}

int Floatnum::workprec = WORKING_PREC;
int Floatnum::evalprec = EVAL_PREC;

Variant Floatnum::call1ArgND(Float1ArgND func) const
{
  floatstruct result;
  float_create(&result);
  float_copy(&result, *this, evalprec);
  func(&result);
  return realresult(&result);
}

Variant Floatnum::call2Args(Variant x2, Float2Args func, bool swap) const
{
  if (isFloatnumType(x2.type()))
  {
    floatstruct result;
    float_create(&result);
    cfloatnum p1 = *this;
    cfloatnum p2 = asfloatnum(x2);
    (swap? func(&result, p1, p2, evalprec) : func(&result, p2, p1, evalprec))
      && float_round(&result, &result, workprec, TONEAREST);
    return realresult(&result);
  }
  return TypeMismatch;
}

Variant Floatnum::call2ArgsND(Variant x2, Float2ArgsND func, bool swap) const
{
  if (isFloatnumType(x2.type()))
  {
    floatstruct result;
    float_create(&result);
    cfloatnum p1 = *this;
    cfloatnum p2 = asfloatnum(x2);
    if (swap)
      func(&result, p1, p2);
    else
      func(&result, p2, p1);
    return realresult(&result);
  }
  return TypeMismatch;
}

bool Floatnum::isFloatnumType(VariantType t)
{
  switch(t)
  {
    case vReal: return true;
    default: return false;
  }
}

Variant Floatnum::boolresult(bool result) const
{
  Error e = float_isnan(&val)? NoOperand : float_geterror();
  if (e != Success)
    return e;
  return result;
}

Variant Floatnum::realresult(floatnum result) const
{
  Error e = float_geterror();
  if (e != Success)
  {
    float_free(result);
    return e;
  }
  Floatnum* r = createSameType(result);
  return r;
}

cfloatnum Floatnum::asfloatnum(const Variant& v)
{
  return &static_cast<const Floatnum*>((const Data*)v)->val;
}

Floatnum::Floatnum()
{
  refcount = 1;
  float_create(&val);
}

void Floatnum::operator=(floatnum x)
{
  float_move(&val, x);
}

void Floatnum::operator=(int x)
{
  float_setinteger(&val, x);
}

void Floatnum::operator=(const char* s)
{
  float_setscientific(&val, s, NULLTERMINATED);
}

Floatnum* Floatnum::createSameType(floatnum x) const
{
  Floatnum* result = static_cast<Floatnum*>(create(type()));
  if (x)
    *result = x;
  return result;
}

Data* Floatnum::clone()
{
  ++refcount;
  return this;
}

void Floatnum::release()
{
  if (--refcount == 0)
    delete this;
}

Floatnum::operator bool() const
{
  return !float_iszero(&val);
}

Data* Floatnum::retype(bool bv) const
{
  Floatnum* result = createSameType();
  *result = bv? 1 : 0;
  return result;
}


Variant Floatnum::operator==(const Variant& other) const
{
  VariantType t = other.type();
  if (isFloatnumType(t))
    return boolresult(float_cmp(&val, asfloatnum(other)) == 0);
  if (t == vBoolean)
    return boolresult(bool(*this) == bool(other));
  return TypeMismatch;
}

Variant Floatnum::operator>(const Variant& other) const
{
  if (isFloatnumType(other.type()))
    return boolresult(float_cmp(&val, asfloatnum(other)) > 0);
  return TypeMismatch;
}

Variant Floatnum::operator<(const Variant& other) const
{
  if (isFloatnumType(other.type()))
    return boolresult(float_cmp(&val, asfloatnum(other)) < 0);
  return TypeMismatch;
}

Variant Floatnum::operator-() const
{
  return call1ArgND(float_neg);
}

Variant Floatnum::operator~() const
{
  return call1ArgND(float_not);
}

Variant Floatnum::operator+(const Variant& other) const
{
  return call2Args(other, checkAdd);
}

Variant Floatnum::operator-(const Variant& other) const
{
  return call2Args(other, checkSub);
}

Variant Floatnum::operator*(const Variant& other) const
{
  return call2Args(other, float_mul);
}

Variant Floatnum::operator/(const Variant& other) const
{
  return call2Args(other, float_div);
}

Variant Floatnum::operator%(const Variant& other) const
{
  return call2Args(other, modwrap);
}

Variant Floatnum::operator&(const Variant& other) const
{
  return call2ArgsND(other, float_and);
}

Variant Floatnum::operator|(const Variant& other) const
{
  return call2ArgsND(other, float_or);
}

Variant Floatnum::operator^(const Variant& other) const
{
  return call2ArgsND(other, float_xor);
}

Variant Floatnum::operator>>(const Variant& other) const
{
  return call2ArgsND(other, float_shr);
}

Variant Floatnum::operator<<(const Variant& other) const
{
  return call2ArgsND(other, float_shl);
}

Variant Floatnum::swapsub(const Variant& other) const
{
  return call2Args(other, checkSub, true);
}

Variant Floatnum::swapdiv(const Variant& other) const
{
  return call2Args(other, float_div, true);
}

Variant Floatnum::swapmod(const Variant& other) const
{
  return call2Args(other, modwrap, true);
}

Variant Floatnum::swapshl(const Variant& other) const
{
  return call2ArgsND(other, float_shl, true);
}

Variant Floatnum::swapshr(const Variant& other) const
{
  return call2ArgsND(other, float_shr, true);
}

bool Floatnum::isZero()
{
  return float_iszero(&val);
}

bool Floatnum::isNaN()
{
  return float_isnan(&val);
}

bool Floatnum::isInteger()
{
  return float_isinteger(&val);
}

Variant::LowLevelIO Floatnum::format(Variant::Format fmt, int digits,
                                     char base, char expbase) const
{
  static Variant::Sign signs[4]
      = { Variant::Compl2, Variant::Minus, Variant::Plus, Variant::Plus };
  floatstruct tmp;
  t_otokens tokens;
  int sz;
  char intbuf[BINPRECISION+1];
  char fracbuf[BINPRECISION+1];
  char expbuf[BITS_IN_EXP+1];
  Variant::LowLevelIO result;

  result.baseSignificand = -1;
  float_create(&tmp);
  switch (base)
  {
    case 2:
      sz = BINPRECISION+1;
      break;
    case 8:
      sz = OCTPRECISION+1;
      break;
    case 10:
      sz = DECPRECISION+1;
      break;
    case 16:
      sz = HEXPRECISION+1;
      break;
  }
  tokens.intpart.sz = sz;
  tokens.intpart.buf = intbuf;
  tokens.fracpart.sz = sz;
  tokens.fracpart.buf = fracbuf;
  tokens.exp.sz = sizeof(expbuf);
  tokens.exp.buf = expbuf;
  float_copy(&tmp, &val, DECPRECISION + 2);
  if (float_out(&tokens, &tmp, digits, base, expbase, char(fmt)))
  {
    result.baseSignificand = base;
    result.baseScale = expbase;
    result.signSignificand = signs[tokens.sign + 2];
    result.signScale = signs[tokens.expsign + 2];
    result.intpart = tokens.intpart.buf;
    result.fracpart = tokens.fracpart.buf;
    result.scale = tokens.exp.buf;
  }
  float_free(&tmp);
  return result;
}

void Floatnum::operator=(const Variant::LowLevelIO& io)
{
  static signed char signs[] 
      = { IO_SIGN_PLUS, IO_SIGN_MINUS, IO_SIGN_COMPLEMENT };
  t_itokens tokens;

  QByteArray intpart, fracpart, scale;
  tokens.sign = signs[int(io.signSignificand)];
  tokens.expsign = signs[int(io.signScale)];
  tokens.base = io.baseSignificand;
  tokens.expbase = io.baseScale;
  tokens.intpart = 0;
  if (!io.intpart.isEmpty())
  {
    intpart = io.intpart.toAscii();
    tokens.intpart = intpart;
  }
  if (!io.fracpart.isEmpty())
  {
    fracpart = io.fracpart.toAscii();
    tokens.fracpart = fracpart;
  }
  if (!io.scale.isEmpty())
  {
    scale = io.scale.toAscii();
    tokens.exp = scale;
  }
  tokens.maxdigits = DECPRECISION + 5;
  Error e = float_in(&val, &tokens);
  if (e != Success)
    *this = e;
}
