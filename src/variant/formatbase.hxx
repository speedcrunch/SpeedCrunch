/* formatbase.hxx: interface of formats */
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

#ifndef _FORMATBASE_H
#define _FORMATBASE_H

#include "variant/variant.hxx"

extern const char* fmtSettings;
extern const char* fmtDefault;

typedef const char* FmtType;

typedef enum {
  fmtInclude,
  fmtFixed,
  fmtRelaxed,
} FmtLinkage;

class FormatIntf;

class Format
{
  friend class InitVariant;
  friend class FormatList;
  public:
    Format(const Format&);
    Format(FormatIntf* = 0);
    ~Format();
    bool operator == (const Format& other) const { return p == other.p; }
    bool operator != (const Format& other) const { return p != other.p; }
    Format& operator = (const Format&);
    void operator = (FormatIntf*);
    bool setBase(const QString&, quint64, FmtLinkage);
    FmtType type() const;
    QString format(const Variant&) const;
    bool isValid() const { return p != 0; };
    bool canHandle(VariantType vt) const;
    bool isCompatible(const Format&) const;
    QString key() const;
    const QStringList* getProps() const;
    bool setProp(const QString& prop, const Variant& val, bool inherit = false);
    Variant getProp(const QString& prop) const;
    static void add(const QString& key, Format);
    static void remove(const QString& key, FmtType aType);
    static QString format(const Variant& val, const QString& key);
  private:
    operator FormatIntf*() const { return p;};
    FormatIntf* p;
    static void initClass();
    static Format find(const QString& key, VariantType);
};

class FormatIntf
{
  public:
    FormatIntf();
    operator Format() { return Format(this); };
    virtual ~FormatIntf() { m_base = 0; };
    virtual FmtType type() const = 0;
    virtual QString format(const Variant&) const { return QString(); };
    virtual const QStringList& getProps() const = 0;
    virtual bool canHandle(VariantType vt) const { return false; };
    virtual bool isCompatible(FmtType ft) const;
    bool setProp(const QString& prop, const Variant& val, bool inherit);
    Variant getProp(const QString& prop);
    bool setBase(const QString& aBase, quint64 iflags, FmtLinkage = fmtRelaxed);
    bool isImporting(const QString& prop) const;
    void lock() { ++refcount; };
    void release();
    QString base() const;
    QString key; // maintained by FormatList
  protected:
    virtual bool setValue(int idx, const Variant& val) { return false; };
    virtual Variant getValue(int idx) const = 0;
    void import();
    void cloneFrom(Format basedOn);
    bool usesBase() const;
    Format findBase();
    int indexOfProp(const QString& prop) const;
    static bool setChar(QChar& dest, const Variant& val);
    static void getChar(Variant& v, QChar val) { v = QString(val); };
    static bool setInt(int& dest, const Variant&);
    static void getInt(Variant& v, int val) { v = (double)val; };
    static void initClass(QStringList** propList, const char** props);
    static quint64 flag(char ofs);
  private:
    int refcount;
    QString m_baseName;
    Format m_base;
    quint64 m_importmask;
};

#endif /*_FORMATBASE_H*/
