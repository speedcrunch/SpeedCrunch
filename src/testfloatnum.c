/* testfloatnum.c: regression test for floatnum. */
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

/*======================================================================

                             REGRESSION TESTS

=======================================================================*/

/* a few tests depend on 32 bit integer size, fix this in future!! */

#include "floatconfig.h"
#include "floatconst.h"
#include "floatcommon.h"
#include "floatseries.h"
#include "floatlog.h"
#include "floatexp.h"
#include "floattrig.h"
#include "floathmath.h"
#include "floatlong.h"
#include "floatconvert.h"
#include "floatipower.h"
#include "floatpower.h"
#include "floatgamma.h"
#include "floatlogic.h"
#include <stdio.h>
#include <string.h>

#ifdef _FLOATNUMTEST

#define msbu (1u << (sizeof(unsigned)*8-1))
#define maxu (msbu + (msbu-1))
#define msbi (1 << (sizeof(int)*8-2))
#define maxi (msbi + (msbi -1))
#define mini (-maxi - 1)

static unsigned rol(unsigned u)
{
  return (u << 1) | (u >> (sizeof(unsigned)*8-1));
}

static unsigned hash(floatnum x)
{
  unsigned result;
  bc_num bc;
  char* p;
  int i;

  result = 0;
  if (x)
  {
    bc = x->significand;
    result = rol(x->exponent) ^ (unsigned)(bc);
    if (bc)
    {
      result = rol(result) ^ bc->n_sign;
      result = rol(result) ^ bc->n_len;
      result = rol(result) ^ bc->n_scale;
      p = bc->n_value;
      result = rol(result) ^ (unsigned)p;
      for (i = 0; i++ <= bc->n_scale;)
        result = rol(result) ^ *(p++);
    }
  }
  return result;
}

static char istruenan(floatnum x)
{
  return x->exponent == EXPNAN && x->significand == NULL;
}

static signed char _cmp(floatnum v1, floatnum v2)
{
  if (float_isnan(v1) && float_isnan(v2))
    return 0;
  return float_cmp(v1, v2);
}

static char mantcmp(bc_num b, const char* s)
{
  char* p;
  int lg, i;

  p = b->n_value;
  lg = strlen(s);
  if (maxdigits < lg)
    lg = maxdigits;
  while (s[lg-1] == '0')
    --lg;
  if (lg != b->n_scale + 1)
    return 0;
  for (i = -1; ++i < lg;)
    if (s[i] - '0' != *(p++))
      return 0;
  return 1;
}

static int scmp(floatnum v, char* s)
{
  int lg;
  char buf[30];

  float_getscientific(buf, 30, v);
  lg = strlen(s);
  return lg = strlen(buf) && memcmp(buf, s, lg) == 0;
}

static char* maxexp(char* buf, char* significand)
{
  int lg;
  lg = strlen(significand);
  memcpy(buf, significand, lg);
  sprintf(buf + lg, "%d", EXPMAX);
  return buf;
}

static char* minexp(char* buf, char* significand)
{
  int lg;
  lg = strlen(significand);
  memcpy(buf, significand, lg);
  sprintf(buf+lg, "%d", EXPMIN);
  return buf;
}

static int tc_fail(int i)
{
  printf("test case %d failed", i + 1);
  return 0;
}

static int tc_longadd(unsigned v1, unsigned v2,
                      unsigned r1, unsigned r2)
{
  unsigned x1, x2, y1, y2;
  x1 = v1;
  x2 = v2;
  y1 = v2;
  y2 = v1;
  _longadd(&x1, &x2);
  _longadd(&y1, &y2);
  return x1 == r1 && x2 == r2 && y1 == r1 && y2 == r2;
}

static int test_longadd()
{

  static struct{
    unsigned v1; unsigned v2; unsigned r1; unsigned r2;
  } testcases[] = {
    {0, 0, 0, 0},
    {1, 2, 3, 0},
    {msbu, 2, msbu+2, 0},
    {1, msbu, msbu+1, 0},
    {maxu-1, 1, maxu, 0},
    {maxu, 1, 0, 1},
    {maxu, maxu, maxu-1, 1},
  };

  int i;

  printf("testing _longadd\n");
  for(i = -1; ++i < sizeof(testcases)/sizeof(testcases[0]);)
    if(!tc_longadd(testcases[i].v1, testcases[i].v2,
                   testcases[i].r1, testcases[i].r2)) return tc_fail(i);
  return 1;
}

static int tc_longmul(unsigned v1, unsigned v2,
                      unsigned r1, unsigned r2)
{
  unsigned x1, x2, y1, y2;
  x1 = v1;
  x2 = v2;
  y1 = v2;
  y2 = v1;
  _longmul(&x1, &x2);
  _longmul(&y1, &y2);
  return x1 == r1 && x2 == r2 && y1 == r1 && y2 == r2;
}

static int test_longmul()
{
  static struct{
    unsigned v1; unsigned v2; unsigned r1; unsigned r2;
  } testcases[] = {
    {0, 0, 0, 0},
    {0, 1, 0, 0},
    {1, 2, 2, 0},
    {msbu, 2, 0, 1},
    {1, msbu, msbu, 0},
    {maxu-1, 1, maxu-1, 0},
    {maxu, 2, maxu-1, 1},
    {maxu, maxu, 1, maxu-1},
  };

  int i;

  printf("testing _longmul\n");
  for(i = -1; ++i < sizeof(testcases)/sizeof(testcases[0]);)
    if(!tc_longmul(testcases[i].v1, testcases[i].v2,
        testcases[i].r1, testcases[i].r2)) return tc_fail(i);
  return 1;
}

static int tc_longshr(unsigned v1, unsigned v2, char shift,
                      unsigned result)
{
  unsigned r;
  r = _longshr(v1, v2, shift);
  return r == result;
}

static int test_longshr()
{
  static struct{
    unsigned v1; unsigned v2; char shift; unsigned r;
  } testcases[] = {
    {0, 0, 5, 0},
    {47, 12, 3, msbu+5},
  };

  int i;

  printf("testing _longshr\n");
  for(i = -1; ++i < sizeof(testcases)/sizeof(testcases[0]);)
    if(!tc_longshr(testcases[i].v1, testcases[i].v2,
        testcases[i].shift, testcases[i].r)) return tc_fail(i);
  return 1;
}

static int tc_checkadd(int v1, int v2, char ok, int result)
{
  int x, y;
  x = v1;
  y = v2;
  return _checkadd(&x, v2) == ok && _checkadd(&y, v1) == ok
      && x == result && y == result;
}

static int test_checkadd()
{
  static struct{
    int v1; int v2; char ok; int r;
  } testcases[] = {
    {0, 0, 1, 0},
    {1, 0, 1, 1},
    {maxi, 0, 1, maxi},
    {0, mini, 1, mini},
    {maxi, 1, 0, mini},
    {mini, -1, 0, maxi},
  };

  int i;

  printf("testing _checkadd\n");
  for(i = -1; ++i < sizeof(testcases)/sizeof(testcases[0]);)
    if(!tc_checkadd(testcases[i].v1, testcases[i].v2,
        testcases[i].ok, testcases[i].r)) return tc_fail(i);
  return 1;
}

static int tc_checkmul(int v1, int v2, char ok, int result)
{
  int x, y;
  x = v1;
  y = v2;
  return _checkmul(&x, v2) == ok && _checkmul(&y, v1) == ok
      && x == result && y == result;
}

static int test_checkmul()
{
  int i;

  static struct{
    int v1; int v2; char ok; int r;
  } testcases[] = {
    {0, 0, 1, 0},
    {1, 1, 1, 1},
    {-1, 1, 1, -1},
    {-1, -1, 1, 1},
    {maxi, 1, 1, maxi},
    {maxi, 2, 0, -2},
    {1, mini, 1, mini},
    {-1, mini, 0, mini},
    {2, mini, 0, 0},
  };

  printf("testing _checkmul\n");
  for(i = -1; ++i < sizeof(testcases)/sizeof(testcases[0]);)
    if(!tc_checkmul(testcases[i].v1, testcases[i].v2,
        testcases[i].ok, testcases[i].r)) return tc_fail(i);
  return 1;
}

static int tc_longarrayadd(unsigned x1, unsigned x2, unsigned x3,
 unsigned smd, unsigned r1, unsigned r2, unsigned r3, unsigned r4,
 int length)
{
  unsigned a[3];
  unsigned r;

  a[0] = x1;
  a[1] = x2;
  a[2] = x3;
  r = _longarrayadd(a, length, smd);
  return a[0] == r1 && a[1] == r2 && a[2] == r3 && r == r4;
}

static int test_longarrayadd()
{
  int i;

  static struct{
    unsigned x1; unsigned x2; unsigned x3; unsigned smd;
    unsigned r1; unsigned r2; unsigned r3; unsigned r4;
    int lg;
  } testcases[] = {
    {0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,3},
    {1,2,3,4,5,2,3,0,3},
    {maxu,2,3,4,3,3,3,0,3},
    {maxu,maxu,3,4,3,0,4,0,3},
    {maxu,maxu,maxu,4,3,0,0,1,3},
    {0,0,0,1,0,0,0,1,0},
    {1,0,0,1,2,0,0,0,1},
    {maxu,0,0,1,0,0,0,1,1},
  };

  printf("testing _longarrayadd\n");
  for(i = -1; ++i < sizeof(testcases)/sizeof(testcases[0]);)
    if(!tc_longarrayadd(testcases[i].x1, testcases[i].x2,
                        testcases[i].x3, testcases[i].smd,
                        testcases[i].r1, testcases[i].r2,
                        testcases[i].r3, testcases[i].r4,
                        testcases[i].lg)) return tc_fail(i);
  return 1;
}

static int tc_longarraymul(unsigned x1, unsigned x2, unsigned x3,
  unsigned factor, unsigned r1, unsigned r2, unsigned r3, unsigned r4,
  int lg)
{
  unsigned a[3];
  unsigned r;

  a[0] = x1;
  a[1] = x2;
  a[2] = x3;
  r = _longarraymul(a, lg, factor);
  return a[0] == r1 && a[1] == r2 && a[2] == r3 && r == r4;
}

static int test_longarraymul()
{
  int i;

  static struct{
    unsigned x1; unsigned x2; unsigned x3; unsigned smd;
    unsigned r1; unsigned r2; unsigned r3; unsigned r4;
    int lg;
  } testcases[] = {
    {0,0,0,0,0,0,0,0,0},
    {0,0,0,7,0,0,0,0,0},
    {7,0,0,7,49,0,0,0,1},
    {0,0,0,3,0,0,0,0,3},
    {1,2,3,4,4,8,12,0,3},
    {maxu,0,0,4,maxu-3,0,0,3,1},
    {maxu,2,3,4,maxu-3,11,12,0,3},
    {maxu,maxu,3,4,maxu-3,maxu,15,0,3},
    {maxu,maxu,maxu,4,maxu-3,maxu,maxu,3,3},
    {maxu,0,0,maxu,1,maxu-1,0,0,3},
    {maxu,maxu,maxu,maxu,1,maxu,maxu,maxu-1,3},
  };

  printf("testing _longarraymul\n");
  for(i = -1; ++i < sizeof(testcases)/sizeof(testcases[0]);)
    if(!tc_longarraymul(testcases[i].x1, testcases[i].x2,
        testcases[i].x3, testcases[i].smd,
        testcases[i].r1, testcases[i].r2,
        testcases[i].r3, testcases[i].r4,
        testcases[i].lg)) return tc_fail(i);
  return 1;
}

static int tc_isnan(void* s, int exp, char result)
{
  floatstruct f;

  f.significand = s;
  f.exponent = exp;
  return float_isnan(&f) == result && exp == f.exponent
         && f.significand == s;
}

static int test_isnan()
{
  static struct{
    void* s; int exp; char result;
  } testcases[] = {
    {NULL, EXPNAN, 1},
    {NULL, EXPZERO, 0},
    {NULL, 0, 1},
    {(void*)1, 0, 0},
    {(void*)1, EXPNAN, 0},
  };
  int i;

  printf("testing float_isnan\n");

  for(i = -1; ++i < sizeof(testcases)/sizeof(testcases[0]);)
    if(!tc_isnan(testcases[i].s, testcases[i].exp,
        testcases[i].result)) return tc_fail(i);
  return 1;
}

static int tc_iszero(void* s, int exp, char result)
{
  floatstruct f;

  f.significand = s;
  f.exponent = exp;
  return float_iszero(&f) == result && exp == f.exponent
      && f.significand == s;
}

static int test_iszero()
{
  static struct{
    void* s; int exp; char result;
  } testcases[] = {
    {NULL, EXPNAN, 0},
    {NULL, EXPZERO, 1},
    {NULL, 0, 0},
    {(void*)1, 0, 0},
    {(void*)1, EXPZERO, 0},
  };
  int i;

  printf("testing float_iszero\n");

  for(i = -1; ++i < sizeof(testcases)/sizeof(testcases[0]);)
    if(!tc_iszero(testcases[i].s, testcases[i].exp,
        testcases[i].result)) return tc_fail(i);
  return 1;
}

static int tc_create(void* s, int exp)
{
  floatstruct f;

  f.significand = s;
  f.exponent = exp;
  float_create(&f);
  return istruenan(&f);
}

static int test_create()
{
  static struct{
    void* s; int exp;
  } testcases[] = {
    {NULL, EXPNAN},
    {NULL, EXPZERO},
    {NULL, 0},
    {(void*)1, 0},
    {(void*)1, EXPNAN},
    {(void*)1, EXPZERO},
  };
  int i;

  printf("testing float_create\n");

  for(i = -1; ++i < sizeof(testcases)/sizeof(testcases[0]);)
    if(!tc_create(testcases[i].s, testcases[i].exp)) return tc_fail(i);
  return 1;
}

static int tc_setnan(char one, int exp)
{
  floatstruct f;
  int refs;

  refs = _one_->n_refs;
  f.significand = NULL;
  if (one)
    f.significand = bc_copy_num(_one_);
  f.exponent = exp;
  float_setnan(&f);
  return istruenan(&f) && refs == _one_->n_refs;
}

static int test_setnan()
{
  static struct{
    char one; int exp;
  } testcases[] = {
    {0, EXPNAN},
    {0, EXPZERO},
    {0, 0},
    {1, 0},
    {1, EXPNAN},
    {1, EXPZERO},
  };
  int i;

  printf("testing float_setnan\n");
  for(i = -1; ++i < sizeof(testcases)/sizeof(testcases[0]);)
    if(!tc_setnan(testcases[i].one, testcases[i].exp)) return tc_fail(i);
  return 1;
}

static int tc_setzero(char one, int exp)
{
  floatstruct f;
  int refs;

  refs = _one_->n_refs;
  f.significand = NULL;
  if (one)
    f.significand = bc_copy_num(_one_);
  f.exponent = exp;
  float_setzero(&f);
  return float_iszero(&f) && refs == _one_->n_refs;
}

static int test_setzero()
{
  static struct{
    char one; int exp;
  } testcases[] = {
    {0, EXPNAN},
    {0, EXPZERO},
    {0, 0},
    {1, 0},
    {1, EXPNAN},
    {1, EXPZERO},
  };
  int i;

  printf("testing float_setzero\n");
  for(i = -1; ++i < sizeof(testcases)/sizeof(testcases[0]);)
    if(!tc_setzero(testcases[i].one, testcases[i].exp)) return tc_fail(i);
  return 1;
}

static int tc_setsignificand(const char* mant, int mlg,
                             const char* result, int dot, int zeros)
{
  floatstruct f;
  int refs;
  int save;
  int z, d, i;
  char retvalue;

  retvalue = 1;
  /* test zero or NaN cases */
  if (result == NULL || result[0] == '0')
  {
    z = -1;
    refs = _one_->n_refs;
    f.exponent = 12;
    f.significand = bc_copy_num(_one_);
    f.significand->n_sign = MINUS;
    d = float_setsignificand(&f, &z, mant, mlg);
    if (result == NULL)
      retvalue = istruenan(&f);
    else if (result[0] == '0')
      retvalue = float_iszero(&f);
    retvalue = retvalue && refs == _one_->n_refs
      && dot == d && z == zeros;
  }
  if (retvalue)
  {
    /* test NULL zeros pointer */
    d = float_setsignificand(&f, NULL, mant, mlg);
    if (result == NULL)
      retvalue = istruenan(&f);
    else if (result[0] == '0')
      retvalue = float_iszero(&f);
    else
      retvalue = f.significand != NULL
          && f.exponent == 0
          && f.significand->n_len == 1
          && f.significand->n_sign == PLUS
          && mantcmp(f.significand, result);
      retvalue = retvalue && dot == d;
  }
  if (retvalue && result != NULL && result[0] != '0')
  {
    save = maxdigits;
    for (i = 0; retvalue && ++i <= save;)
    {
      maxdigits = i;
      refs = _one_->n_refs;
      f.exponent = 12;
      f.significand = bc_copy_num(_one_);
      f.significand->n_sign = MINUS;
      z = -1;
      d = float_setsignificand(&f, &z, mant, mlg);
      retvalue = f.significand != NULL
          && refs == _one_->n_refs
          && f.significand->n_len == 1
          && f.significand->n_sign == PLUS
          && mantcmp(f.significand, result)
          && dot == d && z == zeros
          && f.exponent == 0;
    }
    maxdigits = save;
  }
  _one_->n_sign = PLUS;
  float_setnan(&f);
  return retvalue;
}

static int test_setsignificand()
{
  static struct{
    const char* mant; int lg; const char* result; int dot; int zeros;
  } testcases[] = {
    {NULL, -1, NULL, -1, 0},
    {NULL, 0, NULL, -1, 0},
    {"", NULLTERMINATED, NULL, -1, 0},
    {".", NULLTERMINATED, NULL, -1, 0},
    {"x", NULLTERMINATED, NULL, -1, 0},
    {"-1", NULLTERMINATED, NULL, -1, 0},
    {"+1", NULLTERMINATED, NULL, -1, 0},
    {"1E", NULLTERMINATED, NULL, -1, 0},
    {"1.E", NULLTERMINATED, NULL, -1, 0},
    {"-1.E", NULLTERMINATED, NULL, -1, 0},
    {"1E1", NULLTERMINATED, NULL, -1, 0},
    {"1.E1", NULLTERMINATED, NULL, -1, 0},
    {"1.1E1", NULLTERMINATED, NULL, -1, 0},
    {"1.1e1", NULLTERMINATED, NULL, -1, 0},
    {"1E+1", NULLTERMINATED, NULL, -1, 0},
    {"1.E+1", NULLTERMINATED, NULL, -1, 0},
    {"1.1E+1", NULLTERMINATED, NULL, -1, 0},
    {"1.1e+1", NULLTERMINATED, NULL, -1, 0},
    {"1.1(1)", NULLTERMINATED, NULL, -1, 0},
    {"1.1(+1)", NULLTERMINATED, NULL, -1, 0},
    {"+1E1", NULLTERMINATED, NULL, -1, 0},
    {"+1.E1", NULLTERMINATED, NULL, -1, 0},
    {"+1.1E1", NULLTERMINATED, NULL, -1, 0},
    {"+1.1e1", NULLTERMINATED, NULL, -1, 0},
    {"+1.1(1)", NULLTERMINATED, NULL, -1, 0},
    {"+1.1(+1)", NULLTERMINATED, NULL, -1, 0},
    {"-1.E1", NULLTERMINATED, NULL, -1, 0},
    {"1.E-1", NULLTERMINATED, NULL, -1, 0},
    {"-1.E-1", NULLTERMINATED, NULL, -1, 0},
    {"1111111111111111111111E1", NULLTERMINATED, NULL, -1, 0},
    {"E1", NULLTERMINATED, NULL, -1, 0},
    {".e", NULLTERMINATED, NULL, -1, 0},
    {".1e", NULLTERMINATED, NULL, -1, 0},
    {"1.e1", NULLTERMINATED, NULL, -1, 0},
    {"e1.", NULLTERMINATED, NULL, -1, 0},
    {".1.", NULLTERMINATED, NULL, -1, 0},
    {"1..", NULLTERMINATED, NULL, -1, 0},
    {"1.000000000000000000.0", NULLTERMINATED, NULL, -1, 0},
    {"0", NULLTERMINATED, "0", -1, 0},
    {"0.", NULLTERMINATED, "0", -1, 0},
    {".0", NULLTERMINATED, "0", -1, 0},
    {"000", NULLTERMINATED, "0", -1, 0},
    {"00000000000", NULLTERMINATED, "0", -1, 0},
    {".00000000000", NULLTERMINATED, "0", -1, 0},
    {"00000000000.", NULLTERMINATED, "0", -1, 0},
    {"000000.00000", NULLTERMINATED, "0", -1, 0},
    {"1", -1, NULL, -1, 0},
    {"1", 0, NULL, -1, 0},
    {"1", 1, "1", -1, 0},
    {"1", NULLTERMINATED, "1", -1, 0},
    {"12", 1, "1", -1, 0},
    {"12", NULLTERMINATED, "12", -1, 0},
    {".1", NULLTERMINATED, "1", 0, 0},
    {"1.", NULLTERMINATED, "1", 1, 0},
    {"1.2", NULLTERMINATED, "12", 1, 0},
    {"01.2", NULLTERMINATED, "12", 2, 1},
    {"00.000000000000012", NULLTERMINATED, "12", 2, 15},
    {"12.3456789000", NULLTERMINATED, "123456789", 2, 0},
    {"12345678900.0", NULLTERMINATED, "123456789", 11, 0},
    {"12345678901", NULLTERMINATED, "12345678901", -1, 0},
    {"123456789012", NULLTERMINATED, "12345678901", -1, 0},
    {"1.2345678901", NULLTERMINATED, "12345678901", 1, 0},
    {"0.12345678901", NULLTERMINATED, "12345678901", 1, 1},
    {"0.012345678901", NULLTERMINATED, "12345678901", 1, 2},
    {"1234567890183456789", NULLTERMINATED, "12345678901", -1, 0},
    {"12345678900123456789", NULLTERMINATED, "123456789", -1, 0},
    {"12345678901234567.89", NULLTERMINATED, "12345678901", 17, 0},
  };
  int i, save;

  printf("testing float_setsignificand\n");
  save = maxdigits;
  maxdigits = 11;
  for(i = -1; ++i < sizeof(testcases)/sizeof(testcases[0]);)
    if(!tc_setsignificand(testcases[i].mant, testcases[i].lg,
        testcases[i].result, testcases[i].dot, testcases[i].zeros))
      return tc_fail(i);
  maxdigits = save;
  return 1;
}

static int tc_getsignificand(const char* value, int bufsz, const char* result)
{
  char buf[30];
  int lg, i, j;
  unsigned h;
  floatstruct f;
  char retvalue;

  static int exp[] = {-2, -1, 0, 1, 2, EXPNAN, EXPZERO, MAXEXP, -MAXEXP-1};
  static int sign[] = {PLUS, MINUS};

  float_create(&f);
  float_setsignificand(&f, NULL, value, NULLTERMINATED);
  if (float_isnan(&f) || float_iszero(&f))
  {
    memset(buf, '?', 30);
    h = hash(&f);
    lg = strlen(result);
    retvalue = float_getsignificand(buf+1, bufsz, &f) == lg
        && buf[0] == '?'
        && buf[lg + 1] == '?'
        && memcmp(buf + 1, result, lg) == 0
        && h == hash(&f);
  }
  else
    for (i = sizeof(exp)/sizeof(int); --i >= 0;)
      for (j = -1; ++j < 2;)
      {
        lg = strlen(result);
        memset(buf, '?', 30);
        f.significand->n_sign = sign[j];
        f.exponent = exp[i];
        h = hash(&f);
        retvalue = float_getsignificand(buf+1, bufsz, &f) == lg
          && h == hash(&f)
          && buf[0] == '?'
          && buf[lg + 1] == '?'
          && memcmp(buf + 1, result, lg) == 0;
      }
  float_free(&f);
  return retvalue;
}

static int test_getsignificand()
{
  static const char v1[] = "1";
  static const char v2[] = "12345678901";
  static struct{
    const char* value; int bufsz; const char* result;
  } testcases[] = {
    {v1, -1, ""},
    {v1, 0, ""},
    {v1, 1, "1"},
    {v1, 2, "1"},
    {"N", 0, ""},
    {"N", 1, "N"},
    {"N", 2, "N"},
    {"0", 0, ""},
    {"0", 1, "0"},
    {"0", 2, "0"},
    {v2, 1, "1"},
    {v2, 2, "12"},
    {v2, 10, "1234567890"},
    {v2, 11, "12345678901"},
    {v2, 12, "12345678901"},
  };
  int i;

  printf("testing float_getsignificand\n");

  for(i = -1; ++i < sizeof(testcases)/sizeof(testcases[0]);)
    if(!tc_getsignificand(testcases[i].value,
        testcases[i].bufsz, testcases[i].result))
      return tc_fail(i);
  return 1;
}

static int tc_setexponent(const char* value, int exp, int range,
                          int result, char valid)
{
  floatstruct f;
  int save, i;
  char buf[30];
  static int sign[] = {PLUS, MINUS};

  save = float_setrange(range);
  float_create(&f);
  for (i = -1; ++i < 2;)
  {
    float_setsignificand(&f, NULL, value, NULLTERMINATED);
    if (!float_isnan(&f) && !float_iszero(&f))
    {
      f.exponent = -123;
      f.significand->n_sign = sign[i];
    }
    float_setexponent(&f, exp);
    buf[float_getsignificand(buf, 30, &f)] = '\0';
    if (valid)
    {
      if (strcmp(buf, value) != 0 || f.exponent != result
        || (f.significand && f.significand->n_sign != sign[i]))
        return 0;
    }
    else if(!istruenan(&f))
      return 0;
  }
  float_free(&f);
  float_setrange(save);
  return 1;
}

static int test_setexponent()
{
  int i;
  static struct{
    const char* mant; int exp; int range; int result; char valid;
  } testcases[] = {
    {"N", 0, 100, 0, 0},
    {"N", 1, 100, 0, 0},
    {"N", -1, 100, 0, 0},
    {"N", EXPNAN, 100, 0, 0},
    {"N", 100, 100, 0, 0},
    {"N", EXPZERO, 100, 0, 0},
    {"0", 0, 100, EXPZERO, 1},
    {"0", 1, 100, EXPZERO, 1},
    {"0", -1, 100, EXPZERO, 1},
    {"0", EXPNAN, 100, 0, 0},
    {"0", EXPNAN - 1, 100, 0, 0},
    {"0", EXPZERO, 100, 0, 0},
    {"1", 0, 100, 0, 1},
    {"12", 100, 100, 100, 1},
    {"12", 101, 100, 0, 0},
    {"13", -101, 100, -101, 1},
    {"121", -102, 100, 0, 0},
    {"1432", EXPNAN, 100, 0, 0},
    {"1432", EXPZERO, 100, 0, 0},
    {"1432", MAXEXP, MAXEXP, MAXEXP, 1},
    {"1432", -MAXEXP-1, MAXEXP, -MAXEXP-1, 1},
  };

  printf("testing float_setexponent\n");

  for(i = -1; ++i < sizeof(testcases)/sizeof(testcases[0]);)
    if(!tc_setexponent(testcases[i].mant, testcases[i].exp,
        testcases[i].range, testcases[i].result, testcases[i].valid))
      return tc_fail(i);
  return 1;
}

static int tc_getexponent(const char* value, int exp, int result)
{
  floatstruct f;
  unsigned h;
  char retvalue;

  float_create(&f);
  float_setsignificand(&f, NULL, value, NULLTERMINATED);
  if (!float_isnan(&f) && !float_iszero(&f))
    float_setexponent(&f, exp);
  h = hash(&f);
  retvalue = float_getexponent(&f) == result && hash(&f) == h;
  float_free(&f);
  return retvalue;
}

static int test_getexponent()
{
  static struct{
    const char* value; int exp; int result;
  } testcases[] = {
    {"N", 0, 0},
    {"0", 0, 0},
    {"1", 0, 0},
    {"1234", 5, 5},
    {"24413", -7, -7},
    {"623", MAXEXP, MAXEXP},
    {"6355", -MAXEXP-1, -MAXEXP-1},
  };
  int i, save;

  printf("testing float_getexponent\n");

  save = float_setrange(MAXEXP);
  for(i = -1; ++i < sizeof(testcases)/sizeof(testcases[0]);)
    if(!tc_getexponent(testcases[i].value, testcases[i].exp, testcases[i].result))
      return tc_fail(i);
  float_setrange(save);
  return 1;
}

