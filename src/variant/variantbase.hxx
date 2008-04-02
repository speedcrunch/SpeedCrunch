/* variantbase.hxx: interface of variant type and derivates */
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

#ifndef _VARIANTBASE_H
#define _VARIANTBASE_H

#include "base/errors.h"
#include <QString>
#include <QtXml/QDomNode>
#include <QtXml/QDomDocument>

class Variant;
class VariantData;

typedef const void* VariantType;

class VariantIntf
{
  public: // virtual constructor
    typedef VariantData* (*Constructor)();
    virtual ~VariantIntf() {};
  public: // variant types
    virtual VariantType type() const = 0;
    static VariantType variantType(const char* name);
    static const char* typeName(VariantType);
    // currently supported types
    static const char* nBool;
    static const char* nError;
    static const char* nLongReal;
    static const char* nString;
  public: // IO
    virtual void xmlWrite(QDomDocument& doc, QDomNode& parent) const = 0;
    virtual bool xmlRead(QDomNode&) = 0;
  protected:
    static void xmlWriteText(QDomDocument& doc, QDomNode& parent, QString val);
    static QString xmlReadText(QDomNode& parent, bool* textOnly);
    static void initClass();
    static VariantType registerType(Constructor, const char* name);
    static VariantData* construct(VariantType);
};

class VariantData: public VariantIntf
{
  public: // shallow copy, reference counting
    VariantData() : refcount(0) { };
    void release() { if (--refcount <= 0) delete this; };
    VariantData* lock() { ++refcount; return this; };
    VariantData* clone() const
      { return const_cast<VariantData*>(this); }
    bool isUnique() { return refcount <= 1; };
  private:
    int refcount;
};

#endif /*_VARIANTBASE_H*/
