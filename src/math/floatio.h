/* floatio.h: low level conversion, based on floatnum. */
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

#include <base/errors.h>

#ifndef _FLOATIO_H
# define _FLOATIO_H

#define NO_DIGIT 0x7F

#define IO_BASE_ZERO       1
#define IO_BASE_NAN        0
#define IO_BASE_DEFAULT  (-1)

#define IO_SIGN_PLUS         1
#define IO_SIGN_NONE         0
#define IO_SIGN_MINUS      (-1)
#define IO_SIGN_COMPLEMENT (-2)

#define IO_FLAG_SUPPRESS_PLUS       0x001
#define IO_FLAG_SUPPRESS_BASETAG    0x002
#define IO_FLAG_SUPPRESS_CMPL       0x004
#define IO_FLAG_SUPPRESS_LDG_ZERO   0x008
#define IO_FLAG_SUPPRESS_TRL_ZERO   0x010
#define IO_FLAG_SUPPRESS_DOT        0x020
#define IO_FLAG_SUPPRESS_EXPPLUS    0x040
#define IO_FLAG_SUPPRESS_EXPBASE    0x080
#define IO_FLAG_SUPPRESS_EXPZERO    0x100
#define IO_FLAG_SHOW_BASE           0x200
#define IO_FLAG_SHOW_EXPBASE        0x400

#ifdef __cplusplus
    extern "C"{
#endif

typedef struct{
  int sz;
  char* buf;
}t_buffer;
typedef t_buffer* p_buffer;

/* t_seq_desc describes the format of a sequence of digits.
  leadingSignDigits are the count of leading zeros (or for
  two's complement, F's, 7's or 1's), trailing0 are the number
  of zeros at the end of the sequence and digits are the total
  count of digits in the sequence. If a sequence contains only
  zeros, in some contexts, they are counted as sign digits,
  in others they are trailing zeros.
  base is the number base the sequence is coded in (one of 2, 8,
  10 or 16) and is reserved for callback. */
typedef struct{
  int leadingSignDigits;
  int trailing0;
  int digits;
  int base;
  void* param;
} t_seq_desc;
typedef t_seq_desc* p_seq_desc;

/* the number of digits not being a leading sign digit or a trailing zero */
int _significantdigits(p_seq_desc n);

/* sequences of digits can be encoded in various ways (ASCII, bc_num style,
   packed and so on). In order to access a single digit, a getter has to
   be supplied for each encoding. This is the common interface of these getters.
   ofs is the index of the digit in the sequence, the first (most significant)
   having an index 0. The getter should return a sign digit (mostly 0) for
   negative indices and 0 for indices greator or equal to the length of the
   sequence. Instances of t_getdigit usually access the param field of n to
   find the data structure where the digits are encoded in */
typedef char (*t_getdigit)(int ofs, p_seq_desc param);

/* list of tokens that are created in an output process.
   Instead of returning a single ASCII string, all parts
   of a number are kept in separate places, so a post-processor
   can reorder or beautify them */
typedef struct{
  signed char sign;
  signed char base;
  t_buffer intpart;
  t_buffer fracpart;
  signed char expsign;
  signed char expbase;
  t_buffer exp;
} t_otokens;
typedef t_otokens* p_otokens;

/* list of tokens that are sources in an input process.
   Instead of using a single ASCII string, all parts
   of a number are kept in separate places, stripped off all
   grammar related information. The tokens need not be 0
   terminated, as long as the token is delimited by something
   not mistaken as a part of it. */
typedef struct{
  signed char sign;
  signed char base;
  const char* intpart;
  const char* fracpart;
  signed char expsign;
  signed char expbase;
  const char* exp;
  unsigned maxdigits;
} t_itokens;
typedef t_itokens* p_itokens;

typedef struct{
  t_seq_desc seq;
  t_getdigit getdigit;
} t_ext_seq_desc;
typedef t_ext_seq_desc* p_ext_seq_desc;

typedef struct{
  signed char sign;
  signed char base;
} t_prefix;
typedef t_prefix* p_prefix;

typedef struct{
  t_prefix prefix;
  t_ext_seq_desc intpart;
  t_ext_seq_desc fracpart;
  signed char expbase;
  int exp;
} t_number_desc;
typedef t_number_desc* p_number_desc;

void _clearnumber(p_number_desc n);

Error str2desc(p_number_desc n, p_itokens tokens);
char desc2str(p_otokens tokens, p_number_desc n, int scale);

/*------------   additional stuff   ------------------*/

/* t_ioparams is a data structure that contains all necessary information
   to convert an ASCII character encoded number into a t_token and vice versa.
   Most information is grammar related like dot, basetag and so on. Others
   like maxdigits describe general limits of floatnums. */
typedef struct{
  signed char base;
  signed char expbase;
  char dot;
  char* basetag;
  char* expbegin;
  char* expend;
  char* cmpltag;
  unsigned maxdigits;
} t_ioparams;
typedef t_ioparams* p_ioparams;

const char* basePrefix(char base);
Error parse(p_itokens tokens, const char** buf);
int cattokens(char* buf, int bufsz, p_otokens tokens, unsigned flags);
void float_stdconvert();
char setioparams(p_ioparams params);
char delioparams(signed char base);
p_ioparams getioparams(signed char base);
signed char setdefaultbase(signed char base);

#ifdef __cplusplus
}
#endif

#endif /* _FLOATIO_H */
