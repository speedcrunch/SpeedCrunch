/* floatlong.c: portable double size integer arithmetic. */
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

#include "floatlong.h"

#define HALFSIZE (sizeof(unsigned) * 4)
#define LOWMASK ((1 << HALFSIZE) - 1)
#define MSB (1 << (2*HALFSIZE-1))

/***************  functions handling a single unsigned  ********************/

int
_findfirstbit(
  unsigned value)
{
  int result;

  result = -1;
  while (value != 0)
  {
    value >>= 1;
    ++result;
  }
  return result;
}

/* pre: value != 0 */
static int
_revfindfirstbit(
  unsigned value)
{
  int result;

  result = 0;
  while ((value & 1) == 0)
  {
    value >>= 1;
    ++result;
  }
  return result;
}

/*******************  double unsigned functions  ********************/

static void
_longsplit(
  unsigned value,
  unsigned* low,
  unsigned* high)
{
  *high = value >> HALFSIZE;
  *low = value & LOWMASK;
}

static unsigned
_longcat(
  unsigned low,
  unsigned high)
{
  return (high << HALFSIZE) + low;
}

char
_longadd(
  unsigned* s1,
  unsigned* s2)
{
  unsigned s1h, s1l, s2h, s2l;

  _longsplit(*s1, &s1l, &s1h);
  _longsplit(*s2, &s2l, &s2h);
  s1l += s2l;
  _longsplit(s1l, &s1l, &s2l);
  s1h += s2h + s2l;
  _longsplit(s1h, &s1h, s2);
  *s1 = _longcat(s1l, s1h);
  return *s2 == 0;
}

char
_longmul(
  unsigned* f1,
  unsigned* f2)
{
  unsigned f1h, f1l, f2h, f2l;

  _longsplit(*f1, &f1l, &f1h);
  _longsplit(*f2, &f2l, &f2h);
  *f1 = f1l * f2l;
  *f2 = f1h * f2h;
  f1l *= f2h;
  f2l *= f1h;
  _longadd(&f1l, &f2l);
  _longsplit(f1l, &f1l, &f1h);
  f1l <<= HALFSIZE;
  _longadd(f1, &f1l);
  *f2 += f1l + (f2l << HALFSIZE) + f1h;
  return *f2 == 0;
}

unsigned
_longshr(
  unsigned low,
  unsigned high,
  char shift)
{
  if (shift == 0)
    return low;
  return (low >> shift) | (high << ((2*HALFSIZE)-shift));
}

unsigned
_longshl(
  unsigned low,
  unsigned high,
  char shift)
{
  if (shift == 0)
    return high;
  return (low >> ((2*HALFSIZE)-shift)) | (high << shift);
}

/*****************   unsigned array functions   *****************/

unsigned
_longarrayadd(
  unsigned* uarray,
  int lg,
  unsigned incr)
{
  for(; lg-- > 0 && incr != 0;)
    _longadd(uarray++, &incr);
  return incr;
}

unsigned
_longarraymul(
  unsigned* uarray,
  int lg,
  unsigned factor)
{
  unsigned ovfl, carry;

  carry = 0;
  ovfl = 0;
  for (; lg-- > 0;)
  {
    carry += ovfl;
    ovfl = factor;
    _longmul(uarray, &ovfl);
    _longadd(uarray++, &carry);
  }
  return carry + ovfl;
}

unsigned
_bitsubstr(
  unsigned* uarray,
  int ofs)
{
  int idx;

  if (ofs <= 0)
    return *uarray << -ofs;
  idx = ofs / BITS_IN_UNSIGNED;
  return _longshr(*(uarray+idx), *(uarray+idx+1),
                  ofs - idx * BITS_IN_UNSIGNED);
}

void
_orsubstr(
  unsigned* uarray,
  int bitofs,
  unsigned value)
{
  int idx;
  int ofs;

  idx = bitofs / BITS_IN_UNSIGNED;
  ofs = bitofs - idx * BITS_IN_UNSIGNED;
  if (ofs == 0)
    *(uarray + idx) |= value;
  else
  {
    *(uarray + idx) |= value << ofs;
    *(uarray + idx + 1) |= value >> (BITS_IN_UNSIGNED - ofs);
  }
}

/****************   longint functions    ****************/

char
_isfull(
  t_longint* l)
{
  return l->length >= (int)UARRAYLG - 1;
}

unsigned
_bitlength(
  t_longint* l)
{
  if (l->length == 0)
    return 0;
  return (l->length - 1) * BITS_IN_UNSIGNED
         + _findfirstbit(l->value[l->length-1]) + 1;
}

unsigned
_lastnonzerobit(
  t_longint* l)
{
  int i;

  if (l->length == 0)
    return -1;
  i = -1;
  for (; ++i < l->length && l->value[i] == 0;);
  return i * BITS_IN_UNSIGNED + _revfindfirstbit(l->value[i]);
}

unsigned
_longintadd(
  t_longint* l,
  unsigned summand)
{
  unsigned ovfl;

  ovfl = _longarrayadd(l->value, l->length, summand);
  if (ovfl != 0 && !_isfull(l))
  {
    l->value[l->length] = ovfl;
    ++l->length;
    ovfl = 0;
  }
  return ovfl;
}

unsigned
_longintmul(
  t_longint* l,
  unsigned factor)
{
  unsigned ovfl;

  ovfl = _longarraymul(l->value, l->length, factor);
  if (ovfl != 0 && !_isfull(l))
  {
    l->value[l->length] = ovfl;
    ++l->length;
    ovfl = 0;
  }
  return ovfl;
}

char
_longintsetsize(
  t_longint* l,
  unsigned bitlength)
{
  int i;

  if (bitlength == 0)
    l->length = 0;
  else
    l->length = (bitlength - 1) / BITS_IN_UNSIGNED + 1;
  if (l->length >= (int)UARRAYLG)
    return 0;
  for(i = l->length; i >= 0; --i)
    l->value[i] = 0;
  return 1;
}

/****************   overflow checking integer functions   *************/

char
_checkadd(
  int* s1,
  int s2)
{
  unsigned u1, u2;

  if ((*s1 < 0) ^ (s2 < 0))
  {
    /* you can always add summands of opposite sign */
    *s1 += s2;
    return 1;
  }
  if (s2 < 0)
  {
    u1 = -*s1;
    u2 = -s2;
    _longadd(&u1, &u2);
    *s1 = -(int)u1;
    return u2 == 0 && *s1 < 0;
  }
  u1 = *s1;
  u2 = s2;
  _longadd(&u1, &u2);
  *s1 = u1;
  return *s1 >= 0;
}

char
_checkmul(
  int* f1,
  int f2)
{
  unsigned x1, x2;
  signed char sgn = 1;
  const unsigned msb = MSB;

  if (*f1 >= 0)
    x1 = *f1;
  else
  {
    sgn = -1;
    x1 = -*f1;
  }
  if (f2 >= 0)
    x2 = f2;
  else
  {
    sgn = -sgn;
    x2 = -f2;
  }
  _longmul(&x1, &x2);
  if (sgn < 0)
  {
    *f1 = -(int)x1;
    return (x2 == 0 && x1 <= msb);
  }
  *f1 = (int)x1;
  return (x2 == 0 && x1 < msb);
}
