/* floatnum.c: Arbitrary precision floating point numbers, based on bc. */
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

/* a floating point engine based on bc's decimal
   fix point arithmetic. The speed is not overwhelming,
   but sufficient for calculators with limited demands.
   As bc's number.c is a portable engine, this should be
   portable as well.
*/

#include "floatnum.h"
#include "floatlong.h"
#include <base/errors.h>
#include <stdio.h>
#include <string.h>

#define NOSPECIALVALUE 1

int maxdigits = MAXDIGITS;

Error float_error;
int expmax = EXPMAX;
int expmin = EXPMIN;

/*  general helper routines  */

static int
_max(int x, int y)
{
  return x > y? x : y;
}

static int
_min(int x, int y)
{
  return x < y? x : y;
}

/* the return value points to the first character different
   from accept. */
const char*
_memskip(
  const char* buf,
  const char* end,
  char accept)
{
  for(--buf; ++buf != end && *buf == accept;);
  return buf;
}

/* scans a value in a bc-num (or part of it) for the first
   occurence of a digit *different* from <digit>. The scan
   is limited to <count> bytes. Returns the offset of the
   matching byte, or <count>, if none was found */
static int
_scan_digit(
  const char*p,
  int count,
  char digit)
{
  const char* ps;

  ps = p;
  for (; count-- > 0 && *p == digit; ++p);
  return p - ps;
}

/*  bc_num primitives  */

#define _scaleof(f) (f->significand->n_scale)
#define _lenof(f) (f->significand->n_len)
#define _valueof(f) (f->significand->n_value)
#define _digit(f, offset) (*(_valueof(f) + offset))
#define _setscale(f, value) (_scaleof(f) = value)

/* converts floatnum's sign encodings (+1, -1) to bc_num
   sign encoding (PLUS, MINUS) */
static void
_setsign(
  floatnum f,
  signed char value)
{
  f->significand->n_sign = value < 0? MINUS : PLUS;
#ifdef FLOATDEBUG
  f->value[0] = value < 0? '-' : '+';
#endif
}

/* modifies a bc_num significand such that the last <count>
   (existing) digits of its value are not visible
   to bc_num operations any more.
   A negative <count> reverts this operation (unhide).
   pre: <count> <= n_scale*/
static void
_hidelast(
  floatnum f,
  int count)
{
  f->significand->n_scale -= count;
}

/* modifies a bc_num significand such that the first <count>
   (existing) digits of its value are not visible
   to bc_num operations any more.
   A negative <count> reverts this operation (unhide).
   pre: <count> <= n_scale*/
static void
_hidefirst(
  floatnum f,
  int count)
{
  f->significand->n_value += count;
  f->significand->n_scale -= count;
}

/* Usually, significands are normalized, which means they
   fulfill 1 <= x < 10. This operation moves the decimal
   point <count> places to the right, effectively multiplying
   the significand by a power of 10. A negative <count>
   reverts such an operation.
   pre: <count> < n_scale */
static void
_movepoint(
  floatnum f,
  int digits)
{
  f->significand->n_len += digits;
  f->significand->n_scale -= digits;
}

/*  floatstruct primitives  */

/* a quick check for NaN and 0 */
static char
_is_special(
  cfloatnum f)
{
  return f->significand == NULL;
}

/* creates a shallow working copy of <source> in <dest>,
   using b as a container for the significand.
   On return, <dest> is equal in value to <source>.
   <dest> may then be modified freely in the course
   of an operation, without effecting source,
   *except* that the digits in *(dest->significand->n_value)
   have to be retained (or restored).
   <dest> must *never* be the destination of a
   float_xxx operation, in particiular, it must not be
   freed. Neither must b be modified (or freed) in a bc_num
   operation */
static void
_copyfn(
  floatnum dest,
  cfloatnum source,
  bc_num b)
{
  *dest = *source;
  if (!_is_special(source))
  {
    *b = *(source->significand);
    b->n_ptr = 0;
    dest->significand = b;
  }
}

/* If you want to execute a bc_num operation to a limited scale,
   it is a waste of computation time to pass operands
   with a longer scale, because bc lets the operand's
   scale override your limit. This function hides superfluous
   digits from bc, returning the original scale for restoring
   purposes */
static int
_limit_scale(
  floatnum f,
  int newscale)
{
  int oldscale;
  oldscale = _scaleof(f);
  _setscale(f, _min(oldscale, newscale));
  return oldscale;
}

/*============================   floatnum routines  ===================*/

int
float_getrange()
{
  return expmax;
}

int
float_getprecision()
{
  return maxdigits;
}

int
float_setrange(
  int maxexp)
{
  int result;

  result = expmax;
  expmax = _max(_min(maxexp, MAXEXP), 1);
  expmin = -expmax - 1;
  return result;
}

int
float_setprecision(
  int digits)
{
  int result;

  result = maxdigits;
  maxdigits = _max(_min(digits, MAXDIGITS), 1);
  return result;
}

/* checking the limits on exponents */
char
float_isvalidexp(
  int exp)
{
  return exp >= expmin && exp <= expmax;
}

/* clears the error state as well */
Error
float_geterror()
{
  Error tmp;

  tmp = float_error;
  float_error = Success;
  return tmp;
}

/* the first error blocks all others as it may be the source
   of a cascade of dependent errors */
void
float_seterror(
  Error code)
{
  if (float_error == Success)
    float_error = code;
}

void
floatnum_init()
{
  bc_init_numbers();
  float_geterror();
}

