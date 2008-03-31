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
#include "math/floatconvert.h"
#include "math/floatconst.h"
#include "math/floatipower.h"
#include <QtXml/QDomText>
#include <QByteArray>

#include <math.h>

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
  sgnf.setNum(llrint(ldexp(frexp(x, &exp), 52)));
  floatstruct fs;
  float_create(&fs);
  float_setscientific(&fs, sgnf.data(), NULLTERMINATED);
  float_copy(&val, &c2, EXACT);
  // optimize for integer x
  _raisei(&val, 52 - exp, 22);
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

static bool _isZero(const QString& str)
{
  return str.size() == 1 && str.at(0) == '0';
}

RealFormat::RealFormat()
{
  setMode(Scientific);
  setChars();
  setMinLengths();
  setFlags(fShowRadix|fShowScaleRadix|fShowZeroScale);
}

void RealFormat::setMode(FmtMode m, int dgt, char b, char eb, char sb)
{
  mode = m;
  switch (mode)
  {
    case FixPoint:
    case FixSize: sb = 0; break;
    case Engineering: b = 10; break;
    case IEEE754:
    case Complement2: if (b == 10) mode = Scientific; break;
    default:;
  }
  if (b == 10 || sb == 10 || sb == 0)
    sb = b;
  base = b;
  scalebase = eb;
  significandbase = sb;
  int maxdgt;
  switch (b)
  {
    case  2: maxdgt = BINPRECISION;
    case  8: maxdgt = OCTPRECISION;
    case 16: maxdgt = HEXPRECISION;
    default: maxdgt = DECPRECISION;
  }
  digits = dgt <= 0 || dgt > maxdgt? maxdgt : dgt;
}

void RealFormat::setChars(QChar newdot, QChar newexp, QChar newgroup, int newgrouplg)
{
  if (dot == 0)
    dot = newdot;
  if (newgroup == 0)
    groupchar = newgroup;
  if (newexp == 0)
    scalechar = newexp;
  if (newgrouplg >= 0)
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
  shortScale = flags & fShortScale;
  lowerCaseHexDigit = flags & fLowerCaseDigit;
}

