// This file is part of the SpeedCrunch project
// Copyright (C) 2004-2006 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2007, 2009 Wolf Lammen
// Copyright (C) 2007-2009 Helder Correia <helder.pereira.correia@gmail.com>
// Copyright (C) 2009 Andreas Scherer <andreas_coder@freenet.de>
// Copyright (C) 2011 Enrico Rós <enrico.ros@gmail.com>
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

#include "core/functions.h"
#include "core/settings.h"
#include "math/hmath.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QHash>

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <numeric>

static Functions * s_functionsInstance = 0;

// FIXME destructor seems not to be called
static void s_deleteFunctions()
{
    delete s_functionsInstance;
}

struct Function::Private
{
    int argc;
    QString error;
    QString identifier;
    QString name;
    FunctionPtr ptr;

    Private() : argc( 0 ), error(), identifier(), name(), ptr( 0 ) {}
    const HNumber& checkErrorResult( const HNumber & );
};

const HNumber& Function::Private::checkErrorResult( const HNumber & n )
{
    if (error.isEmpty())
	{
        switch ( n.error() )
        {
            case NoOperand:
                error = Functions::tr("function %1 does not take NaN as an argument");
                break;
            case EvalUnstable:
                error = Functions::tr("computation in %1 is unstable and exceeds the limitations of SpeedCrunch");
                break;
            case Underflow:
                error = Functions::tr("underflow: tiny result of %1 is out of SpeedCrunch's number range");
                break;
            case Overflow:
                error = Functions::tr("overflow: huge result of %1 is out of SpeedCrunch's number range");
                break;
            case ZeroDivide:
                error = Functions::tr("function %1 is infinite for submitted argument(s)");
                break;
            case OutOfDomain:
                error = Functions::tr("function %1 is not defined for submitted argument(s)");
                break;
            case OutOfLogicRange:
                error = Functions::tr("logic overflow: result of %1 exceeds maximum of 256 bits");
                break;
            case OutOfIntegerRange:
                error = Functions::tr("integer overflow: result of %1 exceeds maximum limit for integers");
                break;
            case TooExpensive:
                error = Functions::tr("too time consuming computation in %1 was rejected");
                break;
            case BadLiteral:
            case InvalidPrecision:
            case InvalidParam:
                error = Functions::tr("bug: internal error in %1 that should never occur");
                break;
            default:;
        }
        if (!error.isEmpty())
            error = error.arg(identifier);
    }
    return n;
}

Function::Function( const QString & identifier, FunctionPtr ptr, int argc, QObject * parent )
    : QObject( parent ), d( new Function::Private )
{
    d->identifier = identifier;
    d->ptr = ptr;
    d->argc = argc;
}

Function::Function( const QString & identifier, FunctionPtr ptr, QObject * parent )
    : QObject( parent ), d( new Function::Private )
{
    d->identifier = identifier;
    d->ptr = ptr;
    d->argc = -1;
}

Function::~Function()
{
}

QString Function::name() const
{
    return d->name;
}

void Function::setName( const QString & name )
{
    d->name = name;
}

QString Function::error() const
{
    return d->error;
}

QString Function::identifier() const
{
    return d->identifier;
}

HNumber Function::exec( const QVector<HNumber> & args )
{
    d->error = QString();
    if ( ! d->ptr ) {
        setError( Functions::tr("cannot execute function") );
        return HMath::nan();
    }

    if ( d->argc >= 0 && args.count() != d->argc ) {
        setError( Functions::tr("function %1 accepts %n argument(s)", "", d->argc).arg(identifier()) );
        return HMath::nan();
    }

    return d->checkErrorResult((*d->ptr)( this, args ));
}

void Function::setError( const QString & error )
{
    d->error = error;
}

struct Functions::Private
{
    Functions * p;
    QHash<QString, Function *> functions;
    void createBuiltInFunctions();

