/* initvariant.cpp: initialization on startup */
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

#include "variant/variant.hxx"
#include "variant/real.hxx"
#include "variant/vstring.hxx"

class InitVariant
{
  public:
    InitVariant();
};

InitVariant instance;

// void test()
// {
//   char buf[100];
//   Variant x = Variant::fromUtf8("12345678912345", VariantIntf::nLongReal);
//   double d = x;
//   x = d;
//   float_getscientific(buf, 100, x);
//   if (d == 0)
//     d += 1;
// }

InitVariant::InitVariant()
{
  Variant::initClass();
  LongReal::initClass();
  VString::initClass();

//   test();
}
