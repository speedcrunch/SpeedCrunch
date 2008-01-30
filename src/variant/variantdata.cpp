/* variantbase.cpp: interface of variant type and derivates */
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

#include "variant/variantdata.hxx"
#include "variant/variant.hxx"

static const char* names[] =
{
  "BOOL",
  "LONGREAL"
  "ERROR",
};

const char* VariantIntf::VariantTypeName(VariantType t)
{
  // FIXME bound check
  return names[int(t)];
}

VariantIntf::VariantType VariantIntf::Name2VariantType(const QByteArray& b)
{
  int i = -1;
  // FIXME bound check
  for (; b != names[i]; );
  return VariantType(i);
}

signed char VariantIntf::cmpType(VariantType other) const
{
  if (type() == other)
    return 0;
  return int(type()) < int(other)? -1 : 1;
}

Variant VariantIntf::operator-() const
{ return NotImplemented; }
Variant VariantIntf::operator+() const
{ return NotImplemented; }
Variant VariantIntf::operator~() const
{ return NotImplemented; }
Variant VariantIntf::operator!() const
{ return NotImplemented; }
Variant VariantIntf::operator+(const Variant& other) const
{ return NotImplemented; }
Variant VariantIntf::operator-(const Variant& other) const
{ return NotImplemented; }
Variant VariantIntf::operator*(const Variant& other) const
{ return NotImplemented; }
Variant VariantIntf::operator/(const Variant& other) const
{ return NotImplemented; }
Variant VariantIntf::operator%(const Variant& other) const
{ return NotImplemented; }
Variant VariantIntf::idiv(const Variant& other) const
{ return NotImplemented; }
Variant VariantIntf::operator&(const Variant& other) const
{ return NotImplemented; }
Variant VariantIntf::operator|(const Variant& other) const
{ return NotImplemented; }
Variant VariantIntf::operator^(const Variant& other) const
{ return NotImplemented; }
Variant VariantIntf::operator==(const Variant& other) const
{ return NotImplemented; }
Variant VariantIntf::operator!=(const Variant& other) const
{ return !(*this == other); }
Variant VariantIntf::operator>(const Variant& other) const
{ return NotImplemented; }
Variant VariantIntf::operator>=(const Variant& other) const
{
  Variant result = *this > other;
  if (result.type() == vBoolean && !result)
    result = *this == other;
  return result;
}
Variant VariantIntf::operator<(const Variant& other) const
{ return !(*this >= other); }
Variant VariantIntf::operator<=(const Variant& other) const
{ return !(*this > other); }
Variant VariantIntf::raise(const Variant& other) const
{ return NotImplemented; }

Variant VariantIntf::embed() const
{ return BadCast; }


VariantData::Constructor constructors[int(VariantData::vError)];

void VariantData::registerConstructor(Constructor fct, VariantType t)
{
  constructors[t] = fct;
}

VariantData* VariantData::create(VariantType t)
{
  if (Variant::isBuiltinType(t))
    return 0;
  return constructors[int(t)]();
}

Variant VariantData::swapAdd(const Variant& other) const
{ return *this + other; }
Variant VariantData::swapSub(const Variant& other) const
{ return NotImplemented; }
Variant VariantData::swapMul(const Variant& other) const
{ return *this * other; }
Variant VariantData::swapDiv(const Variant& other) const
{ return NotImplemented; }
Variant VariantData::swapMod(const Variant& other) const
{ return NotImplemented; }
Variant VariantData::swapIdiv(const Variant& other) const
{ return NotImplemented; }
Variant VariantData::swapAnd(const Variant& other) const
{ return *this & other; }
Variant VariantData::swapOr(const Variant& other) const
{ return *this | other; }
Variant VariantData::swapXor(const Variant& other) const
{ return *this ^ other; }
Variant VariantData::swapEq(const Variant& other) const
{ return *this == other; }
Variant VariantData::swapNe(const Variant& other) const
{ return *this != other; }
Variant VariantData::swapGe(const Variant& other) const
{ return *this < other; }
Variant VariantData::swapGt(const Variant& other) const
{ return *this <= other; }
Variant VariantData::swapLe(const Variant& other) const
{ return *this > other; }
Variant VariantData::swapLs(const Variant& other) const
{ return *this >= other; }
Variant VariantData::swapRaise(const Variant& base) const
{ return NotImplemented; }
