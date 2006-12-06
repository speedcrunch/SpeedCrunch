/* HMath: C++ high precision math routines
   Copyright (C) 2004 Ariya Hidayat <ariya.hidayat@gmail.com>   

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

#ifndef HMATH_H
#define HMATH_H

#include <iostream>

class HMath;
class HNumberPrivate;

class HNumber
{
friend class HMath;

public:

  /*!
   * Creates a new number.
   */
  HNumber();
  
  /*!
   * Destroys this number.
   */
  ~HNumber();
  
  /*!
   * Copies from another number.
   */
  HNumber( const HNumber& );
  
  /*!
   * Assigns from another number.
   */
  HNumber& operator=( const HNumber& );

  /*!
   * Creates a new number from an integer value.
   */
  HNumber( int i );
  
  /*!
   * Creates a new number from a string.
   */
  HNumber( const char* );
  
  /*!
   * Returns true if this number is Not a Number (NaN).
   */
  bool isNan() const;
  
  /*!
   * Returns true if this number is zero.
   */
  bool isZero() const;
  
  /*!
   * Returns true if this number is more than zero.
   */
  bool isPositive() const;
  
  /*!
   * Returns true if this number is less than zero.
   */
  bool isNegative() const;

  /*!
   * Adds another number.
   */
  HNumber operator+( const HNumber& ) const;
  
  /*!
   * Adds another number.
   */
  HNumber& operator+=( const HNumber& );

  /*!
   * Subtract from another number.
   */
  HNumber operator-( const HNumber& ) const;

  /*!
   * Subtract from another number.
   */
  HNumber& operator-=( const HNumber& );

  /*!
   * Multiplies with another number.
   */
  HNumber operator*( const HNumber& ) const;

  /*!
   * Multiplies with another number.
   */
  HNumber& operator*=( const HNumber& );

  /*!
   * Divides with another number.
   */
  HNumber operator/( const HNumber& ) const;

  /*!
   * Divides with another number.
   */
  HNumber& operator/=( const HNumber& );
 
  /*!
   * Returns true if this number is greater than n.
   */
  bool operator>( const HNumber& n ) const;

  /*!
   * Returns true if this number is less than n.
   */
  bool operator<( const HNumber& n ) const;

  /*!
   * Returns true if this number is greater than or equal to n.
   */
  bool operator>=( const HNumber& n ) const;
  
  /*!
   * Returns true if this number is less than or equal to n.
   */
  bool operator<=( const HNumber& n ) const;

  /*!
   * Returns true if this number is equal to n.
   */
  bool operator==( const HNumber& n ) const;

  /*!
   * Returns true if this number is not equal to n.
   */
  bool operator!=( const HNumber& n ) const;
  
  /*!
   * Returns a NaN (Not a Number).
   */
  static HNumber nan();

private:
  HNumberPrivate* d;  
};

class HMath
{
public:

  /*!
   * Formats the given number as string, using specified decimal digits.
   * Note that the returned string must be freed.
   */
  static char* format( const HNumber&n, char format = 'g', int prec = -1 );  

  /*!
   * Formats the given number as string, using specified decimal digits.
   * Note that the returned string must be freed.
   */
  static char* formatFixed( const HNumber&n, int prec = -1 );  

  /*!
   * Formats the given number as string, in exponential format.
   * Note that the returned string must be freed.
   */
  static char* formatExp( const HNumber&n, int prec = -1 );  

  /*!
   * Formats the given number as string, using specified decimal digits.
   * Note that the returned string must be freed.
   */
  static char* formatGeneral( const HNumber&n, int prec = -1 );  

  /*!
   * Returns the constant pi.
   */
  static HNumber pi();
  
  /*!
   * Adds two numbers.
   */
  static HNumber add( const HNumber& n1, const HNumber& n2 );
  
  /*!
   * Subtracts two numbers.
   */
  static HNumber sub( const HNumber& n1, const HNumber& n2 );
  
  /*!
   * Multiplies two numbers.
   */
  static HNumber mul( const HNumber& n1, const HNumber& n2 );
  
  /*!
   * Divides two numbers.
   */
  static HNumber div( const HNumber& n1, const HNumber& n2 );
  
  /*!
   * Returns -1, 0, 1 if n1 is less than, equal to, or more than n2.
   */
  static int compare( const HNumber& n1, const HNumber& n2 );
  
  /*!
   * Returns the absolute value of n.
   */
  static HNumber abs( const HNumber& n );
  
  /*!
   * Returns the negative of n.
   */
  static HNumber negate( const HNumber& n );
  
  /*!
   * Returns the integer part of n.
   */
  static HNumber integer( const HNumber& n );
  
  /*!
   * Returns the fraction part of n.
   */
  static HNumber frac( const HNumber& n );
  
  /*!
   * Rounds n to the specified decimal digits.
   */
  static HNumber round( const HNumber&n, int prec = 0 );
    
  /*!
   * Returns the square root of n. If n is negative, returns NaN.
   */
  static HNumber sqrt( const HNumber& n );
  
  /*!
   * Raises n1 to an integer n.
   */
  static HNumber raise( const HNumber& n1, int n );

  /*!
   * Raises n1 to n2.
   */
  static HNumber raise( const HNumber& n1, const HNumber& n2 );

  /*!
   * Returns e raised to x.
   */
  static HNumber exp( const HNumber& x );
  
  /*!
   * Returns the natural logarithm of x.
   * If x is non positive, returns NaN.
   */
  static HNumber ln( const HNumber& x );
  
  /*!
   * Returns the base-10 logarithm of x.
   * If x is non positive, returns NaN.
   */
  static HNumber log( const HNumber& x );
  
  /*!
   * Returns the sine of x. Note that x must be in radians.
   */
  static HNumber sin( const HNumber& x );

  /*!
   * Returns the cosine of x. Note that x must be in radians.
   */
  static HNumber cos( const HNumber& x );

  /*!
   * Returns the tangent of x. Note that x must be in radians.
   */
  static HNumber tan( const HNumber& x );

  /*!
   * Returns the arc sine of x.
   */
  static HNumber asin( const HNumber& x );
  
  /*!
   * Returns the arc cosine of x.
   */
  static HNumber acos( const HNumber& x );
  
  /*!
   * Returns the arc tangent of x.
   */
  static HNumber atan( const HNumber& x );
  
  /*!
   * Returns the hyperbolic sine of x. Note that x must be in radians.
   */
  static HNumber sinh( const HNumber& x );

  /*!
   * Returns the hyperbolic cosine of x. Note that x must be in radians.
   */
  static HNumber cosh( const HNumber& x );

  /*!
   * Returns the hyperbolic tangent of x. Note that x must be in radians.
   */
  static HNumber tanh( const HNumber& x );

   /*!
   * Returns the sign of x.
   */
  static HNumber sign( const HNumber& x );
   
   /*!
   * Returns the binomial coefficient of n and k.
   */
  static HNumber nCr( const HNumber& n,  const HNumber& k);

   /*!
   * Returns the factorial of x.
   * If x is non integer, returns NaN.
   */
  static HNumber factorial( const HNumber& x, const HNumber& base = HNumber(1) );


  /*!
   * Releases all resources. After calling this function, you can not use
   * any other functions as well as class HNumber. 
   */
  static void finalize();
  
};

std::ostream& operator<<( std::ostream& s, HNumber );

#endif // HMATH_H
