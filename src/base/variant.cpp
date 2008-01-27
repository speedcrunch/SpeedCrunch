/* variant.cpp: general value type used throughout SpeedCrunch*/
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

#include "variant.hxx"
#include "math/numdata.hxx"

Data::Constructors Data::constructors;

Data* Data::create(VariantType t)
{
  return constructors[t]();
}

Variant Data::operator+() const { return this; }
Variant Data::operator-() const { return NotImplemented; }
Variant Data::operator~() const { return NotImplemented; }
Variant Data::operator!() const { return NotImplemented; }
Variant Data::operator+(const Variant& other) const { return NotImplemented; }
Variant Data::operator-(const Variant& other) const { return NotImplemented; }
Variant Data::operator*(const Variant& other) const { return NotImplemented; }
Variant Data::operator/(const Variant& other) const { return NotImplemented; }
Variant Data::operator%(const Variant& other) const { return NotImplemented; }
Variant Data::operator&(const Variant& other) const { return NotImplemented; }
Variant Data::operator|(const Variant& other) const { return NotImplemented; }
Variant Data::operator^(const Variant& other) const { return NotImplemented; }
Variant Data::operator<<(const Variant& other) const { return NotImplemented; }
Variant Data::operator>>(const Variant& other) const { return NotImplemented; }
Variant Data::operator==(const Variant& other) const { return NotImplemented; }
Variant Data::operator>(const Variant& other) const { return NotImplemented; }
Variant Data::operator<(const Variant& other) const { return NotImplemented; }
Variant Data::operator!=(const Variant& other) const { return !(*this == other); }
Variant Data::operator>=(const Variant& other) const { return !(*this < other); }
Variant Data::operator<=(const Variant& other) const { return !(*this > other); }
Variant Data::swapadd(const Variant& other) const { return *this + other; }
Variant Data::swapsub(const Variant& other) const { return -(*this - other); }
Variant Data::swapmul(const Variant& other) const { return *this * other; }
Variant Data::swapdiv(const Variant& other) const { return NotImplemented; }
Variant Data::swapmod(const Variant& other) const { return NotImplemented; }
Variant Data::swapand(const Variant& other) const { return *this & other; }
Variant Data::swapor(const Variant& other) const { return *this | other; }
Variant Data::swapxor(const Variant& other) const { return *this ^ other; }
Variant Data::swapshr(const Variant& other) const { return NotImplemented; }
Variant Data::swapshl(const Variant& other) const { return NotImplemented; }
Variant Data::swapeq(const Variant& other) const { return *this == other; }
Variant Data::swapne(const Variant& other) const { return *this != other; }
Variant Data::swapge(const Variant& other) const { return *this < other; }
Variant Data::swaple(const Variant& other) const { return *this > other; }
Variant Data::swapgt(const Variant& other) const { return *this <= other; }
Variant Data::swapls(const Variant& other) const { return *this >= other; }

bool Variant::builtinType() const
{
  switch (type())
  {
    case vEmpty:
    case vBoolean:
    case vError: return true;
    default: return false;
  }
}

void Variant::clear()
{
  if (!builtinType())
    val->release();
  val = 0; // error = Success, boolvar = false
  m_type = vEmpty;
}

bool Variant::setType(Data* newval)
{
  if (!newval)
    return false;
  *this = newval->type();
  val = newval;
  return true;
}

void Variant::operator=(VariantType aType)
{
  clear();
  m_type = aType;
}

void Variant::operator=(Error code)
{
  *this = vError;
  error = code;
}

void Variant::operator=(bool val)
{
  *this = vBoolean;
  boolval = val;
}

void Variant::operator=(Data* other)
{
  if (!setType(other))
    clear();
}

void Variant::operator=(const Variant& other)
{
  switch(other.type())
  {
    case vEmpty: clear(); break;
    case vBoolean: *this = other.boolval; break;
    case vError: *this = other.error; break;
    default: *this = other.val->clone(); break;
  }
}

Variant::operator Error() const
{
  if (type() == vError)
    return error;
  return Success;
}