    static HNumber abs( Function *, const QVector<HNumber> & args );
    static HNumber absdev( Function *, const QVector<HNumber> & args );
    static HNumber acos( Function *, const QVector<HNumber> & args );
    static HNumber and_( Function *, const QVector<HNumber> & args );
    static HNumber arcosh( Function *, const QVector<HNumber> & args );
    static HNumber arsinh( Function *, const QVector<HNumber> & args );
    static HNumber artanh( Function *, const QVector<HNumber> & args );
    static HNumber ashl( Function *, const QVector<HNumber> & args );
    static HNumber ashr( Function *, const QVector<HNumber> & args );
    static HNumber asin( Function *, const QVector<HNumber> & args );
    static HNumber atan( Function *, const QVector<HNumber> & args );
    static HNumber average( Function *, const QVector<HNumber> & args );
    static HNumber bin( Function *, const QVector<HNumber> & args );
    static HNumber binomcdf( Function *, const QVector<HNumber> & args );
    static HNumber binommean( Function *, const QVector<HNumber> & args );
    static HNumber binompmf( Function *, const QVector<HNumber> & args );
    static HNumber binomvar( Function *, const QVector<HNumber> & args );
    static HNumber cbrt( Function *, const QVector<HNumber>& args );
    static HNumber ceil( Function *, const QVector<HNumber> & args );
    static HNumber cos( Function *, const QVector<HNumber> & args );
    static HNumber cosh( Function *, const QVector<HNumber> & args );
    static HNumber cot( Function *, const QVector<HNumber> & args );
    static HNumber csc( Function *, const QVector<HNumber> & args );
    static HNumber dec( Function *, const QVector<HNumber> & args );
    static HNumber degrees( Function *, const QVector<HNumber> & args );
    static HNumber dms( Function *, const QVector<HNumber> & args );
    static HNumber erfc( Function *, const QVector<HNumber> & args );
    static HNumber erf( Function *, const QVector<HNumber> & args );
    static HNumber exp( Function *, const QVector<HNumber>& args );
    static HNumber floor( Function *, const QVector<HNumber> & args );
    static HNumber frac( Function *, const QVector<HNumber> & args );
    static HNumber fromdms( Function *, const QVector<HNumber> & args );
    static HNumber Gamma( Function *, const QVector<HNumber> & args );
    static HNumber gcd( Function *, const QVector<HNumber>& args );
    static HNumber geomean( Function *, const QVector<HNumber> & args );
    static HNumber hex( Function *, const QVector<HNumber> & args );
    static HNumber hypercdf( Function *, const QVector<HNumber> & args );
    static HNumber hypermean( Function *, const QVector<HNumber> & args );
    static HNumber hyperpmf( Function *, const QVector<HNumber> & args );
    static HNumber hypervar( Function *, const QVector<HNumber> & args );
    static HNumber idiv( Function *, const QVector<HNumber> & args );
    static HNumber integer( Function *, const QVector<HNumber> & args );
    static HNumber lg( Function *, const QVector<HNumber>& args );
    static HNumber ln( Function *, const QVector<HNumber>& args );
    static HNumber lnGamma( Function *, const QVector<HNumber> & args );
    static HNumber log( Function *, const QVector<HNumber> & args );
    static HNumber mask( Function *, const QVector<HNumber> & args );
    static HNumber max( Function *, const QVector<HNumber> & args );
    static HNumber median( Function *, const QVector<HNumber> & args );
    static HNumber min( Function *, const QVector<HNumber> & args );
    static HNumber mod( Function *, const QVector<HNumber> & args );
    static HNumber nCr( Function *, const QVector<HNumber> & args );
    static HNumber not_( Function *, const QVector<HNumber> & args );
    static HNumber nPr( Function *, const QVector<HNumber> & args );
    static HNumber oct( Function *, const QVector<HNumber> & args );
    static HNumber or_( Function *, const QVector<HNumber> & args );
    static HNumber poicdf( Function *, const QVector<HNumber> & args );
    static HNumber poimean( Function *, const QVector<HNumber> & args );
    static HNumber poipmf( Function *, const QVector<HNumber> & args );
    static HNumber poivar( Function *, const QVector<HNumber> & args );
    static HNumber product( Function *, const QVector<HNumber> & args );
    static HNumber radians( Function *, const QVector<HNumber> & args );
    static HNumber round( Function *, const QVector<HNumber>& args );
    static HNumber sec( Function *, const QVector<HNumber> & args );
    static HNumber sign( Function *, const QVector<HNumber> & args );
    static HNumber sin( Function *, const QVector<HNumber> & args );
    static HNumber sinh( Function *, const QVector<HNumber> & args );
    static HNumber sqrt( Function *, const QVector<HNumber> & args );
    static HNumber stddev( Function *, const QVector<HNumber> & args );
    static HNumber sum( Function *, const QVector<HNumber> & args );
    static HNumber tan( Function *, const QVector<HNumber> & args );
    static HNumber tanh( Function *, const QVector<HNumber> & args );
    static HNumber trunc( Function *, const QVector<HNumber> & args );
    static HNumber unmask( Function *, const QVector<HNumber> & args );
    static HNumber variance( Function *, const QVector<HNumber> & args );
    static HNumber xor_( Function *, const QVector<HNumber> & args );
};

HNumber Functions::Private::abs( Function *, const QVector<HNumber> & args )
{
    return HMath::abs( args.at(0) );
}

HNumber Functions::Private::absdev( Function * f, const QVector<HNumber> &args )
{
    if ( args.count() < 1 )
        return HMath::nan();

    HNumber mean = average( f, args );
    if ( mean.isNan() )
        return HMath::nan();

    HNumber acc = 0;
    for ( int i = 0; i < args.count(); i++ )
        acc += HMath::abs( args.at(i) - mean );

    return acc / HNumber( args.count() );
}

