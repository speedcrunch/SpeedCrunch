/* This file is part of the SpeedCrunch project
   Copyright (C) 2004-2006 Ariya Hidayat <ariya@kde.org>
                 2007 Helder Correia <helder.pereira.correia@gmail.com>
   last update 2007-11-20 Wolf Lammen

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

#include <base/evaluator.hxx>
#include <base/functions.hxx>
#include <math/hmath.hxx>

#include <QApplication>
#include <QHash>
#include <QObject>
#include <QStringList>
#include <QVector>

#include <float.h>
#include <math.h>

HNumber deg2rad( HNumber x )
{
  return x * HMath::pi() / HNumber(180);
}

HNumber rad2deg( HNumber x )
{
  return HNumber(180) * x / HMath::pi();
}

HNumber function_abs( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::abs( num );
}

HNumber function_int( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::integer( num );
}

HNumber function_trunc( const Evaluator         * evaluator,
                        Function                * function,
                        const FunctionArguments & arguments )
{
  int nArgs = arguments.count();

  if( nArgs != 1 && nArgs != 2 )
  {
    function->setError( function->name(), QApplication::translate( "functions",
      "function requires 1 or 2 arguments" ) );
    return HNumber::nan();
  }

  HNumber num = arguments[0];

  if( nArgs == 2){

    int prec = 0;

    HNumber argprec = arguments[1];
    if (argprec != 0)
    {
      if( !argprec.isInteger() )
      {
        function->setError( function->name(), QApplication::translate( "functions",
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

HNumber function_frac( const Evaluator         * evaluator,
                       Function                * function,
                       const FunctionArguments & arguments )
{
  if ( arguments.count() != 1 )
    return HNumber::nan();

  HNumber x = arguments[0];
  return HMath::frac( x );
}

HNumber function_floor( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::floor( num );
}

HNumber function_ceil( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::ceil( num );
}

HNumber function_gcd( const Evaluator*, Function* fn, const FunctionArguments& args )
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

HNumber function_round( const Evaluator*, Function* fn, const FunctionArguments& args )
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

HNumber function_sqrt( const Evaluator*, Function* fn, const FunctionArguments& args )
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

HNumber function_cbrt( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::cbrt( num );
}

HNumber function_exp( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::exp( num );
}

HNumber function_ln( const Evaluator*, Function* fn, const FunctionArguments& args )
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

HNumber function_log( const Evaluator*, Function* fn, const FunctionArguments& args )
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

HNumber function_lg( const Evaluator*, Function* fn, const FunctionArguments& args )
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

HNumber function_sin( const Evaluator* eval, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  if( eval->angleMode() == Evaluator::Degree )
    angle = deg2rad( angle );

  return HMath::sin( angle );
}

HNumber function_cos( const Evaluator* eval, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  if( eval->angleMode() == Evaluator::Degree )
    angle = deg2rad( angle );

  return HMath::cos( angle );
}

HNumber function_tan( const Evaluator* eval, Function* fn, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  if( eval->angleMode() == Evaluator::Degree )
    angle = deg2rad( angle );

  HNumber result = HMath::tan( angle );
  if ( result.isNan() )
  {
    fn->setError( fn->name(), QApplication::translate( "functions",
      "function undefined for specified argument" ) );
    return HNumber::nan();
  }

  return result;
}

HNumber function_cot( const Evaluator* eval, Function* fn, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  if( eval->angleMode() == Evaluator::Degree )
    angle = deg2rad( angle );

  HNumber result = HMath::cot( angle );
  if ( result.isNan() )
  {
    fn->setError( fn->name(), QApplication::translate( "functions",
      "function undefined for specified argument" ) );
    return HNumber::nan();
  }

  return result;
}

HNumber function_sec( const Evaluator* eval, Function* fn, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  if( eval->angleMode() == Evaluator::Degree )
    angle = deg2rad( angle );

  HNumber result = HMath::sec( angle );
  if ( result.isNan() )
  {
    fn->setError( fn->name(), QApplication::translate( "functions",
      "function undefined for specified argument" ) );
    return HNumber::nan();
  }

  return result;
}

HNumber function_csc( const Evaluator* eval, Function* fn, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  if( eval->angleMode() == Evaluator::Degree )
    angle = deg2rad( angle );

  HNumber result = HMath::csc( angle );
  if ( result.isNan() )
  {
    fn->setError( fn->name(), QApplication::translate( "functions",
      "function undefined for specified argument" ) );
    return HNumber::nan();
  }

  return result;
}

HNumber function_asin( const Evaluator         * evaluator,
                             Function          * function,
                       const FunctionArguments & arguments )
{
  if ( arguments.count() != 1 )
    return HNumber::nan();

  HNumber x = arguments[0];

  HNumber result = HMath::asin( x );

  if ( result.isNan() )
  {
    function->setError( function->name(), QApplication::translate( "functions",
                        "function undefined for specified argument" ) );
    return HNumber::nan();
  }

  if ( evaluator->angleMode() == Evaluator::Degree )
    result = rad2deg( result );

  return result;
}

HNumber function_acos( const Evaluator         * evaluator,
                             Function          * function,
                       const FunctionArguments & arguments )
{
  if ( arguments.count() != 1 )
    return HNumber::nan();

  HNumber x = arguments[0];

  HNumber result = HMath::acos( x );

  if ( result.isNan() )
  {
    function->setError( function->name(), QApplication::translate( "functions",
                        "function undefined for specified argument" ) );
    return HNumber::nan();
  }

  if ( evaluator->angleMode() == Evaluator::Degree )
    result = rad2deg( result );

  return result;
}

HNumber function_atan( const Evaluator* eval, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  HNumber angle = HMath::atan( num );
  if( eval->angleMode() == Evaluator::Degree )
    angle = rad2deg( angle );

  return angle;
}

HNumber function_sinh( const Evaluator* eval, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];
  return HMath::sinh( x );
}

HNumber function_cosh( const Evaluator* eval, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];
  return HMath::cosh( x );
}

HNumber function_tanh( const Evaluator* eval, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];
  return HMath::tanh( x );
}

HNumber function_arsinh( const Evaluator         * evaluator,
                               Function          * function,
                         const FunctionArguments & arguments )
{
  if ( arguments.count() != 1 )
    return HNumber::nan();

  return HMath::arsinh( arguments[0] );
}

HNumber function_arcosh( const Evaluator         * evaluator,
                               Function          * function,
                         const FunctionArguments & arguments )
{
  if ( arguments.count() != 1 )
    return HNumber::nan();

  HNumber x = arguments[0];

  HNumber result = HMath::arcosh( x );

  if ( result.isNan() )
  {
    function->setError( function->name(), QApplication::translate( "functions",
                        "function undefined for specified argument" ) );
    return HNumber::nan();
  }

  return result;
}

HNumber function_artanh( const Evaluator         * evaluator,
                               Function          * function,
                         const FunctionArguments & arguments )
{
  if ( arguments.count() != 1 )
    return HNumber::nan();

  HNumber x = arguments[0];

  HNumber result = HMath::artanh( x );

  if ( result.isNan() )
  {
    function->setError( function->name(), QApplication::translate( "functions",
                        "function undefined for specified argument" ) );
    return HNumber::nan();
  }

  return result;
}

HNumber function_erf( const Evaluator         * evaluator,
                      Function                * function,
                      const FunctionArguments & arguments )
{
  if ( arguments.count() != 1 )
    return HNumber::nan();

  HNumber x = arguments[0];

  HNumber result = HMath::erf( x );

  if ( result.isNan() )
  {
    function->setError( function->name(), QApplication::translate( "functions",
                        "function undefined for specified argument" ) );
    return HNumber::nan();
  }

  return result;
}

HNumber function_erfc( const Evaluator         * evaluator,
                       Function                * function,
                       const FunctionArguments & arguments )
{
  if ( arguments.count() != 1 )
    return HNumber::nan();

  HNumber x = arguments[0];

  HNumber result = HMath::erfc( x );

  if ( result.isNan() )
  {
    function->setError( function->name(), QApplication::translate( "functions",
                        "function undefined for specified argument" ) );
    return HNumber::nan();
  }

  return result;
}

HNumber function_sign( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();
  return HMath::sign( args[0] );
}

HNumber function_nCr( const Evaluator         * evaluator,
                            Function          * function,
                      const FunctionArguments & arguments )
{
  // check number of arguments
  if ( arguments.count() != 2 )
    return HNumber::nan();

  // compute result
  // n = arguments[0]
  // r = arguments[1]
  HNumber result = HMath::nCr( arguments[0], arguments[1] );

  // check invalid usage and set error accordingly
  // FIX ME: overflow causes a NaN, too, so the message is sometimes
  // misleading
  if ( result.isNan() )
    function->setError( function->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber function_nPr( const Evaluator         * evaluator,
                            Function          * function,
                      const FunctionArguments & arguments )
{
  // check number of arguments
  if ( arguments.count() != 2 )
    return HNumber::nan();

  // compute result
  // n = arguments[0]
  // r = arguments[1]
  HNumber result = HMath::nPr( arguments[0], arguments[1] );

  // check invalid usage and set error accordingly
  // FIX ME: overflow causes a NaN, too, so the message is sometimes
  // misleading
  if ( result.isNan() )
    function->setError( function->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber function_degrees( const Evaluator*, Function*, const FunctionArguments& args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  return rad2deg(angle);
}

HNumber function_radians( const Evaluator*, Function*, const FunctionArguments& args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  return deg2rad(angle);
}

HNumber function_max( const Evaluator         * evaluator,
                      Function                * function,
                      const FunctionArguments & arguments )
{
  if ( arguments.count() < 1 )
  {
    function->setError( function->name(), QApplication::translate( "functions",
                          "function requires at least 1 argument" ) );
    return HNumber::nan();
  }

  int totalParams = arguments.count();
  HNumber result = arguments[0];
  if(totalParams > 1)
    for ( int i = 1; i < totalParams; i++ )
      result = HMath::max( result, arguments[i] );

  return result;
}

HNumber function_min( const Evaluator         * evaluator,
                      Function                * function,
                      const FunctionArguments & arguments )
{
  if ( arguments.count() < 1 )
  {
    function->setError( function->name(), QApplication::translate( "functions",
                          "function requires at least 1 argument" ) );
    return HNumber::nan();
  }

  int totalParams = arguments.count();
  HNumber result = arguments[0];
  if(totalParams > 1)
    for ( int i = 1; i < totalParams; i++ )
      result = HMath::min( result, arguments[i] );

  return result;
}

HNumber function_sum( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() <= 0 )
    return HNumber(0);

  HNumber result = args[0];
  for( int c = 1; c < args.count(); c++ )
    result = result + args[c];

  return result;
}

HNumber function_product( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() <= 0 )
    return HNumber(0);

  HNumber result = args[0];
  for( int c = 1; c < args.count(); c++ )
    result = result * args[c];

  return result;
}

HNumber function_average( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() <= 0 )
    return HNumber("NaN");

  HNumber result = args[0];
  for( int c = 1; c < args.count(); c++ )
    result = result + args[c];

  result = result / HNumber(args.count());

  return result;
}

HNumber function_geomean( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() <= 0 )
    return HNumber("NaN");

  HNumber result = args[0];

  for( int c = 1; c < args.count(); c++ )
    result = result * args[c];

  if( result <= HNumber(0))
    return HNumber("NaN");

  if (args.count() == 1)
    return result;

  if (args.count() == 2)
    return HMath::sqrt(result);

  result = HMath::exp(HMath::ln(result) / HNumber(args.count()));

  return result;
}

HNumber function_dec( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() < 1 )
    return HNumber("NaN");
  HNumber result = args[0];
  result.setFormat('g');
  return result;
}

HNumber function_hex( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() < 1 )
    return HNumber("NaN");
  HNumber result = args[0];
  result.setFormat('h');
  return result;
}

HNumber function_oct( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() < 1 )
    return HNumber("NaN");
  HNumber result = args[0];
  result.setFormat('o');
  return result;
}

HNumber function_bin( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() < 1 )
    return HNumber("NaN");
  HNumber result = args[0];
  result.setFormat('b');
  return result;
}

HNumber function_binompmf( const Evaluator         * evaluator,
                           Function                * function,
                           const FunctionArguments & arguments )
{
  if ( arguments.count() != 3 )
    return HNumber::nan();

  HNumber k = arguments[0];
  HNumber n = arguments[1];
  HNumber p = arguments[2];
  HNumber result = HMath::binomialPmf( k, n, p );

  if ( result.isNan() )
    function->setError( function->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber function_binomcdf( const Evaluator         * evaluator,
                                 Function          * function,
                           const FunctionArguments & arguments )
{
  if ( arguments.count() != 3 )
    return HNumber::nan();

  HNumber k = arguments[0];
  HNumber n = arguments[1];
  HNumber p = arguments[2];
  HNumber result = HMath::binomialCdf( k, n, p );

  if ( result.isNan() )
    function->setError( function->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber function_binommean( const Evaluator         * evaluator,
                            Function                * function,
                            const FunctionArguments & arguments )
{
  if ( arguments.count() != 2 )
    return HNumber::nan();

  HNumber n = arguments[0];
  HNumber p = arguments[1];
  HNumber result = HMath::binomialMean( n, p );

  if ( result.isNan() )
    function->setError( function->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber function_binomvar( const Evaluator         * evaluator,
                           Function                * function,
                           const FunctionArguments & arguments )
{
  if ( arguments.count() != 2 )
    return HNumber::nan();

  HNumber n = arguments[0];
  HNumber p = arguments[1];
  HNumber result = HMath::binomialVariance( n, p );

  if ( result.isNan() )
    function->setError( function->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber function_hyperpmf( const Evaluator         * evaluator,
                           Function                * function,
                           const FunctionArguments & arguments )
{
  if ( arguments.count() != 4 )
    return HNumber::nan();

  HNumber k = arguments[0];
  HNumber N = arguments[1];
  HNumber M = arguments[2];
  HNumber n = arguments[3];
  HNumber result = HMath::hypergeometricPmf( k, N, M, n );

  if ( result.isNan() )
    function->setError( function->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber function_hypercdf( const Evaluator         * evaluator,
                           Function                * function,
                           const FunctionArguments & arguments )
{
  if ( arguments.count() != 4 )
    return HNumber::nan();

  HNumber k = arguments[0];
  HNumber N = arguments[1];
  HNumber M = arguments[2];
  HNumber n = arguments[3];
  HNumber result = HMath::hypergeometricCdf( k, N, M, n );

  if ( result.isNan() )
    function->setError( function->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber function_hypermean( const Evaluator         * evaluator,
                            Function                * function,
                            const FunctionArguments & arguments )
{
  if ( arguments.count() != 3 )
    return HNumber::nan();

  HNumber N = arguments[0];
  HNumber M = arguments[1];
  HNumber n = arguments[2];
  HNumber result = HMath::hypergeometricMean( N, M, n );

  if ( result.isNan() )
    function->setError( function->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber function_hypervar( const Evaluator         * evaluator,
                           Function                * function,
                           const FunctionArguments & arguments )
{
  if ( arguments.count() != 3 )
    return HNumber::nan();

  HNumber N = arguments[0];
  HNumber M = arguments[1];
  HNumber n = arguments[2];
  HNumber result = HMath::hypergeometricVariance( N, M, n );

  if ( result.isNan() )
    function->setError( function->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber function_poipmf( const Evaluator         * evaluator,
                         Function                * function,
                         const FunctionArguments & arguments )
{
  if ( arguments.count() != 2 )
    return HNumber::nan();

  HNumber k = arguments[0];
  HNumber l = arguments[1];
  HNumber result = HMath::poissonPmf( k, l );

  if ( result.isNan() )
    function->setError( function->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber function_poicdf( const Evaluator         * evaluator,
                         Function                * function,
                         const FunctionArguments & arguments )
{
  if ( arguments.count() != 2 )
    return HNumber::nan();

  HNumber k = arguments[0];
  HNumber l = arguments[1];
  HNumber result = HMath::poissonCdf( k, l );

  if ( result.isNan() )
    function->setError( function->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber function_poimean( const Evaluator         * evaluator,
                          Function                * function,
                          const FunctionArguments & arguments )
{
  if ( arguments.count() != 1 )
    return HNumber::nan();

  HNumber l = arguments[0];
  HNumber result = HMath::poissonMean( l );

  if ( result.isNan() )
    function->setError( function->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber function_poivar( const Evaluator         * evaluator,
                         Function                * function,
                         const FunctionArguments & arguments )
{
  if ( arguments.count() != 1 )
    return HNumber::nan();

  HNumber l = arguments[0];
  HNumber result = HMath::poissonVariance( l );

  if ( result.isNan() )
    function->setError( function->name(), QApplication::translate( "functions",
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber function_mask( const Evaluator         * evaluator,
                       Function                * function,
                       const FunctionArguments & arguments )
{
  HNumber val = arguments[0];
  HNumber bits = arguments[1];
  return HMath::mask(val, bits);
}

HNumber function_unmask( const Evaluator         * evaluator,
                         Function                * function,
                         const FunctionArguments & arguments )
{
  HNumber val = arguments[0];
  HNumber bits = arguments[1];
  return HMath::sgnext(val, bits);
}

HNumber function_not( const Evaluator         * evaluator,
                      Function                * function,
                      const FunctionArguments & arguments )
{
  HNumber val = arguments[0];
  return ~val;
}

HNumber function_and( const Evaluator         * evaluator,
                      Function                * function,
                      const FunctionArguments & args )
{
  if( args.count() <= 0 )
    return HNumber("NaN");

  HNumber result = args[0];

  for( int c = 1; c < args.count(); c++ )
    result = result & args[c];

  return result;
}

HNumber function_or( const Evaluator         * evaluator,
                     Function                * function,
                     const FunctionArguments & args )
{
  if( args.count() <= 0 )
    return HNumber("NaN");

  HNumber result = args[0];

  for( int c = 1; c < args.count(); c++ )
    result = result | args[c];

  return result;
}

HNumber function_xor( const Evaluator         * evaluator,
                      Function                * function,
                      const FunctionArguments & args )
{
  if( args.count() <= 0 )
    return HNumber("NaN");

  HNumber result = args[0];

  for( int c = 1; c < args.count(); c++ )
    result = result ^ args[c];

  return result;
}

HNumber function_ashl( const Evaluator         * evaluator,
                       Function                * function,
                       const FunctionArguments & args )
{
  HNumber bits = args[1];
  HNumber val = args[0];

  return HMath::ashr(val, -bits);
}

HNumber function_ashr( const Evaluator         * evaluator,
                       Function                * function,
                       const FunctionArguments & args )
{
  HNumber bits = args[1];
  HNumber val = args[0];

  return HMath::ashr(val, bits);
}

HNumber function_idiv( const Evaluator         * evaluator,
                       Function                * function,
                       const FunctionArguments & args )
{
  HNumber dividend = args[0];
  HNumber divisor = args[1];

  return HMath::idiv(dividend, divisor);
}

HNumber function_mod( const Evaluator         * evaluator,
                      Function                * function,
                      const FunctionArguments & args )
{
  HNumber dividend = args[0];
  HNumber divisor = args[1];

  return dividend % divisor;
}

class FunctionPrivate
{
public:
  QString name;
  int argc;
  QString desc;
  QString error;
  FunctionPtr ptr;

  FunctionPrivate(): name(), argc(0), desc(), error(), ptr(0) {}
};

class FunctionRepositoryPrivate
{
public:
  QHash<QString, Function*> functions;
};

Function::Function( const QString& name, int argc, FunctionPtr ptr, const QString& desc ):
d( new FunctionPrivate )
{
  d->name = name;
  d->argc = argc;
  d->desc = QApplication::translate( "FunctionRepository", desc.toLatin1() );
  d->ptr = ptr;
}

Function::Function( const QString& name, FunctionPtr ptr, const QString& desc ):
d( new FunctionPrivate )
{
  d->name = name;
  d->argc = -1;
  d->desc = QApplication::translate( "FunctionRepository", desc.toLatin1() );
  d->ptr = ptr;
}

Function::~Function()
{
  delete d;
}

QString Function::name() const
{
  return d->name;
}

QString Function::description() const
{
  return d->desc;
}

QString Function::error() const
{
  return d->error;
}

void Function::setError( const QString& context, const QString& error )
{
  d->error = context + ": " + error;
}

HNumber Function::exec( const Evaluator* eval, const FunctionArguments& args )
{
  d->error = QString();
  if( !d->ptr )
  {
    setError( QString("error"), QString( QApplication::translate( "functions",
      "cannot execute function %1") ).arg( name() ) );
    return HNumber(0);
  }

  if( d->argc >= 0 )
  if( args.count() != d->argc )
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

FunctionRepository* FunctionRepository::s_self = 0;

FunctionRepository* FunctionRepository::self()
{
  if( !s_self )
    s_self = new FunctionRepository();
  return s_self;
}

FunctionRepository::FunctionRepository()
{
  d = new FunctionRepositoryPrivate;

  /*
                                     ANALYSIS
                                                                              */
  add( new Function( "abs",     1, function_abs,
                     QT_TR_NOOP("Absolute Value")             ) );
  add( new Function( "average",    function_average,
                     QT_TR_NOOP("Average (Arithmetic Mean)")  ) );
  add( new Function( "bin",        function_bin,
                     QT_TR_NOOP("Binary Representation")      ) );
  add( new Function( "ceil",    1, function_ceil,
                     QT_TR_NOOP("Ceiling")                    ) );
  add( new Function( "cbrt",    1, function_cbrt,
                     QT_TR_NOOP("Cube Root")                  ) );
  add( new Function( "dec",        function_dec,
                     QT_TR_NOOP("Decimal Representation")     ) );
  add( new Function( "floor",   1, function_floor,
                     QT_TR_NOOP("Floor")                      ) );
  add( new Function( "frac",    1, function_frac,
                     QT_TR_NOOP("Fractional Part")            ) );
  add( new Function( "geomean",    function_geomean,
                     QT_TR_NOOP("Geometric Mean")             ) );
  add( new Function( "hex",        function_hex,
                     QT_TR_NOOP("Hexadecimal Representation") ) );
  add( new Function( "int",     1, function_int,
                     QT_TR_NOOP("Integer Part")               ) );
  add( new Function( "max",        function_max,
                     QT_TR_NOOP("Maximum")                    ) );
  add( new Function( "min",        function_min,
                     QT_TR_NOOP("Minimum")                    ) );
  add( new Function( "oct",        function_oct,
                     QT_TR_NOOP("Octal Representation")       ) );
  add( new Function( "product",    function_product,
                     QT_TR_NOOP("Product")                    ) );
  add( new Function( "round",      function_round,
                     QT_TR_NOOP("Rounding")                   ) );
  add( new Function( "sign",    1, function_sign,
                     QT_TR_NOOP("Signum")                     ) );
  add( new Function( "sqrt",    1, function_sqrt,
                     QT_TR_NOOP("Square Root")                ) );
  add( new Function( "sum",        function_sum,
                     QT_TR_NOOP("Sum")                        ) );
  add( new Function( "trunc",      function_trunc,
                     QT_TR_NOOP("Truncation")                 ) );

  /*
                                     LOGARITHM
                                                                              */
  add( new Function( "log",     1, function_log,
       QT_TR_NOOP("Base-10 Logarithm")          ) );
  add( new Function( "lg",      1, function_lg,
       QT_TR_NOOP("Base-2 Logarithm")           ) );
  add( new Function( "ln",      1, function_ln,
       QT_TR_NOOP("Natural Logarithm")          ) );
  add( new Function( "exp",     1, function_exp,
       QT_TR_NOOP("Exponential")                ) );
  add( new Function( "arcosh",  1, function_arcosh,
       QT_TR_NOOP("Area Hyperbolic Cosine") ) );
  add( new Function( "arsinh",  1, function_arsinh,
       QT_TR_NOOP("Area Hyperbolic Sine") ) );
  add( new Function( "artanh",  1, function_artanh,
       QT_TR_NOOP("Area Hyperbolic Tangent") ) );
  add( new Function( "cosh",    1, function_cosh,
       QT_TR_NOOP("Hyperbolic Cosine")  ) );
  add( new Function( "sinh",    1, function_sinh,
       QT_TR_NOOP("Hyperbolic Sine")    ) );
  add( new Function( "tanh",    1, function_tanh,
       QT_TR_NOOP("Hyperbolic Tangent") ) );

  /*
                                     DISCRETE
                                                                              */
  add( new Function( "gcd",     function_gcd,
                     QT_TR_NOOP("Greatest Common Divisor")            ) );
  add( new Function( "ncr",  2, function_nCr,
                     QT_TR_NOOP("Combination (Binomial Coefficient)") ) );
  add( new Function( "npr",  2, function_nPr,
                     QT_TR_NOOP("Permutation (Arrangement)")          ) );
  /*
                                    PROBABILITY
                                                                              */
  add( new Function( "binompmf",  3, function_binompmf,
                     QT_TR_NOOP("Binomial Probability Mass Function")        ));
  add( new Function( "binomcdf",  3, function_binomcdf,
                     QT_TR_NOOP("Binomial Cumulative Distribution Function") ));
  add( new Function( "binommean", 2, function_binommean,
                     QT_TR_NOOP("Binomial Distribution Mean")                ));
  add( new Function( "binomvar",  2, function_binomvar,
                     QT_TR_NOOP("Binomial Distribution Variance")            ));
  add( new Function( "hyperpmf",  4, function_hyperpmf,
                     QT_TR_NOOP("Hypergeometric Probability Mass Function")  ));
  add( new Function( "hypercdf",  4, function_hypercdf,
                QT_TR_NOOP("Hypergeometric Cumulative Distribution Function")));
  add( new Function( "hypermean", 3, function_hypermean,
                     QT_TR_NOOP("Hypergeometric Distribution Mean")          ));
  add( new Function( "hypervar",  3, function_hypervar,
                     QT_TR_NOOP("Hypergeometric Distribution Variance")      ));
  add( new Function( "poipmf",    2, function_poipmf,
                     QT_TR_NOOP("Poissonian Probability Mass Function")      ));
  add( new Function( "poicdf",    2, function_poicdf,
                     QT_TR_NOOP("Poissonian Cumulative Distribution Function")));
  add( new Function( "poimean",   1, function_poimean,
                     QT_TR_NOOP("Poissonian Distribution Mean")              ));
  add( new Function( "poivar",    1, function_poivar,
                     QT_TR_NOOP("Poissonian Distribution Variance")          ));
  add( new Function( "erf",    1, function_erf,
                     QT_TR_NOOP("Error Function")                            ));
  add( new Function( "erfc",    1, function_erfc,
                     QT_TR_NOOP("Complementary Error Function")              ));
  /*
                                   TRIGONOMETRY
                                                                              */
  add( new Function( "acos",    1, function_acos,
                     QT_TR_NOOP("Arc Cosine")         ) );
  add( new Function( "asin",    1, function_asin,
                     QT_TR_NOOP("Arc Sine")           ) );
  add( new Function( "atan",    1, function_atan,
                     QT_TR_NOOP("Arc Tangent")        ) );
  add( new Function( "csc",     1, function_csc,
                     QT_TR_NOOP("Cosecant")           ) );
  add( new Function( "cos",     1, function_cos,
                     QT_TR_NOOP("Cosine")             ) );
  add( new Function( "cot",     1, function_cot,
                     QT_TR_NOOP("Cotangent")          ) );
  add( new Function( "degrees", 1, function_degrees,
                     QT_TR_NOOP("Degrees Of Arc")     ) );
  add( new Function( "radians", 1, function_radians,
                     QT_TR_NOOP("Radians")            ) );
  add( new Function( "sec",     1, function_sec,
                     QT_TR_NOOP("Secant")             ) );
  add( new Function( "sin",     1, function_sin,
                     QT_TR_NOOP("Sine")               ) );
  add( new Function( "tan",     1, function_tan,
                     QT_TR_NOOP("Tangent")            ) );

  /*
                                    LOGIC
                                                                              */
  add( new Function( "mask",   2, function_mask,
                     QT_TR_NOOP("mask to a bit size") ) );
  add( new Function( "unmask",   2, function_unmask,
                     QT_TR_NOOP("sign-extent a value")) );
  add( new Function( "not",   1, function_not,
                     QT_TR_NOOP("logical not")        ) );
  add( new Function( "and",         function_and,
                     QT_TR_NOOP("logical and")        ) );
  add( new Function( "or",         function_or,
                     QT_TR_NOOP("logical or")         ) );
  add( new Function( "xor",         function_xor,
                     QT_TR_NOOP("logical exclusive or")) );
  add( new Function( "shl",   2, function_ashl,
                     QT_TR_NOOP("arithmetic shift left")) );
  add( new Function( "shr",   2, function_ashr,
                     QT_TR_NOOP("arithmetic shift right")) );
  add( new Function( "idiv",   2, function_idiv,
                     QT_TR_NOOP("integer quotient")    ) );
  add( new Function( "mod",   2, function_mod,
                     QT_TR_NOOP("modulo")   ) );
}

FunctionRepository::~FunctionRepository()
{
  while( d->functions.size() > 0 )
  {
    delete d->functions[ 0 ];
    d->functions.remove( 0 );
  }

  delete d;
}

void FunctionRepository::add( Function* function )
{
  if( !function ) return;
  d->functions.insert( function->name().toUpper(), function );
}

Function* FunctionRepository::function( const QString& name )
{
  return d->functions.value( name.toUpper() );
}

QStringList FunctionRepository::functionNames() const
{
  QStringList result;
  QHashIterator<QString, Function*> it( d->functions );
  while( it.hasNext() )
  {
    it.next();
    result.append( it.key().toLower() );
  }
  return result;
}