static int tc_setsign(const char* value, int exp)
{
  floatstruct f;
  unsigned h, hm;
  int i;

  static signed char invsign[] = {2, -2, 127, 126, -128, -127};

  float_create(&f);
  float_setsignificand(&f, NULL, value, NULLTERMINATED);
  float_setexponent(&f, exp);
  h = hash(&f);
  if (f.significand)
    f.significand->n_sign = MINUS;
  hm = hash(&f);
  float_setsign(&f, 0);
  if ((f.significand && f.significand->n_sign != MINUS) || hm != hash(&f))
    return 0;
  float_setsign(&f, 1);
  if ((f.significand && f.significand->n_sign != PLUS) || h != hash(&f))
    return 0;
  float_setsign(&f, 0);
  if ((f.significand && f.significand->n_sign != PLUS) || h != hash(&f))
    return 0;
  float_setsign(&f, 1);
  if ((f.significand && f.significand->n_sign != PLUS) || h != hash(&f))
    return 0;
  float_setsign(&f, -1);
  if ((f.significand && f.significand->n_sign != MINUS) || hm != hash(&f))
    return 0;
  float_setsign(&f, -1);
  if ((f.significand && f.significand->n_sign != MINUS) || hm != hash(&f))
    return 0;
  for (i = sizeof(invsign)/sizeof(signed char); --i >= 0;)
  {
    float_setsignificand(&f, NULL, value, NULLTERMINATED);
    float_setexponent(&f, exp);
    float_setsign(&f, invsign[i]);
    if (!istruenan(&f))
      return 0;
  }
  return 1;
}

static int test_setsign()
{
  int i;
  static struct{
    const char* value; int exp;
  } testcases[] = {
    {"N", 0},
    {"0", 0},
    {"1", 0},
    {"123", 12},
    {"18766", -12},
  };

  printf("testing float_setsign\n");

  for(i = -1; ++i < sizeof(testcases)/sizeof(testcases[0]);)
    if(!tc_setsign(testcases[i].value, testcases[i].exp))
      return tc_fail(i);
  return 1;
}

static int tc_getsign(const char* value, int exp, signed char sign, signed char result)
{
  floatstruct f;
  unsigned h;
  char retvalue;

  float_create(&f);
  float_setsignificand(&f, NULL, value, NULLTERMINATED);
  float_setsign(&f, sign);
  float_setexponent(&f, exp);
  h = hash(&f);
  retvalue = float_getsign(&f) == result && hash(&f) == h;
  float_free(&f);
  return retvalue;
}

static int test_getsign()
{
  int i;
  static struct{
    const char* mant; int exp; signed char sign; signed char result;
  } testcases[] = {
    {"N", 0, 0, 0},
    {"0", 0, 0, 0},
    {"1", 0, 1, 1},
    {"123", 0, -1, -1},
    {"281", 3772, 1, 1},
    {"373", 31912, -1, -1},
    {"232", -233, 1, 1},
    {"123", -1442, -1, -1},
  };

  printf("testing float_getsign\n");

  for(i = -1; ++i < sizeof(testcases)/sizeof(testcases[0]);)
    if(!tc_getsign(testcases[i].mant, testcases[i].exp,
        testcases[i].sign, testcases[i].result)) return tc_fail(i);
  return 1;
}

static int tc_getlength(const char* value, int result)
{
  floatstruct f;
  int i;
  int save;
  unsigned h;

  static int exp[] = {0, -1, 1, MAXEXP, -MAXEXP-1};

  save = float_setrange(MAXEXP);
  float_create(&f);
  float_setsignificand(&f, NULL, value, NULLTERMINATED);
  if (float_isnan(&f) || float_iszero(&f))
  {
    h = hash(&f);
    if (float_getlength(&f) != 0 && h != hash(&f))
      return 0;
  }
  else
  {
    for (i = sizeof(exp)/sizeof(int); --i >= 0;)
    {
      float_setexponent(&f, exp[i]);
      h = hash(&f);
      if (float_getlength(&f) != result || h != hash(&f))
        return 0;
      float_setsign(&f, -1);
      h = hash(&f);
      if (float_getlength(&f) != result || h != hash(&f))
        return 0;
    }
  }
  float_free(&f);
  float_setrange(save);
  return 1;
}

static int test_getlength()
{
  int i;
  static struct{
    const char* mant; int result;
  } testcases[] = {
    {"N", 0},
    {"0", 0},
    {"1", 1},
    {"12", 2},
    {"123", 3},
    {"1234", 4},
    {"12345", 5},
    {"123456", 6},
    {"1234567", 7},
    {"12345678", 8},
    {"123456789", 9},
  };

  printf("testing float_getlength\n");

  for(i = -1; ++i < sizeof(testcases)/sizeof(testcases[0]);)
    if(!tc_getlength(testcases[i].mant, testcases[i].result))
      return tc_fail(i);
  return 1;
}

static int tc_getdigit(const char* value)
{
  floatstruct f;
  int i, j, k, save, lg;
  unsigned h;

  static signed char sign[] = {1, -1};
  static int exp[] = {0, -1, 1, MAXEXP, -MAXEXP-1};

  float_create(&f);
  float_setsignificand(&f, NULL, value, NULLTERMINATED);
  if (float_getlength(&f) == 0)
  {
    h = hash(&f);
    for (k = -5; ++k < 5;)
      if (float_getdigit(&f, k) != 0 || h != hash(&f))
        return 0;
  }
  else
  {
    lg = strlen(value);
    save = float_setrange(MAXEXP);
    for (i = sizeof(exp)/sizeof(exp[0]); --i >= 0;)
      for (j = -1; ++j < 2;)
      {
        float_setexponent(&f, exp[i]);
        float_setsign(&f, sign[j]);
        h = hash(&f);
        for (k = lg + 3; --k > -3;)
          if (k < 0 || k >= lg)
          {
            if (float_getdigit(&f, k) != 0 || h != hash(&f))
              return 0;
          }
          else if (float_getdigit(&f, k) != value[k] - '0' || h != hash(&f))
            return 0;
      }
    float_setrange(save);
  }
  float_free(&f);
  return 1;
}

static int test_getdigit()
{
  int i;
  static struct{
    const char* mant;
  } testcases[] = {
    {"N"},
  };

  printf("testing float_getdigit\n");
  for(i = -1; ++i < sizeof(testcases)/sizeof(testcases[0]);)
    if(!tc_getdigit(testcases[i].mant)) return tc_fail(i);
  return 1;
}

static int tc_getscientific(char* msg, floatnum f, int sz, char* result)
{
  char buf[30];
  int lg;

  lg = strlen(result);
  memset(buf, '?', 30);
  printf("%s", msg);
  if (lg == 0)
    return float_getscientific(buf+1, sz, f) == -1
           && buf[0] == '?'
           && buf[1] == '?';
  return float_getscientific(buf+1, sz, f) == lg
         && buf[0] == '?'
         && buf[lg + 2] == '?'
         && memcmp(buf + 1, result, lg) == 0;
}

static int test_getscientific()
{
  floatstruct f;

  printf("\ntesting float_getscientific\n");
  float_create(&f);

  if (!tc_getscientific("testing NaN, length 3\n",
                      &f, 3,"")) return 0;
  if (!tc_getscientific("testing NaN, length 4\n",
                      &f, 4,"NaN")) return 0;
  if (!tc_getscientific("testing NaN, length 5\n",
                      &f, 5,"NaN")) return 0;

  float_setzero(&f);

  if (!tc_getscientific("testing 0.0, length 1\n",
                      &f, 1,"")) return FALSE;
  if (!tc_getscientific("testing 0.0, length 2\n",
                      &f, 2,"0")) return FALSE;
  if (!tc_getscientific("testing 0.0, length 3\n",
                      &f, 3,"0")) return FALSE;

  f.significand = bc_new_num(1, 10);
  f.exponent = 0;
  memcpy(f.significand->n_value, "\01\02\03\04\05\06\07\010\0\0\01", 11);
  f.significand->n_scale = 0;

  if (!tc_getscientific("testing 1e0, length 3\n",
                      &f, 3,"")) return FALSE;
  if (!tc_getscientific("testing 1e0, length 4\n",
                      &f, 4,"")) return FALSE;
  if (!tc_getscientific("testing 1e0, length 5\n",
                      &f, 5,"1.e0")) return FALSE;
  if (!tc_getscientific("testing 1e0, length 6\n",
                      &f, 6,"1.e0")) return FALSE;

  f.significand->n_scale = 1;

  if (!tc_getscientific("testing 1.2e0, length 4\n",
                      &f, 4,"")) return FALSE;
  if (!tc_getscientific("testing 1.2e0, length 5\n",
                      &f, 5,"1.e0")) return FALSE;
  if (!tc_getscientific("testing 1.2e0, length 6\n",
                      &f, 6,"1.2e0")) return FALSE;
  if (!tc_getscientific("testing 1.2e0, length 7\n",
                      &f, 7,"1.2e0")) return FALSE;

  f.exponent = 10;

  if (!tc_getscientific("testing 1.2e10, length 5\n",
                      &f, 5,"")) return FALSE;
  if (!tc_getscientific("testing 1.2e10, length 6\n",
                      &f, 6,"1.e10")) return FALSE;
  if (!tc_getscientific("testing 1.2e10, length 7\n",
                      &f, 7,"1.2e10")) return FALSE;
  if (!tc_getscientific("testing 1.2e10, length 8\n",
                      &f, 8,"1.2e10")) return FALSE;

  f.significand->n_sign = MINUS;

  if (!tc_getscientific("testing -1.2e10, length 6\n",
                      &f, 6,"")) return FALSE;
  if (!tc_getscientific("testing -1.2e10, length 7\n",
                      &f, 7,"-1.e10")) return FALSE;
  if (!tc_getscientific("testing -1.2e10, length 8\n",
                      &f, 8,"-1.2e10")) return FALSE;
  if (!tc_getscientific("testing -1.2e10, length 9\n",
                      &f, 9,"-1.2e10")) return FALSE;

  f.significand->n_scale = 10;

  if (!tc_getscientific("testing -1.2345678001e10, length 13\n",
                      &f, 13,"-1.234567e10")) return FALSE;
  if (!tc_getscientific("testing -1.2345678001e10, length 14\n",
                      &f, 14,"-1.2345678e10")) return FALSE;
  if (!tc_getscientific("testing -1.2345678001e10, length 15\n",
                      &f, 15,"-1.23456780e10")) return FALSE;
  if (!tc_getscientific("testing -1.2345678001e10, length 16\n",
                      &f, 16,"-1.234567800e10")) return FALSE;
  if (!tc_getscientific("testing -1.2345678001e10, length 17\n",
                      &f, 17,"-1.2345678001e10")) return FALSE;

  f.exponent = -10;

  if (!tc_getscientific("testing -1.2345678001e-10, length 18\n",
                      &f, 18,"-1.2345678001e-10")) return FALSE;

  float_setnan(&f);
  return TRUE;
}

static int tc_changesign(char* msg, floatnum f, signed char result)
{
  /* unused: char buf[30]; */
  /* unused: int lg; */

  printf("%s", msg);
  float_changesign(f);
  return result == float_getsign(f)? TRUE : FALSE;
}

static int test_changesign()
{
  floatstruct f;

  printf("\ntesting float_changesign\n");
  float_create(&f);
  if(!tc_changesign("testing NaN\n", &f, 0)) return FALSE;
  float_setzero(&f);
  if(!tc_changesign("testing zero\n", &f, 0)) return FALSE;
  float_setsignificand(&f, NULL, "123", NULLTERMINATED);
  if(!tc_changesign("testing 1.23\n", &f, -1)) return FALSE;
  if(!tc_changesign("testing -1.23\n", &f, 1)) return FALSE;

  return TRUE;
}

static int tc_abs(char* msg, floatnum f, signed char result)
{
  printf("%s", msg);
  float_abs(f);
  return result == float_getsign(f)? TRUE : FALSE;
}

static int test_abs()
{
  floatstruct f;

  printf("\ntesting float_abs\n");
  float_create(&f);
  if(!tc_abs("testing NaN\n", &f, 0)) return FALSE;
  float_setzero(&f);
  if(!tc_abs("testing zero\n", &f, 0)) return FALSE;
  float_setsignificand(&f, NULL, "123", NULLTERMINATED);
  if(!tc_abs("testing 1.23\n", &f, 1)) return FALSE;
  float_setsign(&f, -1);
  if(!tc_abs("testing -1.23\n", &f, 1)) return FALSE;

  return TRUE;
}

static int tc_setscientific(char* msg, char* value, char* result)
{
  floatstruct f;
  int lg;
  char buf[30];

  float_create(&f);
  printf("%s", msg);
  float_setscientific(&f, value, NULLTERMINATED);
  lg = strlen(result);
  float_getscientific(buf, 30, &f);
  return lg == strlen(buf) && memcmp(buf, result, lg) == 0? TRUE : FALSE;
}

static int test_setscientific()
{
  floatstruct f;
  char buf[30];

  printf("\ntesting float_setscientific\n");
  float_create(&f);
  if (!tc_setscientific("testing empty buffer\n", "", "NaN")) return FALSE;
  if (!tc_setscientific("testing E\n", "E", "NaN")) return FALSE;
  if (!tc_setscientific("testing .\n", ".", "NaN")) return FALSE;
  if (!tc_setscientific("testing x\n", "x", "NaN")) return FALSE;
  if (!tc_setscientific("testing 0Ex\n", "0Ex", "NaN")) return FALSE;
  if (!tc_setscientific("testing 0xEx\n", "0xEx", "NaN")) return FALSE;
  if (!tc_setscientific("testing EE0\n", "EE0", "NaN")) return FALSE;
  if (!tc_setscientific("testing +EE0\n", "+EE0", "NaN")) return FALSE;
  if (!tc_setscientific("testing 00.0.0\n", "00.0.0", "NaN")) return FALSE;
  if (!tc_setscientific("testing 0\n", "0", "0")) return FALSE;
  if (!tc_setscientific("testing +0\n", "+0", "0")) return FALSE;
  if (!tc_setscientific("testing -0\n", "-0", "0")) return FALSE;
  if (!tc_setscientific("testing 0E0\n", "0E0", "0")) return FALSE;
  if (!tc_setscientific("testing 0E+0\n", "0E+0", "0")) return FALSE;
  if (!tc_setscientific("testing 0E-0\n", "0E-0", "0")) return FALSE;
  if (!tc_setscientific("testing 0E+99999999999\n", "0E+99999999999", "0")) return FALSE;
  if (!tc_setscientific("testing 0E-99999999999\n", "0E-99999999999", "0")) return FALSE;
  if (!tc_setscientific("testing 0000\n", "0000", "0")) return FALSE;
  if (!tc_setscientific("testing .0000\n", ".0000", "0")) return FALSE;
  if (!tc_setscientific("testing 0000.\n", "0000.", "0")) return FALSE;
  if (!tc_setscientific("testing 00.00\n", "0000.", "0")) return FALSE;
  if (!tc_setscientific("testing 1\n", "1", "1.e0")) return FALSE;
  if (!tc_setscientific("testing 9\n", "9", "9.e0")) return FALSE;
  if (!tc_setscientific("testing 1.\n", "1.", "1.e0")) return FALSE;
  if (!tc_setscientific("testing 1.0\n", "1.0", "1.e0")) return FALSE;
  if (!tc_setscientific("testing 10\n", "10", "1.e1")) return FALSE;
  if (!tc_setscientific("testing 0.1\n", "0.1", "1.e-1")) return FALSE;
  if (!tc_setscientific("testing 100\n", "100", "1.e2")) return FALSE;
  if (!tc_setscientific("testing 0.01\n", "0.01", "1.e-2")) return FALSE;
  if (!tc_setscientific("testing 1E0\n", "1E0", "1.e0")) return FALSE;
  if (!tc_setscientific("testing 1e0\n", "1e0", "1.e0")) return FALSE;
  if (!tc_setscientific("testing 01e0\n", "01e0", "1.e0")) return FALSE;
  if (!tc_setscientific("testing 01.e0\n", "01.e0", "1.e0")) return FALSE;
  if (!tc_setscientific("testing 1e1\n", "1e1", "1.e1")) return FALSE;
  if (!tc_setscientific("testing 1e-1\n", "1e-1", "1.e-1")) return FALSE;
  if (!tc_setscientific("testing 1.e-1\n", "1.e-1", "1.e-1")) return FALSE;
  if (!tc_setscientific("testing 1.0e-1\n", "1.0e-1", "1.e-1")) return FALSE;
  if (!tc_setscientific("testing 0.1e-1\n", "0.1e-1", "1.e-2")) return FALSE;
  if (!tc_setscientific("testing 10e-1\n", "10e-1", "1.e0")) return FALSE;
  if (!tc_setscientific("testing 100e-1\n", "100e-1", "1.e1")) return FALSE;
  if (!tc_setscientific("testing 1e1000\n", "1e1000", "1.e1000")) return FALSE;
  if (!tc_setscientific("testing 1e-1000\n", "1e-1000", "1.e-1000")) return FALSE;
  if (!tc_setscientific("testing -1e1000\n", "-1e1000", "-1.e1000")) return FALSE;
  if (!tc_setscientific("testing -1e-1000\n", "-1e-1000", "-1.e-1000")) return FALSE;
  if (!tc_setscientific("testing +1e1000\n", "+1e1000", "1.e1000")) return FALSE;
  if (!tc_setscientific("testing +1e-1000\n", "+1e-1000", "1.e-1000")) return FALSE;
  if (!tc_setscientific("testing +1e+1000\n", "+1e+1000", "1.e1000")) return FALSE;
  if (!tc_setscientific("testing +1e-1000\n", "+1e-1000", "1.e-1000")) return FALSE;
  if (!tc_setscientific("testing truncation\n", "1.234567890123456789e0", "1.23456789012345e0")) return FALSE;
  maxexp(buf, "2.e");
  if (!tc_setscientific("testing maximum exponent\n", buf, buf)) return FALSE;
  buf[1] = '0';
  if (!tc_setscientific("testing combined overflow\n", buf, "NaN")) return FALSE;
  ++buf[strlen(buf)-1];
  buf[1] = '.';
  if (!tc_setscientific("testing exponent overflow limit\n", buf, "NaN")) return FALSE;
  if (!tc_setscientific("testing large overflow\n", "1E3000000000000000000000000000000", "NaN")) return FALSE;
  minexp(buf, "1.e");
  if (!tc_setscientific("testing minimum exponent\n", buf, buf)) return FALSE;
  buf[0] = '.';
  buf[1] = '1';
  if (!tc_setscientific("testing combined underflow\n", buf, "NaN")) return FALSE;
  buf[0] = '1';
  buf[1] = '.';
  ++buf[strlen(buf)-1];
  if (!tc_setscientific("testing exponent underflow limit\n", buf, "NaN")) return FALSE;
  if (!tc_setscientific("testing large underflow\n", "1E-3000000000000000000000000000000", "NaN")) return FALSE;
  float_setnan(&f);
  return TRUE;
}

static int tc_cmp(char* msg, char* val1, char* val2, int result)
{
  floatstruct v1;
  floatstruct v2;

  float_create(&v1);
  float_create(&v2);
  printf("%s", msg);
  float_setscientific(&v1, val1, NULLTERMINATED);
  float_setscientific(&v2, val2, NULLTERMINATED);
  result -= float_cmp(&v1, &v2);
  float_free(&v1);
  float_free(&v2);
  return result == 0? TRUE : FALSE;
}

static int test_cmp()
{
  printf("\ntesting float_cmp\n");

  if (!tc_cmp("testing NaN - NaN\n", "NaN", "NaN", UNORDERED)) return FALSE;
  if (!tc_cmp("testing NaN - 0\n", "NaN", "0", UNORDERED)) return FALSE;
  if (!tc_cmp("testing NaN - 1\n", "NaN", "1", UNORDERED)) return FALSE;
  if (!tc_cmp("testing NaN - (-1)\n", "NaN", "-1", UNORDERED)) return FALSE;
  if (!tc_cmp("testing 0 - NaN\n", "0", "NaN", UNORDERED)) return FALSE;
  if (!tc_cmp("testing 1 - NaN\n", "1", "NaN", UNORDERED)) return FALSE;
  if (!tc_cmp("testing (-1) - NaN\n", "-1", "NaN", UNORDERED)) return FALSE;
  if (!tc_cmp("testing 0 - 0\n", "0", "0", 0)) return FALSE;
  if (!tc_cmp("testing 1 - 0\n", "1", "0", 1)) return FALSE;
  if (!tc_cmp("testing 10 - 0\n", "10", "0", 1)) return FALSE;
  if (!tc_cmp("testing 0.1 - 0\n", "0.1", "0", 1)) return FALSE;
  if (!tc_cmp("testing (-10) - 0\n", "-10", "0", -1)) return FALSE;
  if (!tc_cmp("testing (-1) - 0\n", "-1", "0", -1)) return FALSE;
  if (!tc_cmp("testing (-0.1) - 0\n", "-0.1", "0", -1)) return FALSE;
  if (!tc_cmp("testing 0 - 10\n", "0", "10", -1)) return FALSE;
  if (!tc_cmp("testing 0 - 1\n", "0", "1", -1)) return FALSE;
  if (!tc_cmp("testing 0 - 0.1\n", "0", "0.1", -1)) return FALSE;
  if (!tc_cmp("testing 0 - (-10)\n", "0", "-10", 1)) return FALSE;
  if (!tc_cmp("testing 0 - (-1)\n", "0", "-1", 1)) return FALSE;
  if (!tc_cmp("testing 0 - (-0.1)\n", "0", "-0.1", 1)) return FALSE;
  if (!tc_cmp("testing 1 - 10\n", "1", "10", -1)) return FALSE;
  if (!tc_cmp("testing 1 - 1\n", "1", "1", 0)) return FALSE;
  if (!tc_cmp("testing 1 - 0.1\n", "1", "0.1", 1)) return FALSE;
  if (!tc_cmp("testing 1 - (-10)\n", "1", "-10", 1)) return FALSE;
  if (!tc_cmp("testing 1 - (-1)\n", "1", "-1", 1)) return FALSE;
  if (!tc_cmp("testing 1 - (-0.1)\n", "1", "-0.1", 1)) return FALSE;
  if (!tc_cmp("testing (-1) - 10\n", "-1", "10", -1)) return FALSE;
  if (!tc_cmp("testing (-1) - 1\n", "-1", "1", -1)) return FALSE;
  if (!tc_cmp("testing (-1) - 0.1\n", "-1", "0.1", -1)) return FALSE;
  if (!tc_cmp("testing (-1) - (-10)\n", "-1", "-10", 1)) return FALSE;
  if (!tc_cmp("testing (-1) - (-1)\n", "-1", "-1", 0)) return FALSE;
  if (!tc_cmp("testing (-1) - (-0.1)\n", "-1", "-0.1", -1)) return FALSE;
  if (!tc_cmp("testing 1.01 - 1\n", "1.01", "1", 1)) return FALSE;
  if (!tc_cmp("testing 1.01 - 1.02\n", "1.01", "1.02", -1)) return FALSE;
  if (!tc_cmp("testing 1.02 - 1.01\n", "1.02", "1.01", 1)) return FALSE;
  if (!tc_cmp("testing 1.021 - 1.011\n", "1.021", "1.011", 1)) return FALSE;
  if (!tc_cmp("testing 1.011 - 1.021\n", "1.011", "1.021", -1)) return FALSE;
  if (!tc_cmp("testing 1.011 - 1.01\n", "1.011", "1.01", 1)) return FALSE;
  if (!tc_cmp("testing 1.01 - 1.011\n", "1.01", "1.011", -1)) return FALSE;
  if (!tc_cmp("testing (-1.01) - (-1)\n", "-1.01", "-1", -1)) return FALSE;
  if (!tc_cmp("testing (-1.01) - (-1.02)\n", "-1.01", "-1.02", 1)) return FALSE;
  if (!tc_cmp("testing (-1.02) - (-1.01)\n", "-1.02", "-1.01", -1)) return FALSE;
  if (!tc_cmp("testing (-1.021) - (-1.011)\n", "-1.021", "-1.011", -1)) return FALSE;
  if (!tc_cmp("testing (-1.011) - (-1.021)\n", "-1.011", "-1.021", 1)) return FALSE;
  if (!tc_cmp("testing (-1.011) - (-1.01)\n", "-1.011", "-1.01", -1)) return FALSE;
  if (!tc_cmp("testing (-1.01) - (-1.011)\n", "-1.01", "-1.011", 1)) return FALSE;
  return TRUE;
}

static int tc_clone(char* msg, char* source, int scale, char* result)
{
  floatstruct src;
  floatstruct dest;
  int lg;
  char ok;
  char save[30];
  char buf[30];

  float_create(&src);
  float_create(&dest);

  printf("%s", msg);
  float_setscientific(&src, source, NULLTERMINATED);
  float_getscientific(save, 30, &src);
  float_clone(&dest, &src, scale);
  float_getscientific(buf, 30, &dest);
  lg = strlen(result);

  ok = strlen(buf) == lg && memcmp(buf, result, lg) == 0? TRUE:FALSE;
  float_getscientific(buf, 30, &src);
  float_free(&src);
  float_free(&dest);
  return ok && strlen(buf) == strlen(save) && memcmp(buf, save, strlen(buf)) == 0;
}

static int test_clone()
{
  floatstruct f;
  char buf[30];

  printf("\ntesting float_clone\n");
  float_create(&f);
  if (!tc_clone("copy NaN, length = -10\n", "NaN", -10, "NaN")) return FALSE;
  if (!tc_clone("copy NaN, length = EXACT\n", "NaN", EXACT, "NaN")) return FALSE;
  if (!tc_clone("copy NaN, length = 0\n", "NaN", 0, "NaN")) return FALSE;
  if (!tc_clone("copy NaN, length = 1\n", "NaN", 1, "NaN")) return FALSE;
  if (!tc_clone("copy NaN, length = 1000\n", "NaN", 1000, "NaN")) return FALSE;
  if (!tc_clone("copy 0, length = EXACT\n", "0", EXACT, "0")) return FALSE;
  if (!tc_clone("copy 0, length = -1\n", "0", -10, "NaN")) return FALSE;
  if (!tc_clone("copy 0, length = 0\n", "0", 0, "NaN")) return FALSE;
  if (!tc_clone("copy 0, length = 1\n", "0", 1, "0")) return FALSE;
  if (!tc_clone("copy 0, length = 1000\n", "0", 1000, "0")) return FALSE;
  if (!tc_clone("copy 1.2305E4, length = EXACT\n", "1.2305E4", EXACT, "1.2305e4")) return FALSE;
  if (!tc_clone("copy 1.2305E4, length = -10\n", "1.2305E4", -10, "NaN")) return FALSE;
  if (!tc_clone("copy 1.2305E4, length = 0\n", "1.2305E4", 0, "NaN")) return FALSE;
  if (!tc_clone("copy 1.2305E4, length = 1\n", "1.2305E4", 1, "1.e4")) return FALSE;
  if (!tc_clone("copy 1.2305E4, length = 2\n", "1.2305E4", 2, "1.2e4")) return FALSE;
  if (!tc_clone("copy 1.2305E4, length = 3\n", "1.2305E4", 3, "1.23e4")) return FALSE;
  if (!tc_clone("copy 1.2305E4, length = 4\n", "1.2305E4", 4, "1.23e4")) return FALSE;
  if (!tc_clone("copy 1.2305E4, length = 5\n", "1.2305E4", 5, "1.2305e4")) return FALSE;
  if (!tc_clone("copy 1.2305E4, length = 6\n", "1.2305E4", 6, "1.2305e4")) return FALSE;
  if (!tc_clone("copy -1.2305E4, length = 5\n", "-1.2305E4", 5, "-1.2305e4")) return FALSE;
  printf("%s", "in place copy, exact");
  float_setscientific(&f, "-1.2345", NULLTERMINATED);
  float_clone(&f, &f, EXACT);
  float_getscientific(buf, 30, &f);
  if(memcmp(buf, "-1.2345e0", 10) != 0) return FALSE;
  printf("%s", "in place copy, exceed");
  float_clone(&f, &f, 20);
  float_getscientific(buf, 30, &f);
  if(memcmp(buf, "-1.2345e0", 10) != 0) return FALSE;
  printf("%s", "in place copy, truncating");
  float_clone(&f, &f, 3);
  float_getscientific(buf, 30, &f);
  if(memcmp(buf, "-1.23e0", 8) != 0) return FALSE;

  float_free(&f);
  return TRUE;
}

static int tc_round(char* msg, char* value, int digits, roundmode mode, char* result)
{
  floatstruct f;
  int lg;
  char buf[30];

  float_create(&f);

  printf("%s", msg);
  float_setscientific(&f, value, NULLTERMINATED);
  float_round(&f, digits, mode);
  float_getscientific(buf, 30, &f);
  lg = strlen(result);
  float_free(&f);
  return strlen(buf) == lg && memcmp(buf, result, lg) == 0? TRUE:FALSE;
}

