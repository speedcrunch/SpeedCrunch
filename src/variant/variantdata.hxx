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

class Variant;

class VariantIntf
{
  public:
    typedef enum
    {
      vBoolean,
      vLongReal,
      vError // has to be the last entry
    } VariantType;

  public:
    virtual ~VariantIntf() {};
  public: // type & casting
    virtual VariantType type() const = 0;
    signed char cmpType(VariantType other) const;
    virtual Variant embed() const;
    virtual operator Error() const { return Success; };
  public: // operators & functions interface
    virtual Variant operator-() const;
    virtual Variant operator+() const;
    virtual Variant operator~() const;
    virtual Variant operator!() const;
    virtual Variant operator+(const Variant& other) const;
    virtual Variant operator-(const Variant& other) const;
    virtual Variant operator==(const Variant& other) const;
};

class VariantData: public VariantIntf
{
  public: // virtual constructor
    typedef VariantData* (*Constructor)();
    static void registerConstructor(Constructor, VariantType);
    static VariantData* create(VariantType);
    virtual void release() { delete this; };
  public: // copying
    virtual VariantData* clone() const { return 0; };
  public: // swapped parameter operations
    virtual Variant swapAdd(const Variant& other) const;
    virtual Variant swapSub(const Variant& other) const;
    virtual Variant swapEq(const Variant& other) const;
};

#endif /*_VARIANTBASE_H*/
