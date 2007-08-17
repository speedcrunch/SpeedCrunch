/* floatnum.h: Arbitrary precision floating point numbers header file. */
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
#ifndef _FLOATNUM_H_
#define _FLOATNUM_H_

#include "number.h"
#include "floatconfig.h"
#include "floatio.h"

#define NULLTERMINATED (-20)
#define UNORDERED (-2)

#define EXACT (-101)
#define INTQUOT (-102)

#define FLOAT_SUCCESS      0
#define FLOAT_NANOPERAND   1
#define FLOAT_OVERFLOW     2
#define FLOAT_UNDERFLOW    3
#define FLOAT_ZERODIVIDE   4
#define FLOAT_OUTOFDOMAIN  5
#define FLOAT_UNSTABLE     6
#define FLOAT_INVALIDPARAM 100

#define float_free(f) float_setnan(f)

#ifdef __cplusplus
extern "C" {
#endif 

#ifdef _USEMAXSCALEVAR
extern int maxscale;
#endif /* _USEMAXSCALEVAR */

extern int float_error;

typedef struct {
  bc_num significand;
  int exponent;
#ifdef FLOATDEBUG
  char value[110];
#endif /* FLOATDEBUG */
} floatstruct;

typedef floatstruct* floatnum;

typedef enum {TONEAREST, TOZERO, TOINFINITY, TOPLUSINFINITY, TOMINUSINFINITY} roundmode;

/* initializes this module. Has to be called prior to the first
   use of any of the following functions */
void floatnum_init();

/* gets the last error and clears the error afterwards */
int float_geterror();

/* returns the current overflow limit. It is the maximum possible
   exponent. The smallest exponent is <return value> - 1.
   This function never reports an error */
int float_getrange();

/* sets the overflow/underflow limit. Results with exponents between
   <maxexp> >= exponent >= -<maxexp>-1 are valid, all others trigger an
   overflow/underflow error.
   <maxexp> cannot be greater than MAXEXP and not less than 1.
   The return value is the old overflow limit.
   This function effects future results only. Current stored values are
   not subject to overflow/underflow checking, even when they are used as
   parameters to an operation.
   This function never reports an error */
int float_setrange(int maxexp);

/* checks whether the submitted exponent is within the current overflow and
   underflow limits.
   This function never reports an error */
char float_isvalidexp(int exp);

/* initializes a new floatnum to NaN. Call this before
   the first use of a floatnum variable.
   The destructing function is float_setnan or its alias, float_free.
   This function never reports an error. */
void float_create(floatnum f);

/* finalizes a variable. To avoid memory leaks, call this before a floatnum
   is freed. A special value (NaN = not a number) is loaded into f, so
   any subsequent arithmetic operation on this variable will fail.
   However, a variable such finalized can still be re-used without prior
   initialization, by making it the destination of an operation.
   If you wish to deliberately "empty" a variable, without detroying it,
   call this function.
   An alias "float_free" to this function is defined that you
   may use anywhere as a replacement for float_setnan.
   This function never reports an error. */
void float_setnan(floatnum f);

/* returns the base 10 exponent of the value in <f>. If <f> is zero or
   NaN, the returned exponent is 0.
   This function never reports an error. */
int float_getexponent(floatnum f);

/* fills the buffer <buf> of size <bufsz> with an ASCII string
   representing the significand of <f>.
   No zeros are padded to the right to fill the buffer in case
   of a short significand.
   If <bufsz> <= 0, the function returns immediately with result 0.
   If the significand does not fit completely into the buffer,
   the output is stopped when the last buffer byte is written to.
   A non-zero significand yields a sequence of digits, without
   a decimal point; zero yields "0" and NaN "N".
   On truncation, no trailing zeros are removed.
   Exponent and sign are ignored, and no decimal point is written.
   No '\0' character is appended to the right.
   The return value are the number of characters written to the buffer.
   This function never reports an error. */
int float_getsignificand(char* buf, int bufsz, floatnum f);

/* returns the number of digits in the significand, or 0 for NaN and zero.
   This function never reports an error. */
int float_getlength(floatnum f);

/* returns 1, if <f> is positive, -1, if <f> is negative, and 0 if <f> is
   zero or NaN.
   This function never reports an error. */
signed char float_getsign(floatnum f);

/* writes the value stored in <f> as an ASCIIZ string into a buffer of
   size <bufsz>. If the buffer is too small to hold the full value,
   the significand is truncated appropriately. If the buffer is too small
   to even store the most significant digit of the significand besides
   exponent and sign, nothing is stored at all, and -1 is returned.
   Else, the length of the output, but without the trailing \0
   character, is returned.
   The output format is the usual scientific format. On truncation,
   no trailing zeros are removed from the output of the significand.
   Examples of an output:

   value  output
   -----  ------
   NaN    NaN\0
   0      0\0
   1      1.e0\0
   1.2    1.2e0\0
   0.5    5.e-1\0
   -1     -1.e0\0
   1.009  1.00e0\0 (truncated to 3 digits)
   10^10  1.e10\0

   This function does not touch the adjacent bytes of the final
   output. If no reasonable output is possible, the complete buffer
   is left unchanged, not even the final \0 character is written.
   This function never reports an error. */
int float_getscientific(char* buf, int bufsz, floatnum f);

/* gets the <ofs>-th digit from the decimal representation
   of the significand of <f>. If <ofs> is negative or greater
   equal to the length of the significand, 0 is returned.
   The return value is between 0 and 9 (not the ASCII representation
   of these digits (0x30 - 0x39)).
   This function never returns an error */
char float_getdigit(floatnum f, int ofs);

/* sets the significand according to the the ASCII text in buffer buf
   of size <bufsz>.
   The buffer must contain digits '0' -'9' only, with one possible
   exception: A single decimal point ('.') may be placed anywhere in
   the buffer. It is skipped while encoding the significand.
   The function searches the buffer for the first non-zero digit,
   and starts the encoding from there.
   <f> is set to NaN, if the buffer fails to fulfill the above conditions.
   If the resulting significand exceeds MAXDIGITS digits, it is truncated.
   The exponent of <f> is set to 0, so the result is always NaN, 0 or a
   number between 1 and 9.99...
   If you want to set both the significand and the exponent of f,
   set the significand first.
   If <leadingzeros> is not NULL, and the result in <f> is neither zero nor
   NaN, the number of leading (and skipped) zeros are stored here.
   In case of NaN or zero, this value is 0.
   All trailing zeros in the significand of <f> are removed.
   The result is the position of the decimal point in <buf>, or -1
   if either none was found, or if it is not relevant (NaN or 0.0).
   This function never reports an error. */
int float_setsignificand(floatnum f, int* leadingzeros, const char* buf,
                         int bufsz);

/* sets the base 10 exponent of f to exponent. The significand is
   only changed when this operation fails.
   Integers greater than EXPMAX or smaller than EXPMIN are not accepted as
   exponent and let the operation fail, setting <f> to NaN.
   You cannot change the exponent of 0 or NaN, this is ignored.
   If you want to set both the significand and the exponent of <f>,
   set the significand first.
   This function never reports an error. */
void float_setexponent(floatnum f, int exponent);

/* converts an ASCII string of length <bufsz> in <buf> to <f>.
   <bufsz> may assume the special value NULLTERMINATED, in which case
   the first found \0 character terminates the input.
   The input format is
   [+|-][digit...][.[digit...][(e|E)[+|-]digit...]
   At least one digit of the significand has to be present.
   Any non-valid input is converted into a NaN.
   This function never reports an error. */
void float_setscientific(floatnum f, const char* buf, int bufsz);

/* if <s> is 1 or -1, the sign of <f> is set accordingly. Has no effect, if
   <f> == NaN or zero, or <s> == 0. f is set to NaN, if abs(s) > 1.
   This function never reports an error. */
void float_setsign(floatnum f, signed char s);

/* sets dest to the value in <value>. <value> is usually not copied,
   but used directly. That is why this function tries to assume
   ownership of <value>. Only, if it can't, because n_refs > 1, a copy
   is created. If <value> == NULL, NaN is the result. In any case, <value>
   is freed once, either reducing the reference count or deallocating
   the number completely.
   On success, dest contains a value numerically equal to the
   original value in <value>.
   This function is the only way to pass more than MAXSCALE
   digits to a floatnum. There is no sense in doing so,
   because almost all operations obey this limit and cut
   the size of the result down. However, there are operations,
   e.g. float_compare, that accept overly long operands and work
   with them. The unwary may see unexpected results then.
   This function never reports an error. */
void float_setbcnum(floatnum dest, bc_num* value);

/* sets dest to the value in <value>.
   This function never reports an error. */
void float_setinteger(floatnum dest, int value);

/* sets a variable to the numerical value zero.
   This function never reports an error. */
void float_setzero (floatnum f);

/* returns 1, if f contains the special NaN value, 0 otherwise.
   This function never reports an error. */
char float_isnan(floatnum f);

/* returns 1, if f contains the value zero, 0 otherwise.
   This function never reports an error. */
char float_iszero(floatnum f);

/* changes the value of <f> to -<f>. Has no effect, if, <f> is zero or NaN.
   A return value of 0 indicates an error.
   errors: FLOAT_NANOPERAND  */
char float_changesign(floatnum f);

/* changes the sign of <f>, if <f> is negative. Has no effect on a NaN.
   A return value of 0 indicates an error.
   errors: FLOAT_NANOPERAND */
char float_abs(floatnum f);

/* compares two values and returns +1 if val1 > val2, 0 if val1 == val2
   and -1 if val1 < val2.
   This function is not intended to be used with NaN's. If you
   pass them as an argument, UNORDERED is returned to indicate an error.
   errors: FLOAT_NANOPERAND */
signed char float_cmp(floatnum val1, floatnum val2);

/* rounds <f> to <digits> digits according to the submitted mode.
   If <digits> <= 0 or mode is not recognized, <f> is changed into a NaN.
   The same holds should the rounding operation overflow.
   mode == TONEAREST: checks whether the first cut off digit
                      is a '5' or greater. In this case, the
                      absolute value of the significand is rounded up,
                      otherwise rounded down. If the part cut off
                      is a single digit '5', the significand is
                      rounded such that the last digit is even.
   mode == TOZERO: cuts off all digits after the <digits>th digit.
                   This mode never overflows.
   mode = TOINFINITY: rounds positive numbers up, negative numbers down
                      (towards greater magnitude).
   mode == TOPLUSINFINITY: always rounds up. So, negative values
                           are effectively truncated.
   mode == TOMINUSINFINITY: always rounds down. So, negative values
                            usually increase in magnitude.
   A return value of 0 indicates an error.
   errors: FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM
           FLOAT_OVERFLOW */
char float_round(floatnum f, int digits, roundmode mode);

/* cuts off the fractional part of <f>. The result is always less
   or equal in magnitude to the passed argument.
   NaN is returned only if the argument is NaN.
   A return value of 0 indicates an error.
   errors: FLOAT_NANOPERAND */
char float_int(floatnum f);

/* cuts off the integer part of <f>. If the result is not equal to 0,
   it has the same sign as the argument.
   NaN is returned only, if the argument is NaN.
   A return value of 0 indicates an error.
   errors: FLOAT_NANOPERAND */
char float_frac(floatnum f);

/* copies source to dest, limiting the significand to at most <digits> digits.
   The parameter <digits> may assume the value EXACT, in which case a full
   copy is made.
   If source and dest coincide, float_clone tries to re-use the significand.
   This prevents unnecessary copying.
   If a copy has to be made, the allocated space is just big enough to hold
   the significand, so no memory is wasted.
   float_clone is not subject to the MAXSCALE limit.
   A return value of 0 indicates an error.
   errors: FLOAT_INVALIDPARAM */
char float_clone(floatnum dest, floatnum source, int digits);

/* transfers the contents of source to dest. source is assigned NaN
   afterwards.
   In contrast to float_clone, float_move does not create a copy of
   the significand (which employs memory allocation and copying),
   instead it transfers simply the data from the source to the destination.
   This function has been designed to implement, for example, swapping
   of variables in a fast way.
   If dest == source, nothing happens.
   This function never reports an error */
void float_move(floatnum dest, floatnum source);

/* adds the values in <summand1> and <summand2> and stores the result in
   <dest>. <dest> may coincide with either summand (or even both).
   The result is evaluated to <digits> or <digits>+1 digits. If <digits>
   is EXACT, the sum is evaluated to full scale (if possible).
   NaN is returned, if
   - (at least) one operand is NaN;
   - the result overflows or underflows;
   - <digits> is invalid, or the resulting digits exceed MAXSCALE.
   A return value of 0 indicates an error.
   errors: FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM
           FLOAT_OVERFLOW
           FLOAT_UNDERFLOW */
char float_add(floatnum dest, floatnum summand1, floatnum summand2,
  int digits);

/* subtracts <subtrahend> from <minuend> and stores the result in
   <dest>. <dest> may coincide with either operand (or even both).
   The result is evaluated to <digits> or <digits>+1 digits. If <digits>
   is EXACT, the difference is evaluated to full scale (if possible).
   NaN is returned, if
   - (at least) one operand is NaN;
   - the result overflows or underflows;
   - <digits> is invalid, or the resulting digits exceed MAXSCALE.
   A return value of 0 indicates an error.
   errors: FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM
           FLOAT_OVERFLOW
           FLOAT_UNDERFLOW */
char float_sub(floatnum dest, floatnum minuend, floatnum subtrahend,
  int digits);

/* multiplies both factors and stores the result in <dest>. <dest> may
   coincide with either factor (or even both). The result is
   evaluated to <digits> or <digits>+1 digits, or, if <digits> ==
   EXACT, to full scale (if possible).
   NaN is returned, if
   - (at least) one operand is NaN;
   - the result overflows or underflows;
   - <digits> is invalid, or the resulting scale exceeds MAXSCALE.
   A return value of 0 indicates an error.
   errors: FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM
           FLOAT_OVERFLOW
           FLOAT_UNDERFLOW */
char float_mul(floatnum dest, floatnum factor1, floatnum factor2,
  int digits);

/* divides <dividend> by <divisor> and stores the result in <dest>. <dest>
   may coincide with either operand (or even both). The result is
   evaluated to <digits> or <digits>+1 digits, or, if <digits> == INTQUOT,
   to the size of the integer part of the quotient.
   EXACT is not allowed, even in cases where the dividend is divisible
   by the divisor.
   NaN is returned, if
   - (at least) one operand is NaN;
   - the result overflows or underflows;
   - the divisor is zero;
   - <digits> is invalid, or the resulting scale exceeds MAXSCALE.
   A return value of 0 indicates an error.
   errors: FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM
           FLOAT_OVERFLOW
           FLOAT_UNDERFLOW
           FLOAT_ZERODIVIDE */
char float_div(floatnum dest, floatnum dividend, floatnum divisor,
  int digits);

/* evaluates the quotient, using <digits> steps of the schoolbook
   division algorithm. The quotient, thus, has <digits> or <digits>-1 digits,
   and is always truncated towards zero.
   The remainder fulfills the equation:
     remainder = dividend - quotient * divisor.
   <digits> may assume the special value INTQUOT, in which case the integer
   part of the quotient is calculated.
   This function is an exact operation anyway, so EXACT is not allowed
   here.
   <digits> is subject to the MAXSCALE limit.
   <remainder> and <quotient> have to be different variables, but besides
   this, there are no other restrictions on the passed variables.
   If this function fails, both result variables are set to NaN.
   A return value of 0 indicates an error.
   errors: FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM
           FLOAT_OVERFLOW
           FLOAT_UNDERFLOW
           FLOAT_ZERODIVIDE */
char float_divmod(floatnum quotient, floatnum remainder, floatnum dividend,
  floatnum divisor, int digits);

/* computes the sqare root of <value> to <digits> or <digits>+1 digits.
   <digits> == EXACT is not allowed, even if the argument is a square.
   NaN is returned, if
   - the operand is NaN,
   - <digits> exceed MAXSCALE
   - the operand is negative.
   return value == 0 indicates an error.
   errors: FLOAT_NANOPERAND
           FLOAT_INVALIDPARAM
           FLOAT_OUTOFDOMAIN */
char float_sqrt(floatnum value, int digits);

#ifdef __cplusplus
}
#endif 

#endif /* _FLOATNUM_H_ */