static int test_round()
{
  char buf[30];

  printf("\ntesting float_round\n");
  if (!tc_round("NaN, up, -1\n", "NaN", -1, TOPLUSINFINITY, "NaN")) return FALSE;
  if (!tc_round("NaN, down, -1\n", "NaN", -1, TOMINUSINFINITY, "NaN")) return FALSE;
  if (!tc_round("NaN, zero, -1\n", "NaN", -1, TOZERO, "NaN")) return FALSE;
  if (!tc_round("NaN, infinity, -1\n", "NaN", -1, TOINFINITY, "NaN")) return FALSE;
  if (!tc_round("NaN, nearest, -1\n", "NaN", -1, TONEAREST, "NaN")) return FALSE;
  if (!tc_round("NaN, invalid, -1\n", "NaN", -1, (roundmode)100, "NaN")) return FALSE;
  if (!tc_round("NaN, up, 0\n", "NaN", 0, TOPLUSINFINITY, "NaN")) return FALSE;
  if (!tc_round("NaN, down, 0\n", "NaN", 0, TOMINUSINFINITY, "NaN")) return FALSE;
  if (!tc_round("NaN, zero, 0\n", "NaN", 0, TOZERO, "NaN")) return FALSE;
  if (!tc_round("NaN, infinity, 0\n", "NaN", 0, TOINFINITY, "NaN")) return FALSE;
  if (!tc_round("NaN, nearest, 0\n", "NaN", 0, TONEAREST, "NaN")) return FALSE;
  if (!tc_round("NaN, up, 1\n", "NaN", 1, TOPLUSINFINITY, "NaN")) return FALSE;
  if (!tc_round("NaN, down, 1\n", "NaN", 1, TOMINUSINFINITY, "NaN")) return FALSE;
  if (!tc_round("NaN, zero, 1\n", "NaN", 1, TOZERO, "NaN")) return FALSE;
  if (!tc_round("NaN, infinity, 1\n", "NaN", 1, TOINFINITY, "NaN")) return FALSE;
  if (!tc_round("NaN, nearest, 1\n", "NaN", 1, TONEAREST, "NaN")) return FALSE;
  if (!tc_round("NaN, invalid, 1\n", "NaN", 1, (roundmode)100, "NaN")) return FALSE;
  if (!tc_round("0, up, -1\n", "0", -1, TOPLUSINFINITY, "NaN")) return FALSE;
  if (!tc_round("0, down, -1\n", "0", -1, TOMINUSINFINITY, "NaN")) return FALSE;
  if (!tc_round("0, zero, -1\n", "0", -1, TOZERO, "NaN")) return FALSE;
  if (!tc_round("0, infinity, -1\n", "0", -1, TOINFINITY, "NaN")) return FALSE;
  if (!tc_round("0, nearest, -1\n", "0", -1, TONEAREST, "NaN")) return FALSE;
  if (!tc_round("0, invalid, -1\n", "0", -1, (roundmode)100, "NaN")) return FALSE;
  if (!tc_round("0, up, 0\n", "0", 0, TOPLUSINFINITY, "NaN")) return FALSE;
  if (!tc_round("0, down, 0\n", "0", 0, TOMINUSINFINITY, "NaN")) return FALSE;
  if (!tc_round("0, zero, 0\n", "0", 0, TOZERO, "NaN")) return FALSE;
  if (!tc_round("0, infinity, 0\n", "0", 0, TOINFINITY, "NaN")) return FALSE;
  if (!tc_round("0, nearest, 0\n", "0", 0, TONEAREST, "NaN")) return FALSE;
  if (!tc_round("0, up, 1\n", "0", 1, TOPLUSINFINITY, "0")) return FALSE;
  if (!tc_round("0, down, 1\n", "0", 1, TOMINUSINFINITY, "0")) return FALSE;
  if (!tc_round("0, zero, 1\n", "0", 1, TOZERO, "0")) return FALSE;
  if (!tc_round("0, infinity, 1\n", "0", 1, TOINFINITY, "0")) return FALSE;
  if (!tc_round("0, nearest, 1\n", "0", 1, TONEAREST, "0")) return FALSE;
  if (!tc_round("0, invalid, 1\n", "0", 1, (roundmode)100, "NaN")) return FALSE;
  if (!tc_round("1, up, -1\n", "1", -1, TOPLUSINFINITY, "NaN")) return FALSE;
  if (!tc_round("1, down, -1\n", "1", -1, TOMINUSINFINITY, "NaN")) return FALSE;
  if (!tc_round("1, zero, -1\n", "1", -1, TOZERO, "NaN")) return FALSE;
  if (!tc_round("1, infinity, -1\n", "1", -1, TOINFINITY, "NaN")) return FALSE;
  if (!tc_round("1, nearest, -1\n", "1", -1, TONEAREST, "NaN")) return FALSE;
  if (!tc_round("1, invalid, -1\n", "1", -1, (roundmode)100, "NaN")) return FALSE;
  if (!tc_round("1, up, 0\n", "1", 0, TOPLUSINFINITY, "NaN")) return FALSE;
  if (!tc_round("1, down, 0\n", "1", 0, TOMINUSINFINITY, "NaN")) return FALSE;
  if (!tc_round("1, zero, 0\n", "1", 0, TOZERO, "NaN")) return FALSE;
  if (!tc_round("1, infinity, 0\n", "1", 0, TOINFINITY, "NaN")) return FALSE;
  if (!tc_round("1, nearest, 0\n", "1", 0, TONEAREST, "NaN")) return FALSE;
  if (!tc_round("1, up, 1\n", "1", 1, TOPLUSINFINITY, "1.e0")) return FALSE;
  if (!tc_round("1, down, 1\n", "1", 1, TOMINUSINFINITY, "1.e0")) return FALSE;
  if (!tc_round("1, zero, 1\n", "1", 1, TOZERO, "1.e0")) return FALSE;
  if (!tc_round("1, infinity, 1\n", "1", 1, TOINFINITY, "1.e0")) return FALSE;
  if (!tc_round("1, nearest, 1\n", "1", 1, TONEAREST, "1.e0")) return FALSE;
  if (!tc_round("1, invalid, 1\n", "1", 1, (roundmode)100, "NaN")) return FALSE;
  if (!tc_round("1, up, 2\n", "1", 2, TOPLUSINFINITY, "1.e0")) return FALSE;
  if (!tc_round("1, down, 2\n", "1", 2, TOMINUSINFINITY, "1.e0")) return FALSE;
  if (!tc_round("1, zero, 2\n", "1", 2, TOZERO, "1.e0")) return FALSE;
  if (!tc_round("1, infinity, 2\n", "1", 2, TOINFINITY, "1.e0")) return FALSE;
  if (!tc_round("1, nearest, 2\n", "1", 2, TONEAREST, "1.e0")) return FALSE;
  if (!tc_round("1.903, down, 1\n", "1.903", 1, TOMINUSINFINITY, "1.e0")) return FALSE;
  if (!tc_round("1.903, down, 2\n", "1.903", 2, TOMINUSINFINITY, "1.9e0")) return FALSE;
  if (!tc_round("1.903, down, 3\n", "1.903", 3, TOMINUSINFINITY, "1.9e0")) return FALSE;
  if (!tc_round("1.903, down, 4\n", "1.903", 4, TOMINUSINFINITY, "1.903e0")) return FALSE;
  if (!tc_round("-1.2, down, 1\n", "-1.2", 1, TOMINUSINFINITY, "-2.e0")) return FALSE;
  if (!tc_round("1.29903, up, 1\n", "1.29903", 1, TOPLUSINFINITY, "2.e0")) return FALSE;
  if (!tc_round("1.29903, up, 2\n", "1.29903", 2, TOPLUSINFINITY, "1.3e0")) return FALSE;
  if (!tc_round("1.29903, up, 3\n", "1.29903", 3, TOPLUSINFINITY, "1.3e0")) return FALSE;
  if (!tc_round("1.29903, up, 4\n", "1.29903", 4, TOPLUSINFINITY, "1.3e0")) return FALSE;
  if (!tc_round("1.29903, up, 5\n", "1.29903", 5, TOPLUSINFINITY, "1.2991e0")) return FALSE;
  if (!tc_round("1.29903, up, 6\n", "1.29903", 6, TOPLUSINFINITY, "1.29903e0")) return FALSE;
  if (!tc_round("9.99, up, 1\n", "9.99", 1, TOPLUSINFINITY, "1.e1")) return FALSE;
  if (!tc_round("9.99, up, 2\n", "9.99", 2, TOPLUSINFINITY, "1.e1")) return FALSE;
  if (!tc_round("9.99, up, 3\n", "9.99", 3, TOPLUSINFINITY, "9.99e0")) return FALSE;
  if (!tc_round("-1.2, up, 1\n", "-1.2", 1, TOPLUSINFINITY, "-1.e0")) return FALSE;
  if (!tc_round("testing overflow, up\n", maxexp(buf, "+9.9E"), 1, TOPLUSINFINITY, "NaN")) return FALSE;
  if (!tc_round("1.2, infinity, 1\n", "1.2", 1, TOINFINITY, "2.e0")) return FALSE;
  if (!tc_round("-1.2, infinity, 1\n", "-1.2", 1, TOINFINITY, "-2.e0")) return FALSE;
  if (!tc_round("testing overflow, infinity, pos\n", buf, 1, TOINFINITY, "NaN")) return FALSE;
  buf[0] = '-'; /* -9.9Emaxint */
  if (!tc_round("testing overflow, infinity, neg\n", buf, 1, TOINFINITY, "NaN")) return FALSE;
  if (!tc_round("1.9, zero, 1\n", "1.9", 1, TOZERO, "1.e0")) return FALSE;
  if (!tc_round("-1.9, zero, 1\n", "-1.9", 1, TOZERO, "-1.e0")) return FALSE;
  if (!tc_round("1.26, nearest, 2\n", "1.26", 2, TONEAREST, "1.3e0")) return FALSE;
  if (!tc_round("1.2501, nearest, 2\n", "1.2501", 2, TONEAREST, "1.3e0")) return FALSE;
  if (!tc_round("1.2499, nearest, 2\n", "1.2499", 2, TONEAREST, "1.2e0")) return FALSE;
  if (!tc_round("1.25, nearest, 2\n", "1.25", 2, TONEAREST, "1.2e0")) return FALSE;
  if (!tc_round("1.35, nearest, 2\n", "1.35", 2, TONEAREST, "1.4e0")) return FALSE;
  if (!tc_round("-1.26, nearest, 2\n", "-1.26", 2, TONEAREST, "-1.3e0")) return FALSE;
  if (!tc_round("testing overflow, nearest, neg\n", buf, 1, TONEAREST, "NaN")) return FALSE;
  return TRUE;
}

static int tc_add(char* msg, char* val1, char* val2, int digits, char* result)
{
  floatstruct v1;
  floatstruct v2;
  floatstruct b1;
  floatstruct b2;
  floatstruct sum;
  int lg;
  char ok;
  char buf[30];

  float_create(&v1);
  float_create(&v2);
  float_create(&b1);
  float_create(&b2);
  float_create(&sum);

  printf("%s", msg);
  float_setscientific(&v1, val1, NULLTERMINATED);
  float_setscientific(&v2, val2, NULLTERMINATED);
  float_clone(&b1, &v1, EXACT);
  float_clone(&b2, &v2, EXACT);
  float_add(&sum, &v1, &v2, digits);

  float_getscientific(buf, 30, &sum);
  lg = strlen(result);
  ok = _cmp(&v1, &b1) == 0
       && _cmp(&v2, &b2) == 0
       && lg == strlen(buf) 
       && memcmp(buf, result, lg) == 0? TRUE : FALSE;

  float_free(&v1);
  float_free(&v2);
  float_free(&b1);
  float_free(&b2);
  float_free(&sum);

  return ok;
}

static int test_add()
{
  floatstruct v1;
  floatstruct v2;
  floatstruct save;
  char nmb[30];
  char nmb2[30];

  printf("\ntesting float_add\n");
  float_create(&v1);
  float_create(&v2);
  float_create(&save);
  maxexp(nmb, "+9.9e");

  if (!tc_add("invalid length\n", "0", "0", 0, "NaN")) return FALSE;
  if (!tc_add("NaN, both op\n", "NaN", "NaN", 3, "NaN")) return FALSE;
  if (!tc_add("NaN, first op\n", "NaN", "0", 3, "NaN")) return FALSE;
  if (!tc_add("NaN, second op\n", "0", "NaN", 3, "NaN")) return FALSE;
  if (!tc_add("0 + 0\n", "0", "0", 3, "0")) return FALSE;
  if (!tc_add("0 + 1\n", "0", "1", 3, "1.e0")) return FALSE;
  if (!tc_add("1 + 0\n", "1", "0", 3, "1.e0")) return FALSE;
  if (!tc_add("1 + 1\n", "1", "1", EXACT, "2.e0")) return FALSE;
  if (!tc_add("1 + 0.1\n", "1", "0.1", EXACT, "1.1e0")) return FALSE;
  if (!tc_add("0.1 + 1\n", "0.1", "1", EXACT, "1.1e0")) return FALSE;
  if (!tc_add("scale overflow, 1E-20 + 1\n", "1.E-20", "1", EXACT, "NaN")) return FALSE;
  if (!tc_add("1E-20 + 1\n", "1.E-20", "1", 10, "1.e0")) return FALSE;
  if (!tc_add("1.234 + 0.1\n", "1.234", "0.1", EXACT, "1.334e0")) return FALSE;
  if (!tc_add("1.234 + 0.12345\n", "1.234", "0.12345", EXACT, "1.35745e0")) return FALSE;
  if (!tc_add("1.234 + 0.12345, lg 5\n", "1.234", "0.12345", 5, "1.3574e0")) return FALSE;
  if (!tc_add("1.234 + 0.12345, lg 4\n", "1.234", "0.12345", 4, "1.357e0")) return FALSE;
  if (!tc_add("1.234 + 0.12345, lg 3\n", "1.234", "0.12345", 3, "1.35e0")) return FALSE;
  if (!tc_add("1.234 + 0.12345, lg 2\n", "1.234", "0.12345", 2, "1.3e0")) return FALSE;
  if (!tc_add("1.234 + 0.12345, lg 1\n", "1.234", "0.12345", 1, "1.e0")) return FALSE;
  if (!tc_add("123.001 + (-122.85)\n", "123.001", "-122.85", 1, "2.e-1")) return FALSE;
  if (!tc_add("123.0014 + (-122.954)\n", "123.0014", "-122.954", 1, "5.e-2")) return FALSE;
  if (!tc_add("123.0014 + (-122.9954)\n", "123.0014", "-122.9954", 1, "6.e-3")) return FALSE;
  if (!tc_add("123 + (-122.9954)\n", "123", "-122.9954", 1, "5.e-3")) return FALSE;
  if (!tc_add("2 + (-1.9954)\n", "2", "-1.9954", 1, "5.e-3")) return FALSE;
  if (!tc_add("(-1.9954) + 2\n", "-1.9954", "2", 1, "5.e-3")) return FALSE;
  if (!tc_add("1 + (-0.9954)\n", "1", "-0.9954", 1, "5.e-3")) return FALSE;
  if (!tc_add("1.00001 + (-0.9954)\n", "1.00001", "-0.9954", 1, "5.e-3")) return FALSE;
  if (!tc_add("1.00001 + (-0.99)\n", "1.00001", "-0.99", 1, "1.e-2")) return FALSE;
  if (!tc_add("1.00001 + (-0.9999999)\n", "1.00001", "-0.9999999", 1, "2.e-5")) return FALSE;
  if (!tc_add("max + 1E-10000\n", nmb, "1E-10000", 2, nmb+1)) return FALSE;
  if (!tc_add("overflow\n", nmb, nmb, 2, "NaN")) return FALSE;
  if (!tc_add("2 + (-1)\n", "2", "-1", 2, "1.e0")) return FALSE;
  if (!tc_add("(-1) + 2\n", "-1", "2", 2, "1.e0")) return FALSE;
  if (!tc_add("2 + (-0.1)\n", "2", "-0.1", EXACT, "1.9e0")) return FALSE;
  if (!tc_add("1 + (-0.1)\n", "1", "-0.1", EXACT, "9.e-1")) return FALSE;
  if (!tc_add("1 + (-0.1), lg 1\n", "1", "-0.1", 1, "9.e-1")) return FALSE;
  if (!tc_add("123 + (-100)\n", "123", "-100", EXACT, "2.3e1")) return FALSE;
  if (!tc_add("(-123) + 100\n", "-123", "100", EXACT, "-2.3e1")) return FALSE;
  if (!tc_add("123 + (-120)\n", "123", "-120", EXACT, "3.e0")) return FALSE;
  if (!tc_add("123 + (-124)\n", "123", "-124", EXACT, "-1.e0")) return FALSE;
  if (!tc_add("123 + (-123)\n", "123", "-123", EXACT, "0")) return FALSE;
  if (!tc_add("123.04 + (-123)\n", "123.04", "-123", EXACT, "4.e-2")) return FALSE;
  if (!tc_add("123.04 + (-123)\n", "123.04", "-123", 1, "4.e-2")) return FALSE;
  if (!tc_add("123.24 + (-123.11)\n", "123.24", "-123.11", 1, "1.3e-1")) return FALSE;
  if (!tc_add("123.046 + (-123.115)\n", "123.046", "-123.115", 1, "-7.e-2")) return FALSE;
  if (!tc_add("123 + (-120), lg 1\n", "123", "-120", 1, "3.e0")) return FALSE;
  if (!tc_add("underflow\n", minexp(nmb, "1.1e"), minexp(nmb2, "-1e"), 5, "NaN")) return FALSE;
  if (!tc_add("underflow due to borrow\n", minexp(nmb, "2.0001e"), minexp(nmb2, "-1.99955e"), 5, "NaN")) return FALSE;
  if (!tc_add("underflow due to cancel\n", minexp(nmb, "1.234e"), minexp(nmb2, "-1.231e"), 5, "NaN")) return FALSE;
  printf("%s\n", "in place add, first");
  float_setscientific(&v1, "120", NULLTERMINATED);
  float_setscientific(&v2, "-3.4e-1", NULLTERMINATED);
  float_clone(&save, &v2, EXACT);
  float_add(&v1, &v1, &v2, EXACT);
  float_getscientific(nmb, 30, &v1);
  if (memcmp("1.1966e2", nmb, 9) != 0 || _cmp(&v2, &save) != 0) return FALSE;
  printf("%s\n", "in place add, second");
  float_clone(&save, &v1, EXACT);
  float_add(&v2, &v1, &v2, EXACT);
  float_getscientific(nmb, 30, &v2);
  if (memcmp("1.1932e2", nmb, 9) != 0 || _cmp(&v1, &save) != 0) return FALSE;
  printf("%s\n", "in place add, both");
  float_add(&v2, &v2, &v2, EXACT);
  float_getscientific(nmb, 30, &v2);
  if (memcmp("2.3864e2", nmb, 9) != 0) return FALSE;

  float_free(&v1);
  float_free(&v2);
  float_free(&save);
  return TRUE;
}

static int tc_sub(char* msg, char* val1, char* val2, int digits, char* result)
{
  floatstruct v1;
  floatstruct v2;
  floatstruct b1;
  floatstruct b2;
  floatstruct diff;
  int lg;
  char ok;
  char buf[30];

  float_create(&v1);
  float_create(&v2);
  float_create(&b1);
  float_create(&b2);
  float_create(&diff);

  printf("%s", msg);
  float_setscientific(&v1, val1, NULLTERMINATED);
  float_setscientific(&v2, val2, NULLTERMINATED);
  float_clone(&b1, &v1, EXACT);
  float_clone(&b2, &v2, EXACT);
  float_sub(&diff, &v1, &v2, digits);

  float_getscientific(buf, 30, &diff);
  lg = strlen(result);
  ok = _cmp(&v1, &b1) == 0
       && _cmp(&v2, &b2) == 0
       && lg == strlen(buf) 
       && memcmp(buf, result, lg) == 0? TRUE : FALSE;

  float_free(&v1);
  float_free(&v2);
  float_free(&b1);
  float_free(&b2);
  float_free(&diff);

  return ok;
}

static int test_sub()
{
  floatstruct v1;
  floatstruct v2;
  floatstruct save;
  char nmb[30];

  printf("\ntesting float_sub\n");
  float_create(&v1);
  float_create(&v2);
  float_create(&save);
  if (!tc_sub("invalid length\n", "0", "0", 0, "NaN")) return FALSE;
  if (!tc_sub("NaN, both op\n", "NaN", "NaN", 3, "NaN")) return FALSE;
  if (!tc_sub("NaN, first op\n", "NaN", "0", 3, "NaN")) return FALSE;
  if (!tc_sub("NaN, second op\n", "0", "NaN", 3, "NaN")) return FALSE;
  if (!tc_sub("0 - 0\n", "0", "0", 3, "0")) return FALSE;
  if (!tc_sub("0 - 1\n", "0", "1", 3, "-1.e0")) return FALSE;
  if (!tc_sub("1 - 0\n", "1", "0", 3, "1.e0")) return FALSE;
  if (!tc_sub("1 - 1\n", "1", "1", EXACT, "0")) return FALSE;
  if (!tc_sub("1 - 0.1\n", "1", "0.1", EXACT, "9.e-1")) return FALSE;
  if (!tc_sub("0.1 - 1\n", "0.1", "1", EXACT, "-9.e-1")) return FALSE;
  printf("%s\n", "in place sub, first");
  float_setscientific(&v1, "120", NULLTERMINATED);
  float_setscientific(&v2, "-3.4e-1", NULLTERMINATED);
  float_clone(&save, &v2, EXACT);
  float_sub(&v1, &v1, &v2, EXACT);
  float_getscientific(nmb, 30, &v1);
  if (memcmp("1.2034e2", nmb, 9) != 0 || _cmp(&v2, &save) != 0) return FALSE;
  printf("%s\n", "in place sub, second");
  float_clone(&save, &v1, EXACT);
  float_sub(&v2, &v1, &v2, EXACT);
  float_getscientific(nmb, 30, &v2);
  if (memcmp("1.2068e2", nmb, 9) != 0 || _cmp(&v1, &save) != 0) return FALSE;
  printf("%s\n", "in place sub, both");
  float_sub(&v2, &v2, &v2, EXACT);
  if (!float_iszero(&v2)) return FALSE;
  float_free(&v1);
  float_free(&v2);
  float_free(&save);
  return TRUE;
}

static int tc_mul(char* msg, char* val1, char* val2, int digits, char* result)
{
 floatstruct v1;
  floatstruct v2;
  floatstruct b1;
  floatstruct b2;
  floatstruct prod;
  int lg;
  char ok;
  char buf[30];

  float_create(&v1);
  float_create(&v2);
  float_create(&b1);
  float_create(&b2);
  float_create(&prod);

  printf("%s", msg);
  float_setscientific(&v1, val1, NULLTERMINATED);
  float_setscientific(&v2, val2, NULLTERMINATED);
  float_clone(&b1, &v1, EXACT);
  float_clone(&b2, &v2, EXACT);
  float_mul(&prod, &v1, &v2, digits);

  float_getscientific(buf, 30, &prod);
  lg = strlen(result);
  ok = _cmp(&v1, &b1) == 0
       && _cmp(&v2, &b2) == 0
       && lg == strlen(buf) 
       && memcmp(buf, result, lg) == 0? TRUE : FALSE;

  float_free(&v1);
  float_free(&v2);
  float_free(&b1);
  float_free(&b2);
  float_free(&prod);

  return ok;
}

static int test_mul()
{
  floatstruct v1;
  floatstruct v2;
  floatstruct save;
  char nmb[30];
  char nmb2[30];

  printf("\ntesting float_mul\n");
  float_create(&v1);
  float_create(&v2);
  float_create(&save);

  if (!tc_mul("invalid length\n", "0", "0", 0, "NaN")) return FALSE;
  if (!tc_mul("NaN, both op\n", "NaN", "NaN", 3, "NaN")) return FALSE;
  if (!tc_mul("NaN, first op\n", "NaN", "0", 3, "NaN")) return FALSE;
  if (!tc_mul("NaN, second op\n", "0", "NaN", 3, "NaN")) return FALSE;
  if (!tc_mul("0 * 0\n", "0", "0", 3, "0")) return FALSE;
  if (!tc_mul("0 * 1\n", "0", "1", 3, "0")) return FALSE;
  if (!tc_mul("1 * 0\n", "1", "0", 3, "0")) return FALSE;
  if (!tc_mul("1 * 1\n", "1", "1", EXACT, "1.e0")) return FALSE;
  if (!tc_mul("1 * -1\n", "1", "-1", EXACT, "-1.e0")) return FALSE;
  if (!tc_mul("-1 * 1\n", "-1", "1", EXACT, "-1.e0")) return FALSE;
  if (!tc_mul("-1 * -1\n", "-1", "-1", EXACT, "1.e0")) return FALSE;
  if (!tc_mul("2 * 3\n", "2", "3", EXACT, "6.e0")) return FALSE;
  if (!tc_mul("3 * 4\n", "3", "4", EXACT, "1.2e1")) return FALSE;
  if (!tc_mul("123 * 456\n", "123", "456", EXACT, "5.6088e4")) return FALSE;
  if (!tc_mul("3E100 * 4E-100\n", "3E100", "4E-100", EXACT, "1.2e1")) return FALSE;
  if (!tc_mul("big exp diff\n", maxexp(nmb, "5e"), minexp(nmb2, "2e"), EXACT, "1.e0")) return FALSE;
  if (!tc_mul("almost overflow\n", maxexp(nmb, "4.9e"), "2", EXACT, maxexp(nmb2, "9.8e"))) return FALSE;
  if (!tc_mul("overflow\n", maxexp(nmb, "5e"), "2", EXACT, "NaN")) return FALSE;
  if (!tc_mul("big overflow\n", maxexp(nmb, "5e"), nmb, EXACT, "NaN")) return FALSE;
  if (!tc_mul("almost underflow\n", minexp(nmb, "5e"), "0.2", EXACT, minexp(nmb2, "1.e"))) return FALSE;
  if (!tc_mul("underflow\n", minexp(nmb, "4e"), "0.2", EXACT, "NaN")) return FALSE;
  if (!tc_mul("big underflow\n", minexp(nmb, "1e"), nmb, EXACT, "NaN")) return FALSE;
  if (!tc_mul("scale overflow\n", "1.2345678901", "1.2345678901", EXACT, "NaN")) return FALSE;
  printf("%s\n", "in place mul, first");
  float_setscientific(&v1, "2", NULLTERMINATED);
  float_setscientific(&v2, "-3", NULLTERMINATED);
  float_clone(&save, &v2, EXACT);
  float_mul(&v1, &v1, &v2, EXACT);
  float_getscientific(nmb, 30, &v1);
  if (memcmp("-6.e0", nmb, 6) != 0 || _cmp(&v2, &save) != 0) return FALSE;
  printf("%s\n", "in place mul, second");
  float_clone(&save, &v1, EXACT);
  float_mul(&v2, &v1, &v2, EXACT);
  float_getscientific(nmb, 30, &v2);
  if (memcmp("1.8e1", nmb, 6) != 0 || _cmp(&v1, &save) != 0) return FALSE;
  printf("%s\n", "in place mul, both");
  float_mul(&v2, &v2, &v2, EXACT);
  float_getscientific(nmb, 30, &v2);
  if (memcmp("3.24e2", nmb, 7) != 0) return FALSE;
  float_free(&v1);
  float_free(&v2);
  float_free(&save);
  return TRUE;
}

static int tc_div(char* msg, char* val1, char* val2, int digits, char* result)
{
 floatstruct v1;
  floatstruct v2;
  floatstruct b1;
  floatstruct b2;
  floatstruct quot;
  int lg;
  char ok;
  char buf[30];

  float_create(&v1);
  float_create(&v2);
  float_create(&b1);
  float_create(&b2);
  float_create(&quot);

  printf("%s", msg);
  float_setscientific(&quot, "1.823452", NULLTERMINATED);
  float_setscientific(&v1, val1, NULLTERMINATED);
  float_setscientific(&v2, val2, NULLTERMINATED);
  float_clone(&b1, &v1, EXACT);
  float_clone(&b2, &v2, EXACT);
  float_div(&quot, &v1, &v2, digits);

  float_getscientific(buf, 30, &quot);
  lg = strlen(result);
  ok = _cmp(&v1, &b1) == 0
       && _cmp(&v2, &b2) == 0
       && lg == strlen(buf) 
       && memcmp(buf, result, lg) == 0? TRUE : FALSE;

  float_free(&v1);
  float_free(&v2);
  float_free(&b1);
  float_free(&b2);
  float_free(&quot);

  return ok;
}

