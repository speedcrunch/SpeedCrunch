/* errors.h global list of error codes
   Copyright (C) 2007, 2008 Wolf Lammen ookami1 <at> gmx <dot> de

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

#ifndef _ERRORS_H
#define _ERRORS_H

#ifdef __cplusplus
extern "C"{
#endif

typedef enum
{
  Success,

/* a NaN or an empty Variant was submitted as a parameter. Only a few
  functions in the math engine accept such a value. All arithmetic functions
  fail on such an operand */
  NoOperand,

/* This error is returned if a result is mathematically defined, but
  cannot be computed reliably without extending the working precision
  considerably and/or requiring considerable computing time */
  EvalUnstable,

/* the result is in absolute value smaller than the smallest non-zero
  value the math engine can handle */
  Underflow,

/* the result is in absolute value bigger than the biggest number
  the math engine can handle */
  Overflow,

/* operation requests a division by zero, or a function was evaluated
  at a pole */
  ZeroDivide,

/* One or more parameters to a function lie outside of the function's
  domain */
  OutOfDomain,

/* a number exceeds the logic number range, so logic operations cannot be
  applied */
  OutOfLogicRange,

/* a number or a result exceeds the integer range */
  OutOfIntegerRange,

/* This error indicates a failed conversion from an ASCII string.
   Functions setting this error may report a more detailed IO... error code,
   too */
  BadLiteral,

/* A request to calculate something to more places than is (currently)
   acceptable */
  InvalidPrecision,

/* A parameter violates the limitations of the engine, or is completely
  meaningless, e.g. the evaluation of a quotient and a remainder in the same variable.
  This error indicates a bug, because the calling program should never submit
  such a parameter (combinations) */
  InvalidParam,

/* returned when an operation request is mathematically valid, but
  would require too much time */
  TooExpensive,

/* For correct conversion of a digit sequence, the IO routines need information
  about the radix to use. This error is returned if none was specified.
  This error indicates a bug, because a calling routine should always
  supply this information */
  IONoBase,

/* This error occurs if you request a two's complement conversion, and either
  additionally specify a sign, or gave a non-zero fraction */
  IOInvalidComplement,

/* You must specify at least one digit of the significant */
  IONoSignificand,

/* invalid characters in exponent, e.g. a decimal dot */
  IOBadExp,

/* the exponent exceeds the allowed range */
  IOExpOverflow,

/* internal (string) buffer overflow in a conversion. This indicates a bug
  because range checking should be done in advance */
  IOBufferOverflow,

/* request to convert more digits to another base than internal buffers
  can hold. This occurs when you try to convert huge values in fixpoint
  format */
  IOConversionOverflow,

/* request to convert a tiny number in fixpoint format, so that only
  leading zeros are displayed. */
  IOConversionUnderflow,

/* a function was called with the wrong count of parameters
  e.g. sin(12;13) (sin takes only 1 parameter) */
  InvalidParamCount,

/* parameter type mismatch */
  TypeMismatch,

/* cannot overwrite an existing key in a table */
  KeyExists,

/* could not retrieve a symbol by the submitted key */
  SymbolNotFound,

/* no matching close token for an open token */
  CloseSymbolMissing,

/* unable to clone a symbol, most probably because it
   was of a special type, like a close parenthesis */
  SymbolCloneError,

/* unable to perform a requested type cast */
  BadCast,

/* used with variants, when an operation is not implemented
  for a particular data type */
  NotImplemented,

/* this value is used internally to indicate the absence of
  any error information altogether */
  NotAnError,
} Error;

#ifdef __cplusplus
}
#endif

#endif /* _ERRORS_H */
