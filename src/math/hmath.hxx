/* HMath: C++ high precision math routines
   Copyright (C) 2004 Ariya Hidayat <ariya.hidayat@gmail.com>
                 2007 Helder Correia <helder.pereira.correia@gmail.com>

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

   Modified and extended by Wolf Lammen
 */

#ifndef HMATH_H
#define HMATH_H

#include <iostream>
#include "math/floatconfig.h"

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
   * Returns true if this number is integer.
   */
  bool isInteger() const;

   /*!
   * Returns the preferred format (base/precision), default is 0
   */
   char format() const;

   /*!
   * Sets the preferred format (base/precision), default is 0
   */
   void setFormat(char c = 0) const;

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
   * Modulo (rest of integer division)
   */
  HNumber operator%( const HNumber& ) const;

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
   * Bitwise ANDs the integral parts of both operands.
   * Yields NaN, if any operand exeeds the logic range
   */
  HNumber operator&( const HNumber& ) const;

  /*!
   * Bitwise ANDs the integral parts of both operands.
   * Yields NaN, if any operand exeeds the logic range
   */
  HNumber& operator&=( const HNumber& );

  /*!
   * Bitwise ORs the integral parts of both operands.
   * Yields NaN, if any operand exeeds the logic range
   */
  HNumber operator|( const HNumber& ) const;

  /*!
   * Bitwise ORs the integral parts of both operands.
   * Yields NaN, if any operand exeeds the logic range
   */
  HNumber& operator|=( const HNumber& );

  /*!
   * Bitwise XORs the integral parts of both operands.
   * Yields NaN, if any operand exeeds the logic range
   */
  HNumber operator^( const HNumber& ) const;

  /*!
   * Bitwise XORs the integral parts of both operands.
   * Yields NaN, if any operand exeeds the logic range
   */
  HNumber& operator^=( const HNumber& );

  /*!
   * Bitwise NOTs the integral part *this.
   * Yields NaN, if *this exeeds the logic range
   */
  HNumber operator~() const;

  /*!
   * Changes the sign
   */
  HNumber operator-() const;

  /*!
   * Shifts the integral part of <*this> to the right by
   * the parameters value's bits. The most significand
   * bit is duplicated to the left, shifted out bits are dropped
   * (signed or arithmethic shift right).
   * Yields NaN, if the operand exeeds the logic range,
   * or the shift count is not a non-negative integer.
   */
  HNumber operator>>( const HNumber& ) const;

  /*!
   * Shifts the integral part of <*this> to the left by
   * the parameters value's bits. Zeros are shifted in
   * to the right, shifted out bits are dropped.
   * Yields NaN, if the operand exeeds the logic range,
   * or the shift count is not a non-negative integer.
   */
  HNumber operator<<( const HNumber& ) const;

  /*!
   * Returns a NaN (Not a Number).
   */
  static HNumber nan();

  /*!
   * Returns the number as an int.
   * It is meant to convert small (integer) numbers only and no
   * checking is done whatsoever.
   */
  int toInt();

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
   * Formats the given number as string, in scientific format.
   * Note that the returned string must be freed.
   */
  static char* formatScientific( const HNumber&n, int prec = -1 );

  /*!
   * Formats the given number as string, in engineering notation.
   * Note that the returned string must be freed.
   */
  static char* formatEngineering( const HNumber&n, int prec = -1 );

  /*!
   * Formats the given number as string, using specified decimal digits.
   * Note that the returned string must be freed.
   */
  static char* formatGeneral( const HNumber&n, int prec = -1 );

   /*!
   * Formats the given number as string, using hexadecimal digits. (NaN if hn isn't integer)
   * Note that the returned string must be freed.
   */
  static char* formatHexadec( const HNumber&n );

   /*!
   * Formats the given number as string, using octal digits. (NaN if hn isn't integer)
   * Note that the returned string must be freed.
   */
  static char* formatOctal( const HNumber&n );

   /*!
   * Formats the given number as string, using binary digits. (NaN if hn isn't integer)
   * Note that the returned string must be freed.
   */
  static char* formatBinary( const HNumber&n );

  /*!
   * Returns the constant phi (golden number).
   */
  static HNumber phi();

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
   * Performs an integer divide
   */
  static HNumber idiv( const HNumber& n1, const HNumber& n2 );

  /*!
   * Returns -1, 0, 1 if n1 is less than, equal to, or more than n2.
   */
  static int compare( const HNumber& n1, const HNumber& n2 );

  /*!
   * Returns the maximum of two numbers.
   */
  static HNumber max( const HNumber& n1, const HNumber& n2 );

  /*!
   * Returns the minimum of two numbers.
   */
  static HNumber min( const HNumber& n1, const HNumber& n2 );

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
   * Returns the floor of n.
   */
  static HNumber floor( const HNumber& n );

  /*!
   * Returns the ceiling of n.
   */
  static HNumber ceil( const HNumber& n );

  /*!
   * Returns the greatest common divisor of n1 and n2.
   */
  static HNumber gcd( const HNumber& n1, const HNumber& n2 );

  /*!
   * Rounds n to the specified decimal digits.
   */
  static HNumber round( const HNumber&n, int prec = 0 );

  /*!
   * Truncates n to the specified decimal digits.
   */
  static HNumber trunc( const HNumber&n, int prec = 0 );

  /*!
   * Returns the square root of n. If n is negative, returns NaN.
   */
  static HNumber sqrt( const HNumber& n );

  /*!
   * Returns the cube root of n.
   */
  static HNumber cbrt( const HNumber& n );

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
   * Returns the base-2 logarithm of x.
   * If x is non positive, returns NaN.
   */
  static HNumber lg( const HNumber& x );

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
   * Returns the cotangent of x. Note that x must be in radians.
   */
  static HNumber cot( const HNumber& x );

  /*!
   * Returns the secant of x. Note that x must be in radians.
   */
  static HNumber sec( const HNumber& x );

  /*!
   * Returns the cosecant of x. Note that x must be in radians.
   */
  static HNumber csc( const HNumber& x );

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
   * Returns the hyperbolic sine of x.
   */
  static HNumber sinh( const HNumber& x );

  /*!
   * Returns the hyperbolic cosine of x.
   */
  static HNumber cosh( const HNumber& x );

  /*!
   * Returns the hyperbolic tangent of x.
   */
  static HNumber tanh( const HNumber& x );

  /*!
   * Returns the area hyperbolic sine of x.
   */
  static HNumber arsinh( const HNumber& x );

  /*!
   * Returns the area hyperbolic cosine of x.
   */
  static HNumber arcosh( const HNumber& x );

  /*!
   * Returns the area hyperbolic tangent of x.
   */
  static HNumber artanh( const HNumber& x );

   /*!
   * Returns the sign of x.
   */
  static HNumber sign( const HNumber& x );

   /*!
   * Returns the binomial coefficient of n and k.
   */
  static HNumber nCr( const HNumber& n, const HNumber& k );

   /*!
   * Returns the permutation of n elements chosen r elements.
   */
  static HNumber nPr( const HNumber& n, const HNumber& r );

   /*!
   * Returns the falling Pochhammer symbol x*(x-1)*..*base.
   * For base == 1, this is the usual factorial x!, what this
   * function is named after.
   * This function has been extended using the Gamma function,
   * so that actually Gamma(x+1)/Gamma(base) is computed, a
   * value that equals the falling Pochhammer symbol, when
   * x - base is an integer, but allows other differences as well.
   */
  static HNumber factorial( const HNumber& x, const HNumber& base = HNumber(1) );

   /*!
   * Returns the Gamma function.
   */
  static HNumber Gamma( const HNumber& x);

   /*!
   * Returns ln(abs(Gamma(x))).
   */
  static HNumber lnGamma( const HNumber& x);

  /*
                                   PROBABILITY
                                                                              */
  /**
   * Calculates the binomial discrete distribution probability mass function:
   * \f[X{\sim}B(n,p)\f]
   * \f[\Pr(X=k|n,p)={n\choose k}p^{k}(1-p)^{n-k}\f]
   *
   * \param[in] k the number of probed exact successes
   * \param[in] n the number of trials
   * \param[in] p the probability of success in a single trial
   *
   * \return the probability of exactly \p k successes, otherwise \p NaN if the
   * function is not defined for the specified parameters.
   */
  static HNumber binomialPmf( const HNumber & k,
                              const HNumber & n,
                              const HNumber & p );

  /**
   * Calculates the binomial cumulative distribution function:
   * \f[X{\sim}B(n,p)\f]
   * \f[\Pr(X \leq k|n,p)=\sum_{i=0}^{k}{n\choose i}p^{i}(1-p)^{n-i}\f]
   *
   * \param[in] k the maximum number of probed successes
   * \param[in] n the number of trials
   * \param[in] p the probability of success in a single trial
   *
   * \return the probability of up to \p k successes, otherwise \p NaN if the
   * function is not defined for the specified parameters.
   */
  static HNumber binomialCdf( const HNumber & k,
                              const HNumber & n,
                              const HNumber & p );

  /**
   * Calculates the expected value of a binomially distributed random variable:
   * \f[X{\sim}B(n,p)\f]
   * \f[E(X)=np\f]
   *
   * \param[in] n the number of trials
   * \param[in] p the probability of success in a single trial
   *
   * \return the expected value of the variable, otherwise \p NaN if the
   * function is not defined for the specified parameters.
   */
  static HNumber binomialMean( const HNumber & n,
                               const HNumber & p );

  /**
   * Calculates the variance of a binomially distributed random variable:
   * \f[X{\sim}B(n,p)\f]
   * \f[Var(X)=np(1-p)\f]
   *
   * \param[in] n the number of trials
   * \param[in] p the probability of success in a single trial
   *
   * \return the variance of the variable, otherwise \p NaN if the
   * function is not defined for the specified parameters.
   */
  static HNumber binomialVariance( const HNumber & n,
                                   const HNumber & p );

  /**
   * Calculates the hypergeometric discrete distribution probability mass
   * function:
   * \f[X{\sim}H(N,M,n)\f]
   * \f[\Pr(X=k|N,M,n)=\frac{{M\choose k}{N-M\choose n-k}}{{N\choose n}}\f]
   *
   * \param[in] k the number of probed exact successes
   * \param[in] N the number of total elements
   * \param[in] M the number of success elements
   * \param[in] n the number of selected elements
   *
   * \return the probability of exactly \p k successes, otherwise \p NaN if the
   * function is not defined for the specified parameters.
   */
  static HNumber hypergeometricPmf( const HNumber & k,
                                    const HNumber & N,
                                    const HNumber & M,
                                    const HNumber & n );

  /**
   * Calculates the hypergeometric cumulative distribution function:
   * \f[X{\sim}H(N,M,n)\f]
   * \f[\Pr(X\leq k|N,M,n)=
   *   \sum_{i=0}^{k}\frac{{M\choose k}{N-M\choose n-k}}{{N\choose n}}\f]
   *
   * \param[in] k the maximum number of probed successes
   * \param[in] N the number of total elements
   * \param[in] M the number of success elements
   * \param[in] n the number of selected elements
   *
   * \return the probability of up to \p k successes, otherwise \p NaN if the
   * function is not defined for the specified parameters.
   */
  static HNumber hypergeometricCdf( const HNumber & k,
                                    const HNumber & N,
                                    const HNumber & M,
                                    const HNumber & n );

  /**
   * Calculates the expected value of a hypergeometrically distributed random
   * variable:
   * \f[X{\sim}H(N,M,n)\f]
   * \f[E(X)=n\frac{M}{N}\f]
   *
   * \param[in] N the number of total elements
   * \param[in] M the number of success elements
   * \param[in] n the number of selected elements
   *
   * \return the expected value of the variable, otherwise \p NaN if the
   * function is not defined for the specified parameter.
   */
  static HNumber hypergeometricMean( const HNumber & N,
                                     const HNumber & M,
                                     const HNumber & n );

  /**
   * Calculates the variance of a hypergeometrically distributed random variable:
   * \f[X{\sim}H(N,M,n)\f]
   * \f[Var(X)=\frac{n\frac{M}{N}(1-\frac{M}{N})(N-n)}{N-1}\f]
   *
   * \param[in] N the number of total elements
   * \param[in] M the number of success elements
   * \param[in] n the number of selected elements
   *
   * \return the variance of the variable, otherwise \p NaN if the function is
   * not defined for the specified parameter.
   */
  static HNumber hypergeometricVariance( const HNumber & N,
                                         const HNumber & M,
                                         const HNumber & n );

  /**
   * Calculates the poissonian discrete distribution probability mass function:
   * \f[X{\sim}P(\lambda)\f]
   * \f[\Pr(X=k|\lambda)=\frac{e^{-\lambda}\lambda^k}{k!}\f]
   *
   * \param[in] k the number of event occurrences
   * \param[in] l the expected number of occurrences that occur in an interval
   *
   * \return the probability of exactly \p k event occurrences, otherwise \p NaN
   * if the function is not defined for the specified parameters.
   */
  static HNumber poissonPmf( const HNumber & k,
                             const HNumber & l );

  /**
   * Calculates the poissonian cumulative distribution function:
   * \f[X{\sim}P(\lambda)\f]
   * \f[\Pr(X\leq k|\lambda)=\sum_{i=0}^{k}\frac{e^{-\lambda}\lambda^k}{k!}\f]
   *
   * \param[in] k the maximum number of event occurrences
   * \param[in] l the expected number of occurrences that occur in an interval
   *
   * \return the probability of up to \p k event occurrences, otherwise \p NaN
   * if the function is not defined for the specified parameters.
   */
  static HNumber poissonCdf( const HNumber & k,
                             const HNumber & l );

  /**
   * Calculates the expected value of a Poisson distributed random variable:
   * \f[X{\sim}P(\lambda)\f]
   * \f[E(X)=\lambda\f]
   *
   * \param[in] l the expected number of occurrences that occur in an interval
   *
   * \return the expected value of the variable, otherwise \p NaN if the
   * function is not defined for the specified parameter.
   */
  static HNumber poissonMean( const HNumber & l );

  /**
   * Calculates the variance of a Poisson distributed random variable:
   * \f[X{\sim}P(\lambda)\f]
   * \f[Var(X)=\lambda\f]
   *
   * \param[in] l the expected number of occurrences that occur in an interval
   *
   * \return the variance of the variable, otherwise \p NaN if the function is
   * not defined for the specified parameter.
   */
  static HNumber poissonVariance( const HNumber & l );

  /**
   * Restricts a logic value to a given bit size
   */
  static HNumber mask ( const HNumber & val, const HNumber & bits);

  /**
   * sign-extends an unsigned value
   */
  static HNumber sgnext ( const HNumber & val, const HNumber & bits);

  /**
   * for bits >= 0 does an arithmetic shift right
   * for bits < 0 a shift left
   */
  static HNumber ashr( const HNumber & val, const HNumber & bits );

  /*!
   * Releases all resources. After calling this function, you can not use
   * any other functions as well as class HNumber.
   */
  static void finalize();
};

std::ostream& operator<<( std::ostream& s, const HNumber& n );

#endif // HMATH_H
