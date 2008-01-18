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

#include "base/variant.hxx"

Data* Data::create(VariantType)
{
  return 0;
}

Variant Data::operator+() const { return NotImplemented; }
Variant Data::operator-() const { return NotImplemented; }
Variant Data::operator~() const { return NotImplemented; }
Variant Data::operator!() const { return NotImplemented; }
Variant Data::operator+(const Variant other) const { return NotImplemented; }
Variant Data::operator-(const Variant other) const { return NotImplemented; }
Variant Data::operator*(const Variant other) const { return NotImplemented; }
Variant Data::operator/(const Variant other) const { return NotImplemented; }
Variant Data::operator%(const Variant other) const { return NotImplemented; }
Variant Data::operator&(const Variant other) const { return NotImplemented; }
Variant Data::operator|(const Variant other) const { return NotImplemented; }
Variant Data::operator^(const Variant other) const { return NotImplemented; }
Variant Data::operator==(const Variant other) const { return NotImplemented; }
Variant Data::operator!=(const Variant other) const { return NotImplemented; }
Variant Data::operator>=(const Variant other) const { return NotImplemented; }
Variant Data::operator<=(const Variant other) const { return NotImplemented; }
Variant Data::operator>(const Variant other) const { return NotImplemented; }
Variant Data::operator<(const Variant other) const { return NotImplemented; }

void Variant::operator=(VariantType aType)
{
  clear();
  switch (aType)
  {
    case vError: error = Success; break;
    case vBoolean: boolval = false; break;
    default: val = 0;
  }
  m_type = aType;
}

void Variant::clear()
{
  switch (type())
  {
    case vBoolean:
    case vEmpty:
    case vError: break;
    default: 
      val->release();
      val = 0;
  }
  m_type = vEmpty;
}

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

void Variant::operator=(const Variant& other)
{
  if (type() != other.type())
    *this = other.type();
  switch(type())
  {
    case vEmpty: break;
    case vBoolean: boolval = other.boolval;
    case vError: error = other.error; break;
    default: val = other.val->clone(); break;
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

bool Variant::setType(VariantType newtype, Data* newval)
{
  if (!newval)
    return  false;
  *this = newtype;
  val = newval;
  return true;
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

bool Variant::retype(VariantType newtype)
{
  if (type() == newtype)
    return true;
  switch (newtype)
  {
    case vEmpty: clear(); break;
    case vError: *this = Error(*this); break;
    case vBoolean: *this = bool(*this); break;
    default:
      switch (type())
      {
        case vEmpty:
        case vError: return false;
        default:
          if (setType(newtype, val->retype(newtype)))
            return true; // fall through
        case vBoolean: return setType(newtype, convert(newtype));
      }
  }
  return true;
}

Variant Variant::operator+() const
{
  switch (type())
  {
    case vEmpty:
    case vError: return *this;
    case vBoolean: return NotImplemented;
    default: return +*val;
  }
}

Variant Variant::operator-() const
{
  switch (type())
  {
    case vEmpty:
    case vError: return *this;
    case vBoolean: return NotImplemented;
    default: return -*val;
  }
}

Variant Variant::operator~() const
{
  switch (type())
  {
    case vEmpty:
    case vError: return *this;
    case vBoolean: return NotImplemented;
    default: return ~*val;
  }
}

Variant Variant::operator!() const
{
  switch (type())
  {
    case vEmpty:
    case vError: return *this;
    case vBoolean: return !boolval;
    default: return !*val;
  }
}

Variant Variant::operator+(const Variant& other) const
{
  if (type() == vEmpty || other.type() == vEmpty)
    return NaNOperand;
  if (type() == vError)
    return *this;
  if (other.type() == vError)
    return other;
  switch (type())
  {
    case vEmpty:
    case vError: return *this;
    case vBoolean: return !boolval;
    default: return !*val;
  }
}
