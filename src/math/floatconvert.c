/* floatconvert.c: radix conversion, based on floatnum. */
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

#include "floatconvert.h"
#include "floatcommon.h"
#include "floatconfig.h"
#include "floatconst.h"
#include "floatipower.h"
#include "main/errors.h"
#include <stdlib.h>

typedef struct{
  t_number_desc n;
  t_longint l;
  floatnum f;
  int bitlength;
  int trailing0;
  int lgbase;
}t_ext_number;
typedef t_ext_number* p_ext_number;

/************************   conversion to/from longint   *******************/

static unsigned
_digitblock(
  floatnum f,
  int ofs,
  int count)
{
  unsigned result;

  result = 0;
  for (; --count >= 0;)
    result = 10 * result + float_getdigit(f, ofs++);
  return result;
}

char
_floatnum2longint(
  t_longint* longint,
  floatnum f)
{
  int digits;
  int i;
  unsigned factor;

  longint->length = 0;
  digits = float_getexponent(f) + 1;
  i = digits % 9;
  _longintadd(longint, _digitblock(f, 0, i));
  factor = 1000000000;
  while (i < digits)
  {
    if (_longintmul(longint, factor) 
        || _longintadd(longint, _digitblock(f, i, 9)))
      return 0;
    i += 9;
  }
  /* extra element for floatlong operations */
  *(longint->value+longint->length) = 0;
  return 1;
}

static void
_setunsigned(
  floatnum f,
  unsigned value)
{
  float_setinteger(f, value);
  if ((int)value < 0)
    float_add(f, f, &cUnsignedBound, EXACT);
}

void
_longint2floatnum(
  floatnum f,
  t_longint* longint)
{
  floatstruct tmp;
  int idx;

  float_setzero(f);
  if(longint->length == 0)
    return;
  float_create(&tmp);
  idx = longint->length - 1;
  for (; idx >= 0; --idx)
  {
    _setunsigned(&tmp, longint->value[idx]);
    float_mul(f, f, &cUnsignedBound, EXACT);
    float_add(f, f, &tmp, EXACT);
  }
  float_free(&tmp);
}

/**************************   io routines   **************************/

static int
_max(
  int a,
  int b)
{
  return a > b? a : b;
}

static char
_validmode(
  char mode)
{
  return mode >= IO_MODE_SCIENTIFIC && mode <= IO_MODE_COMPLEMENT;
}

static int _lg2(
  floatnum x)
  /* The evaluation of _lg2(x) is based
     on an approximation suggested by Abramowitz, Stegun,
     Handbook of mathematical functions.
     The returned base 2 logarithm is valid to
     4 (decimal) digits after the decimal point. */
{
  int i;
  float t;
  char gtsqrt10;

  i = leadingdigits(x, 5);
  gtsqrt10 = i > 31622;
  if (gtsqrt10)
    t = ((float)(i - 100000))/(i + 100000);
  else
    t = ((float)(i - 10000))/(i + 10000);
  i = ((0.36415*t*t + 0.86304) * t
      + (float_getexponent(x)+(gtsqrt10?1:0)))*3.32192809488736235;
  return i >= 0? i : i - 1;
}

static int
lgbase(
  signed char base)
{
  switch(base)
  {
    case 2:
      return 1;
    case 8:
      return 3;
    case 16:
      return 4;
  }
  return 0;
}

static char
_getfnintdigit(
  int ofs,
  p_seq_desc n)
{
  if (ofs <= n->digits)
    return float_getdigit((floatnum)(n->param), ofs);
  return 0;
}

static char
_getfnfracdigit(
  int ofs,
  p_seq_desc n)
{
  floatnum x;
  int exp;

  x = (floatnum)(n->param);
  exp = float_getexponent(x);
  if (ofs >= 0)
    return float_getdigit(x, ofs + exp + 1);
  return 0;
}

