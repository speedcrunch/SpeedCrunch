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

#include "core/functions.hxx"

#include "core/settings.hxx"

#include <QApplication>
#include <QHash>

#include <cfloat>
#include <cmath>
#include <numeric>

struct Function::Private
{
  Functions * functions;

  int         argc;
  QString     desc;
  QString     error;
  QString     name;
  FunctionPtr ptr;

  Private() : argc( 0 ), desc(), error(), name(), ptr( 0 ) {}
};

Function::Function( const QString & name, int argc, FunctionPtr ptr,
                    Functions * parent )
  : d( new Function::Private )
{
  d->functions = parent;
  d->name      = name;
  d->argc      = argc;
  d->ptr       = ptr;
}

Function::Function( const QString & name, FunctionPtr ptr, Functions * parent )
  : d( new Function::Private )
{
  d->functions = parent;
  d->name      = name;
  d->argc      = -1;
  d->ptr       = ptr;
}

QString Function::description() const
{
  return d->desc;
}

void Function::setDescription( const QString & description )
{
  d->desc = description;
}

QString Function::error() const
{
  return d->error;
}

QString Function::name() const
{
  return d->name;
}

HNumber Function::exec( const QVector<HNumber> & args )
{
  d->error = QString();
  if ( ! d->ptr )
  {
    setError( QString("error"), QString( Functions::tr(
                "cannot execute function %1") ).arg( name() ) );
    return HNumber(0);
  }

  if ( d->argc >= 0 && args.count() != d->argc )
  {
    if ( d->argc == 1 )
      setError( d->name, QString( Functions::tr(
                  "function accepts 1 argument" ) ) );
    else
      setError( d->name, QString( Functions::tr(
                  "function accepts %1 arguments" ) ).arg( d->argc ) );

    return HNumber(0);
  }

  return (*d->ptr)( this, args );
}

Functions * Function::functions() const
{
  return d->functions;
}


void Function::setError( const QString & context, const QString & error )
{
  d->error = context + ": " + error;
}

Function::~Function()
{
  delete d;
}

struct Functions::Private
{
  Functions * p;
  QHash<QString, Function *> functions;
  void createBuiltInFunctions();

  static HNumber abs( Function *, const QVector<HNumber> & args );
  static HNumber integer( Function *, const QVector<HNumber> & args );
  static HNumber trunc( Function *, const QVector<HNumber> & args );
  static HNumber frac( Function *, const QVector<HNumber> & args );
  static HNumber floor( Function *, const QVector<HNumber> & args );
  static HNumber ceil( Function *, const QVector<HNumber> & args );
  static HNumber gcd( Function *, const QVector<HNumber>& args );
  static HNumber round( Function *, const QVector<HNumber>& args );
  static HNumber sqrt( Function *, const QVector<HNumber> & args );
  static HNumber cbrt( Function *, const QVector<HNumber>& args );
  static HNumber exp( Function *, const QVector<HNumber>& args );
  static HNumber ln( Function *, const QVector<HNumber>& args );
  static HNumber log( Function *, const QVector<HNumber> & args );
  static HNumber lg( Function *, const QVector<HNumber>& args );
  static HNumber sin( Function *, const QVector<HNumber> & args );
  static HNumber cos( Function *, const QVector<HNumber> & args );
  static HNumber tan( Function *, const QVector<HNumber> & args );
  static HNumber cot( Function *, const QVector<HNumber> & args );
  static HNumber sec( Function *, const QVector<HNumber> & args );
  static HNumber csc( Function *, const QVector<HNumber> & args );
  static HNumber asin( Function *, const QVector<HNumber> & args );
  static HNumber acos( Function *, const QVector<HNumber> & args );
  static HNumber atan( Function *, const QVector<HNumber> & args );
  static HNumber sinh( Function *, const QVector<HNumber> & args );
  static HNumber cosh( Function *, const QVector<HNumber> & args );
  static HNumber tanh( Function *, const QVector<HNumber> & args );
  static HNumber arsinh( Function *, const QVector<HNumber> & args );
  static HNumber arcosh( Function *, const QVector<HNumber> & args );
  static HNumber artanh( Function *, const QVector<HNumber> & args );
  static HNumber erf( Function *, const QVector<HNumber> & args );
  static HNumber erfc( Function *, const QVector<HNumber> & args );
  static HNumber Gamma( Function *, const QVector<HNumber> & args );
  static HNumber lnGamma( Function *, const QVector<HNumber> & args );
  static HNumber sign( Function *, const QVector<HNumber> & args );
  static HNumber nCr( Function *, const QVector<HNumber> & args );
  static HNumber nPr( Function *, const QVector<HNumber> & args );
  static HNumber degrees( Function *, const QVector<HNumber> & args );
  static HNumber radians( Function *, const QVector<HNumber> & args );
  static HNumber max( Function *, const QVector<HNumber> & args );
  static HNumber min( Function *, const QVector<HNumber> & args );
  static HNumber sum( Function *, const QVector<HNumber> & args );
  static HNumber product( Function *, const QVector<HNumber> & args );
  static HNumber average( Function *, const QVector<HNumber> & args );
  static HNumber geomean( Function *, const QVector<HNumber> & args );
  static HNumber dec( Function *, const QVector<HNumber> & args );
  static HNumber hex( Function *, const QVector<HNumber> & args );
  static HNumber oct( Function *, const QVector<HNumber> & args );
  static HNumber bin( Function *, const QVector<HNumber> & args );
  static HNumber binompmf( Function *, const QVector<HNumber> & args );
  static HNumber binomcdf( Function *, const QVector<HNumber> & args );
  static HNumber binommean( Function *, const QVector<HNumber> & args );
  static HNumber binomvar( Function *, const QVector<HNumber> & args );
  static HNumber hyperpmf( Function *, const QVector<HNumber> & args );
  static HNumber hypercdf( Function *, const QVector<HNumber> & args );
  static HNumber hypermean( Function *, const QVector<HNumber> & args );
  static HNumber hypervar( Function *, const QVector<HNumber> & args );
  static HNumber poipmf( Function *, const QVector<HNumber> & args );
  static HNumber poicdf( Function *, const QVector<HNumber> & args );
  static HNumber poimean( Function *, const QVector<HNumber> & args );
  static HNumber poivar( Function *, const QVector<HNumber> & args );
  static HNumber mask( Function *, const QVector<HNumber> & args );
  static HNumber unmask( Function *, const QVector<HNumber> & args );
  static HNumber not_( Function *, const QVector<HNumber> & args );
  static HNumber and_( Function *, const QVector<HNumber> & args );
  static HNumber or_( Function *, const QVector<HNumber> & args );
  static HNumber xor_( Function *, const QVector<HNumber> & args );
  static HNumber ashl( Function *, const QVector<HNumber> & args );
  static HNumber ashr( Function *, const QVector<HNumber> & args );
  static HNumber idiv( Function *, const QVector<HNumber> & args );
  static HNumber mod( Function *, const QVector<HNumber> & args );
};


