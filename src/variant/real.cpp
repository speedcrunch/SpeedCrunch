/* real.cpp: real numbers */
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

#include "variant/real.hxx"

static floatstruct NaNVal;

void LongReal::initClass()
{
  float_create(&NaNVal);
  registerConstructor(create, vLongReal);
}

LongReal::LongReal()
  : refcount(1)
{
  float_create(&val);
}

LongReal::~LongReal()
{
  float_free(&val);
}

VariantData* LongReal::create()
{
  return new LongReal;
}

void LongReal::release()
{
  if (--refcount <= 0)
    delete this;
}

VariantData* LongReal::clone() const
{
  ++refcount;
  return const_cast<LongReal*>(this);
}

cfloatnum LongReal::NaN()
{
  return &NaNVal;
}

bool LongReal::move(floatnum x)
{
  if (refcount != 1)
    return false;
  float_move(&val, x);
  return true;
}

Variant LongReal::operator+() const
{
  if (float_isnan(&val))
    return NoOperand;
  return this->clone();
}

Variant LongReal::operator-() const
{
  floatstruct result;
  float_create(&result);
  float_copy(&result, &val, EXACT);
  float_neg(&result);
  Variant r;
  r.move(&result, float_geterror());
  return r;
}
