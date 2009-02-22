/* floatlogic.c: logic functions, based on floatnum. */
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

#include "floatlogic.h"
#include "floatconst.h"

#define MAXIDX ((LOGICRANGE-1) / BITS_IN_UNSIGNED)
#define SIGNBIT (LOGICRANGE - 1 - MAXIDX * BITS_IN_UNSIGNED)

static void
_zeroextend(
  t_longint* longint)
{
  int idx;

  idx = longint->length - 1;
  for (; ++idx <= MAXIDX;)
    longint->value[idx] = 0;
}

signed char
_signof(
  t_longint* longint)
{
  return (longint->value[MAXIDX] & (1 << SIGNBIT)) != 0? -1:1;
}

static char
_signextend(
  t_longint* longint)
{
  unsigned mask;
  signed char sign;

  sign = _signof(longint);
  mask = (~0) << SIGNBIT;
  if (sign < 0)
    longint->value[MAXIDX] |= mask;
  else
    longint->value[MAXIDX] &= ~mask;
  return sign;
}

static void
_neg(
  t_longint* longint)
{
  int idx;

  idx = -1;
  while (++idx <= MAXIDX && longint->value[idx] == 0);
  if (idx <= MAXIDX)
    longint->value[idx] = - longint->value[idx];
  while (++idx <= MAXIDX)
    longint->value[idx] = ~longint->value[idx];
}

char
_floatnum2logic(
  t_longint* longint,
  cfloatnum x)
{
  floatstruct tmp;
  int digits;

  digits = float_getexponent(x)+1;
  if (float_iszero(x) || digits <= 0)
  {
    longint->length = 1;
    longint->value[0] = 0;
  }
  else
  {
    if (digits > MATHPRECISION)
      return 0;

    float_create(&tmp);
    /* floatnum2longint rounds, we have to truncate first */
    float_copy(&tmp, x, digits);
    if (float_getsign(x) < 0)
      float_add(&tmp, &tmp, &c1, EXACT);
    _floatnum2longint(longint, &tmp);
    float_free(&tmp);
    if (_bitlength(longint) > LOGICRANGE)
      return 0;
  }
  _zeroextend(longint);
  if (float_getsign(x) < 0)
    _not(longint);
  return 1;
}

void
_logic2floatnum(
  floatnum f,
  t_longint* longint)
{
  int idx;
  signed char sign;

  sign = _signextend(longint);
  if (sign < 0)
    _neg(longint);
  idx = MAXIDX;
  while (idx >= 0 && longint->value[idx] == 0)
    --idx;
  if (idx < 0)
    longint->length = 0;
  else
    longint->length = idx + 1;
  _longint2floatnum(f, longint);
  float_setsign(f, sign);
}

void
_not(
  t_longint* longint)
{
  int idx;

  for (idx = -1; ++idx <= MAXIDX;)
    longint->value[idx] = ~(longint->value[idx]);
}

void
_and(
  t_longint* x1,
  t_longint* x2)
{
  int idx;

  for (idx = -1; ++idx <= MAXIDX;)
    x1->value[idx] = x1->value[idx] & x2->value[idx];
}

void
_or(
  t_longint* x1,
  t_longint* x2)
{
  int idx;

  for (idx = -1; ++idx <= MAXIDX;)
    x1->value[idx] = x1->value[idx] | x2->value[idx];
}

void
_xor(
  t_longint* x1,
  t_longint* x2)
{
  int idx;

  for (idx = -1; ++idx <= MAXIDX;)
    x1->value[idx] = x1->value[idx] ^ x2->value[idx];
}

void
_shr(
  t_longint* x,
  unsigned shift)
{
  int moves, idx;
  unsigned sign;

  sign = _signof(x) < 0? ~0 : 0;
  moves = shift/BITS_IN_UNSIGNED;
  if (moves > 0)
  {
    shift -= moves * BITS_IN_UNSIGNED;
    for (idx = moves-1; ++idx <= MAXIDX;)
      x->value[idx-moves] = x->value[idx];
    idx = MAXIDX - moves + 1;
    if (idx < 0)
      idx = 0;
    for (; idx <= MAXIDX; ++idx)
      x->value[idx] = sign;
  }
  if (shift > 0)
  {
    for (idx = -1; ++idx < MAXIDX;)
      x->value[idx] = _longshr(x->value[idx], x->value[idx+1], shift);
    x->value[MAXIDX] = _longshr(x->value[MAXIDX], sign, shift);
  }
}

void
_shl(
  t_longint* x,
  unsigned shift)
{
  int moves, idx;

  moves = shift/BITS_IN_UNSIGNED;
  if (moves > 0)
  {
    shift -= moves * BITS_IN_UNSIGNED;
    for (idx = MAXIDX; idx >= moves; --idx)
      x->value[idx] = x->value[idx-moves];
    if (moves > MAXIDX)
      moves = MAXIDX+1;
    for (idx = -1; ++idx < moves;)
      x->value[idx] = 0;
  }
  if (shift > 0)
  {
    for (idx = MAXIDX; idx > 0; --idx)
      x->value[idx] = _longshl(x->value[idx-1], x->value[idx], shift);
    x->value[0] <<= shift;
  }
}
