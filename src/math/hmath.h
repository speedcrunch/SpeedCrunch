// HMath: C++ high precision math routines
// Copyright (C) 2004 Ariya Hidayat <ariya.hidayat@gmail.com>
// Copyright (C) 2007-2008, 2014 Helder Correia <helder.pereira.correia@gmail.com>
// Copyright (C) 2008 Wolf Lammen
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#ifndef MATH_HMATH_H
#define MATH_HMATH_H

#include "core/errors.h"

#include <ostream>

class HMath;
class HNumberPrivate;

class HNumber
{
  friend class HMath;
  friend HNumber operator-( const HNumber & );
  friend HNumber operator-( const HNumber &, const HNumber& );
  friend bool operator>( const HNumber& l, const HNumber& r );
  friend bool operator<( const HNumber& l, const HNumber& r );
  friend bool operator>=( const HNumber& l, const HNumber& r );
  friend bool operator<=( const HNumber& l, const HNumber& r );
  friend bool operator==( const HNumber& l, const HNumber& r );
  friend bool operator!=( const HNumber& l, const HNumber& r );

  public:
    HNumber();
    HNumber( const HNumber& );
    HNumber( int i );
    HNumber( const char* );
    ~HNumber();

    bool isNan() const;
    bool isZero() const;
    bool isPositive() const;
    bool isNegative() const;
    bool isInteger() const;

//TODO formats should be separated from numbers

    // 'g': decimal general (default)
    // 'f': decimal fixed
    // 'e': decimal scientific
    // 'n': decimal engineering
    // 'h': hexadecimal
    // 'H': hexadecimal two's complement for negative integers
    // 'o': octal
    // 'O': octal two's complement for negative integers
    // 'b': binary
    // 'B': binary two's complement for negative integers
    char format() const;
    HNumber& setFormat( char c = 0 );

    int toInt() const;
    Error error() const;

    HNumber& operator=( const HNumber& );
    HNumber operator+( const HNumber& ) const;
    HNumber& operator+=( const HNumber& );
    HNumber& operator-=( const HNumber& );
    HNumber operator*( const HNumber& ) const;
    HNumber& operator*=( const HNumber& );
    HNumber operator/( const HNumber& ) const;
    HNumber& operator/=( const HNumber& );
    HNumber operator%( const HNumber& ) const;
    HNumber operator&( const HNumber& ) const;
    HNumber& operator&=( const HNumber& );
    HNumber operator|( const HNumber& ) const;
    HNumber& operator|=( const HNumber& );
    HNumber operator^( const HNumber& ) const;
    HNumber& operator^=( const HNumber& );
    HNumber operator~() const;
    HNumber operator>>( const HNumber& ) const;
    HNumber operator<<( const HNumber& ) const;

  private:
    HNumberPrivate * d;

    int compare( const HNumber & other ) const;
};

class HMath
{
  public:
    // FORMAT
    static char * format( const HNumber & n, char format = 'g', int prec = -1 );
    // CONSTANTS
    static HNumber e();
    static HNumber phi();
    static HNumber pi();
    static HNumber nan(Error error = Success);
    // GENERAL MATH
    static HNumber rad2deg( const HNumber & angle );
    static HNumber deg2rad( const HNumber & angle );
    static HNumber max( const HNumber & n1, const HNumber & n2 );
    static HNumber min( const HNumber & n1, const HNumber & n2 );
    static HNumber abs( const HNumber & n );
    static HNumber integer( const HNumber & n );
    static HNumber frac( const HNumber & n );
    static HNumber floor( const HNumber & n );
    static HNumber ceil( const HNumber & n );
    static HNumber gcd( const HNumber & n1, const HNumber & n2 );
    static HNumber idiv( const HNumber& n1, const HNumber& n2 );
    static HNumber round( const HNumber & n, int prec = 0 );
    static HNumber trunc( const HNumber & n, int prec = 0 );
    static HNumber sqrt( const HNumber & n );
    static HNumber cbrt( const HNumber & n );
    static HNumber raise( const HNumber & n1, int n );
    static HNumber raise( const HNumber & n1, const HNumber & n2 );
    static HNumber sgn( const HNumber & x );
    // EXPONENTIAL FUNCTION AND RELATED
    static HNumber exp( const HNumber & x );
    static HNumber ln( const HNumber & x );
    static HNumber lg( const HNumber & x );
    static HNumber lb( const HNumber & x );
    static HNumber log( const HNumber & base, const HNumber & x );
    static HNumber sinh( const HNumber & x );
    static HNumber cosh( const HNumber & x );
    static HNumber tanh( const HNumber & x );
    static HNumber arsinh( const HNumber & x );
    static HNumber arcosh( const HNumber & x );
    static HNumber artanh( const HNumber & x );
    // TRIGONOMETRY
    static HNumber sin( const HNumber & x );
    static HNumber cos( const HNumber & x );
    static HNumber tan( const HNumber & x );
    static HNumber cot( const HNumber & x );
    static HNumber sec( const HNumber & x );
    static HNumber csc( const HNumber & x );
    static HNumber arcsin( const HNumber & x );
    static HNumber arccos( const HNumber & x );
    static HNumber arctan( const HNumber & x );
    // HIGHER MATH FUNCTIONS
    static HNumber factorial( const HNumber & x, const HNumber & base = HNumber(1) );
    static HNumber gamma( const HNumber & x);
    static HNumber lnGamma( const HNumber & x);
    static HNumber erf( const HNumber & x );
    static HNumber erfc( const HNumber & x );
    // PROBABILITY
    static HNumber nCr( const HNumber & n, const HNumber & k );
    static HNumber nPr( const HNumber & n, const HNumber & r );
    static HNumber binomialPmf( const HNumber & k, const HNumber & n, const HNumber & p );
    static HNumber binomialCdf( const HNumber & k, const HNumber & n, const HNumber & p );
    static HNumber binomialMean( const HNumber & n, const HNumber & p );
    static HNumber binomialVariance( const HNumber & n, const HNumber & p );
    static HNumber hypergeometricPmf( const HNumber & k, const HNumber & N, const HNumber & M, const HNumber & n );
    static HNumber hypergeometricCdf( const HNumber & k, const HNumber & N, const HNumber & M, const HNumber & n );
    static HNumber hypergeometricMean( const HNumber & N, const HNumber & M, const HNumber & n );
    static HNumber hypergeometricVariance( const HNumber & N, const HNumber & M, const HNumber & n );
    static HNumber poissonPmf( const HNumber & k, const HNumber & l );
    static HNumber poissonCdf( const HNumber & k, const HNumber & l );
    static HNumber poissonMean( const HNumber & l );
    static HNumber poissonVariance( const HNumber & l );
    // LOGIC
    static HNumber mask( const HNumber & val, const HNumber & bits );
    static HNumber compln( const HNumber & val, const HNumber & bits_grp );
    static HNumber sgnext( const HNumber & val, const HNumber & bits );
    static HNumber ashr( const HNumber & val, const HNumber & bits );
};

std::ostream & operator<<( std::ostream &, const HNumber & );

#endif