HNumber Functions::Private::abs( Function *, const QVector<HNumber> & args )
{
  return args.count() != 1 ? HMath::nan() : HMath::abs( args[0] );
}


HNumber Functions::Private::integer( Function *, const QVector<HNumber> & args )
{
  return args.count() != 1 ? HMath::nan() : HMath::integer( args[0] );
}


HNumber Functions::Private::trunc( Function * f, const QVector<HNumber> & args )
{
  int nArgs = args.count();

  if ( nArgs != 1 && nArgs != 2 )
  {
    f->setError( f->name(), Functions::tr(
      "function requires 1 or 2 arguments" ) );
    return HMath::nan();
  }

  HNumber num = args[0];

  if ( nArgs == 2){

    int prec = 0;

    HNumber argprec = args[1];
    if ( argprec != 0 )
    {
      if ( ! argprec.isInteger() )
      {
        f->setError( f->name(), Functions::tr(
                              "function undefined for specified arguments" ) );
        return HMath::nan();
      }
      if ( (prec = argprec.toInt()) != 0 )
        return HMath::trunc( num, prec );
      // the 2. parameter exceeds the integer limits
      if ( argprec < 0 )
        return HNumber(0);
      return num;
    }
  }
  return HMath::trunc( num );
}


HNumber Functions::Private::frac( Function *, const QVector<HNumber> & args )
{
  return args.count() != 1 ? HMath::nan() : HMath::frac( args[0] );
}


HNumber Functions::Private::floor( Function *, const QVector<HNumber> & args )
{
  return args.count() != 1 ? HMath::nan() : HMath::floor( args[0] );
}


HNumber Functions::Private::ceil( Function *, const QVector<HNumber> & args )
{
  return args.count() != 1 ? HMath::nan() : HMath::ceil( args[0] );
}


HNumber Functions::Private::gcd( Function * f, const QVector<HNumber> & args )
{
  int nArgs = args.count();

  if ( nArgs < 2 )
  {
    f->setError( f->name(), Functions::tr(
                   "function requires at least 2 arguments" ) );
    return HMath::nan();
  }

  for ( int i = 0; i < args.count(); i++ )
    if ( ! args[i].isInteger() )
    {
      f->setError( f->name(), Functions::tr(
                     "function requires integer arguments" ) );
      return HMath::nan();
    }

  return std::accumulate(args.begin() + 1, args.end(), args[0],
          HMath::gcd);
}