static void
_setfndesc(
  p_number_desc n,
  floatnum x)
{
  int digits;

  n->intpart.seq.base = 10;
  digits = _max(float_getexponent(x) + 1, 0);
  n->intpart.seq.digits = digits;
  n->intpart.seq.trailing0 = _max(digits - float_getlength(x), 0);
  n->intpart.seq.param = x;
  n->intpart.getdigit = _getfnintdigit;
  n->fracpart.seq.base = 10;
  n->fracpart.seq.leadingSignDigits = _max(-float_getexponent(x) - 1, 0);
  n->fracpart.seq.digits = float_getlength(x) - digits 
                           + n->fracpart.seq.leadingSignDigits;
  n->fracpart.seq.param = x;
  n->fracpart.getdigit = _getfnfracdigit;
}

static Error
_pack2longint(
  t_longint* l,
  p_ext_seq_desc n)
{
  int bitofs;
  int ofs;
  int logbase;

  logbase = lgbase(n->seq.base);
  ofs = n->seq.leadingSignDigits;
  if (_significantdigits(&n->seq) == 0)
      /* can be true in complement case: 0xFF00 */
    --ofs;
  bitofs = (n->seq.digits - ofs) * logbase;
  if (!_longintsetsize(l, bitofs))
    return IOBufferOverflow;
  for (; bitofs > 0;)
  {
    bitofs -= logbase;
    _orsubstr(l->value, bitofs, n->getdigit(ofs++, &n->seq));
  }
  return Success;
}

static char
_getlongintdigit(
  int ofs,
  p_seq_desc n)
{
  if (ofs < 0 || ofs >= n->digits)
    return 0;
  return _bitsubstr(((t_longint*)(n->param))->value,
                    (n->digits - ofs - 1) * lgbase(n->base))
         & (n->base - 1);
}

static char
_getlongintofsdigit(
  int ofs,
  p_seq_desc n)
{
  p_number_desc nmb;
  int digits;

  nmb = (p_number_desc)(n->param);
  digits = n->digits;
  if (ofs < 0 || ofs >= digits)
    return 0;
  digits += nmb->fracpart.seq.digits;
  return _bitsubstr(((t_longint*)(nmb->fracpart.seq.param))->value,
                    (digits - ofs - 1) * lgbase(n->base))
         & (n->base - 1);
}

static void
_setlongintdesc(
  p_ext_seq_desc n,
  t_longint* l,
  signed char base)
{
  int lg;

  n->seq.base = base;
  lg = lgbase(base);
  n->seq.digits = (_bitlength(l) + lg - 1) / lg;
  n->seq.leadingSignDigits = 0;
  n->seq.trailing0 = _lastnonzerobit(l) / lg;
  n->seq.param = l;
  n->getdigit = _getlongintdigit;
}

static Error
_packdec2int(
  floatnum x,
  p_ext_seq_desc n)
{
  int ofs;
  int exp;
  int bufsz;
  int i;
  char buf[DECPRECISION];

  float_setnan(x);
  ofs = n->seq.leadingSignDigits;
  exp = n->seq.trailing0;
  bufsz = n->seq.digits - ofs - exp;
  if (bufsz > DECPRECISION)
    return IOBufferOverflow;
  if (bufsz == 0)
    float_setzero(x);
  else
    for (i = -1; ++i < bufsz;)
      buf[i] = n->getdigit(ofs++, &n->seq) + '0';
  float_setsignificand(x, NULL, buf, bufsz);
  float_setexponent(x, exp + bufsz - 1);
  return Success;
}

static Error
_packbin2int(
  floatnum x,
  p_ext_seq_desc n)
{
  t_longint l;
  Error result;

  float_setnan(x);
  if ((result = _pack2longint(&l, n)) != Success)
    return result;
  _longint2floatnum(x, &l);
  return Success;
}

