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

void Variant::initClass()
{
  registerConstructor(create, vError);
  registerConstructor(create, vBoolean);
}

VariantData* Variant::create()
{
  return new Variant;
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

void Variant::clear()
{
  if (!isBuiltinType(type()))
    val->release();
  m_type = vError;
  error = NoOperand;
}

void Variant::operator=(VariantType t)
{
  clear();
  m_type = t;
  switch (t)
  {
    case vBoolean: boolval = false; break;
    case vError: break;
    default: val = VariantData::create(t); break;
  }
}

void Variant::operator=(Error e)
{
  *this = vError;
  error = e;
}

void Variant::operator=(bool b)
{
  *this = vBoolean;
  boolval = b;
}

void Variant::operator=(const Variant& other)
{
  *this = other.type();
  switch (type())
  {
    case vBoolean: boolval = other.boolval; break;
    case vError: error = other.error; break;
    default: val = other.val->clone();
  }
}

Variant::operator bool() const
{
  switch (type())
  {
    case vBoolean: return boolval;
    case vError: return error == Success;
    default: return val != 0;
  }
}

Variant::operator Error() const
{
  switch (type())
  {
    case vBoolean: return Success;
    case vError: return error;
    default: return val == 0? NoOperand: Success;
  }
}

void Variant::operator=(VariantData* newval)
{
  if (newval == 0)
    *this = NoOperand;
  else
  {
    *this = newval->type();
    val = newval;
  }
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

Variant Variant::operator-(const Variant& other) const
{
  if (cmpType(other.type()) >= 0)
    return overloadfct(&VariantData::operator-, other);
  return overloadfct(&VariantData::swapSub, other);
}

Variant Variant::operator+(const Variant& other) const
{
  if (cmpType(other.type()) >= 0)
    return overloadfct(&VariantData::operator+, other);
  return overloadfct(&VariantData::swapAdd, other);
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
    *this = vLongReal;
    static_cast<LongReal*>(val)->move(x);
  }
}
