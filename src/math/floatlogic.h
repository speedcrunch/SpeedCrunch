/* floatlogic.h: logic functions, based on floatnum. */
/*
    Copyright (C) 2007, 2008 Wolf Lammen.

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

#ifndef FLOATLOGIC_H
# define FLOATLOGIC_H

#include "floatnum.h"
#include "floatconvert.h"

#ifdef __cplusplus
extern "C" {
#endif

char _canconvert(cfloatnum x);
char _floatnum2logic(t_longint* longint, cfloatnum x);
void _logic2floatnum(floatnum f, t_longint* longint);
void _not(t_longint* longint);
void _and(t_longint* x1, t_longint* x2);
void _or(t_longint* x1, t_longint* x2);
void _xor(t_longint* x1, t_longint* x2);
void _shr(t_longint* x, unsigned shift);
void _shl(t_longint* x, unsigned shift);

#ifdef __cplusplus
}
#endif

#endif /* FLOATLOGIC_H */
