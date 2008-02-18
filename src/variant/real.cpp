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
static LongReal::EvalMode longrealEvalMode;

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

static LongReal::Sign _cvtSign(signed char sign)
{
  switch (sign)
  {
    case IO_SIGN_COMPLEMENT: return LongReal::Compl2;
    case IO_SIGN_MINUS     : return LongReal::Minus;
    case IO_SIGN_PLUS      : return LongReal::Plus;
    default                : return LongReal::None;
  }
}

static signed char _cvtSign(LongReal::Sign sign)
{
  switch (sign)
  {
    case LongReal::Compl2: return IO_SIGN_COMPLEMENT;
    case LongReal::Minus : return IO_SIGN_MINUS;
    case LongReal::Plus  : return IO_SIGN_PLUS;
    default              : return IO_SIGN_NONE;
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
  precision(PrecDefault);
  evalMode(EvalRelaxed);
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
  return call2(other, float_mul);
}

Variant LongReal::operator/(const Variant& other) const
{
  return call2(other, float_div);
}

Variant LongReal::operator%(const Variant& other) const
{
  return call2ND(other, _mod);
}

Variant LongReal::idiv(const Variant& other) const
{
  return call2ND(other, _idiv);
}

Variant LongReal::raise(const Variant& other) const
{
  return call2(other, float_raise);
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

Variant LongReal::swapRaise(const Variant& other) const
{
  return call2(other, float_raise, true);
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

LongReal::operator QByteArray() const
{
  char buffer[DECPRECISION+30];
  float_getscientific(buffer, sizeof(buffer), &val);
  return buffer;
}

LongReal::BasicIO LongReal::convert(int digits, FmtMode mode,
                   char base, char scalebase) const
{
  t_otokens tokens;
  floatstruct workcopy;
  BasicIO result;
  char intpart[BINPRECISION+5];
  char fracpart[BINPRECISION+5];

  tokens.intpart.buf = intpart;
  tokens.intpart.sz = sizeof(intpart);
  tokens.fracpart.buf = fracpart;
  tokens.fracpart.sz = sizeof(fracpart);
  float_create(&workcopy);
  float_copy(&workcopy, &val, evalPrec());
  result.error = float_out(&tokens, &workcopy, digits,
                           base, scalebase, _cvtMode(mode));
  if (result.error == Success)
  {
    if (tokens.exp >= 0)
    {
      result.scale = tokens.exp;
      result.signScale = tokens.exp > 0? LongReal::Plus
        : LongReal::None;
    }
    else
    {
      result.scale = -tokens.exp;
      result.signScale = LongReal::Minus;
    }
    result.baseSignificand = base;
    result.baseScale = scalebase;
    result.signSignificand = _cvtSign(tokens.sign);
    result.intpart = QString::fromAscii(tokens.intpart.buf);
    result.fracpart = QString::fromAscii(tokens.fracpart.buf);
  }
  return result;
}

Variant LongReal::convert(const BasicIO& io, const QString& scale)
{
  t_itokens tokens;
  QByteArray intpart = io.intpart.toUtf8();
  QByteArray fracpart = io.fracpart.toUtf8();
  tokens.intpart = intpart.data();
  tokens.fracpart = intpart.data();
  tokens.exp = 0;
  tokens.expbase = IO_BASE_NAN;
  tokens.expsign = IO_SIGN_NONE;
  tokens.sign = _cvtSign(io.signSignificand);
  tokens.base = io.baseSignificand;
  tokens.maxdigits = evalPrec();
  if (!scale.isEmpty())
  {
    QByteArray bscale = scale.toUtf8();
    tokens.exp = bscale.data();
    tokens.expbase = io.baseScale;
    tokens.expsign = io.signScale;
  }
  floatstruct val;
  float_create(&val);
  Error e = float_in(&val, &tokens);
  return Variant(&val, e);
}

static bool _isZero(const QString& str)
{
  return str.size() == 1 && str.at(0) == '0';
}

RealFormat::RealFormat()
{
  setMode(LongReal::Scientific);
  setGroupChars();
  setMinLengths();
  setFlags(fShowRadix|fShowScaleRadix|fShowZeroScale);
}

void RealFormat::setMode(LongReal::FmtMode m, int dgt, char b, char sb)
{
  mode = m;
  base = b;
  scalebase = sb;
  int maxdgt;
  switch (b)
  {
    case  2: maxdgt = BINPRECISION;
    case  8: maxdgt = OCTPRECISION;
    case 16: maxdgt = HEXPRECISION;
    default: maxdgt = DECPRECISION;
  }
  if (dgt <= 0 || dgt > maxdgt)
    digits = maxdgt;
  else
    digits = dgt;
}

void RealFormat::setGroupChars(QChar newdot, QChar newgroup, int newgrouplg)
{
  dot = newdot;
  groupchar = newgroup;
  grouplg = newgrouplg;
}

void RealFormat::setMinLengths(int newMinInt, int newMinFrac, int newMinScale)
{
  minIntLg = newMinInt;
  minFracLg = newMinFrac;
  minScaleLg = newMinScale;
}

void RealFormat::setFlags(unsigned flags)
{
  showZeroScale = flags & fShowZeroScale;
  showPlus = flags & fShowPlus;
  showScalePlus = flags & fShowScalePlus;
  showRadix = flags & fShowRadix;
  showScaleRadix = flags & fShowScaleRadix;
  showLeadingZero = flags & fShowLeadingZero;
  showScaleLeadingZero = flags & fShowScaleLeadingZero;
  showTrailingZero = flags & fShowTrailingZero;
  showTrailingDot = flags & fShowTrailingDot;
  lowerCaseHexDigit = flags & fLowerCaseDigit;
}

QString RealFormat::getPrefix(LongReal::Sign sign, char base,
                              bool isCompl)
{
  QString result;
  const char* radix;
  switch (sign)
  {
    case LongReal::None:
    case LongReal::Plus:
      if (showPlus)
        result = '+';
      break;
    default:
      result = '-';
  }
  if (showRadix)
  {
    switch (base)
    {
      case 16: radix = "0x"; break;
      case  8: radix = "0o"; break;
      case  2: radix = "0b"; break;
      default: radix = "0d";
    }
    if (isCompl)
      result = radix + result;
    else
      result += radix;
  }
  if (lowerCaseHexDigit)
    result = result.toUpper();
  return result;
}

QString RealFormat::getSignificandPrefix(LongReal::BasicIO& io)
{
  return getPrefix(io.signSignificand, io.baseSignificand,
                   mode == LongReal::Complement2);
}

QString RealFormat::getSignificandSuffix(LongReal::BasicIO& io)
{
  return QString();
}

QString RealFormat::getScalePrefix(LongReal::BasicIO& io)
{
  QString result('e');
  if (io.baseSignificand != 10)
    result = '(';
  return result
         + getPrefix(io.signScale, io.baseScale, false);
}

QString RealFormat::getScaleSuffix(LongReal::BasicIO& io)
{
  if (io.baseSignificand == 10)
    return QString();
  return QString(')');
}

QString RealFormat::formatNaN()
{
  return "NaN";
}

QString RealFormat::formatZero()
{
  const char* result;
  if (showTrailingDot)
    result = "0.";
  else
    result = "0";
  return result;
}

QString RealFormat::formatInt(LongReal::BasicIO& io)
{
  if (minIntLg <= 0 &&!showLeadingZero && _isZero(io.intpart))
    return QString();
  QString intpart = io.intpart;
  if (minIntLg < intpart.size())
  {
    QChar pad = ' ';
    if (showLeadingZero)
      pad = '0';
    if (io.signSignificand == LongReal::Compl2)
      switch (io.baseSignificand)
      {
        case 2 : pad = '1'; break;
        case 8 : pad = '7'; break;
        case 16: pad = 'F'; break;
      }
    intpart = QString(minIntLg - intpart.size(), pad) + intpart;
  }
  if (lowerCaseHexDigit)
    intpart = intpart.toLower();
  if (!useGrouping() || grouplg >= intpart.size())
    return intpart;
  QString result;
  int idx = intpart.size();
  while (idx > 0)
  {
    idx -= grouplg;
    QChar gchar = ' ';
    if (intpart.at(idx) != ' ')
      gchar = groupchar;
    if (idx <= 0)
      result = intpart.mid(0, idx + grouplg) + result;
    else
      result = intpart.mid(idx, grouplg) + gchar + result;
  }
  return result;
}

QString RealFormat::formatFrac(LongReal::BasicIO& io)
{
  QString fracpart = io.fracpart;
  if (fracpart.size() < minFracLg)
  {
    QChar pad = ' ';
    if (showTrailingZero)
      pad = '0';
    fracpart += QString(minFracLg - fracpart.size());
  }
  else if (!showTrailingZero)
  {
    int i = fracpart.size();
    while (--i >= minFracLg && fracpart.at(i) == '0');
    fracpart = fracpart.mid(0, i);
  }
  if (fracpart.isEmpty())
  {
    if (showTrailingDot)
      return dot;
    else
      return QString();
  }
  if (lowerCaseHexDigit)
    fracpart = fracpart.toLower();
  if (!useGrouping() || fracpart.size() <= grouplg)
    return dot + fracpart;
  QString result = dot;
  result += fracpart.mid(0, grouplg);
  for (int idx = grouplg; idx < fracpart.size(); idx += grouplg)
  {
    QChar gchar = ' ';
    if (fracpart.at(idx) != ' ')
      gchar = groupchar;
    result += gchar + fracpart.mid(idx, grouplg);
  }
  return result;
}

QString RealFormat::formatScale(LongReal::BasicIO& io)
{
  QString result = QString::number(io.scale, (int)io.baseScale);
  if (io.baseScale == 16 && !lowerCaseHexDigit)
    result = result.toUpper();
  if (result.size() < minScaleLg)
    result = QString(result.size() - minScaleLg,
                     showScaleLeadingZero? '0' : ' ') + result;
  return result;
}

QString RealFormat::format(const Variant& val)
{
  QString result;
  const LongReal* vr = dynamic_cast<const LongReal*>((const VariantData*)val);
  if (!vr)
    return result;
  if (vr->isNaN())
    return formatNaN();
  if (vr->isZero())
    return formatZero();
  LongReal::BasicIO basicIO = vr->convert(digits, mode, base, scalebase);
  if (basicIO.error != Success)
    return result;
  result = getSignificandPrefix(basicIO)
           + formatInt(basicIO)
           + formatFrac(basicIO)
           + getSignificandSuffix(basicIO);
  if (useScale(basicIO))
    result += getScalePrefix(basicIO)
           + formatScale(basicIO)
           + getScaleSuffix(basicIO);
  return result;
}

bool RealFormat::useScale(const LongReal::BasicIO& io)
{
  return io.scale != 0 || showZeroScale;
}