HNumber Functions::Private::integer( Function *, const QVector<HNumber> & args )
{
    return HMath::integer( args.at(0) );
}

HNumber Functions::Private::trunc( Function * f, const QVector<HNumber> & args )
{
    int nArgs = args.count();

    if ( nArgs != 1 && nArgs != 2 ) {
        f->setError( Functions::tr("function requires 1 or 2 arguments") );
        return HMath::nan();
    }

    HNumber num = args.at( 0 );

    if ( nArgs == 2 ) {
        int prec = 0;
        HNumber argprec = args.at( 1 );
        if ( argprec != 0 ) {
            if ( ! argprec.isInteger() ) {
                f->setError( Functions::tr("function undefined for specified arguments") );
                return HMath::nan();
            }
            if ( (prec = argprec.toInt()) != 0 )
                return HMath::trunc( num, prec );
            // the 2. parameter exceeds the integer limits
            if ( argprec < 0 )
                return HNumber( 0 );

            return num;
        }
    }

    return HMath::trunc( num );
}

HNumber Functions::Private::frac( Function *, const QVector<HNumber> & args )
{
    return HMath::frac( args.at(0) );
}

HNumber Functions::Private::floor( Function *, const QVector<HNumber> & args )
{
    return HMath::floor( args.at(0) );
}

HNumber Functions::Private::ceil( Function *, const QVector<HNumber> & args )
{
    return HMath::ceil( args.at(0) );
}

HNumber Functions::Private::gcd( Function * f, const QVector<HNumber> & args )
{
    int nArgs = args.count();

    if ( nArgs < 2 ) {
        f->setError( Functions::tr("function requires at least 2 arguments") );
        return HMath::nan();
    }

    for ( int i = 0; i < args.count(); i++ )
        if ( ! args.at(i).isInteger() ) {
            f->setError( Functions::tr("function requires integer arguments") );
            return HMath::nan();
        }

    return std::accumulate( args.begin() + 1, args.end(), args.at(0), HMath::gcd );
}

HNumber Functions::Private::round( Function * f, const QVector<HNumber> & args )
{
    int nArgs = args.count();

    if ( nArgs != 1 && nArgs != 2 ) {
        f->setError( Functions::tr("function requires 1 or 2 arguments") );
        return HMath::nan();
    }

    HNumber num = args.at( 0 );

    if ( nArgs == 2 ) {
        int prec = 0;
        HNumber argprec = args.at( 1 );
        if ( argprec != 0 ) {
            if ( ! argprec.isInteger() ) {
                f->setError( Functions::tr("function undefined for specified arguments") );
                return HMath::nan();
            }
            if ( (prec = argprec.toInt()) != 0 )
                return HMath::round( num, prec );
            // the 2. parameter exceeds the integer limits
            if ( argprec < 0 )
                return HNumber( 0 );

            return num;
        }
    }

    return HMath::round( num );
}

HNumber Functions::Private::sqrt( Function *, const QVector<HNumber> & args )
{
    return HMath::sqrt( args.at( 0 ) );
}

HNumber Functions::Private::stddev( Function * f, const QVector<HNumber> & args )
{
    return args.count() < 1 ? HMath::nan() : HMath::sqrt( variance( f, args ));
}

HNumber Functions::Private::cbrt( Function *, const QVector<HNumber> & args )
{
    return HMath::cbrt( args.at(0) );
}

HNumber Functions::Private::exp( Function *, const QVector<HNumber> & args )
{
    return HMath::exp( args.at(0) );
}

HNumber Functions::Private::ln( Function *, const QVector<HNumber> & args )
{
    return HMath::ln( args.at( 0 ) );
}

HNumber Functions::Private::log( Function *, const QVector<HNumber> & args )
{
    return HMath::log( args.at( 0 ) );
}

HNumber Functions::Private::lg( Function *, const QVector<HNumber> & args )
{
    return HMath::lg( args.at( 0 ) );
}

HNumber Functions::Private::sin( Function *, const QVector<HNumber> & args )
{
    HNumber angle = args.at( 0 );

    if ( Settings::instance()->angleUnit == 'd' )
        angle = HMath::deg2rad( angle );

    return HMath::sin( angle );
}

HNumber Functions::Private::cos( Function *, const QVector<HNumber> & args )
{
    HNumber angle = args.at( 0 );

    if ( Settings::instance()->angleUnit == 'd' )
        angle = HMath::deg2rad( angle );

	return HMath::cos( angle );
}

HNumber Functions::Private::tan( Function *, const QVector<HNumber> & args )
{
    HNumber angle = args.at( 0 );

    if ( Settings::instance()->angleUnit == 'd' )
        angle = HMath::deg2rad( angle );

    return HMath::tan( angle );
}

