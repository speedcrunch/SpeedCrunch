/* HMath: C++ high precision math routines
   Copyright (C) 2004 Ariya Hidayat <ariya.hidayat@gmail.com>
                 2007 Helder Correia <helder.pereira.correia@gmail.com>
   Last update: June 11, 2007

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

#include "hmath.h"
#include "number.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sstream>
#include <iostream>

// internal number of decimal digits
#define HMATH_MAX_PREC 150

// digits used for number comparison
// (not all are used, to work around propagated error problem)
#define HMATH_COMPARE_PREC 70

// maximum shown digits if prec is negative
#define HMATH_MAX_SHOWN 20

// from number.c, need to be freed somehow
extern bc_num _zero_;
extern bc_num _one_;
extern bc_num _two_;

class HNumberPrivate
{
public:
  bc_num num;
  bool nan;
  char format;
};

void out_of_memory(void){
  return;
}

void rt_warn(char * ,...){
  return;
}

void rt_error(char * ,...){
  return;
}

static bc_num h_create( int len = 1, int scale = 0 )
{
  bc_num temp;
  temp = (bc_num) malloc( sizeof(bc_struct) );
  temp->n_sign = PLUS;
  temp->n_len = len;
  temp->n_scale = scale;
  temp->n_refs = 1;
  temp->n_ptr = (char*) malloc( len+scale+1 );
  temp->n_value = temp->n_ptr;
  temp->n_next = 0;
  memset (temp->n_ptr, 0, len+scale+1);
  return temp;
}

static void h_destroy( bc_num n )
{
  free( n->n_ptr );
  free( n );
}

// reclaim and free one bc_num from the freelist
// workaround for number.c, because it doesn't really free a number
// but instead put it in the pool of unused numbers
// this function will take it back from that pool and set it really free
static void h_grabfree()
{
  bc_num t = bc_new_num( 1, 0 );
  h_destroy( t );
}

// make an exact (explicit) copy
static bc_num h_copy( bc_num n )
{
  int len = n->n_len;
  int scale = n->n_scale;
  bc_num result = h_create( len, scale );
  memcpy( result->n_ptr, n->n_value, len+scale+1 );
  result->n_sign = n->n_sign;
  result->n_value = result->n_ptr;
  h_grabfree();
  return result;
}

// same as copy, but readjust decimal digits
static bc_num h_rescale( bc_num n, int sc )
{
  int len = n->n_len;
  int scale = MIN( sc, n->n_scale );
  bc_num result = h_create( len, scale );
  memcpy( result->n_ptr, n->n_value, len+scale+1 );
  result->n_sign = n->n_sign;
  result->n_value = result->n_ptr;
  h_grabfree();
  return result;
}

// add two numbers, return newly allocated number
static bc_num h_add( bc_num n1, bc_num n2 )
{
  bc_num r = h_create();
  bc_add( n1, n2, &r, 1 );
  h_grabfree();
  return r;
}

// multiply two numbers, return newly allocated number
static bc_num h_mul( bc_num n1, bc_num n2 )
{
  bc_num r = h_create();
  bc_multiply( n1, n2, &r, HMATH_MAX_PREC );
  h_grabfree();
  return r;
}

enum Base {Decimal, Hexadec, Octal, Binary};

static bool isValidDigit(const char c, const Base b)
{
   switch (b)
   {
   case Hexadec:
      return ((c >= '0' && c <= '9') || (c >= 'A' && c < 'G') || (c >= 'a' && c < 'g'));
   case Octal:
      return (c >= '0' && c < '8');
   case Binary:
      return ( c == '0' || c == '1' );
   case Decimal:
   default:
      break;
   }
   return (c >= '0' && c <= '9');
}

// convert simple string to number
static bc_num h_str2num( const char* str, int scale = HMATH_MAX_PREC )
{
  int digits, strscale;
  const char *ptr;
  char *nptr;
  bool zero_int;

  /* Check for valid number and count digits. */
  ptr = str;
  digits = 0;
  strscale = 0;
  zero_int = false;
  Base base = Decimal;
  if (*ptr == '+' || *ptr == '-') ptr++;  /* Sign */
   if ( *ptr == '0' ) //leadeing 0, could be hexadec etc.
  {
     ptr++;
     if (*ptr == 'x') { base = Hexadec; ptr++; }
     else if (*ptr == 'o') { base = Octal; ptr++; }
     else if (*ptr == 'b') { base = Binary; ptr++; }
     else if (*ptr == 'd') ptr++;
  }
  while (*ptr == '0') ptr++;			/* Skip leading zeros. */
  while (isValidDigit((int)*ptr, base)) ptr++, digits++;	/* digits, maybe non decimal base */
  if (*ptr == '.') { if (base == Decimal) ptr++; else return h_create(); } /* decimal point */
  while (isdigit((int)*ptr)) ptr++, strscale++;	/* digits, must be decimal, we had a period ; */
  if ((*ptr != '\0') || (digits+strscale == 0))
    return h_create();

   bc_num num;

   switch (base)
   {
   case Hexadec:
   {
      int chr;
      bc_num _16 = h_create(); bc_int2num( &_16, 16 );
      bc_num factor = h_create(); bc_int2num( &factor, 1 );
      bc_num tmp1, tmp2;
      num = h_create(); bc_int2num( &num, 0 );
      for (;digits > 0; --digits) // n = n+f*x;
      {
         --ptr;
         chr = CH_HEX(*ptr);
         if (chr) // skip increment if digit is '0'
         {
            tmp1 = h_create();
            bc_int2num( &tmp1, chr ); // x
            tmp2 = h_mul( factor, tmp1 ); // f*x
            h_destroy( tmp1 );
            tmp1 = h_add( num, tmp2 ); // n+f*x;
            h_destroy( num ); h_destroy( tmp2 );
            num = h_copy( tmp1 ); // n = n+f*x;
            h_destroy( tmp1 );
         }
         tmp1 = h_mul( factor, _16 ); // f*16
         h_destroy( factor );
         factor = h_copy( tmp1 ); // f = f*16;
         h_destroy( tmp1 );
      }
      if (*str == '-') num->n_sign = MINUS; else num->n_sign = PLUS;
      break;
   }
   case Octal:
   {
      int chr;
      bc_num _8 = h_create(); bc_int2num( &_8, 8 );
      bc_num factor = h_create(); bc_int2num( &factor, 1 );
      bc_num tmp1, tmp2;
      num = h_create(); bc_int2num( &num, 0 );
      for (;digits > 0; --digits) // n = n+f*x;
      {
         --ptr;
         chr = CH_VAL(*ptr);
         if (chr) // skip increment if digit is '0'
         {
            tmp1 = h_create();
            bc_int2num( &tmp1, chr ); // x
            tmp2 = h_mul( factor, tmp1 ); // f*x
            h_destroy( tmp1 );
            tmp1 = h_add( num, tmp2 ); // n+f*x;
            h_destroy( num ); h_destroy( tmp2 );
            num = h_copy( tmp1 ); // n = n+f*x;
            h_destroy( tmp1 );
         }
         tmp1 = h_mul( factor, _8 ); // f*8
         h_destroy( factor );
         factor = h_copy( tmp1 ); // f = f*8;
         h_destroy( tmp1 );
      }
      if (*str == '-') num->n_sign = MINUS; else num->n_sign = PLUS;
      break;
   }
   case Binary:
   {
      bc_num _2 = h_create(); bc_int2num( &_2, 2 );
      bc_num factor = h_create(); bc_int2num( &factor, 1 );
      bc_num tmp;
      num = h_create(); bc_int2num( &num, 0 );
      for (;digits > 0; --digits) // n = n+f;
      {
         --ptr;
         if (CH_VAL(*ptr)) // only increment if bit is set
         {
            tmp = h_add( num, factor ); // n+f;
            h_destroy( num ); num = h_copy( tmp ); // n = n+f;
            h_destroy( tmp );
         }
         tmp = h_mul( factor, _2 ); // f*2
         factor = h_copy( tmp ); // f = f*2;
         h_destroy( tmp );
      }
      if (*str == '-') num->n_sign = MINUS; else num->n_sign = PLUS;
      break;
   }
   case Decimal:
   default:
   {
      /* Adjust numbers and allocate storage and initialize fields. */
      strscale = MIN(strscale, scale);
      if (digits == 0)
      {
         zero_int = true;
         digits = 1;
      }

      num = h_create( digits, strscale );

      ptr = str;
      if (*ptr == '-')
      {
         num->n_sign = MINUS;
         ptr++;
      }
      else
      {
         num->n_sign = PLUS;
         if (*ptr == '+') ptr++;
      }
      while (*ptr == '0') ptr++;
      nptr = num->n_value;
      if (zero_int)
      {
         *nptr++ = 0;
         digits = 0;
      }
      for (;digits > 0; digits--)
         *nptr++ = (char)CH_VAL(*ptr++);


      if (strscale > 0)
      {
         ptr++;
         for (;strscale > 0; strscale--)
      *nptr++ = (char)CH_VAL(*ptr++);
      }
   }
   }
   return num;
}


