/* floatio.c: low level conversion, based on floatnum. */
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

#include "floatio.h"
#include "floatlong.h"
#include <string.h>

typedef enum { NmbNormal, NmbSpecial, NmbBufferOverflow } NmbType;

/* Besides the regular bases 2, 8, 10 and 16, there are
   three others (IO_BASE_DEFAULT, IO_BASE_NAN and IO_BASE_ZERO)
   used internally to mark special situations. They are called
   pseudo-bases, because you cannot use them as parameter to
   floatnum functions */
static char
_isspecial(
  signed char base)
{
  switch (base)
  {
  case IO_BASE_NAN:
  case IO_BASE_ZERO:
  case IO_BASE_DEFAULT:
    return 1;
  }
  return 0;
}

/*--------------   ASCII based functions   -------------*/

static char emptystr = '\0';

/* sort of alias for (strlen == 0). I don't have access to a
   POSIX manual, so I don't know how strlen reacts on NULL.*/
static char
_isempty(
  const char* p)
{
  return p == NULL || *p == '\0';
}

/* copies the null-terminated ASCIIZ string src into the
   buffer dest. If the buffer is too small to hold src,
   0 is returned */
static char
_setstr(
  p_buffer dest,
  const char* src)
{
  if (dest->sz > 0)
  {
    if (src == NULL)
      *(dest->buf) = '\0';
    else if (dest->sz < strlen(src) + 1)
      return 0;
    else
      strcpy(dest->buf, src);
  }
  return 1;
}

/* looks, whether dest begins with the substring in
   pattern */
static int
_match(
  const char* dest,
  const char* pattern)
{
  int lg;

  if (_isempty(dest) || _isempty(pattern))
    return 0;
  lg = strlen(pattern);
  return strncmp(dest, pattern, lg) == 0? lg : 0;
}

/* converts an ASCII encoded hexagesimal digit a
   corresponding value < 16 */
char
_ascii2digit(
  char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'A' && c <= 'F')
    return (c - 'A') + 10;
  if (c >= 'a' && c <= 'f')
    return (c - 'a') + 10;
  return NO_DIGIT;
}

static char hexdigits[] = "0123456789ABCDEF";

/* converts a value 0 <=x < 16 into a hexdigit */
char
_digit2ascii(
  int value)
{
  return hexdigits[value];
}

/* advances the pointer buf until it points to the
   first character not being a valid digit of base
   base. The return pointer is the initial value
   of buf, or NULL if no digit was found */
static const char*
_scandigits(
  const char** buf,
  char base)
{
  const char* p;
  const char* result;

  result = *buf;
  p = result - 1;
  while (_ascii2digit(*++p) < base);
  *buf = p;
  return p == result? NULL : result;
}

/*--------------------  t_seq_desc   ---------------*/

/* returns the offset6 of the last significant digit */
static int
_ofslastnz(
  p_seq_desc n)
{
  return n->digits - n->trailing0 - 1;
}

/* number of significant digits in a number sequence */
int
_significantdigits(
  p_seq_desc n)
{
  return n->digits - n->leadingSignDigits - n->trailing0;
}

/* number of digits without leading zeros in a sequence.
  If the sequence describes an integer, these are the digits
  of this integer */
static int
_intdigits(
  p_seq_desc n)
{
  return n->digits - n->leadingSignDigits;
}

/* has a sequence significant digits? */
static char
_iszero(
  p_seq_desc n)
{
  return _significantdigits(n) == 0;
}

/* returns a digit from an ASCIIZ string containing
   digits only */
static char
_getseqdigit(
  int ofs,
  p_seq_desc n)
{
  if (ofs < 0 || ofs > _ofslastnz(n))
    return 0;
  return _ascii2digit(*((char*)(n->param) + ofs));
}

/* returns a digit from an ASCIIZ string containing
   digits only, but complements them  */
static char
_getcmpldigit(
  int ofs,
  p_seq_desc n)
{
  int lastnz;
  char c;

  lastnz = _ofslastnz(n);
  if (ofs > lastnz)
    return 0;
  c = n->base - _getseqdigit(ofs, n) - (ofs == lastnz? 0 : 1);
  return c == n->base? 1 : c;
}

/* initializes a structure used for copying digit
   sequences */
static void
_clearint(
  p_ext_seq_desc n)
{
  n->seq.leadingSignDigits = 0;
  n->seq.digits = 0;
  n->seq.trailing0 = 0;
  n->seq.base = IO_BASE_ZERO;
  n->seq.param = NULL;
  n->getdigit = _getseqdigit;
}