void
float_create(
  floatnum f)
{
  f->significand = NULL;
  f->exponent = EXPNAN;
#ifdef FLOATDEBUG
  memcpy(f->value, "NaN", 4);
#endif
}

void
float_setnan (
  floatnum f)
{
  bc_free_num(&(f->significand));
  float_create(f);
}

char
_setnan(
  floatnum result)
{
  float_setnan(result);
  return FALSE;
}

char
_seterror(
  floatnum result,
  Error code)
{
  float_seterror(code);
  return _setnan(result);
}

void
float_setzero (
  floatnum f)
{
  bc_free_num(&(f->significand));
  f->exponent = EXPZERO;
#ifdef FLOATDEBUG
  f->value[0] ='0';
  f->value[1] = 0;
#endif
}

char
_setzero(
  floatnum result)
{
  float_setzero(result);
  return TRUE;
}

char
float_isnan(
  cfloatnum f)
{
  return _is_special(f) && f->exponent != EXPZERO;
}

char
float_iszero(
  cfloatnum f)
{
  return _is_special(f) && f->exponent == EXPZERO;
}

int
float_getlength(
  cfloatnum f)
{
  return _is_special(f)? 0 : _scaleof(f) + 1;
}

char
float_getdigit(
  cfloatnum f,
  int ofs)
{
  if (ofs >= float_getlength(f) || ofs < 0)
    return 0;
  return _digit(f, ofs);
}

/* checks whether f is a NaN and sets the float_error
   variable accordingly. Used in parameter checks when
   float_xxx calls are executed.
   FALSE is returned if a NaN is encountered. */
char
_checknan(
  cfloatnum f)
{
  if (!float_isnan(f))
    return TRUE;
  float_seterror(NoOperand);
  return FALSE;
}

/* checks whether <digits> is positive and
   sets the float_error variable accordingly. Used in
   parameter checks when float_xxx calls are executed.
   Some operations accept a special value like EXACT,
   that has to pass this check, even though its numerical
   encoding violates the boundaries.
   If a function does not accept a special value,
   use NOSPECIALVALUE as a parameter for <specialval>.
   TRUE is returned if the check is passed.
   The check for the limit MAXDIGITS is not executed
   here, because some intermediate operations have to succeed
   on more than MAXDIGITS digits */
static char
_checkdigits(
  int digits,
  int specialval)
{
  if ((digits > 0 && digits <= maxdigits) || digits == specialval)
    return TRUE;
  float_seterror(InvalidPrecision);
  return FALSE;
}

/* backward-scans the significand in <f>. Returns the number of
   digits equal to <digit> beginning with the <scale>+1-th digit of
   f->significand->n_value. */
static int
_bscandigit(
  cfloatnum f,
  int scale,
  char digit)
{
  char* p;
  char* ps;

  ps = _valueof(f);
  for (p = ps + scale + 1; p-- != ps && *p == digit;);
  return scale - (p - ps);
}

/* scans two significands for the first occurence
   of a pair of differnt digits. Returns the number
   of equal digits at the beginning */
static int
_scan_equal(
  floatnum v1,
  floatnum v2)
{
  int count, i;
  char* p1;
  char* p2;

  count = _min(_scaleof(v1), _scaleof(v2));
  p1 = _valueof(v1);
  p2 = _valueof(v2);
  i = 0;
  for (; *(p1++) == *(p2++) && ++i <= count;);
  return i;
}

/* scans two significands until it finds a digit different
   from 0 in the first significand, or a digit different from
   9 in the second operand. The scan is limited by <count>
   compares and starts with the *second* digit in the
   significands. Returns the number of found (0,9) pairs. */
static int
_scan_09pairs(
  floatnum f1,
  floatnum f2,
  int count)
{
  char* p;
  char* p1;
  char* p2;

  p1 = _valueof(f1) + 1;
  p2 = _valueof(f2) + 1;
  p = p1;
  for (; count-- > 0 && *p1 == 0 && *(p2++) == 9; ++p1);
  return p1 - p;
}

signed char
float_getsign(
  cfloatnum f)
{
  if(_is_special(f))
    return 0;
  return f->significand->n_sign == PLUS? 1 : -1;
}

void
float_setsign(
  floatnum f,
  signed char s)
{
  if (s == 1 || s == -1)
  {
    if(!_is_special(f))
      _setsign(f, s);
  }
  else if (s != 0)
    float_setnan(f);
}

char
float_neg(
  floatnum f)
{
  float_setsign(f, -float_getsign(f));
  return _checknan(f);
}

char
float_abs(
  floatnum f)
{
  if(float_getsign(f) == -1)
    float_neg(f);
  return _checknan(f);
}

signed char
float_cmp(
  cfloatnum val1,
  cfloatnum val2)
{
  signed char sgn1;

  if (!_checknan(val1) || !_checknan(val2))
    return UNORDERED;
  sgn1 = float_getsign(val1);
  if (float_getsign(val2) != sgn1)
  {
    if (sgn1 != 0)
      return sgn1;
    return -float_getsign(val2);
  }
  if (val1->exponent > val2->exponent)
    return sgn1;
  if (val1->exponent < val2->exponent)
    return -sgn1;
  if (_is_special(val1))
    return 0;
  return (bc_compare(val1->significand, val2->significand));
}

/* normalizing process:
   hides leading zeros in a significand and corrects the
   exponent accordingly */
static void
_corr_lead_zero(
  floatnum f)
{
  int count;

  count = _scan_digit(_valueof(f), float_getlength(f), 0);
  _hidefirst(f, count);
  f->exponent-=count;
}

