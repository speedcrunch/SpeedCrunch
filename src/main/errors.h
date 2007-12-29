/* errors.h global list of error codes
   Copyright (C) 2007 Wolf Lammen ookami1 <at> gmx <dot> de

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
 */

/*======================   math engine   ==========================*/
/* FIXME convert this into an enum with continuous values */

#define FLOAT_SUCCESS       0

/* a NaN was submitted as a parameter. Only a few functions in
   the math engine accept such a value. All arithmetic functions
   fail on such an operand */
#define FLOAT_NANOPERAND    1

/* This error is returned if a result is mathematically defined, but
   cannot be computed reliably without extending the working precision
   considerably and/or requiring considerable computing time */
#define FLOAT_UNSTABLE      2

/* the result is in absolute value smaller than the smallest non-zero
   value the math engine can produce */
#define FLOAT_UNDERFLOW     3

/* the result is in absolute value bigger than the biggest number
   the math engine can produce */
#define FLOAT_OVERFLOW      4

/* operation requests a division by zero, or a function was evaluated
   at a pole */
#define FLOAT_ZERODIVIDE    5

/* One or more parameters to a function lie outside of the function's
   domain */
#define FLOAT_OUTOFDOMAIN   6

/* This error indicates a failed conversion from an ASCII string */
#define FLOAT_BADLITERAL    7

/* A parameter exceeds the bounds of the engine, or is completely
   meaningless, like the request to evaluate something to 0 decimal
   places. This error indicates a bug, because the calling program
   should never submit such a parameter */
#define FLOAT_INVALIDPARAM 10

/*---------------   low level ASCII conversion errors   ------------------*/
#define IO_NO_ERROR            0

/* For correct conversion of a digit sequence, the IO routines need information
   about the radix to use. This error is returned if none was specified.
   This error indicates a bug, because a calling routine should always
   supply this information */
#define IO_ERROR_NO_BASE       20

/* This error occurs if you request a two's complement conversion, and either
   additionally specify a sign, or gave a non-zero fraction */
#define IO_ERROR_SIGN_CONFLICT 21

/* You must specify at least one digit of the significant */
#define IO_ERROR_NO_DIGIT      22

/* unknown characters in exponent, e.g. a two's complement tag */
#define IO_ERROR_EXP           23

/* the exponent exceeds the possible range */
#define IO_ERROR_EXP_RANGE     24

/*---------------  high level ASCII conversion errors   ------------*/

/* internal buffer overflow. This indicates a bug, because the high
   level conversion routines should set up a proper limit for the
   low level routines */
#define IO_BUFFER_OVERFLOW     30

//=============================   hmath   ============================*/

/* returned when an integer result cannot be represented without
   loss of digits */
#define HMATH_INTEGER_OVERFLOW 40