/*----------------------   t_[io]token   ------------------*/

/* initializes a structure used for describing a
   floating point number */
void
_clearnumber(
  p_number_desc n)
{
  n->prefix.sign = IO_SIGN_NONE;
  n->prefix.base = IO_BASE_NAN;
  _clearint(&n->intpart);
  _clearint(&n->fracpart);
  n->exp = 0;
}

/* creates a sequence descriptor from an ASCII digit
   sequence, not necessarily terminated by \0.
   After maxdigits digits, all following digits are
   assumed to be zero, regardless of their true
   value. */
static void
_str2seq(
  p_ext_seq_desc n,
  const char* digits,
  int maxdigits,
  signed char base,
  char complement)
{
  /* pre: n has to be initialized to describe a zero */
  const char* p;
  const char* pz;
  char leadingdigit;
  unsigned char c;

  leadingdigit = 0;
  if (complement)
  {
    leadingdigit = base - 1;
    n->getdigit = _getcmpldigit;
  }
  p = digits;
  /* skip sign digits (usually 0, in complement mode
     F, 7 or 1 */
  for (; _ascii2digit(*p) == leadingdigit; ++p);
  n->seq.leadingSignDigits = p - digits;
  /* pz is pointer to first trailing zero */
  pz = p;
  for (; (c = _ascii2digit(*(p++))) < base;)
  {
    if (--maxdigits >= 0 && c != 0)
      pz = p;
  }
  n->seq.trailing0 = p - pz - 1;
  n->seq.digits = p - digits - 1;
  n->seq.param = (void*)digits;
  if (complement || _significantdigits(&n->seq) != 0)
    n->seq.base = base;
}

/* copy count digits (leading zeros included) from the
   digit sequence described by n to an ASCII buffer */
static Error
_seq2str(
  p_buffer dest,
  int count,
  p_ext_seq_desc n)
{
  int ofs;
  char* buf;

  ofs = 0;
  if (count >= dest->sz)
    return IOBufferOverflow;
  buf = dest->buf;
  for (; ofs < count; ++ofs)
    *(buf++) = _digit2ascii(n->getdigit(ofs, &n->seq));
  *(buf) = '\0';
  return Success;
}

/* copy all digits (leading zeros included) from the
   digit sequence described by n to an ASCII buffer, but
   complement the sequence before writing to the buffer */
static Error
_cmplseq2str(
  p_buffer dest,
  p_ext_seq_desc n)
{
  int bound;
  int ofs;
  int lastnz;
  char* buf;
  char c;

  buf = dest->buf;
  bound = _intdigits(&n->seq);
  if (bound + 1 > dest->sz)
    return IOBufferOverflow;
  lastnz = _ofslastnz(&n->seq);
  for (ofs = -1; ++ofs < lastnz;)
  {
    c = n->seq.base - n->getdigit(ofs, &n->seq) - 1;
    *(buf++) = _digit2ascii(c);
  }
  c = n->getdigit(ofs, &n->seq);
  if (c != 1 || _significantdigits(&n->seq) != 1)
    *(buf++) = _digit2ascii(n->seq.base - c);
  else
  {
    --bound;
    --ofs;
  }
  for (; ++ofs < bound;)
    *(buf++) = _digit2ascii(0);
  *(buf) = '\0';
  return Success;
}

/* create a descriptor from a sequence of digits,
   assuming the sequence is an integer */
static Error
str2int(
  p_ext_seq_desc n,
  const char* value,
  p_prefix prefix,
  int maxdigits)
{
  char complement;

  if (prefix->base == IO_BASE_NAN)
    return IONoBase;
  complement = prefix->sign == IO_SIGN_COMPLEMENT;
  if (complement)
  {
    n->getdigit = _getcmpldigit;
    /* necessary, because when value is empty, base = IO_BASE_ZERO */
    n->seq.base = prefix->base;
  }
  if (value)
    _str2seq(n, value, maxdigits, prefix->base, complement);
  return Success;
}

/* if base describes a special value (0 or NaN), the normal
   conversion routines fail. This routine creates special output
   values for these bases, and return
   nmbBufferOverflow: if the buffer is too small
   nmbSpecial: if it created output
   nmbNormal: if base stands for a usual number that the
               normal routines should deal with */