/* normalizing process:
   if the significand is > 10 in magnitude, this function
   corrects this */
static void
_corr_overflow(
  floatnum f)
{
  int shift;

  shift = _lenof(f) - 1;
  _movepoint(f, -shift);
  f->exponent += shift;
}

/* cuts off trailing zeros at the end of a significand */
static void
_corr_trailing_zeros(
  floatnum f)
{
  _hidelast(f, _bscandigit(f, _scaleof(f), 0));
}

static char hexdigits[] = "0123456789ABCDEF";

int
float_getsignificand(
  char* buf,
  int bufsz,
  cfloatnum f)
{
  int idx, lg;

  if (bufsz <= 0)
    return 0;
  if (float_isnan(f))
  {
    *buf = 'N';
    return 1;
  }
  if (float_iszero(f))
  {
    *buf = '0';
    return 1;
  }
  idx = -1;
  lg = _min(bufsz, float_getlength(f));
  for(; ++idx < lg;)
    *(buf++) = hexdigits[(int)float_getdigit(f, idx)];
  return lg;
}

int
float_getexponent(
  cfloatnum f)
{
  if (_is_special(f))
    return 0;
  return f->exponent;
}

int float_getscientific(
  char* buf,
  int bufsz,
  cfloatnum f)
{
  char b[42]; /* supports exponents encoded in up to 128 bits */
  int sgnlg, explg, mlg;

  /* handle special cases */
  if(float_isnan(f))
  {
    if (bufsz < 4)
      return -1;
    memcpy(buf, "NaN\0", 4);
    return 3;
  }
  if(float_iszero(f))
  {
    if (bufsz < 2)
      return -1;
    *buf = '0';
    *(buf+1) = '\0';
    return 1;
  }

  /* set one byte aside for sign? */
  sgnlg = 0;
  if(float_getsign(f) < 0)
    sgnlg = 1;

  /* convert the exponent */
  sprintf(b, "%d", float_getexponent(f));
  explg = strlen(b);

  /* 3 extra bytes for dot, exp char and terminating \0 */
  bufsz -= explg + sgnlg + 3; /* rest is for significand */
  if (bufsz <= 0)
    /* buffer too small */
    return-1;

  if(sgnlg > 0)
    *(buf++) = '-';

  /* get the digit sequence of the significand, trailing zeros cut off */
  mlg = float_getsignificand(++buf, bufsz, f) - 1;

  /* move the first digit one byte to the front and fill
    the gap with a dot */
  *(buf-1) = *buf;
  *(buf++) = '.';

  /* append the exponent */
  *(buf+mlg) = 'e';
  memcpy(buf+mlg+1, b, explg);

  /* the trailing \0 */
  *(buf+mlg+explg+1) = '\0';
  return sgnlg + mlg + explg + 3;
}

#ifdef FLOATDEBUG
void _setvalue_(floatnum f)
{
  f->value[float_getsignificand(f->value+2, sizeof(f->value)-3, f)+2] = 0;
  f->value[1] = f->value[2];
  f->value[2] = '.';
  f->value[0] = float_getsign(f) < 0? '-' : '+';
}
#endif

int
float_setsignificand(
  floatnum f,
  int* leadingzeros,
  const char* buf,
  int bufsz)
{
  const char* p;
  const char* dot;
  const char* last;
  const char* b;
  char* bcp;
  int zeros;
  int lg;
  char c;

  float_setnan(f);
  if (bufsz == NULLTERMINATED)
    bufsz = strlen(buf);

  /* initialize the output parameters for all
     early out branches */
  if (leadingzeros != NULL)
    *leadingzeros = 0;

  if (bufsz <= 0)
    return -1;

  dot = memchr(buf, '.', bufsz);
  /* do not accept more than 1 dots */
  if (dot != NULL && memchr(dot + 1, '.', bufsz - (dot - buf)) != NULL)
    return -1;

  last = buf + bufsz; /* points behind the input buffer */

  /* skip all leading zeros */
  b = _memskip(buf, last, '0');

  /* is the first non-zero character found a dot? */
  if (b == dot)
    /* then skip all zeros following the dot */
    b = _memskip(b+1, last, '0');

  /* the 'leading zeros' */
  zeros = b - buf - (dot == NULL || dot >= b? 0:1);

  /* only zeros found? */
  if (b == last)
  {
    /* indicate no dot, no leading zeros, because
       this does not matter in case of zero */
    if (bufsz > (dot == NULL? 0:1))
      float_setzero(f);
    /* do not accept a dot without any zero */
    return -1;
  }

  /* size of the rest buffer without leading zeros */
  bufsz -= b - buf;

  /* does the rest buffer contain a dot? */
  lg = dot >= b && dot - b < maxdigits? 1 : 0;

  /* points behind the last significant digit */
  p = b + _min(maxdigits + lg, bufsz);

  /* digits, limited by MAXDIGITS */
  lg = _min(maxdigits, bufsz - lg);

  /* reduce lg by the number of trailing zeros */
  for (; *--p == '0'; --lg);
  if (*(p--) == '.')
    for (; *(p--) == '0'; --lg);

  /* get a bc_num of sufficient size */
  f->significand = bc_new_num(1, lg - 1);
  if (f->significand == NULL)
    return -1;

  /* exponent is forced to 0 */
  f->exponent = 0;

  /* copy lg digits into bc_num buffer,
     scan the rest for invalid characters */
  bcp = _valueof(f);
  for(; --bufsz >= 0;)
  {
    c = *(b++);
    if (c != '.') /* ignore a dot */
    {
      if (c < '0' || c > '9')
      {
        /* invalid character */
        float_setnan(f);
        return -1;
      }
      if (--lg >= 0)
        *(bcp++) = c - '0';
    }
  }

  if (leadingzeros != NULL)
    *leadingzeros = zeros;
#ifdef FLOATDEBUG
  _setvalue_(f);
#endif
  return dot == NULL? -1 : dot - buf;
}

