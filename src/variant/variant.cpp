/* variant.cpp: variant class */
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

#include "variant/variant.hxx"

const char cFalse[] = "false";
const char cTrue[] = "true";

void Variant::initClass()
{
  VariantData::registerConstructor(0, vError);
  VariantData::registerConstructor(0, vBoolean);
}

bool Variant::isBuiltinType(VariantType t)
{
  switch(t)
  {
    case vError:
    case vBoolean: return true;
    default: return false;
  }
}

void Variant::retype(VariantType t)
{
  if (!isBuiltinType(type()))
    val->release();
  m_type = t;
  val = 0;
}

void Variant::clear()
{
  retype(vError);
  error = NoOperand;
}

void Variant::defaultType(VariantType t)
{
  retype(t);
  switch (t)
  {
    case vBoolean: boolval = false; break;
    case vError: error = NoOperand; break;
    default: val = VariantData::create(t); break;
  }
}

void Variant::operator=(Error e)
{
  retype(vError);
  error = e;
}

void Variant::operator=(bool b)
{
  retype(vBoolean);
  boolval = b;
}

void Variant::operator=(VariantData* newval)
{
  if (newval == 0)
    clear();
  else
  {
    retype(newval->type());
    val = newval;
  }
}

void Variant::operator=(const Variant& other)
{
  switch (other.type())
  {
    case vBoolean: *this = other.boolval; break;
    case vError: *this = other.error; break;
    default: *this = other.val->clone();
  }
}

bool Variant::assign(VariantType t, const char* s)
{
  defaultType(t);
  bool ok = true;
  switch (t)
  {
    case vBoolean:
      if (qstrcmp(s, cTrue) == 0)
        boolval = true;
      else if (qstrcmp(s, cFalse) == 0)
        boolval = false;
      else ok = false;
      break;
    case vError:
      error = Error(QByteArray(s).toInt(&ok));
      break;
    default:
      ok = val->assign(s);
      break;
  }
  if (!ok)
    *this = BadLiteral;
  return ok;
}

bool Variant::assign(const QByteArray& typeValue)
{
  VariantType t = name2VariantType(typeValue);
  if (t > vError)
    return false;
  return assign(t, typeValue.data() + qstrlen(variantTypeName(t)));
}

Variant::operator QByteArray() const
{
  QByteArray result;
  switch (type())
  {
    case vBoolean:
      if (boolval)
        result = cTrue;
      else
        result = cFalse;
      break;
    case vError:
      result.setNum(int(error));
      break;
    default:
      result = *val;
      break;
  }
  return variantTypeName(type()) + result;
}

Variant::operator bool() const
{
  switch (type())
  {
    case vBoolean: return boolval;
    case vError: return error == Success;
    default: return Error(*val) == Success;
  }
}

Variant::operator Error() const
{
  switch (type())
  {
    case vBoolean: return Success;
    case vError: return error;
    default: return val == 0? NoOperand: Error(*val);
  }
}
Variant::operator const VariantData*() const
{
  return isBuiltinType(type())? 0 : val;
}

Variant Variant::embed() const
{
  if (isBuiltinType(type()))
    return BadCast;
  return val->embed();
}

Variant Variant::overloadfct(Method1 fct) const
{
  switch (type())
  {
    case vError: return *this;
    case vBoolean: return TypeMismatch;
    default: ;
  }
  Variant v = (val->*fct)();
  if (Error(v) == NotImplemented)
  {
    v = embed();
    if (Error(v) == BadCast)
      return TypeMismatch;
    return v.overloadfct(fct);
  }
  return v;
}

Variant Variant::overloadfct(Method2 fct, const Variant& other) const
{
  switch (type())
  {
    case vError: return *this;
    case vBoolean: return TypeMismatch;
    default: ;
  }
  Variant v = (val->*fct)(other);
  while (Error(v) == TypeMismatch)
  {
    v = other.embed();
    if (Error(v) == BadCast || cmpType(v.type()) < 0)
    {
      v = NotImplemented;
      break;
    }
    v = (val->*fct)(v);
  }
  if (Error(v) == NotImplemented)
  {
    v = embed();
    if (Error(v) == BadCast)
      return TypeMismatch;
    return v.overloadfct(fct, other);
  }
  return v;
}