// subtract two numbers, return newly allocated number
static bc_num h_sub( bc_num n1, bc_num n2 )
{
  bc_num r = h_create();
  bc_sub( n1, n2, &r, 1 );
  h_grabfree();
  return r;
}

// divide two numbers, return newly allocated number
static bc_num h_div( bc_num n1, bc_num n2 )
{
  bc_num r = h_create();
  bc_divide( n1, n2, &r, HMATH_MAX_PREC );
  h_grabfree();
  return r;
}

// find 10 raise to num
// e.g.: when num is 5, it results 100000
static bc_num h_raise10( int n )
{
  // calculate proper factor
  int len = abs(n)+2;
  char* sf = new char[len+1];
  sf[len] = '\0';
  if( n >= 0 )
  {
    sf[0] = '1';
    sf[len-1] = '\0';
    sf[len-2] = '\0';
    for( int i = 0; i < n; i++ )
      sf[i+1] = '0';
  }
  else
  {
    sf[0] = '0'; sf[1] = '.';
    for( int i = 0; i < -n; i++ )
      sf[i+2] = '0';
    sf[len-1] = '1';
  }

  bc_num factor = h_str2num( sf, abs(n) );
  delete[] sf;

  return factor;
}

// round up to certain decimal digits
static bc_num h_round( bc_num n, int prec )
{
  // no need to round?
  if( prec >= n->n_scale )
    return h_copy( n );

  // example: rounding "3.14159" to 4 decimal digits means
  //   adding 0.5e-4 to 3.14159, it becomes 3.14164
  //   taking only 4 decimal digits, so it's now 3.1416
  if( prec < 0 ) prec = 0;
  bc_num x = h_raise10( -prec-1 );
  bc_num y = 0;
  bc_int2num( &y, 5 );
  bc_num z = h_mul( x, y );
  z->n_sign = n->n_sign;
  bc_num r = h_add( n, z );
  h_destroy( x );
  h_destroy( y );
  h_destroy( z );

  // only digits we are interested in
  bc_num v = h_rescale( r, prec );
  h_destroy( r );

  return v;
}

// trunc up to certain decimal digits
static bc_num h_trunc( bc_num n, int prec )
{
  // no need to truncate?
  if( prec >= n->n_scale )
    return h_copy( n );

  if( prec < 0 )
    prec = 0;

  // only digits we are interested in
  bc_num v = h_rescale( n, prec );

  return v;
}

// remove trailing zeros
static void h_trimzeros( bc_num num )
{
  while( ( num->n_scale > 0 ) && ( num->n_len+num->n_scale > 0 ) )
    if( num->n_value[num->n_len+num->n_scale-1] == 0 )
      num->n_scale--;
    else break;
}

static void h_init()
{
  static bool h_initialized = false;
  if( !h_initialized )
  {
    h_initialized = true;
    bc_init_numbers();
  }
}


HNumber::HNumber(): d(0)
{
  h_init();
  d = new HNumberPrivate;
  d->nan = false;
  d->format = 0;
  d->num = h_create();
}

HNumber::HNumber( const HNumber& hn ): d(0)
{
  h_init();
  d = new HNumberPrivate;
  d->nan = false;
  d->num = h_create();
  operator=( hn );
}

HNumber::HNumber( int i ): d(0)
{
  h_init();
  d = new HNumberPrivate;
  d->nan = false;
  d->num = h_create();
  d->format = 0;
  bc_int2num( &d->num, i );
}