HNumber Functions::Private::round( Function * f, const QVector<HNumber> & args )
{
  int nArgs = args.count();

  if ( nArgs != 1 && nArgs != 2 )
  {
    f->setError( f->name(), Functions::tr(
                    "function requires 1 or 2 arguments" ) );
    return HMath::nan();
  }

  HNumber num = args[0];

  if( nArgs == 2){

    int prec = 0;

    HNumber argprec = args[1];
    if (argprec != 0)
    {
      if( !argprec.isInteger() )
      {
        f->setError( f->name(), Functions::tr(
                        "function undefined for specified arguments" ) );
        return HMath::nan();
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


HNumber Functions::Private::sqrt( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HMath::nan();

  HNumber num = args[0];
  if( num < HNumber(0) )
  {
    f->setError( f->name(), Functions::tr(
                    "function undefined for specified argument" ) );
    return HMath::nan();
  }

  return HMath::sqrt( num );
}


HNumber Functions::Private::cbrt( Function *, const QVector<HNumber> & args )
{
  return args.count() != 1 ? HMath::nan() : HMath::cbrt( args[0] );
}


HNumber Functions::Private::exp( Function *, const QVector<HNumber> & args )
{
  return args.count() != 1 ? HMath::nan() : HMath::exp( args[0] );
}


HNumber Functions::Private::ln( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HMath::nan();

  HNumber x = args[0];
  HNumber result = HMath::ln( x );

  if ( result.isNan() )
    f->setError( f->name(), Functions::tr(
                    "function undefined for specified argument" ) );

  return result;
}


HNumber Functions::Private::log( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HMath::nan();

  HNumber x = args[0];
  HNumber result = HMath::log( x );

  if ( result.isNan() )
    f->setError( f->name(), Functions::tr(
                    "function undefined for specified argument" ) );

  return result;
}


HNumber Functions::Private::lg( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HMath::nan();

  HNumber x = args[0];
  HNumber result = HMath::lg( x );

  if ( result.isNan() )
    f->setError( f->name(), Functions::tr(
                    "function undefined for specified argument" ) );

  return result;
}


HNumber Functions::Private::sin( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HMath::nan();

  Settings * settings = Settings::instance();
  HNumber angle = args[0];

  if ( settings->angleUnit == 'd' )
    angle = HMath::deg2rad( angle );

  return HMath::sin( angle );
}

HNumber Functions::Private::cos( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HMath::nan();

  Settings * settings = Settings::instance();
  HNumber angle = args[0];

  if ( settings->angleUnit == 'd' )
    angle = HMath::deg2rad( angle );

  return HMath::cos( angle );
}

HNumber Functions::Private::tan( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HMath::nan();

  Settings * settings = Settings::instance();
  HNumber angle = args[0];

  if ( settings->angleUnit == 'd' )
    angle = HMath::deg2rad( angle );

  HNumber result = HMath::tan( angle );
  if ( result.isNan() )
  {
    f->setError( f->name(), Functions::tr(
                    "function undefined for specified argument" ) );
    return HMath::nan();
  }

  return result;
}

HNumber Functions::Private::cot( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HMath::nan();

  Settings * settings = Settings::instance();
  HNumber angle = args[0];

  if ( settings->angleUnit == 'd' )
    angle = HMath::deg2rad( angle );

  HNumber result = HMath::cot( angle );
  if ( result.isNan() )
  {
    f->setError( f->name(), Functions::tr(
                    "function undefined for specified argument" ) );
    return HMath::nan();
  }

  return result;
}

HNumber Functions::Private::sec( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HMath::nan();

  Settings * settings = Settings::instance();
  HNumber angle = args[0];

  if ( settings->angleUnit == 'd' )
    angle = HMath::deg2rad( angle );

  HNumber result = HMath::sec( angle );
  if ( result.isNan() )
  {
    f->setError( f->name(), Functions::tr(
                    "function undefined for specified argument" ) );
    return HMath::nan();
  }

  return result;
}

HNumber Functions::Private::csc( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HMath::nan();

  Settings * settings = Settings::instance();
  HNumber angle = args[0];

  if ( settings->angleUnit == 'd' )
    angle = HMath::deg2rad( angle );

  HNumber result = HMath::csc( angle );
  if ( result.isNan() )
  {
    f->setError( f->name(), Functions::tr(
                    "function undefined for specified argument" ) );
    return HMath::nan();
  }

  return result;
}

HNumber Functions::Private::asin( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HMath::nan();

  HNumber x = args[0];
  HNumber result = HMath::asin( x );

  if ( result.isNan() )
  {
    f->setError( f->name(), Functions::tr(
                          "function undefined for specified argument" ) );
    return HMath::nan();
  }

  Settings * settings = Settings::instance();

  if ( settings->angleUnit == 'd' )
    result = HMath::rad2deg( result );

  return result;
}

HNumber Functions::Private::acos( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HMath::nan();

  HNumber x = args[0];
  HNumber result = HMath::acos( x );

  if ( result.isNan() )
  {
    f->setError( f->name(), Functions::tr(
                        "function undefined for specified argument" ) );
    return HMath::nan();
  }

  Settings * settings = Settings::instance();

  if ( settings->angleUnit == 'd' )
    result = HMath::rad2deg( result );

  return result;
}

HNumber Functions::Private::atan( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HMath::nan();

  HNumber num = args[0];
  HNumber result = HMath::atan( num );

  Settings * settings = Settings::instance();

  if ( settings->angleUnit == 'd' )
    result = HMath::rad2deg( result );

  return result;
}

HNumber Functions::Private::sinh( Function *, const QVector<HNumber> & args )
{
  return args.count() != 1 ? HMath::nan() : HMath::sinh( args[0] );
}

HNumber Functions::Private::cosh( Function *, const QVector<HNumber> & args )
{
  return args.count() != 1 ? HMath::nan() : HMath::cosh( args[0] );
}

HNumber Functions::Private::tanh( Function *, const QVector<HNumber> & args )
{
  return args.count() != 1 ? HMath::nan() : HMath::tanh( args[0] );
}

HNumber Functions::Private::arsinh( Function *, const QVector<HNumber> & args )
{
  return args.count() != 1 ? HMath::nan() : HMath::arsinh( args[0] );
}

HNumber Functions::Private::arcosh( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HMath::nan();

  HNumber x = args[0];
  HNumber result = HMath::arcosh( x );

  if ( result.isNan() )
  {
    f->setError( f->name(), Functions::tr(
                          "function undefined for specified argument" ) );
    return HMath::nan();
  }

  return result;
}

HNumber Functions::Private::artanh( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HMath::nan();

  HNumber x = args[0];
  HNumber result = HMath::artanh( x );

  if ( result.isNan() )
  {
    f->setError( f->name(), Functions::tr(
                          "function undefined for specified argument" ) );
    return HMath::nan();
  }

  return result;
}

HNumber Functions::Private::erf( Function *, const QVector<HNumber> & args )
{
  return args.count() != 1 ? HMath::nan() : HMath::erf( args[0] );
}

HNumber Functions::Private::erfc( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HMath::nan();

  HNumber x = args[0];
  HNumber result = HMath::erfc( x );

  if ( result.isNan() && ! x.isNan() )
  {
    f->setError( f->name(), Functions::tr(
                          "underflow" ) );
  }

  return result;
}

HNumber Functions::Private::Gamma( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HMath::nan();

  HNumber x = args[0];
  HNumber result = HMath::gamma( x );

  if ( result.isNan() )
  {
    f->setError( f->name(), Functions::tr(
                          "function undefined for specified argument" ) );
    return HMath::nan();
  }

  return result;
}

HNumber Functions::Private::lnGamma( Function * f,
                                     const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HMath::nan();

  HNumber x = args[0];
  HNumber result = HMath::lnGamma( x );

  if ( result.isNan() )
  {
    f->setError( f->name(), Functions::tr(
                          "function undefined for specified argument" ) );
    return HMath::nan();
  }

  return result;
}

HNumber Functions::Private::sign( Function *, const QVector<HNumber> & args )
{
  return args.count() != 1 ? HMath::nan() : HMath::sign( args[0] );
}

HNumber Functions::Private::nCr( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 2 )
    return HMath::nan();

  // n = args[0]; r = args[1]
  HNumber result = HMath::nCr( args[0], args[1] );

  // FIX ME: overflow causes a NaN, too, so the message is sometimes
  // misleading
  if ( result.isNan() )
    f->setError( f->name(), Functions::tr(
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber Functions::Private::nPr( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 2 )
    return HMath::nan();

  // n = args[0]; r = args[1]
  HNumber result = HMath::nPr( args[0], args[1] );

  // FIX ME: overflow causes a NaN, too, so the message is sometimes
  // misleading
  if ( result.isNan() )
    f->setError( f->name(), Functions::tr(
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber Functions::Private::degrees( Function *, const QVector<HNumber> & args )
{
  return args.count() != 1 ? HMath::nan() : HMath::rad2deg( args[0] );
}

HNumber Functions::Private::radians( Function *, const QVector<HNumber> & args )
{
  return args.count() != 1 ? HMath::nan() : HMath::deg2rad( args[0] );
}

HNumber Functions::Private::max( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() < 1 )
  {
    f->setError( f->name(), Functions::tr(
                          "function requires at least 1 argument" ) );
    return HMath::nan();
  }

  return * std::max_element(args.begin(), args.end());
}

HNumber Functions::Private::min( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() < 1 )
  {
    f->setError( f->name(), Functions::tr(
                          "function requires at least 1 argument" ) );
    return HMath::nan();
  }

  return * std::min_element(args.begin(), args.end());
}

HNumber Functions::Private::sum( Function *, const QVector<HNumber> & args )
{
  return args.count() <= 0 ? HNumber(0)
      : std::accumulate(args.begin(), args.end(), HNumber(0));
}

HNumber Functions::Private::product( Function *, const QVector<HNumber> & args )
{
  return args.count() <= 0 ? HNumber(0)
      : std::accumulate(args.begin(), args.end(), HNumber(1),
          std::multiplies<HNumber>());
}

HNumber Functions::Private::average( Function *, const QVector<HNumber> & args )
{
  return args.count() <= 0 ? HNumber("NaN")
      : std::accumulate(args.begin(), args.end(), HNumber(0))
      / HNumber(args.count());
}

HNumber Functions::Private::geomean( Function *, const QVector<HNumber> & args )
{
  if ( args.count() <= 0 )
    return HNumber("NaN");

  HNumber result = std::accumulate(args.begin(), args.end(), HNumber(1),
          std::multiplies<HNumber>());

  if ( result <= HNumber(0))
    return HNumber("NaN");

  if ( args.count() == 1 )
    return result;

  if ( args.count() == 2 )
    return HMath::sqrt( result );

  return HMath::exp( HMath::ln(result) / HNumber(args.count()) );
}

HNumber Functions::Private::dec( Function *, const QVector<HNumber> & args )
{
  return (args.count() != 1) ? HNumber("NaN") : HNumber(args[0]).setFormat('g');
}

HNumber Functions::Private::hex( Function *, const QVector<HNumber> & args )
{
  return (args.count() != 1) ? HNumber("NaN") : HNumber(args[0]).setFormat('h');
}

HNumber Functions::Private::oct( Function *, const QVector<HNumber> & args )
{
  return (args.count() != 1) ? HNumber("NaN") : HNumber(args[0]).setFormat('o');
}

HNumber Functions::Private::bin( Function *, const QVector<HNumber> & args )
{
  return (args.count() != 1) ? HNumber("NaN") : HNumber(args[0]).setFormat('b');
}

HNumber Functions::Private::binompmf( Function * f,
                                      const QVector<HNumber> & args )
{
  if ( args.count() != 3 )
    return HMath::nan();

  HNumber k = args[0];
  HNumber n = args[1];
  HNumber p = args[2];
  HNumber result = HMath::binomialPmf( k, n, p );

  if ( result.isNan() )
    f->setError( f->name(), Functions::tr(
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber Functions::Private::binomcdf( Function * f,
                                      const QVector<HNumber> & args )
{
  if ( args.count() != 3 )
    return HMath::nan();

  HNumber k = args[0];
  HNumber n = args[1];
  HNumber p = args[2];
  HNumber result = HMath::binomialCdf( k, n, p );

  if ( result.isNan() )
    f->setError( f->name(), Functions::tr(
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber Functions::Private::binommean( Function * f,
                                       const QVector<HNumber> & args )
{
  if ( args.count() != 2 )
    return HMath::nan();

  HNumber n = args[0];
  HNumber p = args[1];
  HNumber result = HMath::binomialMean( n, p );

  if ( result.isNan() )
    f->setError( f->name(), Functions::tr(
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber Functions::Private::binomvar( Function * f,
                                      const QVector<HNumber> & args )
{
  if ( args.count() != 2 )
    return HMath::nan();

  HNumber n = args[0];
  HNumber p = args[1];
  HNumber result = HMath::binomialVariance( n, p );

  if ( result.isNan() )
    f->setError( f->name(), Functions::tr(
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber Functions::Private::hyperpmf( Function * f,
                                      const QVector<HNumber> & args )
{
  if ( args.count() != 4 )
    return HMath::nan();

  HNumber k = args[0];
  HNumber N = args[1];
  HNumber M = args[2];
  HNumber n = args[3];
  HNumber result = HMath::hypergeometricPmf( k, N, M, n );

  if ( result.isNan() )
    f->setError( f->name(), Functions::tr(
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber Functions::Private::hypercdf( Function * f,
                                      const QVector<HNumber> & args )
{
  if ( args.count() != 4 )
    return HMath::nan();

  HNumber k = args[0];
  HNumber N = args[1];
  HNumber M = args[2];
  HNumber n = args[3];
  HNumber result = HMath::hypergeometricCdf( k, N, M, n );

  if ( result.isNan() )
    f->setError( f->name(), Functions::tr(
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber Functions::Private::hypermean( Function * f,
                                       const QVector<HNumber> & args )
{
  if ( args.count() != 3 )
    return HMath::nan();

  HNumber N = args[0];
  HNumber M = args[1];
  HNumber n = args[2];
  HNumber result = HMath::hypergeometricMean( N, M, n );

  if ( result.isNan() )
    f->setError( f->name(), Functions::tr(
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber Functions::Private::hypervar( Function * f,
                                      const QVector<HNumber> & args )
{
  if ( args.count() != 3 )
    return HMath::nan();

  HNumber N = args[0];
  HNumber M = args[1];
  HNumber n = args[2];
  HNumber result = HMath::hypergeometricVariance( N, M, n );

  if ( result.isNan() )
    f->setError( f->name(), Functions::tr(
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber Functions::Private::poipmf( Function * f,
                                    const QVector<HNumber> & args )
{
  if ( args.count() != 2 )
    return HMath::nan();

  HNumber k = args[0];
  HNumber l = args[1];
  HNumber result = HMath::poissonPmf( k, l );

  if ( result.isNan() )
    f->setError( f->name(), Functions::tr(
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber Functions::Private::poicdf( Function * f,
                                    const QVector<HNumber> & args )
{
  if ( args.count() != 2 )
    return HMath::nan();

  HNumber k = args[0];
  HNumber l = args[1];
  HNumber result = HMath::poissonCdf( k, l );

  if ( result.isNan() )
    f->setError( f->name(), Functions::tr(
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber Functions::Private::poimean( Function * f,
                                     const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HMath::nan();

  HNumber l = args[0];
  HNumber result = HMath::poissonMean( l );

  if ( result.isNan() )
    f->setError( f->name(), Functions::tr(
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber Functions::Private::poivar( Function * f,
                                    const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HMath::nan();

  HNumber l = args[0];
  HNumber result = HMath::poissonVariance( l );

  if ( result.isNan() )
    f->setError( f->name(), Functions::tr(
                          "function undefined for specified arguments" ) );

  return result;
}

HNumber Functions::Private::mask( Function *, const QVector<HNumber> & args )
{
  HNumber val = args[0];
  HNumber bits = args[1];
  return HMath::mask( val, bits );
}

HNumber Functions::Private::unmask( Function *,
                                    const QVector<HNumber> & args )
{
  HNumber val = args[0];
  HNumber bits = args[1];
  return HMath::sgnext( val, bits );
}

HNumber Functions::Private::not_( Function *, const QVector<HNumber> & args )
{
  HNumber val = args[0];
  return ~val;
}

HNumber Functions::Private::and_( Function *, const QVector<HNumber> & args )
{
  return args.count() <= 0 ? HNumber("NaN")
      : std::accumulate(args.begin(), args.end(), HNumber(-1),
          std::mem_fun_ref(&HNumber::operator&));
}

HNumber Functions::Private::or_( Function *, const QVector<HNumber> & args )
{
  return args.count() <= 0 ? HNumber("NaN")
      : std::accumulate(args.begin(), args.end(), HNumber(0),
          std::mem_fun_ref(&HNumber::operator|));
}

HNumber Functions::Private::xor_( Function *, const QVector<HNumber> & args )
{
  return args.count() <= 0 ? HNumber("NaN")
      : std::accumulate(args.begin(), args.end(), HNumber(0),
          std::mem_fun_ref(&HNumber::operator^));
}

HNumber Functions::Private::ashl( Function *, const QVector<HNumber> & args )
{
  HNumber bits = args[1];
  HNumber val = args[0];

  return HMath::ashr( val, -bits );
}

HNumber Functions::Private::ashr( Function *, const QVector<HNumber> & args )
{
  HNumber bits = args[1];
  HNumber val = args[0];

  return HMath::ashr(val, bits);
}

HNumber Functions::Private::idiv( Function *, const QVector<HNumber> & args )
{
  HNumber dividend = args[0];
  HNumber divisor = args[1];

  return HMath::idiv(dividend, divisor);
}

HNumber Functions::Private::mod( Function *, const QVector<HNumber> & args )
{
  HNumber dividend = args[0];
  HNumber divisor = args[1];

  return dividend % divisor;
}

void Functions::Private::createBuiltInFunctions()
{
  // ANALYSIS
  p->add( new Function( "abs",     1, abs,     p ) );
  p->add( new Function( "average",    average, p ) );
  p->add( new Function( "bin",        bin,     p ) );
  p->add( new Function( "cbrt",    1, cbrt,    p ) );
  p->add( new Function( "ceil",    1, ceil,    p ) );
  p->add( new Function( "dec",        dec,     p ) );
  p->add( new Function( "floor",   1, floor,   p ) );
  p->add( new Function( "frac",    1, frac,    p ) );
  p->add( new Function( "gamma",      Gamma,   p ) );
  p->add( new Function( "geomean",    geomean, p ) );
  p->add( new Function( "hex",        hex,     p ) );
  p->add( new Function( "int",     1, integer, p ) );
  p->add( new Function( "lngamma",    lnGamma, p ) );
  p->add( new Function( "max",        max,     p ) );
  p->add( new Function( "min",        min,     p ) );
  p->add( new Function( "oct",        oct,     p ) );
  p->add( new Function( "product",    product, p ) );
  p->add( new Function( "round",      round,   p ) );
  p->add( new Function( "sign",    1, sign,    p ) );
  p->add( new Function( "sqrt",    1, sqrt,    p ) );
  p->add( new Function( "sum",        sum,     p ) );
  p->add( new Function( "trunc",      trunc,   p ) );

  // LOGARITHM
  p->add( new Function( "arcosh", 1, arcosh, p ) );
  p->add( new Function( "arsinh", 1, arsinh, p ) );
  p->add( new Function( "artanh", 1, artanh, p ) );
  p->add( new Function( "cosh",   1, cosh,   p ) );
  p->add( new Function( "exp",    1, exp,    p ) );
  p->add( new Function( "lg",     1, lg,     p ) );
  p->add( new Function( "ln",     1, ln,     p ) );
  p->add( new Function( "log",    1, log,    p ) );
  p->add( new Function( "sinh",   1, sinh,   p ) );
  p->add( new Function( "tanh",   1, tanh,   p ) );

  // DISCRETE
  p->add( new Function( "gcd",    gcd, p ) );
  p->add( new Function( "ncr", 2, nCr, p ) );
  p->add( new Function( "npr", 2, nPr, p ) );

  // PROBABILITY
  p->add( new Function( "binomcdf",  3, binomcdf,  p ) );
  p->add( new Function( "binommean", 2, binommean, p ) );
  p->add( new Function( "binompmf",  3, binompmf,  p ) );
  p->add( new Function( "binomvar",  2, binomvar,  p ) );
  p->add( new Function( "erf",       1, erf,       p ) );
  p->add( new Function( "erfc",      1, erfc,      p ) );
  p->add( new Function( "hypercdf",  4, hypercdf,  p ) );
  p->add( new Function( "hypermean", 3, hypermean, p ) );
  p->add( new Function( "hyperpmf",  4, hyperpmf,  p ) );
  p->add( new Function( "hypervar",  3, hypervar,  p ) );
  p->add( new Function( "poicdf",    2, poicdf,    p ) );
  p->add( new Function( "poimean",   1, poimean,   p ) );
  p->add( new Function( "poipmf",    2, poipmf,    p ) );
  p->add( new Function( "poivar",    1, poivar,    p ) );

  // TRIGONOMETRY
  p->add( new Function( "acos",    1, acos,    p ) );
  p->add( new Function( "asin",    1, asin,    p ) );
  p->add( new Function( "atan",    1, atan,    p ) );
  p->add( new Function( "cos",     1, cos,     p ) );
  p->add( new Function( "cot",     1, cot,     p ) );
  p->add( new Function( "csc",     1, csc,     p ) );
  p->add( new Function( "degrees", 1, degrees, p ) );
  p->add( new Function( "radians", 1, radians, p ) );
  p->add( new Function( "sec",     1, sec,     p ) );
  p->add( new Function( "sin",     1, sin,     p ) );
  p->add( new Function( "tan",     1, tan,     p ) );

  // LOGIC
  p->add( new Function( "mask",   2, mask,   p ) );
  p->add( new Function( "unmask", 2, unmask, p ) );
  p->add( new Function( "not",    1, not_,   p ) );
  p->add( new Function( "and",       and_,   p ) );
  p->add( new Function( "or",        or_,    p ) );
  p->add( new Function( "xor",       xor_,   p ) );
  p->add( new Function( "shl",    2, ashl,   p ) );
  p->add( new Function( "shr",    2, ashr,   p ) );
  p->add( new Function( "idiv",   2, idiv,   p ) );
  p->add( new Function( "mod",    2, mod,    p ) );
}

Functions::Functions( QObject * parent )
  : QObject( parent ), d( new Functions::Private )
{
  d->p = this;
  d->createBuiltInFunctions();
  retranslateText();
}

void Functions::add( Function * f )
{
  if ( f )
    d->functions.insert( f->name().toUpper(), f );
}

Function * Functions::function( const QString & name ) const
{
  return d->functions.value( name.toUpper(), 0 );
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
  QList<Function *> values = d->functions.values();
  for ( int i = 0; i < values.size(); i++ )
    delete values[i];

  delete d;
}

void Functions::retranslateText()
{
  // ANALYSIS
  function( "abs"     )->setDescription( tr( "Absolute Value"                     ) );
  function( "average" )->setDescription( tr( "Average (Arithmetic Mean)"          ) );
  function( "bin"     )->setDescription( tr( "Binary Representation"              ) );
  function( "cbrt"    )->setDescription( tr( "Cube Root"                          ) );
  function( "ceil"    )->setDescription( tr( "Ceiling"                            ) );
  function( "dec"     )->setDescription( tr( "Decimal Representation"             ) );
  function( "floor"   )->setDescription( tr( "Floor"                              ) );
  function( "frac"    )->setDescription( tr( "Fractional Part"                    ) );
  function( "gamma"   )->setDescription( tr( "Extension of Factorials [= (x-1)!]" ) );
  function( "geomean" )->setDescription( tr( "Geometric Mean"                     ) );
  function( "hex"     )->setDescription( tr( "Hexadecimal Representation"         ) );
  function( "int"     )->setDescription( tr( "Integer Part"                       ) );
  function( "lngamma" )->setDescription( tr( "ln(abs(Gamma))"                     ) );
  function( "max"     )->setDescription( tr( "Maximum"                            ) );
  function( "min"     )->setDescription( tr( "Minimum"                            ) );
  function( "oct"     )->setDescription( tr( "Octal Representation"               ) );
  function( "product" )->setDescription( tr( "Product"                            ) );
  function( "round"   )->setDescription( tr( "Rounding"                           ) );
  function( "sign"    )->setDescription( tr( "Signum"                             ) );
  function( "sqrt"    )->setDescription( tr( "Square Root"                        ) );
  function( "sum"     )->setDescription( tr( "Sum"                                ) );
  function( "trunc"   )->setDescription( tr( "Truncation"                         ) );

  //// LOGARITHM
  function( "arcosh" )->setDescription( tr( "Area Hyperbolic Cosine"  ) );
  function( "arsinh" )->setDescription( tr( "Area Hyperbolic Sine"    ) );
  function( "artanh" )->setDescription( tr( "Area Hyperbolic Tangent" ) );
  function( "cosh"   )->setDescription( tr( "Hyperbolic Cosine"       ) );
  function( "exp"    )->setDescription( tr( "Exponential"             ) );
  function( "lg"     )->setDescription( tr( "Base-2 Logarithm"        ) );
  function( "ln"     )->setDescription( tr( "Natural Logarithm"       ) );
  function( "log"    )->setDescription( tr( "Base-10 Logarithm"       ) );
  function( "sinh"   )->setDescription( tr( "Hyperbolic Sine"         ) );
  function( "tanh"   )->setDescription( tr( "Hyperbolic Tangent"      ) );

  //// DISCRETE
  function( "gcd" )->setDescription( tr( "Greatest Common Divisor"            ) );
  function( "ncr" )->setDescription( tr( "Combination (Binomial Coefficient)" ) );
  function( "npr" )->setDescription( tr( "Permutation (Arrangement)"          ) );

  //// PROBABILITY
  function( "binomcdf"  )->setDescription( tr( "Binomial Cumulative Distribution Function"       ) );
  function( "binommean" )->setDescription( tr( "Binomial Distribution Mean"                      ) );
  function( "binompmf"  )->setDescription( tr( "Binomial Probability Mass Function"              ) );
  function( "binomvar"  )->setDescription( tr( "Binomial Distribution Variance"                  ) );
  function( "erf"       )->setDescription( tr( "Error Function"                                  ) );
  function( "erfc"      )->setDescription( tr( "Complementary Error Function"                    ) );
  function( "hypercdf"  )->setDescription( tr( "Hypergeometric Cumulative Distribution Function" ) );
  function( "hypermean" )->setDescription( tr( "Hypergeometric Distribution Mean"                ) );
  function( "hyperpmf"  )->setDescription( tr( "Hypergeometric Probability Mass Function"        ) );
  function( "hypervar"  )->setDescription( tr( "Hypergeometric Distribution Variance"            ) );
  function( "poicdf"    )->setDescription( tr( "Poissonian Cumulative Distribution Function"     ) );
  function( "poimean"   )->setDescription( tr( "Poissonian Distribution Mean"                    ) );
  function( "poipmf"    )->setDescription( tr( "Poissonian Probability Mass Function"            ) );
  function( "poivar"    )->setDescription( tr( "Poissonian Distribution Variance"                ) );

  //// TRIGONOMETRY
  function( "acos"    )->setDescription( tr( "Arc Cosine"     ) );
  function( "asin"    )->setDescription( tr( "Arc Sine"       ) );
  function( "atan"    )->setDescription( tr( "Arc Tangent"    ) );
  function( "cos"     )->setDescription( tr( "Cosine"         ) );
  function( "cot"     )->setDescription( tr( "Cotangent"      ) );
  function( "csc"     )->setDescription( tr( "Cosecant"       ) );
  function( "degrees" )->setDescription( tr( "Degrees of Arc" ) );
  function( "radians" )->setDescription( tr( "Radians"        ) );
  function( "sec"     )->setDescription( tr( "Secant"         ) );
  function( "sin"     )->setDescription( tr( "Sine"           ) );
  function( "tan"     )->setDescription( tr( "Tangent"        ) );

  //// LOGIC
  function( "mask"   )->setDescription( tr( "Mask to a bit size"     ) );
  function( "unmask" )->setDescription( tr( "Sign-extent a value"    ) );
  function( "not"    )->setDescription( tr( "Logical NOT"            ) );
  function( "and"    )->setDescription( tr( "Logical AND"            ) );
  function( "or"     )->setDescription( tr( "Logical OR"             ) );
  function( "xor"    )->setDescription( tr( "Logical XOR"            ) );
  function( "shl"    )->setDescription( tr( "Arithmetic Shift Left"  ) );
  function( "shr"    )->setDescription( tr( "Arithmetic Shift Right" ) );
  function( "idiv"   )->setDescription( tr( "Integer Quotient"       ) );
  function( "mod"    )->setDescription( tr( "Modulo"                 ) );
}

