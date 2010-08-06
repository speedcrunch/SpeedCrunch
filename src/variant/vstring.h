/* vstring.h: variant string class */
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

#ifndef _VSTRING_H
#define _VSTRING_H

#include "variant/variant.h"

class VString: public VariantData
{
  friend class InitVariant;
  public:
    static VString* create(const QString&);
    VariantType type() const { return vtString; };
    void xmlWrite(QDomDocument&, QDomNode&) const;
    bool xmlRead(QDomNode&);
    operator QString() const { return val; };
  private:
    QString val;
    static void initClass();
    static VariantType vtString;
    static VariantData* create();
};

#endif /*_VSTRING_H*/