static Error
_pack2int(
  floatnum x,
  p_ext_seq_desc n)
{
  switch(n->seq.base)
  {
  case IO_BASE_NAN:
    float_setnan(x);
    break;
  case IO_BASE_ZERO:
    float_setzero(x);
    break;
  case 10:
    return _packdec2int(x, n);
  default:
    return _packbin2int(x, n);
  }
  return Success;
}

static Error
_pack2frac(
  floatnum x,
  p_ext_seq_desc n,
  int digits)
{
  floatstruct tmp;
  int exp;
  Error result;

  n->seq.digits -= n->seq.trailing0;
  n->seq.trailing0 = 0;
  switch(n->seq.base)
  {
  case IO_BASE_NAN:
    float_setnan(x);
    break;
  case IO_BASE_ZERO:
    float_setzero(x);
    break;
  default:
    if ((result = _pack2int(x, n)) != Success)
      return result;
    float_create(&tmp);
    float_setinteger(&tmp, n->seq.base);
    _raiseposi(&tmp, &exp, n->seq.digits, digits+2);
    float_div(x, x, &tmp, digits + 2);
    float_setexponent(x, float_getexponent(x) - exp);
    float_free(&tmp);
  }
  n->seq.digits += n->seq.trailing0;
  return Success;
}

Error
pack2floatnum(
  floatnum x,
  p_number_desc n)
{
  floatstruct tmp;
  int digits;
  int saveerr;
  int saverange;
  Error result;
  signed char base;

  if ((result = _pack2int(x, &n->intpart)) != Success)
    return result;
  if (float_isnan(x))
    return Success;
  saveerr = float_geterror();
  saverange = float_setrange(MAXEXP);
  float_create(&tmp);
  float_move(&tmp, x);
  float_setzero(x);
  digits = DECPRECISION - float_getexponent(&tmp);
  if (digits <= 0
      || (result = _pack2frac(x, &n->fracpart, digits)) == Success)
    float_add(x, x, &tmp, DECPRECISION);
  if (result != Success)
    return result;
  if (!float_getlength(x) == 0) /* no zero, no NaN? */
  {
    base = n->prefix.base;
    float_setinteger(&tmp, base);
    if (n->exp >= 0)
    {
      __raiseposi(&tmp, n->exp, DECPRECISION + 2);
      float_mul(x, x, &tmp, DECPRECISION + 2);
    }
    else
    {
      __raiseposi(&tmp, -n->exp, DECPRECISION + 2);
      float_div(x, x, &tmp, DECPRECISION + 2);
    }
  }
  float_free(&tmp);
  float_setsign(x, n->prefix.sign == IO_SIGN_COMPLEMENT? -1 : n->prefix.sign);
  float_geterror();
  float_seterror(saveerr);
  float_setrange(saverange);
  if (!float_isvalidexp(float_getexponent(x)))
    float_setnan(x);
  return float_isnan(x)? IOExpOverflow : Success;
}

static char
_outscidec(
  p_otokens tokens,
  floatnum x,
  p_number_desc n,
  int scale)
{
  float_checkedround(x, scale + 1);
  n->exp = float_getexponent(x);
  float_setexponent(x, 0);
  _setfndesc(n, x);
  return desc2str(tokens, n, scale);
}

static int
_checkbounds(
  floatnum x,
  int digits,
  signed char base)
{
  if (float_getexponent(x) < 0)
  {
    float_muli(x, x, base, digits);
    return -1;
  }
  else if (float_asinteger(x) >= base)
  {
    float_divi(x, x, base, digits);
    return 1;
  }
  return 0;
}

static void
_scale2int(
  floatnum x,
  int scale,
  signed char base)
{
  floatstruct pwr;
  int pwrexp;

  if (scale != 0)
  {
    float_create(&pwr);
    float_setinteger(&pwr, base);
    _raiseposi(&pwr, &pwrexp, scale, DECPRECISION+4);
    float_mul(x, x, &pwr, DECPRECISION+4);
    float_addexp(x, pwrexp);
    float_free(&pwr);
  }
  float_roundtoint(x, TONEAREST);
}