HNumber::HNumber( const char* str ): d(0)
{
  h_init();
  d = new HNumberPrivate;
  d->nan = false;
  d->num = h_create();
  d->format = 0;

  if( str )
  if( strlen(str) == 3 )
  if( tolower(str[0])=='n' )
  if( tolower(str[1])=='a' )
  if( tolower(str[2])=='n' )
    d->nan = true;

  if( str && !d->nan )
  {
    char* s = new char[ strlen(str)+1 ];
    strcpy( s, str );

    bool isHex = false, isDecimal = true;
    char* p = s;
    for( ;; p++ )
    {
      if (*p == 'x' && *(p-1) == '0')
      {
         isHex = true;
         isDecimal = false;
         continue;
      }
      if ((*p == 'b' || *p == 'o') && *(p-1) == '0')
      {
         isDecimal = false;
         continue;
      }
      if( *p != '+' )
      if( *p != '-' )
      if( *p != '.' )
      if( !isdigit(*p) )
      if(!(isHex && *p >= 'a' && *p < 'g'))
        break;
     }

     int expd = 0;
     if (isDecimal)
     {
         if( ( *p == 'e' ) || ( *p == 'E' ) )
         {
            *p = '\0';
            expd = atoi( p+1 );
         }
     }

    h_destroy( d->num );
    d->num = h_str2num( s );
    delete [] s;
    if (isDecimal)
    {
      if( expd >= HMATH_MAX_PREC ) // too large
      {
         d->nan = true;
      }
      else
      if( expd <= -HMATH_MAX_PREC ) // too small
      {
         d->nan = true;
      }
      else
      if( expd != 0 )
      {
         bc_num factor = h_raise10( expd );
         bc_num nn = h_copy( d->num );
         h_destroy( d->num );
         d->num = h_mul( nn, factor );
         h_destroy( nn );
         h_destroy( factor );
      }
    }
    h_trimzeros( d->num );
  }

}

HNumber::~HNumber()
{
  h_destroy( d->num );
  delete d;
}

bool HNumber::isNan() const
{
  return d->nan;
}

bool HNumber::isZero() const
{
  return !d->nan && ( bc_is_zero( d->num )!=0 );
}

bool HNumber::isPositive() const
{
  return !d->nan && !isNegative() && !isZero();
}

bool HNumber::isNegative() const
{
  return !d->nan && ( bc_is_neg( d->num )!=0 );
}

bool HNumber::isInteger() const
{
  return *this == HMath::integer( *this );
}

char HNumber::format() const
{
   return d->format;
}

void HNumber::setFormat(char c) const
{
   d->format = d->nan?0:c;
}

HNumber HNumber::nan()
{
  HNumber n;
  n.d->nan = true;
  return n;
}

int HNumber::toInt()
{
  char* str = HMath::formatFixed( *this );
  std::string s( str );
  delete[] str;
  std::istringstream iss( s );
  int i;
  iss >> i;

  return i;
}

HNumber& HNumber::operator=( const HNumber& hn )
{
  d->nan = hn.d->nan;
  h_destroy( d->num );
  d->num = h_copy( hn.d->num );
  d->format = hn.format();
  return *this;
}

HNumber HNumber::operator+( const HNumber& num ) const
{
  if( isNan() ) return HNumber( *this );
  if( num.isNan() ) return HNumber( num );

  HNumber result;
  h_destroy( result.d->num );
  result.d->num = h_add( d->num, num.d->num );

  return result;
}

HNumber& HNumber::operator+=( const HNumber& num )
{
  HNumber n = HNumber(*this) + num;
  operator=( n );
  return *this;
}

HNumber HNumber::operator-( const HNumber& num ) const
{
  if( isNan() ) return HNumber( *this );
  if( num.isNan() ) return HNumber( num );

  HNumber result;
  h_destroy( result.d->num );
  result.d->num = h_sub( d->num, num.d->num );

  return result;
}

HNumber& HNumber::operator-=( const HNumber& num )
{
  HNumber n = HNumber(*this) - num;
  operator=( n );
  return *this;
}

HNumber HNumber::operator*( const HNumber& num ) const
{
  if( isNan() ) return HNumber( *this );
  if( num.isNan() ) return HNumber( num );

  HNumber result;
  h_destroy( result.d->num );
  result.d->num = h_mul( d->num, num.d->num );
  return result;
}

HNumber& HNumber::operator*=( const HNumber& num )
{
  HNumber n = HNumber(*this) * num;
  operator=( n );
  return *this;
}

HNumber HNumber::operator/( const HNumber& num ) const
{
  if( isNan() ) return HNumber( *this );
  if( num.isNan() ) return HNumber( num );

  HNumber result;
  h_destroy( result.d->num );
  result.d->num = h_div( d->num, num.d->num );
  return result;
}

HNumber& HNumber::operator/=( const HNumber& num )
{
  HNumber n = HNumber(*this) / num;
  operator=( n );
  return *this;
}

HNumber HNumber::operator%( const HNumber& num ) const
{
  if( isNan() ) return HNumber( *this );
  if( num.isNan() ) return HNumber( num );

  HNumber result;
  bc_modulo (d->num, num.d->num, &result.d->num, 0);
  return result;
}

bool HNumber::operator>( const HNumber& n ) const
{
  return HMath::compare( *this, n ) > 0;
}

bool HNumber::operator<( const HNumber& n ) const
{
  return HMath::compare( *this, n ) < 0;
}

bool HNumber::operator>=( const HNumber& n ) const
{
  return HMath::compare( *this, n ) >= 0;
}

bool HNumber::operator<=( const HNumber& n ) const
{
  return HMath::compare( *this, n ) <= 0;
}

bool HNumber::operator==( const HNumber& n ) const
{
  return HMath::compare( *this, n ) == 0;
}

bool HNumber::operator!=( const HNumber& n ) const
{
  return HMath::compare( *this, n ) != 0;
}