static NmbType
_special2str(
  p_otokens tokens,
  signed char base)
{
  const char* p;
  switch (base)
  {
  case IO_BASE_ZERO:
    p = "0";
    break;
  case IO_BASE_NAN:
    p = "NaN";
    break;
  default:
    return NmbNormal;
  }
  return _setstr(&tokens->intpart, p)? NmbSpecial : NmbBufferOverflow;
}

/* create an ASCIIZ sequence of the integer part,
   set sign and base. */
static Error
int2str(
  p_otokens tokens,
  p_number_desc n,
  char complement)
{
  tokens->sign = n->prefix.sign;
  tokens->base = n->prefix.base;
  switch (_special2str(tokens, n->prefix.base))
  {
    case NmbSpecial: return Success;
    case NmbBufferOverflow: return IOBufferOverflow;
    default: break; /* NmbNormal */
  }
  /* no special encodings */
  if (complement)
    return _cmplseq2str(&tokens->intpart, &n->intpart);
  return _seq2str(&tokens->intpart, _intdigits(&n->intpart.seq),
                        &n->intpart);
}

/* do some sanity checks, create descriptors of integer and fraction
   part in tokens, set sign and base */
static Error
str2fixp(
  p_number_desc n,
  p_itokens tokens)
{
  Error result;
  int maxdigits;

  maxdigits = tokens->maxdigits;
  n->prefix.base = tokens->base;
  n->prefix.sign = tokens->sign;
  if (tokens->sign == IO_SIGN_COMPLEMENT
      && (!_isempty(tokens->fracpart) || tokens->exp))
    return IOInvalidComplement;
  result = str2int(&n->intpart, tokens->intpart, &n->prefix, maxdigits);
  if (_isspecial(n->prefix.base))
    return result;
  if (!_isempty(tokens->fracpart))
    _str2seq(&n->fracpart, tokens->fracpart,
             maxdigits - _intdigits(&n->intpart.seq),
             n->prefix.base, 0);
  if (n->prefix.sign != IO_SIGN_COMPLEMENT
      && n->intpart.seq.digits + n->fracpart.seq.digits == 0)
    return IONoSignificand;
  if (n->prefix.sign != IO_SIGN_COMPLEMENT
      && _iszero(&n->intpart.seq) && _iszero(&n->fracpart.seq))
    n->prefix.base = IO_BASE_ZERO;
  return Success;
}

/* convert integer and fraction part into ASCIIZ sequences */
static Error
fixp2str(
  p_otokens tokens,
  p_number_desc n,
  int scale)
{
  Error result;

  result = int2str(tokens, n, n->prefix.sign == IO_SIGN_COMPLEMENT);
  if (result != Success || _isspecial(n->prefix.base))
    return result;
  return _seq2str(&tokens->fracpart, scale, &n->fracpart);
}

/* create a descriptor from the digit sequence of the exponent */
static Error
_exp2desc(
  p_number_desc n,
  p_itokens tokens)
{
  if (tokens->expsign != IO_SIGN_NONE || tokens->exp)
  {
    unsigned upperLimit;
    signed char sign;
    switch (tokens->base)
    {
      case 2 : upperLimit = BITS_IN_BINEXP - 1; break;
      case 8 : upperLimit = BITS_IN_OCTEXP - 1; break;
      case 16: upperLimit = BITS_IN_HEXEXP - 1; break;
      default: upperLimit = BITS_IN_EXP - 1; break;
    }
    upperLimit = (1 << (upperLimit)) - 1;
    sign = tokens->expsign;
    switch (sign)
    {
      case IO_SIGN_COMPLEMENT:
        return IOBadExp;
      case IO_SIGN_NONE:
        sign = IO_SIGN_PLUS; break;
      case IO_SIGN_MINUS:
        ++upperLimit; break;
      default:;
    }
    if (tokens->exp > upperLimit)
      return IOExpOverflow;
    n->exp = sign < 0? -(int)(tokens->exp) : tokens->exp;
  }
  return Success;
}

/* create a descriptor from the floating point number given in
   tokens */
Error
str2desc(
  p_number_desc n,
  p_itokens tokens)
{
  Error result;

  _clearnumber(n);
  result = str2fixp(n, tokens);
  if (result == Success)
    result = _exp2desc(n, tokens);
  if (result != Success)
    n->prefix.base = IO_BASE_NAN;
  return result;
}

Error
desc2str(
  p_otokens tokens,
  p_number_desc n,
  int scale)
{
  Error result;

  result = fixp2str(tokens, n, scale);
  if (result != Success || _isspecial(n->prefix.base))
    return result;
  tokens->exp = n->exp;
  return Success;
}

