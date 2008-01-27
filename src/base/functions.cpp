// This file is part of the SpeedCrunch project
// Copyright (C) 2004-2006 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2007 Wolf Lammen
// Copyright (C) 2007-2008 Helder Correia <helder.pereira.correia@gmail.com>
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


#include "functions.hxx"

#include "math/hmath.hxx"

#include <QApplication>
#include <QHash>
#include <QObject>
#include <QStringList>
#include <QVector>

#include <float.h>
#include <math.h>


HNumber checkRad2Deg( HNumber angle )
{
  return HMath::angleMode() == 'd' ? HMath::rad2deg( angle ) : angle;
}


HNumber checkDeg2Rad( HNumber angle )
{
  return HMath::angleMode() == 'r' ? HMath::deg2rad( angle ) : angle;
}


HNumber function_abs( const Evaluator *, Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::abs( num );
}


HNumber function_int( const Evaluator *, Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::integer( num );
}


HNumber function_trunc( const Evaluator         * e,
                        Function                * f,
                        const QVector<HNumber> & args )
{
  int nArgs = args.count();

  if( nArgs != 1 && nArgs != 2 )
  {
    f->setError( f->name(), QApplication::translate( "functions",
      "function requires 1 or 2 arguments" ) );
    return HNumber::nan();
  }

  HNumber num = args[0];

  if( nArgs == 2){

    int prec = 0;

    HNumber argprec = args[1];
    if (argprec != 0)
    {
      if( !argprec.isInteger() )
      {
        f->setError( f->name(), QApplication::translate( "functions",
                              "function undefined for specified arguments" ) );
        return HNumber::nan();
      }
      if ((prec = argprec.toInt()) != 0)
        return HMath::trunc( num, prec );
      // the 2. parameter exceeds the integer limits
      if (argprec < 0)
        return HNumber(0);
      return num;
    }
  }
  return HMath::trunc( num );
}


HNumber function_frac( const Evaluator         * e,
                       Function                * f,
                       const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];
  return HMath::frac( x );
}


HNumber function_floor( const Evaluator*, Function*, const QVector<HNumber>& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::floor( num );
}


HNumber function_ceil( const Evaluator*, Function*, const QVector<HNumber>& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::ceil( num );
}


HNumber function_gcd( const Evaluator*, Function* fn, const QVector<HNumber>& args )
{
  int nArgs = args.count();

  if ( nArgs < 2 )
  {
    fn->setError( fn->name(), QApplication::translate( "functions",
                    "function requires at least 2 arguments" ) );
    return HNumber::nan();
  }

  for ( int i = 0; i < args.count(); i++ )
    if ( !args[i].isInteger() )
    {
      fn->setError( fn->name(), QApplication::translate( "functions",
                      "function requires integer arguments" ) );
      return HNumber::nan();
    }

  HNumber result = HMath::gcd( args[0], args[1] );
  for ( int i = 2; i < nArgs; i++ )
  {
    result = HMath::gcd( result, args[i] );
  }
  return result;
}


HNumber function_round( const Evaluator*, Function* fn, const QVector<HNumber>& args )
{
  int nArgs = args.count();

  if( nArgs != 1 && nArgs != 2 )
  {
    fn->setError( fn->name(), QApplication::translate( "functions",
                    "function requires 1 or 2 arguments" ) );
    return HNumber::nan();
  }

  HNumber num = args[0];

  if( nArgs == 2){

    int prec = 0;

    HNumber argprec = args[1];
    if (argprec != 0)
    {
      if( !argprec.isInteger() )
      {
        fn->setError( fn->name(), QApplication::translate( "functions",
                        "function undefined for specified arguments" ) );
        return HNumber::nan();
      }
      if ((prec = argprec.toInt()) != 0)
        return HMath::round( num, prec );
      // the 2. parameter exceeds the integer limits
      if (argprec < 0)
        return HNumber(0);
      return num;
    }
  }
  return HMath::round( num );
}


HNumber function_sqrt( const Evaluator*, Function* fn, const QVector<HNumber>& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  if( num < HNumber(0) )
  {
    fn->setError( fn->name(), QApplication::translate( "functions",
                    "function undefined for specified argument" ) );
    return HNumber::nan();
  }

  return HMath::sqrt( num );
}


