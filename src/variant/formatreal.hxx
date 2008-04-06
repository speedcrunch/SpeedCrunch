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

extern const char* fmtLongReal;

class RealFormat: public FormatIntf
{
  friend class InitVariant;
  public:
    FmtType type() const { return fmtLongReal; };
    const QStringList& getProps() const { return *propList; };
    bool canHandle(VariantType vt) const;
    QString format(const Variant&);
    bool setDigits(int);
    int getDigits() const;
  protected:
    bool setValue(int idx, const Variant& val);
    Variant getValue(int idx) const;
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
    static bool isValidBase(char);
  protected:
    // format type
    FmtMode mode;
    // base of the scale
    char base;
    // radix to what the significand is shown, != 10
    // ignored for base = 10
    char significandbase;
    // radix to what the scale is shown
    char scalebase;
    // bits of the fractional part covered by the output
    int precision;
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
    bool setMode(FmtMode fm);
    FmtMode getMode() const { return mode; };
    bool setBase(char);
    char getBase() const;
    bool setSignificandBase(char);
    char getSignificandBase() const;
    bool setPrecision(int);
    int getPrecision() const { return precision; };
    bool setScaleBase(char);
    char getScaleBase() const;
  private:
    QString doFormat(const Variant&) const;
    static void initClass();
    static QStringList* propList;
};

#endif /*_FORMATREAL_H*/