// format number in engineering notation
char* HMath::formatEngineering( const HNumber& hn, int prec )
{
  if( hn.isNan() )
  {
    char* str = (char*)malloc( 4 );
    str[0] = 'N';
    str[1] = 'a';
    str[2] = 'N';
    str[3] = '\0';
    return str;
  }

  int    nIntDigs  = hn.d->num->n_len;
  int    nFracDigs = hn.d->num->n_scale;
  char * digs      = hn.d->num->n_value;
  int    tenExp;

  // find the exponent and the factor
  int tzeros = 0;
  for( int c=0; c<hn.d->num->n_len+hn.d->num->n_scale; c++, tzeros++ )
    if( hn.d->num->n_value[c]!= 0 ) break;
  int expd = hn.d->num->n_len - tzeros - 1;

  if ( hn >= 1 || hn <= -1 )
  {
    // point must be shifted to the left, if needed

    if ( nIntDigs % 3 == 0 )
    {
      // 3n digits to the left
      if ( hn > 999 || hn < -999 )
        // n > 1
        tenExp = nIntDigs - 3;
      else
        // n = 1
        tenExp = 0;
    }
    else
      // non-3n digits to the left
      tenExp = nIntDigs - nIntDigs % 3;
  }
  else
  {
    // point must be shifted to the right
    // find first non-zero digit to the right of the point
    int startDigIdx = nIntDigs;
    int endDigIdx   = nIntDigs + nFracDigs - 1;
    int idx;
    for ( idx = startDigIdx; idx <= endDigIdx; idx++ )
      if ( digs[idx] != 0 )
        break;
    // calculate exponent to shift right
    while ( idx % 3 != 0 )
      idx++;
    tenExp = -idx;
  }

  // scale the number by a new factor
  HNumber nn = hn * HMath::raise( 10, -tenExp );

  // too close to zero?
  if( hn.isZero() || ( expd <= -HMATH_COMPARE_PREC ) )
  {
    nn = HNumber( 0 );
    tenExp = 0;
  }

  // build result expression string with E notation
  char* str = formatFixed( nn, prec );
  std::string resString = std::string( str ) + "e";
  free( str );
  std::stringstream ss;
  ss << tenExp;
  resString += ss.str();
  char * result = (char *) malloc( resString.size() + 1 );
  strcpy( result, resString.c_str() );

  return result;
}

// format number with fixed number of decimal digits
char* HMath::formatFixed( const HNumber& hn, int prec )
{
  if( hn.isNan() )
  {
    char* str = (char*)malloc( 4 );
    str[0] = 'N';
    str[1] = 'a';
    str[2] = 'N';
    str[3] = '\0';
    return str;
  }

  bc_num n = h_copy( hn.d->num );
  h_trimzeros( n );

  int oprec = prec;
  if( prec < 0 )
  {
    prec = HMATH_MAX_SHOWN;
    if( n->n_scale < HMATH_MAX_SHOWN )
      prec = n->n_scale;
  }

  // yes, this is necessary!
  bc_num m = h_round( n, prec );
  h_trimzeros( m );
  h_destroy( n );
  n = m;
  if( oprec < 0 )
  {
    prec = HMATH_MAX_SHOWN;
    if( n->n_scale < HMATH_MAX_SHOWN )
      prec = n->n_scale;
  }

  // how many to allocate?
  int len = n->n_len + prec;
  if( n->n_sign != PLUS ) len++;
  if( prec > 0 ) len++;

  char* str = (char*)malloc( len+1 );
  char* p = str;

  // the sign and the integer part
  // but avoid printing "-0"
  if( n->n_sign != PLUS )
    if( !bc_is_zero( n ) ) *p++ = '-';
  for( int c=0; c<n->n_len; c++ )
    *p++ = (char)BCD_CHAR( n->n_value[c] );

  // the fraction part
  if( prec > 0 )
  {
    *p++ = '.';
    int k = (prec < n->n_scale) ? prec : n->n_scale;
    for( int d=0; d<k; d++ )
      *p++ = (char)BCD_CHAR( n->n_value[n->n_len+d] );
    for( int r=n->n_scale; r<prec; r++ )
      *p++ = '0';
  }

  *p = '\0';
  h_destroy( n );

  return str;
}

// format number with exponential
char* HMath::formatExp( const HNumber& hn, int prec )
{
  if( hn.isNan() )
  {
    char* str = (char*)malloc( 4 );
    str[0] = 'N';
    str[1] = 'a';
    str[2] = 'N';
    str[3] = '\0';
    return str;
  }

  // find the exponent and the factor
  int tzeros = 0;
  for( int c=0; c<hn.d->num->n_len+hn.d->num->n_scale; c++, tzeros++ )
    if( hn.d->num->n_value[c]!= 0 ) break;
  int expd = hn.d->num->n_len - tzeros - 1;

  // extra digits needed for the exponent part
  int expn = 0;
  for( int e = ::abs(expd); e > 0; e/=10 ) expn++;
  if( expd <= 0 ) expn++;

  // scale the number by a new factor
  HNumber nn = hn * HMath::raise( 10, -expd );

  // too close to zero?
  if( hn.isZero() || ( expd <= -HMATH_COMPARE_PREC ) )
  {
    nn = HNumber(0);
    expd = 0;
    expn = 1;
  }

  char* str = formatFixed( nn, prec );
  char* result = (char*) malloc( strlen(str)+expn+2 );
  strcpy( result, str );
  free( str );

  // the exponential part
  char* p = result + strlen(result);
  *p++ = 'e'; p[expn] = '\0';
  if( expd < 0 ) *p = '-';
  for( int k=expn; k>0; k-- )
  {
    int digit = expd % 10;
    p[k-1] = (char)('0' + ::abs( digit ));
    expd = expd / 10;
    if( expd == 0 ) break;
  }

  return result;
}

char* HMath::formatGeneral( const HNumber& hn, int prec )
{
  if( hn.isNan() )
  {
    char* str = (char*)malloc( 4 );
    str[0] = 'N';
    str[1] = 'a';
    str[2] = 'N';
    str[3] = '\0';
    return str;
  }

  // find the exponent and the factor
  int tzeros = 0;
  for( int c=0; c<hn.d->num->n_len+hn.d->num->n_scale; c++, tzeros++ )
    if( hn.d->num->n_value[c]!= 0 ) break;
  int expd = hn.d->num->n_len - tzeros - 1;

  char* str;
  if( expd > 5 )
    str = formatExp( hn, prec );
  else if( ( expd < -4 ) && (expd>-HMATH_COMPARE_PREC ) )
    str = formatExp( hn, prec );
  else if ( (expd < 0) && (prec>0) && (expd < -prec) )
    str = formatExp( hn, prec );
  else
    str = formatFixed( hn, prec );

  return str;
}