static int test_div()
{
  floatstruct v1;
  floatstruct v2;
  floatstruct save;
  char nmb[30];
  char nmb2[30];

  printf("\ntesting float_div\n");
  float_create(&v1);
  float_create(&v2);
  float_create(&save);

  if(!tc_div("invalid digits\n", "1", "1", EXACT, "NaN")) return FALSE;
  if(!tc_div("NaN, both\n", "NaN", "NaN", 5, "NaN")) return FALSE;
  if(!tc_div("NaN, first\n", "NaN", "1", 5, "NaN")) return FALSE;
  if(!tc_div("NaN, second\n", "1", "NaN", 5, "NaN")) return FALSE;
  if(!tc_div("0/1\n", "0", "1", 5, "0")) return FALSE;
  if(!tc_div("1/0\n", "1", "0", 5, "NaN")) return FALSE;
  if(!tc_div("0/0\n", "0", "0", 5, "NaN")) return FALSE;
  if(!tc_div("1/1\n", "1", "1", 5, "1.e0")) return FALSE;
  if(!tc_div("(-1)/1\n", "-1", "1", 5, "-1.e0")) return FALSE;
  if(!tc_div("1/(-1)\n", "1", "-1", 5, "-1.e0")) return FALSE;
  if(!tc_div("(-1)/(-1)\n", "-1", "-1", 5, "1.e0")) return FALSE;
  if(!tc_div("1/2\n", "1", "2", 5, "5.e-1")) return FALSE;
  if(!tc_div("1/3\n", "1", "3", 5, "3.3333e-1")) return FALSE;
  if(!tc_div("4/3\n", "4", "3", 5, "1.33333e0")) return FALSE;
  if(!tc_div("40/0.3\n", "40", "0.3", 5, "1.33333e2")) return FALSE;
  if(!tc_div("0.4/0.03\n", "0.4", "0.03", 5, "1.33333e1")) return FALSE;
  if(!tc_div("400/30\n", "400", "30", 5, "1.33333e1")) return FALSE;
  if(!tc_div("400/30000\n", "400", "30000", 5, "1.33333e-2")) return FALSE;
  if(!tc_div("0.0004/0.3\n", "0.0004", "0.3", 5, "1.33333e-3")) return FALSE;
  if(!tc_div("integer quotient, 1/3\n", "1", "3", INTQUOT, "0")) return FALSE;
  if(!tc_div("integer quotient, 4/3\n", "4", "3", INTQUOT, "1.e0")) return FALSE;
  if(!tc_div("integer quotient, 4/3\n", "4", "3", INTQUOT, "1.e0")) return FALSE;
  if(!tc_div("integer quotient, 4/3\n", "4", "0.3", INTQUOT, "1.3e1")) return FALSE;
  if(!tc_div("integer quotient, 1/3\n", "1", "0.3", INTQUOT, "3.e0")) return FALSE;
  if(!tc_div("integer quotient, 1/30\n", "1", "30", INTQUOT, "0")) return FALSE;
  if(!tc_div("almost overflow\n", maxexp(nmb, "1.e"), "0.3", 5, maxexp(nmb2, "3.3333e"))) return FALSE;
  if(!tc_div("overflow\n", maxexp(nmb, "4.e"), "0.3", 5, "NaN")) return FALSE;
  if(!tc_div("big overflow\n", maxexp(nmb, "4.e"), minexp(nmb2, "1.e"), 5, "NaN")) return FALSE;
  if(!tc_div("almost underflow\n", minexp(nmb, "4.e"), "3", 5, minexp(nmb2, "1.33333e"))) return FALSE;
  if(!tc_div("underflow\n", minexp(nmb, "1.e"), "3", 5, "NaN")) return FALSE;
  if(!tc_div("big underflow\n", minexp(nmb, "1.e"), maxexp(nmb2, "1.e"), 5, "NaN")) return FALSE;
  if(!tc_div("scale overflow\n", "1", "2", 20, "NaN")) return FALSE;
  printf("%s\n", "in place div, first");
  float_setscientific(&v1, "1", NULLTERMINATED);
  float_setscientific(&v2, "-2", NULLTERMINATED);
  float_clone(&save, &v2, EXACT);
  float_div(&v1, &v1, &v2, 5);
  float_getscientific(nmb, 30, &v1);
  if (memcmp("-5.e-1", nmb, 7) != 0 || _cmp(&v2, &save) != 0) return FALSE;
  printf("%s\n", "in place div, second");
  float_clone(&save, &v1, EXACT);
  float_div(&v2, &v1, &v2, 5);
  float_getscientific(nmb, 30, &v2);
  if (memcmp("2.5e-1", nmb, 7) != 0 || _cmp(&v1, &save) != 0) return FALSE;
  printf("%s\n", "in place div, both");
  float_div(&v2, &v2, &v2, 5);
  float_getscientific(nmb, 30, &v2);
  if (memcmp("1.e0", nmb, 5) != 0) return FALSE;
  float_free(&v1);
  float_free(&v2);
  float_free(&save);
  return TRUE;
}

static int tc_sqrt(char* msg, char* val, int digits, char* result)
{
  floatstruct v;
  int lg;
  char buf[30];

  printf("%s", msg);
  float_create(&v);
  float_setscientific(&v, val, NULLTERMINATED);
  float_sqrt(&v, digits);
  float_round(&v, digits - 1, TONEAREST);
  float_getscientific(buf, 30, &v);
  float_free(&v);
  lg = strlen(result);
  return strlen(buf) == lg && memcmp(buf, result, lg) == 0? TRUE : FALSE;
}

static int test_sqrt()
{
  printf("\ntesting float_sqrt\n");

  if (!tc_sqrt("invalid length\n", "1", 0, "NaN")) return FALSE;
  if (!tc_sqrt("NaN\n", "NaN", 5, "NaN")) return FALSE;
  if (!tc_sqrt("negative value\n", "-1", 5, "NaN")) return FALSE;
  if (!tc_sqrt("0\n", "0", 5, "0")) return FALSE;
  if (!tc_sqrt("1\n", "1", 5, "1.e0")) return FALSE;
  if (!tc_sqrt("4\n", "4", 5, "2.e0")) return FALSE;
  if (!tc_sqrt("16\n", "16", 5, "4.e0")) return FALSE;
  if (!tc_sqrt("0.64\n", "0.64", 5, "8.e-1")) return FALSE;
  if (!tc_sqrt("0.04\n", "0.04", 5, "2.e-1")) return FALSE;
  return TRUE;
}

static int tc_setbcnum(char* msg, char* value, int scale, char* result)
{
  bc_num bc;
  floatstruct f;
  char buf[30];
  int lg;

  printf("%s", msg);
  float_create(&f);
  bc = NULL;
  bc_str2num(&bc, value, scale);
  float_setbcnum(&f, &bc);
  float_getscientific(buf, 30, &f);
  float_free(&f);
  lg = strlen(result);
  return bc == NULL && lg == strlen(buf) && memcmp(buf, result, lg) == 0? TRUE : FALSE;
}

static int test_setbcnum()
{
  bc_num bc;
  floatstruct f;
  int refs;
  char buf[30];

  printf("\ntesting float_setbcnum\n");
  float_create(&f);
  float_setzero(&f);
  if (!tc_setbcnum("test 0\n", "0", 0, "0")) return FALSE;
  if (!tc_setbcnum("test 0, lg 2\n", "0", 1, "0")) return FALSE;
  if (!tc_setbcnum("test 1\n", "1", 0, "1.e0")) return FALSE;
  if (!tc_setbcnum("test 1, lg 2\n", "1", 1, "1.e0")) return FALSE;
  if (!tc_setbcnum("test 10\n", "10", 0, "1.e1")) return FALSE;
  if (!tc_setbcnum("test 10, lg 2\n", "10", 1, "1.e1")) return FALSE;
  if (!tc_setbcnum("test 12, lg 2\n", "12", 1, "1.2e1")) return FALSE;
  if (!tc_setbcnum("test 0.1\n", "0.1", 1, "1.e-1")) return FALSE;
  if (!tc_setbcnum("test 0.01\n", "0.01", 2, "1.e-2")) return FALSE;
  if (!tc_setbcnum("test MAXSCALE\n", "12345678901234567890", 50, "1.234567890123456789e19")) return FALSE;
  bc = NULL;
  printf("%s\n", "test NULL");
  float_setbcnum(&f, &bc);
  bc_free_num(&bc);
  if(!float_isnan(&f)) return FALSE;
  printf("%s\n", "test refcount > 0");
  refs = _one_->n_refs;
  bc = bc_copy_num(_one_);
  float_setbcnum(&f, &bc);
  float_getscientific(buf, 30, &f);
  if (memcmp(buf, "1.e0", 5) != 0 || _one_->n_refs != refs || bc != NULL) return FALSE;
  float_free(&f);
  return TRUE;
}

static int tc_setinteger(char* msg, int value, char* result)
{
  floatstruct f;
  int lg;
  char buf[30];

  printf("%s", msg);
  float_create(&f);
  float_setinteger(&f, value);
  float_getscientific(buf, 30, &f);
  lg = strlen(result);
  float_free(&f);
  return (lg == strlen(buf) && memcmp(buf, result, lg) == 0);
}

static int test_setinteger()
{
  printf("\ntesting float_setinteger\n");

  if (!tc_setinteger("test 0\n", 0, "0")) return FALSE;
  if (!tc_setinteger("test -5\n", -5, "-5.e0")) return FALSE;
  return TRUE;
}

static int tc_int(char* msg, char* value, char* result)
{
  floatstruct f;
  int lg;
  char buf[30];

  printf("%s", msg);
  float_create(&f);
  float_setscientific(&f, value, NULLTERMINATED);
  float_int(&f);
  float_getscientific(buf, 30, &f);
  lg = strlen(result);
  float_free(&f);
  return (lg == strlen(buf) && memcmp(buf, result, lg) == 0);
}

static int test_int()
{
  printf("\ntesting float_int\n");
  if (!tc_int("NaN\n", "NaN", "NaN")) return FALSE;
  if (!tc_int("0\n", "0", "0")) return FALSE;
  if (!tc_int("1\n", "1", "1.e0")) return FALSE;
  if (!tc_int("10\n", "10", "1.e1")) return FALSE;
  if (!tc_int("10.1\n", "10.1", "1.e1")) return FALSE;
  if (!tc_int("0.1\n", "0.1", "0")) return FALSE;
  if (!tc_int("-1\n", "-1", "-1.e0")) return FALSE;
  if (!tc_int("-10\n", "-10", "-1.e1")) return FALSE;
  if (!tc_int("-10.1\n", "-10.1", "-1.e1")) return FALSE;
  if (!tc_int("-0.1\n", "-0.1", "0")) return FALSE;
  return TRUE;
}

static int tc_frac(char* msg, char* value)
{
  floatstruct f, r;

  printf("%s", msg);
  float_create(&f);
  float_create(&r);
  float_setscientific(&f, value, NULLTERMINATED);
  float_clone(&r, &f, EXACT);
  float_int(&r);
  float_sub(&r, &f, &r, EXACT);
  float_frac(&f);
  return _cmp(&f, &r) == 0;
}

static int test_frac()
{
  printf("\ntesting float_frac\n");
  if (!tc_frac("NaN\n", "NaN")) return FALSE;
  if (!tc_frac("0\n", "0")) return FALSE;
  if (!tc_frac("1\n", "1")) return FALSE;
  if (!tc_frac("10\n", "10")) return FALSE;
  if (!tc_frac("10.1\n", "10.1")) return FALSE;
  if (!tc_frac("0.1\n", "0.1")) return FALSE;
  if (!tc_frac("-1\n", "-1")) return FALSE;
  if (!tc_frac("-10\n", "-10")) return FALSE;
  if (!tc_frac("-10.1\n", "-10.1")) return FALSE;
  if (!tc_frac("-0.1\n", "-0.1")) return FALSE;
  return TRUE;
}

static int tc_divmod(char* msg, char* dvd, char* dvs, int dig, char* q, char* r)
{
  int result;
  floatstruct v1;
  floatstruct v2;
  floatstruct b1;
  floatstruct b2;
  floatstruct r1;
  floatstruct r2;

  printf("%s", msg);

  float_create(&v1);
  float_create(&v2);
  float_create(&b1);
  float_create(&b2);
  float_create(&r1);
  float_create(&r2);

  float_setscientific(&r1, "-1.821923", NULLTERMINATED);
  float_setscientific(&r2, "-1.821923", NULLTERMINATED);
  float_setscientific(&v1, dvd, NULLTERMINATED);
  float_setscientific(&v2, dvs, NULLTERMINATED);

  float_clone(&b1, &v1, EXACT);
  float_clone(&b2, &v2, EXACT);

  float_divmod(&r1, &r2, &v1, &v2, dig);

  result = (scmp(&r1, q) && scmp(&r2, r) && _cmp(&v1, &b1) == 0 && _cmp(&v2, &b2) == 0);

  float_free(&v1);
  float_free(&v2);
  float_free(&b1);
  float_free(&b2);
  float_free(&r1);
  float_free(&r2);
  return result;
}

static int test_divmod()
{
  floatstruct f1;
  floatstruct f2;
  floatstruct f3;
  floatstruct vlg;
  bc_num bcvlg;
  char nmb1[30];
  char nmb2[30];

  printf("\ntesting float_divmod\n");
  bcvlg = NULL;
  float_create(&f1);
  float_create(&f2);
  float_create(&f3);
  float_create(&vlg);
  bc_str2num(&bcvlg, "12345678901234567890", 0);
  float_setbcnum(&vlg, &bcvlg);

  float_setinteger(&f1, 1);
  printf("%s", "test rem == quot\n");
  float_divmod(&f1, &f1, &f1, &f1, 5);
  if (!float_isnan(&f1)) return FALSE;
  if (!tc_divmod("NaN, both\n", "NaN", "NaN", 5, "NaN", "NaN")) return FALSE;
  if (!tc_divmod("NaN, 1. op\n", "NaN", "1", 5, "NaN", "NaN")) return FALSE;
  if (!tc_divmod("NaN, 2. op\n", "1", "NaN", 5, "NaN", "NaN")) return FALSE;
  if (!tc_divmod("invalid digits\n", "1", "1", -5, "NaN", "NaN")) return FALSE;
  if (!tc_divmod("test divide by 0\n", "1", "0", 5, "NaN", "NaN")) return FALSE;
  if (!tc_divmod("test scale too long\n", "1", "1", 100, "NaN", "NaN")) return FALSE;
  if (!tc_divmod("0/1\n", "0", "1", 5, "0", "0")) return FALSE;
  if (!tc_divmod("1/100, INTQUOT\n", "1", "100", INTQUOT, "0", "1.e0")) return FALSE;
  if (!tc_divmod("INTQUOT, scale too long\n", "1e100", "1", INTQUOT, "NaN", "NaN")) return FALSE;
  if (!tc_divmod("1/2, INTQUOT\n", "1", "2", INTQUOT, "0", "1.e0")) return FALSE;
  if (!tc_divmod("3/2, INTQUOT\n", "3", "2", INTQUOT, "1.e0", "1.e0")) return FALSE;
  if (!tc_divmod("4/2, INTQUOT\n", "4", "2", INTQUOT, "2.e0", "0")) return FALSE;
  if (!tc_divmod("213/13, lg 3\n", "213", "13", 3, "1.63e1", "1.1e0")) return FALSE;
  if (!tc_divmod("17/16, INTQUOT\n", "17", "16", INTQUOT, "1.e0", "1.e0")) return FALSE;
  if (!tc_divmod("1.7/1.6, INTQUOT\n", "1.7", "1.6", INTQUOT, "1.e0", "1.e-1")) return FALSE;
  if (!tc_divmod("1.7/1.6, lg 3\n", "1.7", "1.6", 3, "1.06e0", "4.e-3")) return FALSE;
  if (!tc_divmod("underflow in quot\n", minexp(nmb1, "1.7e"), "1.6e1", 3, "NaN", "NaN")) return FALSE;
  if (!tc_divmod("overflow in quot\n", maxexp(nmb1, "1.7e"), "1.6e-1", 3, "NaN", "NaN")) return FALSE;
  if (!tc_divmod("underflow in rem\n", minexp(nmb1, "1.7e"), minexp(nmb2, "1.6e"), INTQUOT, "NaN", "NaN")) return FALSE;
  printf("%s", "long divisor\n");
  float_setzero(&f1);
  float_setzero(&f2);
  bc_str2num(&bcvlg, "12345678901234567890", 0);
  float_setbcnum(&f3, &bcvlg);
  float_divmod(&f1, &f2, &f3, &f3, 3);
  if (!float_isnan(&f1) || !float_isnan(&f2) || float_cmp(&vlg, &f3) != 0) return FALSE;
  printf("%s", "dividend and divisor overwritten by quotient\n");
  float_setinteger(&f1, 2);
  float_setnan(&f2);
  float_divmod(&f1, &f2, &f1, &f1, 3);
  if (!scmp(&f1, "1.e0") || !scmp(&f2, "0")) return FALSE;
  printf("%s", "dividend and divisor overwritten by remainder\n");
  float_setinteger(&f1, 2);
  float_setnan(&f2);
  float_divmod(&f2, &f1, &f1, &f1, 3);
  if (!scmp(&f2, "1.e0") || !scmp(&f1, "0")) return FALSE;
  printf("%s", "dividend overwritten by quotient, divisor by remainder\n");
  float_setinteger(&f1, 213);
  float_setinteger(&f2, 13);
  float_divmod(&f1, &f2, &f1, &f2, 3);
  if (!scmp(&f1, "1.63e1") || !scmp(&f2, "1.1e0")) return FALSE;
  printf("%s", "dividend overwritten by remainder, divisor by quotient\n");
  float_setinteger(&f1, 213);
  float_setinteger(&f2, 13);
  float_divmod(&f2, &f1, &f1, &f2, 3);
  if (!scmp(&f2, "1.63e1") || !scmp(&f1, "1.1e0")) return FALSE;
  float_free(&f1);
  float_free(&f2);
  float_free(&f3);
  float_free(&vlg);
  return TRUE;
}

static void _relerror(floatnum x1, floatnum x2)
{
  float_sub(x1, x1, x2, 3);
  if (!float_iszero(x1))
    float_div(x1, x1, x2, 3);
  float_abs(x1);
}

static char _cmprelerror(floatnum x1, floatnum x2, int exp)
{
  _relerror(x1, x2);
  return (float_iszero(x1) || float_getexponent(x1) < exp);
}

static void _sub_ulp(floatnum x, int sz)
{
  floatstruct delta;

  if (!float_iszero(x))
  {
    float_create(&delta);
    float_setinteger(&delta, 1);
    float_setexponent(&delta, float_getexponent(x)-sz);
    float_setsign(&delta, float_getsign(x));
    float_sub(x, x, &delta, EXACT);
    float_free(&delta);
  }
}