HNumber function_cbrt( const Evaluator*, Function*, const QVector<HNumber>& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::cbrt( num );
}


HNumber function_exp( const Evaluator*, Function*, const QVector<HNumber>& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::exp( num );
}


HNumber function_ln( const Evaluator*, Function* fn, const QVector<HNumber>& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];
  HNumber result = HMath::ln( x );

  if( result.isNan() )
    fn->setError( fn->name(), QApplication::translate( "functions",
                    "function undefined for specified argument" ) );

  return result;
}


HNumber function_log( const Evaluator*, Function* fn, const QVector<HNumber>& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];
  HNumber result = HMath::log( x );

  if( result.isNan() )
    fn->setError( fn->name(), QApplication::translate( "functions",
                    "function undefined for specified argument" ) );

  return result;
}


HNumber function_lg( const Evaluator*, Function* fn, const QVector<HNumber>& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];
  HNumber result = HMath::lg( x );

  if( result.isNan() )
    fn->setError( fn->name(), QApplication::translate( "functions",
                    "function undefined for specified argument" ) );

  return result;
}


HNumber function_sin( const Evaluator* eval, Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];

  return HMath::sin( checkDeg2Rad( angle ) );
}


HNumber function_cos( const Evaluator * eval, Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];

  return HMath::cos( checkDeg2Rad( angle ) );
}


HNumber function_tan( const Evaluator * eval, Function * fn, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];

  HNumber result = HMath::tan( checkDeg2Rad(angle) );
  if ( result.isNan() )
  {
    fn->setError( fn->name(), QApplication::translate( "functions",
                    "function undefined for specified argument" ) );
    return HNumber::nan();
  }

  return result;
}


HNumber function_cot( const Evaluator * eval, Function * fn, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  HNumber result = HMath::cot( checkDeg2Rad( angle ) );
  if ( result.isNan() )
  {
    fn->setError( fn->name(), QApplication::translate( "functions",
                    "function undefined for specified argument" ) );
    return HNumber::nan();
  }

  return result;
}


HNumber function_sec( const Evaluator * eval, Function * fn, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  HNumber result = HMath::sec( checkDeg2Rad(angle) );
  if ( result.isNan() )
  {
    fn->setError( fn->name(), QApplication::translate( "functions",
                    "function undefined for specified argument" ) );
    return HNumber::nan();
  }

  return result;
}


HNumber function_csc( const Evaluator * eval, Function * fn, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  HNumber result = HMath::csc( checkDeg2Rad(angle) );
  if ( result.isNan() )
  {
    fn->setError( fn->name(), QApplication::translate( "functions",
                    "function undefined for specified argument" ) );
    return HNumber::nan();
  }

  return result;
}


HNumber function_asin( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];

  HNumber result = HMath::asin( x );

  if ( result.isNan() )
  {
    f->setError( f->name(), QApplication::translate( "functions",
                          "function undefined for specified argument" ) );
    return HNumber::nan();
  }

  return checkRad2Deg(result);
}


HNumber function_acos( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];

  HNumber result = HMath::acos( x );

  if ( result.isNan() )
  {
    f->setError( f->name(), QApplication::translate( "functions",
                        "function undefined for specified argument" ) );
    return HNumber::nan();
  }

  return checkRad2Deg( result );
}


HNumber function_atan( const Evaluator * eval, Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  HNumber angle = HMath::atan( num );
  return checkRad2Deg(angle);
}


HNumber function_sinh( const Evaluator * eval, Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];
  return HMath::sinh( x );
}


HNumber function_cosh( const Evaluator * eval, Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];
  return HMath::cosh( x );
}


HNumber function_tanh( const Evaluator * eval, Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];
  return HMath::tanh( x );
}


HNumber function_arsinh( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  return HMath::arsinh( args[0] );
}


HNumber function_arcosh( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];
  HNumber result = HMath::arcosh( x );

  if ( result.isNan() )
  {
    f->setError( f->name(), QApplication::translate( "functions",
                          "function undefined for specified argument" ) );
    return HNumber::nan();
  }

  return result;
}


