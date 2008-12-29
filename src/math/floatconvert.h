/* floatconvert.h: radix conversion, based on floatnum. */
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

#ifndef FLOATCONVERT_H
# define FLOATCONVERT_H

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
   IOConversionOverflow, if the bigint overflows */
Error _floatnum2longint(t_longint* longint, floatnum f);
/* converts a binary coded bigint into a floatnum */
void _longint2floatnum(floatnum f, t_longint* longint);

/* the output process destroys x
   'digits' are the number of digits after the dot.
   Regardless of the value of 'digits', a conversion is always
   done to DECPRECISION places
   Before reducing to 'digits' places the (converted) value is rounded.
   Trailing zeros are padded, if necessary, to fill to the right size.
   Errors: InvalidParam (if any of the parameters makes no sense
                         like digits <= 0, or a not supported base)
           IOBufferOverflow (if the caller does not provide enough
                             buffer in tokens)
           IOConversionOverflow (request requires too much buffer
                                 space for radix conversion)
           IOConversionUnderflow (request would produce leading
                                  zeros only)
           IOInvalidComplement (two's complement cannot be generated) */
Error float_out(p_otokens tokens, floatnum x, int digits,
                signed char base, char outmode);
/* returns Success or one of the IO... codes
   Errors: BadLiteral, set in addition to the returned result */
Error float_in(floatnum x, p_itokens tokens);

#ifdef __cplusplus
}
#endif

#endif /* FLOATCONVERT_H */
