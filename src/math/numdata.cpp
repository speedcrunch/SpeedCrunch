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

#define WORKING_PREC (DECPRECISION + 3)
#define EVAL_PREC (WORKING_PREC + 2)

/*class InitFloatnum
{
  private:
    InitFloatnum() { Data::constructors[vReal] = Floatnum::createReal; };
    static InitFloatnum instance;
};

InitFloatnum InitFloatnum::instance;
*/

void static checkfullcancellation( cfloatnum op1, cfloatnum op2,
                                   floatnum r )
{
  int expr;

  if (float_getlength(op1) != 0
      && float_getlength(op2) != 0
      && float_getlength(r) != 0)
  {
    /* NaN or zero not involved in computation */
    expr = float_getexponent(r);
    if (float_getexponent(op1) - expr >= WORKING_PREC - 1
        || float_getexponent(op1) - expr >= WORKING_PREC - 1)
      float_setzero(r);
  }
}

static char checkAdd(floatnum dest, cfloatnum s1, cfloatnum s2, int digits)
{
  if (float_add(dest, s1, s2, digits)
      && float_getsign(s1) + float_getsign(s2) == 0)
    checkfullcancellation(s1, s2, dest);
  return float_isnan(dest);
}

static char checkSub(floatnum dest, cfloatnum s1, cfloatnum s2, int digits)
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

Variant Floatnum::call1ArgND(Float1ArgND func) const
{
  floatstruct result;
  float_create(&result);
  float_copy(&result, *this, EVAL_PREC);
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
    if (swap)
    {
      cfloatnum tmp = p1;
      p1 = p2;
      p2 = tmp;
    }
    func(&result, p1, p2, EVAL_PREC)
      && float_round(&result, &result, WORKING_PREC, TONEAREST);
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
    {
      cfloatnum tmp = p1;
      p1 = p2;
      p2 = tmp;
    }
    func(&result, p1, p2);
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
