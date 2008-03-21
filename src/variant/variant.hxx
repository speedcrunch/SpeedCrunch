/* variant.hxx: variant class */
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

#ifndef _VARIANT_H
#define _VARIANT_H

#include "base/errors.h"
#include "variant/variantbase.hxx"
#include "variant/real.hxx"

class VariantBase: public VariantIntf
// takes care of builtin data types
{
  public:
    VariantBase() { setup(); };
    VariantType type() const;
    operator bool() const;
    operator Error() const;
    bool operator = (bool);
    Error operator = (Error);
    operator const VariantData*() const;
    QByteArray xmlWrite() const;
    bool xmlRead(const char*);
  protected:
    bool isBuiltIn() const { return m_type != btExtern; };
    void operator = (VariantData*);
    VariantData* variantData() { return val; };
    void operator = (const VariantBase& other);
    virtual void teardown() = 0;
    static void initClass();
  private:
    enum
    {
      btError,
      btBool,
      btExtern
    } m_type;
    union
    {
      Error error;
      bool boolval;
      VariantData* val;
    };
    bool isBool() const { return m_type == btBool; };
    bool isError() const { return m_type == btError; };
    void setup();
    static VariantData* builtInConstructor() { return 0; };
    static VariantType vtBool;
    static VariantType vtError;
};

class Variant: public VariantBase
{
  friend class InitVariant;
  public: // constructors
    Variant() { };
    Variant(bool v) { *this = v; };
    Variant(Error e) { *this = e; };
    Variant(VariantData* vd) { *this = vd; };
    ~Variant() { teardown(); };
  public: // assignment
    Variant& operator = (const Variant& other);
    void operator = (VariantData*);
  public: // types & conversion
  public: // operators & functions
  protected:
    void teardown();
  private:
    static void initClass();
  public: // support of LongReal
    Variant(floatnum, Error);
};

#endif /*_VARIANT_H*/