Variant::operator bool() const
{
  switch (type())
  {
    case vEmpty: return false;
    case vError: return error == Success;
    case vBoolean: return boolval;
    default: return (bool)(*val);
  }
}

Variant::operator const Data*() const
{
  if (builtinType())
    return 0;
  return val;
}

Data* Variant::convert(VariantType newtype)
{
  // pre: type == vBoolean || ! builtinType()
  Data* result;
  Data* dummy = Data::create(newtype);
  if (type() == vBoolean)
    result = dummy->retype(boolval);
  else
    result = dummy->retype(val);
  dummy-> release();
  return result;
}

Error Variant::checkError(const Variant& other, bool checkBool) const
{
  Error result = NotAnError;
  if (type() == vError)
  {
    result = error;
    if (result == Success)
      result = NoOperand;
  }
  else if (other.type() == vError)
  {
    result = other.error;
    if (result == Success)
      result = NoOperand;
  }
  else if (type() == vEmpty || other.type() == vEmpty)
    result = NoOperand;
  else if (checkBool && type() == vBoolean && other.type() == vBoolean)
    result = NotImplemented;
  return result;
}

Variant Variant::as(VariantType othertype)
{
  Variant result;
  result = *this;
  result.retype(othertype);
  return result;
}

bool Variant::retype(VariantType newtype)
{
  if (type() == newtype)
    return true;
  if (newtype > type())
    switch (newtype)
    {
      case vEmpty: clear(); return true;
      case vError: *this = Success; return true;
      default: return setType(convert(newtype));
    }
  // newtype < type()
  switch (newtype)
  {
    case vError: *this = Error(*this); return true;
    case vBoolean: *this = bool(*this); return true;
    default: return setType(val->retype(newtype));
  }
}

Variant Variant::operator+() const
{
  switch (type())
  {
    case vEmpty: return NoOperand;
    case vError: return *this;
    case vBoolean: return NotImplemented;
    default: return +*val;
  }
}

Variant Variant::operator-() const
{
  switch (type())
  {
    case vEmpty: return NoOperand;
    case vError: return *this;
    case vBoolean: return NotImplemented;
    default: return -*val;
  }
}

Variant Variant::operator~() const
{
  switch (type())
  {
    case vEmpty: return NoOperand;
    case vError: return *this;
    case vBoolean: return NotImplemented;
    default: return ~*val;
  }
}

Variant Variant::operator!() const
{
  switch (type())
  {
    case vEmpty: return NoOperand;
    case vError: return *this;
    case vBoolean: return !boolval;
    default: return !*val;
  }
}

Variant Variant::operator+(const Variant& other) const
{
  Error e = checkError(other);
  if (e != NotAnError)
    return e;
  if (type() >= other.type())
    return *val + other;
  return other.val->swapadd(*this);
}

Variant Variant::operator-(const Variant& other) const
{
  Error e = checkError(other);
  if (e != NotAnError)
    return e;
  if (type() >= other.type())
    return *val - other;
  return other.val->swapsub(*this);
}

Variant Variant::operator*(const Variant& other) const
{
  Error e = checkError(other);
  if (e != NotAnError)
    return e;
  if (type() >= other.type())
    return *val * other;
  return other.val->swapmul(*this);
}

Variant Variant::operator/(const Variant& other) const
{
  Error e = checkError(other);
  if (e != NotAnError)
    return e;
  if (type() >= other.type())
    return *val / other;
  return other.val->swapdiv(*this);
}

Variant Variant::operator%(const Variant& other) const
{
  Error e = checkError(other);
  if (e != NotAnError)
    return e;
  if (type() >= other.type())
    return *val % other;
  return other.val->swapmod(*this);
}

Variant Variant::operator&(const Variant& other) const
{
  Error e = checkError(other, false);
  if (e != NotAnError)
    return e;
  if (type() >= other.type())
  {
    if (type() == vBoolean)
      return boolval & other.boolval;
    return *val & other;
  }
  return other.val->swapand(*this);
}

Variant Variant::operator|(const Variant& other) const
{
  Error e = checkError(other, false);
  if (e != NotAnError)
    return e;
  if (type() >= other.type())
  {
    if (type() == vBoolean)
      return boolval | other.boolval;
    return *val | other;
  }
  return other.val->swapor(*this);
}

