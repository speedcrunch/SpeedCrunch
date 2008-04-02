/* formatreal.cpp: Formatting real numbers */
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

#include "variant/formatreal.hxx"

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