static char
_fixp2longint(
  p_number_desc n,
  t_longint* l,
  floatnum x,
  int scale)
{
  _scale2int(x, scale, n->prefix.base);
  if (!_floatnum2longint(l, x))
    return 0;
  _setlongintdesc(&n->fracpart, l, n->prefix.base);
  return 1;
}

static int
_extractexp(
  floatnum x,
  int scale,
  signed char base)
{
  floatstruct pwr;
  floatstruct fbase;
  int decprec;
  int pwrexp;
  int exp;
  int logbase;

  logbase = lgbase(base);
  decprec = DECPRECISION + 3;
  if (float_getexponent(x) < 0)
    exp = _lg2(x)-3;
  else
    exp = _lg2(x);
  exp /= logbase;
  if (exp != 0)
  {
    float_create(&fbase);
    float_setinteger(&fbase, base);
    float_create(&pwr);
    float_copy(&pwr, &fbase, EXACT);
    _raiseposi(&pwr, &pwrexp, exp < 0? -exp : exp, decprec);
    if (float_getexponent(x) < 0)
    {
      float_addexp(x, pwrexp);
      float_mul(x, x, &pwr, decprec);
    }
    else
    {
      float_addexp(x, -pwrexp);
      float_div(x, x, &pwr, decprec);
    }
    float_free(&pwr);
    float_free(&fbase);
  }
  exp += _checkbounds(x, decprec, base);
  return exp;
}

static void
_setscale(
  p_number_desc n,
  t_longint* l,
  int scale)
{
  n->intpart.seq.leadingSignDigits = 0;
  n->intpart.seq.trailing0 = n->fracpart.seq.trailing0 - scale;
  if (n->intpart.seq.trailing0 < 0)
    n->intpart.seq.trailing0 = 0;
  n->intpart.seq.base = n->fracpart.seq.base;
  n->intpart.seq.digits = n->fracpart.seq.digits - scale;
  n->intpart.getdigit = _getlongintofsdigit;
  n->intpart.seq.param = n;

  n->fracpart.seq.digits = scale;
  if (n->fracpart.seq.trailing0 >= scale)
  {
    n->fracpart.seq.base = IO_BASE_ZERO;
    n->fracpart.seq.trailing0 = scale;
  }
}

static char
_outscihex(
  p_otokens tokens,
  floatnum x,
  p_number_desc n,
  int scale)
{
  t_longint l;

  n->exp = _extractexp(x, scale, n->prefix.base);
  if (!_fixp2longint(n, &l, x, scale))
    return 0;
  /* rounding in _fixp2longint may have increased the exponent */
  n->exp += n->fracpart.seq.digits - 1 - scale;
  _setscale(n, &l, n->fracpart.seq.digits - 1);
  return desc2str(tokens, n, scale);
}

static char
_isvalidbase(
  signed char base)
{
  return base == 10 || lgbase(base) != 0;
}

static char
_outsci(
  p_otokens tokens,
  floatnum x,
  p_number_desc n,
  int scale)
{
  if (n->prefix.base == 10)
    return _outscidec(tokens, x, n, scale);
  return _outscihex(tokens, x, n, scale);
}

static char
_outfixpdec(
  p_otokens tokens,
  floatnum x,
  p_number_desc n,
  int scale)
{
  int digits;

  digits = float_getexponent(x) + scale + 1;
  if (digits <= 0)
    /* underflow */
    return 0;
  float_round(x, x, digits, TONEAREST);
  _setfndesc(n, x);
  n->expbase = IO_BASE_NAN;
  return desc2str(tokens, n, scale);
}

