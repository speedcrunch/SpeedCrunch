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
#include "math/floatconfig.h"
#include "math/floatconvert.h"

enum { less = 1, equal = 2, greater = 4 };

static floatstruct NaNVal;
static int longrealPrec;

static int _cvtMode(LongReal::FmtMode mode)
{
  switch (mode)
  {
    case LongReal::FixPoint: return IO_MODE_FIXPOINT;
    case LongReal::Engineering: return IO_MODE_ENG;
    case LongReal::Complement2: return IO_MODE_COMPLEMENT;
    default: return IO_MODE_SCIENTIFIC;
  }
}

static char _mod(floatnum dest, cfloatnum dividend, cfloatnum modulo)
{
  enum { maxdivloops = 250 };
  int save = float_setprecision(int(maxdivloops));
  floatstruct dummy;
  float_create(&dummy);
  char result = float_divmod(&dummy, dest, dividend, modulo, INTQUOT);
  float_free(&dummy);
  float_setprecision(save);
  return result;
}

static char _idiv(floatnum dest, cfloatnum dividend, cfloatnum modulo)
{
  int save = float_setprecision(DECPRECISION);
  floatstruct dummy;
  float_create(&dummy);
  char result = float_divmod(dest, &dummy, dividend, modulo, INTQUOT);
  float_free(&dummy);
  float_setprecision(save);
  return result;
}

void LongReal::initClass()
{
  precision(precDefault);
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

bool LongReal::assign(const char* str)
{
  if (refcount != 1)
    return false;
  float_setscientific(&val, str, NULLTERMINATED);
  return !float_isnan(&val);
}

Variant LongReal::call2(const Variant& other, Fct2 fct, bool swap) const
{
  if (type() != other.type())
    return NotImplemented;
  floatstruct result;
  float_create(&result);
  (swap? fct(&result, other, *this, evalPrec())
       : fct(&result, *this, other, evalPrec()))
    && float_round(&result, &result, workPrec(), TONEAREST);
  return Variant(&result, float_geterror());
}

Variant LongReal::call2ND(const Variant& other, Fct2ND fct, bool swap) const
{
  if (type() != other.type())
    return NotImplemented;
  floatstruct result;
  float_create(&result);
  if (swap)
    fct(&result, other, *this);
  else
    fct(&result, *this, other);
  return Variant(&result, float_geterror());
}

Variant LongReal::callCmp(const Variant& other, char mask) const
{
  if (type() != other.type())
    return NotImplemented;
  signed char cmp = float_cmp(*this, other);
  if (cmp == UNORDERED)
    return NoOperand;
  if (cmp < 0 && (mask & less) != 0)
    return true;
  if (cmp > 0 && (mask & greater) != 0)
    return true;
  if (cmp == 0 && (mask & equal) != 0)
    return true;
  return false;
}

Variant LongReal::operator+() const
{
  if (float_isnan(&val))
    return NoOperand;
  return this;
}

Variant LongReal::operator-() const
{
  floatstruct result;
  float_create(&result);
  float_copy(&result, &val, EXACT);
  float_neg(&result);
  return Variant(&result, float_geterror());
}

Variant LongReal::operator+(const Variant& other) const
{
  return call2(other, float_add);
}

Variant LongReal::operator-(const Variant& other) const
{
  return call2(other, float_sub);
}

Variant LongReal::operator*(const Variant& other) const
{
  return call2(other, float_add);
}

Variant LongReal::operator/(const Variant& other) const
{
  return call2(other, float_add);
}

Variant LongReal::operator%(const Variant& other) const
{
  return call2ND(other, _mod);
}

Variant LongReal::idiv(const Variant& other) const
{
  return call2ND(other, _idiv);
}

Variant LongReal::operator==(const Variant& other) const
{
  return callCmp(other, equal);
}

Variant LongReal::operator!=(const Variant& other) const
{
  return callCmp(other, less | greater);
}

Variant LongReal::operator>(const Variant& other) const
{
  return callCmp(other, greater);
}

Variant LongReal::operator>=(const Variant& other) const
{
  return callCmp(other, greater | equal);
}

Variant LongReal::operator<(const Variant& other) const
{
  return callCmp(other, less);
}

Variant LongReal::operator<=(const Variant& other) const
{
  return callCmp(other, less | equal);
}

Variant LongReal::swapSub(const Variant& other) const
{
  return call2(other, float_sub, true);
}

Variant LongReal::swapDiv(const Variant& other) const
{
  return call2(other, float_div, true);
}

Variant LongReal::swapMod(const Variant& other) const
{
  return call2ND(other, _mod, true);
}

Variant LongReal::swapIdiv(const Variant& other) const
{
  return call2ND(other, _idiv, true);
}

int LongReal::precision(int newprec)
{
  int result = longrealPrec;
  if (newprec == 0 || newprec > DECPRECISION)
    newprec = DECPRECISION;
  if (newprec > 0)
    longrealPrec = newprec;
  return result;
}

int LongReal::evalPrec()
{
  return longrealPrec + 5;
}

int LongReal::workPrec()
{
  return longrealPrec + 3;
}

LongReal::operator QByteArray() const
{
  char buffer[DECPRECISION+30];
  float_getscientific(buffer, sizeof(buffer), &val);
  return buffer;
}

// LongReal::RawIO LongReal::convert(int prec, FmtMode mode,
//                                   char base, char scalebase)
// {
//   t_otokens tokens;
//   float_out(&tokens, &val, prec, base, scalebase, _cvtMode(mode));
// }
// 
// LongReal* LongReal::convert(const RawIO&)
// {
// }
