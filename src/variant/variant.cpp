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

typedef VariantIntf::VariantType VariantType;

const char* VariantIntf::nBool = "Boolean";
const char* VariantIntf::nError = "Error";
VariantType VariantBase::vtBool;
VariantType VariantBase::vtError;

void VariantBase::initClass()
{
  vtBool = registerType(builtInConstructor, nBool);
  vtError = registerType(builtInConstructor, nError);
}

void Variant::initClass()
{
  VariantIntf::initClass();
  VariantBase::initClass();
}

VariantBase::operator Error() const
{
  return isError()? error : Success;
}

VariantBase::operator bool() const
{
  return isBool()? boolval : (Error)(*this) == Success;
}

VariantBase::operator const VariantData*() const
{
  return isBuiltIn()? 0 : val;
}

void VariantBase::setup()
{
  m_type = btError;
  error = NoOperand;
}

VariantType VariantBase::type() const
{
  if (m_type == btBool)
    return vtBool;
  return vtError;
}

bool VariantBase::operator = (bool newval)
{
  teardown();
  m_type = btBool;
  boolval = newval;
  return newval;
}

Error VariantBase::operator = (Error newval)
{
  teardown();
  m_type = btError;
  error = newval;
  return newval;
}

void VariantBase::operator = (VariantData* newval)
{
  m_type = btExtern;
  val = newval;
}

void VariantBase::operator = (const VariantBase& other)
{
  if (other.isBool())
    *this = (bool)other;
  else
    *this = (Error)other;
}

QByteArray VariantBase::xmlWrite() const
{
  QByteArray result;
  if (isBool())
    result.setNum(int(boolval));
  else
    result.setNum(int(Error(*this)));
  return result;
}

bool VariantBase::xmlRead(const char* data)
{
  unsigned val = QByteArray::fromRawData(data,
                        xmlDataLength(data + xmlTrimLeft(data))).toUInt();
  if (isBool())
    boolval = val != 0;
  else
    error = Error(val);
  return error <= NotAnError;
}

void Variant::operator = (VariantData* newval)
{
  if (newval != (VariantData*)this || !isBuiltIn())
  {
    teardown();
    VariantBase::operator = (newval->lock());
  }
}

Variant& Variant::operator = (const Variant& other)
{
  if (&other != this)
  {
    if (!other.isBuiltIn())
      *this = ((const VariantData*)(other))->clone();
    else
      VariantBase::operator = (other);
  }
  return *this;
}

void Variant::teardown()
{
  if (!isBuiltIn())
    variantData()->release();
}

Variant::Variant(floatnum f, Error e)
{
  if (e != Success)
    *this = e;
  else
    *this = LongReal::create(f);
}