Variant Variant::operator+() const
{
  return overloadfct(&VariantData::operator+);
}

Variant Variant::operator-() const
{
  return overloadfct(&VariantData::operator-);
}

Variant Variant::operator~() const
{
  return overloadfct(&VariantData::operator~);
}

Variant Variant::operator!() const
{
  if (type() == vBoolean)
    return !boolval;
  return overloadfct(&VariantData::operator!);
}

Variant Variant::call2(Method2 normal, Method2 swapped,
                       const Variant& other) const
{
  if (cmpType(other.type()) >= 0)
    return overloadfct(normal, other);
  return other.overloadfct(swapped, *this);
}

Variant Variant::operator+(const Variant& other) const
{
  return call2(&VariantData::operator+, &VariantData::swapAdd, other);
}

Variant Variant::operator-(const Variant& other) const
{
  return call2(&VariantData::operator-, &VariantData::swapSub, other);
}

Variant Variant::operator*(const Variant& other) const
{
  return call2(&VariantData::operator*, &VariantData::swapMul, other);
}

Variant Variant::operator/(const Variant& other) const
{
  return call2(&VariantData::operator/, &VariantData::swapDiv, other);
}

Variant Variant::operator%(const Variant& other) const
{
  return call2(&VariantData::operator%, &VariantData::swapMod, other);
}

Variant Variant::idiv(const Variant& other) const
{
  return call2(&VariantData::idiv, &VariantData::swapIdiv, other);
}

Variant Variant::operator&(const Variant& other) const
{
  if (type() == vBoolean && other.type() == vBoolean)
    return bool(*this) && bool(other);
  return call2(&VariantData::operator&, &VariantData::swapAnd, other);
}

Variant Variant::operator|(const Variant& other) const
{
  if (type() == vBoolean && other.type() == vBoolean)
    return bool(*this) || bool(other);
  return call2(&VariantData::operator|, &VariantData::swapOr, other);
}

Variant Variant::operator^(const Variant& other) const
{
  if (type() == vBoolean && other.type() == vBoolean)
    return bool(*this) ^ bool(other);
  return call2(&VariantData::operator^, &VariantData::swapXor, other);
}

Variant Variant::operator==(const Variant& other) const
{
  if (type() == vBoolean && other.type() == vBoolean)
    return bool(*this) == bool(other);
  return call2(&VariantData::operator==, &VariantData::swapEq, other);
}

Variant Variant::operator!=(const Variant& other) const
{
  if (type() == vBoolean && other.type() == vBoolean)
    return bool(*this) != bool(other);
  return call2(&VariantData::operator!=, &VariantData::swapNe, other);
}

Variant Variant::operator>(const Variant& other) const
{
  return call2(&VariantData::operator>, &VariantData::swapGt, other);
}

Variant Variant::operator>=(const Variant& other) const
{
  return call2(&VariantData::operator>=, &VariantData::swapGe, other);
}

Variant Variant::operator<(const Variant& other) const
{
  return call2(&VariantData::operator<, &VariantData::swapLs, other);
}

Variant Variant::operator<=(const Variant& other) const
{
  return call2(&VariantData::operator<=, &VariantData::swapLe, other);
}

Variant Variant::raise(const Variant& other) const
{
  return call2(&VariantData::raise, &VariantData::swapRaise, other);
}

Variant::operator cfloatnum() const
{
  if (type() == vLongReal)
    return (cfloatnum)(*static_cast<LongReal*>(val));
  return LongReal::NaN();
}

void Variant::move(floatnum x, Error e)
{
  if (e != Success)
    *this = e;
  else
  {
    defaultType(vLongReal);
    static_cast<LongReal*>(val)->move(x);
  }
}