void
float_setexponent(
  floatnum f,
  int exponent)
{
  if (!_is_special(f))
  {
    if (!float_isvalidexp(exponent))
      float_setnan(f);
    else
      f->exponent = exponent;
  }
}

void
float_setscientific(
  floatnum f,
  const char* buf,
  int bufsz)
{
  int exppos;
  int zeros;
  int dotpos;
  unsigned exp, ovfl;
  signed char expsign, msign;
  const char* expptr;
  const char* last;
  char c;

  float_setnan(f);

  if (bufsz == NULLTERMINATED)
    bufsz = strlen(buf);

  /* find the offset of the exponent character,
     or -1, if not found */
  for(exppos = bufsz; --exppos >= 0;)
    if ((c = *(buf+exppos)) == 'E' || c == 'e')
      break;

  /* marks the end of the exponent string */
  last = buf + bufsz;

  /* pre-set exponent to +0, which is the right value,
     if there is no exponent. */
  exp = 0;
  expsign = 1;
  ovfl = 0;
  if (exppos >= 0)
  {
    /* points behind the exponent character */
    expptr = buf + (exppos + 1);
    if (expptr == last)
      /* do not accept an exponent char without an integer */
      return;

    /* get the exponent sign */
    switch(*expptr)
    {
    case '-':
      expsign = -1; /* and fall through */
    case '+':
      ++expptr;
    }
    if (expptr == last)
      /* do not accept a sign without a digit following */
      return;

    /* encode the sequence of digits into an unsignedeger */
    for (;expptr != last ;)
    {
      if (*expptr < '0' || *expptr > '9')
        /* invalid char encountered */
        return;
      ovfl = 10;
      if (_longmul(&exp, &ovfl))
      {
        ovfl = *(expptr++) - '0';
        _longadd(&exp, &ovfl);
      }
      if (ovfl != 0 || exp > EXPMAX+1)
      {
        /* do not return immediately, because the
           significand can be zero */
        ovfl = 1;
        break;
      }
    }
    /* move the last pointer to the exponent char.*/
    last = buf + exppos;
  }
  /* last points behind the significand part.
     exp is at most -EXPMIN */

  /* get the sign of the significand */
  msign = 1;
  if (buf != last)
    switch(*buf)
    {
    case '-':
      msign = -1; /* fall through */
    case '+':
      ++buf;
    }

  /* let setsignificand convert the sequence of digits
     into a significand. If a dot is found, its position
     is given in dotpos, -1 otherwise.
     zeros are the count of leading '0' digits before
     the first non_zero digit. */
  dotpos = float_setsignificand(f, &zeros, buf, last-buf);
  if (_is_special(f))
    /* setsignificand either found a zero or encountered
       invalid characters */
    return;

  /* if we did not find a dot, we assume an integer,
       and put the dot after last digit */
  if (dotpos == -1)
    dotpos = last - buf;

  /* leading zeros shift the dot to the left.
     dotpos is now the exponent that results
     from the position of the dot in the significand. */
  dotpos -= zeros+1;

    /* combine the dot position with the explicit exponent */
  if (ovfl != 0 || !_checkadd(&dotpos, expsign * (int)exp))
      /* exponent overflow */
    float_setnan(f);

  float_setexponent(f, dotpos);
  float_setsign(f, msign);
}

/* normalizes a significand such that 1 <= x < 10.
   If the exponent overflows during this operation
   this is notified. */
static char
_normalize(
  floatnum f)
{
  _corr_lead_zero(f);
  if (f->significand != NULL && _lenof(f) > 1)
    _corr_overflow(f);
  if (f->significand != NULL)
    _corr_trailing_zeros(f);
  if (f->significand != NULL && !float_isvalidexp(f->exponent))
  {
    float_seterror(Underflow);
    if (f->exponent > 0)
      float_seterror(Overflow);
    float_setnan(f);
  }
#ifdef FLOATDEBUG
  if (f->significand != NULL)
    _setvalue_(f);
#endif
  return f->significand != NULL;
}

void
float_setinteger(floatnum dest, int value)
{
  char buf[BITS_IN_UNSIGNED/3 + 1];

  sprintf(buf, "%d", value);
  float_setscientific(dest, buf, NULLTERMINATED);
}

void
float_move(
  floatnum dest,
  floatnum source)
{
  if (dest != source)
  {
    float_setnan(dest);
    *dest = *source;
    float_create(source);
  }
}

/* creates a copy of <source> and assigns it to
   <dest>. The significand is guaranteed to have
   <scale>+1 digits. The <dest> significand is
   truncated, or padded with zeros to the right,
   to achieve the desired length.
   <scale> may assume the special value EXACT, in
   which case a true copy is generated.
   This function allows an in-place copy
   (dest == source). */