HNumber Functions::Private::cot( Function *, const QVector<HNumber> & args )
{
    HNumber angle = args.at( 0 );

    if ( Settings::instance()->angleUnit == 'd' )
        angle = HMath::deg2rad( angle );

    return HMath::cot( angle );
}

HNumber Functions::Private::sec( Function *, const QVector<HNumber> & args )
{
    HNumber angle = args.at( 0 );

    if ( Settings::instance()->angleUnit == 'd' )
        angle = HMath::deg2rad( angle );

    return HMath::sec( angle );
}

HNumber Functions::Private::csc( Function *, const QVector<HNumber> & args )
{

    HNumber angle = args.at( 0 );

    if ( Settings::instance()->angleUnit == 'd' )
        angle = HMath::deg2rad( angle );

    return HMath::csc( angle );
}

HNumber Functions::Private::asin( Function *, const QVector<HNumber> & args )
{
    HNumber result = HMath::asin( args.at( 0 ) );

    if ( Settings::instance()->angleUnit == 'd' )
        result = HMath::rad2deg( result );

    return result;
}

HNumber Functions::Private::acos( Function *, const QVector<HNumber> & args )
{
    HNumber result = HMath::acos( args.at( 0 ) );

    if ( Settings::instance()->angleUnit == 'd' )
        result = HMath::rad2deg( result );

    return result;
}

HNumber Functions::Private::atan( Function *, const QVector<HNumber> & args )
{
    HNumber result = HMath::atan( args.at( 0 ) );

    if ( Settings::instance()->angleUnit == 'd' )
        result = HMath::rad2deg( result );

    return result;
}

HNumber Functions::Private::fromdms( Function *, const QVector<HNumber> & args )
{
    HNumber result = HMath::dms2deg( args.at( 0 ), args.at( 1 ), args.at( 2 ) );

    if ( Settings::instance()->angleUnit == 'r' )
        result = HMath::deg2rad( result );

    return result;
}

HNumber Functions::Private::sinh( Function *, const QVector<HNumber> & args )
{
    return HMath::sinh( args.at(0) );
}

HNumber Functions::Private::cosh( Function *, const QVector<HNumber> & args )
{
    return HMath::cosh( args.at(0) );
}

HNumber Functions::Private::tanh( Function *, const QVector<HNumber> & args )
{
    return HMath::tanh( args.at(0) );
}

HNumber Functions::Private::arsinh( Function *, const QVector<HNumber> & args )
{
    return HMath::arsinh( args.at(0) );
}

HNumber Functions::Private::arcosh( Function *, const QVector<HNumber> & args )
{
    return HMath::arcosh( args.at(0) );
}

HNumber Functions::Private::artanh( Function *, const QVector<HNumber> & args )
{
    return HMath::artanh( args.at(0) );
}

HNumber Functions::Private::erf( Function *, const QVector<HNumber> & args )
{
    return HMath::erf( args.at(0) );
}

HNumber Functions::Private::erfc( Function *, const QVector<HNumber> & args )
{
    return HMath::erfc( args.at(0) );
}

HNumber Functions::Private::Gamma( Function *, const QVector<HNumber> & args )
{
    return HMath::gamma( args.at(0) );
}

HNumber Functions::Private::lnGamma( Function *, const QVector<HNumber> & args )
{
    return HMath::lnGamma( args.at(0) );
}

HNumber Functions::Private::sign( Function *, const QVector<HNumber> & args )
{
    return HMath::sign( args.at(0) );
}

HNumber Functions::Private::nCr( Function *, const QVector<HNumber> & args )
{
    // arg 0: n; arg 1: r
    return HMath::nCr( args.at(0), args.at(1) );
}

HNumber Functions::Private::nPr( Function *, const QVector<HNumber> & args )
{
    // arg 0: n; arg 1: r
    return HMath::nPr( args.at(0), args.at(1) );
}

HNumber Functions::Private::degrees( Function *, const QVector<HNumber> & args )
{
    return HMath::rad2deg( args.at(0) );
}

HNumber Functions::Private::radians( Function *, const QVector<HNumber> & args )
{
    return HMath::deg2rad( args.at(0) );
}

HNumber Functions::Private::dms( Function *, const QVector<HNumber> & args )
{
    return HNumber( args.at(0) ).setFormat( 'm' );
}

HNumber Functions::Private::max( Function * f, const QVector<HNumber> & args )
{
    if ( args.count() < 1 ) {
        f->setError( Functions::tr("function requires at least 1 argument") );
        return HMath::nan();
    }

    return *std::max_element( args.begin(), args.end() );
}

