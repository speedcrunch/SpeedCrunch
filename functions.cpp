/* This file is part of the SpeedCrunch project
   Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "evaluator.h"
#include "functions.h"
#include "hmath.h"

#include <QApplication>
#include <QHash>
#include <QObject>
#include <QStringList>
#include <QVector>

#include <float.h>
#include <math.h>

HNumber deg2rad( HNumber x )
{
  return x * HMath::pi() / 180;
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

HNumber function_frac( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::frac( num );
}

HNumber function_round( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  return HMath::round( num );
}

HNumber function_sqrt( const Evaluator*, Function* fn, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  if( num < 0 )
  {
    fn->setError( qApp->translate( "Error",
      "Function sqrt expects positive argument" ) );
    return HNumber::nan();
  }

  return HMath::sqrt( num );
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

  HNumber num = args[0];
  if( num < 0 )
  {
    fn->setError( qApp->translate( "Error",
      "Function ln expects positive argument" ) );
    return HNumber( 0 );
  }

  return HMath::ln( num );
}

HNumber function_log( const Evaluator*, Function* fn, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  if( num < 0 )
  {
    fn->setError( qApp->translate( "Error",
      "Function log expects positive argument" ) );
    return HNumber( 0 );
  }

  return HMath::log( num );
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

  HNumber halfpi = HMath::pi() / HNumber(2);
  if( ( angle == halfpi ) || ( angle == halfpi*3 ) )
  {
    fn->setError( qApp->translate( "Error",
      "Invalid input to function tan" ) );
    return HNumber::nan();
  }

  return HMath::tan( angle );
}

HNumber function_asin( const Evaluator* eval, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  HNumber angle = HMath::asin( num );
  if( eval->angleMode() == Evaluator::Degree )
    angle = rad2deg( angle );

  return angle;
}

HNumber function_acos( const Evaluator* eval, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber num = args[0];
  HNumber angle = HMath::acos( num );
  if( eval->angleMode() == Evaluator::Degree )
    angle = rad2deg( angle );

  return angle;
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

  HNumber angle = args[0];
  if( eval->angleMode() == Evaluator::Degree )
    angle = deg2rad( angle );
  return HMath::sinh( angle );
}

HNumber function_cosh( const Evaluator* eval, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  if( eval->angleMode() == Evaluator::Degree )
    angle = deg2rad( angle );
  return HMath::cosh( angle );
}

HNumber function_tanh( const Evaluator* eval, Function*, const FunctionArguments& args )
{
  if( args.count() != 1 )
    return HNumber::nan();

  HNumber angle = args[0];
  if( eval->angleMode() == Evaluator::Degree )
    angle = deg2rad( angle );
  return HMath::tanh( angle );
}

HNumber function_max( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() <= 0 )
    return HNumber::nan();
    
  HNumber result = args[0];
  for( int c = 1; c < args.count(); c++ )
    if( HMath::compare( result, args[c] ) == -1 )
      result = args[c];
      
  return result;
}

HNumber function_min( const Evaluator*, Function*, const FunctionArguments& args )
{
  if( args.count() <= 0 )
    return HNumber::nan();
    
  HNumber result = args[0];
  for( int c = 1; c < args.count(); c++ )
    if( HMath::compare( result, args[c] ) == 1 )
      result = args[c];
      
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

class Function::Private
{
public:
  QString name;
  int argc;
  QString desc;
  QString error;
  FunctionPtr ptr;
};

class FunctionRepository::Private
{
public:
  QHash<QString, Function*> functions;
};

Function::Function( const QString& name, int argc, FunctionPtr ptr, const QString& desc )
{
  d = new Private;
  d->name = name;
  d->argc = argc;
  d->desc = qApp->translate( "FunctionRepository", desc );
  d->ptr = ptr;
}

Function::Function( const QString& name, FunctionPtr ptr, const QString& desc )
{
  d = new Private;
  d->name = name;
  d->argc = -1;
  d->desc = qApp->translate( "FunctionRepository", desc );
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

void Function::setError( const QString& e )
{
  d->error = e;
}

HNumber Function::exec( const Evaluator* eval, const FunctionArguments& args )
{
  d->error = QString::null;
  if( !d->ptr )
  {
    setError( QString( qApp->translate( "Error",
      "Cannot execute function %1") ).arg( name() ) );
    return 0;
  }

  if( d->argc >= 0 )
  if( args.count() != d->argc )
  {
    setError( QString( qApp->translate( "Error",
      "Function %1 accepts %2 argument" ) ).arg( d->name ).arg( d->argc ) );
    return 0;
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
  d = new Private;

  add( new Function( "abs",   1, function_abs, QT_TR_NOOP("Absolute" ) ) );
  add( new Function( "int",   1, function_int, QT_TR_NOOP("Integer" ) ) );
  add( new Function( "frac",  1, function_frac, QT_TR_NOOP("Fraction" ) ) );
  add( new Function( "round", 1, function_round, QT_TR_NOOP("Round" ) ) );
  add( new Function( "sqrt",  1, function_sqrt, QT_TR_NOOP("Square Root" ) ) );
  add( new Function( "exp",   1, function_exp, QT_TR_NOOP("Exponent" ) ) );
  add( new Function( "ln",    1, function_ln, QT_TR_NOOP("Natural Logarithm" ) ) );
  add( new Function( "log",   1, function_log, QT_TR_NOOP("Base-10 Logarithm" ) ) );
  
  add( new Function( "sin",   1, function_sin, QT_TR_NOOP("Sine" ) ) );
  add( new Function( "cos",   1, function_cos, QT_TR_NOOP("Cosine" ) ) );
  add( new Function( "tan",   1, function_tan, QT_TR_NOOP("Tangent" ) ) );
  add( new Function( "asin",  1, function_asin, QT_TR_NOOP("Arc Sine" ) ) );
  add( new Function( "acos",  1, function_acos, QT_TR_NOOP("Arc Cosine" ) ) );
  add( new Function( "atan",  1, function_atan, QT_TR_NOOP("Arc Tangent" ) ) );
  add( new Function( "sinh",  1, function_sinh, QT_TR_NOOP("Hyperbolic Sine" ) ) );
  add( new Function( "cosh",  1, function_cosh, QT_TR_NOOP("Hyperbolic Cosine" ) ) );
  add( new Function( "tanh",  1, function_tanh, QT_TR_NOOP("Hyperbolic Tangent" ) ) );

  add( new Function( "min",  function_min, QT_TR_NOOP("Minimum" ) ) );
  add( new Function( "max",  function_max, QT_TR_NOOP("Maximum" ) ) );
  add( new Function( "sum",  function_sum, QT_TR_NOOP("Sum" ) ) );
  add( new Function( "product",  function_product, QT_TR_NOOP("Product" ) ) );
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
  d->functions.insert( function->name().upper(), function );
}

Function* FunctionRepository::function( const QString& name )
{
  return d->functions[ name.upper() ];
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