static void
_scaled_clone(
  floatnum dest,
  cfloatnum source,
  int scale)
{
  /* dest == source allowed! */

  bc_num mant;
  unsigned exp;
  signed char sign;

  mant = NULL;
  if(scale == EXACT)
    scale = _scaleof(source);
  if (dest == source && scale <= _scaleof(source))
  {
    _setscale(dest, scale);
    return;
  }
  mant = bc_new_num(1, scale);
  scale = _min(scale, _scaleof(source));
  memcpy(mant->n_value, _valueof(source), scale+1);
  sign = float_getsign(source);
  exp = source->exponent;
  float_setnan(dest);
  dest->exponent = exp;
  dest->significand = mant;
#ifdef FLOATDEBUG
  _setvalue_(dest);
#endif
  float_setsign(dest, sign);
}

char
float_copy(
  floatnum dest,
  cfloatnum source,
  int digits)
{
  int scale, save;

  if (digits == EXACT)
    digits = _max(1, float_getlength(source));
  if (!_checkdigits(digits, NOSPECIALVALUE))
    return _seterror(dest, InvalidPrecision);
  if (_is_special(source))
  {
    if (dest != source)
      float_free(dest);
    *dest = *source;
  }
  else
  {
    // invariant: source has to be restored, if it is != dest
    scale = _min(digits - 1, _scaleof(source));
    save = _limit_scale((floatnum)source, scale);
    _corr_trailing_zeros((floatnum)source);
    _scaled_clone(dest, source, EXACT);
    if (dest != source)
      _setscale(source, save);
  }
  return TRUE;
}

/* rounding a value towards zero */
static void
_trunc(
  floatnum dest,
  cfloatnum x,
  int scale)
{
  scale -= _bscandigit(x, scale, 0);
  _scaled_clone(dest, x, scale);
#ifdef FLOATDEBUG
  _setvalue_(dest);
#endif
}

/* rounding a value towards infinity */
static char
_roundup(
  floatnum dest,
  cfloatnum x,
  int scale)
{
  scale -= _bscandigit(x, scale, 9);
  _scaled_clone(dest, x, _max(0, scale));
  if (scale < 0)
  {
    *_valueof(dest) = 1;
    if (!float_isvalidexp(++dest->exponent))
      return FALSE;
  }
  else
  {
    ++*(_valueof(dest) + scale);
  }
#ifdef FLOATDEBUG
  _setvalue_(dest);
#endif
  return TRUE;
}

char
float_round(
  floatnum dest,
  cfloatnum src,
  int digits,
  roundmode mode)
{
  int scalediff, scale;
  char digit;
  signed char sign, updown;

  if (mode > TOMINUSINFINITY)
    return _seterror(dest, InvalidParam);
  if (!_checkdigits(digits, NOSPECIALVALUE))
    return _setnan(dest);
  if (float_isnan(src))
    return _seterror(dest, NoOperand);
  updown = 0;
  scale = digits - 1;
  if (float_getlength(src) > digits)
  {
    sign = float_getsign(src);
    switch(mode)
    {
      case TONEAREST:
        scalediff = _scaleof(src) - scale;
        if (scalediff > 0)
        {
          digit = _digit(src, digits);
          if (digit < 5
              || (digit == 5
              && scalediff == 1
              && (_digit(src, scale) & 1) == 0))
            updown = -1;
          else
            updown = 1;
        }
        break;
      case TOZERO:
        updown = -1;
        break;
      case TOINFINITY:
        updown = 1;
        break;
      case TOPLUSINFINITY:
        updown = sign;
        break;
      case TOMINUSINFINITY:
        updown = -sign;
        break;
    }
  }
  switch (updown)
  {
  case 1:
    if (!_roundup(dest, src, scale))
      return _seterror(dest, Overflow);
    break;
  case 0:
    float_copy(dest, src, digits);
    break;
  case -1:
    _trunc(dest, src, scale);
    break;
  }
  return TRUE;
}

char
float_int(
  floatnum f)
{
  if (!_checknan(f))
    return FALSE;
  if (f->exponent < 0)
    float_setzero(f);
  else if (!float_iszero(f))
    float_round(f, f, f->exponent+1, TOZERO);
  return TRUE;
}

char
float_frac(
  floatnum f)
{
  if (!_checknan(f) || float_iszero(f) || f->exponent < 0)
    return !float_isnan(f);
  if (_scaleof(f) <= f->exponent)
    float_setzero(f);
  else
  {
    int leadingzeros = 0;
    _hidefirst(f, f->exponent + 1);
    leadingzeros = _scan_digit(_valueof(f), float_getlength(f), 0);
    _hidefirst(f, leadingzeros);
    f->exponent = -leadingzeros - 1;
#ifdef FLOATDEBUG
    _setvalue_(f);
#endif
  }
  return TRUE;
}

/* the general purpose add/subtract routine that deals with
   the ordinary case. */