HNumber function_artanh( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];
  HNumber result = HMath::artanh( x );

  if ( result.isNan() )
  {
    f->setError( f->name(), QApplication::translate( "functions",
                          "function undefined for specified argument" ) );
    return HNumber::nan();
  }

  return result;
}


HNumber function_erf( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];
  return HMath::erf( x );
}


HNumber function_erfc( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];
  HNumber result = HMath::erfc( x );

  if ( result.isNan() && !x.isNan() )
  {
    f->setError( f->name(), QApplication::translate( "functions",
                          "underflow" ) );
  }

  return result;
}


HNumber function_Gamma( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];
  HNumber result = HMath::gamma( x );

  if ( result.isNan() )
  {
    f->setError( f->name(), QApplication::translate( "functions",
                          "function undefined for specified argument" ) );
    return HNumber::nan();
  }

  return result;
}


HNumber function_lnGamma( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];
  HNumber result = HMath::lnGamma( x );

  if ( result.isNan() )
  {
    f->setError( f->name(), QApplication::translate( "functions",
                          "function undefined for specified argument" ) );
    return HNumber::nan();
  }

  return result;
}


HNumber function_sign( const Evaluator *, Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  return HMath::sign( args[0] );
}


HNumber function_nCr( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 2 )
    return HNumber::nan();

  // n = args[0]; r = args[1]
  HNumber result = HMath::nCr( args[0], args[1] );

  // FIX ME: overflow causes a NaN, too, so the message is sometimes
  // misleading
  if ( result.isNan() )
    f->setError( f->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}


HNumber function_nPr( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 2 )
    return HNumber::nan();

  // n = args[0]; r = args[1]
  HNumber result = HMath::nPr( args[0], args[1] );

  // FIX ME: overflow causes a NaN, too, so the message is sometimes
  // misleading
  if ( result.isNan() )
    f->setError( f->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}


HNumber function_degrees( const Evaluator *, Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  return HMath::rad2deg(angle);
}


HNumber function_radians( const Evaluator *, Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  return HMath::deg2rad(angle);
}


HNumber function_max( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() < 1 )
  {
    f->setError( f->name(), QApplication::translate( "functions",
                          "function requires at least 1 argument" ) );
    return HNumber::nan();
  }

  int totalParams = args.count();
  HNumber result = args[0];
  if ( totalParams > 1 )
    for ( int i = 1; i < totalParams; i++ )
      result = HMath::max( result, args[i] );

  return result;
}


HNumber function_min( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() < 1 )
  {
    f->setError( f->name(), QApplication::translate( "functions",
                          "function requires at least 1 argument" ) );
    return HNumber::nan();
  }

  int totalParams = args.count();
  HNumber result = args[0];
  if ( totalParams > 1 )
    for ( int i = 1; i < totalParams; i++ )
      result = HMath::min( result, args[i] );

  return result;
}


HNumber function_sum( const Evaluator *, Function *, const QVector<HNumber> & args )
{
  if ( args.count() <= 0 )
    return HNumber(0);

  HNumber result = args[0];
  for ( int c = 1; c < args.count(); c++ )
    result = result + args[c];

  return result;
}


HNumber function_product( const Evaluator *, Function *, const QVector<HNumber> & args )
{
  if ( args.count() <= 0 )
    return HNumber(0);

  HNumber result = args[0];
  for ( int c = 1; c < args.count(); c++ )
    result = result * args[c];

  return result;
}


HNumber function_average( const Evaluator *, Function *, const QVector<HNumber> & args )
{
  if ( args.count() <= 0 )
    return HNumber("NaN");

  HNumber result = args[0];
  for ( int c = 1; c < args.count(); c++ )
    result = result + args[c];
  result = result / HNumber(args.count());

  return result;
}


HNumber function_geomean( const Evaluator *, Function *, const QVector<HNumber> & args )
{
  if ( args.count() <= 0 )
    return HNumber("NaN");

  HNumber result = args[0];

  for ( int c = 1; c < args.count(); c++ )
    result = result * args[c];

  if ( result <= HNumber(0))
    return HNumber("NaN");

  if ( args.count() == 1 )
    return result;

  if ( args.count() == 2 )
    return HMath::sqrt( result );

  result = HMath::exp( HMath::ln(result) / HNumber(args.count()) );

  return result;
}