HNumber Functions::Private::median(Function *, const QVector<HNumber> & args)
{
    const int nArgs = args.count();
    if ( nArgs < 1 )
        return HMath::nan();

    QVector<HNumber> sortedArgs = args;
    qSort(sortedArgs);

    if ((nArgs & 1) == 1)
        return sortedArgs.at( (nArgs - 1) / 2 );

    const int centerLeft = nArgs / 2 - 1;
    return ( sortedArgs.at( centerLeft ) + sortedArgs.at( centerLeft + 1 ) ) / HNumber( 2 );
}

HNumber Functions::Private::min( Function * f, const QVector<HNumber> & args )
{
    if ( args.count() < 1 ) {
        f->setError( Functions::tr("function requires at least 1 argument") );
        return HMath::nan();
    }

    return *std::min_element( args.begin(), args.end() );
}

HNumber Functions::Private::sum( Function *, const QVector<HNumber> & args )
{
    return args.count() < 1 ? HNumber( 0 )
        : std::accumulate( args.begin(), args.end(), HNumber(0) );
}

HNumber Functions::Private::product( Function *, const QVector<HNumber> & args )
{
    return args.count() < 1 ? HNumber( 1 )
        : std::accumulate( args.begin(), args.end(), HNumber(1), std::multiplies<HNumber>() );
}

HNumber Functions::Private::average( Function *, const QVector<HNumber> & args )
{
    return args.count() < 1 ? HNumber( "NaN" )
        : std::accumulate( args.begin(), args.end(), HNumber(0) ) / HNumber( args.count() );
}

HNumber Functions::Private::geomean( Function *, const QVector<HNumber> & args )
{
    if ( args.count() < 1 )
        return HNumber( "NaN" );

    HNumber result = std::accumulate( args.begin(), args.end(), HNumber(1),
                                      std::multiplies<HNumber>() );

    if ( result <= HNumber(0) )
        return HNumber( "NaN" );

    if ( args.count() == 1 )
        return result;

    if ( args.count() == 2 )
        return HMath::sqrt( result );

    return HMath::exp( HMath::ln(result) / HNumber(args.count()) );
}

HNumber Functions::Private::dec( Function *, const QVector<HNumber> & args )
{
    return HNumber( args.at(0) ).setFormat( 'g' );
}

HNumber Functions::Private::hex( Function *, const QVector<HNumber> & args )
{
    return HNumber( args.at(0) ).setFormat( 'h' );
}

HNumber Functions::Private::oct( Function *, const QVector<HNumber> & args )
{
    return HNumber( args.at(0) ).setFormat( 'o' );
}

HNumber Functions::Private::bin( Function *, const QVector<HNumber> & args )
{
    return HNumber( args.at(0) ).setFormat( 'b' );
}

HNumber Functions::Private::binompmf( Function *, const QVector<HNumber> & args )
{
    // arg 0: k; arg 1: n; arg 2: p
    return HMath::binomialPmf( args.at( 0 ), args.at( 1 ), args.at( 2 ) );
}

HNumber Functions::Private::binomcdf( Function *, const QVector<HNumber> & args )
{
    // arg 0: k; arg 1: n; arg 2: p
    return HMath::binomialCdf( args.at( 0 ), args.at( 1 ), args.at( 2 ) );
}

HNumber Functions::Private::binommean( Function *, const QVector<HNumber> & args )
{
    // arg 0: n; arg 1: p
    return HMath::binomialMean( args.at( 0 ), args.at( 1 ) );
}

HNumber Functions::Private::binomvar( Function *, const QVector<HNumber> & args )
{
    // arg 0: n; arg 1: p
    return HMath::binomialVariance( args.at( 0 ), args.at( 1 ) );
}

HNumber Functions::Private::hyperpmf( Function *, const QVector<HNumber> & args )
{
    // arg 0: k; arg 1: N; arg 2: M; arg3: n
    return HMath::hypergeometricPmf( args.at( 0 ), args.at( 1 ), args.at( 2 ), args.at( 3 ) );
}

HNumber Functions::Private::hypercdf( Function *, const QVector<HNumber> & args )
{
    // arg 0: k; arg 1: N; arg 2: M; arg3: n
    return HMath::hypergeometricCdf( args.at( 0 ), args.at( 1 ), args.at( 2 ), args.at( 3 ) );
}

HNumber Functions::Private::hypermean( Function *, const QVector<HNumber> & args )
{
    // arg 0: N; arg 1: M; arg 2: n
    return HMath::hypergeometricMean( args.at( 0 ), args.at( 1 ), args.at( 2 ) );
}

HNumber Functions::Private::hypervar( Function *, const QVector<HNumber> & args )
{
    // arg 0: N; arg 1: M; arg 2: n
    return HMath::hypergeometricVariance( args.at( 0 ), args.at( 1 ), args.at( 2 ) );
}