static int test_coshminus1near0()
{
  floatstruct x, x1, tmp, max;
  int i;
  char buf[50];

  float_create(&x);
  float_create(&x1);
  float_create(&tmp);
  float_create(&max);
  float_setzero(&max);

  printf("%s\n", "testing coshminus1near0");

  printf("verifying special argument x == 0:\n");
  float_setzero(&x);
  coshminus1near0(&x, 100);
  if (!float_iszero(&x))
  {
    printf("FAILED\n");
    return 0;
  }

  /* testing the validity of the series evaluation */
  printf("verifying result:\n");
  float_setasciiz(&x, "0.005");
  float_setasciiz(&tmp, ".000012500026041688368065243678286383060391"
                        "9872831398280829063464786043942297654015309"
                        "5900368700065231210065496027359337789353280"
                        "9524874015321513239965");
  coshminus1near0(&x, 100);
  if (!_cmprelerror(&tmp, &x, -99))
  {
    printf("FAILED\n");
    return 0;
  }

  printf("testing error limit:\n");
  /* overall scan for maximum relative error */
  float_setasciiz(&tmp, ".00001");
  for (i = -1; ++i <= 1000;)
  {
    float_muli(&x, &tmp, i, EXACT);
    _sub_ulp(&x, 101);
    float_clone(&x1, &x, EXACT);
    coshminus1near0(&x,100);
    coshminus1near0(&x1, 110);
    _relerror(&x1, &x);
    if (float_cmp(&x1, &max) > 0)
    {
      float_clone(&max, &x1, EXACT);
      if (float_getexponent(&x1) >= -99)
      {
        printf("exceeding error for test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _coshminus1near0: %s\n", buf);

  float_free(&tmp);
  float_free(&x);
  float_free(&x1);
  float_free(&max);
  return 1;
}

static int test_artanhnear0()
{
  floatstruct x, x1, tmp, max;
  int i;
  char buf[50];

  float_create(&x);
  float_create(&x1);
  float_create(&tmp);
  float_create(&max);
  float_setzero(&max);

  printf("%s\n", "testing artanhnear0");

  printf("verifying special argument x == 0:\n");
  float_setzero(&x);
  artanhnear0(&x, 100);
  if (!float_iszero(&x))
  {
    printf("FAILED\n");
    return 0;
  }

  /* testing correct replacement of artanh by id */

  printf("small x handling:\n");
  float_setasciiz(&x, "6.7e-51");
  float_clone(&x1, &x, EXACT);
  artanhnear0(&x, 100);
  artanhnear0(&x1, 110);
  if (!_cmprelerror(&x1, &x, -99))
  {
    printf("FAILED\n");
    return 0;
  }

  /* testing the validity of the series evaluation */
  printf("verifying result:\n");
  float_setasciiz(&x, "0.01");
  artanhnear0(&x, 100);
  float_setasciiz(&x1,".0100003333533347620158821075514042238870"
                      "97308807344335621654492648224836625944612"
                      "34119694455592889442890269461918262602394"
                      "9110421598243");
  if (!_cmprelerror(&x1, &x, -99))
  {
    printf("FAILED\n");
    return 0;
  }

  printf("testing error limit:\n");
  /* overall scan for maximum relative error */
  float_setscientific(&tmp, ".0001", NULLTERMINATED);
  for (i = -101; ++i <= 100;)
  {
    float_muli(&x, &tmp, i, EXACT);
    _sub_ulp(&x, 101);
    float_clone(&x1, &x, EXACT);
    artanhnear0(&x,100);
    artanhnear0(&x1, 110);
    _relerror(&x1, &x);
    if (float_cmp(&x1, &max) > 0)
    {
      float_clone(&max, &x1, EXACT);
      if (float_getexponent(&x1) >= -99)
      {
        printf("exceeding error for test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error artanhnear0: %s\n", buf);
  float_free(&tmp);
  float_free(&x);
  float_free(&x1);
  float_free(&max);
  return 1;
}

static int test_arctannear0()
{
  floatstruct x, x1, tmp, max;
  int i;
  char buf[50];

  float_create(&x);
  float_create(&x1);
  float_create(&tmp);
  float_create(&max);
  float_setzero(&max);

  printf("%s\n", "testing arctannear0");

  printf("verifying special argument x == 0:\n");
  float_setzero(&x);
  arctannear0(&x, 100);
  if (!float_iszero(&x))
  {
    printf("FAILED\n");
    return 0;
  }

  /* testing correct replacement of arctan by id */

  printf("small x handling:\n");
  float_setasciiz(&x, "6.7e-51");
  float_clone(&x1, &x, EXACT);
  arctannear0(&x, 100);
  arctannear0(&x1, 110);
  if (!_cmprelerror(&x1, &x, -99))
  {
    printf("FAILED\n");
    return 0;
  }

  /* testing the validity of the series evaluation */
  printf("verifying result:\n");
  float_setasciiz(&x, "0.01");
  arctannear0(&x, 100);
  float_setasciiz(&x1,".00999966668666523820634011620927954856136935"
                      "254437663962793941819645655320405877997944664"
                      "518667409041670798128407565295492085345632123");
  if (!_cmprelerror(&x1, &x, -99))
  {
    printf("FAILED for x == 0.01\n");
    return 0;
  }

  printf("testing error limit:\n");
  /* overall scan for maximum relative error */
  float_setscientific(&tmp, ".0001", NULLTERMINATED);
  for (i = -101; ++i <= 100;)
  {
    float_muli(&x, &tmp, i, EXACT);
    _sub_ulp(&x, 101);
    float_clone(&x1, &x, EXACT);
    arctannear0(&x,100);
    arctannear0(&x1, 110);
    _relerror(&x1, &x);
    if (float_cmp(&x1, &max) > 0)
    {
      float_clone(&max, &x1, EXACT);
      if (float_getexponent(&x1) >= -99)
      {
        printf("exceeding error for test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error arctannear0: %s\n", buf);
  float_free(&tmp);
  float_free(&x);
  float_free(&x1);
  float_free(&max);
  return 1;
}

static int test_cosminus1near0()
{
  floatstruct x, x1, tmp, max;
  int i;
  char buf[50];

  float_create(&x);
  float_create(&x1);
  float_create(&tmp);
  float_create(&max);
  float_setzero(&max);

  printf("%s\n", "testing cosminus1near0");

  printf("verifying special argument x == 0:\n");
  float_setzero(&x);
  cosminus1near0(&x, 100);
  if (!float_iszero(&x))
  {
    printf("FAILED\n");
    return 0;
  }

  /* testing the validity of the series evaluation */
  printf("verifying result:\n");
  float_setasciiz(&x, "0.01");
  cosminus1near0(&x, 100);
  float_setasciiz(&x1,"-.4999958333472221974206624779332678752941601"
                      "972288887772423135353991187035650836487445623"
                      "870262734376943805848695816541718420700624513737e-4");
  if (!_cmprelerror(&x1, &x, -99))
  {
    printf("FAILED for x == 0.01\n");
    return 0;
  }

  printf("testing error limit:\n");
  /* overall scan for maximum relative error */
  float_setscientific(&tmp, ".0001", NULLTERMINATED);
  for (i = -1; ++i <= 100;)
  {
    float_muli(&x, &tmp, i, EXACT);
    _sub_ulp(&x, 101);
    float_clone(&x1, &x, EXACT);
    cosminus1near0(&x,100);
    cosminus1near0(&x1, 110);
    _relerror(&x1, &x);
    if (float_cmp(&x1, &max) > 0)
    {
      float_clone(&max, &x1, EXACT);
      if (float_getexponent(&x1) >= -99)
      {
        printf("exceeding error for test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error cosminus1near0: %s\n", buf);
  float_free(&tmp);
  float_free(&x);
  float_free(&x1);
  float_free(&max);
  return 1;
}

static int test_lnxplus1near0()
{
  floatstruct x, x1, tmp, max;
  int i;
  char buf[50];

  printf("testing _lnxplus1near0\n");

  float_create(&x);
  float_create(&x1);
  float_create(&tmp);
  float_create(&max);

  /* verifying function result */
  printf("verifying result:\n");
  float_setasciiz(&x, "0.01");
  float_setasciiz(&tmp,".009950330853168082848215357544260741688"
                       "6796099400587978646095597668666642476389"
                       "1103268569656276016116452560517384648478");
  _lnxplus1near0(&x, 100);
  if (!_cmprelerror(&tmp, &x, -99))
  {
    printf("FAILED\n");
    return 0;
  }

  printf("verifying special argument x == 0:\n");
  float_setzero(&x);
  _lnxplus1near0(&x, 100);
  if (!float_iszero(&x))
  {
    printf("FAILED\n");
    return 0;
  }

  /* overall scan for maximum relative error */
  printf("testing error limit:\n");
  float_setzero(&max);
  float_setscientific(&tmp, ".0002", NULLTERMINATED);
  for (i = -100; ++i <= 100;)
  {
    float_muli(&x, &tmp, i, EXACT);
    _sub_ulp(&x, 101);
    float_clone(&x1, &x, EXACT);
    _lnxplus1near0(&x,100);
    _lnxplus1near0(&x1, 110);
    _relerror(&x1, &x);
    if (float_cmp(&x1, &max) > 0)
    {
      float_clone(&max, &x1, EXACT);
      if (float_getexponent(&x1) >= -99)
      {
        printf("exceeding error for test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _lnxplus1near0: %s\n", buf);
  float_free(&tmp);
  float_free(&x);
  float_free(&x1);
  float_free(&max);
  return 1;
}

static int test_lnreduce()
{
  floatstruct max, x, step, delta, ofs, tmp, lnfactor, cmp;
  int i;
  char buf[50];

  printf("testing _reduce\n");

  float_create(&x);
  float_create(&max);
  float_create(&step);
  float_create(&delta);
  float_create(&ofs);
  float_create(&tmp);
  float_create(&lnfactor);
  float_create(&cmp);
  float_setzero(&max);
  float_setasciiz(&step, "0.01");
  float_setasciiz(&ofs, "0.005");
  float_setasciiz(&delta, "0.0049999999");
  float_setasciiz(&cmp, "0.01");
  printf("testing boundary case -0.4\n");
  float_setasciiz(&x, "-.4");
  _lnreduce(&x, &lnfactor, 10);
  float_abs(&x);
  if (float_cmp(&x, &cmp) > 0)
  {
    float_getscientific(buf, 50, &x);
    printf("reduction failed for x=-0.4: %s\n", buf);
    return 0;
  }
  float_abs(&lnfactor);
  float_getsignificand(buf, 1, &lnfactor);
  if (float_getexponent(&lnfactor) >= 0
     || (float_getexponent(&lnfactor) == -1 && buf[0] >= '7'))
  {
    printf("guess incorrect for x=-0.4\n");
    return 1;
  }
  for (i = -41; ++i < 100;)
  {
    if (i == -1 || i == 0)
      continue;
    float_muli(&tmp, &step, i, EXACT);
    float_add(&tmp, &tmp, &ofs, EXACT);
    float_sub(&x, &tmp, &delta, EXACT);
    _lnreduce(&x, &lnfactor, 10);
    float_abs(&x);
    if(float_cmp(&x, &max) > 0)
    {
      float_clone(&max, &x, EXACT);
      if (float_cmp(&x, &cmp) > 0)
      {
        float_getscientific(buf, 50, &x);
        printf("reduction failed for test case %d: %s\n", i, buf);
        return 0;
      }
    }
    float_abs(&lnfactor);
    float_getsignificand(buf, 1, &lnfactor);
    if (float_getexponent(&lnfactor) >= 0
       || (float_getexponent(&lnfactor) == -1 && buf[0] >= '7'))
    {
      printf("guess incorrect for test case %d\n", i);
      return 1;
    }
    float_add(&x, &tmp, &delta, EXACT);
    _lnreduce(&x, &lnfactor, 10);
    float_abs(&x);
    if(float_cmp(&x, &max) > 0)
    {
      float_clone(&max, &x, EXACT);
      if (float_cmp(&x, &cmp) >0)
      {
        float_getscientific(buf, 50, &x);
        printf("reduction failed for test case %d: %s\n", i, buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max reduced value: %s\n", buf);
  float_free(&x);
  float_free(&max);
  float_free(&step);
  float_free(&delta);
  float_free(&ofs);
  float_free(&tmp);
  float_free(&lnfactor);
  float_free(&cmp);
  return 1;
}

static int test_lnxplus1lt1()
{
  floatstruct x, x1, max, step, ofs, tmp, ref;
  int i;
  char buf[50];

  printf("testing _lnxplus1lt1\n");

  float_create(&x);
  float_create(&max);
  float_create(&step);
  float_create(&ofs);
  float_create(&tmp);
  float_create(&ref);

  printf("verifying special argument x == 0:\n");
  float_setzero(&x);
  _lnxplus1lt1(&x, 100);
  if (!float_iszero(&x))
  {
    printf("FAILED\n");
    return 0;
  }

  printf("verifying special argument x == -0.4:\n");
  float_clone(&x, &cMinus0_4, EXACT);
  _lnxplus1lt1(&x, 100);
  float_setasciiz(&x1, "-.510825623765990683205514096303661934878110"
                       "79644576827017795355783668469448904879775651"
                       "81232794475220119962739557576971573560010034");
  if (!_cmprelerror(&x1, &x, -99))
  {
    printf("FAILED\n");
    return 0;
  }

  printf("verifying function results\n");
  float_setasciiz(&step, "0.005");
  float_clone(&ref, &step, EXACT);
  float_clone(&x, &step, EXACT);
  _lnxplus1near0(&ref, 130);
  i = 1;
  while(float_cmp(&x, &c1) < 0)
  {
    float_clone(&x1, &x, EXACT);
    _lnxplus1lt1(&x, 100);
    float_muli(&tmp, &ref, i, 130);
    _relerror(&tmp, &x);
    if (float_getexponent(&tmp) >= -99)
    {
      float_getscientific(buf, 50, &tmp);
      printf("%s\n", buf);
      printf("FAILED for test case %d\n", i);
      return 0;
    }
    float_mul(&tmp, &step, &x1, 130);
    float_add(&tmp, &tmp, &step, EXACT);
    float_add(&x, &x1, &tmp, 130);
    ++i;
  }
  float_setasciiz(&step, "-0.005");
  float_clone(&ref, &step, EXACT);
  float_clone(&x, &step, EXACT);
  _lnxplus1near0(&ref, 130);
  i = 1;
  while(float_cmp(&x, &cMinus0_4) >= 0)
  {
    float_clone(&x1, &x, EXACT);
    _lnxplus1lt1(&x, 100);
    float_muli(&tmp, &ref, i, 130);
    _relerror(&tmp, &x);
    if (float_getexponent(&tmp) >= -99)
    {
      float_getscientific(buf, 50, &tmp);
      printf("%s\n", buf);
      printf("FAILED for test case %d\n", i);
      return 0;
    }
    float_mul(&tmp, &step, &x1, 130);
    float_add(&tmp, &tmp, &step, EXACT);
    float_add(&x, &x1, &tmp, 130);
    ++i;
  }

  printf("testing error limit:\n");
  float_setzero(&max);
  float_setasciiz(&step, "0.01");
  float_setasciiz(&ofs, "0.00117436738983647399324678823992433673"
         "24672389823467828246764829833467849839838778485784386584539"
         "85343784568643548923998439823432792346");
  for(i = -41; ++i < 100;)
  {
    float_setinteger(&tmp, i);
    float_mul(&tmp, &tmp, &step, EXACT);
    float_add(&x, &tmp, &ofs, EXACT);
    float_clone(&tmp, &x, EXACT);
    _lnxplus1lt1(&x, 100);
    _lnxplus1lt1(&tmp, 110);
    _relerror(&x, &tmp);
    if (float_cmp(&x, &max) > 0)
    {
      float_clone(&max, &x, EXACT);
      if (float_getexponent(&x) >= -99)
      {
        printf("exceeding error in test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _lnxplus1: %s\n", buf);
  float_free(&x);
  float_free(&max);
  float_free(&step);
  float_free(&ofs);
  float_free(&tmp);
  float_free(&ref);
  return 1;
}

static int test_ln()
{
  floatstruct x, max, step, ofs, tmp;
  int i;
  char buf[50];

  printf("testing _ln\n");

  float_create(&x);
  float_create(&max);
  float_create(&step);
  float_create(&ofs);
  float_create(&tmp);

  printf("verifying special argument x == 1:\n");
  float_clone(&x, &c1, EXACT);
  _ln(&x, 100);
  if (!float_iszero(&x))
  {
    printf("FAILED\n");
    return 0;
  }

  printf("verifying function results:\n");
  for (i = 1; ++i < 10;)
  {
    switch(i)
    {
    case 2:
      float_clone(&tmp, &cLn2, 110);
      break;
    case 3:
      float_clone(&tmp, &cLn3, 110);
      break;
    case 4:
      float_muli(&tmp, &cLn2, 2, 110);
      break;
    case 5:
      float_sub(&tmp, &cLn10, &cLn2, 110);
      break;
    case 6:
      float_add(&tmp, &cLn2, &cLn3, 110);
      break;
    case 7:
      float_clone(&tmp, &cLn7, 110);
      break;
    case 8:
      float_muli(&tmp, &cLn2, 3, 110);
      break;
    case 9:
      float_muli(&tmp, &cLn3, 2, 110);
      break;
    }
    float_setinteger(&x, i);
    _ln(&x, 100);
    if (!_cmprelerror(&tmp, &x, -99))
    {
      float_getscientific(buf, 50, &tmp);
      printf("FAILED for argument %d: %s\n", i, buf);
      return 0;
    }
  }

  printf("testing exponent handling\n");
  float_setinteger(&x, 20);
  _ln(&x, 100);
  float_add(&tmp, &cLn2, &cLn10, 110);
  if (!_cmprelerror(&tmp, &x, -99))
  {
    printf("FAILED for exponent 1\n");
    return 0;
  }
  float_setasciiz(&x, "0.2");
  _ln(&x, 100);
  float_sub(&tmp, &cLn2, &cLn10, 110);
  if (!_cmprelerror(&tmp, &x, -99))
  {
    printf("FAILED for exponent -1\n");
    return 0;
  }

  printf("testing error limit:\n");
  float_setzero(&max);
  float_setasciiz(&step, "0.05");
  float_setasciiz(&ofs, "0.00147436738983647399324678823992433673"
         "24672389823467828246764829833467849839838778485784386584539"
         "85343784568643548923998439823432792346");
  for(i = 19; ++i < 200;)
  {
    float_muli(&tmp, &step, i, EXACT);
    float_add(&x, &tmp, &ofs, EXACT);
    if (i > 63)
      float_setexponent(&tmp, -1);
    float_clone(&tmp, &x, EXACT);
    _ln(&x, 100);
    _ln(&tmp, 110);
    _relerror(&x, &tmp);
    if (float_cmp(&x, &max) > 0)
    {
      float_clone(&max, &x, EXACT);
      if (float_getexponent(&x) >= -99)
      {
        printf("exceeding error in test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _ln: %s\n", buf);
  float_free(&x);
  float_free(&max);
  float_free(&step);
  float_free(&ofs);
  float_free(&tmp);
  return 1;
}

static int test_lnxplus1()
{
  floatstruct x, x1;

  printf("testing _lnxplus1\n");
  float_create(&x);
  float_create(&x1);

  printf("verifying function results:\n");

  float_setasciiz(&x, "-.3");
  _lnxplus1(&x, 100);
  float_clone(&x1, &cLn7, 110);
  float_sub(&x1, &x1, &cLn10, 110);
  if (!_cmprelerror(&x1, &x, -99))
  {
    printf("FAILED x == -0.3\n");
    return 0;
  }

  float_setasciiz(&x, "6");
  _lnxplus1(&x, 100);
  if (!_cmprelerror(&x, &cLn7, -99))
  {
    printf("FAILED x == 6\n");
    return 0;
  }
  float_free(&x);
  float_free(&x1);
  return 1;
}

static int test_artanh1minusx()
{
  floatstruct x, tmp, max, step, ofs;
  int i;
  char buf[50];

  printf("testing _artanh1minusx\n");

  float_create(&x);
  float_create(&tmp);
  float_create(&max);
  float_create(&step);
  float_create(&ofs);
  printf("verifying function results:\n");

  float_clone(&x, &c1Div2, EXACT);
  _artanh1minusx(&x, 100);
  float_clone(&tmp, &cLn3, 110);
  float_mul(&tmp, &tmp, &c1Div2, 110);
  if (!_cmprelerror(&tmp, &x, -99))
  {
    printf("FAILED\n");
    return 0;
  }

  printf("testing error limit:\n");
  float_setzero(&max);
  float_setasciiz(&step, "0.01");
  float_setasciiz(&ofs, "0.00147436738983647399324678823992433673"
         "24672389823467828246764829833467849839838778485784386584539"
         "85343784568643548923998439823432792346");
  for(i = 0; ++i < 50;)
  {
    float_muli(&tmp, &step, i, EXACT);
    float_add(&x, &tmp, &ofs, EXACT);
    float_clone(&tmp, &x, EXACT);
    _artanh1minusx(&x, 100);
    _artanh1minusx(&tmp, 110);
    _relerror(&x, &tmp);
    if (float_cmp(&x, &max) > 0)
    {
      float_clone(&max, &x, EXACT);
      if (float_getexponent(&x) >= -99)
      {
        printf("exceeding error in test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _artanh1minusx: %s\n", buf);

  float_free(&x);
  float_free(&tmp);
  float_free(&max);
  float_free(&step);
  float_free(&ofs);
  return 1;
}

static int test_artanhlt0_5()
{
  floatstruct x, tmp, max, step, ofs;
  int i;
  char buf[50];

  printf("testing _artanhlt0_5\n");

  float_create(&x);
  float_create(&tmp);
  float_create(&max);
  float_create(&step);
  float_create(&ofs);
  printf("verifying function results:\n");

  float_clone(&x, &c1Div2, EXACT);
  _artanhlt0_5(&x, 100);
  float_clone(&tmp, &cLn3, 110);
  float_mul(&tmp, &tmp, &c1Div2, 110);
  if (!_cmprelerror(&tmp, &x, -99))
  {
    printf("FAILED\n");
    return 0;
  }

  printf("testing error limit:\n");
  float_setzero(&max);
  float_setasciiz(&step, "0.01");
  float_setasciiz(&ofs, "0.00147436738983647399324678823992433673"
         "24672389823467828246764829833467849839838778485784386584539"
         "85343784568643548923998439823432792346");
  for(i = 0; ++i < 50;)
  {
    float_muli(&tmp, &step, i, EXACT);
    float_add(&x, &tmp, &ofs, EXACT);
    float_clone(&tmp, &x, EXACT);
    _artanhlt0_5(&x, 100);
    _artanhlt0_5(&tmp, 110);
    _relerror(&x, &tmp);
    if (float_cmp(&x, &max) > 0)
    {
      float_clone(&max, &x, EXACT);
      if (float_getexponent(&x) >= -99)
      {
        printf("exceeding error in test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _artanh1minusx: %s\n", buf);

  float_free(&x);
  float_free(&tmp);
  float_free(&max);
  float_free(&step);
  float_free(&ofs);
 return 1;
}

static int test_arsinh()
{
  floatstruct x, tmp, max, step, ofs;
  int i;
  char buf[50];

  printf("testing _arsinh\n");

  float_create(&x);
  float_create(&tmp);
  float_create(&max);
  float_create(&step);
  float_create(&ofs);

  printf("verifying function results:\n");
  float_setzero(&x);
  _arsinh(&x, 100);
  if(!float_iszero(&x))
  {
    printf("FAILED for x==0\n");
    return 0;
  }
  float_setasciiz(&x, "1e-1000");
  float_clone(&tmp, &x, EXACT);
  _arsinh(&x, 100);
  if (!_cmprelerror(&tmp, &x, -99))
  {
    printf("FAILED for x==1e-1000\n");
    return 0;
  }
  float_setasciiz(&x, "0.1");
  float_setasciiz(&tmp, ".099834078899207563327303124704769443267712"
                        "9117088250107423826956515917683936134651063"
                        "484492769032061884984061246778732266658350084462074");
  _arsinh(&x, 100);
  if (!_cmprelerror(&tmp, &x, -99))
  {
    printf("FAILED for x==0.1\n");
    return 0;
  }
  float_setasciiz(&x, "4");
  float_setasciiz(&tmp, "2.09471254726110129424482284606552865345315"
                        "1048198673265886805979926247949695627249150"
                        "4293654887408011476586881386829965289261091");
  _arsinh(&x, 100);
  if (!_cmprelerror(&tmp, &x, -99))
  {
    printf("FAILED for x==4\n");
    return 0;
  }
  float_setasciiz(&x, "1e100");
  float_muli(&tmp, &cLn10, 100, 110);
  float_add(&tmp, &tmp, &cLn2, 110);
  _arsinh(&x, 100);
  if (!_cmprelerror(&tmp, &x, -99))
  {
    printf("FAILED for x==1e100\n");
    return 0;
  }

  printf("testing error limit:\n");
  float_setzero(&max);
  float_setasciiz(&step, "0.05");
  float_setasciiz(&ofs, "0.00147436738983647399324678823992433673"
         "24672389823467828246764829833467849839838778485784386584539"         "85343784568643548923998439823432792346");
  for(i = 0; ++i < 200;)
  {
    float_muli(&tmp, &step, i, EXACT);
    float_add(&x, &tmp, &ofs, EXACT);
    float_clone(&tmp, &x, EXACT);
    _arsinh(&x, 100);
    _arsinh(&tmp, 110);
    _relerror(&x, &tmp);
    if (float_cmp(&x, &max) > 0)
    {
      float_clone(&max, &x, EXACT);
      if (float_getexponent(&x) >= -99)
      {
        printf("exceeding error in test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _arsinh: %s\n", buf);

  float_free(&x);
  float_free(&tmp);
  float_free(&max);
  float_free(&step);
  float_free(&ofs);
  return 1;
}

static int test_arcoshxplus1()
{
  floatstruct x, tmp, max, step, ofs;
  int i;
  char buf[50];

  printf("testing _arcoshxplus1\n");

  float_create(&x);
  float_create(&tmp);
  float_create(&max);
  float_create(&step);
  float_create(&ofs);

  printf("verifying function results:\n");
  float_setzero(&x);
  _arcoshxplus1(&x, 100);
  if(!float_iszero(&x))
  {
    printf("FAILED for x==0\n");
    return 0;
  }
  float_setinteger(&x, 10);
  float_setasciiz(&tmp, "3.08896990484460301791647985373629509453345"
                        "4441136435131225532448209215351305027698930"
                        "521085626874864770396311532541165684221072841899917");
  _arcoshxplus1(&x, 100);
  if (!_cmprelerror(&tmp, &x, -99))
  {
    printf("FAILED for x==10\n");
    return 0;
  }

  printf("testing error limit:\n");
  float_setzero(&max);
  float_setasciiz(&step, "0.05");
  float_setasciiz(&ofs, "0.00147436738983647399324678823992433673"
         "24672389823467828246764829833467849839838778485784386584539"
         "85343784568643548923998439823432792346");
  for(i = 0; ++i < 200;)
  {
    float_muli(&tmp, &step, i, EXACT);
    float_add(&x, &tmp, &ofs, EXACT);
    float_clone(&tmp, &x, EXACT);
    _arcoshxplus1(&x, 100);
    _arcoshxplus1(&tmp, 110);
    _relerror(&x, &tmp);
    if (float_cmp(&x, &max) > 0)
    {
      float_clone(&max, &x, EXACT);
      if (float_getexponent(&x) >= -99)
      {
        printf("exceeding error in test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  for(i = 0; ++i < 200;)
  {
    float_muli(&tmp, &step, i, EXACT);
    float_add(&x, &tmp, &ofs, EXACT);
    float_clone(&tmp, &x, EXACT);
    _arcoshxplus1(&x, 100);
    _arcoshxplus1(&tmp, 110);
    _relerror(&x, &tmp);
    if (float_cmp(&x, &max) > 0)
    {
      float_clone(&max, &x, EXACT);
      if (float_getexponent(&x) >= -99)
      {
        printf("exceeding error in test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _arcoshxplus1: %s\n", buf);

  float_free(&tmp);
  float_free(&x);
  float_free(&max);
  float_free(&step);
  float_free(&ofs);
  return 1;
}

static int test_coshminus1lt1()
{
  floatstruct x, x1, tmp, max;
  int i;
  char buf[50];

  printf("%s\n", "testing _coshminus1lt1");

  float_create(&x);
  float_create(&x1);
  float_create(&tmp);
  float_create(&max);
  float_setzero(&max);

  printf("verifying special argument x == 0:\n");
  float_setzero(&x);
  _coshminus1lt1(&x, 100);
  if (!float_iszero(&x))
  {
    printf("FAILED x== 0\n");
    return 0;
  }

  /* testing the validity of the reduction */
  printf("verifying result:\n");
  float_setasciiz(&x, "0.5");
  float_clone(&tmp, &x, EXACT);
  float_arcoshxplus1(&x, 110);
  _coshminus1lt1(&x, 100);
  if (!_cmprelerror(&tmp, &x, -99))
  {
    printf("FAILED for arcosh(1.5)-1\n");
    return 0;
  }

  printf("testing error limit:\n");
  /* overall scan for maximum relative error */
  float_setasciiz(&tmp, ".001");
  for (i = -1; ++i <= 1000;)
  {
    float_muli(&x, &tmp, i, EXACT);
    _sub_ulp(&x, 101);
    float_clone(&x1, &x, EXACT);
    _coshminus1lt1(&x,100);
    _coshminus1lt1(&x1, 110);
    _relerror(&x1, &x);
    if (float_cmp(&x1, &max) > 0)
    {
      float_clone(&max, &x1, EXACT);
      if (float_getexponent(&x1) >= -99)
      {
        printf("exceeding error for test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _coshminus1lt1: %s\n", buf);

  float_free(&tmp);
  float_free(&x);
  float_free(&x1);
  float_free(&max);
  return 1;
}

static int test_sinhlt1()
{
  floatstruct x, x1, tmp, max;
  int i;
  char buf[50];

  printf("%s\n", "testing _sinhlt1");

  float_create(&x);
  float_create(&x1);
  float_create(&tmp);
  float_create(&max);
  float_setzero(&max);

  printf("verifying special argument x == 0:\n");
  float_setzero(&x);
  _sinhlt1(&x, 100);
  if (!float_iszero(&x))
  {
    printf("FAILED for x== 0\n");
    return 0;
  }

  printf("verifying positive result:\n");
  float_setasciiz(&x, "1");
  float_clone(&tmp, &x, EXACT);
  float_arsinh(&x, 110);
  _sinhlt1(&x, 100);
  if (!_cmprelerror(&tmp, &x, -99))
  {
    printf("FAILED\n");
    return 0;
  }
  printf("verifying negative result:\n");
  float_setasciiz(&x, "-1");
  float_clone(&tmp, &x, EXACT);
  float_arsinh(&x, 110);
  _sinhlt1(&x, 100);
  if (!_cmprelerror(&tmp, &x, -99))
  {
    printf("FAILED\n");
    return 0;
  }

  printf("testing error limit:\n");
  /* overall scan for maximum relative error */
  float_setasciiz(&tmp, ".001");
  for (i = -1; ++i <= 1000;)
  {
    float_muli(&x, &tmp, i, EXACT);
    _sub_ulp(&x, 101);
    float_clone(&x1, &x, EXACT);
    _sinhlt1(&x,100);
    _sinhlt1(&x1, 110);
    _relerror(&x1, &x);
    if (float_cmp(&x1, &max) > 0)
    {
      float_clone(&max, &x1, EXACT);
      if (float_getexponent(&x1) >= -99)
      {
        printf("exceeding error for test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _sinhlt1: %s\n", buf);

  float_free(&tmp);
  float_free(&x);
  float_free(&x1);
  float_free(&max);
  return 1;
}

static int test_expminus1lt1()
{
  floatstruct x, x1, tmp, max;
  int i;
  char buf[50];

  printf("%s\n", "testing _expminus1lt1");

  float_create(&x);
  float_create(&x1);
  float_create(&tmp);
  float_create(&max);
  float_setzero(&max);

  printf("verifying special argument x == 0:\n");
  float_setzero(&x);
  _expminus1lt1(&x, 100);
  if (!float_iszero(&x))
  {
    printf("FAILED for x== 0\n");
    return 0;
  }

  printf("verifying positive result:\n");
  float_setasciiz(&x, "1");
  float_clone(&tmp, &x, EXACT);
  float_lnxplus1(&x, 110);
  _expminus1lt1(&x, 100);
  if (!_cmprelerror(&tmp, &x, -99))
  {
    printf("FAILED\n");
    return 0;
  }
  printf("verifying negative result:\n");  float_setasciiz(&x, "-.5");
  float_clone(&tmp, &x, EXACT);
  float_lnxplus1(&x, 110);
  _expminus1lt1(&x, 100);
  if (!_cmprelerror(&tmp, &x, -99))
  {
    printf("FAILED\n");
    return 0;
  }

  printf("testing error limit:\n");
  /* overall scan for maximum relative error */
  float_setasciiz(&tmp, ".001");
  for (i = -1001; ++i <= 1000;)
  {
    float_muli(&x, &tmp, i, EXACT);
    _sub_ulp(&x, 101);
    float_clone(&x1, &x, EXACT);
    _expminus1lt1(&x,100);
    _expminus1lt1(&x1, 110);
    _relerror(&x1, &x);
    if (float_cmp(&x1, &max) > 0)
    {
      float_clone(&max, &x1, EXACT);
      if (float_getexponent(&x1) >= -99)
      {
        printf("exceeding error for test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _expminus1lt1: %s\n", buf);

  float_free(&tmp);
  float_free(&x);
  float_free(&x1);
  float_free(&max);
  return 1;
}

static int test_expltln10()
{
  floatstruct x, x1, tmp, max;
  int i;
  char buf[50];

  printf("%s\n", "testing _expltln10");

  float_create(&x);
  float_create(&x1);
  float_create(&tmp);
  float_create(&max);
  float_setzero(&max);

  printf("verifying special argument x == 0:\n");
  float_setzero(&x);
  _expltln10(&x, 100);
  if (float_cmp(&x, &c1) != 0)
  {
    printf("FAILED\n");
    return 0;
  }

  float_setasciiz(&tmp, ".1");
  for (i = 0; ++i < 24;)
  {
    float_muli(&x1, &tmp, i, EXACT);
    float_clone(&x, &x1, EXACT);
    _expltln10(&x, 100);
    float_ln(&x, 110);
    if(!_cmprelerror(&x, &x1, -99))
    {
      float_getscientific(buf, 50, &x1);
      printf("FAILED for x == %s\n", buf);
      return 0;
    }
  }

  printf("testing error limit:\n");
  /* overall scan for maximum relative error */
  float_setasciiz(&tmp, ".005");
  for (i = -1; ++i <= 460;)
  {
    float_muli(&x, &tmp, i, EXACT);
    _sub_ulp(&x, 101);
    float_clone(&x1, &x, EXACT);
    _expltln10(&x,100);
    _expltln10(&x1, 110);
    _relerror(&x1, &x);
    if (float_cmp(&x1, &max) > 0)
    {
      float_clone(&max, &x1, EXACT);
      if (float_getexponent(&x1) >= -99)
      {
        printf("exceeding error for test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _expltln10: %s\n", buf);

  float_free(&tmp);
  float_free(&x);
  float_free(&x1);
  float_free(&max);
  return 1;
}

static int test_exp()
{
  floatstruct x, x1, tmp, max;
  int i;
  char buf[50];

  printf("%s\n", "testing _exp");

  float_create(&x);
  float_create(&x1);
  float_create(&tmp);
  float_create(&max);
  float_setzero(&max);

  printf("verifying special argument x == 0:\n");
  float_setzero(&x);
  _exp(&x, 100);
  if (float_cmp(&x, &c1) != 0)
  {
    printf("FAILED\n");
    return 0;
  }

  printf("verifying results:\n");
  float_setasciiz(&tmp, "1");
  for (i = -2; ++i < 9;)
  {
    float_setexponent(&tmp, i);
    float_clone(&x, &tmp, EXACT);
    _exp(&x, 100);
    float_ln(&x, 110);
    if(!_cmprelerror(&x, &tmp, -99))
    {
      float_getscientific(buf, 50, &tmp);
      printf("FAILED for x == %s\n", buf);
      return 0;
    }
  }

  float_setasciiz(&tmp, "-1");
  for (i = -2; ++i < 9;)
  {
    float_setexponent(&tmp, i);
    float_clone(&x, &tmp, EXACT);
    _exp(&x, 100);
    float_ln(&x, 110);
    if(!_cmprelerror(&x, &tmp, -99))
    {
      float_getscientific(buf, 50, &tmp);
      printf("FAILED for x == %s\n", buf);
      return 0;
    }
  }

  printf("testing error limit:\n");
  /* overall scan for maximum relative error */
  float_setasciiz(&tmp, ".005");
  for (i = -461; ++i <= 0;)
  {
    float_muli(&x, &tmp, i, EXACT);
    _sub_ulp(&x, 101);
    float_clone(&x1, &x, EXACT);
    _exp(&x,100);
    _exp(&x1, 110);
    _relerror(&x1, &x);
    if (float_cmp(&x1, &max) > 0)
    {
      float_clone(&max, &x1, EXACT);
      if (float_getexponent(&x1) >= -99)
      {
        printf("exceeding error for test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_muli(&x, &cLn10, EXPMAX, 110);
  float_clone(&x1, &x, EXACT);
  _exp(&x,100);
  _exp(&x1, 110);
  _relerror(&x1, &x);
  if (float_cmp(&x1, &max) > 0)
  {
    float_clone(&max, &x1, EXACT);
    if (float_getexponent(&x1) >= -99)
    {
      printf("exceeding error for near overflow case: ");
      float_getscientific(buf, 50, &max);
      printf("%s\n", buf);
      return 0;
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _exp: %s\n", buf);

  float_free(&tmp);
  float_free(&x);
  float_free(&x1);
  float_free(&max);
  return 1;
}

static int test_expminus1()
{
  floatstruct x, tmp;

  printf("%s\n", "testing _expminus1");

  float_create(&x);
  float_create(&tmp);

  printf("verifying special argument x == 0:\n");
  float_setzero(&x);
  _expminus1(&x, 100);
  if (!float_iszero(&x))
  {
    printf("FAILED\n");
    return 0;
  }

  printf("verifying results:\n");
  float_setasciiz(&x, "-1e100000");
  float_setinteger(&tmp, -1);
  _expminus1(&x, 100);
  if(!_cmprelerror(&x, &tmp, -99))
  {
    printf("FAILED for x == -1e100000\n");
    return 0;
  }
  float_setasciiz(&x, "-1");
  float_setinteger(&tmp, -1);
  _expminus1(&x, 100);
  float_lnxplus1(&x, 110);
  if(!_cmprelerror(&x, &tmp, -99))
  {
    printf("FAILED for x == -1\n");
    return 0;
  }
  float_setasciiz(&x, "-1e-10");
  float_clone(&tmp, &x, EXACT);
  _expminus1(&x, 100);
  float_lnxplus1(&x, 110);
  if(!_cmprelerror(&x, &tmp, -99))
  {
    printf("FAILED for x == -1e-10\n");
    return 0;
  }
  float_setasciiz(&x, "1e-10");
  float_clone(&tmp, &x, EXACT);
  _expminus1(&x, 100);
  float_lnxplus1(&x, 110);
  if(!_cmprelerror(&x, &tmp, -99))
  {
    printf("FAILED for x == 1e-10\n");
    return 0;
  }

  float_setasciiz(&x, "1");
  float_clone(&tmp, &x, EXACT);
  _expminus1(&x, 100);
  float_lnxplus1(&x, 110);
  if(!_cmprelerror(&x, &tmp, -99))
  {
    printf("FAILED for x == 1\n");
    return 0;
  }
  float_setasciiz(&x, "100000");
  float_clone(&tmp, &x, EXACT);
  _expminus1(&x, 100);
  float_lnxplus1(&x, 110);
  if(!_cmprelerror(&x, &tmp, -99))
  {
    printf("FAILED for x == 100000\n");
    return 0;
  }
  float_free(&tmp);
  float_free(&x);
  return 1;
}

static int test_coshminus1()
{
  floatstruct x, x1, tmp, max;
  int i;
  char buf[50];

  printf("%s\n", "testing _coshminus1");

  float_create(&x);
  float_create(&tmp);
  float_create(&x1);
  float_create(&max);
  float_setzero(&max);

  printf("verifying function results:\n");
  float_setzero(&x);
  _coshminus1(&x, 100);
  if (!float_iszero(&x))
  {
    printf("FAILED for x == 0\n");
    return 0;
  }

  float_setasciiz(&x, ".1");
  float_clone(&tmp, &x, EXACT);
  _coshminus1(&x, 100);
  float_arcoshxplus1(&x, 110);
  if(!_cmprelerror(&x, &tmp, -99))
  {
    printf("FAILED for x == 0.1\n");
    return 0;
  }

  float_setasciiz(&x, "2");
  float_clone(&tmp, &x, EXACT);
  _coshminus1(&x, 100);
  float_arcoshxplus1(&x, 110);
  if(!_cmprelerror(&x, &tmp, -99))
  {
    printf("FAILED for x == 2\n");
    return 0;
  }

  float_setasciiz(&x, "10000");
  float_clone(&tmp, &x, EXACT);
  _coshminus1(&x, 100);
  float_arcoshxplus1(&x, 110);
  if(!_cmprelerror(&x, &tmp, -99))
  {
    printf("FAILED for x == 10000\n");
    return 0;
  }

  printf("testing error limit:\n");
  /* overall scan for maximum relative error */
  float_setasciiz(&tmp, ".005");
  for (i = 200; ++i <= 500;)
  {
    float_muli(&x, &tmp, i, EXACT);
    _sub_ulp(&x, 101);
    float_clone(&x1, &x, EXACT);
    _coshminus1(&x,100);
    _coshminus1(&x1, 110);
    _relerror(&x1, &x);
    if (float_cmp(&x1, &max) > 0)
    {
      float_clone(&max, &x1, EXACT);
      if (float_getexponent(&x1) >= -99)
      {
        printf("exceeding error for test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _coshminus1: %s\n", buf);

  float_free(&tmp);
  float_free(&x);
  float_free(&x1);
  float_free(&max);
  return 1;
}

static int test_tanhlt0_5()
{
  floatstruct x, x1, tmp, max;
  int i;
  char buf[50];

  printf("%s\n", "testing _tanhlt0_5");

  float_create(&x);
  float_create(&tmp);
  float_create(&x1);
  float_create(&max);
  float_setzero(&max);

  printf("verifying function results:\n");
  float_setzero(&x);
  _tanhlt0_5(&x, 100);
  if (!float_iszero(&x))
  {
    printf("FAILED for x == 0\n");
    return 0;
  }

  float_setasciiz(&x, ".1");
  float_clone(&tmp, &x, EXACT);
  _tanhlt0_5(&x, 100);
  float_artanh(&x, 110);
  if(!_cmprelerror(&x, &tmp, -99))
  {
    printf("FAILED for x == 0.1\n");
    return 0;
  }

  printf("testing error limit:\n");
  /* overall scan for maximum relative error */
  float_setasciiz(&tmp, ".005");
  for (i = 0; ++i <= 100;)
  {
    float_muli(&x, &tmp, i, EXACT);
    _sub_ulp(&x, 101);
    float_clone(&x1, &x, EXACT);
    _tanhlt0_5(&x,100);
    _tanhlt0_5(&x1, 110);
    _relerror(&x1, &x);
    if (float_cmp(&x1, &max) > 0)
    {
      float_clone(&max, &x1, EXACT);
      if (float_getexponent(&x1) >= -99)
      {
        printf("exceeding error for test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _tanhlt0_5: %s\n", buf);

  float_free(&tmp);
  float_free(&x);
  float_free(&x1);
  float_free(&max);
  return 1;
}

static int test_tanhminus1gt0()
{
  floatstruct x, x1, tmp, max;
  int i;
  char buf[50];

  printf("%s\n", "testing _tanhminus1gt0");

  float_create(&x);
  float_create(&tmp);
  float_create(&x1);
  float_create(&max);
  float_setzero(&max);

  float_setasciiz(&x, "1");
  float_clone(&tmp, &x, EXACT);
  _tanhminus1gt0(&x, 100);
  float_artanhxplus1(&x, 110);
  if(!_cmprelerror(&x, &tmp, -99))
  {
    printf("FAILED for x == 1\n");
    return 0;
  }

  printf("testing error limit:\n");
  /* overall scan for maximum relative error */
  float_setasciiz(&tmp, ".005");
  for (i = 0; ++i <= 300;)
  {
    float_muli(&x, &tmp, i, EXACT);
    _sub_ulp(&x, 101);
    float_clone(&x1, &x, EXACT);
    _tanhminus1gt0(&x,100);
    _tanhminus1gt0(&x1, 110);
    _relerror(&x1, &x);
    if (float_cmp(&x1, &max) > 0)
    {
      float_clone(&max, &x1, EXACT);
      if (float_getexponent(&x1) >= -99)
      {
        printf("exceeding error for test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  for (i = 1; i <= EXPMAX;)
  {
    float_setinteger(&x, 1);
    _sub_ulp(&x, 101);
    float_setexponent(&x, i);
    i*=10;
    float_clone(&x1, &x, EXACT);
    _tanhminus1gt0(&x,100);
    _tanhminus1gt0(&x1, 110);
    _relerror(&x1, &x);
    if (float_cmp(&x1, &max) > 0)
    {
      float_clone(&max, &x1, EXACT);
      if (float_getexponent(&x1) >= -99)
      {
        printf("exceeding error for test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _tanhminus1gt0: %s\n", buf);

  float_free(&tmp);
  float_free(&x);
  float_free(&x1);
  float_free(&max);
  return 1;
}

static int test_arctanlt1()
{
  floatstruct x, tmp, max, step, ofs;
  int i;
  char buf[50];

  printf("testing _arctanlt1\n");

  float_create(&x);
  float_create(&tmp);
  float_create(&max);
  float_create(&step);
  float_create(&ofs);

  printf("verifying function results:\n");
  float_setzero(&x);
  _arctanlt1(&x, 100);
  if(!float_iszero(&x))
  {
    printf("FAILED for x==0\n");
    return 0;
  }
  float_setasciiz(&x, "-0.5");
  float_setasciiz(&tmp, "-.46364760900080611621425623146121440202853"
                        "7054286120263810933088720197864165741705300"
                        "60028398488789255652985225119083751350581818162501");
  _arctanlt1(&x, 100);
  if (!_cmprelerror(&tmp, &x, -99))
  {
    printf("FAILED for x==-0.5\n");
    return 0;
  }

  printf("testing error limit:\n");
  float_setzero(&max);
  float_setasciiz(&step, "0.005");
  float_setasciiz(&ofs, "0.000147436738983647399324678823992433673"
         "24672389823467828246764829833467849839838778485784386584539"
         "85343784568643548923998439823432792346");
  for(i = 0; ++i < 200;)
  {
    float_muli(&tmp, &step, i, EXACT);
    float_add(&x, &tmp, &ofs, EXACT);
    float_clone(&tmp, &x, EXACT);
    _arctanlt1(&x, 100);
    _arctanlt1(&tmp, 110);
    _relerror(&x, &tmp);
    if (float_cmp(&x, &max) > 0)
    {
      float_clone(&max, &x, EXACT);
      if (float_getexponent(&x) >= -99)
      {
        printf("exceeding error in test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _arctanlt1: %s\n", buf);

  float_free(&tmp);
  float_free(&x);
  float_free(&max);
  float_free(&step);
  float_free(&ofs);
  return 1;
}

static int test_arctan()
{
  floatstruct x, tmp, max, step, ofs;
  int i;
  char buf[50];

  printf("testing _arctan\n");

  float_create(&x);
  float_create(&tmp);
  float_create(&max);
  float_create(&step);
  float_create(&ofs);

  float_setasciiz(&x, "-1.5");
  float_setasciiz(&tmp, "-.98279372324732906798571061101466601449687"
                        "7453631628556761425088317988071549796035389"
                        "70653437281731110816513970201193676622994103918188");
  _arctan(&x, 100);
  if (!_cmprelerror(&tmp, &x, -99))
  {
    printf("FAILED for x==-1.5\n");
    return 0;
  }

  printf("testing error limit:\n");
  float_setzero(&max);
  float_setasciiz(&step, "0.005");
  float_setasciiz(&ofs, "0.000147436738983647399324678823992433673"
         "24672389823467828246764829833467849839838778485784386584539"
         "85343784568643548923998439823432792346");
  for(i = 200; ++i < 400;)
  {
    float_muli(&tmp, &step, i, EXACT);
    float_add(&x, &tmp, &ofs, EXACT);
    float_clone(&tmp, &x, EXACT);
    _arctan(&x, 100);
    _arctan(&tmp, 110);
    _relerror(&x, &tmp);
    if (float_cmp(&x, &max) > 0)
    {
      float_clone(&max, &x, EXACT);
      if (float_getexponent(&x) >= -99)
      {
        printf("exceeding error in test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _arctan: %s\n", buf);

  float_free(&tmp);
  float_free(&x);
  float_free(&max);
  float_free(&step);
  float_free(&ofs);
  return 1;
}

static int test_arccosxplus1lt0_5()
{
  floatstruct x, tmp, max, step, ofs;
  int i;
  char buf[50];

  printf("testing _arccosxplus1lt0_5\n");

  float_create(&x);
  float_create(&tmp);
  float_create(&max);
  float_create(&step);
  float_create(&ofs);

  printf("verifying function results:\n");
  float_setzero(&x);
  _arccosxplus1lt0_5(&x, 100);
  if(!float_iszero(&x))
  {
    printf("FAILED for x==0\n");
    return 0;
  }
  float_setasciiz(&x, "-0.5");
  float_setasciiz(&tmp, "1.04719755119659774615421446109316762806572"
                        "3133125035273658314864102605468762069666209"
                        "34494178070568932738269550442743554903128153651686");
  _arccosxplus1lt0_5(&x, 100);
  if (!_cmprelerror(&tmp, &x, -99))
  {
    printf("FAILED for x==-0.5\n");
    return 0;
  }

  printf("testing error limit:\n");
  float_setzero(&max);
  float_setasciiz(&step, "0.005");
  float_setasciiz(&ofs, "0.000147436738983647399324678823992433673"
         "24672389823467828246764829833467849839838778485784386584539"
         "85343784568643548923998439823432792346");
  for(i = 0; ++i < 100;)
  {
    float_muli(&tmp, &step, i, EXACT);
    float_add(&x, &tmp, &ofs, EXACT);
    float_clone(&tmp, &x, EXACT);
    _arccosxplus1lt0_5(&x, 100);
    _arccosxplus1lt0_5(&tmp, 110);
    _relerror(&x, &tmp);
    if (float_cmp(&x, &max) > 0)
    {
      float_clone(&max, &x, EXACT);
      if (float_getexponent(&x) >= -99)
      {
        printf("exceeding error in test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _arccosxplus1lt0_5: %s\n", buf);

  float_free(&tmp);
  float_free(&x);
  float_free(&max);
  float_free(&step);
  float_free(&ofs);
  return 1;
}

static int test_arcsinlt0_5()
{
  floatstruct x, tmp, max, step, ofs;
  int i;
  char buf[50];

  printf("testing _arcsinlt0_5\n");

  float_create(&x);
  float_create(&tmp);
  float_create(&max);
  float_create(&step);
  float_create(&ofs);

  printf("verifying function results:\n");
  float_setzero(&x);
  _arccosxplus1lt0_5(&x, 100);
  if(!float_iszero(&x))
  {
    printf("FAILED for x==0\n");
    return 0;
  }
  float_setasciiz(&x, "-0.5");
  float_setasciiz(&tmp, "-.52359877559829887307710723054658381403286"
                        "1566562517636829157432051302734381034833104"
                        "67247089035284466369134775221371777451564076825843");
  _arcsinlt0_5(&x, 100);
  if (!_cmprelerror(&tmp, &x, -99))
  {
    printf("FAILED for x==-0.5\n");
    return 0;
  }

  printf("testing error limit:\n");
  float_setzero(&max);
  float_setasciiz(&step, "0.005");
  float_setasciiz(&ofs, "0.000147436738983647399324678823992433673"
         "24672389823467828246764829833467849839838778485784386584539"
         "85343784568643548923998439823432792346");
  for(i = 0; ++i < 100;)
  {
    float_muli(&tmp, &step, i, EXACT);
    float_add(&x, &tmp, &ofs, EXACT);
    float_clone(&tmp, &x, EXACT);
    _arcsinlt0_5(&x, 100);
    _arcsinlt0_5(&tmp, 110);
    _relerror(&x, &tmp);
    if (float_cmp(&x, &max) > 0)
    {
      float_clone(&max, &x, EXACT);
      if (float_getexponent(&x) >= -99)
      {
        printf("exceeding error in test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _arcsinlt0_5: %s\n", buf);

  float_free(&tmp);
  float_free(&x);
  float_free(&max);
  float_free(&step);
  float_free(&ofs);
  return 1;
}

static int test_arccos()
{
  floatstruct x, tmp, max, step, ofs;
  int i;
  char buf[50];

  printf("testing _arccos\n");

  float_create(&x);
  float_create(&tmp);
  float_create(&max);
  float_create(&step);
  float_create(&ofs);

  printf("verifying function results:\n");
  float_setinteger(&x, 1);
  _arccos(&x, 100);
  if(!float_iszero(&x))
  {
    printf("FAILED for x==1\n");
    return 0;
  }
  float_setasciiz(&x, "0.5");
  float_setasciiz(&tmp, "1.04719755119659774615421446109316762806572"
                        "3133125035273658314864102605468762069666209"
                        "34494178070568932738269550442743554903128153651686");
  _arccos(&x, 100);
  if (!_cmprelerror(&tmp, &x, -99))
  {
    printf("FAILED for x==0.5\n");
    return 0;
  }
  float_setzero(&x);
  _arccos(&x, 100);
  if (!_cmprelerror(&x, &cPiDiv2, -99))
  {
    printf("FAILED for x==0\n");
    return 0;
  }
  float_setinteger(&x, -1);
  _arccos(&x, 100);
  if (!_cmprelerror(&x, &cPi, -99))
  {
    printf("FAILED for x==-1\n");
    return 0;
  }

  printf("testing error limit:\n");
  float_setzero(&max);
  float_setasciiz(&step, "0.01");
  float_setasciiz(&ofs, "0.000147436738983647399324678823992433673"
         "24672389823467828246764829833467849839838778485784386584539"
         "85343784568643548923998439823432792346");
  for(i = -100; ++i < 100;)
  {
    float_muli(&tmp, &step, i, EXACT);
    float_add(&x, &tmp, &ofs, EXACT);
    float_clone(&tmp, &x, EXACT);
    _arccos(&x, 100);
    _arccos(&tmp, 110);
    _relerror(&x, &tmp);
    if (float_cmp(&x, &max) > 0)
    {
      float_clone(&max, &x, EXACT);
      if (float_getexponent(&x) >= -99)
      {
        printf("exceeding error in test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _arccos: %s\n", buf);

  float_free(&tmp);
  float_free(&x);
  float_free(&max);
  float_free(&step);
  float_free(&ofs);
  return 1;
}

static int test_arcsin()
{
  floatstruct x, tmp, max, step, ofs;
  int i;
  char buf[50];

  printf("testing _arcsin\n");

  float_create(&x);
  float_create(&tmp);
  float_create(&max);
  float_create(&step);
  float_create(&ofs);

  printf("verifying function results:\n");
  float_setzero(&x);
  _arcsin(&x, 100);
  if(!float_iszero(&x))
  {
    printf("FAILED for x==0\n");
    return 0;
  }
  float_setasciiz(&x, "-0.5");
  float_setasciiz(&tmp, "-.52359877559829887307710723054658381403286"
                        "1566562517636829157432051302734381034833104"
                        "67247089035284466369134775221371777451564076825843");
  _arcsin(&x, 100);
  if (!_cmprelerror(&tmp, &x, -99))
  {
    printf("FAILED for x==-0.5\n");
    return 0;
  }
  float_setinteger(&x, 1);
  _arcsin(&x, 100);
  if (!_cmprelerror(&x, &cPiDiv2, -99))
  {
    printf("FAILED for x==1\n");
    return 0;
  }
  float_setinteger(&x, -1);
  _arcsin(&x, 100);
  float_changesign(&x);
  if (!_cmprelerror(&x, &cPiDiv2, -99))
  {
    printf("FAILED for x==-1\n");
    return 0;
  }

  printf("testing error limit:\n");
  float_setzero(&max);
  float_setasciiz(&step, "0.005");
  float_setasciiz(&ofs, "0.000147436738983647399324678823992433673"
         "24672389823467828246764829833467849839838778485784386584539"
         "85343784568643548923998439823432792346");
  for(i = 0; ++i < 200;)
  {
    float_muli(&tmp, &step, i, EXACT);
    float_add(&x, &tmp, &ofs, EXACT);
    float_clone(&tmp, &x, EXACT);
    _arcsin(&x, 100);
    _arcsin(&tmp, 110);
    _relerror(&x, &tmp);
    if (float_cmp(&x, &max) > 0)
    {
      float_clone(&max, &x, EXACT);
      if (float_getexponent(&x) >= -99)
      {
        printf("exceeding error in test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _arcsin: %s\n", buf);

  float_free(&tmp);
  float_free(&x);
  float_free(&max);
  float_free(&step);
  float_free(&ofs);
  return 1;
}

static int test_cosminus1ltPiDiv4()
{
  floatstruct x, tmp, max, step, ofs;
  int i;
  char buf[50];

  printf("testing _cosminus1ltPiDiv4\n");

  float_create(&x);
  float_create(&tmp);
  float_create(&max);
  float_create(&step);
  float_create(&ofs);

  printf("verifying function results:\n");
  float_setzero(&x);
  _cosminus1ltPiDiv4(&x, 100);
  if(!float_iszero(&x))
  {
    printf("FAILED for x==0\n");
    return 0;
  }
  float_setasciiz(&x, "-0.5");
  float_setasciiz(&tmp, "-.12241743810962728388371841739617034800835"
                        "4802890255947002389131684049236725786052594"
                        "205815915317741644521599406890946006586172");
  _cosminus1ltPiDiv4(&x, 100);
  if (!_cmprelerror(&tmp, &x, -99))
  {
    printf("FAILED for x==-0.5\n");
    return 0;
  }

  printf("testing error limit:\n");
  float_setzero(&max);
  float_setasciiz(&step, "0.005");
  float_setasciiz(&ofs, "0.000147436738983647399324678823992433673"
         "24672389823467828246764829833467849839838778485784386584539"
         "85343784568643548923998439823432792346");
  for(i = 0; ++i < 158;)
  {
    float_muli(&tmp, &step, i, EXACT);
    float_add(&x, &tmp, &ofs, EXACT);
    float_clone(&tmp, &x, EXACT);
    _cosminus1ltPiDiv4(&x, 100);
    _cosminus1ltPiDiv4(&tmp, 110);
    _relerror(&x, &tmp);
    if (float_cmp(&x, &max) > 0)
    {
      float_clone(&max, &x, EXACT);
      if (float_getexponent(&x) >= -99)
      {
        printf("exceeding error in test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _cosminus1ltPiDiv4: %s\n", buf);

  float_free(&tmp);
  float_free(&x);
  float_free(&max);
  float_free(&step);
  float_free(&ofs);
  return 1;
}

static int test_sinltPiDiv4()
{
  floatstruct x, tmp, max, step, ofs;
  int i;
  char buf[50];

  printf("testing _sinltPiDiv4\n");

  float_create(&x);
  float_create(&tmp);
  float_create(&max);
  float_create(&step);
  float_create(&ofs);

  printf("verifying function results:\n");
  float_setzero(&x);
  _sinltPiDiv4(&x, 100);
  if(!float_iszero(&x))
  {
    printf("FAILED for x==0\n");
    return 0;
  }
  float_setasciiz(&x, "-0.5");
  float_setasciiz(&tmp, "-.47942553860420300027328793521557138808180"
                        "3367940600675188616613125535000287814832209"
                        "63127468434826908613209108450571741781109374860994");
  _sinltPiDiv4(&x, 100);
  if (!_cmprelerror(&tmp, &x, -99))
  {
    printf("FAILED for x==-0.5\n");
    return 0;
  }

  printf("testing error limit:\n");
  float_setzero(&max);
  float_setasciiz(&step, "0.005");
  float_setasciiz(&ofs, "0.000147436738983647399324678823992433673"
         "24672389823467828246764829833467849839838778485784386584539"
         "85343784568643548923998439823432792346");
  for(i = 0; ++i < 158;)
  {
    float_muli(&tmp, &step, i, EXACT);
    float_add(&x, &tmp, &ofs, EXACT);
    float_clone(&tmp, &x, EXACT);
    _sinltPiDiv4(&x, 100);
    _sinltPiDiv4(&tmp, 110);
    _relerror(&x, &tmp);
    if (float_cmp(&x, &max) > 0)
    {
      float_clone(&max, &x, EXACT);
      if (float_getexponent(&x) >= -99)
      {
        printf("exceeding error in test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _sinltPiDiv4: %s\n", buf);

  float_free(&tmp);
  float_free(&x);
  float_free(&max);
  float_free(&step);
  float_free(&ofs);
  return 1;
}

static int test_tanltPiDiv4()
{
  floatstruct x, tmp, max, step, ofs;
  int i;
  char buf[50];

  printf("testing _tanltPiDiv4\n");

  float_create(&x);
  float_create(&tmp);
  float_create(&max);
  float_create(&step);
  float_create(&ofs);

  printf("verifying function results:\n");
  float_setzero(&x);
  _tanltPiDiv4(&x, 100);
  if(!float_iszero(&x))
  {
    printf("FAILED for x==0\n");
    return 0;
  }
  float_setasciiz(&x, "-0.5");
  float_setasciiz(&tmp, "-.54630248984379051325517946578028538329755"
                        "1720179791246164091385932907510518025815715"
                        "1806482706562185891048626002641142654932300911684");
  _tanltPiDiv4(&x, 100);
  if (!_cmprelerror(&tmp, &x, -99))
  {
    printf("FAILED for x==-0.5\n");
    return 0;
  }

  printf("testing error limit:\n");
  float_setzero(&max);
  float_setasciiz(&step, "0.005");
  float_setasciiz(&ofs, "0.000147436738983647399324678823992433673"
         "24672389823467828246764829833467849839838778485784386584539"
         "85343784568643548923998439823432792346");
  for(i = 0; ++i < 158;)
  {
    float_muli(&tmp, &step, i, EXACT);
    float_add(&x, &tmp, &ofs, EXACT);
    float_clone(&tmp, &x, EXACT);
    _tanltPiDiv4(&x, 100);
    _tanltPiDiv4(&tmp, 110);
    _relerror(&x, &tmp);
    if (float_cmp(&x, &max) > 0)
    {
      float_clone(&max, &x, EXACT);
      if (float_getexponent(&x) >= -99)
      {
        printf("exceeding error in test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _tanltPiDiv4: %s\n", buf);

  float_free(&tmp);
  float_free(&x);
  float_free(&max);
  float_free(&step);
  float_free(&ofs);
  return 1;
}

static int test_cos()
{
  floatstruct x, tmp, max, step, ofs;
  int i;
  char buf[50];

  printf("testing _cos\n");

  float_create(&x);
  float_create(&tmp);
  float_create(&max);
  float_create(&step);
  float_create(&ofs);

  printf("verifying function results:\n");
  float_setzero(&x);
  _cos(&x, 100);
  if(float_cmp(&x, &c1)!=0)
  {
    printf("FAILED for x==0\n");
    return 0;
  }
  float_setasciiz(&step, "0.1");
  for (i = 0; ++i < 15;)
  {
    float_muli(&x, &step, i, EXACT);
    float_add(&tmp, &x, &x, EXACT);
    _cos(&x, 100);
    _cos(&tmp, 100);
    float_mul(&x, &x, &x, 110);
    float_add(&x, &x, &x, EXACT);
    float_sub(&x, &x, &c1, EXACT);
    if (!_cmprelerror(&tmp, &x, -95))
    {
      printf("FAILED for testcase %d\n", i);
      return 0;
    }
  }

  printf("testing error limit:\n");
  float_setzero(&max);
  float_setasciiz(&step, "0.05");
  float_setasciiz(&ofs, "0.000147436738983647399324678823992433673"
         "24672389823467828246764829833467849839838778485784386584539"
         "85343784568643548923998439823432792346");
  for(i = -63; ++i < 63;)
  {
    float_muli(&tmp, &step, i, EXACT);
    float_add(&x, &tmp, &ofs, EXACT);
    float_clone(&tmp, &x, EXACT);
    _cos(&x, 100);
    _cos(&tmp, 110);
    _relerror(&x, &tmp);
    if (float_cmp(&x, &max) > 0)
    {
      float_clone(&max, &x, EXACT);
      if (float_getexponent(&x) >= -99)
      {
        printf("exceeding error in test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _cos: %s\n", buf);

  float_free(&tmp);
  float_free(&x);
  float_free(&max);
  float_free(&step);
  float_free(&ofs);
  return 1;
}

static int test_sin()
{
  floatstruct x, tmp, max, step, ofs;
  int i;

  printf("testing _sin\n");

  float_create(&x);
  float_create(&tmp);
  float_create(&max);
  float_create(&step);
  float_create(&ofs);

  printf("verifying function results:\n");
  float_setzero(&x);
  _sin(&x, 100);
  if(!float_iszero(&x))
  {
    printf("FAILED for x==0\n");
    return 0;
  }
  float_setasciiz(&step, "0.1");
  for (i = 0; ++i < 32;)
  {
    float_muli(&x, &step, i, EXACT);
    float_sub(&tmp, &x, &cPiDiv2, 110);
    _sin(&x, 100);
    _cos(&tmp, 100);
    if (!_cmprelerror(&tmp, &x, -95))
    {
      printf("FAILED for testcase %d\n", i);
      return 0;
    }
  }

  float_setasciiz(&step, "0.5");
  for (i = -7; ++i < 0;)
  {
    float_muli(&x, &step, i, EXACT);
    float_clone(&tmp, &x, EXACT);
    float_abs(&tmp);
    _sin(&x, 100);
    _sin(&tmp, 100);
    float_changesign(&tmp);
    if (!_cmprelerror(&tmp, &x, -95))
    {
      printf("FAILED for testcase %d\n", i);
      return 0;
    }
  }
  float_free(&tmp);
  float_free(&x);
  float_free(&max);
  float_free(&step);
  float_free(&ofs);
  return 1;
}

static int test_tan()
{
  floatstruct x, tmp, max, step, ofs;
  int i;
  char buf[50];

  printf("testing _tan\n");

  float_create(&x);
  float_create(&tmp);
  float_create(&max);
  float_create(&step);
  float_create(&ofs);

  printf("verifying function results:\n");
  float_setzero(&x);
  _tan(&x, 100);
  if(!float_iszero(&x))
  {
    printf("FAILED for x==0\n");
    return 0;
  }
  float_setasciiz(&step, "0.1");
  for (i = -32; ++i < 32;)
  {
    if (i == 0)
      continue;
    float_muli(&x, &step, i, EXACT);
    float_clone(&tmp, &x, EXACT);
    float_clone(&ofs, &x, EXACT);
    _sin(&x, 100);
    _cos(&tmp, 100);
    float_div(&tmp, &x, &tmp, 110);
    _tan(&ofs, 100);
    if (!_cmprelerror(&tmp, &ofs, -95))
    {
      printf("FAILED for testcase %d\n", i);
      return 0;
    }
  }

  printf("testing error limit:\n");
  float_setzero(&max);
  float_setasciiz(&step, "0.05");
  float_setasciiz(&ofs, "0.000147436738983647399324678823992433673"
         "24672389823467828246764829833467849839838778485784386584539"
         "85343784568643548923998439823432792346");
  for(i = -63; ++i < 63;)
  {
    float_muli(&tmp, &step, i, EXACT);
    float_add(&x, &tmp, &ofs, EXACT);
    float_clone(&tmp, &x, EXACT);
    _tan(&x, 100);
    _tan(&tmp, 110);
    _relerror(&x, &tmp);
    if (float_cmp(&x, &max) > 0)
    {
      float_clone(&max, &x, EXACT);
      if (float_getexponent(&x) >= -99)
      {
        printf("exceeding error in test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _tan: %s\n", buf);

  return 1;
}

static int tc_floatnum2longint(char* value,
  int lg, unsigned x0, unsigned x1, unsigned x2)
{
  floatstruct x;
  t_longint longint;

  float_create(&x);
  float_setasciiz(&x, value);
  _floatnum2longint(&longint, &x);
  float_free(&x);
  return lg == longint.length && (lg == 0 || x0 == longint.value[0])
         && (lg < 32 || x1 == longint.value[1])
         && (lg < 64 || x2 == longint.value[2]);
}

static int test_floatnum2longint()
{
  /* depends on wordsize!! */
  printf("testing floatnum2longint\n");

  if (!tc_floatnum2longint("0", 0, 0, 0, 0)) return 0;
  if (!tc_floatnum2longint("1", 1, 1, 0, 0)) return 0;
  if (!tc_floatnum2longint("1234567890", 1, 1234567890, 0, 0)) return 0;
  if (!tc_floatnum2longint("8589934592", 2, 0, 2, 0)) return 0;
  return 1;
}

static int tc_longint2floatnum(unsigned x0, unsigned x1, unsigned x2, int lg, char* value)
{
  /* depends on wordsize!! */
  floatstruct x, y;
  t_longint l;
  int i;

  float_create(&x);
  float_create(&y);

  for (i = 0; i < UARRAYLG; ++i)
    l.value[i] = 0;
  l.value[0] = x0;
  l.value[1] = x1;
  l.value[2] = x2;
  l.length = lg;
  float_setasciiz(&y, value);
  _longint2floatnum(&x, &l);
  if (float_cmp(&x, &y) != 0)
  {
    printf("conversion FAILED");
    return 0;
  }
  float_free(&x);
  float_free(&y);
  return 1;
}

static int test_longint2floatnum()
{
  printf("testing longint2floatnum\n");
  if (!tc_longint2floatnum(0, 0, 0, 0, "0")) return 0;
  if (!tc_longint2floatnum(1, 0, 0, 1, "1")) return 0;
  if (!tc_longint2floatnum(2000000000, 0, 0, 1, "2000000000")) return 0;
  if (!tc_longint2floatnum(0, 2, 0, 2, "8589934592")) return 0;
  if (!tc_longint2floatnum(1000000, 2, 0, 2, "8590934592")) return 0;
  return 1;
}

static int tc_raiseposi(char* base, unsigned exponent)
{
  floatstruct x, y;
  int expx, r, result;

  float_create(&x);
  float_create(&y);
  float_setasciiz(&x, base);
  float_clone(&y, &x, EXACT);
  float_abs(&y);
  float_log(&y, 100);
  r = _raiseposi(&x, &expx, exponent, 100);
  float_abs(&x);
  float_log(&x, 199);
  float_addi(&x, &x, expx, 100);
  float_divi(&x, &x, exponent, 100);
  result = _cmprelerror(&x, &y, 95);
  float_free(&x);
  float_free(&y);
  return result;
}

static int test_raiseposi()
{
  floatstruct x, tmp, max, step, ofs;
  int expx;
  int i, j;
  char buf[50];

  printf("testing _raiseposi\n");

  float_create(&x);
  float_create(&tmp);
  float_create(&max);
  float_create(&step);
  float_create(&ofs);

  if (!tc_raiseposi("1", 1)) return 0;
  if (!tc_raiseposi("2", 1)) return 0;
  if (!tc_raiseposi("2", 2)) return 0;
  if (!tc_raiseposi("2", 3)) return 0;
  float_setinteger(&x, -1);
  _raiseposi(&x, &expx, 3, 100);
  if (float_cmp(&x, &cMinus1) != 0 || expx != 0)
  {
    printf("test case -1 ^ 3 FAILED");
    return 0;
  }
  _raiseposi(&x, &expx, 2, 100);
  if (float_cmp(&x, &c1) != 0 || expx != 0)
  {
    printf("test case -1 ^ 2 FAILED");
    return 0;
  }
  printf("testing error limit:\n");
  float_setzero(&max);
  float_setasciiz(&step, "1");
  float_setasciiz(&ofs, "1.000147436738983647399324678823992433673"
         "24672389823467828246764829833467849839838778485784386584539"
         "85343784568643548923998439823432792346");
  for (j = 0; j++ < 30;)
    for(i = 0; ++i < 10;)
    {
      float_muli(&tmp, &step, i, EXACT);
      float_add(&x, &tmp, &ofs, EXACT);
      float_clone(&tmp, &x, EXACT);
      _raiseposi(&x, &expx, (1 << j)-1, 100);
      _raiseposi(&tmp, &expx, (1 << j)-1, 110);
      _relerror(&x, &tmp);
      if (float_cmp(&x, &max) > 0)
      {
        float_clone(&max, &x, EXACT);
        if (float_getexponent(&x) >= -99)
        {
          printf("exceeding error in test case %d: ", i);
          float_getscientific(buf, 50, &max);
          printf("%s\n", buf);
          return 0;
        }
      }
    }
  float_getscientific(buf, 50, &max);
  printf("max error _raiseposi: %s\n", buf);

  float_free(&tmp);
  float_free(&x);
  float_free(&max);
  float_free(&step);
  float_free(&ofs);
  return 1;
}

static int test_raisei()
{
  floatstruct x, y;

  printf ("testing _raisei\n");
  float_create(&x);
  float_create(&y);
  float_clone(&x, &c1, EXACT);
  _raisei(&x, 0, 100);
  if (float_cmp(&x, &c1) != 0)
  {
    printf("test case 1^0 FAILED\n");
    return 0;
  }
  _raisei(&x, 1, 100);
  if (float_cmp(&x, &c1) != 0)
  {
    printf("test case 1^1 FAILED\n");
    return 0;
  }
  _raisei(&x, 100, 100);
  if (float_cmp(&x, &c1) != 0)
  {
    printf("test case 1^100 FAILED\n");
    return 0;
  }
  float_clone(&x, &c2, EXACT);
  _raisei(&x, 0, 100);
  if (float_cmp(&x, &c1) != 0)
  {
    printf("test case 2^0 FAILED\n");
    return 0;
  }
  float_clone(&x, &c2, EXACT);
  _raisei(&x, 1, 100);
  if (float_cmp(&x, &c2) != 0)
  {
    printf("test case 2^1 FAILED\n");
    return 0;
  }
  float_setinteger(&y, 4);
  _raisei(&x, 2, 100);
  if (float_cmp(&x, &y) != 0)
  {
    printf("test case 2^2 FAILED\n");
    return 0;
  }
  float_clone(&x, &c2, EXACT);
  _raisei(&x, -1, 100);
  if (float_cmp(&x, &c1Div2) != 0)
  {
    printf("test case 2^-1 FAILED\n");
    return 0;
  }

  float_free(&y);
  float_free(&x);
  return 1;
}

static int test_lngammaseries()
{
  floatstruct x, y, tmp;
  int i;

  printf("testing lngammaseries\n");

  float_create(&x);
  float_create(&y);
  float_create(&tmp);
  printf("verifying result\n");
  float_setinteger(&tmp, 77);
  float_ln(&tmp, 110);
  float_setasciiz(&y, "76.5");
  float_mul(&tmp, &tmp, &y, 110);
  float_setinteger(&y, 2);
  for (i = 2; ++i < 77;)
    float_muli(&y, &y, i, EXACT);
  float_ln(&y, 110);
  float_sub(&y, &y, &tmp, 110);
  float_addi(&y, &y, 77, 110);
  float_sub(&y, &y, &cLnSqrt2PiMinusHalf, 110);
  float_sub(&y, &y, &c1Div2, 110);
  float_setinteger(&x, 77);
  lngammaseries(&x, 100);
  if (!_cmprelerror(&y, &x, -95))
  {
    printf("verification FAILED\n");
    return 0;
  }
  float_free(&x);
  float_free(&y);
  float_free(&tmp);
  return 1;
}

static int tc_pochhammer(char* x, char* n, char* result)
{
  floatstruct fx, fn, fr;

  float_create(&fx);
  float_create(&fn);
  float_create(&fr);
  float_setasciiz(&fx, x);
  float_setasciiz(&fn, n);
  float_setasciiz(&fr, result);
  float_pochhammer(&fx, &fn, 100);
  if (float_isnan(&fr))
    return float_isnan(&fx);
  if (!_cmprelerror(&fr, &fx, -95))
  {
    printf("verification FAILED\n");
    return 0;
  }
  return 1;
}

static int test_pochhammer()
{
  printf("testing _pochhammer\n");
  printf("verifying results\n");
  if (!tc_pochhammer("0", "0", "1")) return 0;
  if (!tc_pochhammer("1", "0", "1")) return 0;
  if (!tc_pochhammer("-1", "0", "1")) return 0;
  if (!tc_pochhammer("1e100000", "0", "1")) return 0;
  if (!tc_pochhammer("-1e100000", "0", "1")) return 0;
  if (!tc_pochhammer("0.5", "0", "1")) return 0;
  if (!tc_pochhammer("-0.5", "0", "1")) return 0;
  if (!tc_pochhammer("0", "1", "0")) return 0;
  if (!tc_pochhammer("1", "1", "1")) return 0;
  if (!tc_pochhammer("-1", "1", "-1")) return 0;
  if (!tc_pochhammer("1e100000", "1", "1e100000")) return 0;
  if (!tc_pochhammer("-1e100000", "1", "-1e100000")) return 0;
  if (!tc_pochhammer("0.5", "1", "0.5")) return 0;
  if (!tc_pochhammer("-0.5", "1", "-0.5")) return 0;
  if (!tc_pochhammer("0", "2", "0")) return 0;
  if (!tc_pochhammer("1", "2", "2")) return 0;
  if (!tc_pochhammer("-1", "2", "0")) return 0;
  if (!tc_pochhammer("1e100000", "2", "1e200000")) return 0;
  if (!tc_pochhammer("-1e100000", "2", "1e200000")) return 0;
  if (!tc_pochhammer("0.5", "2", "0.75")) return 0;
  if (!tc_pochhammer("-0.5", "2", "-0.25")) return 0;
  if (!tc_pochhammer("1", "100", "93326215443944152681699238856266700490715968264381621468592963895217"
       "59999322991560894146397615651828625369792082722375825118521091686400"
       "0000000000000000000000")) return 0;
  if (!tc_pochhammer("-100", "101", "0")) return 0;
  if (!tc_pochhammer("-100", "100", "93326215443944152681699238856266700490715968264381621468592963895217"
       "59999322991560894146397615651828625369792082722375825118521091686400"
       "0000000000000000000000")) return 0;
  if (!tc_pochhammer("1", "-1", "NaN")) return 0;
  if (!tc_pochhammer("2", "-1", "1")) return 0;
  if (!tc_pochhammer("2", "-2", "NaN")) return 0;
  if (!tc_pochhammer("1.5", "-1", "2")) return 0;
  if (!tc_pochhammer("1.5", "-2", "-4")) return 0;
  if (!tc_pochhammer("1.5", "-100", "-.000000000000000000000000000000000000000000000000000000000000000000"
                     "00000000000000000000000000000000000000000000000000000000000000000000"
                     "00000000000000000000378414024807910640737099845525006763221785165860"
                     "06641218189552393913245454474853055919575212143764296945522753626016"
                     "300639832411419246985800884362")) return 0;
  if (!tc_pochhammer("-1.5", "100", "40242764267707289912333641217950774848709656376758843494819950464847"
                     "04420564703857828107982141126998570896720926829585700250010032188267"
                     "20546901916750645.18900647214084211566623156559209401240552373723543")) return 0;
  if (!tc_pochhammer("-3", "-1", "-0.25")) return 0;
  if (!tc_pochhammer("-1e100000", "-1", "-1e-100000")) return 0;
  if (!tc_pochhammer("-2.2", "1.3", "4.793948981105061902495028514824576929486499142574590370996037700670"
                     "07956370445687178188113235399766021571905874962934089400569549735499"
                     "47062783690929661636680480933415006255499710782394427770555172981506")) return 0;
  return 1;
}

static int test_lngamma()
{
  floatstruct x, tmp, max, step, ofs;
  int i;
  char buf[50];

  printf("testing _lngamma\n");
  float_create(&x);
  float_create(&tmp);
  float_create(&max);
  float_create(&step);
  float_create(&ofs);

  printf("verifying results\n");
  float_clone(&x, &c3, EXACT);
  _lngamma(&x, 100);
  if(!_cmprelerror(&x, &cLn2, -95))
  {
    printf("verification FAILED\n");
    return 0;
  }
  printf("testing error limit:\n");
  float_setzero(&max);
  float_setasciiz(&step, "0.05");
  float_setasciiz(&ofs, "0.000147436738983647399324678823992433673"
         "24672389823467828246764829833467849839838778485784386584539"
         "85343784568643548923998439823432792346");
  for(i = 100; ++i < 200;)
  {
    float_muli(&tmp, &step, i, EXACT);
    float_add(&x, &tmp, &ofs, EXACT);
    float_clone(&tmp, &x, EXACT);
    _lngamma(&x, 100);
    _lngamma(&tmp, 110);
    _relerror(&x, &tmp);
    if (float_cmp(&x, &max) > 0)
    {
      float_clone(&max, &x, EXACT);
      if (float_getexponent(&x) >= -99)
      {
        printf("exceeding error in test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  float_getscientific(buf, 50, &max);
  printf("max error _lngamma: %s\n", buf);
  float_free(&tmp);
  float_free(&x);
  float_free(&max);
  float_free(&step);
  float_free(&ofs);
  return 1;
}

static int test_gamma()
{
  floatstruct x, tmp, max, step, ofs;
  int i;
  char buf[50];

  printf("testing _gamma\n");

  float_create(&x);
  float_create(&tmp);
  float_create(&max);
  float_create(&step);
  float_create(&ofs);
  printf("verifying results\n");
  float_setasciiz(&x, "0.2");
  float_setasciiz(&tmp, "4.590843711998803053204758275929152003434109998293403"
                        "01778885313623003927310644499897403940828778507456912");
  _gamma(&x, 100);
  if(!_cmprelerror(&x, &tmp, -95))
  {
    printf("verification FAILED\n");
    return 0;
  }
  float_setasciiz(&x, "-30.2");
  float_setasciiz(&tmp, "-1.0165368282364803216788184978362164979757349922035594"
                        "1791712115025420329582124053302476303836291973429411315e-32");
  _gamma(&x, 100);
  if(!_cmprelerror(&x, &tmp, -95))
  {
    printf("verification FAILED\n");
    return 0;
  }
  float_setasciiz(&x, "-1.5");
  float_setasciiz(&tmp, "0.75");
  float_div(&tmp, &cSqrtPi, &tmp, 110);
  _gamma(&x, 100);
  if(!_cmprelerror(&x, &tmp, -95))
  {
    printf("verification FAILED\n");
    return 0;
  }
  float_setasciiz(&x, "10");
  float_setasciiz(&tmp, "362880");
  _gamma(&x, 100);
  if(!_cmprelerror(&x, &tmp, -95))
  {
    printf("verification FAILED\n");
    return 0;
  }
  printf("testing error limit:\n");
  float_setzero(&max);
  float_setasciiz(&step, "0.05");
  float_setasciiz(&ofs, "0.000147436738983647399324678823992433673"
         "24672389823467828246764829833467849839838778485784386584539"
         "85343784568643548923998439823432792346");
  for(i = 100; ++i < 200;)
  {
    float_muli(&tmp, &step, i, EXACT);
    float_add(&x, &tmp, &ofs, EXACT);
    float_clone(&tmp, &x, EXACT);
    _gamma(&x, 100);
    _gamma(&tmp, 110);
    _relerror(&x, &tmp);
    if (float_cmp(&x, &max) > 0)
    {
      float_clone(&max, &x, EXACT);
      if (float_getexponent(&x) >= -99)
      {
        printf("exceeding error in test case %d: ", i);
        float_getscientific(buf, 50, &max);
        printf("%s\n", buf);
        return 0;
      }
    }
  }
  printf("testing asymptotic series of gamma:\n");
  for(i = 0; ++i < 100;)
  {
    float_setasciiz(&x, "1.01");
    printf("testing precision %d: ", i);
    _gamma(&x, i);
    printf("asymptotic series converging\n");
  }
  float_getscientific(buf, 50, &max);
  printf("max error _gamma: %s\n", buf);
  float_free(&tmp);
  float_free(&x);
  float_free(&max);
  float_free(&step);
  float_free(&ofs);
  return 1;
}

static int test_gammaint()
{
  floatstruct x, y;

  printf("testing _gammaint\n");

  float_create(&x);
  float_create(&y);
  printf("verifying results\n");
  float_setinteger(&x, 1);
  float_setinteger(&y, 1);
  _gammaint(&x, 100);
  if (float_cmp(&x, &y) != 0)
  {
    printf("verification FAILED\n");
    return 0;
  }
  float_setinteger(&x, 2);
  float_setinteger(&y, 1);
  _gammaint(&x, 100);
  if (float_cmp(&x, &y) != 0)
  {
    printf("verification FAILED\n");
    return 0;
  }
  float_setinteger(&x, 3);
  float_setinteger(&y, 2);
  _gammaint(&x, 100);
  if (float_cmp(&x, &y) != 0)
  {
    printf("verification FAILED\n");
    return 0;
  }

  float_free(&x);
  float_free(&y);
  return 1;
}

static int tc_gamma0_5(char* value)
{
  floatstruct x, y;

  float_create(&x);
  float_create(&y);
  float_setasciiz(&x, value);
  float_clone(&y, &x, EXACT);
  _gamma0_5(&x, 100);
  _gamma(&y, 100);
  if (!_cmprelerror(&x, &y, -95))
  {
    printf("verification FAILED\n");
    return 0;
  }
  float_free(&x);
  float_free(&y);
  return 1;
}

static int test_gamma0_5()
{
  printf("testing _gamma0_5\n");

  printf("verifying results\n");
  if (!tc_gamma0_5(".5")) return 0;
  if (!tc_gamma0_5("-.5")) return 0;
  if (!tc_gamma0_5("1.5")) return 0;
  if (!tc_gamma0_5("-1.5")) return 0;
  if (!tc_gamma0_5("-30.5")) return 0;
  if (!tc_gamma0_5("30.5")) return 0;
  return 1;
}

static int tc_floatnum2logic(char* value, unsigned x0, unsigned x1, unsigned x2)
{
  floatstruct x;
  t_longint r;

  float_create(&x);
  float_setasciiz(&x, value);
  _floatnum2logic(&r, &x);
  float_free(&x);
  return r.value[0] == x0 && r.value[1] == x1 && r.value[2] == x2;
}

static int test_floatnum2logic()
{
  /* based on the size of unsigned */
  printf("testing _floatnum2logic\n");

  if(!tc_floatnum2logic("0", 0, 0, 0)) return 0;
  if(!tc_floatnum2logic("1", 1, 0, 0)) return 0;
  if(!tc_floatnum2logic("-1", ~0, ~0, ~0)) return 0;
  if(!tc_floatnum2logic("-2147483648", ~2147483647, ~0, ~0)) return 0;
  if(!tc_floatnum2logic("8589934592", 0, 2, 0)) return 0;
  return 1;
}

static int tc_logic2floatnum(char* value)
{
  t_longint l;
  floatstruct x, y;

  float_create(&x);
  float_create(&y);
  float_setasciiz(&x, value);
  _floatnum2logic(&l, &x);
  _logic2floatnum(&y, &l);
  if (float_cmp(&x, &y) != 0)
  {
    printf("test case %s FAILED\n", value);
    return 0;
  }
  float_free(&x);
  float_free(&y);
  return 1;
}

static int test_logic2floatnum()
{
  printf("testing _logic2floatnum\n");

  if(!tc_logic2floatnum("0")) return 0;
  if(!tc_logic2floatnum("1")) return 0;
  if(!tc_logic2floatnum("-1")) return 0;
  if(!tc_logic2floatnum("-2147483648")) return 0;
  if(!tc_logic2floatnum("-2147483649")) return 0;
  if(!tc_logic2floatnum("4294967295")) return 0;
  if(!tc_logic2floatnum("-4294967296")) return 0;
  if(!tc_logic2floatnum("8589934592")) return 0;
  return 1;
}

static int tc_not(char* value, char* result)
{
  floatstruct x, y;

  float_create(&x);
  float_create(&y);
  float_setasciiz(&x, value);
  float_setasciiz(&y, result);
  float_not(&x);
  if (float_cmp(&x, &y) != 0) return 0;
  float_free(&x);
  float_free(&y);
  return 1;
}

static int test_not()
{
  printf("testing float_not\n");

  if (!tc_not("0", "-1")) return 0;
  if (!tc_not("1", "-2")) return 0;
  if (!tc_not("-1", "0")) return 0;
  if (!tc_not("-2", "1")) return 0;
  return 1;
}

static int test_and()
{
  floatstruct x, y;

  printf("testing float_and\n");

  float_create(&x);
  float_create(&y);
  float_setasciiz(&x, "5");
  float_setasciiz(&y, "6");
  float_and(&x, &x, &y);
  float_setasciiz(&y, "4");
  if (float_cmp(&x, &y) != 0) return 0;
  float_free(&x);
  float_free(&y);
  return 1;
}

static int test_or()
{
  floatstruct x, y;

  printf("testing float_or\n");

  float_create(&x);
  float_create(&y);
  float_setasciiz(&x, "5");
  float_setasciiz(&y, "6");
  float_or(&x, &x, &y);
  float_setasciiz(&y, "7");
  if (float_cmp(&x, &y) != 0) return 0;
  float_free(&x);
  float_free(&y);
  return 1;
}

static int test_xor()
{
  floatstruct x, y;

  printf("testing float_xor\n");

  float_create(&x);
  float_create(&y);
  float_setasciiz(&x, "5");
  float_setasciiz(&y, "6");
  float_xor(&x, &x, &y);
  float_setasciiz(&y, "3");
  if (float_cmp(&x, &y) != 0) return 0;
  float_free(&x);
  float_free(&y);
  return 1;
}

static int tc_shl(char* value, char* shift, char* result)
{
  floatstruct x, y;

  float_create(&x);
  float_create(&y);

  float_setasciiz(&x, value);
  float_setasciiz(&y, shift);
  float_shl(&x, &x, &y);
  float_setasciiz(&y, result);

  if(float_cmp(&x, &y) != 0) return 0;

  float_free(&x);
  float_free(&y);
  return 1;
}

static int test_shl()
{
  printf("testing float_shl\n");

  if (!tc_shl("0", "0", "0")) return 0;
  if (!tc_shl("-1", "0", "-1")) return 0;
  if (!tc_shl("1", "1", "2")) return 0;
  if (!tc_shl("1", "32", "4294967296")) return 0;
  if (!tc_shl("1", "64", "18446744073709551616")) return 0;
  if (!tc_shl("1", "96", "0")) return 0;
  if (!tc_shl("1", "95", "-39614081257132168796771975168")) return 0;
  if (!tc_shl("1", "1000", "0")) return 0;
  if (!tc_shl("4294967295", "1", "8589934590")) return 0;
  return 1;
}

static int tc_shr(char* value, char* shift, char* result)
{
  floatstruct x, y;

  float_create(&x);
  float_create(&y);

  float_setasciiz(&x, value);
  float_setasciiz(&y, shift);
  float_shr(&x, &x, &y);
  float_setasciiz(&y, result);

  if(float_cmp(&x, &y) != 0) return 0;

  float_free(&x);
  float_free(&y);
  return 1;
}

static int test_shr()
{
  printf("testing float_shr\n");

  if (!tc_shr("0", "0", "0")) return 0;
  if (!tc_shr("1", "1", "0")) return 0;
  if (!tc_shr("2", "1", "1")) return 0;
  if (!tc_shr("4294967296", "32", "1")) return 0;
  if (!tc_shr("-1", "32", "-1")) return 0;
  if (!tc_shr("-4", "1", "-2")) return 0;
  if (!tc_shr("-39614081257132168796771975168", "94", "-2")) return 0;
  return 1;
}

static int tc_out(char* value, int digits, char base, char mode, char* result)
{
  char intbuf[150];
  char fracbuf[150];
  char exp[33];
  char buffer[350];
  t_otokens tokens;
  floatstruct x;

  float_create(&x);
  float_setasciiz(&x, value);
  tokens.intpart.sz = sizeof(intbuf);
  tokens.intpart.buf = intbuf;
  tokens.fracpart.sz = 150;
  tokens.fracpart.buf = fracbuf;
  tokens.exp.buf = exp;
  tokens.exp.sz = 33;
  if (!float_out(&tokens, &x, digits, base, base != 2? base : 16, mode))
    return result == NULL || *result == '\0';
  cattokens(buffer, 350, &tokens,
    IO_FLAG_SHOW_BASE + IO_FLAG_SHOW_EXPBASE + IO_FLAG_SUPPRESS_DOT
    + IO_FLAG_SUPPRESS_LDG_ZERO);
  float_free(&x);
  return strcmp(buffer, result) == 0;
}

static int test_out()
{
  t_ioparams iop;
  t_ioparams saveiop;

  iop.base = 10;
  iop.expbase = 10;
  iop.dot = '.';
  iop.basetag = "0d";
  iop.expbegin = "(";
  iop.expend = ")";
  iop.cmpltag = NULL;
  iop.maxdigits = 70;
  saveiop = *getioparams(10);
  setioparams(&iop);
  printf("testing float_out\n");

  if (!tc_out("NaN", 0, 10, IO_MODE_SCIENTIFIC, "NaN")) return 0;
  if (!tc_out("0", 0, IO_BASE_NAN, IO_MODE_SCIENTIFIC, "")) return 0;
  if (!tc_out("0", 0, IO_BASE_ZERO, IO_MODE_SCIENTIFIC, "")) return 0;
  if (!tc_out("1", 0, IO_BASE_NAN, IO_MODE_SCIENTIFIC, "")) return 0;
  if (!tc_out("0", 0, 5, IO_MODE_SCIENTIFIC, "")) return 0;
  if (!tc_out("0", 0, 10, -1, "")) return 0;
  if (!tc_out("0", 0, 10, IO_MODE_SCIENTIFIC, "0")) return 0;
  if (!tc_out("1", 0, 10, IO_MODE_SCIENTIFIC, "+0d1(0)")) return 0;
  if (!tc_out("-1", 0, 10, IO_MODE_SCIENTIFIC, "-0d1(0)")) return 0;
  if (!tc_out("1.000000002", 0, 10, IO_MODE_SCIENTIFIC, "+0d1(0)")) return 0;
  if (!tc_out("1.0002", 1, 10, IO_MODE_SCIENTIFIC, "+0d1.0(0)")) return 0;
  if (!tc_out("1.07", 1, 10, IO_MODE_SCIENTIFIC, "+0d1.1(0)")) return 0;
  if (!tc_out("1.2", 1, 10, IO_MODE_SCIENTIFIC, "+0d1.2(0)")) return 0;
  if (!tc_out("1.2", 2, 10, IO_MODE_SCIENTIFIC, "+0d1.20(0)")) return 0;
  if (!tc_out("10", 0, 10, IO_MODE_SCIENTIFIC, "+0d1(+0d1)")) return 0;
  if (!tc_out("100", 0, 10, IO_MODE_SCIENTIFIC, "+0d1(+0d2)")) return 0;
  if (!tc_out("1e12345678", 0, 10, IO_MODE_SCIENTIFIC, "+0d1(+0d12345678)")) return 0;
  if (!tc_out("1.234e12345678", 0, 10, IO_MODE_SCIENTIFIC, "+0d1(+0d12345678)")) return 0;
  if (!tc_out("1.234e12345678", 1, 10, IO_MODE_SCIENTIFIC, "+0d1.2(+0d12345678)")) return 0;
  if (!tc_out("1.234e12345678", 3, 10, IO_MODE_SCIENTIFIC, "+0d1.234(+0d12345678)")) return 0;
  if (!tc_out("1.234e12345678", 4, 10, IO_MODE_SCIENTIFIC, "+0d1.2340(+0d12345678)")) return 0;
  if (!tc_out("0.1", 0, 10, IO_MODE_SCIENTIFIC, "+0d1(-0d1)")) return 0;
  if (!tc_out("1.234e-12345678", 0, 10, IO_MODE_SCIENTIFIC, "+0d1(-0d12345678)")) return 0;
  if (!tc_out("1.234e-12345678", 1, 10, IO_MODE_SCIENTIFIC, "+0d1.2(-0d12345678)")) return 0;
  if (!tc_out("1.234e-12345678", 3, 10, IO_MODE_SCIENTIFIC, "+0d1.234(-0d12345678)")) return 0;
  if (!tc_out("1.234e-12345678", 4, 10, IO_MODE_SCIENTIFIC, "+0d1.2340(-0d12345678)")) return 0;
  if (!tc_out("1", 0, 16, IO_MODE_SCIENTIFIC, "+0x1(0)")) return 0;
  if (!tc_out("15", 0, 16, IO_MODE_SCIENTIFIC, "+0xF(0)")) return 0;
  if (!tc_out("16", 0, 16, IO_MODE_SCIENTIFIC, "+0x1(+0x1)")) return 0;
  if (!tc_out("17", 0, 16, IO_MODE_SCIENTIFIC, "+0x1(+0x1)")) return 0;
  if (!tc_out("18", 1, 16, IO_MODE_SCIENTIFIC, "+0x1.2(+0x1)")) return 0;
  if (!tc_out("4096", 0, 16, IO_MODE_SCIENTIFIC, "+0x1(+0x3)")) return 0;
  if (!tc_out("6144", 0, 16, IO_MODE_SCIENTIFIC, "+0x2(+0x3)")) return 0;
  if (!tc_out("1099511627776", 0, 16, IO_MODE_SCIENTIFIC, "+0x1(+0xA)")) return 0;
  if (!tc_out("18446744073709551616", 0, 16, IO_MODE_SCIENTIFIC, "+0x1(+0x10)")) return 0;
  if (!tc_out("18446744073709551616", 15, 16, IO_MODE_SCIENTIFIC, "+0x1.000000000000000(+0x10)")) return 0;
  if (!tc_out("1.8", 0, 16, IO_MODE_SCIENTIFIC, "+0x2(0)")) return 0;
  if (!tc_out("-1.5", 1, 16, IO_MODE_SCIENTIFIC, "-0x1.8(0)")) return 0;
  if (!tc_out("0.5", 0, 16, IO_MODE_SCIENTIFIC, "+0x8(-0x1)")) return 0;
  if (!tc_out("0.25", 0, 16, IO_MODE_SCIENTIFIC, "+0x4(-0x1)")) return 0;
  if (!tc_out("0.125", 0, 16, IO_MODE_SCIENTIFIC, "+0x2(-0x1)")) return 0;
  if (!tc_out("0.0625", 0, 16, IO_MODE_SCIENTIFIC, "+0x1(-0x1)")) return 0;
  if (!tc_out("0.03125", 0, 16, IO_MODE_SCIENTIFIC, "+0x8(-0x2)")) return 0;
  if (!tc_out("0.75", 0, 16, IO_MODE_SCIENTIFIC, "+0xC(-0x1)")) return 0;
  if (!tc_out("0.96875", 0, 16, IO_MODE_SCIENTIFIC, "+0x1(0)")) return 0;
  if (!tc_out("5.42101086242752217003726400434970855712890625e-20", 0, 16, IO_MODE_SCIENTIFIC, "+0x1(-0x10)")) return 0;
  if (!tc_out("5.42101086242752217003726400434970855712890625e-20", 16, 16, IO_MODE_SCIENTIFIC,
              "+0x1.0000000000000000(-0x10)")) return 0;
  if (!tc_out("5.42101086242752217003726400434970855712890625e-20", 15, 16, IO_MODE_SCIENTIFIC,
              "+0x1.000000000000000(-0x10)")) return 0;
  if (!tc_out("5.42101086242752217003726400434970855712890625e-20", 14, 16, IO_MODE_SCIENTIFIC,
              "+0x1.00000000000000(-0x10)")) return 0;
  if (!tc_out("5.42101086242752217003726400434970855712890625e-20", 13, 16, IO_MODE_SCIENTIFIC,
              "+0x1.0000000000000(-0x10)")) return 0;
  if (!tc_out("5.42101086242752217003726400434970855712890625e-20", 12, 16, IO_MODE_SCIENTIFIC,
              "+0x1.000000000000(-0x10)")) return 0;
  if (!tc_out("5.42101086242752217003726400434970855712890625e-20", 11, 16, IO_MODE_SCIENTIFIC,
              "+0x1.00000000000(-0x10)")) return 0;
  if (!tc_out("5.42101086242752217003726400434970855712890625e-20", 10, 16, IO_MODE_SCIENTIFIC,
              "+0x1.0000000000(-0x10)")) return 0;
  if (!tc_out("5.42101086242752217003726400434970855712890625e-20", 9, 16, IO_MODE_SCIENTIFIC,
              "+0x1.000000000(-0x10)")) return 0;
  if (!tc_out("5.42101086242752217003726400434970855712890625e-20", 8, 16, IO_MODE_SCIENTIFIC,
              "+0x1.00000000(-0x10)")) return 0;
  if (!tc_out("5.42101086242752217003726400434970855712890625e-20", 7, 16, IO_MODE_SCIENTIFIC,
              "+0x1.0000000(-0x10)")) return 0;
  if (!tc_out("5.42101086242752217003726400434970855712890625e-20", 6, 16, IO_MODE_SCIENTIFIC,
              "+0x1.000000(-0x10)")) return 0;
  if (!tc_out("5.42101086242752217003726400434970855712890625e-20", 5, 16, IO_MODE_SCIENTIFIC,
              "+0x1.00000(-0x10)")) return 0;
  if (!tc_out("5.42101086242752217003726400434970855712890625e-20", 4, 16, IO_MODE_SCIENTIFIC,
              "+0x1.0000(-0x10)")) return 0;
  if (!tc_out("5.42101086242752217003726400434970855712890625e-20", 3, 16, IO_MODE_SCIENTIFIC,
              "+0x1.000(-0x10)")) return 0;
  if (!tc_out("5.42101086242752217003726400434970855712890625e-20", 2, 16, IO_MODE_SCIENTIFIC,
              "+0x1.00(-0x10)")) return 0;
  if (!tc_out("5.42101086242752217003726400434970855712890625e-20", 1, 16, IO_MODE_SCIENTIFIC,
              "+0x1.0(-0x10)")) return 0;
  if (!tc_out("1", 0, 2, IO_MODE_SCIENTIFIC, "+0b1(0)")) return 0;
  if (!tc_out("2", 0, 2, IO_MODE_SCIENTIFIC, "+0b1(+0x1)")) return 0;
  if (!tc_out("3", 0, 2, IO_MODE_SCIENTIFIC, "+0b1(+0x2)")) return 0;
  if (!tc_out("3", 1, 2, IO_MODE_SCIENTIFIC, "+0b1.1(+0x1)")) return 0;
  if (!tc_out("3", 2, 2, IO_MODE_SCIENTIFIC, "+0b1.10(+0x1)")) return 0;
  if (!tc_out("0.5", 0, 2, IO_MODE_SCIENTIFIC, "+0b1(-0x1)")) return 0;
  if (!tc_out("0.25", 0, 2, IO_MODE_SCIENTIFIC, "+0b1(-0x2)")) return 0;
  if (!tc_out("0.75", 0, 2, IO_MODE_SCIENTIFIC, "+0b1(-0x1)")) return 0;
  if (!tc_out("-0.75", 1, 2, IO_MODE_SCIENTIFIC, "-0b1.1(-0x1)")) return 0;
  if (!tc_out("1", 0, 8, IO_MODE_SCIENTIFIC, "+0o1(0)")) return 0;
  if (!tc_out("2", 0, 8, IO_MODE_SCIENTIFIC, "+0o2(0)")) return 0;
  if (!tc_out("4", 0, 8, IO_MODE_SCIENTIFIC, "+0o4(0)")) return 0;
  if (!tc_out("8", 0, 8, IO_MODE_SCIENTIFIC, "+0o1(+0o1)")) return 0;
  if (!tc_out("16", 0, 8, IO_MODE_SCIENTIFIC, "+0o2(+0o1)")) return 0;
  if (!tc_out("0.5", 0, 8, IO_MODE_SCIENTIFIC, "+0o4(-0o1)")) return 0;
  if (!tc_out("0.75", 0, 8, IO_MODE_SCIENTIFIC, "+0o6(-0o1)")) return 0;
  if (!tc_out("0.875", 0, 8, IO_MODE_SCIENTIFIC, "+0o7(-0o1)")) return 0;
  if (!tc_out("0.9375", 1, 8, IO_MODE_SCIENTIFIC, "+0o7.4(-0o1)")) return 0;
  if (!tc_out("0.9375", 0, 8, IO_MODE_SCIENTIFIC, "+0o1(0)")) return 0;
  if (!tc_out("NaN", 1, 10, IO_MODE_FIXPOINT, "NaN")) return 0;
  if (!tc_out("0", 1, IO_BASE_NAN, IO_MODE_FIXPOINT, "")) return 0;
  if (!tc_out("0", 1, IO_BASE_ZERO, IO_MODE_FIXPOINT, "")) return 0;
  if (!tc_out("1", 1, IO_BASE_NAN, IO_MODE_FIXPOINT, "")) return 0;
  if (!tc_out("0", 1, 5, IO_MODE_FIXPOINT, "")) return 0;
  if (!tc_out("0", 1, 10, IO_MODE_FIXPOINT, "0")) return 0;
  if (!tc_out("1", 0, 10, IO_MODE_FIXPOINT, "+0d1")) return 0;
  if (!tc_out("1", 1, 10, IO_MODE_FIXPOINT, "+0d1.0")) return 0;
  if (!tc_out("10", 0, 10, IO_MODE_FIXPOINT, "+0d10")) return 0;
  if (!tc_out("-10", 0, 10, IO_MODE_FIXPOINT, "-0d10")) return 0;
  if (!tc_out("10", 3, 10, IO_MODE_FIXPOINT, "+0d10.000")) return 0;
  if (!tc_out("10.1", 0, 10, IO_MODE_FIXPOINT, "+0d10")) return 0;
  if (!tc_out("10.1", 1, 10, IO_MODE_FIXPOINT, "+0d10.1")) return 0;
  if (!tc_out("10.1", 2, 10, IO_MODE_FIXPOINT, "+0d10.10")) return 0;
  if (!tc_out("10.12", 1, 10, IO_MODE_FIXPOINT, "+0d10.1")) return 0;
  if (!tc_out("10.16", 1, 10, IO_MODE_FIXPOINT, "+0d10.2")) return 0;
  if (!tc_out("10.86", 0, 10, IO_MODE_FIXPOINT, "+0d11")) return 0;
  if (!tc_out("0.1", 1, 10, IO_MODE_FIXPOINT, "+0d.1")) return 0;
  if (!tc_out("0.12", 1, 10, IO_MODE_FIXPOINT, "+0d.1")) return 0;
  if (!tc_out("0.0123", 2, 10, IO_MODE_FIXPOINT, "+0d.01")) return 0;
  if (!tc_out("18446744073709551616", 0, 10, IO_MODE_FIXPOINT, "+0d18446744073709551616")) return 0;
  if (!tc_out("0.75", 1, 16, IO_MODE_FIXPOINT, "+0x.C")) return 0;
  if (!tc_out("18446744073709551616", 0, 16, IO_MODE_FIXPOINT, "+0x10000000000000000")) return 0;
  if (!tc_out("18446744073709551615", 0, 16, IO_MODE_FIXPOINT, "+0xFFFFFFFFFFFFFFFF")) return 0;
  if (!tc_out("18446744073709551615", 1, 16, IO_MODE_FIXPOINT, "+0xFFFFFFFFFFFFFFFF.0")) return 0;
  if (!tc_out("5.42101086242752217003726400434970855712890625e-20", 16, 16, IO_MODE_FIXPOINT,
              "+0x.0000000000000001")) return 0;
  if (!tc_out("-0.75", 2, 2, IO_MODE_FIXPOINT, "-0b.11")) return 0;
  if (!tc_out("0.9375", 2, 8, IO_MODE_FIXPOINT, "+0o.74")) return 0;
  if (!tc_out("4.9375", 2, 8, IO_MODE_FIXPOINT, "+0o4.74")) return 0;
  if (!tc_out("4.9375", 1, 8, IO_MODE_FIXPOINT, "+0o5.0")) return 0;
  if (!tc_out("4.9375", 0, 10, IO_MODE_FIXPOINT, "+0d5")) return 0;
  if (!tc_out("4.9375", 0, 16, IO_MODE_FIXPOINT, "+0x5")) return 0;
  if (!tc_out("4.9375", 0, 8, IO_MODE_FIXPOINT, "+0o5")) return 0;
  if (!tc_out("4.9375", 0, 2, IO_MODE_FIXPOINT, "+0b101")) return 0;
  if (!tc_out("1.23", 2, 10, IO_MODE_ENG, "+0d1.23(0)")) return 0;
  if (!tc_out("12.3", 2, 10, IO_MODE_ENG, "+0d12.3(0)")) return 0;
  if (!tc_out("123", 2, 10, IO_MODE_ENG, "+0d123(0)")) return 0;
  if (!tc_out("1234", 2, 10, IO_MODE_ENG, "+0d1.23(+0d3)")) return 0;
  if (!tc_out("0.1234", 2, 10, IO_MODE_ENG, "+0d123(-0d3)")) return 0;
  if (!tc_out("0.0123", 2, 10, IO_MODE_ENG, "+0d12.3(-0d3)")) return 0;
  if (!tc_out("0.00123", 2, 10, IO_MODE_ENG, "+0d1.23(-0d3)")) return 0;
  if (!tc_out("0.000123", 2, 10, IO_MODE_ENG, "+0d123(-0d6)")) return 0;
  if (!tc_out("-0.000123", 2, 10, IO_MODE_ENG, "-0d123(-0d6)")) return 0;
  if (!tc_out("0", 0, 16, IO_MODE_COMPLEMENT, "0")) return 0;
  if (!tc_out("1", 0, 16, IO_MODE_COMPLEMENT, "0x1")) return 0;
  if (!tc_out("-1", 0, 16, IO_MODE_COMPLEMENT, "0xsF")) return 0;
  if (!tc_out("-2", 0, 16, IO_MODE_COMPLEMENT, "0xsFE")) return 0;
  if (!tc_out("-16", 0, 16, IO_MODE_COMPLEMENT, "0xsF0")) return 0;
  if (!tc_out("-17", 0, 16, IO_MODE_COMPLEMENT, "0xsFEF")) return 0;
  setioparams(&saveiop);
  return 1;
}

static int tc_in(const char* text, char* result)
{
  t_itokens tokens;
  floatstruct x;
  floatstruct r;
  int cmp;

  float_create(&x);
  float_create(&r);
  float_setasciiz(&r, result);
  parse(&tokens, &text);
  float_in(&x, &tokens);
  if (float_isnan(&r) && float_isnan(&x))
    cmp = 0;
  else
    cmp = float_cmp(&x, &r);
  float_free(&x);
  float_free(&r);
  return cmp == 0;
}

static int test_in()
{
  printf("testing float_in\n");

  if (!tc_in("", "NaN")) return 0;
  if (!tc_in("NaN", "NaN")) return 0;
  if (!tc_in("0", "0")) return 0;
  if (!tc_in("0.", "0")) return 0;
  if (!tc_in(".0", "0")) return 0;
  if (!tc_in("+0", "0")) return 0;
  if (!tc_in("0d0", "0")) return 0;
  if (!tc_in("+0d0", "0")) return 0;
  if (!tc_in("-0", "0")) return 0;
  if (!tc_in("0000", "0")) return 0;
  if (!tc_in(".0000", "0")) return 0;
  if (!tc_in("00.00", "0")) return 0;
  if (!tc_in("1", "1")) return 0;
  if (!tc_in("0d1", "1")) return 0;
  if (!tc_in("1.", "1")) return 0;
  if (!tc_in("1.00", "1")) return 0;
  if (!tc_in("001.00", "1")) return 0;
  if (!tc_in("+1", "1")) return 0;
  if (!tc_in("-1", "-1")) return 0;
  if (!tc_in("10", "10")) return 0;
  if (!tc_in("010", "10")) return 0;
  if (!tc_in("010.00", "10")) return 0;
  if (!tc_in("123456789", "123456789")) return 0;
  if (!tc_in("0.1", "0.1")) return 0;
  if (!tc_in(".1", "0.1")) return 0;
  if (!tc_in(".1000", "0.1")) return 0;
  if (!tc_in("000.1000", "0.1")) return 0;
  if (!tc_in(".01000", "0.01")) return 0;
  if (!tc_in(".0123456789000", "0.0123456789")) return 0;
  if (!tc_in("2.1", "2.1")) return 0;
  if (!tc_in("001234.567800", "1234.5678")) return 0;
  if (!tc_in("0012340.567800", "12340.5678")) return 0;
  if (!tc_in("001234.0567800", "1234.05678")) return 0;
  if (!tc_in("0012340.0567800", "12340.05678")) return 0;
  if (!tc_in("1e0", "1")) return 0;
  if (!tc_in("1(0)", "1")) return 0;
  if (!tc_in("1e+0", "1")) return 0;
  if (!tc_in("1e-0", "1")) return 0;
  if (!tc_in("0d1e-0d0", "1")) return 0;
  if (!tc_in("1e1", "10")) return 0;
  if (!tc_in("1e+1", "10")) return 0;
  if (!tc_in("1e-1", "0.1")) return 0;
  if (!tc_in("1.e-1", "0.1")) return 0;
  if (!tc_in("1.2e-1", "0.12")) return 0;
  if (!tc_in("0.2e-1", "0.02")) return 0;
  if (!tc_in("12e-1", "1.2")) return 0;
  if (!tc_in("12e-2", "0.12")) return 0;
  if (!tc_in("12e1", "120")) return 0;
  if (!tc_in("0.12e1", "1.2")) return 0;
  if (!tc_in("0.12e2", "12")) return 0;
  if (!tc_in("0x0", "0")) return 0;
  if (!tc_in("0x1", "1")) return 0;
  if (!tc_in("0x1.0", "1")) return 0;
  if (!tc_in("0x0001", "1")) return 0;
  if (!tc_in("0x0001.00", "1")) return 0;
  if (!tc_in("-0x1", "-1")) return 0;
  if (!tc_in("+0x1", "1")) return 0;
  if (!tc_in("0xA", "10")) return 0;
  if (!tc_in("0xa", "10")) return 0;
  if (!tc_in("0x10", "16")) return 0;
  if (!tc_in("0x10000000000000000", "18446744073709551616")) return 0;
  if (!tc_in("0xFFFFFFFFFFFFFFFF", "18446744073709551615")) return 0;
  if (!tc_in("0x.8", "0.5")) return 0;
  if (!tc_in("0x.4", "0.25")) return 0;
  if (!tc_in("0x.08", "0.03125")) return 0;
  if (!tc_in("0x1.2", "1.125")) return 0;
  if (!tc_in("0x10.C", "16.75")) return 0;
  if (!tc_in("0x1(1)", "16")) return 0;
  if (!tc_in("0x1(-1)", "0.0625")) return 0;
  if (!tc_in("0x1(A)", "1099511627776")) return 0;
  if (!tc_in("0x1(0d10)", "1099511627776")) return 0;
  if (!tc_in("0x1(10)", "18446744073709551616")) return 0;
  if (!tc_in("0x1.1(1)", "17")) return 0;
  if (!tc_in("0b0", "0")) return 0;
  if (!tc_in("0b1", "1")) return 0;
  if (!tc_in("0b1.0", "1")) return 0;
  if (!tc_in("-0b1", "-1")) return 0;
  if (!tc_in("0b10", "2")) return 0;
  if (!tc_in("0b11", "3")) return 0;
  if (!tc_in("0b.11", ".75")) return 0;
  if (!tc_in("0b1.1", "1.5")) return 0;
  if (!tc_in("0b1.1(-1)", "0.75")) return 0;
  if (!tc_in("0b1.1(1)", "3")) return 0;
  if (!tc_in("0b1.1(0b1)", "3")) return 0;
  if (!tc_in("0b1.1(10)", "6")) return 0;
  if (!tc_in("0o0", "0")) return 0;
  if (!tc_in("0o1", "1")) return 0;
  if (!tc_in("0o1.0", "1")) return 0;
  if (!tc_in("-0o1", "-1")) return 0;
  if (!tc_in("0o10", "8")) return 0;
  if (!tc_in("0o14", "12")) return 0;
  if (!tc_in("0o.6", ".75")) return 0;
  if (!tc_in("0o1.1", "1.125")) return 0;
  if (!tc_in("0o1.4(-1)", "0.1875")) return 0;
  if (!tc_in("0o1.4(1)", "12")) return 0;
  if (!tc_in("0o1.4(0o1)", "12")) return 0;
  if (!tc_in("0o2(10)", "33554432")) return 0;
  if (!tc_in("0xsF", "-1")) return 0;
  if (!tc_in("0xsFF", "-1")) return 0;
  if (!tc_in("0xsFE", "-2")) return 0;
  if (!tc_in("0xsFFE", "-2")) return 0;
  if (!tc_in("0xsF0", "-16")) return 0;
  if (!tc_in("0xsF01", "-255")) return 0;
  if (!tc_in("0bs1", "-1")) return 0;
  if (!tc_in("0bs11", "-1")) return 0;
  if (!tc_in("0bs10", "-2")) return 0;
  if (!tc_in("0bs110", "-2")) return 0;
  if (!tc_in("0bs101", "-3")) return 0;
  if (!tc_in("0os7", "-1")) return 0;
  if (!tc_in("0os77", "-1")) return 0;
  if (!tc_in("0os76", "-2")) return 0;
  if (!tc_in("0os70", "-8")) return 0;
  if (!tc_in("0os767", "-9")) return 0;
  if (!tc_in("0os701", "-63")) return 0;
  return 1;
}

static int testfailed(char* msg)
{
  printf("\n%s FAILED, tests aborted\n", msg);
  return 1;
}

#endif /* _FLOATNUMTEST */

int main(int argc, char** argv)
{

#ifdef _FLOATNUMTEST
  int scalesave;

  printf("\ntestblock floatlong\n");
  floatmath_init();
  float_stdconvert();
  maxdigits = 150;

  if(!test_longadd()) return testfailed("_longadd");
  if(!test_longmul()) return testfailed("_longmul");
  if(!test_longshr()) return testfailed("_longshr");
  if(!test_checkadd()) return testfailed("_checkadd");
  if(!test_checkmul()) return testfailed("_checkmul");
  if(!test_longarrayadd()) return testfailed("_longarrayadd");
  if(!test_longarraymul()) return testfailed("_longarrayadd");

  printf("\ntestblock floatnum\n");

  scalesave = maxdigits;
  maxdigits = 15;

  if(!test_create()) return testfailed("float_create");
  if(!test_isnan()) return testfailed("float_isnan");
  if(!test_iszero()) return testfailed("float_iszero");
  if(!test_setnan()) return testfailed("float_setnan");
  if(!test_setzero()) return testfailed("float_setzero");
  if(!test_setsignificand()) return testfailed("float_setsignificand");
  if(!test_getsignificand()) return testfailed("float_getsignificand");
  if(!test_setexponent()) return testfailed("float_setexponent");
  if(!test_getexponent()) return testfailed("float_getexponent");
  if(!test_setsign()) return testfailed("float_setsign");
  if(!test_getsign()) return testfailed("float_getsign");
  if(!test_getlength()) return testfailed("float_getlength");
  if(!test_getdigit()) return testfailed("float_getdigit");
  if(!test_getscientific()) return testfailed("float_getscientific");
  if(!test_changesign()) return testfailed("float_changesign");
  if(!test_abs()) return testfailed("float_abs");
  if(!test_setscientific()) return testfailed("float_setscientific");
  if(!test_cmp()) return testfailed("float_cmp");
  if(!test_clone()) return testfailed("float_clone");
  if(!test_round()) return testfailed("float_round");
  if(!test_add()) return testfailed("float_add");
  if(!test_sub()) return testfailed("float_sub");
  if(!test_mul()) return testfailed("float_mul");
  if(!test_div()) return testfailed("float_div");
  if(!test_sqrt()) return testfailed("float_sqrt");
  if(!test_setbcnum()) return testfailed("float_setbcnum");
  if(!test_setinteger()) return testfailed("float_setinteger");
  if(!test_int()) return testfailed("float_int");
  if(!test_frac()) return testfailed("float_frac");
  if(!test_divmod()) return testfailed("float_divmod");
  printf("\nall floatnum tests PASSED\n\n");
  maxdigits = scalesave;

  if(!test_floatnum2longint()) return testfailed("_floatnum2longint");
  if(!test_longint2floatnum()) return testfailed("_longint2floatnum");
  if(!test_out()) return testfailed("float_out");
  if(!test_in()) return testfailed("float_in");

  if(!test_floatnum2logic()) return testfailed("_floatnum2logic");
  if(!test_logic2floatnum()) return testfailed("_logic2floatnum");
  if(!test_not()) return testfailed("float_not");
  if(!test_and()) return testfailed("float_and");
  if(!test_or()) return testfailed("float_or");
  if(!test_xor()) return testfailed("float_xor");
  if(!test_shl()) return testfailed("float_shl");
  if(!test_shr()) return testfailed("float_shr");
  if(!test_raisei()) return testfailed("_raisei");

  if(!test_raiseposi()) return testfailed("_raiseposi");
  if(!test_coshminus1near0()) return testfailed("coshminus1near0");
  if(!test_artanhnear0()) return testfailed("artanhnear0");
  if(!test_cosminus1near0()) return testfailed("cosminus1near0");
  if(!test_arctannear0()) return testfailed("arctannear0");
  if(!test_lnxplus1near0()) return testfailed("_lnxplus1near0");
  if(!test_lnreduce()) return testfailed("_lnreduce");
  if(!test_lnxplus1lt1()) return testfailed("_lnxplus1lt1");
  if(!test_ln()) return testfailed("_ln");
  if(!test_lnxplus1()) return testfailed("_lnxplus1");
  if(!test_artanh1minusx()) return testfailed("_artanh1minusx");
  if(!test_artanhlt0_5()) return testfailed("_artanhlt0_5");
  if(!test_arsinh()) return testfailed("_arsinh");
  if(!test_arcoshxplus1()) return testfailed("_arcoshxplus1");
  if(!test_coshminus1lt1()) return testfailed("_coshminus1lt1");
  if(!test_sinhlt1()) return testfailed("_sinhlt1");
  if(!test_expminus1lt1()) return testfailed("_expminus1lt1");
  if(!test_expltln10()) return testfailed("_expltln10");
  if(!test_exp()) return testfailed("_exp");
  if(!test_expminus1()) return testfailed("_expminus1");
  if(!test_coshminus1()) return testfailed("_coshminus1");
  if(!test_tanhlt0_5()) return testfailed("_tanhlt0_5");
  if(!test_tanhminus1gt0()) return testfailed("_tanhminus1gt0");
  if(!test_arctanlt1()) return testfailed("_arctanlt1");
  if(!test_arctan()) return testfailed("_arctan");
  if(!test_arccosxplus1lt0_5()) return testfailed("_arccosxplus1lt0_5");
  if(!test_arcsinlt0_5()) return testfailed("_arcsinlt0_5");
  if(!test_arccos()) return testfailed("_arccos");
  if(!test_arcsin()) return testfailed("_arcsin");
  if(!test_cosminus1ltPiDiv4()) return testfailed("_cosminus1ltPiDiv4");
  if(!test_sinltPiDiv4()) return testfailed("_sinltPiDiv4");
  if(!test_tanltPiDiv4()) return testfailed("_tanltPiDiv4");
  if(!test_cos()) return testfailed("_cos");
  if(!test_sin()) return testfailed("_sin");
  if(!test_tan()) return testfailed("_tan");
  if(!test_lngammaseries()) return testfailed("lngammaseries");
  if(!test_pochhammer()) return testfailed("_pochhammer");
  if(!test_lngamma()) return testfailed("_lngamma");
  if(!test_gamma()) return testfailed("_gamma");
  if(!test_gammaint()) return testfailed("_gammaint");
  if(!test_gamma0_5()) return testfailed("_gamma0_5");

  printf("\nall tests PASSED\n");
#endif /* _FLOATNUMTEST */
  return 0;
};