Error
exp2str(
  p_buffer dest,
  int exp,
  char base)
{
  char tmp[BITS_IN_EXP + 3];
  int idx = 0;
  int di = 0;
  if (exp < 0)
    exp = -exp;
  while (exp != 0)
  {
    tmp[idx++] = hexdigits[exp % base];
    exp /= base;
  }
  if (idx == 0)
    tmp[idx++] = hexdigits[0];
  if (dest->sz <= idx)
    return IOBufferOverflow;
  for (; --idx >= 0;)
    dest->buf[di++] = tmp[idx];
  dest->buf[di] = 0;
  return Success;
}


/* *****************   additional stuff, just to get started   *************/

static t_ioparams stdioparams[4] =
{
  {10, 10, '.', "0d", "eE(", "  )", "", DECPRECISION},
  {16, 10, '.', "0x", "(", ")", "sF", HEXPRECISION},
  {2, 10, '.', "0b", "(", ")", "s1", BINPRECISION},
  {8, 10, '.', "0o", "(", ")", "s7", OCTPRECISION}
};

enum {idzero, idx10, idx16, idx2, idx8, idxcount};

static t_ioparams ioparams[idxcount] = {
  {IO_BASE_ZERO, IO_BASE_ZERO, '\0', "", "", "", "", 0x7FFFFFFF},
  {IO_BASE_DEFAULT},
  {IO_BASE_DEFAULT},
  {IO_BASE_DEFAULT},
  {IO_BASE_DEFAULT}
};

static p_ioparams _defaultbase = NULL;

static char
_isvalidioparams(
  p_ioparams param)
{
  return param != NULL && param->base != IO_BASE_DEFAULT;
}

static void
_invalidateioparams(
  p_ioparams param)
{
  if (param)
    param->base = IO_BASE_DEFAULT;
}

p_ioparams
_base2ioparams(
  signed char base)
{
  int idx;

  switch (base)
  {
  case 10:
    idx = idx10;
    break;
  case 16:
    idx = idx16;
    break;
  case 2:
    idx = idx2;
    break;
  case 8:
    idx = idx8;
    break;
  case IO_BASE_ZERO:
    idx = idzero;
    break;
  default:
    return NULL;
  }
  return &ioparams[idx];
}

char
setioparams(
  p_ioparams params)
{
  p_ioparams dest;

  if (!_isvalidioparams(params))
    return 0;
  dest = _base2ioparams(params->base);
  if (dest == NULL)
    return 0;
  *dest = *params;
  return 1;
}

char
delioparams(
  signed char base)
{
  p_ioparams dest;

  dest = _base2ioparams(base);
  _invalidateioparams(dest);
  return dest != NULL;
}

p_ioparams
getioparams(
  signed char base)
{
  p_ioparams result;

  if (base == IO_BASE_DEFAULT)
    return _defaultbase;
  result = _base2ioparams(base);
  return _isvalidioparams(result)? result : NULL;
}

static signed char
_getdefaultbase()
{
  p_ioparams param;

  param = getioparams(IO_BASE_DEFAULT);
  return param == NULL? IO_BASE_DEFAULT : param->base;
}

signed char
setdefaultbase(
  signed char base)
{
  char result;

  result = _getdefaultbase();
  _defaultbase = _base2ioparams(base);
  return result;
}

void
float_stdconvert()
{
  int i;

  for (i = 0; i < 4; ++i)
    setioparams(&stdioparams[i]);
  setdefaultbase(10);
}

const char* basePrefix(char base)
{
  return getioparams(base)->basetag;
}

static signed char
_parsesign(
  const char** buf)
{
  signed char result;

  result = IO_SIGN_NONE;
  if (!_isempty(*buf))
    switch (**buf)
    {
    case '-':
      result = IO_SIGN_MINUS;
      break;
    case '+':
      result = IO_SIGN_PLUS;
    }
  if (result != IO_SIGN_NONE)
    (*buf)++;
  return result;
}

static signed char
_parsebase(
  const char** buf,
  char defaultbase)
{
  signed char base;
  int lg, i;

  lg = 0;
  base = IO_BASE_DEFAULT;
  if (!_isempty(*buf))
    for (i = 0; i < idxcount; ++i)
    {
      lg = _match(*buf, ioparams[i].basetag);
      if (lg > 0)
      {
        base = ioparams[i].base;
        break;
      }
    }
  *buf += lg;
  return base == IO_BASE_DEFAULT? defaultbase : base;
}