HNumber Functions::Private::poipmf( Function *, const QVector<HNumber> & args )
{
    // arg 0: k; arg 1: l
    return HMath::poissonPmf( args.at( 0 ), args.at( 1 ) );
}

HNumber Functions::Private::poicdf( Function *, const QVector<HNumber> & args )
{
    // arg 0: k; arg 1: l
    return HMath::poissonCdf( args.at( 0 ), args.at( 1 ) );
}

HNumber Functions::Private::poimean( Function *, const QVector<HNumber> & args )
{
    return HMath::poissonMean( args.at( 0 ) );
}

HNumber Functions::Private::poivar( Function *, const QVector<HNumber> & args )
{
    return HMath::poissonVariance( args.at( 0 ) );
}

HNumber Functions::Private::mask( Function *, const QVector<HNumber> & args )
{
    // arg 0: value; arg 1: bits
    return HMath::mask( args.at( 0 ), args.at( 1 ) );
}

HNumber Functions::Private::unmask( Function *, const QVector<HNumber> & args )
{
    // arg 0: value; arg 1: bits
    return HMath::sgnext( args.at( 0 ), args.at( 1 ) );
}

HNumber Functions::Private::variance( Function * f, const QVector<HNumber> & args )
{
    if ( args.count() < 1 )
        return HMath::nan();

    HNumber mean = average( f, args );
    if ( mean.isNan() )
        return HMath::nan();

    HNumber acc = 0;
    for ( int i = 0; i < args.count(); i++ )
        acc += (args.at(i) - mean) * (args.at(i) - mean);

    return acc / HNumber( args.count() );
}

HNumber Functions::Private::not_( Function *, const QVector<HNumber> & args )
{
	return ~args.at( 0 );
}

HNumber Functions::Private::and_( Function *, const QVector<HNumber> & args )
{
    return args.count() < 1 ? HNumber( "NaN" )
        : std::accumulate( args.begin(), args.end(), HNumber(-1),
                           std::mem_fun_ref(&HNumber::operator&) );
}

HNumber Functions::Private::or_( Function *, const QVector<HNumber> & args )
{
    return args.count() < 1 ? HNumber( "NaN" )
        : std::accumulate( args.begin(), args.end(), HNumber(0),
                           std::mem_fun_ref(&HNumber::operator|) );
}

HNumber Functions::Private::xor_( Function *, const QVector<HNumber> & args )
{
    return args.count() < 1 ? HNumber( "NaN" )
        : std::accumulate( args.begin(), args.end(), HNumber(0),
                           std::mem_fun_ref(&HNumber::operator^) );
}

HNumber Functions::Private::ashl( Function *, const QVector<HNumber> & args )
{
    // arg 0: value; arg 1: bits
    return HMath::ashr( args.at( 0 ), -args.at( 1 ) );
}

HNumber Functions::Private::ashr( Function *, const QVector<HNumber> & args )
{
    // arg 0: value; arg 1: bits
    return HMath::ashr( args.at( 0 ), args.at( 1 ) );
}

HNumber Functions::Private::idiv( Function *, const QVector<HNumber> & args )
{
    // arg 0: dividend; arg 1: divisor
    return HMath::idiv(args.at(0), args.at(1));
}

HNumber Functions::Private::mod( Function *, const QVector<HNumber> & args )
{
    // arg 0: dividend; arg 1: divisor
    return args.at(0) % args.at(1);
}