HNumber function_dec( const Evaluator *, Function *, const QVector<HNumber> & args )
{
  if ( args.count() < 1 )
    return HNumber("NaN");

  HNumber result = args[0];
  result.setFormat( 'g' );
  return result;
}


HNumber function_hex( const Evaluator *, Function *, const QVector<HNumber> & args )
{
  if ( args.count() < 1 )
    return HNumber("NaN");

  HNumber result = args[0];
  result.setFormat( 'h' );
  return result;
}


HNumber function_oct( const Evaluator *, Function *, const QVector<HNumber> & args )
{
  if( args.count() < 1 )
    return HNumber("NaN");

  HNumber result = args[0];
  result.setFormat( 'o' );
  return result;
}


HNumber function_bin( const Evaluator *, Function *, const QVector<HNumber> & args )
{
  if ( args.count() < 1 )
    return HNumber("NaN");

  HNumber result = args[0];
  result.setFormat( 'b' );
  return result;
}


HNumber function_binompmf( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 3 )
    return HNumber::nan();

  HNumber k = args[0];
  HNumber n = args[1];
  HNumber p = args[2];
  HNumber result = HMath::binomialPmf( k, n, p );

  if ( result.isNan() )
    f->setError( f->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}


HNumber function_binomcdf( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 3 )
    return HNumber::nan();

  HNumber k = args[0];
  HNumber n = args[1];
  HNumber p = args[2];
  HNumber result = HMath::binomialCdf( k, n, p );

  if ( result.isNan() )
    f->setError( f->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}


HNumber function_binommean( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 2 )
    return HNumber::nan();

  HNumber n = args[0];
  HNumber p = args[1];
  HNumber result = HMath::binomialMean( n, p );

  if ( result.isNan() )
    f->setError( f->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}


HNumber function_binomvar( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 2 )
    return HNumber::nan();

  HNumber n = args[0];
  HNumber p = args[1];
  HNumber result = HMath::binomialVariance( n, p );

  if ( result.isNan() )
    f->setError( f->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}


HNumber function_hyperpmf( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 4 )
    return HNumber::nan();

  HNumber k = args[0];
  HNumber N = args[1];
  HNumber M = args[2];
  HNumber n = args[3];
  HNumber result = HMath::hypergeometricPmf( k, N, M, n );

  if ( result.isNan() )
    f->setError( f->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}


HNumber function_hypercdf( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 4 )
    return HNumber::nan();

  HNumber k = args[0];
  HNumber N = args[1];
  HNumber M = args[2];
  HNumber n = args[3];
  HNumber result = HMath::hypergeometricCdf( k, N, M, n );

  if ( result.isNan() )
    f->setError( f->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}


HNumber function_hypermean( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 3 )
    return HNumber::nan();

  HNumber N = args[0];
  HNumber M = args[1];
  HNumber n = args[2];
  HNumber result = HMath::hypergeometricMean( N, M, n );

  if ( result.isNan() )
    f->setError( f->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}


HNumber function_hypervar( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 3 )
    return HNumber::nan();

  HNumber N = args[0];
  HNumber M = args[1];
  HNumber n = args[2];
  HNumber result = HMath::hypergeometricVariance( N, M, n );

  if ( result.isNan() )
    f->setError( f->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}


HNumber function_poipmf( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 2 )
    return HNumber::nan();

  HNumber k = args[0];
  HNumber l = args[1];
  HNumber result = HMath::poissonPmf( k, l );

  if ( result.isNan() )
    f->setError( f->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}


HNumber function_poicdf( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 2 )
    return HNumber::nan();

  HNumber k = args[0];
  HNumber l = args[1];
  HNumber result = HMath::poissonCdf( k, l );

  if ( result.isNan() )
    f->setError( f->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}