static char
_parsecmpl(
  const char** buf,
  char base)
{
  int lg;
  p_ioparams param;

  param = getioparams(base);
  lg = 0;
  if (_isvalidioparams(param))
    lg = _match(*buf, param->cmpltag);
  *buf += lg;
  return lg > 0;
}

Error
parse(
  p_itokens tokens,
  const char** buffer)
{
  p_ioparams params;
  const char* p;
  char* expchar;
  signed char base;
  int idx;
  char dot;
  char* expbegin;
  char* expend;

  tokens->fracpart = NULL;
  tokens->exp = 0;
  tokens->expsign = IO_SIGN_NONE;
  tokens->maxdigits = 0;
  dot = '.';
  expbegin = "(";
  expend = ")";
  p = *buffer;
  tokens->sign = _parsesign(&p);
  base = _parsebase(&p, _getdefaultbase());
  params = getioparams(base);
  if (params != NULL)
  {
    dot = params->dot;
    if (params->expbegin != NULL);
      expbegin = params->expbegin;
    if (params->expend != NULL)
      expend = params->expend;
    tokens->maxdigits = params->maxdigits;
  }
  else
    base = 10;
  tokens->base = base;
  if (tokens->maxdigits <= 0)
    tokens->maxdigits = DECPRECISION;
  if (_parsecmpl(&p, tokens->base))
  {
    if (tokens->sign != IO_SIGN_NONE)
      return IOInvalidComplement;
    tokens->sign = IO_SIGN_COMPLEMENT;
  }
  tokens->intpart = _scandigits(&p, base);
  if (*p == dot)
  {
    ++p;
    tokens->fracpart = _scandigits(&p, base);
  }
  if (!tokens->intpart && !tokens->fracpart
      && tokens->sign != IO_SIGN_COMPLEMENT)
    return IONoSignificand;
  expchar = strchr(expbegin, *p);
  if (!_isempty(expchar))
  {
    const char* expptr;
    int i;
    int e = 0;
    int expbase;
    ++p;
    idx = expchar - expbegin;
    tokens->expsign = _parsesign(&p);
    expbase = _parsebase(&p, base);
    expptr = _scandigits(&p, expbase);
    if (!expptr || (*(expend + idx) != ' ' && *(expend + idx) != *p))
      return IOBadExp;
    for (i = 0; i < p-expptr; ++i)
    {
      if (!_checkmul(&e, expbase)
           || !_checkadd(&e, _ascii2digit(*(expptr+i))))
        return IOExpOverflow;
    }
    tokens->exp = e;
  }
  *buffer = p;
  return Success;
}

static char
_decodesign(
  signed char s)
{
  switch (s)
  {
  case IO_SIGN_PLUS:
    return '+';
  case IO_SIGN_MINUS:
    return '-';
  }
  return '\0';
}

static char*
_decodebase(
  signed char base)
{
  p_ioparams param;

  param = getioparams(base);
  if (!_isvalidioparams(param) || _isempty(param->basetag))
    return &emptystr;
  return param->basetag;
}

static char*
_decodecomplement(
  signed char sign,
  signed char base)
{
  p_ioparams param;

  param = getioparams(base);
  if (sign != IO_SIGN_COMPLEMENT || !_isvalidioparams(param)
      || _isempty(param->cmpltag))
    return &emptystr;
  return param->cmpltag;
}

static void
_cattoken(
  char* buf,
  char* token,
  char enable)
{
  if (enable && !_isempty(token))
    strcat(buf, token);
}

