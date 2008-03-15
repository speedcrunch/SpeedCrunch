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
#include <QByteArray>
#include <QString>

class Variant;
class VariantData;

class VariantIntf
{
  public: // virtual constructor
    typedef VariantData* (*Constructor)();
  public: // variant types
    typedef const void* VariantType;
    virtual VariantType type() const = 0;
    static const char* nBool;
    static const char* nError;
    static const char* nLongReal;
    static VariantType variantType(const char* name);
    static const char* typeName(VariantType);
  public:
    virtual ~VariantIntf() {};
  protected:
    static void initClass();
    static VariantType registerType(Constructor, const char* name);
    static VariantData* construct(VariantType);
};

class VariantData: public VariantIntf
{
  public: // deep copy, no reference counting
    virtual void release() { delete this; };
    virtual VariantData* lock() { return this; };
    virtual VariantData* clone() const;
};

class VariantRefData: public VariantData
{
  public: // shallow copy, reference counting
    VariantRefData() : refcount(0) { };
    void release() { if (--refcount <= 0) delete this; };
    VariantData* lock() { ++refcount; return this; };
    VariantData* clone() const
      { return const_cast<VariantRefData*>(this); }
    bool isUnique() { return refcount == 1; };
  private:
    int refcount;
};

class FormatIntf
{
  public:
    virtual ~FormatIntf() {};
    virtual QString format(const Variant&) = 0;
};

#endif /*_VARIANTBASE_H*/
