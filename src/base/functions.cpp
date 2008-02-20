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


// public (Function)

Function::Function( const QString & name, int argc, FunctionPtr ptr,
                    const QString & desc, Functions * parent )
  : d( new Function::Private )
{
  d->functions = parent;
  d->name      = name;
  d->argc      = argc;
  d->desc      = QApplication::translate( "Functions", desc.toLatin1() );
  d->ptr       = ptr;
}


Function::Function( const QString & name, FunctionPtr ptr, const QString & desc,
                    Functions * parent )
  : d( new Function::Private )
{
  d->functions = parent;
  d->name      = name;
  d->argc      = -1;
  d->desc      = QApplication::translate( "Functions", desc.toLatin1() );
  d->ptr       = ptr;
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


HNumber Function::exec( const QVector<HNumber> & args )
{
  d->error = QString();
  if ( ! d->ptr )
  {
    setError( QString("error"), QString( QApplication::translate( "functions",
                "cannot execute function %1") ).arg( name() ) );
    return HNumber(0);
  }

  if ( d->argc >= 0 && args.count() != d->argc )
  {
    if ( d->argc == 1 )
      setError( d->name, QString( QApplication::translate( "functions",
                  "function accepts 1 argument" ) ) );
    else
      setError( d->name, QString( QApplication::translate( "functions",
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
  QHash<QString, Function *> functions;
  char angleMode;

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
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::abs( num );
}


HNumber Functions::Private::integer( Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::integer( num );
}


HNumber Functions::Private::trunc( Function * f, const QVector<HNumber> & args )
{
  int nArgs = args.count();

  if ( nArgs != 1 && nArgs != 2 )
  {
    f->setError( f->name(), QApplication::translate( "functions",
      "function requires 1 or 2 arguments" ) );
    return HNumber::nan();
  }

  HNumber num = args[0];

  if ( nArgs == 2){

    int prec = 0;

    HNumber argprec = args[1];
    if ( argprec != 0 )
    {
      if ( ! argprec.isInteger() )
      {
        f->setError( f->name(), QApplication::translate( "functions",
                              "function undefined for specified arguments" ) );
        return HNumber::nan();
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
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];
  return HMath::frac( x );
}


HNumber Functions::Private::floor( Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::floor( num );
}


HNumber Functions::Private::ceil( Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::ceil( num );
}


HNumber Functions::Private::gcd( Function * f, const QVector<HNumber> & args )
{
  int nArgs = args.count();

  if ( nArgs < 2 )
  {
    f->setError( f->name(), QApplication::translate( "functions",
                   "function requires at least 2 arguments" ) );
    return HNumber::nan();
  }

  for ( int i = 0; i < args.count(); i++ )
    if ( ! args[i].isInteger() )
    {
      f->setError( f->name(), QApplication::translate( "functions",
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


HNumber Functions::Private::round( Function * f, const QVector<HNumber> & args )
{
  int nArgs = args.count();

  if ( nArgs != 1 && nArgs != 2 )
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
    return HNumber::nan();

  HNumber num = args[0];
  if( num < HNumber(0) )
  {
    f->setError( f->name(), QApplication::translate( "functions",
                    "function undefined for specified argument" ) );
    return HNumber::nan();
  }

  return HMath::sqrt( num );
}


HNumber Functions::Private::cbrt( Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::cbrt( num );
}


HNumber Functions::Private::exp( Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::exp( num );
}


HNumber Functions::Private::ln( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];
  HNumber result = HMath::ln( x );

  if ( result.isNan() )
    f->setError( f->name(), QApplication::translate( "functions",
                    "function undefined for specified argument" ) );

  return result;
}


HNumber Functions::Private::log( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];
  HNumber result = HMath::log( x );

  if ( result.isNan() )
    f->setError( f->name(), QApplication::translate( "functions",
                    "function undefined for specified argument" ) );

  return result;
}


HNumber Functions::Private::lg( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];
  HNumber result = HMath::lg( x );

  if ( result.isNan() )
    f->setError( f->name(), QApplication::translate( "functions",
                    "function undefined for specified argument" ) );

  return result;
}


HNumber Functions::Private::sin( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  if ( f->functions()->angleMode() == 'd' )
    angle = HMath::deg2rad( angle );

  return HMath::sin( angle );
}


HNumber Functions::Private::cos( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  if ( f->functions()->angleMode() == 'd' )
    angle = HMath::deg2rad( angle );

  return HMath::cos( angle );
}


HNumber Functions::Private::tan( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  if ( f->functions()->angleMode() == 'd' )
    angle = HMath::deg2rad( angle );

  HNumber result = HMath::tan( angle );
  if ( result.isNan() )
  {
    f->setError( f->name(), QApplication::translate( "functions",
                    "function undefined for specified argument" ) );
    return HNumber::nan();
  }

  return result;
}


HNumber Functions::Private::cot( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  if ( f->functions()->angleMode() == 'd' )
    angle = HMath::deg2rad( angle );

  HNumber result = HMath::cot( angle );
  if ( result.isNan() )
  {
    f->setError( f->name(), QApplication::translate( "functions",
                    "function undefined for specified argument" ) );
    return HNumber::nan();
  }

  return result;
}


HNumber Functions::Private::sec( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  if ( f->functions()->angleMode() == 'd' )
    angle = HMath::deg2rad( angle );

  HNumber result = HMath::sec( angle );
  if ( result.isNan() )
  {
    f->setError( f->name(), QApplication::translate( "functions",
                    "function undefined for specified argument" ) );
    return HNumber::nan();
  }

  return result;
}


HNumber Functions::Private::csc( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  if ( f->functions()->angleMode() == 'd' )
    angle = HMath::deg2rad( angle );

  HNumber result = HMath::csc( angle );
  if ( result.isNan() )
  {
    f->setError( f->name(), QApplication::translate( "functions",
                    "function undefined for specified argument" ) );
    return HNumber::nan();
  }

  return result;
}


HNumber Functions::Private::asin( Function * f, const QVector<HNumber> & args )
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

  if ( f->functions()->angleMode() == 'd' )
    result = HMath::rad2deg( result );
  return result;
}


HNumber Functions::Private::acos( Function * f, const QVector<HNumber> & args )
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

  if ( f->functions()->angleMode() == 'd' )
    result = HMath::rad2deg( result );
  return result;
}


HNumber Functions::Private::atan( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  HNumber result = HMath::atan( num );

  if ( f->functions()->angleMode() == 'd' )
    result = HMath::rad2deg( result );
  return result;
}


HNumber Functions::Private::sinh( Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];
  return HMath::sinh( x );
}


HNumber Functions::Private::cosh( Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];
  return HMath::cosh( x );
}


HNumber Functions::Private::tanh( Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];
  return HMath::tanh( x );
}


HNumber Functions::Private::arsinh( Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  return HMath::arsinh( args[0] );
}


HNumber Functions::Private::arcosh( Function * f, const QVector<HNumber> & args )
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


HNumber Functions::Private::artanh( Function * f, const QVector<HNumber> & args )
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


HNumber Functions::Private::erf( Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];
  return HMath::erf( x );
}


HNumber Functions::Private::erfc( Function * f, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber x = args[0];
  HNumber result = HMath::erfc( x );

  if ( result.isNan() && ! x.isNan() )
  {
    f->setError( f->name(), QApplication::translate( "functions",
                          "underflow" ) );
  }

  return result;
}


HNumber Functions::Private::Gamma( Function * f, const QVector<HNumber> & args )
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


HNumber Functions::Private::lnGamma( Function * f,
                                     const QVector<HNumber> & args )
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


HNumber Functions::Private::sign( Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  return HMath::sign( args[0] );
}


HNumber Functions::Private::nCr( Function * f, const QVector<HNumber> & args )
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


HNumber Functions::Private::nPr( Function * f, const QVector<HNumber> & args )
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


HNumber Functions::Private::degrees( Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  return HMath::rad2deg(angle);
}


HNumber Functions::Private::radians( Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  return HMath::deg2rad(angle);
}


HNumber Functions::Private::max( Function * f, const QVector<HNumber> & args )
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


HNumber Functions::Private::min( Function * f, const QVector<HNumber> & args )
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


HNumber Functions::Private::sum( Function *, const QVector<HNumber> & args )
{
  if ( args.count() <= 0 )
    return HNumber(0);

  HNumber result = args[0];
  for ( int c = 1; c < args.count(); c++ )
    result = result + args[c];

  return result;
}


HNumber Functions::Private::product( Function *, const QVector<HNumber> & args )
{
  if ( args.count() <= 0 )
    return HNumber(0);

  HNumber result = args[0];
  for ( int c = 1; c < args.count(); c++ )
    result = result * args[c];

  return result;
}


HNumber Functions::Private::average( Function *, const QVector<HNumber> & args )
{
  if ( args.count() <= 0 )
    return HNumber("NaN");

  HNumber result = args[0];
  for ( int c = 1; c < args.count(); c++ )
    result = result + args[c];
  result = result / HNumber(args.count());

  return result;
}


HNumber Functions::Private::geomean( Function *, const QVector<HNumber> & args )
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


HNumber Functions::Private::dec( Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber("NaN");

  HNumber result = args[0];
  result.setFormat( 'g' );
  return result;
}


HNumber Functions::Private::hex( Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber("NaN");

  HNumber result = args[0];
  result.setFormat( 'h' );
  return result;
}


HNumber Functions::Private::oct( Function *, const QVector<HNumber> & args )
{
  if( args.count() != 1 )
    return HNumber("NaN");

  HNumber result = args[0];
  result.setFormat( 'o' );
  return result;
}


HNumber Functions::Private::bin( Function *, const QVector<HNumber> & args )
{
  if ( args.count() != 1 )
    return HNumber("NaN");

  HNumber result = args[0];
  result.setFormat( 'b' );
  return result;
}


HNumber Functions::Private::binompmf( Function * f,
                                      const QVector<HNumber> & args )
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


HNumber Functions::Private::binomcdf( Function * f,
                                      const QVector<HNumber> & args )
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


HNumber Functions::Private::binommean( Function * f,
                                       const QVector<HNumber> & args )
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


HNumber Functions::Private::binomvar( Function * f,
                                      const QVector<HNumber> & args )
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


HNumber Functions::Private::hyperpmf( Function * f,
                                      const QVector<HNumber> & args )
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


HNumber Functions::Private::hypercdf( Function * f,
                                      const QVector<HNumber> & args )
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


HNumber Functions::Private::hypermean( Function * f,
                                       const QVector<HNumber> & args )
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


HNumber Functions::Private::hypervar( Function * f,
                                      const QVector<HNumber> & args )
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


HNumber Functions::Private::poipmf( Function * f,
                                    const QVector<HNumber> & args )
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


HNumber Functions::Private::poicdf( Function * f,
                                    const QVector<HNumber> & args )
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


HNumber Functions::Private::poimean( Function * f,
                                     const QVector<HNumber> & args )
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


HNumber Functions::Private::poivar( Function * f,
                                    const QVector<HNumber> & args )
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
  if ( args.count() <= 0 )
    return HNumber("NaN");

  HNumber result = args[0];
  for( int c = 1; c < args.count(); c++ )
    result = result & args[c];

  return result;
}


HNumber Functions::Private::or_( Function *, const QVector<HNumber> & args )
{
  if ( args.count() <= 0 )
    return HNumber("NaN");

  HNumber result = args[0];
  for ( int c = 1; c < args.count(); c++ )
    result = result | args[c];

  return result;
}


HNumber Functions::Private::xor_( Function *, const QVector<HNumber> & args )
{
  if ( args.count() <= 0 )
    return HNumber("NaN");

  HNumber result = args[0];
  for ( int c = 1; c < args.count(); c++ )
    result = result ^ args[c];

  return result;
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

  return HNumber::idiv(dividend, divisor);
}


HNumber Functions::Private::mod( Function *, const QVector<HNumber> & args )
{
  HNumber dividend = args[0];
  HNumber divisor = args[1];

  return dividend % divisor;
}


// public (Functions)

Functions::Functions( char angleMode, QObject * parent )
  : QObject( parent ), d( new Functions::Private )
{
  d->angleMode = angleMode;

  // ANALYSIS
  add( new Function( "abs",     1, d->abs,     tr("Absolute Value"),                     this ) );
  add( new Function( "average",    d->average, tr("Average (Arithmetic Mean)"),          this ) );
  add( new Function( "bin",        d->bin,     tr("Binary Representation"),              this ) );
  add( new Function( "cbrt",    1, d->cbrt,    tr("Cube Root"),                          this ) );
  add( new Function( "ceil",    1, d->ceil,    tr("Ceiling"),                            this ) );
  add( new Function( "dec",        d->dec,     tr("Decimal Representation"),             this ) );
  add( new Function( "floor",   1, d->floor,   tr("Floor"),                              this ) );
  add( new Function( "frac",    1, d->frac,    tr("Fractional Part"),                    this ) );
  add( new Function( "gamma",      d->Gamma,   tr("Extension of Factorials [= (x-1)!]"), this ) );
  add( new Function( "geomean",    d->geomean, tr("Geometric Mean"),                     this ) );
  add( new Function( "hex",        d->hex,     tr("Hexadecimal Representation"),         this ) );
  add( new Function( "int",     1, d->integer, tr("Integer Part"),                       this ) );
  add( new Function( "lngamma",    d->lnGamma, tr("ln(abs(Gamma))"),                     this ) );
  add( new Function( "max",        d->max,     tr("Maximum"),                            this ) );
  add( new Function( "min",        d->min,     tr("Minimum"),                            this ) );
  add( new Function( "oct",        d->oct,     tr("Octal Representation"),               this ) );
  add( new Function( "product",    d->product, tr("Product"),                            this ) );
  add( new Function( "round",      d->round,   tr("Rounding"),                           this ) );
  add( new Function( "sign",    1, d->sign,    tr("Signum"),                             this ) );
  add( new Function( "sqrt",    1, d->sqrt,    tr("Square Root"),                        this ) );
  add( new Function( "sum",        d->sum,     tr("Sum"),                                this ) );
  add( new Function( "trunc",      d->trunc,   tr("Truncation"),                         this ) );

  // LOGARITHM
  add( new Function( "arcosh", 1, d->arcosh, tr("Area Hyperbolic Cosine"),  this ) );
  add( new Function( "arsinh", 1, d->arsinh, tr("Area Hyperbolic Sine"),    this ) );
  add( new Function( "artanh", 1, d->artanh, tr("Area Hyperbolic Tangent"), this ) );
  add( new Function( "cosh",   1, d->cosh,   tr("Hyperbolic Cosine"),       this ) );
  add( new Function( "exp",    1, d->exp,    tr("Exponential"),             this ) );
  add( new Function( "lg",     1, d->lg,     tr("Base-2 Logarithm"),        this ) );
  add( new Function( "ln",     1, d->ln,     tr("Natural Logarithm"),       this ) );
  add( new Function( "log",    1, d->log,    tr("Base-10 Logarithm"),       this ) );
  add( new Function( "sinh",   1, d->sinh,   tr("Hyperbolic Sine"),         this ) );
  add( new Function( "tanh",   1, d->tanh,   tr("Hyperbolic Tangent"),      this ) );

  // DISCRETE
  add( new Function( "gcd",    d->gcd, tr("Greatest Common Divisor"),            this ) );
  add( new Function( "ncr", 2, d->nCr, tr("Combination (Binomial Coefficient)"), this ) );
  add( new Function( "npr", 2, d->nPr, tr("Permutation (Arrangement)"),          this ) );

  // PROBABILITY
  add( new Function( "binomcdf",  3, d->binomcdf,  tr("Binomial Cumulative Distribution Function"),       this ) );
  add( new Function( "binommean", 2, d->binommean, tr("Binomial Distribution Mean"),                      this ) );
  add( new Function( "binompmf",  3, d->binompmf,  tr("Binomial Probability Mass Function"),              this ) );
  add( new Function( "binomvar",  2, d->binomvar,  tr("Binomial Distribution Variance"),                  this ) );
  add( new Function( "erf",       1, d->erf,       tr("Error Function"),                                  this ) );
  add( new Function( "erfc",      1, d->erfc,      tr("Complementary Error Function"),                    this ) );
  add( new Function( "hypercdf",  4, d->hypercdf,  tr("Hypergeometric Cumulative Distribution Function"), this ) );
  add( new Function( "hypermean", 3, d->hypermean, tr("Hypergeometric Distribution Mean"),                this ) );
  add( new Function( "hyperpmf",  4, d->hyperpmf,  tr("Hypergeometric Probability Mass Function"),        this ) );
  add( new Function( "hypervar",  3, d->hypervar,  tr("Hypergeometric Distribution Variance"),            this ) );
  add( new Function( "poicdf",    2, d->poicdf,    tr("Poissonian Cumulative Distribution Function"),     this ) );
  add( new Function( "poimean",   1, d->poimean,   tr("Poissonian Distribution Mean"),                    this ) );
  add( new Function( "poipmf",    2, d->poipmf,    tr("Poissonian Probability Mass Function"),            this ) );
  add( new Function( "poivar",    1, d->poivar,    tr("Poissonian Distribution Variance"),                this ) );

  // TRIGONOMETRY
  add( new Function( "acos",    1, d->acos,    tr("Arc Cosine"),     this ) );
  add( new Function( "asin",    1, d->asin,    tr("Arc Sine"),       this ) );
  add( new Function( "atan",    1, d->atan,    tr("Arc Tangent"),    this ) );
  add( new Function( "cos",     1, d->cos,     tr("Cosine"),         this ) );
  add( new Function( "cot",     1, d->cot,     tr("Cotangent"),      this ) );
  add( new Function( "csc",     1, d->csc,     tr("Cosecant"),       this ) );
  add( new Function( "degrees", 1, d->degrees, tr("Degrees of Arc"), this ) );
  add( new Function( "radians", 1, d->radians, tr("Radians"),        this ) );
  add( new Function( "sec",     1, d->sec,     tr("Secant"),         this ) );
  add( new Function( "sin",     1, d->sin,     tr("Sine"),           this ) );
  add( new Function( "tan",     1, d->tan,     tr("Tangent"),        this ) );

  // LOGIC
  add( new Function( "mask",   2, d->mask,   tr("Mask to a bit size"),     this ) );
  add( new Function( "unmask", 2, d->unmask, tr("Sign-extent a value"),    this ) );
  add( new Function( "not",    1, d->not_,   tr("Logical NOT"),            this ) );
  add( new Function( "and",       d->and_,   tr("Logical AND"),            this ) );
  add( new Function( "or",        d->or_,    tr("Logical OR"),             this ) );
  add( new Function( "xor",       d->xor_,   tr("Logical XOR"),            this ) );
  add( new Function( "shl",   2,  d->ashl,   tr("Arithmetic Shift Left"),  this ) );
  add( new Function( "shr",   2,  d->ashr,   tr("Arithmetic Shift Right"), this ) );
  add( new Function( "idiv",  2,  d->idiv,   tr("Integer Quotient"),       this ) );
  add( new Function( "mod",   2,  d->mod,    tr("Modulo"),                 this ) );
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


char Functions::angleMode()
{
  return d->angleMode;
}


void Functions::setAngleMode( char c )
{
  d->angleMode = c;
}