int
cattokens(
  char* buf,
  int bufsz,
  p_otokens tokens,
  signed char expbase,
  unsigned flags)
{
  int sz;
  int fraclg;
  p_ioparams ioparams;
  char* expbegin;
  char* expend;
  char* cmpltag;
  char* basetag;
  char* expbasetag;
  signed char base;
  char dot;
  char cbuf[2];
  char printsign;
  char printbasetag;
  char printcmpl;
  char printleading0;
  char printdot;
  char printexp;
  char printexpsign;
  char printexpbase;
  char printexpbegin;
  char printexpend;
  char exp[BITS_IN_BINEXP+2];
  t_buffer expBuf;

  expBuf.sz = sizeof(exp);
  expBuf.buf = exp;
  cbuf[1] = '\0';
  fraclg = 0;
  if (!_isempty(tokens->fracpart.buf))
  {
    fraclg = strlen(tokens->fracpart.buf) - 1;
    if ((flags & IO_FLAG_SUPPRESS_TRL_ZERO) != 0)
      while (fraclg >= 0 && tokens->fracpart.buf[fraclg] == '0')
        --fraclg;
    ++fraclg;
  }
  ioparams = getioparams(IO_BASE_DEFAULT);
  base = tokens->base;
  printbasetag = !_isspecial(base)
                 && (flags & IO_FLAG_SUPPRESS_BASETAG) == 0
                 && (ioparams == NULL || ioparams->base != base);
  ioparams = getioparams(base);
  basetag = _decodebase(base);
  cmpltag = _decodecomplement(tokens->sign, base);
  expbasetag = NULL;
  if (base == IO_BASE_DEFAULT)
    flags |= IO_FLAG_SUPPRESS_DOT | IO_FLAG_SUPPRESS_LDG_ZERO;
  if ((flags & IO_FLAG_SHOW_BASE) != 0)
    printbasetag = 1;
  printcmpl = tokens->sign == IO_SIGN_COMPLEMENT
                  && (flags & IO_FLAG_SUPPRESS_CMPL) == 0;
  printsign = !printcmpl
              && tokens->sign != IO_SIGN_NONE
              && (tokens->sign != IO_SIGN_PLUS
                  || (flags & IO_FLAG_SUPPRESS_PLUS) == 0);
  printleading0 = _isempty(tokens->intpart.buf)
                  && (flags & IO_FLAG_SUPPRESS_LDG_ZERO) == 0;
  printdot = fraclg > 0 || (flags & IO_FLAG_SUPPRESS_DOT) == 0;
  printexp = base != IO_BASE_NAN && base != IO_BASE_ZERO
             && ((flags & IO_FLAG_SUPPRESS_EXPZERO) == 0
                  || tokens->exp != 0);
  if (printexp)
  {
    if (expbase < 2)
      expbase = ioparams->expbase;
    expbasetag = _decodebase(expbase);
    printexpsign = tokens->exp < 0
                   || (flags & IO_FLAG_SUPPRESS_EXPPLUS) == 0;
    printexpbase = expbasetag != NULL
                   && (flags & IO_FLAG_SUPPRESS_EXPBASE) == 0
                   && (_isempty(basetag)
                       || strcmp(basetag, expbasetag) != 0);
    if ((flags & IO_FLAG_SHOW_EXPBASE) != 0)
      printexpbase = 1;
  }
  dot = '.';
  expbegin = "(";
  expend = ")";
  if (ioparams != NULL)
  {
    dot = ioparams->dot;
    expbegin = ioparams->expbegin;
    expend = ioparams->expend;
  }
  printexpbegin = *expbegin != '\0';
  printexpend = *expend != '\0' && *expend != ' ';
  sz = 1;
  if (printsign)
    sz += 1;
  if (printbasetag)
    sz += strlen(basetag);
  if (printcmpl)
    sz += strlen(cmpltag);
  if (printleading0)
    ++sz;
  if (!_isempty(tokens->intpart.buf))
    sz += strlen(tokens->intpart.buf);
  if (printdot)
    sz += 1;
  sz += fraclg;
  if (printexp)
  {
    exp2str(&expBuf, tokens->exp, expbase);
    if (printexpbegin)
      ++sz;
    if (printexpsign)
      sz += 1;
    if (printexpbase)
      sz += strlen(expbasetag);
    sz += strlen(expBuf.buf);
    if (printexpend)
      ++sz;
  }
  if (sz <= bufsz)
  {
    *buf = '\0';
    cbuf[0] = _decodesign(tokens->sign);
    _cattoken(buf, cbuf, printsign);
    _cattoken(buf, basetag, printbasetag);
    _cattoken(buf, cmpltag, printcmpl);
    _cattoken(buf, "0", printleading0);
    _cattoken(buf, tokens->intpart.buf, 1);
    cbuf[0] = dot;
    _cattoken(buf, cbuf, printdot);
    if (fraclg > 0)
      strncat(buf, tokens->fracpart.buf, fraclg);
    if (printexp)
    {
      cbuf[0] = *expbegin;
      _cattoken(buf, cbuf, printexpbegin);
      cbuf[0] = _decodesign(tokens->exp < 0? -1:1);
      _cattoken(buf, cbuf, printexpsign);
      _cattoken(buf, expbasetag, printexpbase);
      strcat(buf, expBuf.buf);
      cbuf[0] = *expend;
      _cattoken(buf, cbuf, printexpend);
    }
  }
  return sz;
}
