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

#include "math/floatconvert.h"
#include "math/floatconst.h"
#include "math/floatipower.h"
#include "variant/variant.hxx"
#include "variant/real.hxx"
#include <QtXml/QDomText>
#include <QByteArray>

#include <math.h>

#define LLRINT_AVAILABLE

const char* VariantIntf::nLongReal = "LongReal";
VariantType LongReal::vtLongReal;

static floatstruct NaNVal;
static int longrealPrec;
static LongReal::EvalMode longrealEvalMode;

static int _cvtMode(FmtMode mode)
{
  switch (mode)
  {
    case FixPoint: return IO_MODE_FIXPOINT;
    case Engineering: return IO_MODE_ENG;
    case Complement2: return IO_MODE_COMPLEMENT;
    default: return IO_MODE_SCIENTIFIC;
  }
}

static Sign _cvtSign(signed char sign)
{
  switch (sign)
  {
    case IO_SIGN_COMPLEMENT: return Compl2;
    case IO_SIGN_MINUS     : return Minus;
    case IO_SIGN_PLUS      : return Plus;
    default                : return None;
  }
}

static signed char _cvtSign(Sign sign)
{
  switch (sign)
  {
    case Compl2: return IO_SIGN_COMPLEMENT;
    case Minus : return IO_SIGN_MINUS;
    case Plus  : return IO_SIGN_PLUS;
    default    : return IO_SIGN_NONE;
  }
}

// static char _mod(floatnum dest, cfloatnum dividend, cfloatnum modulo)
// {
//   enum { maxdivloops = 250 };
//   int save = float_setprecision(int(maxdivloops));
//   floatstruct dummy;
//   float_create(&dummy);
//   char result = float_divmod(&dummy, dest, dividend, modulo, INTQUOT);
//   float_free(&dummy);
//   float_setprecision(save);
//   return result;
// }
// 
// static char _idiv(floatnum dest, cfloatnum dividend, cfloatnum modulo)
// {
//   int save = float_setprecision(DECPRECISION);
//   floatstruct dummy;
//   float_create(&dummy);
//   char result = float_divmod(dest, &dummy, dividend, modulo, INTQUOT);
//   float_free(&dummy);
//   float_setprecision(save);
//   return result;
// }

void LongReal::initClass()
{
  floatmath_init();
  precision(PrecDefault);
  evalMode(EvalRelaxed);
  float_create(&NaNVal);
  vtLongReal = registerType(create, nLongReal);
}

VariantData* LongReal::create()
{
  return new LongReal;
}

LongReal* LongReal::create(floatnum f)
{
  // post: sets f to NaN
  LongReal* lr = static_cast<LongReal*>(create());
  lr->move(f);
  return lr;
}

cfloatnum LongReal::NaN()
{
  return &NaNVal;
}

bool LongReal::move(floatnum x)
{
  if (!isUnique())
    return false;
  float_move(&val, x);
  return true;
}

int LongReal::precision(int newprec)
{
  int result = longrealPrec;
  if (newprec == PrecDefault || newprec > DECPRECISION)
    newprec = DECPRECISION;
  if (newprec != PrecQuery)
    longrealPrec = newprec;
  return result;
}

LongReal::EvalMode LongReal::evalMode(EvalMode newMode)
{
  EvalMode result = longrealEvalMode;
  if (newMode != EvalQuery)
    longrealEvalMode = newMode;
  return result;
}

bool LongReal::isNaN() const
{
  return float_isnan(&val);
}

bool LongReal::isZero() const
{
  return float_iszero(&val);
}

int LongReal::evalPrec()
{
  return longrealPrec + 5;
}

int LongReal::workPrec()
{
  return longrealPrec + 3;
}

LongReal::operator double() const
{
  if (isZero())
    return 0;
  RawFloatIO io = convert(52, Scientific, 2);
  qint64 sgnf = Q_INT64_C(4503599627370496)
                + io.fracpart.toLongLong(0, 2);
  double r = io.signSignificand == Plus? sgnf : -sgnf;
  int exp = io.scale;
  return ldexp(ldexp(r, -52), io.signScale == Plus? exp : -exp);
}