static char
_addsub_normal(
  floatnum dest,
  floatnum summand1,
  floatnum summand2,
  int digits)
{
  floatstruct tmp;
  int expdiff;
  int scale;
  int fulllength;
  int extradigit;

  /* the operands are ordered by their exponent */

  expdiff = (unsigned)(summand1->exponent - summand2->exponent);

  /* the full length of the sum (without carry) */
  fulllength = _max(expdiff + _scaleof(summand2), _scaleof(summand1)) + 1;

  extradigit = 0;
  if (digits == EXACT || digits > fulllength)
    digits = fulllength;
  else
  {
    if (float_getsign(summand1) + float_getsign(summand2) == 0)
      extradigit = 1; /* a true subtraction needs no space for a carry */
    if (expdiff > digits + extradigit)
      /* second operand underflows due to exponent diff */
      return float_copy(dest, summand1, digits+extradigit);
  }

  if (digits > maxdigits)
    return _seterror(dest, InvalidPrecision);

  /* we cannot add the operands directly
     because of possibly different exponents.
     So we assume the second operand "to be OK"
     and shift the decimal point of the first
     appropriately to the right.
     There is a cheap way to do this:
     increment len by expdiff and decrement
     scale by the same amount.
     But: Check the operand is long enough
     to do this. */

  float_create(&tmp);
  if (_scaleof(summand1) < expdiff)
  {
    _scaled_clone(&tmp, summand1, expdiff);
    summand1 = &tmp;
  }
  scale = digits + extradigit - (int)expdiff - 1;
  _movepoint(summand1, expdiff);

  /* truncate overly long operands */
  _limit_scale(summand1, scale);
  _limit_scale(summand2, scale);

  /* add */
  dest->exponent = summand2->exponent;
  bc_add(summand1->significand,
         summand2->significand,
         &(dest->significand),
         scale);

  float_free(&tmp);

  return _normalize(dest);
}

static char
_sub_checkborrow(
  floatnum dest,
  floatnum summand1,
  floatnum summand2,
  int digits)
{
  /* the operands have opposite signs, the same exponent,
     but their first digit of the significand differ.
     The operands are ordered by this digit. */
  int result;
  int borrow;
  int scale1, scale2;
  char save;
  char* v1;
  char* v2;

/* Cancellation occurs, when the operands are of type
   p.000...yyy - q.999...xxx, p-q == 1, because a borrow
   propagates from the difference ..0yyy.. - ..9xxx..,
   leaving zeros in the first part. We check for this here. */

  borrow = 0;
  if (_digit(summand1, 0) - _digit(summand2, 0) == 1)
  {
    scale1 = _scaleof(summand1);
    scale2 = _scaleof(summand2);
    if (scale1 == 0)
      /* the special case of a one-digit first operand
         p. - q.999..xxx */
      borrow = _scan_digit(_valueof(summand2) + 1, scale2, 9);
    else if (scale2 > 0)
      /* count the 0 - 9 pairs after the first digit, the area
         where a borrow can propagate */
      borrow = _scan_09pairs(summand1, summand2, _min(scale1, scale2));

    /* In case of a one-digit second operand (p.yyy.. - q. == 1.yyy..),
       nothing is cancelled out. Borrow is already set to 0, and this is
       the correct value for this case, so nothing has to be done here */

    if (borrow > 0)
    {
      /* we have cancellation here. We skip all digits, that
         cancel out due to a propagating borrow. These
         include the first digit and all following
         (0,9) digit pairs, except the last one. The last
         pair may be subject to cancellation or not, we do not
         care, because the following digit pair either yields a
         non-zero difference, or creates no borrow. Our standard
         adder is good enough to deal with such a limited
         cancelling effect. We will replace the last (0,9)
         digit pair with a (9,8) pair. This prevents the
         creation of a borrow, and yet, will yield the correct
         result */

      /* hide all digits until the last found 0 - 9 pair */
      summand2->exponent -= borrow;
      summand1->exponent -= borrow;
      /* in case of a one-digit significand, there is nothing to hide */
      if (scale1 > 0)
        _hidefirst(summand1, borrow);
      _hidefirst(summand2, borrow);

      /* we replace the last found 0 - 9 pair by a 9 - 8 pair,
         avoiding a carry, yet yielding the correct result */
      save = *(v1 = _valueof(summand1));
      *v1 = 9;
      *(v2 = _valueof(summand2)) = 8;
    }
  }
  result = _addsub_normal(dest, summand1, summand2, digits);

  /* restore the modified digits */
  if (borrow > 0)
  {
    if (summand1 != dest)
      *v1 = save;
    if (summand2 != dest)
      *v2 = 9;
  }
  return result;
}

static char
_sub_expdiff0(
  floatnum dest,
  floatnum summand1,
  floatnum summand2,
  int digits)
{
  int eq;
  int result;

/* the operands are ordered by their significand length,
   and have the same exponent, and different sign */

  /* One type of cancellation is when both significands set out
     with the same digits. Since these digits cancel out
     during subtraction, we look out for the first pair
     of different digits. eq receives the number of
     equal digits, which may be 0 */
  eq = _scan_equal(summand1, summand2);
  if (float_getlength(summand2) == eq)
  {
    /* the complete second operand is cancelled out */
    if (float_getlength(summand1) == eq)
      /* op1 == -op2 */
      return _setzero(dest);
    /* If xxx.. denotes the second operand, the (longer)
       first one is of form xxx..yyy.., since it has
       the same digits in the beginning. During
       subtraction the xxx... part is cancelled out, and
       this leaves yyy... as the subtraction result.
       By copying the yyy... to the result, we can shortcut the
       subtraction.
       But before doing so, we have to check yyy... for
       leading zeros, because the cancellation continues in
       this case. */
    eq += _scan_digit(_valueof(summand1) + eq,
                      float_getlength(summand1) - eq, 0);
    _hidefirst(summand1, eq);
    result = float_copy(dest, summand1, digits);
    dest->exponent -= eq;
    return result != 0 && _normalize(dest);
  }
  /* hide the identical digits, and do the
     subtraction without them. */
  summand1->exponent -= eq;
  summand2->exponent -= eq;
  _hidefirst(summand1, eq);
  _hidefirst(summand2, eq);

  /* order the operands by their first digit */
  if (_digit(summand1, 0) >= _digit(summand2, 0))
    return _sub_checkborrow(dest, summand1, summand2, digits);
  return _sub_checkborrow(dest, summand2, summand1, digits);
}