char* HMath::formatHexadec( const HNumber& hn )
{
  char* str;
  if( hn.isNan() || !hn.isInteger())
  {
    str = (char*)malloc( 4 );
    str[0] = 'N';
    str[1] = 'a';
    str[2] = 'N';
    str[3] = '\0';
    return str;
  }

  int digits = 1; HNumber _16(16);
  bool negative = (hn < 0);
  HNumber tmp = negative ? HNumber(0)-hn : hn;
  while (integer(tmp/=_16) > 0) ++digits; // how many digits
  str = (char*)malloc( digits+3+negative );
  char* ptr = &str[digits+2+negative]; *ptr = '\0';
  HNumber val;
  int i;
  tmp = negative ? HNumber(0)-hn : hn;
  HNumber f;
  HNumber f_old(1);
  while (digits--)
  {
     f = f_old*_16;
     val = tmp % (f); tmp -= val;
     val /= f_old; f_old = f; i = bc_num2long( val.d->num);
     *--ptr = (i < 10) ? '0'+i : 'A'+i-10;
  }
  *--ptr = 'x'; *--ptr = '0'; if (negative) *--ptr = '-';

  return str;
}

char* HMath::formatOctal( const HNumber& hn )
{
  char* str;
  if( hn.isNan() || !hn.isInteger())
  {
    str = (char*)malloc( 4 );
    str[0] = 'N';
    str[1] = 'a';
    str[2] = 'N';
    str[3] = '\0';
    return str;
  }

  int digits = 1; HNumber _8(8);
  bool negative = (hn < 0);
  HNumber tmp = negative ? HNumber(0)-hn : hn;
  while (integer(tmp/=_8) > 0) ++digits; // how many digits
  str = (char*)malloc( digits+3+negative );
  char* ptr = &str[digits+2+negative]; *ptr = '\0';
  HNumber val;
  tmp = negative ? HNumber(0)-hn : hn;
  HNumber f;
  HNumber f_old(1);
  while (digits--)
  {
     f = f_old*_8;
     val = tmp % (f); tmp -= val;
     val /= f_old; f_old = f;
     *--ptr = '0'+bc_num2long( val.d->num);
  }
  *--ptr = 'o'; *--ptr = '0'; if (negative) *--ptr = '-';

  return str;
}

char* HMath::formatBinary( const HNumber& hn )
{
  char* str;
  if( hn.isNan() || !hn.isInteger())
  {
    str = (char*)malloc( 4 );
    str[0] = 'N';
    str[1] = 'a';
    str[2] = 'N';
    str[3] = '\0';
    return str;
  }

  int digits = 1; HNumber _2(2);
  bool negative = (hn < 0);
  HNumber tmp = negative ? HNumber(0)-hn : hn;
  while (integer(tmp/=_2) > 0) ++digits; // how many digits
  str = (char*)malloc( digits+3+negative );
  char* ptr = &str[digits+2+negative]; *ptr = '\0';
  HNumber val;
  HNumber _0(0);
  tmp = negative ? _0-hn : hn;
  HNumber f;
  HNumber f_old(1);
  while (digits--)
  {
     f = f_old*_2;
     val = tmp % (f); tmp -= val;
     val /= f_old; f_old = f;
     *--ptr = (val == _0) ?'0':'1';
  }
  *--ptr = 'b'; *--ptr = '0'; if (negative) *--ptr = '-';

  return str;
}

char* HMath::format( const HNumber& hn, char format, int prec )
{
  if( hn.isNan() )
  {
    char* str = (char*)malloc( 4 );
    str[0] = 'N';
    str[1] = 'a';
    str[2] = 'N';
    str[3] = '\0';
    return str;
  }

  if     ( format=='g' ) return formatGeneral    ( hn, prec );
  else if( format=='f' ) return formatFixed      ( hn, prec );
  else if( format=='n' ) return formatEngineering( hn, prec );
  else if( format=='e' ) return formatExp        ( hn, prec );
  else if( format=='h' ) return formatHexadec    ( hn       );
  else if( format=='o' ) return formatOctal      ( hn       );
  else if( format=='b' ) return formatBinary     ( hn       );

  // fallback to 'g'
  return formatGeneral( hn, prec );
}

HNumber HMath::pi()
{
  return HNumber("3.14159265358979323846264338327950288419716939937510"
                   "58209749445923078164062862089986280348253421170679"
                   "82148086513282306647093844609550582231725359408128"
                   "48111745028410270193852110555964462294895493038196"
                   "44288109756659334461284756482337867831652712019091"
                   "45648566923460348610454326648213393607260249141273"
                   "72458700660631558817488152092096282925409171536436"
                   "78925903600113305305488204665213841469519415116094"
                   "33057270365759591953092186117381932611793105118548"
                   "07446237996274956735188575272489122793818301194912"
                   "98336733624406566430860213949463952247371907021798"
                   "60943702770539217176293176752384674818467669405132"
                   "00056812714526356082778577134275778960917363717872"
                   "14684409012249534301465495853710507922796892589235"
                   "42019956112129021960864034418159813629774771309960"
                   "51870721134999999837297804995105973173281609631859"
                   "50244594553469083026425223082533446850352619311881"
                   "71010003137838752886587533208381420617177669147303"
                   "59825349042875546873115956286388235378759375195778"
                   "1857780532171226806613001927876611195909216420198" );
}

HNumber HMath::add( const HNumber& n1, const HNumber& n2 )
{
  HNumber result = n1 + n2;
  return result;
}

HNumber HMath::sub( const HNumber& n1, const HNumber& n2 )
{
  HNumber result = n1 - n2;
  return result;
}

HNumber HMath::mul( const HNumber& n1, const HNumber& n2 )
{
  HNumber result = n1 * n2;
  return result;
}

HNumber HMath::div( const HNumber& n1, const HNumber& n2 )
{
  HNumber result = n1 / n2;
  return result;
}

int HMath::compare( const HNumber& n1, const HNumber& n2 )
{
  if( n1.isNan() && n2.isNan() ) return 0;

  HNumber delta = sub( n1, n2 );
  delta = HMath::round( delta, HMATH_COMPARE_PREC );
  if( delta.isZero() ) return 0;
  else if( delta.isNegative() ) return -1;
  return  1;
}

HNumber HMath::abs( const HNumber& n )
{
  HNumber r( n );
  r.d->num->n_sign = PLUS;
  return r;
}