#ifndef LLRINT_AVAILABLE
qint64 llrint(double x)
{
  qint64 result = 0;
  x = ldexp(x, -52);
  for (int i = -1; ++i <= 52;)
  {
    result <<= 1;
    if (x >= 1)
    {
      ++result;
      x -= 1;
    }
    x *= 2;
  }
  return result;
}
#endif

void LongReal::operator = (double x)
{
  // the result is undefined for x = NaN or Inf
  if (x == 0)
  {
    float_setzero(&val);
    return;
  }
  int exp;
  QByteArray sgnf;
  sgnf.setNum(llrint(ldexp(frexp(x, &exp), 53)));
  floatstruct fs;
  float_create(&fs);
  float_setscientific(&fs, sgnf.data(), NULLTERMINATED);
  float_copy(&val, &c2, EXACT);
  // optimize for integer x
  _raisei(&val, 53 - exp, 22);
  float_div(&val, &fs, &val, 19);
  float_round(&val, &val, 16, TONEAREST);
  float_free(&fs);
}

void LongReal::xmlWrite(QDomDocument& doc, QDomNode& parent) const
{
  // fill a buffer with a description of val in ASCII
  int lg = float_getlength(&val) + BITS_IN_EXP + 2;
  QByteArray buf(lg, 0);
  float_getscientific(buf.data(), lg, &val);

  // copy the buffer to the text portion of the given element
  xmlWriteText(doc, parent, buf);
}

bool LongReal::xmlRead(QDomNode& node)
{
  // pre: node is an element
  floatstruct newval;
  float_create(&newval);
  bool ok;
  QByteArray buf = xmlReadText(node, &ok).toUtf8();
  if (ok)
  {
    float_setscientific(&newval, buf.data(), NULLTERMINATED);
    ok = (!float_isnan(&newval) || buf == "NaN");
  }
  if (ok)
    float_move(&val, &newval);
  return ok;
}

RawFloatIO LongReal::convert(int digits, FmtMode mode,
                             char base) const
{
  t_otokens tokens;
  floatstruct workcopy;
  RawFloatIO result;
  char intpart[BINPRECISION+5];
  char fracpart[BINPRECISION+5];

  tokens.intpart.buf = intpart;
  tokens.intpart.sz = sizeof(intpart);
  tokens.fracpart.buf = fracpart;
  tokens.fracpart.sz = sizeof(fracpart);
  float_create(&workcopy);
  float_copy(&workcopy, &val, evalPrec());
  result.error = float_out(&tokens, &workcopy, digits,
                           base, _cvtMode(mode));
  if (result.error == Success)
  {
    if (tokens.exp >= 0)
    {
      result.scale = tokens.exp;
      result.signScale = tokens.exp > 0? Plus : None;
    }
    else
    {
      result.scale = -tokens.exp;
      result.signScale = Minus;
    }
    result.baseSignificand = base;
    result.signSignificand = _cvtSign(tokens.sign);
    result.intpart = QString::fromAscii(tokens.intpart.buf);
    result.fracpart = QString::fromAscii(tokens.fracpart.buf);
  }
  return result;
}

Variant LongReal::convert(const RawFloatIO& io)
{
  if (io.error != Success)
    return io.error;
  t_itokens tokens;
  QByteArray intpart = io.intpart.toUtf8();
  QByteArray fracpart = io.fracpart.toUtf8();
  tokens.intpart = intpart.data();
  tokens.fracpart = intpart.data();
  tokens.sign = _cvtSign(io.signSignificand);
  tokens.base = io.baseSignificand;
  tokens.exp = 0;
  tokens.expsign = IO_SIGN_NONE;
  if (io.scale != 0)
  {
    tokens.exp = io.scale;
    tokens.expsign = io.signScale;
  }
  tokens.maxdigits = evalPrec();
  floatstruct val;
  float_create(&val);
  Error e = float_in(&val, &tokens);
  return Variant(&val, e);
}