void Functions::Private::createBuiltInFunctions()
{
    // ANALYSIS
    p->add( new Function("abs",     abs,     1, p) );
    p->add( new Function("absdev",  absdev,     p) );
    p->add( new Function("average", average,    p) );
    p->add( new Function("bin",     bin,     1, p) );
    p->add( new Function("cbrt",    cbrt,    1, p) );
    p->add( new Function("ceil",    ceil,    1, p) );
    p->add( new Function("dec",     dec,     1, p) );
    p->add( new Function("floor",   floor,   1, p) );
    p->add( new Function("frac",    frac,    1, p) );
    p->add( new Function("gamma",   Gamma,   1, p) );
    p->add( new Function("geomean", geomean,    p) );
    p->add( new Function("hex",     hex,     1, p) );
    p->add( new Function("int",     integer, 1, p) );
    p->add( new Function("lngamma", lnGamma, 1, p) );
    p->add( new Function("max",     max,        p) );
    p->add( new Function("min",     min,        p) );
    p->add( new Function("oct",     oct,     1, p) );
    p->add( new Function("product", product,    p) );
    p->add( new Function("round",   round,      p) );
    p->add( new Function("sign",    sign,    1, p) );
    p->add( new Function("sqrt",    sqrt,    1, p) );
    p->add( new Function("stddev",  stddev,     p) );
    p->add( new Function("sum",     sum,        p) );
    p->add( new Function("trunc",   trunc,      p) );
    p->add( new Function("variance",variance,   p) );

    // LOGARITHM
    p->add( new Function("arcosh", arcosh, 1, p) );
    p->add( new Function("arsinh", arsinh, 1, p) );
    p->add( new Function("artanh", artanh, 1, p) );
    p->add( new Function("cosh",   cosh,   1, p) );
    p->add( new Function("exp",    exp,    1, p) );
    p->add( new Function("lg",     lg,     1, p) );
    p->add( new Function("ln",     ln,     1, p) );
    p->add( new Function("log",    log,    1, p) );
    p->add( new Function("sinh",   sinh,   1, p) );
    p->add( new Function("tanh",   tanh,   1, p) );

    // DISCRETE
    p->add( new Function("gcd", gcd,    p) );
    p->add( new Function("ncr", nCr, 2, p) );
    p->add( new Function("npr", nPr, 2, p) );

    // PROBABILITY
    p->add( new Function("binomcdf",  binomcdf,  3, p) );
    p->add( new Function("binommean", binommean, 2, p) );
    p->add( new Function("binompmf",  binompmf,  3, p) );
    p->add( new Function("binomvar",  binomvar,  2, p) );
    p->add( new Function("erf",       erf,       1, p) );
    p->add( new Function("erfc",      erfc,      1, p) );
    p->add( new Function("hypercdf",  hypercdf,  4, p) );
    p->add( new Function("hypermean", hypermean, 3, p) );
    p->add( new Function("hyperpmf",  hyperpmf,  4, p) );
    p->add( new Function("hypervar",  hypervar,  3, p) );
    p->add( new Function("median",    median,       p) );
    p->add( new Function("poicdf",    poicdf,    2, p) );
    p->add( new Function("poimean",   poimean,   1, p) );
    p->add( new Function("poipmf",    poipmf,    2, p) );
    p->add( new Function("poivar",    poivar,    1, p) );

    // TRIGONOMETRY
    p->add( new Function("acos",    acos,    1, p) );
    p->add( new Function("asin",    asin,    1, p) );
    p->add( new Function("atan",    atan,    1, p) );
    p->add( new Function("cos",     cos,     1, p) );
    p->add( new Function("cot",     cot,     1, p) );
    p->add( new Function("csc",     csc,     1, p) );
    p->add( new Function("degrees", degrees, 1, p) );
    p->add( new Function("radians", radians, 1, p) );
    p->add( new Function("dms"    , dms,     1, p) );
    p->add( new Function("fromdms", fromdms, 3, p) );
    p->add( new Function("sec",     sec,     1, p) );
    p->add( new Function("sin",     sin,     1, p) );
    p->add( new Function("tan",     tan,     1, p) );

    // LOGIC
    p->add( new Function("mask",   mask,   2, p) );
    p->add( new Function("unmask", unmask, 2, p) );
    p->add( new Function("not",    not_,   1, p) );
    p->add( new Function("and",    and_,      p) );
    p->add( new Function("or",     or_,       p) );
    p->add( new Function("xor",    xor_,      p) );
    p->add( new Function("shl",    ashl,   2, p) );
    p->add( new Function("shr",    ashr,   2, p) );
    p->add( new Function("idiv",   idiv,   2, p) );
    p->add( new Function("mod",    mod,    2, p) );
}

Functions * Functions::instance()
{
    if ( ! s_functionsInstance ) {
        s_functionsInstance = new Functions;
        qAddPostRoutine( s_deleteFunctions );
    }

    return s_functionsInstance;
}

Functions::Functions()
    : d( new Functions::Private )
{
    d->p = this;
    d->createBuiltInFunctions();
    retranslateText();
}

void Functions::add( Function * f )
{
    if ( f )
        d->functions.insert( f->identifier().toUpper(), f );
}

Function * Functions::function( const QString & identifier ) const
{
    return d->functions.value( identifier.toUpper(), 0 );
}

QStringList Functions::names() const
{
    QStringList result = d->functions.keys();
    std::transform( result.begin(), result.end(), result.begin(),
                    std::mem_fun_ref(&QString::toLower) );
    return result;
}

Functions::~Functions()
{
}