HNumber function_poimean( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber l = args[0];
  HNumber result = HMath::poissonMean( l );

  if ( result.isNan() )
    f->setError( f->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}


HNumber function_poivar( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber l = args[0];
  HNumber result = HMath::poissonVariance( l );

  if ( result.isNan() )
    f->setError( f->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}


HNumber function_mask( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  HNumber val = args[0];
  HNumber bits = args[1];
  return HMath::mask( val, bits );
}


HNumber function_unmask( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  HNumber val = args[0];
  HNumber bits = args[1];
  return HMath::sgnext( val, bits );
}


HNumber function_not( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  HNumber val = args[0];
  return ~val;
}


HNumber function_and( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() <= 0 )
    return HNumber("NaN");

  HNumber result = args[0];
  for( int c = 1; c < args.count(); c++ )
    result = result & args[c];

  return result;
}


HNumber function_or( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() <= 0 )
    return HNumber("NaN");

  HNumber result = args[0];
  for ( int c = 1; c < args.count(); c++ )
    result = result | args[c];

  return result;
}


HNumber function_xor( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  if ( args.count() <= 0 )
    return HNumber("NaN");

  HNumber result = args[0];
  for ( int c = 1; c < args.count(); c++ )
    result = result ^ args[c];

  return result;
}


HNumber function_ashl( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  HNumber bits = args[1];
  HNumber val = args[0];

  return HMath::ashr( val, -bits );
}


HNumber function_ashr( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  HNumber bits = args[1];
  HNumber val = args[0];

  return HMath::ashr(val, bits);
}


HNumber function_idiv( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  HNumber dividend = args[0];
  HNumber divisor = args[1];

  return HNumber::idiv(dividend, divisor);
}


HNumber function_mod( const Evaluator * e, Function * f, const QVector<HNumber> & args )
{
  HNumber dividend = args[0];
  HNumber divisor = args[1];

  return dividend % divisor;
}


class FunctionPrivate
{
  public:
    int         argc;
    QString     desc;
    QString     error;
    QString     name;
    FunctionPtr ptr;

    FunctionPrivate() : argc( 0 ), desc(), error(), name(), ptr( 0 ) {}
};


class FunctionsPrivate
{
  public:
    QHash<QString, Function*> functions;
};


// public (Function)

Function::Function( const QString & name, int argc, FunctionPtr ptr, const QString & desc )
  : d( new FunctionPrivate )
{
  d->name = name;
  d->argc = argc;
  d->desc = QApplication::translate( "Functions", desc.toLatin1() );
  d->ptr = ptr;
}


Function::Function( const QString& name, FunctionPtr ptr, const QString& desc )
  : d( new FunctionPrivate )
{
  d->name = name;
  d->argc = -1;
  d->desc = QApplication::translate( "Functions", desc.toLatin1() );
  d->ptr = ptr;
}


QString Function::description() const
{
  return d->desc;
}


QString Function::error() const
{
  return d->error;
}


QString Function::name() const
{
  return d->name;
}


HNumber Function::exec( const Evaluator * eval, const QVector<HNumber> & args )
{
  d->error = QString();
  if ( ! d->ptr )
  {
    setError( QString("error"), QString( QApplication::translate( "functions",
                "cannot execute function %1") ).arg( name() ) );
    return HNumber(0);
  }

  if ( d->argc >= 0 )
  if ( args.count() != d->argc )
  {
    if ( d->argc == 1 )
      setError( d->name, QString( QApplication::translate( "functions",
                  "function accepts 1 argument" ) ) );
    else
      setError( d->name, QString( QApplication::translate( "functions",
                  "function accepts %1 arguments" ) ).arg( d->argc ) );

    return HNumber(0);
  }

  return (*d->ptr)( eval, this, args );
}


void Function::setError( const QString & context, const QString & error )
{
  d->error = context + ": " + error;
}


Function::~Function()
{
  delete d;
}


Functions * Functions::s_self = 0;


// public (Functions)

