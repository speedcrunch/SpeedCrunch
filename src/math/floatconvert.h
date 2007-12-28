/* floatconvert.h: radix conversion, based on floatnum. */
/*
    Copyright (C) 2007 Wolf Lammen.

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

#ifndef _FLOATCONVERT_H
# define _FLOATCONVERT_H

#include "floatnum.h"
#include "floatio.h"
#include "floatlong.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IO_MODE_SCIENTIFIC 0
#define IO_MODE_FIXPOINT   1
#define IO_MODE_ENG        2
#define IO_MODE_COMPLEMENT 3

/* converts the integer part of f to a binary coded bigint. Returns
   0, if the bigint overflows */
char _floatnum2longint(t_longint* longint, floatnum f);
/* converts a binary coded bigint into a floatnum */
void _longint2floatnum(floatnum f, t_longint* longint);

char float_out(p_otokens tokens, floatnum x, int digits, signed char base,
               signed char expbase, char outmode);
/* returns one of the IO_ERROR... codes */
int float_in(floatnum x, p_itokens tokens);

#ifdef __cplusplus
}
#endif

#endif /* _FLOATCONVERT_H */