void Functions::retranslateText()
{
    // ANALYSIS
    function( "abs"     )->setName( tr("Absolute Value") );
    function( "absdev"  )->setName( tr("Absolute Deviation") );
    function( "average" )->setName( tr("Average (Arithmetic Mean)") );
    function( "bin"     )->setName( tr("Binary Representation") );
    function( "cbrt"    )->setName( tr("Cube Root") );
    function( "ceil"    )->setName( tr("Ceiling") );
    function( "dec"     )->setName( tr("Decimal Representation") );
    function( "floor"   )->setName( tr("Floor") );
    function( "frac"    )->setName( tr("Fractional Part") );
    function( "gamma"   )->setName( tr("Extension of Factorials [= (x-1)!]") );
    function( "geomean" )->setName( tr("Geometric Mean") );
    function( "hex"     )->setName( tr("Hexadecimal Representation") );
    function( "int"     )->setName( tr("Integer Part") );
    function( "lngamma" )->setName( tr("ln(abs(Gamma))") );
    function( "max"     )->setName( tr("Maximum") );
    function( "min"     )->setName( tr("Minimum") );
    function( "oct"     )->setName( tr("Octal Representation") );
    function( "product" )->setName( tr("Product") );
    function( "round"   )->setName( tr("Rounding") );
    function( "sign"    )->setName( tr("Signum") );
    function( "sqrt"    )->setName( tr("Square Root") );
    function( "stddev"  )->setName( tr("Standard Deviation (Square Root of Variance)") );
    function( "sum"     )->setName( tr("Sum") );
    function( "trunc"   )->setName( tr("Truncation") );
    function( "variance")->setName( tr("Variance") );

    //// LOGARITHM
    function( "arcosh" )->setName( tr("Area Hyperbolic Cosine") );
    function( "arsinh" )->setName( tr("Area Hyperbolic Sine") );
    function( "artanh" )->setName( tr("Area Hyperbolic Tangent") );
    function( "cosh"   )->setName( tr("Hyperbolic Cosine") );
    function( "exp"    )->setName( tr("Exponential") );
    function( "lg"     )->setName( tr("Base-2 Logarithm") );
    function( "ln"     )->setName( tr("Natural Logarithm") );
    function( "log"    )->setName( tr("Base-10 Logarithm") );
    function( "sinh"   )->setName( tr("Hyperbolic Sine") );
    function( "tanh"   )->setName( tr("Hyperbolic Tangent") );

    //// DISCRETE
    function( "gcd" )->setName( tr("Greatest Common Divisor") );
    function( "ncr" )->setName( tr("Combination (Binomial Coefficient)") );
    function( "npr" )->setName( tr("Permutation (Arrangement)") );

    //// PROBABILITY
    function( "binomcdf"  )->setName( tr("Binomial Cumulative Distribution Function") );
    function( "binommean" )->setName( tr("Binomial Distribution Mean") );
    function( "binompmf"  )->setName( tr("Binomial Probability Mass Function") );
    function( "binomvar"  )->setName( tr("Binomial Distribution Variance") );
    function( "erf"       )->setName( tr("Error Function") );
    function( "erfc"      )->setName( tr("Complementary Error Function") );
    function( "hypercdf"  )->setName( tr("Hypergeometric Cumulative Distribution Function") );
    function( "hypermean" )->setName( tr("Hypergeometric Distribution Mean") );
    function( "hyperpmf"  )->setName( tr("Hypergeometric Probability Mass Function") );
    function( "hypervar"  )->setName( tr("Hypergeometric Distribution Variance") );
    function( "median"    )->setName( tr("Median Value (50th Percentile)") );
    function( "poicdf"    )->setName( tr("Poissonian Cumulative Distribution Function") );
    function( "poimean"   )->setName( tr("Poissonian Distribution Mean") );
    function( "poipmf"    )->setName( tr("Poissonian Probability Mass Function") );
    function( "poivar"    )->setName( tr("Poissonian Distribution Variance") );

    //// TRIGONOMETRY
    function( "acos"    )->setName( tr("Arc Cosine") );
    function( "asin"    )->setName( tr("Arc Sine") );
    function( "atan"    )->setName( tr("Arc Tangent") );
    function( "cos"     )->setName( tr("Cosine") );
    function( "cot"     )->setName( tr("Cotangent") );
    function( "csc"     )->setName( tr("Cosecant") );
    function( "degrees" )->setName( tr("Degrees of Arc") );
    function( "radians" )->setName( tr("Radians") );
    function( "dms"     )->setName( tr("Degree-Minute-Second") );
    function( "fromdms" )->setName( tr("Degrees of Arc") );
    function( "sec"     )->setName( tr("Secant") );
    function( "sin"     )->setName( tr("Sine") );
    function( "tan"     )->setName( tr("Tangent") );

    //// LOGIC
    function( "mask"   )->setName( tr("Mask to a bit size") );
    function( "unmask" )->setName( tr("Sign-extent a value") );
    function( "not"    )->setName( tr("Logical NOT") );
    function( "and"    )->setName( tr("Logical AND") );
    function( "or"     )->setName( tr("Logical OR") );
    function( "xor"    )->setName( tr("Logical XOR") );
    function( "shl"    )->setName( tr("Arithmetic Shift Left") );
    function( "shr"    )->setName( tr("Arithmetic Shift Right") );
    function( "idiv"   )->setName( tr("Integer Quotient") );
    function( "mod"    )->setName( tr("Modulo") );
}

