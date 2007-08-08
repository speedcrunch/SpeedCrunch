/* floatdefines.h: basic settings in floatnum. */
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

/* this file contains the basic settings, that control the overall
   behaviour of floatnum and derivatives.
   Uncomment or set any of the following defines according to your needs.
*/

#ifndef _FLOATCONFIG_H
#define _FLOATCONFIG_H

/* FLOATDEBUG introduces some extensions to floatnum, so you can
   easily follow operations of floatnum in a debugger like ddd.
   Uncomment this, if you develop code based on floatnum, and if you want
   to look into floatnum variables during a debugger session. */
/* #define FLOATDEBUG */

/* enables a pre-defined set of macros so that a regression test suite
   of floatnum can be executed. These settings are such that corner cases
   can easily be triggered. The settings are not useful in a real application
   of floatnum.
   Uncomment this if you want to run the standard regression test suite
   of floatnum */
/* #define _FLOATNUMTEST */

/* floatnum uses bc's bc_num format to store and operate on data. Since
   bc_num is an arbitrary precision format, operands may grow to extreme
   sizes, where complex operations take considerable time to execute,
   up to an hour or more. floatconfig defines a guard value to avoid extreme
   long operands. Any request to produce a result of more than MAXDIGITS
   digits is considered an error and yields a NaN result.
   When setting this value, bear in mind, this is a global value that
   effects internal operations as well as 'user' requests. When using
   the routines for transcendent mathematical functions, you should allow
   extra 14 digits, so that operations like float_exp do not fail when
   they compute something near maximum precision. */
#define MAXDIGITS 250

/* the number of bits into which the exponent of a floatnum is encoded.
   In order to avoid integer overflow, this should be at least two bits
   less than the bits in the integer type choosen for the exponent.
   Conversion into binary formats needs two extra bits. The
   default is four bits less than the bits in 'int' */
/* #define BITS_IN_EXP 28 */

/* floatnum puts an upper limit on the base 10 exponent of its numbers that
   is based on the size of an integer, but even for 16 bit integers this
   limit is as big as 4095. Real applications usually do not need such
   big numbers. If you want to limit the range of possible numbers, set
   this value accordingly. An operation result exceeding this limit is
   converted into a NaN, and an overflow/underflow is reported. The
   default is the maximum positive value that can be encoded in
   BITS_IN_EXP bits.
   If you change EXPMAX, you might want to reduce BITS_IN_EXP as well. */
/* #define EXPMAX 5000 */

/* The precision of basic operations like + or * is limited by MAXDIGITS.
   In addition, some higher mathematical functions involve constants,
   that, of course, are stored to a limited precision only. This puts
   another bound on floatnum, described by the value MATHPRECISION.
   Since procedures for higher mathematical functions employ basic operations
   to determine their result, MATHPRECISION is <= MAXDIGITS.
   The current math library version of floatnum limits higher mathematical
   functions to 100 digits precision.
   One can say, MATHPRECISION describes the granularity of the number
   space, because higher functions do not (reliably) produce different results for
   values closer to each other than this granularity.
   You may re-define granularity by setting DECPRECISION. This will never
   bypass MATHPRECISION, being always the limit for the math library,
   but basic operations may benefit from finer granularity, up to the
   overall limit MAXDIGITS.
   If you lower granularity, that saves some memory and evaluation time in
   a few places.
   Granularity means that integers with more than DECPRECISION digits
   might not be saved without loss of digits. So DECPRECISION defines
   the integer range of floatnum.
   Because base convertion and logic operations are integer based, both
   are limited by DECPRECISION as well.
   By default, DECPRECISION is set to MATHPRECISION */
#define DECPRECISION 78

/* The integer domain of logical functions is a true subset of the integer range,
   because, according to their nature, they operate modulo a power of two, so
   the limit on their input is best choosen to be a power of 2.
   If you do not declare a limit here, an appropriate value is derived from
   DECPRECISION. If you change this value, make sure 2^LOGICRANGE
   is less than 10^DECPRECISION */
 #define LOGICRANGE 256

/***************************************************************************

                      END OF USER SETABLE DEFINES

***************************************************************************/

/* the limit of the math library */
#define MATHPRECISION 100

#if defined(_FLOATNUMTEST)
#  undef MAXDIGITS
#  undef MATHPRECISION
#  undef DECPRECISION
#  undef LOGICRANGE
#  define _USEMAXSCALEVAR
#  define MAXDIGITS maxscale
#  define MATHPRECISION 130
#  define LOGICRANGE 96
#endif

#ifndef BITS_IN_EXP
/* we need 4 extra bits during conversion, so that the exponent
   does not overflow while computing a base 2 expression */
# define BITS_IN_EXP (sizeof(int)*8-4)
#endif

/* necessary width of an integer to hold all possible
   exponents after a conversion to another base */
#define BITS_IN_HEXEXP   ((BITS_IN_EXP*8305)/10000 + 1)
#define BITS_IN_OCTEXP   ((BITS_IN_EXP*11073)/10000 + 1)
#define BITS_IN_BINEXP   ((BITS_IN_EXP*33219)/10000 + 1)

#ifndef MAXDIGITS
#  define MAXDIGITS 500
#endif /* MAXDIGITS */

#ifndef EXPMAX
#  define EXPMAX ((1 << (BITS_IN_EXP)) - 1)
#endif /* EXPMAX */

#define EXPMIN (-EXPMAX - 1)

#define EXPZERO ((int)((-1) << (sizeof(int)*8-1)))
#define EXPNAN ((int)(~EXPZERO))

#ifndef DECPRECISION
  #define DECPRECISION MATHPRECISION
#endif
#define BINPRECISION ((33219*DECPRECISION)/10000 + 1)
#define OCTPRECISION ((11073*DECPRECISION)/10000 + 1)
#define HEXPRECISION ((8305*DECPRECISION)/10000 + 1)

#ifndef LOGICRANGE
# define LOGICRANGE (16*((BINPRECISION-2)/16))
#endif

#endif /* _FLOATCONFIG_H */