static char
_sub_expdiff1(
  floatnum dest,
  floatnum summand1,
  floatnum summand2,
  int digits)
{
  /* Cancellation occurs when subtracting 0.9xxx from
     1.0yyy */

  int result;
  char singledigit;
  char* v1;
  char* v2;

  /* the operands have different sign, are ordered by their
     exponent, and the difference of the exponents is 1 */

  /* 1.0yyy may be given as a single digit 1 or as a string of
     digits starting with 1.0 */
  singledigit = _scaleof(summand1) == 0;
  if (_digit(summand1, 0) != 1
      || _digit(summand2, 0) != 9
      || (!singledigit && _digit(summand1, 1) != 0))
    return _addsub_normal(dest, summand1, summand2, digits);

  /* we have cancellation here. We transform this
     case into that of equal exponents. */

  /* we align both operands by hiding the first digit (1) of the
     greater operand. This leaves .0yyy which matches the
     second operand .9xxx. Unfortunately, if the first operand
     has only one digit, we cannot hide it, so we have to
     work around this then. */
  if (!singledigit)
    _hidefirst(summand1, 1);
  /* we change the leading digits into a '9' and a '8' resp.
     So we finally subtract .8xxx from .9yyy, yielding
     the correct result. */
  v1 = _valueof(summand1);
  v2 = _valueof(summand2);
  *v1 = 9;
  *v2 = 8;
  summand1->exponent--;
  result = _sub_checkborrow(dest, summand1, summand2, digits);

  /* restore the original digits */
  if (summand1 != dest)
    *v1 = singledigit? 1 : 0;
  if (summand2 != dest)
    *v2 = 9;
  return result;
}

static char
_sub_ordered(
  floatnum dest,
  floatnum summand1,
  floatnum summand2,
  int digits)
{
  /* we have to check for cancellation when subtracting.
     Cancellation occurs when the operands are almost
     equal in magnitude. E.g. in 1.234 - 1.226 = 0.008,
     the result is on quite a different scale than the
     operands. Actually, this is a big problem, because
     it means that the (true) subtraction is numerically not
     stable. There is no way to get around this; you always
     have to take this into account, when subtracting.
     We make the best out of it, and check for cancellation
     in advance, so the result is at least valid to all digits,
     if the operands are known to be exact.
     Cancellation occurs only, if the difference between the
     exponents is 1 at most. We prepare the critical cases in
     specialized routines, and let the standard routine do the
     rest. */

  /* the operands are ordered by their exponent */

  unsigned expdiff;

  expdiff = (unsigned)(summand1->exponent - summand2->exponent);
  switch (expdiff)
  {
    case 0:
      /* order the operands by their length of the significands */
      if (float_getlength(summand1) >= float_getlength(summand2))
        return _sub_expdiff0(dest, summand1, summand2, digits);
      return _sub_expdiff0(dest, summand2, summand1, digits);
    case 1:
      return _sub_expdiff1(dest, summand1, summand2, digits);
  }
  return _addsub_normal(dest, summand1, summand2, digits);
}

static char
_addsub_ordered(
  floatnum dest,
  floatnum summand1,
  floatnum summand2,
  int digits)
{
  /* operands are ordered by their exponent */

  /* handle a bunch of special cases */
  if (!_checkdigits(digits, EXACT) || !_checknan(summand1))
     return _setnan(dest);
  if (float_iszero(summand2))
    return float_copy(dest, summand1, digits);

  /* separate true addition from true subtraction */
  if (float_getsign(summand1) == float_getsign(summand2))
    return _addsub_normal(dest, summand1, summand2, digits);
  return _sub_ordered(dest, summand1, summand2, digits);
}

char
float_add(
  floatnum dest,
  cfloatnum summand1,
  cfloatnum summand2,
  int digits)
{
  bc_struct bc1, bc2;
  floatstruct tmp1, tmp2;
  floatnum s1, s2;

  /* the adder may occasionally adjust operands to
     his needs. Hence we work on temporary structures */
  s1 = dest;
  s2 = dest;
  if (dest != summand1)
  {
    _copyfn(&tmp1, summand1, &bc1);
    s1 = &tmp1;
  }
  if (dest != summand2)
  {
    _copyfn(&tmp2, summand2, &bc2);
    s2 = &tmp2;
  }

  /* order the operands by their exponent. This should
     bring a NaN always to the front, and keeps zeros after any
     other number. */
  if (s1->exponent >= s2->exponent)
    return _addsub_ordered(dest, s1, s2, digits);
  return _addsub_ordered(dest, s2, s1, digits);
}

char
float_sub(
  floatnum dest,
  cfloatnum minuend,
  cfloatnum subtrahend,
  int scale)
{
  int result;
  if (minuend == subtrahend)
  {
    /* changing the sign of one operand would change that of
       the other as well. So this is a special case */
    if(!_checknan(minuend))
      return FALSE;
    _setzero(dest);
  }
  /* do not use float_neg, because it may change float_error */
  float_setsign((floatnum)subtrahend, -float_getsign(subtrahend));
  result = float_add(dest, minuend, subtrahend, scale);
  if (dest != subtrahend)
    float_setsign((floatnum)subtrahend, -float_getsign(subtrahend));
  return result;
}