Variant Variant::operator^(const Variant& other) const
{
  Error e = checkError(other, false);
  if (e != NotAnError)
    return e;
  if (type() >= other.type())
  {
    if (type() == vBoolean)
      return boolval ^ other.boolval;
    return *val ^ other;
  }
  return other.val->swapxor(*this);
}

Variant Variant::operator>>(const Variant& other) const
{
  Error e = checkError(other);
  if (e != NotAnError)
    return e;
  if (type() >= other.type())
    return *val >> other;
  return other.val->swapshr(*this);
}

Variant Variant::operator<<(const Variant& other) const
{
  Error e = checkError(other);
  if (e != NotAnError)
    return e;
  if (type() >= other.type())
    return *val << other;
  return other.val->swapshl(*this);
}

Variant Variant::operator==(const Variant& other) const
{
  Error e = checkError(other, false);
  if (e != NotAnError)
    return e;
  if (type() >= other.type())
  {
    if (type() == vBoolean)
      return boolval == other.boolval;
    return *val == other;
  }
  return other.val->swapeq(*this);
}

Variant Variant::operator!=(const Variant& other) const
{
  Error e = checkError(other, false);
  if (e != NotAnError)
    return e;
  if (type() >= other.type())
  {
    if (type() == vBoolean)
      return boolval != other.boolval;
    return *val != other;
  }
  return other.val->swapne(*this);
}

Variant Variant::operator>=(const Variant& other) const
{
  Error e = checkError(other);
  if (e != NotAnError)
    return e;
  if (type() >= other.type())
    return *val >= other;
  return other.val->swapge(*this);
}

Variant Variant::operator<=(const Variant& other) const
{
  Error e = checkError(other);
  if (e != NotAnError)
    return e;
  if (type() >= other.type())
    return *val <= other;
  return other.val->swaple(*this);
}

Variant Variant::operator>(const Variant& other) const
{
  Error e = checkError(other, true);
  if (e != NotAnError)
    return e;
  if (type() >= other.type())
    return *val > other;
  return other.val->swapgt(*this);
}

Variant Variant::operator<(const Variant& other) const
{
  Error e = checkError(other, true);
  if (e != NotAnError)
    return e;
  if (type() >= other.type())
    return *val < other;
  return other.val->swapls(*this);
}

bool Variant::isZero()
{
  switch(type())
  {
    case vBoolean:
    case vError:
    case vEmpty: return false;
    default: return val->isZero();
  }
}

bool Variant::isNaN()
{
  switch(type())
  {
    case vBoolean:
    case vError:
    case vEmpty: return true;
    default: return val->isNaN();
  }
}

bool Variant::isInteger()
{
  switch(type())
  {
    case vBoolean:
    case vError:
    case vEmpty: return false;
    default: return val->isInteger();
  }
}

int Variant::precision(int newprec)
{
  return Floatnum::precision(newprec);
}

void Variant::operator=(int x)
{
  *this = Data::create(vReal);
  *static_cast<Floatnum*>(val) = x;
}

void Variant::operator=(const char* s)
{
  *this = Data::create(vReal);
  *static_cast<Floatnum*>(val) = s;
}

void Variant::operator=(floatnum x)
{
  *this = Data::create(vReal);
  *static_cast<Floatnum*>(val) = x;
}

void Variant::operator=(const LowLevelIO& io)
{
  *this = Data::create(vReal);
  *static_cast<Floatnum*>(val) = io;
}

Variant::LowLevelIO Variant::format(Format fmt, int scale,
                                    char base, char expbase) const
{
  switch (type())
  {
    case vReal:
      return static_cast<Real*>(val)->format(fmt, scale, base, expbase);
    default: ;
  }
  LowLevelIO io;
  io.baseSignificand = -1;
  io.baseScale = -1;
  return io;
}

Variant::operator cfloatnum() const
{
  switch (type())
  {
    case vReal:
      return (cfloatnum)static_cast<Real*>(val);
    default: return Floatnum::NaN();
  }
}
