/* HMath: C++ high precision math routines
   Copyright (C) 2004 Ariya Hidayat <ariya.hidayat@gmail.com>   
   Last update: November 15, 2004

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, write to:
      The Free Software Foundation, Inc.
      59 Temple Place, Suite 330
      Boston, MA 02111-1307 USA.
   
 */
 
#include "hmath.h"
#include "number.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

class HNumber::Private
{
public:
  bc_num num;
  bool nan;
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

// convert simple string to number
static bc_num h_str2num( const char* str, int scale = HMATH_MAX_PREC )
{
  int digits, strscale;
  const char *ptr;
  char *nptr;
  char zero_int;

  /* Check for valid number and count digits. */
  ptr = str;
  digits = 0;
  strscale = 0;
  zero_int = FALSE;
  if ( (*ptr == '+') || (*ptr == '-'))  ptr++;  /* Sign */
  while (*ptr == '0') ptr++;			/* Skip leading zeros. */
  while (isdigit((int)*ptr)) ptr++, digits++;	/* digits */
  if (*ptr == '.') ptr++;			/* decimal point */
  while (isdigit((int)*ptr)) ptr++, strscale++;	/* digits */
  if ((*ptr != '\0') || (digits+strscale == 0))
    return h_create();

  /* Adjust numbers and allocate storage and initialize fields. */
  strscale = MIN(strscale, scale);
  if (digits == 0)
    {
      zero_int = TRUE;
      digits = 1;
    }
    
  bc_num num = h_create( digits, strscale );  

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
    
   return num; 
}


// add two numbers, return newly allocated number
static bc_num h_add( bc_num n1, bc_num n2 )
{  
  bc_num r = h_create();
  bc_add( n1, n2, &r, 1 );
  h_grabfree();
  return r;
}

// subtract two numbers, return newly allocated number
static bc_num h_sub( bc_num n1, bc_num n2 )
{  
  bc_num r = h_create();
  bc_sub( n1, n2, &r, 1 );
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


HNumber::HNumber()
{
  h_init();
  d = new Private;
  d->nan = false;
  d->num = h_create();
}

HNumber::HNumber( const HNumber& hn )
{
  h_init();
  d = new Private;
  d->nan = false;
  d->num = h_create();
  operator=( hn );
}

HNumber::HNumber( int i )
{
  h_init();
  d = new Private;
  d->nan = false;
  d->num = h_create();
  bc_int2num( &d->num, i );
}

HNumber::HNumber( const char* str )
{
  h_init();
  d = new Private;
  d->nan = false;
  d->num = h_create();

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
    
    char* p = s;
    for( ;; p++ )
    {
      if( *p != '+' )
      if( *p != '-' )
      if( *p != '.' )
      if( !isdigit(*p) )
        break;
     }
     
    int expd = 0;  
    
    if( ( *p == 'e' ) || ( *p == 'E' ) )
    {
      *p = '\0';
      expd = atoi( p+1 );
    }
    
    h_destroy( d->num );
    d->num = h_str2num( s );
    delete [] s;
    
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

HNumber HNumber::nan()
{
  HNumber n;
  n.d->nan = true;
  return n;  
}

HNumber& HNumber::operator=( const HNumber& hn )
{
  d->nan = hn.d->nan;
  h_destroy( d->num );
  d->num = h_copy( hn.d->num );
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

  if( format=='g' )
    return formatGeneral( hn, prec );  
  else if( format=='f' )
    return formatFixed( hn, prec );
  else if( format=='e' )
    return formatExp( hn, prec );  

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

HNumber HMath::raise( const HNumber& n1, int n )
{
  if( n1.isNan() ) return n1;
  
  if( n1.isZero() ) return n1;
  if( n1 == HNumber(1) ) return n1;
  if( n == 0 ) return HNumber(1);
  if( n == 1 ) return n1;
  
  HNumber result = n1;
  for( ; n > 1; n-- ) result *= n1;
  for( ; n < 1; n++ ) result /= n1;

  return result;
}

HNumber HMath::raise( const HNumber& n1, const HNumber& n2  )
{
  if( n1.isNan() ) return HNumber::nan();
  if( n2.isNan() ) return HNumber::nan();

  if( n1.isZero() ) return HNumber(0);
  if( n1 == HNumber(1) ) return n1;
  if( n2.isZero() ) return HNumber(1);
  if( n2 == HNumber(1) ) return n1;
  
  // x^y = exp( y*ln(x) )
  HNumber result = n2 * HMath::ln(n1);
  result = HMath::exp( result );
  
  return result;
}

HNumber HMath::exp( const HNumber& x )
{
  if( x.isNan() )
    return HNumber::nan();
    
  bool negative = x.isNegative();
  HNumber xs = HMath::abs( x );

  // adjust so that x is less than 1
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

// ensure angle is within 0 to 2*pi
// useful for sin, cos
static HNumber simplifyAngle( const HNumber& x )
{
  if( x.isNan() )
    return HNumber::nan();
    
  HNumber pi2 = HMath::pi() * 2; 
  HNumber nn = x / pi2;  
  HNumber xs = x - HMath::integer(nn)*pi2;
  if( xs.isNegative() ) xs += pi2;

  return xs;
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

std::ostream& operator<<( std::ostream& s, HNumber num )
{
  char* str = HMath::formatFixed( num );
  s << str;
  delete[] str;
  return s;
}

#ifdef HMATH_TEST

#include <iostream>
#include <string.h>

static int hmath_total_tests = 0;
static int hmath_failed_tests = 0;

#define CHECK(x,y)  check_value(__FILE__,__LINE__,#x,x,y)
#define CHECK_FORMAT(f,p,x,y) check_format(__FILE__,__LINE__,#x,x,f,p,y)
#define CHECK_PRECISE(x,y)  check_precise(__FILE__,__LINE__,#x,x,y)

static void check_value( const char *file, int line, const char* msg, 
const HNumber&n, const char* expected )
{
  hmath_total_tests++;
  char* result = HMath::formatFixed( n );
  if( strcmp( result, expected ) )
  {
    hmath_failed_tests++;
    std::cout << file << "["<< line <<"]: " << msg;
    std::cout << "  Result: " << result;
    std::cout << ", ";
    std::cout << "Expected: " << expected;
    std::cout << std::endl;
  }
  free( result );
}

static void check_format( const char *file, int line, const char* msg, 
const HNumber&n, char format, int prec, const char* expected )
{
  hmath_total_tests++;
  char* result = HMath::format( n, format, prec );
  if( strcmp( result, expected ) )
  {
    hmath_failed_tests++;
    std::cout << file << "["<< line <<"]: " << msg;
    std::cout << "  Result: " << result;
    std::cout << ", ";
    std::cout << "Expected: " << expected;
    std::cout << std::endl;
  }
  free( result );
}

static void check_precise( const char *file, int line, const char* msg, 
const HNumber&n, const char* expected )
{
  hmath_total_tests++;
  char* result = HMath::formatFixed( n, 50 );
  if( strcmp( result, expected ) )
  {
    hmath_failed_tests++;
    std::cout << file << "["<< line <<"]: " << msg;
    std::cout << "  Result: " << result;
    std::cout << ", ";
    std::cout << "Expected: " << expected;
    std::cout << std::endl;
  }
  free( result );
}

void test_create()
{
  CHECK( HNumber("1.0"), "1" );
  CHECK( HNumber("2.0"), "2" );
  CHECK( HNumber("1e-3"), "0.001" );
  
  // too large or too small
  CHECK( HNumber("1e200"), "NaN" );
  CHECK( HNumber("1e-200"), "NaN" );
  CHECK_FORMAT( 'e', 2, HNumber("1e200"), "NaN" );
  CHECK_FORMAT( 'e', 2, HNumber("1e-200"), "NaN" );
}

void test_format()
{
  // fixed decimal digits
  CHECK_FORMAT( 'f', 0, HNumber("NaN"), "NaN" );
  CHECK_FORMAT( 'f', 0, HNumber("0"), "0" );
  CHECK_FORMAT( 'f', 0, HNumber("1.1"), "1" );
  CHECK_FORMAT( 'f', 1, HNumber("2.11"), "2.1" );
  CHECK_FORMAT( 'f', 2, HNumber("3.111"), "3.11" );
  CHECK_FORMAT( 'f', 3, HNumber("4.1111"), "4.111" );
  CHECK_FORMAT( 'f', 2, HNumber("3.14"), "3.14" );
  CHECK_FORMAT( 'f', 3, HNumber("3.14"), "3.140" );
  CHECK_FORMAT( 'f', 5, HNumber("3.14"), "3.14000" );
  CHECK_FORMAT( 'f', 7, HNumber("3.14"), "3.1400000" );
  CHECK_FORMAT( 'f', 7, HNumber("-0.001"), "-0.0010000" );
  CHECK_FORMAT( 'f', 8, HNumber("-0.001"), "-0.00100000" );
  CHECK_FORMAT( 'f', 9, HNumber("-0.001"), "-0.001000000" );
  CHECK_FORMAT( 'f', 1, HNumber("4.001"), "4.0" );
  CHECK_FORMAT( 'f', -1, HNumber("4.000000000000000000000000000000000000000000001"), "4" );

  // exponential format
  CHECK_FORMAT( 'e', 0, HNumber("NaN"), "NaN" );
  CHECK_FORMAT( 'e', 0, HNumber("0"), "0e0" );
  CHECK_FORMAT( 'e', 0, HNumber("3.14"), "3e0" );
  CHECK_FORMAT( 'e', 1, HNumber("3.14"), "3.1e0" );
  CHECK_FORMAT( 'e', 2, HNumber("3.14"), "3.14e0" );
  CHECK_FORMAT( 'e', 3, HNumber("3.14"), "3.140e0" );
  CHECK_FORMAT( 'e', 5, HNumber("3.14"), "3.14000e0" );
  CHECK_FORMAT( 'e', 7, HNumber("3.14"), "3.1400000e0" );
  CHECK_FORMAT( 'e', 3, HNumber("-0.001"), "-1.000e-3" );
  CHECK_FORMAT( 'e', 2, HNumber("0.0001"), "1.00e-4" );
  CHECK_FORMAT( 'e', 2, HNumber("0.001"), "1.00e-3" );
  CHECK_FORMAT( 'e', 2, HNumber("0.01"), "1.00e-2" );
  CHECK_FORMAT( 'e', 2, HNumber("0.1"), "1.00e-1" );
  CHECK_FORMAT( 'e', 2, HNumber("1"), "1.00e0" );
  CHECK_FORMAT( 'e', 2, HNumber("10"), "1.00e1" );
  CHECK_FORMAT( 'e', 2, HNumber("100"), "1.00e2" );
  CHECK_FORMAT( 'e', 2, HNumber("1000"), "1.00e3" );
  CHECK_FORMAT( 'e', 2, HNumber("10000"), "1.00e4" );
  CHECK_FORMAT( 'e', 2, HNumber("100000"), "1.00e5" );
  CHECK_FORMAT( 'e', 2, HNumber("1000000"), "1.00e6" );
  CHECK_FORMAT( 'e', 2, HNumber("10000000"), "1.00e7" );
  
  // general format
  CHECK_FORMAT( 'g', -1, HMath::pi(), "3.14159265358979323846" );
  CHECK_FORMAT( 'g', 3, HNumber("0"), "0.000" );
  CHECK_FORMAT( 'g', 3, HNumber("0.000000001"), "1.000e-9" );
  CHECK_FORMAT( 'g', 3, HNumber("0.00000001"), "1.000e-8" );
  CHECK_FORMAT( 'g', 3, HNumber("0.0000001"), "1.000e-7" );
  CHECK_FORMAT( 'g', 3, HNumber("0.000001"), "1.000e-6" );
  CHECK_FORMAT( 'g', 3, HNumber("0.00001"), "1.000e-5" );
  CHECK_FORMAT( 'g', 3, HNumber("0.0001"), "1.000e-4" );
  CHECK_FORMAT( 'g', 3, HNumber("0.001"), "0.001" );
  CHECK_FORMAT( 'g', 3, HNumber("0.01"), "0.010" );
  CHECK_FORMAT( 'g', 3, HNumber("0.1"), "0.100" );
  CHECK_FORMAT( 'g', 3, HNumber("10"), "10.000" );
  CHECK_FORMAT( 'g', 3, HNumber("100"), "100.000" );
  CHECK_FORMAT( 'g', 3, HNumber("1000"), "1000.000" );
  CHECK_FORMAT( 'g', 3, HNumber("10000"), "10000.000" );
  CHECK_FORMAT( 'g', 3, HNumber("100000"), "100000.000" );
  CHECK_FORMAT( 'g', 3, HNumber("1000000"), "1.000e6" );
  CHECK_FORMAT( 'g', 3, HNumber("10000000"), "1.000e7" );
  CHECK_FORMAT( 'g', 3, HNumber("100000000"), "1.000e8" );
  CHECK_FORMAT( 'g', 3, HNumber("1403.1977"), "1403.198" );
  CHECK_FORMAT( 'g', 3, HNumber("2604.1980"), "2604.198" );
  CHECK_FORMAT( 'g', 3, HNumber("2.47e4"), "24700.000" );

}

void test_op()
{
  // addition
  CHECK( HNumber(0)+HNumber(0), "0" );
  CHECK( HNumber(1)+HNumber(0), "1" );
  CHECK( HNumber(1)+HNumber(1), "2" );
  CHECK( HNumber(1)+HNumber(2), "3" );
  CHECK( HNumber(1)+HNumber(10), "11" );
  CHECK( HNumber(1)+HNumber(100), "101" );
  CHECK( HNumber(1)+HNumber(1000), "1001" );
  
  // subtraction
  CHECK( HNumber(0)-HNumber(0), "0" );
  CHECK( HNumber(1)-HNumber(0), "1" );
  CHECK( HNumber(1)-HNumber(2), "-1" );
  
  // division
  CHECK( HNumber(1)/HNumber(2), "0.5" );
  CHECK_PRECISE( HNumber(1)/HNumber(3), "0.33333333333333333333333333333333333333333333333333" );
  CHECK_PRECISE( HNumber(2)/HNumber(3), "0.66666666666666666666666666666666666666666666666667" );
  CHECK_PRECISE( HNumber(1)/HNumber(7), "0.14285714285714285714285714285714285714285714285714" );
  CHECK_PRECISE( HNumber(2)/HNumber(7), "0.28571428571428571428571428571428571428571428571429" );
  CHECK_PRECISE( HNumber(3)/HNumber(7), "0.42857142857142857142857142857142857142857142857143" );
  CHECK_PRECISE( HNumber(4)/HNumber(7), "0.57142857142857142857142857142857142857142857142857" );
  CHECK_PRECISE( HNumber(1)/HNumber(9), "0.11111111111111111111111111111111111111111111111111" );

  // multiplication
  CHECK( HNumber(0)*HNumber(0), "0" );
  CHECK( HNumber("1.0")*HNumber("0.0"), "0" );
  CHECK( HNumber(1)*HNumber(1), "1" );
  CHECK( HNumber(3)*HNumber(-4), "-12" );
  CHECK( HNumber(-2)*HNumber(5), "-10" );
  CHECK( HNumber(6)*HNumber(7), "42" );
  CHECK( HNumber("1.5")*HNumber("1.5"), "2.25" );
}

void test_functions()
{
  // pi
  CHECK( HMath::pi(), "3.14159265358979323846" );
  CHECK_PRECISE( HMath::pi(), "3.14159265358979323846264338327950288419716939937511" );
  
  // abs
  CHECK( HMath::abs("0"), "0" );
  CHECK( HMath::abs("1"), "1" );
  CHECK( HMath::abs("100"), "100" );
  CHECK( HMath::abs("-100"), "100" );
  CHECK( HMath::abs("-3.14159"), "3.14159" );
  CHECK( HMath::abs("NaN"), "NaN" );

  // round
  CHECK( HMath::round( "3.14" ), "3" );
  CHECK( HMath::round( "1.77" ), "2" );
  CHECK( HMath::round( "3.14159", 4 ), "3.1416" );
  CHECK( HMath::round( "3.14159", 3 ), "3.142" );
  CHECK( HMath::round( "3.14159", 2 ), "3.14" );
  CHECK( HMath::round( "3.14159", 1 ), "3.1" );
  CHECK( HMath::round( "3.14159", 0 ), "3" );
  CHECK( HMath::round( "-2.6041980", 4 ), "-2.6042" );
  CHECK( HMath::round( "-2.6041980", 3 ), "-2.604" );
  CHECK( HMath::round( "-2.6041980", 2 ), "-2.6" );
  CHECK( HMath::round( "-2.6041980", 1 ), "-2.6" );
  CHECK( HMath::round( "-2.6041980", 0 ), "-3" );
  CHECK( HMath::round( "NaN" ), "NaN" );
  
  // integer
  CHECK( HMath::integer( "0" ), "0" );
  CHECK( HMath::integer( "0.25" ), "0" );
  CHECK( HMath::integer( "0.85" ), "0" );
  CHECK( HMath::integer( "14.0377" ), "14" );
  CHECK( HMath::integer( "-0.25" ), "0" );
  CHECK( HMath::integer( "-0.85" ), "0" );
  CHECK( HMath::integer( "-14.0377" ), "-14" );
  CHECK( HMath::integer( "NaN" ), "NaN" );

  // frac
  CHECK( HMath::frac( "0" ), "0" );
  CHECK( HMath::frac( "3.14159" ), "0.14159" );
  CHECK( HMath::frac( "0.14159" ), "0.14159" );
  CHECK( HMath::frac( "-3.14159" ), "-0.14159" );
  CHECK( HMath::frac( "-0.14159" ), "-0.14159" );
  CHECK( HMath::frac( "NaN" ), "NaN" );
  
  // checking function 'sqrt'
  CHECK( HMath::sqrt(1), "1" );
  CHECK( HMath::sqrt(4), "2" );
  CHECK( HMath::sqrt(9), "3" );
  CHECK( HMath::sqrt(16), "4" );
  CHECK_PRECISE( HMath::sqrt(2), "1.41421356237309504880168872420969807856967187537695" );
  CHECK_PRECISE( HMath::sqrt(3), "1.73205080756887729352744634150587236694280525381038" );
  CHECK_PRECISE( HMath::sqrt(5), "2.23606797749978969640917366873127623544061835961153" );
  CHECK_PRECISE( HMath::sqrt(7), "2.64575131106459059050161575363926042571025918308245" );
  CHECK_PRECISE( HMath::sqrt(8), "2.82842712474619009760337744841939615713934375075390" );
  CHECK_PRECISE( HMath::sqrt(10), "3.16227766016837933199889354443271853371955513932522" );
  CHECK_PRECISE( HMath::sqrt(11), "3.31662479035539984911493273667068668392708854558935" );
  CHECK_PRECISE( HMath::sqrt(12), "3.46410161513775458705489268301174473388561050762076" );
  CHECK_PRECISE( HMath::sqrt(13), "3.60555127546398929311922126747049594625129657384525" );
  CHECK_PRECISE( HMath::sqrt(14), "3.74165738677394138558374873231654930175601980777873" );
  CHECK_PRECISE( HMath::sqrt(15), "3.87298334620741688517926539978239961083292170529159" );
  CHECK_PRECISE( HMath::sqrt(17), "4.12310562561766054982140985597407702514719922537362" );
  CHECK_PRECISE( HMath::sqrt(18), "4.24264068711928514640506617262909423570901562613084" );
  CHECK_PRECISE( HMath::sqrt(19), "4.35889894354067355223698198385961565913700392523244" );
  CHECK_PRECISE( HMath::sqrt(20), "4.47213595499957939281834733746255247088123671922305" );
  CHECK( HMath::sqrt("0.04"), "0.2" );
  CHECK( HMath::sqrt("0.09"), "0.3" );
  CHECK( HMath::sqrt("0.16"), "0.4" );
  CHECK( HMath::sqrt(-1), "NaN" );
  CHECK( HMath::sqrt("NaN"), "NaN" );

  // raise
  CHECK( HMath::raise(10,-3), "0.001" );
  CHECK( HMath::raise(10,-2), "0.01" );
  CHECK( HMath::raise(10,-1), "0.1" );
  CHECK( HMath::raise(10,0), "1" );
  CHECK( HMath::raise(10,1), "10" );
  CHECK( HMath::raise(10,2), "100" );
  CHECK( HMath::raise(10,3), "1000" );
  CHECK( HMath::raise(10,4), "10000" );
  CHECK( HMath::raise("2","2"), "4" );
  CHECK( HMath::raise("3","3"), "27" );
  CHECK( HMath::raise("4","4"), "256" );
  CHECK_PRECISE( HMath::raise("2","0.1"), "1.07177346253629316421300632502334202290638460497756" );
  CHECK_PRECISE( HMath::raise("2","0.2"), "1.14869835499703500679862694677792758944385088909780" );
  CHECK_PRECISE( HMath::raise("2","0.3"), "1.23114441334491628449939306916774310987613776110082" );
  CHECK( HMath::raise("NaN","0"), "NaN" );
  CHECK( HMath::raise("-1","NaN"), "NaN" );
  
  // exp
  CHECK_PRECISE( HMath::exp("0.1"), "1.10517091807564762481170782649024666822454719473752" );
  CHECK_PRECISE( HMath::exp("0.2"), "1.22140275816016983392107199463967417030758094152050" );
  CHECK_PRECISE( HMath::exp("0.3"), "1.34985880757600310398374431332800733037829969735937" );
  CHECK_PRECISE( HMath::exp("0.4"), "1.49182469764127031782485295283722228064328277393743" );
  CHECK_PRECISE( HMath::exp("0.5"), "1.64872127070012814684865078781416357165377610071015" );
  CHECK_PRECISE( HMath::exp("0.6"), "1.82211880039050897487536766816286451338223880854644" );
  CHECK_PRECISE( HMath::exp("0.7"), "2.01375270747047652162454938858306527001754239414587" );
  CHECK_PRECISE( HMath::exp("0.8"), "2.22554092849246760457953753139507675705363413504848" );
  CHECK_PRECISE( HMath::exp("0.9"), "2.45960311115694966380012656360247069542177230644008" );
  CHECK_PRECISE( HMath::exp("1.0"), "2.71828182845904523536028747135266249775724709369996" );
  
  // ln
  CHECK_PRECISE( HMath::ln("0.1"), "-2.30258509299404568401799145468436420760110148862877" );
  CHECK_PRECISE( HMath::ln("0.2"), "-1.60943791243410037460075933322618763952560135426852" );
  CHECK_PRECISE( HMath::ln("0.3"), "-1.20397280432593599262274621776183850295361093080602" );
  CHECK_PRECISE( HMath::ln("0.4"), "-0.91629073187415506518352721176801107145010121990826" );
  CHECK_PRECISE( HMath::ln("0.5"), "-0.69314718055994530941723212145817656807550013436026" );
  CHECK_PRECISE( HMath::ln("0.6"), "-0.51082562376599068320551409630366193487811079644577" );
  CHECK_PRECISE( HMath::ln("0.7"), "-0.35667494393873237891263871124118447796401675904691" );
  CHECK_PRECISE( HMath::ln("0.8"), "-0.22314355131420975576629509030983450337460108554801" );
  CHECK_PRECISE( HMath::ln("0.9"), "-0.10536051565782630122750098083931279830612037298327" );
  CHECK_PRECISE( HMath::ln("1.0"),  "0.00000000000000000000000000000000000000000000000000" );
  CHECK_PRECISE( HMath::ln("1.1"),  "0.09531017980432486004395212328076509222060536530864" );
  CHECK_PRECISE( HMath::ln("1.2"),  "0.18232155679395462621171802515451463319738933791449" );
  CHECK_PRECISE( HMath::ln("1.3"),  "0.26236426446749105203549598688095439720416645613143" );
  CHECK_PRECISE( HMath::ln("1.4"),  "0.33647223662121293050459341021699209011148337531334" );
  CHECK_PRECISE( HMath::ln("1.5"),  "0.40546510810816438197801311546434913657199042346249" );
  CHECK_PRECISE( HMath::ln("1.6"),  "0.47000362924573555365093703114834206470089904881225" );
  CHECK_PRECISE( HMath::ln("1.7"),  "0.53062825106217039623154316318876232798710152395697" );
  CHECK_PRECISE( HMath::ln("1.8"),  "0.58778666490211900818973114061886376976937976137698" );
  CHECK_PRECISE( HMath::ln("1.9"),  "0.64185388617239477599103597720348932963627777267036" );
  CHECK_PRECISE( HMath::ln("2.0"),  "0.69314718055994530941723212145817656807550013436026" );
  CHECK_PRECISE( HMath::ln("3.0"),  "1.09861228866810969139524523692252570464749055782275" );
  CHECK_PRECISE( HMath::ln("4.0"),  "1.38629436111989061883446424291635313615100026872051" );
  CHECK_PRECISE( HMath::ln("100"),  "4.60517018598809136803598290936872841520220297725755" );
  
  // log
  CHECK( HMath::log("1e-5"), "-5" );
  CHECK( HMath::log("1e-4"), "-4" );
  CHECK( HMath::log("1e-3"), "-3" );
  CHECK( HMath::log("10"), "1" );
  CHECK( HMath::log("100"), "2" );
  CHECK( HMath::log("1000"), "3" );
  CHECK( HMath::log("10000"), "4" );
  CHECK( HMath::log("1e5"), "5" );
  CHECK( HMath::log("1e6"), "6" );
  CHECK( HMath::log("1e7"), "7" );
  CHECK( HMath::log("1e8"), "8" );
  CHECK( HMath::log("1e9"), "9" );
  CHECK( HMath::log("1e10"), "10" );
  CHECK( HMath::log("-1"), "NaN" );
  CHECK( HMath::log("NaN"), "NaN" );

  // sin
  CHECK( HMath::sin( "0" ), "0" );
  CHECK( HMath::sin( HMath::pi()/4 ), "0.7071067811865475244" );
  CHECK( HMath::sin( HMath::pi()/3 ), "0.86602540378443864676");
  CHECK( HMath::sin( HMath::pi()/2 ), "1" );
  CHECK( HMath::sin( HMath::pi()/1 ), "0" );
  CHECK( HMath::sin( HMath::pi()*2/3 ), "0.86602540378443864676");
  CHECK( HMath::sin( HMath::pi()*4/3 ), "-0.86602540378443864676");
  CHECK( HMath::sin( HMath::pi()*5/3 ), "-0.86602540378443864676");
  CHECK( HMath::sin( HMath::pi()*6/3 ), "0");
  CHECK( HMath::sin( HMath::pi()*7/3 ), "0.86602540378443864676");
  CHECK( HMath::sin( HMath::pi()*9/3 ), "0");
  CHECK_PRECISE( HMath::sin("0.0"), "0.00000000000000000000000000000000000000000000000000" );
  CHECK_PRECISE( HMath::sin("0.1"), "0.09983341664682815230681419841062202698991538801798" );
  CHECK_PRECISE( HMath::sin("0.2"), "0.19866933079506121545941262711838975037020672954021" );
  CHECK_PRECISE( HMath::sin("0.3"), "0.29552020666133957510532074568502737367783211174262" );
  CHECK_PRECISE( HMath::sin("0.4"), "0.38941834230865049166631175679570526459306018344396" );
  CHECK_PRECISE( HMath::sin("0.5"), "0.47942553860420300027328793521557138808180336794060" );
  CHECK_PRECISE( HMath::sin("0.6"), "0.56464247339503535720094544565865790710988808499415" );
  CHECK_PRECISE( HMath::sin("0.7"), "0.64421768723769105367261435139872018306581384457369" );
  CHECK_PRECISE( HMath::sin("0.8"), "0.71735609089952276162717461058138536619278523779142" );
  CHECK_PRECISE( HMath::sin("0.9"), "0.78332690962748338846138231571354862314014792572031" );
  CHECK_PRECISE( HMath::sin("1.0"), "0.84147098480789650665250232163029899962256306079837" );
  CHECK_PRECISE( HMath::sin("2.0"), "0.90929742682568169539601986591174484270225497144789" );
  CHECK_PRECISE( HMath::sin("3.0"), "0.14112000805986722210074480280811027984693326425227" );
  CHECK_PRECISE( HMath::sin("4.0"), "-0.75680249530792825137263909451182909413591288733647" );
  CHECK_PRECISE( HMath::sin("5.0"), "-0.95892427466313846889315440615599397335246154396460" );
  
  // cos
  CHECK( HMath::cos( "0" ), "1" );
  CHECK( HMath::cos( HMath::pi()/4 ), "0.7071067811865475244" );
  CHECK( HMath::cos( HMath::pi()/3 ), "0.5");
  CHECK( HMath::cos( HMath::pi()/2 ), "0" );
  CHECK( HMath::cos( HMath::pi()/1 ), "-1" );
  CHECK( HMath::cos( HMath::pi()*2/3 ), "-0.5");
  CHECK( HMath::cos( HMath::pi()*4/3 ), "-0.5");
  CHECK( HMath::cos( HMath::pi()*5/3 ), "0.5");
  CHECK( HMath::cos( HMath::pi()*6/3 ), "1");
  CHECK( HMath::cos( HMath::pi()*7/3 ), "0.5");
  CHECK( HMath::cos( HMath::pi()*9/3 ), "-1");
  CHECK_PRECISE( HMath::cos("0.0"), "1.00000000000000000000000000000000000000000000000000" );
  CHECK_PRECISE( HMath::cos("0.1"), "0.99500416527802576609556198780387029483857622541508" );
  CHECK_PRECISE( HMath::cos("0.2"), "0.98006657784124163112419651674816887739352436080657" );
  CHECK_PRECISE( HMath::cos("0.3"), "0.95533648912560601964231022756804989824421408263204" );
  CHECK_PRECISE( HMath::cos("0.4"), "0.92106099400288508279852673205180161402585956931985" );
  CHECK_PRECISE( HMath::cos("0.5"), "0.87758256189037271611628158260382965199164519710974" );
  CHECK_PRECISE( HMath::cos("0.6"), "0.82533561490967829724095249895537603887809103918847" );
  CHECK_PRECISE( HMath::cos("0.7"), "0.76484218728448842625585999019186490926821055037370" );
  CHECK_PRECISE( HMath::cos("0.8"), "0.69670670934716542092074998164232492610178601370806" );
  CHECK_PRECISE( HMath::cos("0.9"), "0.62160996827066445648471615140713350872176136659124" );
  CHECK_PRECISE( HMath::cos("1.0"), "0.54030230586813971740093660744297660373231042061792" );
  CHECK_PRECISE( HMath::cos("2.0"), "-0.41614683654714238699756822950076218976600077107554" );
  CHECK_PRECISE( HMath::cos("3.0"), "-0.98999249660044545727157279473126130239367909661559" );
  CHECK_PRECISE( HMath::cos("4.0"), "-0.65364362086361191463916818309775038142413359664622" );

  // tan
  CHECK( HMath::tan( HMath::pi()/4 ), "1" );
  CHECK( HMath::tan( HMath::pi()/3 ), "1.73205080756887729353");
  CHECK( HMath::tan( HMath::pi()/1 ), "0" );
  CHECK_PRECISE( HMath::tan("0.0"), "0.00000000000000000000000000000000000000000000000000" );
  CHECK_PRECISE( HMath::tan("0.1"), "0.10033467208545054505808004578111153681900480457644" );
  CHECK_PRECISE( HMath::tan("0.2"), "0.20271003550867248332135827164753448262687566965163" );
  CHECK_PRECISE( HMath::tan("0.3"), "0.30933624960962323303530367969829466725781590680046" );
  CHECK_PRECISE( HMath::tan("0.4"), "0.42279321873816176198163542716529033394198977271569" );
  CHECK_PRECISE( HMath::tan("0.5"), "0.54630248984379051325517946578028538329755172017979" );
  CHECK_PRECISE( HMath::tan("0.6"), "0.68413680834169231707092541746333574524265408075678" );
  CHECK_PRECISE( HMath::tan("0.7"), "0.84228838046307944812813500221293771718722125080420" );
  CHECK_PRECISE( HMath::tan("0.8"), "1.02963855705036401274636117282036528416821960677231" );
  CHECK_PRECISE( HMath::tan("0.9"), "1.26015821755033913713457548539574847783362583439629" );
  CHECK_PRECISE( HMath::tan("1.0"), "1.55740772465490223050697480745836017308725077238152" );
  CHECK_PRECISE( HMath::tan("2.0"), "-2.18503986326151899164330610231368254343201774622766" );
  CHECK_PRECISE( HMath::tan("3.0"), "-0.14254654307427780529563541053391349322609228490180" );
  CHECK_PRECISE( HMath::tan("4.0"), "1.15782128234957758313734241826732392311976276736714" );
  
  // atan
  CHECK_PRECISE( HMath::atan("0.0"), "0.00000000000000000000000000000000000000000000000000" );
  CHECK_PRECISE( HMath::atan("0.1"), "0.09966865249116202737844611987802059024327832250431" );
  CHECK_PRECISE( HMath::atan("0.2"), "0.19739555984988075837004976519479029344758510378785" );
  CHECK_PRECISE( HMath::atan("0.3"), "0.29145679447786709199560462143289119350316759901207" );
  CHECK_PRECISE( HMath::atan("0.4"), "0.38050637711236488630358791681043310449740571365810" );
  CHECK_PRECISE( HMath::atan("0.5"), "0.46364760900080611621425623146121440202853705428612" );
  CHECK_PRECISE( HMath::atan("0.6"), "0.54041950027058415544357836460859991013514825146259" );
  CHECK_PRECISE( HMath::atan("1.0"), "0.78539816339744830961566084581987572104929234984378" );
  CHECK_PRECISE( HMath::atan("-0.1"), "-0.09966865249116202737844611987802059024327832250431" );
  CHECK_PRECISE( HMath::atan("-0.2"), "-0.19739555984988075837004976519479029344758510378785" );
  CHECK_PRECISE( HMath::atan("-0.3"), "-0.29145679447786709199560462143289119350316759901207" );
  CHECK_PRECISE( HMath::atan("-0.4"), "-0.38050637711236488630358791681043310449740571365810" );
  CHECK_PRECISE( HMath::atan("-0.5"), "-0.46364760900080611621425623146121440202853705428612" );
  CHECK_PRECISE( HMath::atan("-0.6"), "-0.54041950027058415544357836460859991013514825146259" );
  CHECK_PRECISE( HMath::atan("-1.0"), "-0.78539816339744830961566084581987572104929234984378" );

  // asin
  CHECK_PRECISE( HMath::asin("0.0"), "0.00000000000000000000000000000000000000000000000000" );
  CHECK_PRECISE( HMath::asin("0.1"), "0.10016742116155979634552317945269331856867597222963" );
  CHECK_PRECISE( HMath::asin("0.2"), "0.20135792079033079145512555221762341024003808140223" );
  CHECK_PRECISE( HMath::asin("0.3"), "0.30469265401539750797200296122752916695456003170678" );
  CHECK_PRECISE( HMath::asin("0.4"), "0.41151684606748801938473789761733560485570113512703" );

  // acos
  CHECK_PRECISE( HMath::acos("0.1"), "1.47062890563333682288579851218705812352990872745792" );
  CHECK_PRECISE( HMath::acos("0.2"), "1.36943840600456582777619613942212803185854661828532" );
  CHECK_PRECISE( HMath::acos("0.3"), "1.26610367277949911125931873041222227514402466798078" );
  CHECK_PRECISE( HMath::acos("0.4"), "1.15927948072740859984658379402241583724288356456053" );

  // consistency: tan vs atan
  CHECK( HMath::atan("0.10033467208545054505808004578111153681900480457644"), "0.1" );
  CHECK( HMath::atan("0.20271003550867248332135827164753448262687566965163"), "0.2" );
  CHECK( HMath::atan("0.30933624960962323303530367969829466725781590680046"), "0.3" );
  CHECK( HMath::atan("0.42279321873816176198163542716529033394198977271569"), "0.4" );
  CHECK( HMath::atan("0.54630248984379051325517946578028538329755172017979"), "0.5" );
  CHECK( HMath::atan("0.68413680834169231707092541746333574524265408075678"), "0.6" );
  CHECK( HMath::atan("0.84228838046307944812813500221293771718722125080420"), "0.7" );
  CHECK( HMath::atan("1.02963855705036401274636117282036528416821960677231"), "0.8" );
  CHECK( HMath::atan("1.26015821755033913713457548539574847783362583439629"), "0.9" );
  CHECK( HMath::atan("1.55740772465490223050697480745836017308725077238152"), "1" );
  
  // consistency: sin vs asin for small angle
  CHECK( HMath::asin("0.09983341664682815230681419841062202698991538801798" ), "0.1");
  CHECK( HMath::asin("0.19866933079506121545941262711838975037020672954021" ), "0.2");
  CHECK( HMath::asin("0.29552020666133957510532074568502737367783211174262" ), "0.3");
  CHECK( HMath::asin("0.38941834230865049166631175679570526459306018344396" ), "0.4");
  CHECK( HMath::asin("0.47942553860420300027328793521557138808180336794060" ), "0.5");
  CHECK( HMath::asin("0.56464247339503535720094544565865790710988808499415" ), "0.6");
  CHECK( HMath::asin("0.64421768723769105367261435139872018306581384457369" ), "0.7");
  CHECK( HMath::asin("0.71735609089952276162717461058138536619278523779142" ), "0.8");

  // sinh
  CHECK_PRECISE( HMath::sinh("0.1"), "0.10016675001984402582372938352190502351492091687856" );
  CHECK_PRECISE( HMath::sinh("0.2"), "0.20133600254109398762556824301031737297449484262574" );
  CHECK_PRECISE( HMath::sinh("0.3"), "0.30452029344714261895843526700509522909802423268018" );
  CHECK_PRECISE( HMath::sinh("0.4"), "0.41075232580281550854021001384469810435315092436331" );
  CHECK_PRECISE( HMath::sinh("0.5"), "0.52109530549374736162242562641149155910592898261148" );
  CHECK_PRECISE( HMath::sinh("0.6"), "0.63665358214824127112345437546514831902496342592790" );
  CHECK_PRECISE( HMath::sinh("0.7"), "0.75858370183953350345987464759276815415493761421703" );
  CHECK_PRECISE( HMath::sinh("0.8"), "0.88810598218762300657471757318975698055970959688815" );
  CHECK_PRECISE( HMath::sinh("0.9"), "1.02651672570817527595833616197842235379403446513485" );
  CHECK_PRECISE( HMath::sinh("1.0"), "1.17520119364380145688238185059560081515571798133410" );
  
  // cosh
  CHECK_PRECISE( HMath::cosh("0.1"), "1.00500416805580359898797844296834164470962627785896" );
  CHECK_PRECISE( HMath::cosh("0.2"), "1.02006675561907584629550375162935679733308609889476" );
  CHECK_PRECISE( HMath::cosh("0.3"), "1.04533851412886048502530904632291210128027546467919" );
  CHECK_PRECISE( HMath::cosh("0.4"), "1.08107237183845480928464293899252417629013184957412" );
  CHECK_PRECISE( HMath::cosh("0.5"), "1.12762596520638078522622516140267201254784711809867" );
  CHECK_PRECISE( HMath::cosh("0.6"), "1.18546521824226770375191329269771619435727538261853" );
  CHECK_PRECISE( HMath::cosh("0.7"), "1.25516900563094301816467474099029711586260477992884" );
  CHECK_PRECISE( HMath::cosh("0.8"), "1.33743494630484459800481995820531977649392453816033" );
  CHECK_PRECISE( HMath::cosh("0.9"), "1.43308638544877438784179040162404834162773784130523" );
  CHECK_PRECISE( HMath::cosh("1.0"), "1.54308063481524377847790562075706168260152911236586" );

}

int test_hmath()
{
  hmath_total_tests = 0;
  hmath_failed_tests = 0;

  test_create();
  test_format();
  test_op();
  test_functions();
  
  std::cout << hmath_total_tests << " total, ";
  std::cout << hmath_failed_tests << " failed\n";

  HMath::finalize();  
  return hmath_failed_tests;
};

int main()
{
  return test_hmath();
}

#endif