char
float_mul(
  floatnum dest,
  cfloatnum factor1,
  cfloatnum factor2,
  int digits)
{
  int result;
  int fullscale;
  int savescale1, savescale2;
  int scale;

  /* handle a bunch of special cases */
  if (!_checkdigits(digits, EXACT)
      || !_checknan(factor1)
      || !_checknan(factor2))
    /* invalid scale value or NaN operand */
    return _setnan(dest);
  if (float_iszero(factor1) || float_iszero(factor2))
    return _setzero(dest);

  scale = digits - 1;
  fullscale = _scaleof(factor1) + _scaleof(factor2);
  if (digits == EXACT || scale > fullscale)
    scale = fullscale;

  if (scale >= maxdigits)
    /* scale too large */
    return _seterror(dest, InvalidPrecision);

  /* limit the scale of the operands to sane sizes */
  savescale1 = _limit_scale((floatnum)factor1, scale);
  savescale2 = _limit_scale((floatnum)factor2, scale);

  /* multiply */
  dest->exponent = factor1->exponent + factor2->exponent;
  bc_multiply(factor1->significand, factor2->significand, &(dest->significand), scale);
  result = _normalize(dest);

  /* reverse order is necessary in case factor1 == factor2 */
  if (dest != factor2)
    _setscale(factor2, savescale2);
  if (dest != factor1)
    _setscale(factor1, savescale1);
  return result;
}

char
float_div(
  floatnum dest,
  cfloatnum dividend,
  cfloatnum divisor,
  int digits)
{
  int result;
  int savescale1, savescale2;
  int exp;

  /* handle a bunch of special cases */
  if (!_checkdigits(digits, INTQUOT) || !_checknan(dividend)
      || !_checknan(divisor))
    return _setnan(dest);
  if (float_iszero(divisor))
    return _seterror(dest, ZeroDivide);
  if (float_iszero(dividend))
    /* 0/x == 0 */
    return _setzero(dest);

  exp = dividend->exponent - divisor->exponent;

  /* check for integer quotient */
  if(digits == INTQUOT)
  {
    if (exp < 0)
      return _setzero(dest);
    digits = exp;
  }

  /* scale OK? */
  if(digits > maxdigits)
    return _seterror(dest, InvalidPrecision);

  /* limit the scale of the operands to sane sizes */
  savescale1 = _limit_scale((floatnum)dividend, digits);
  savescale2 = _limit_scale((floatnum)divisor, digits);

  /* divide */
  result = TRUE;
  dest->exponent = exp;
  bc_divide(dividend->significand,
            divisor->significand,
            &(dest->significand),
            digits);
  if (bc_is_zero(dest->significand))
    float_setzero(dest);
  else
    result = _normalize(dest);

  /* reverse order is necessary in case divisor == dividend */
  if (dest != divisor)
    _setscale(divisor, savescale2);
  if (dest != dividend)
    _setscale(dividend, savescale1);
  return result;
}

char
float_divmod(
  floatnum quotient,
  floatnum remainder,
  cfloatnum dividend,
  cfloatnum divisor,
  int digits)
{
  int exp, exp1;

  if (!_checkdigits(digits, INTQUOT) || !_checknan(dividend)
      || !_checknan(divisor) || quotient == remainder
      || float_iszero(divisor) || float_getlength(divisor) > maxdigits)
  {
    if (quotient == remainder)
      float_seterror(InvalidParam);
    if (float_getlength(divisor) > maxdigits)
      float_seterror(TooExpensive);
    if (float_iszero(divisor))
      float_seterror(ZeroDivide);
    float_setnan(quotient);
    return _setnan(remainder);
  }
  if (float_iszero(dividend))
  {
    float_setzero(quotient);
    return _setzero(remainder);
  }
  exp1 = dividend->exponent;
  exp = exp1 - divisor->exponent;
  if(digits-- == INTQUOT)
  {
    if (exp < 0)
    {
      if (float_copy(remainder, dividend, EXACT))
        return _setzero(quotient);
      return _setnan(quotient);
    }
    digits = exp;
  }
  if (digits > maxdigits)
  {
    float_setnan(quotient);
    return _seterror(remainder, TooExpensive);
  }

  /* divide */
  quotient->exponent = exp;
  remainder->exponent = exp1;
  bc_divmod(dividend->significand,
            divisor->significand,
            &(quotient->significand),
            &(remainder->significand),
            digits);

  /* if something goes wrong (one of the results overflows
     or underflows), always set both quotient and remainder
     to NaN */
  if (bc_is_zero(remainder->significand))
    float_setzero(remainder);
  else if (!_normalize(remainder))
    return _setnan(quotient);
  if (bc_is_zero(quotient->significand))
    float_setzero(quotient);
  else if (!_normalize(quotient))
    return _setnan(remainder);
  return TRUE;
}

char
float_sqrt(floatnum value, int digits)
{
  if (!_checkdigits(digits, NOSPECIALVALUE) || !_checknan(value))
    return _setnan(value);
  switch (float_getsign(value))
  {
    case -1:
      return _seterror(value, OutOfDomain);
    case 0:
      return TRUE;
  }
  if ((value->exponent & 1) != 0)
  {
    if (float_getlength(value) == 1)
      _scaled_clone(value, value, 1);
    _movepoint(value, 1);
  }
  bc_sqrt(&value->significand, digits - 1);
#ifdef FLOATDEBUG
  _setvalue_(value);
#endif
  if (value->exponent >= 0)
    value->exponent >>= 1;
  else
    value->exponent = -((1-value->exponent) >> 1);
  return TRUE;
}