static char
_outfixphex(
  p_otokens tokens,
  floatnum x,
  p_number_desc n,
  int scale)
{
  floatstruct tmp;
  t_longint l;
  char result;

  float_create(&tmp);
  float_copy(&tmp, x, DECPRECISION+1);
  if (!_fixp2longint(n, &l, x, scale) || l.length == 0)
    /* overflow or underflow */
    result = 0;
  else
  {
    _setscale(n, &l, scale);
    n->expbase = IO_BASE_NAN;
    result = desc2str(tokens, n, scale);
  }
  float_free(&tmp);
  return result;
}

static char
_outfixp(
  p_otokens tokens,
  floatnum x,
  p_number_desc n,
  int scale)
{
  if (n->prefix.base == 10)
    return _outfixpdec(tokens, x, n, scale);
  return _outfixphex(tokens, x, n, scale);
}

static char
_outengdec(
  p_otokens tokens,
  floatnum x,
  p_number_desc n,
  int scale)
{
  int shift;

  float_checkedround(x, scale + 1);
  n->exp = float_getexponent(x);
  if (n->exp < 0)
    shift = 2 - (-n->exp-1) % 3;
  else
    shift = n->exp % 3;
  float_setexponent(x, shift);
  n->exp -= shift;
  _setfndesc(n, x);
  return desc2str(tokens, n, scale - shift);
}

static char
_outeng(
  p_otokens tokens,
  floatnum x,
  p_number_desc n,
  int scale)
{
  return n->prefix.base == 10  && scale >= 2
         && _outengdec(tokens, x, n, scale);
}

static char
_outcompl(
  p_otokens tokens,
  floatnum x,
  p_number_desc n,
  int scale)
{
  if (!float_isinteger(x) || lgbase(n->prefix.base) == 0)
    return 0;
  if (n->prefix.sign == IO_SIGN_MINUS)
    n->prefix.sign = IO_SIGN_COMPLEMENT;
  else
    n->prefix.sign = IO_SIGN_NONE;
  return _outfixphex(tokens, x, n, 0);
}

static void
_emptybuffer(
  p_buffer token)
{
  if (token->sz > 0)
    *(token->buf) = '\0';
  else
    token->buf = NULL;
}

static void
_emptytokens(
  p_otokens tokens)
{
  _emptybuffer(&tokens->intpart);
  _emptybuffer(&tokens->fracpart);
  _emptybuffer(&tokens->exp);
  tokens->sign = IO_SIGN_NONE;
  tokens->expsign = IO_SIGN_NONE;
  tokens->base = IO_BASE_NAN;
  tokens->expbase = IO_BASE_NAN;
}

char float_out(
  p_otokens tokens,
  floatnum x,
  int scale,
  signed char base,
  signed char expbase,
  char outmode)
{
  t_number_desc n;

  _emptytokens(tokens);
  /* do some sanity checks first */
  if (!_validmode(outmode) || scale < 0 || !_isvalidbase(base)
       || !_isvalidbase(expbase))
    return 0;
  _clearnumber(&n);
  if (float_iszero(x))
    n.prefix.base = IO_BASE_ZERO;
  else if (!float_isnan(x))
  {
    n.prefix.base = base;
    n.expbase = expbase;
  }
  if (!_isvalidbase(n.prefix.base))
    /* NaN and 0 are handled here */
    return desc2str(tokens, &n, 0);
  n.prefix.sign = float_getsign(x);
  float_abs(x);
  switch (outmode)
  {
  case IO_MODE_FIXPOINT:
    return _outfixp(tokens, x, &n, scale);
  case IO_MODE_ENG:
    return _outeng(tokens, x, &n, scale);
  case IO_MODE_COMPLEMENT:
    return _outcompl(tokens, x, &n, 0);
  default:
    return _outsci(tokens, x, &n, scale);
  }
}

Error
float_in(
  floatnum x,
  p_itokens tokens)
{
  t_number_desc n;
  int result;

  if ((result = str2desc(&n, tokens)) == Success)
    result = pack2floatnum(x, &n);
  if (result != Success)
  {
    _seterror(x, BadLiteral);
    float_setnan(x);
  }
  return result;
}