HNumber HMath::negate( const HNumber& n )
{
  if( n.isNan() || n.isZero() )
    return HNumber( n );

  HNumber result( n );
  result.d->num->n_sign = ( n.d->num->n_sign == PLUS ) ? MINUS : PLUS;
  return result;
}

HNumber HMath::round( const HNumber& n, int prec )
{
  if( n.isNan() )
    return HNumber::nan();

  HNumber result;
  h_destroy( result.d->num );
  result.d->num = h_round( n.d->num, prec );
  return result;
}

HNumber HMath::trunc( const HNumber& n, int prec )
{
  if( n.isNan() )
    return HNumber::nan();

  HNumber result;
  h_destroy( result.d->num );
  result.d->num = h_trunc( n.d->num, prec );
  return result;
}

HNumber HMath::integer( const HNumber& n )
{
  if( n.isNan() )
    return HNumber::nan();

  if( n.isZero() )
    return HNumber( 0 );

  HNumber result;
  h_destroy( result.d->num );
  result.d->num = h_rescale( n.d->num, 0 );
  return result;
}

HNumber HMath::frac( const HNumber& n )
{
  if( n.isNan() )
    return HNumber::nan();

  return n - integer(n);
}

HNumber HMath::floor( const HNumber& n )
{
  if( n.isNan() )
    return HNumber::nan();

  if( n.isZero() )
    return HNumber(0);

  if( n.isPositive() )
    return n - frac(n);
  else
    return n - HNumber(1) - frac(n);
}

HNumber HMath::ceil( const HNumber& n )
{
  if( n.isNan() )
    return HNumber::nan();

  if( n.isZero() )
    return HNumber(0);

  if( n.isPositive() )
    return n + HNumber(1) - frac(n);
  else
    return n - frac(n);
}

HNumber HMath::gcd( const HNumber& n1, const HNumber& n2 )
{
  if( n1.isNan() || n2.isNan() )
    return HNumber::nan();

  HNumber a = abs( n1 );
  HNumber b = abs( n2 );

  if ( a == 0 )
    return b;
  if ( b == 0 )
    return a;

  // run Euclidean algorithm
  while ( true )
  {
    a = a % b;

    if ( a == 0 )
      return b;

    b = b % a;

    if ( b == 0 )
      return a;
  }
}

HNumber HMath::sqrt( const HNumber& n )
{
  if( n.isNan() )
    return HNumber::nan();

  if( n.isZero() )
    return n;

  if( n.isNegative() )
    return HNumber::nan();

  // useful constant
  HNumber half("0.5");

  // Use Netwon-Raphson algorithm
  HNumber r( 1 );
  for( int i = 0; i < HMATH_MAX_PREC; i++ )
  {
    HNumber q = n / r;
    if( r == q ) break;
    HNumber s = r + q;
    r = s * half;
  }

  return r;
}

HNumber HMath::cbrt( const HNumber& n )
{
  if( n.isNan() )
    return HNumber::nan();

  if( n.isZero() )
    return n;

  // useful constants
  HNumber three     = HNumber("3");
  HNumber twoThirds = HNumber("2") / three;

  // iterations to approximate result
  // X[i+1] = (2/3)X[i] + n / (3 * X[i]^2))
  // initial guess = sqrt( n )
  // r = X[i], q = X[i+1], a = n
  HNumber a = n.isNegative() ? n * HNumber("-1") : n;
  HNumber r = sqrt( a );
  for( int i = 0; i < HMATH_MAX_PREC; i++ )
  {
    HNumber q = (twoThirds * r) + (a / (three * r * r));
    if( r == q )
      break;
    r = q;
  }

  if( n.isNegative())
    return r * HNumber("-1");
  else
    return r;
}

HNumber HMath::raise( const HNumber& n1, int n )
{
  if( n1.isNan() ) return n1;

  // http://en.wikipedia.org/wiki/Exponentiation#Powers_of_zero
  if( n1.isZero() )
  {
    if( n < 0 )
      return HNumber::nan();
    if( n > 0 )
      return HNumber(0);

    // debatable, see http://en.wikipedia.org/wiki/Empty_product#0_raised_to_the_0th_power
    // vs http://mathworld.wolfram.com/Power.html
    if( n == 0 )
      return HNumber(1);
  }

  if( n1 == HNumber(1) ) return n1;
  if( n == 0 ) return HNumber(1);
  if( n == 1 ) return n1;

  if(n > 0)
  {
    // squaring algorithm to find exponentiation
    // see http://en.wikipedia.org/wiki/Exponentiation_by_squaring
    HNumber result = HNumber(1);
    HNumber x = n1;
    while(n > 0)
    {
      if(n & 1)
      {
        result = result * x;
        n--;
      }
      x = x * x;
      n = n >> 1;
    }
    return result;
  }

  HNumber result = n1;
  for( ; n < 1; n++ )
    result /= n1;

  return result;
}

HNumber HMath::raise( const HNumber& n1, const HNumber& n2  )
{
  if( n1.isNan() ) return HNumber::nan();
  if( n2.isNan() ) return HNumber::nan();

  // see previous function
  if( n1.isZero() )
  {
    if( n2.isNegative() )
      return HNumber::nan();
    if( n2.isPositive() )
      return HNumber(0);
    if( n2.isZero() )
      return HNumber(1);
  }

  if( n1 == HNumber(1) ) return n1;
  if( n2.isZero() ) return HNumber(1);
  if( n2 == HNumber(1) ) return n1;

  HNumber result;

  // n1 is negative, n2 must be integer
  if( n1.isNegative() )
  {
    if( HMath::integer(n2) != n2)
      result = HNumber::nan();
    else
    {
      // use integer raise function
      HNumber nn = n2;
      result = raise( n1, atoi( HMath::formatFixed(nn, 0) ) );
    }
  }
  else
  {
    // n2 integer? use the integer raise version
    if( HMath::integer(n2) == n2)
    {
      // use integer raise function
      HNumber nn = n2;
      result = raise( n1, atoi( HMath::formatFixed(nn, 0) ) );
    }
    else
    {
      // x^y = exp( y*ln(x) )
      result = n2 * HMath::ln(n1);
      result = HMath::exp( result );
    }
  }

  return result;
}