QString RealFormat::getPrefix(Sign sign, char base,
                              bool isCompl) const
{
  QString result;
  const char* radix;
  switch (sign)
  {
    case None:
    case Plus:
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

QString RealFormat::getSignificandPrefix(const RawFloatIO& io) const
{
  return getPrefix(io.signSignificand, significandbase,
                   mode == Complement2);
}

QString RealFormat::getSignificandSuffix(const RawFloatIO& io) const
{
  return QString();
}

QString RealFormat::getScalePrefix(const RawFloatIO& io) const
{
  QString result;
  QChar sc = getScaleChar();
  bool useParen = mode == IEEE754 || sc == '(';
  if (useShortScale())
    result = sc;
  else
  {
    result.setNum(base);
    result = 'x' + result + sc;
  }
  if (useParen)
    result += '(';
  return result
         + getPrefix(io.signScale, scalebase, false);
}

QString RealFormat::getScaleSuffix(const RawFloatIO& io) const
{
  if (mode == IEEE754)
    return "+bias)";
  if (useShortScale() && getScaleChar() != '(')
    return QString();
  return ")";
}

QString RealFormat::formatNaN() const
{
  return "NaN";
}

QString RealFormat::formatZero() const
{
  const char* result;
  if (showTrailingDot)
    result = "0.";
  else
    result = "0";
  return result;
}

QString RealFormat::formatInt(const RawFloatIO& io) const
{
  if (minIntLg <= 0 && !showLeadingZero && _isZero(io.intpart))
    return QString();
  QString intpart = intPart(io);
  if (minIntLg < intpart.size())
  {
    QChar pad = ' ';
    if (showLeadingZero)
      pad = '0';
    if (io.signSignificand == Compl2)
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

QString RealFormat::formatFrac(const RawFloatIO& io) const
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

QString RealFormat::formatScale(const RawFloatIO& io) const
{
  QString result = QString::number(io.scale, (int)scalebase);
  if (scalebase == 16 && !lowerCaseHexDigit)
    result = result.toUpper();
  if (result.size() < minScaleLg)
    result = QString(result.size() - minScaleLg,
                     showScaleLeadingZero? '0' : ' ') + result;
  return result;
}

static char digitsz(char base)
{
  char result = 1;
  switch (base)
  {
    case 16: ++result;
    case 8:  result+=2;
    default: ;
  }
  return result;
}

static QString changeBase(int idigitsz, int odigitsz, int osz,
                          QString input, int bufofs, bool lc)
{
  QString result(osz, 0);
  char buf = 0;
  char mask = (1 << odigitsz) - 1;
  char hexofs = lc? 'a' - 10 : 'A' - 10;
  char c;
  for (int i = -1; ++i < osz;)
  {
    while (bufofs < odigitsz)
    {
      c = input.at(i).toAscii() - '0';
      if (c > '9')
        c = (c - ('A' - '0' - 10)) & 0xF;
      buf = (buf << idigitsz) + c;
      bufofs += idigitsz;
    }
    bufofs -= odigitsz;
    char c = (buf >> bufofs) & mask;
    result[i] = c <= 9? c + '0' : c + hexofs;
  }
  return result;
}

QString RealFormat::intPart(const RawFloatIO& io) const
{
  if (significandbase == base)
    return io.intpart;
  int idigitsz = digitsz(base);
  int ibitlg = (io.intpart.size() << idigitsz);
  // reduce length by count of leading zero bits in the first digit
  const char* p = "1248";
  QChar c = io.intpart.at(0);
  while (*p)
    if (c < *(p++))
    --ibitlg;
  if (ibitlg == 0)
    return "0";
  int odigitsz = digitsz(significandbase);
  int osz = ((ibitlg - 1) >> odigitsz) + 1;
  return changeBase(idigitsz, odigitsz, osz, io.intpart,
                    (osz << odigitsz) - ibitlg, lowerCaseHexDigit);
}

QString RealFormat::fracPart(const RawFloatIO& io) const
{
  if (significandbase == base || digits == 0)
    return io.fracpart;
  return changeBase(digitsz(base), digitsz(significandbase), digits,
                    io.fracpart, 0, lowerCaseHexDigit);
}

QString RealFormat::format(const Variant& val) const
{
  QString result;
  const LongReal* vr = dynamic_cast<const LongReal*>((const VariantData*)val);
  if (!vr)
    return result;
  if (vr->isNaN())
    return formatNaN();
  if (vr->isZero())
    return formatZero();
  int usedigits = significandbase == base || digits == 0?
                  digits :
                  (digitsz(significandbase) * digits - 1) / digitsz(base) + 1;
  RawFloatIO RawFloatIO = vr->convert(usedigits, mode, base);
  if (RawFloatIO.error != Success)
    return result;
  result = getSignificandPrefix(RawFloatIO)
           + formatInt(RawFloatIO)
           + formatFrac(RawFloatIO)
           + getSignificandSuffix(RawFloatIO);
  if (useScale(RawFloatIO))
    result += getScalePrefix(RawFloatIO)
           + formatScale(RawFloatIO)
           + getScaleSuffix(RawFloatIO);
  return result;
}

bool RealFormat::useScale(const RawFloatIO& io) const
{
  return mode == IEEE754 || io.scale != 0 || showZeroScale;
}

bool RealFormat::useShortScale() const
{
  return shortScale && significandbase == base;
}

QChar RealFormat::getScaleChar() const
{
  if (scalechar == ' ')
  {
    if (lowerCaseHexDigit)
      return significandbase == 10? 'E' : 'P';
    else
      return significandbase == 10? 'e' : 'p';
  }
  return scalechar;
}
