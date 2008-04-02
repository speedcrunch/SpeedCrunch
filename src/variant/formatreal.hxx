/* formatreal.hxx: Formatting real numbers */
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

#ifndef _FORMATREAL_H
#define _FORMATREAL_H

#include "variant/formatbase.hxx"

class RealFormat: public FormatIntf
{
  public:
    typedef enum
    {
      fShowZeroScale = 1,
      fShowPlus = 2,
      fShowScalePlus = 4,
      fShowRadix = 8,
      fShowScaleRadix = 0x10,
      fShowLeadingZero = 0x20,
      fShowScaleLeadingZero = 0x40,
      fShowTrailingZero = 0x80,
      fShowTrailingDot = 0x100,
      fShortScale = 0x200,
      fLowerCaseDigit = 0x400,
    };
  public:
    RealFormat();
    QString format(const Variant&) const;
    void setMode(FmtMode, int digits = -1,
                 char base = 10, char scalebase = 10, char sgnfbase = 0);
    void setChars(QChar dot = '.', QChar exp = ' ', QChar group = ',', int grouplg = -1);
    void setMinLengths(int newMinInt = 0, int newMinFrac = 0,
                       int newMinScale = 0);
    void setFlags(unsigned flags);
  protected:
    virtual QString getSignificandPrefix(const RawFloatIO&) const;
    virtual QString getSignificandSuffix(const RawFloatIO&) const;
    virtual QString getScalePrefix(const RawFloatIO&) const;
    virtual QString getScaleSuffix(const RawFloatIO&) const;
    virtual QString formatNaN() const;
    virtual QString formatZero() const;
    virtual QString formatInt(const RawFloatIO&) const;
    virtual QString formatFrac(const RawFloatIO&) const;
    virtual QString formatScale(const RawFloatIO&) const;
    virtual QString intPart(const RawFloatIO&) const;
    virtual QString fracPart(const RawFloatIO&) const;
  protected:
    // format type
    FmtMode mode;
    // base of the scale
    char base;
    // radix to what the scale is shown
    char scalebase;
    // radix to what the significand is shown, != 10
    // ignored for base = 10
    char significandbase;
    // shown digits of the fraction of the significand
    int  digits;
    // minimum length of the integer part
    int  minIntLg;
    // minimum lensgth of the fraction part
    int  minFracLg;
    // minimum length of the scale
    int  minScaleLg;
    // length of a digit group
    int  grouplg;
    // dot character
    QChar dot;
    // character for digit grouping
    QChar groupchar;
    // character used to indicate the beginning of the scale
    // if ' ': 'e' is used for decimal bases, 'p' else
    // for better readability the case is opposite to lowerCaseHexDigit
    QChar scalechar;
    // show the scale, even if it is zero
    bool showZeroScale;
    // show the sign, even if the significand is not negative
    bool showPlus;
    // show the plus sign if the scale is not negative
    bool showScalePlus;
    // shows the radix tag of the significand
    bool showRadix;
    // show the radix tag of the scale
    bool showScaleRadix;
    // shows leading zeros to pad the integer part to the minimum size
    bool showLeadingZero;
    // shows leading zeros to pad the scale to the minimum size
    bool showScaleLeadingZero;
    // show trailing zeros in the fractional part
    bool showTrailingZero;
    // shows a trailing dot, if the fractional part is zero
    bool showTrailingDot;
    // use 'e' or 'p' as scale prefix only if significandbase == base
    bool shortScale;
    // use lower case hex digits
    bool lowerCaseHexDigit;
    // helpers
    bool useGrouping() const { return groupchar != 0 && grouplg > 0; };
    bool useScale(const RawFloatIO&) const;
    bool useShortScale() const;
    QChar getScaleChar() const;
    QString getPrefix(Sign, char base, bool isCompl) const;
};

#endif /*_FORMATREAL_H*/