HNumber HMath::exp( const HNumber& x )
{
  if( x.isNan() )
    return HNumber::nan();

  bool negative = x.isNegative();
  HNumber xs = HMath::abs( x );

  // adjust so that x is less than 1
  // use the fact that e^x = (e^(x/2))^2
  HNumber one(1);
  HNumber half("0.5");
  unsigned factor = 0;
  while( xs > one )
  {
    factor ++;
    xs = xs * half;
  }

  // Taylor expansion: e^x = 1 + x + x^2/2! + x^3/3! + ...
  HNumber num = xs;
  HNumber den = 1;
  HNumber sum = xs + 1;

  // now loop to sum the series
  for( int i = 2; i < HMATH_MAX_PREC; i++ )
  {
    num *= xs;
    den *= HNumber(i);
    if( num.isZero() ) break;
    HNumber s = HMath::div( num, den );
    if( s.isZero() ) break;
    sum += s;
  }

  HNumber result = sum;
  if( factor > 0 )
    while( factor > 0 )
    {
      factor--;
      result *= result;
    }

  if( negative )
    result = HMath::div( HNumber(1), result );

  return result;
};

HNumber HMath::ln( const HNumber& x )
{
  if( x.isNan() )
    return HNumber::nan();

  if( !x.isPositive() )
    return HNumber::nan();

  // short circuit
  if( x == HNumber(10) )
    return HNumber("2.30258509299404568401799145468436420760110148862877"
                     "29760333279009675726102948650438303813865953227795"
                     "49054520440916779445247118780973037711833599749301"
                     "72118016928228381938415404059160910960135436620869" );

  // useful constants
  HNumber two(2);
  HNumber one(1);
  HNumber half("0.5");

  // adjust so that x is between 0.5 and 2.0
  // use the fact that ln(x^2) = 2*ln(x)
  HNumber xs( x );
  unsigned factor = 2;
  while( xs >= two )
  {
    factor *= 2;
    xs = HMath::sqrt( xs );
  }
  while( xs <= half )
  {
    factor *= 2;
    xs = HMath::sqrt( xs );
  }

  // Taylor expansion: ln(x) = 2(a+a^3/3+a^5/5+...)
  // where a=(x-1)/(x+1)
  HNumber p = xs - 1;
  HNumber q = xs + 1;
  HNumber a = p / q;
  HNumber as = a*a;
  HNumber t = a;
  HNumber sum = a;

  // loop for the series (limited to avoid nasty cases)
  for( int i = 3; i < HMATH_MAX_PREC; i+= 2 )
  {
    t *= as;
    if( t.isZero() ) break;
    HNumber s = HMath::div( t, HNumber(i) );
    if( s.isZero() ) break;
    sum += s;
  }

  HNumber result = sum * HNumber( factor );
  return result;
}

HNumber HMath::log( const HNumber& x )
{
  if( x.isNan() )
    return HNumber::nan();

  if( !x.isPositive() )
    return HNumber::nan();

  HNumber result = HMath::ln( x ) / HMath::ln(10);
  return result;
}

HNumber HMath::lg( const HNumber& x )
{
  if( x.isNan() )
    return HNumber::nan();

  if( !x.isPositive() )
    return HNumber::nan();

  HNumber result = HMath::ln( x ) / HMath::ln(2);
  return result;
}

// ensure angle is within 0 to 2*pi
// useful for sin, cos
static HNumber simplifyAngle( const HNumber& x )
{
  if( x.isNan() )
    return HNumber::nan();

#if 1
  // using simple method
  HNumber pi2 = HMath::pi() * 2;
  HNumber nn = x / pi2;
  HNumber xs = x - HMath::integer(nn)*pi2;
  if( xs.isNegative() ) xs += pi2;

  return xs;
#else
  // using argument reduction method
  // see http://www.derekroconnor.net/Software/Ng--ArgReduction.pdf
  HNumber factor = HNumber(2) / HMath::pi();
  HNumber y = x * factor;
  HNumber f = HMath::frac(y);
  HNumber r = f * HMath::pi()/HNumber(2);

  // find int(y) mod 4
  HNumber n = HMath::integer( y );
  HNumber m = n - HNumber(4)*HMath::integer( n / HNumber(4) );
  HNumber halfpi = HMath::pi() / HNumber(2);
  if(m == HNumber(1))
    r += halfpi;
  if(m == HNumber(2))
    r += HMath::pi();
  if(m == HNumber(3))
    r += halfpi + HMath::pi();

  return r;
#endif
}

HNumber HMath::sin( const HNumber& x )
{
  if( x.isNan() )
    return HNumber::nan();

  // short circuit
  if( x.isZero() )
    return x;

  // adjust to small angle for speedup
  HNumber xs = simplifyAngle( x );

  // Taylor expansion: sin(x) = x - x^3/3! + x^5/5! - x^7/7! ...
  HNumber xsq = xs*xs;
  HNumber num = xs;
  HNumber den = 1;
  HNumber sum = xs;

  // loop for the series (limited to avoid nasty cases)
  for( int i = 3; i < HMATH_MAX_PREC; i+=2 )
  {
    num *= xsq;
    if( num.isZero() ) break;
    den *= HNumber(i-1);
    den *= HNumber(i);
    den = HMath::negate( den );
    HNumber s = HMath::div( num, den );
    if( s.isZero() ) break;
    sum += s;
  }

  HNumber result = sum;
  return result;
}


HNumber HMath::cos( const HNumber& x )
{
  if( x.isNan() )
    return HNumber::nan();

  // short circuit
  if( x.isZero() )
    return HNumber( 1 );

  // adjust to small angle for speedup
  HNumber xs = simplifyAngle( x );

  // Taylor expansion: cos(x) = 1 - x^2/2! + x^4/4! - x^6/6! ...
  HNumber xsq = xs*xs;
  HNumber num = 1;
  HNumber den = 1;
  HNumber sum = 1;

  // loop for the series (limited to avoid nasty cases)
  for( int i = 2; i < HMATH_MAX_PREC; i+=2 )
  {
    num *= xsq;
    if( num.isZero() ) break;
    den *= HNumber(i-1);
    den *= HNumber(i);
    den = HMath::negate( den );
    HNumber s = num / den;
    if( s.isZero() ) break;
    sum += s;
  }

  HNumber result = sum;
  return result;
}