Functions::Functions()
{
  d = new FunctionsPrivate;

  // ANALYSIS
  add( new Function( "abs",     1, function_abs,     QT_TR_NOOP("Absolute Value")                     ) );
  add( new Function( "average",    function_average, QT_TR_NOOP("Average (Arithmetic Mean)")          ) );
  add( new Function( "bin",        function_bin,     QT_TR_NOOP("Binary Representation")              ) );
  add( new Function( "cbrt",    1, function_cbrt,    QT_TR_NOOP("Cube Root")                          ) );
  add( new Function( "ceil",    1, function_ceil,    QT_TR_NOOP("Ceiling")                            ) );
  add( new Function( "dec",        function_dec,     QT_TR_NOOP("Decimal Representation")             ) );
  add( new Function( "floor",   1, function_floor,   QT_TR_NOOP("Floor")                              ) );
  add( new Function( "frac",    1, function_frac,    QT_TR_NOOP("Fractional Part")                    ) );
  add( new Function( "gamma",      function_Gamma,   QT_TR_NOOP("Extension of Factorials [= (x-1)!]") ) );
  add( new Function( "geomean",    function_geomean, QT_TR_NOOP("Geometric Mean")                     ) );
  add( new Function( "hex",        function_hex,     QT_TR_NOOP("Hexadecimal Representation")         ) );
  add( new Function( "int",     1, function_int,     QT_TR_NOOP("Integer Part")                       ) );
  add( new Function( "lngamma",    function_lnGamma, QT_TR_NOOP("ln(abs(Gamma))")                     ) );
  add( new Function( "max",        function_max,     QT_TR_NOOP("Maximum")                            ) );
  add( new Function( "min",        function_min,     QT_TR_NOOP("Minimum")                            ) );
  add( new Function( "oct",        function_oct,     QT_TR_NOOP("Octal Representation")               ) );
  add( new Function( "product",    function_product, QT_TR_NOOP("Product")                            ) );
  add( new Function( "round",      function_round,   QT_TR_NOOP("Rounding")                           ) );
  add( new Function( "sign",    1, function_sign,    QT_TR_NOOP("Signum")                             ) );
  add( new Function( "sqrt",    1, function_sqrt,    QT_TR_NOOP("Square Root")                        ) );
  add( new Function( "sum",        function_sum,     QT_TR_NOOP("Sum")                                ) );
  add( new Function( "trunc",      function_trunc,   QT_TR_NOOP("Truncation")                         ) );

  // LOGARITHM
  add( new Function( "arcosh", 1, function_arcosh, QT_TR_NOOP("Area Hyperbolic Cosine")  ) );
  add( new Function( "arsinh", 1, function_arsinh, QT_TR_NOOP("Area Hyperbolic Sine")    ) );
  add( new Function( "artanh", 1, function_artanh, QT_TR_NOOP("Area Hyperbolic Tangent") ) );
  add( new Function( "cosh",   1, function_cosh,   QT_TR_NOOP("Hyperbolic Cosine")       ) );
  add( new Function( "exp",    1, function_exp,    QT_TR_NOOP("Exponential")             ) );
  add( new Function( "lg",     1, function_lg,     QT_TR_NOOP("Base-2 Logarithm")        ) );
  add( new Function( "ln",     1, function_ln,     QT_TR_NOOP("Natural Logarithm")       ) );
  add( new Function( "log",    1, function_log,    QT_TR_NOOP("Base-10 Logarithm")       ) );
  add( new Function( "sinh",   1, function_sinh,   QT_TR_NOOP("Hyperbolic Sine")         ) );
  add( new Function( "tanh",   1, function_tanh,   QT_TR_NOOP("Hyperbolic Tangent")      ) );

  // DISCRETE
  add( new Function( "gcd",    function_gcd, QT_TR_NOOP("Greatest Common Divisor")            ) );
  add( new Function( "ncr", 2, function_nCr, QT_TR_NOOP("Combination (Binomial Coefficient)") ) );
  add( new Function( "npr", 2, function_nPr, QT_TR_NOOP("Permutation (Arrangement)")          ) );

  // PROBABILITY
  add( new Function( "binomcdf",  3, function_binomcdf,  QT_TR_NOOP("Binomial Cumulative Distribution Function")       ) );
  add( new Function( "binommean", 2, function_binommean, QT_TR_NOOP("Binomial Distribution Mean")                      ) );
  add( new Function( "binompmf",  3, function_binompmf,  QT_TR_NOOP("Binomial Probability Mass Function")              ) );
  add( new Function( "binomvar",  2, function_binomvar,  QT_TR_NOOP("Binomial Distribution Variance")                  ) );
  add( new Function( "erf",       1, function_erf,       QT_TR_NOOP("Error Function")                                  ) );
  add( new Function( "erfc",      1, function_erfc,      QT_TR_NOOP("Complementary Error Function")                    ) );
  add( new Function( "hypercdf",  4, function_hypercdf,  QT_TR_NOOP("Hypergeometric Cumulative Distribution Function") ) );
  add( new Function( "hypermean", 3, function_hypermean, QT_TR_NOOP("Hypergeometric Distribution Mean")                ) );
  add( new Function( "hyperpmf",  4, function_hyperpmf,  QT_TR_NOOP("Hypergeometric Probability Mass Function")        ) );
  add( new Function( "hypervar",  3, function_hypervar,  QT_TR_NOOP("Hypergeometric Distribution Variance")            ) );
  add( new Function( "poicdf",    2, function_poicdf,    QT_TR_NOOP("Poissonian Cumulative Distribution Function")     ) );
  add( new Function( "poimean",   1, function_poimean,   QT_TR_NOOP("Poissonian Distribution Mean")                    ) );
  add( new Function( "poipmf",    2, function_poipmf,    QT_TR_NOOP("Poissonian Probability Mass Function")            ) );
  add( new Function( "poivar",    1, function_poivar,    QT_TR_NOOP("Poissonian Distribution Variance")                ) );

  // TRIGONOMETRY
  add( new Function( "acos",    1, function_acos,    QT_TR_NOOP("Arc Cosine")     ) );
  add( new Function( "asin",    1, function_asin,    QT_TR_NOOP("Arc Sine")       ) );
  add( new Function( "atan",    1, function_atan,    QT_TR_NOOP("Arc Tangent")    ) );
  add( new Function( "cos",     1, function_cos,     QT_TR_NOOP("Cosine")         ) );
  add( new Function( "cot",     1, function_cot,     QT_TR_NOOP("Cotangent")      ) );
  add( new Function( "csc",     1, function_csc,     QT_TR_NOOP("Cosecant")       ) );
  add( new Function( "degrees", 1, function_degrees, QT_TR_NOOP("Degrees of Arc") ) );
  add( new Function( "radians", 1, function_radians, QT_TR_NOOP("Radians")        ) );
  add( new Function( "sec",     1, function_sec,     QT_TR_NOOP("Secant")         ) );
  add( new Function( "sin",     1, function_sin,     QT_TR_NOOP("Sine")           ) );
  add( new Function( "tan",     1, function_tan,     QT_TR_NOOP("Tangent")        ) );

  // LOGIC
  add( new Function( "mask",   2, function_mask,   QT_TR_NOOP("Mask to a bit size")     ) );
  add( new Function( "unmask", 2, function_unmask, QT_TR_NOOP("Sign-extent a value")    ) );
  add( new Function( "not",    1, function_not,    QT_TR_NOOP("Logical NOT")            ) );
  add( new Function( "and",       function_and,    QT_TR_NOOP("Logical AND")            ) );
  add( new Function( "or",        function_or,     QT_TR_NOOP("Logical OR")             ) );
  add( new Function( "xor",       function_xor,    QT_TR_NOOP("Logical XOR")            ) );
  add( new Function( "shl",   2,  function_ashl,   QT_TR_NOOP("Arithmetic Shift Left")  ) );
  add( new Function( "shr",   2,  function_ashr,   QT_TR_NOOP("Arithmetic Shift Right") ) );
  add( new Function( "idiv",  2,  function_idiv,   QT_TR_NOOP("Integer Quotient")       ) );
  add( new Function( "mod",   2,  function_mod,    QT_TR_NOOP("Modulo")                 ) );
}


Functions * Functions::self()
{
  if ( ! s_self )
    s_self = new Functions();
  return s_self;
}


void Functions::add( Function * f )
{
  if ( ! f )
    return;

  d->functions.insert( f->name().toUpper(), f );
}


Function * Functions::function( const QString & name )
{
  return d->functions.value( name.toUpper() );
}


QStringList Functions::functionNames() const
{
  QStringList result;
  QHashIterator<QString, Function *> it( d->functions );
  while ( it.hasNext() )
  {
    it.next();
    result.append( it.key().toLower() );
  }

  return result;
}


Functions::~Functions()
{
  while ( d->functions.size() > 0 )
  {
    delete d->functions[ 0 ];
    d->functions.remove( 0 );
  }

  delete d;
}