HNumber HMath::tan( const HNumber& x )
{
  if( x.isNan() )
    return HNumber::nan();

  // tan(x) = cos(x)/sin(x)

  HNumber s = HMath::sin( x );
  if( s.isZero() )
    return s;

  HNumber c = HMath::cos( x );
  if( c.isZero() )
    return HNumber::nan();

  HNumber result = s / c;
  return result;
}

HNumber HMath::atan( const HNumber& x )
{
  if( x.isNan() )
    return HNumber::nan();

  // useful constants
  HNumber one("1.0");
  HNumber c( "0.2" );

  // short circuit
  if( x == c )
    return HNumber("0.19739555984988075837004976519479029344758510378785"
                     "21015176889402410339699782437857326978280372880441"
                     "12628118073691360104456479886794239355747565495216"
                     "30327005221074700156450155600612861855266332573187" );

  if( x == one )
    // essentially equals to HMath::pi()/4;
    return HNumber("0.78539816339744830961566084581987572104929234984377"
                     "64552437361480769541015715522496570087063355292669"
                     "95537021628320576661773461152387645557931339852032"
                     "12027936257102567548463027638991115573723873259549" );

  bool negative = x.isNegative();
  HNumber xs = HMath::abs( x );

  // adjust so that x is less than c (we choose c = 0.2)
  // use the fact that atan(x) = atan(c) + atan((x-c)/(1+xc))
  HNumber factor(0);
  HNumber base(0);
  while( xs > c )
  {
    base = HMath::atan( c );
    factor += one;
    HNumber p = xs - c;
    HNumber q = xs * c;
    xs = p / (q+one);
  }

  // Taylor series: atan(x) = x - x^3/3 + x^5/5 - x^7/7 + ...
  HNumber num = xs;
  HNumber xsq = xs*xs;
  HNumber den = 1;
  HNumber sum = xs;

  // loop for the series (limited to avoid nasty cases)
  for( int i = 3; i < HMATH_MAX_PREC; i+=2 )
  {
    num *= xsq;
    if( num.isZero() ) break;
    den = HNumber(i);
    int n = (i-1)/2;
    if( n&1 ) den = HNumber(-i);
    HNumber s = HMath::div( num, den );
    if( s.isZero() ) break;
    sum += s;
  }

  HNumber result = factor*base + sum;
  if( negative ) result = HMath::negate( result );
  return result;
};

HNumber HMath::asin( const HNumber& x )
{
  if( x.isNan() )
    return HNumber::nan();

  // asin(x) = atan(x/sqrt(1-x*x));
  HNumber d = HMath::sqrt( HNumber(1) - x*x );
  if( d.isZero() )
  {
    HNumber result = HMath::pi()/2;
    if( x.isNegative() )
      result = HMath::negate( result );
    return result;
  }

  HNumber result = HMath::atan( x / d );
  return result;
};

HNumber HMath::acos( const HNumber& x )
{
  if( x.isNan() )
    return HNumber::nan();

  if( x.isZero() )
    return HMath::pi()/2;

  // acos(x) = atan(sqrt(1-x*x)/x);
  HNumber n = HMath::sqrt( HNumber(1) - x*x );

  HNumber result = HMath::atan( n / x );
  return result;
};

HNumber HMath::sinh( const HNumber& x )
{
  if( x.isNan() )
    return HNumber::nan();

  // sinh(x) = 0.5*(e^x - e^(-x) )
  HNumber result = HMath::exp(x) - HMath::exp( HMath::negate(x) );
  result = result / 2;

  return result;
}

HNumber HMath::cosh( const HNumber& x )
{
  if( x.isNan() )
    return HNumber::nan();

  // cosh(x) = 0.5*(e^x - e^(-x) )
  HNumber result = HMath::exp(x) + HMath::exp( HMath::negate(x) );
  result = result / 2;

  return result;
}

HNumber HMath::tanh( const HNumber& x )
{
  if( x.isNan() )
    return HNumber::nan();

  // tanh(h) = sinh(x)/cosh(x)
  HNumber c = HMath::cosh( x );
  if( c.isZero() )
    return HNumber::nan();

  HNumber s = HMath::sinh( x );
  HNumber result = s / c;

  return result;
}

HNumber HMath::sign( const HNumber& x )
{
  if( x.isNan() )
    return HNumber::nan();

  HNumber result( 0 );
  if (x == result)
     return result;
  if (x < result)
     result = HNumber(-1);
  else
     result = HNumber(1);

  return result;
}

HNumber HMath::nCr( const HNumber& n, const HNumber& r )
{
  if( n.isNan() || r.isNan() || r > n)
    return HNumber::nan();

   if (r == HNumber(0) || r == n)
      return HNumber(1);

   if (r == HNumber(1))
      return HNumber(n);

   if (r > n/2)
      return factorial(n, r+1) / factorial((n-r), 1);
   else
      return factorial(n, (n-r+1)) / factorial(r, 1);
}

HNumber HMath::nPr( const HNumber& n, const HNumber& r )
{
  if( n.isNan() || r.isNan() || r > n)
    return HNumber::nan();

  if (r == HNumber(0))
    return HNumber(1);

  if (r == HNumber(1))
    return n;

  if (r == n)
    return factorial(n);

  return factorial(n, (n-r+1));
}

HNumber HMath::factorial( const HNumber& x, const HNumber& base )
{
  if( x.isNan()
      || x    < HNumber(0) || !x.isInteger()
      || base < HNumber(1) || !base.isInteger()
      || (x < base && x != HNumber(0))  )
    return HNumber::nan();

  if( x == HNumber(0) || x == HNumber(1) )
    return HNumber(1);

  if( x == base )
    return x;

  HNumber one(1);
  HNumber result = one;
  HNumber n = base - one;
  do
  {
     n = n + one;
     result = result * n;
  }
  while (n < x);

  return result;
}

void HMath::finalize()
{
  bc_free_num( &_zero_ );
  bc_free_num( &_one_ );
  bc_free_num( &_two_ );
  free( _one_ );
  free( _zero_ );
  free( _two_ );
  h_grabfree();
  h_grabfree();
  h_grabfree();
  h_grabfree();
}

std::ostream& operator<<( std::ostream& s, const HNumber& n )
{
  char* str = HMath::formatFixed( n );
  s << str;
  delete[] str;
  return s;
}
