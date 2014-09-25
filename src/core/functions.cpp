// This file is part of the SpeedCrunch project
// Copyright (C) 2004-2006 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2007, 2009 Wolf Lammen
// Copyright (C) 2007-2009, 2013, 2014 Helder Correia <helder.pereira.correia@gmail.com>
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

#include <QCoreApplication>
#include <QHash>

#include <algorithm>
#include <functional>
#include <cfloat>
#include <cmath>
#include <numeric>

#define FUNCTION_INSERT(ID) insert(new Function(#ID, function_ ## ID, this))
#define FUNCTION_USAGE(ID, USAGE) find(#ID)->setUsage(QString::fromLatin1(USAGE));
#define FUNCTION_USAGE_TR(ID, USAGE) find(#ID)->setUsage(USAGE);
#define FUNCTION_NAME(ID, NAME) find(#ID)->setName(NAME)

#define ENSURE_POSITIVE_ARGUMENT_COUNT() \
    if (args.count() < 1) { \
        f->setError(InvalidParamCount); \
        return HMath::nan(); \
    }

#define ENSURE_ARGUMENT_COUNT(i) \
    if (args.count() != (i)) { \
        f->setError(InvalidParamCount); \
        return HMath::nan(); \
    }

#define ENSURE_EITHER_ARGUMENT_COUNT(i, j) \
    if (args.count() != (i) && args.count() != (j)) { \
        f->setError(InvalidParamCount); \
        return HMath::nan(); \
    }

static FunctionRepo* s_FunctionRepoInstance = 0;

// FIXME: destructor seems not to be called
static void s_deleteFunctions()
{
    delete s_FunctionRepoInstance;
}

HNumber Function::exec(const Function::ArgumentList& args)
{
    if (!m_ptr)
        return HMath::nan();
    setError(Success);
    HNumber result = (*m_ptr)(this, args);
    return result;
}

HNumber function_abs(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::abs(args.at(0));
}

HNumber function_average(Function* f, const Function::ArgumentList& args)
{
    ENSURE_POSITIVE_ARGUMENT_COUNT();
    return std::accumulate(args.begin(), args.end(), HNumber(0)) / HNumber(args.count());
}

HNumber function_absdev(Function* f, const Function::ArgumentList& args)
{
    ENSURE_POSITIVE_ARGUMENT_COUNT();
    HNumber mean = function_average(f, args);
    if (mean.isNan())
        return HMath::nan();
    HNumber acc = 0;
    for (int i = 0; i < args.count(); ++i)
        acc += HMath::abs(args.at(i) - mean);
    return acc / HNumber(args.count());
}

HNumber function_int(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::integer(args.at(0));
}

HNumber function_trunc(Function* f, const Function::ArgumentList& args)
{
    ENSURE_EITHER_ARGUMENT_COUNT(1, 2);
    HNumber num = args.at(0);
    if (args.count() == 2) {
        HNumber argprec = args.at(1);
        if (argprec != 0) {
            if (!argprec.isInteger()) {
                f->setError(OutOfDomain);
                return HMath::nan();
            }
            int prec = argprec.toInt();
            if (prec)
                return HMath::trunc(num, prec);
            // The second parameter exceeds the integer limits.
            if (argprec < 0)
                return HNumber(0);
            return num;
        }
    }
    return HMath::trunc(num);
}

HNumber function_frac(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::frac(args.at(0));
}

HNumber function_floor(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::floor(args.at(0));
}

HNumber function_ceil(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::ceil(args.at(0));
}

HNumber function_gcd(Function* f, const Function::ArgumentList& args)
{
    ENSURE_POSITIVE_ARGUMENT_COUNT();
    for (int i = 0; i < args.count(); ++i)
        if (!args.at(i).isInteger()) {
            f->setError(OutOfDomain);
            return HMath::nan();
        }
    return std::accumulate(args.begin() + 1, args.end(), args.at(0), HMath::gcd);
}

HNumber function_round(Function* f, const Function::ArgumentList& args)
{
    ENSURE_EITHER_ARGUMENT_COUNT(1, 2);
    HNumber num = args.at(0);
    if (args.count() == 2) {
        HNumber argPrecision = args.at(1);
        if (argPrecision != 0) {
            if (!argPrecision.isInteger()) {
                f->setError(OutOfDomain);
                return HMath::nan();
            }
            int prec = argPrecision.toInt();
            if (prec)
                return HMath::round(num, prec);
            // The second parameter exceeds the integer limits.
            if (argPrecision < 0)
                return HNumber(0);
            return num;
        }
    }
    return HMath::round(num);
}

HNumber function_sqrt(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::sqrt(args.at(0));
}

HNumber function_variance(Function* f, const Function::ArgumentList& args)
{
    ENSURE_POSITIVE_ARGUMENT_COUNT();

    HNumber mean = function_average(f, args);
    if (mean.isNan())
        return HMath::nan();

    HNumber acc = 0;
    for (int i = 0; i < args.count(); ++i)
        acc += (args.at(i) - mean) * (args.at(i) - mean);

    return acc / HNumber(args.count());
}

HNumber function_stddev(Function* f, const Function::ArgumentList& args)
{
    ENSURE_POSITIVE_ARGUMENT_COUNT();
    return HMath::sqrt(function_variance(f, args));
}

HNumber function_cbrt(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::cbrt(args.at(0));
}

HNumber function_exp(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::exp(args.at(0));
}

HNumber function_ln(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::ln(args.at(0));
}

HNumber function_lg(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::lg(args.at(0));
}

HNumber function_lb(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::lb(args.at(0));
}

HNumber function_log(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(2);
    return HMath::log(args.at(0), args.at(1));
}

HNumber function_sin(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    HNumber angle = args.at(0);
    if (Settings::instance()->angleUnit == 'd')
        angle = HMath::deg2rad(angle);
    return HMath::sin(angle);
}

HNumber function_cos(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    HNumber angle = args.at(0);
    if (Settings::instance()->angleUnit == 'd')
        angle = HMath::deg2rad(angle);
    return HMath::cos(angle);
}

HNumber function_tan(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    HNumber angle = args.at(0);
    if (Settings::instance()->angleUnit == 'd')
        angle = HMath::deg2rad(angle);
    return HMath::tan(angle);
}

HNumber function_cot(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    HNumber angle = args.at(0);
    if (Settings::instance()->angleUnit == 'd')
        angle = HMath::deg2rad(angle);
    return HMath::cot(angle);
}

HNumber function_sec(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    HNumber angle = args.at(0);
    if (Settings::instance()->angleUnit == 'd')
        angle = HMath::deg2rad(angle);
    return HMath::sec(angle);
}

HNumber function_csc(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    HNumber angle = args.at(0);
    if (Settings::instance()->angleUnit == 'd')
        angle = HMath::deg2rad(angle);
    return HMath::csc(angle);
}

HNumber function_arcsin(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    HNumber result = HMath::arcsin(args.at(0));
    if (Settings::instance()->angleUnit == 'd')
        result = HMath::rad2deg(result);
    return result;
}

HNumber function_arccos(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    HNumber result = HMath::arccos(args.at(0));
    if (Settings::instance()->angleUnit == 'd')
        result = HMath::rad2deg(result);
    return result;
}

HNumber function_arctan(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    HNumber result = HMath::arctan(args.at(0));
    if (Settings::instance()->angleUnit == 'd')
        result = HMath::rad2deg(result);
    return result;
}

HNumber function_sinh(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::sinh(args.at(0));
}

HNumber function_cosh(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::cosh(args.at(0));
}

HNumber function_tanh(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::tanh(args.at(0));
}

HNumber function_arsinh(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::arsinh(args.at(0));
}

HNumber function_arcosh(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::arcosh(args.at(0));
}

HNumber function_artanh(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::artanh(args.at(0));
}

HNumber function_erf(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::erf(args.at(0));
}

HNumber function_erfc(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::erfc(args.at(0));
}

HNumber function_gamma(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::gamma(args.at(0));
}

HNumber function_lngamma(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::lnGamma(args.at(0));
}

HNumber function_sgn(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::sgn(args.at(0));
}

HNumber function_ncr(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(2);
    return HMath::nCr(args.at(0), args.at(1));
}

HNumber function_npr(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(2);
    return HMath::nPr(args.at(0), args.at(1));
}

HNumber function_degrees(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::rad2deg(args.at(0));
}

HNumber function_radians(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::deg2rad(args.at(0));
}

HNumber function_max(Function* f, const Function::ArgumentList& args)
{
    ENSURE_POSITIVE_ARGUMENT_COUNT();
    return *std::max_element(args.begin(), args.end());
}

HNumber function_median(Function* f, const Function::ArgumentList& args)
{
    ENSURE_POSITIVE_ARGUMENT_COUNT();

    Function::ArgumentList sortedArgs = args;
    qSort(sortedArgs);

    if ((args.count() & 1) == 1)
        return sortedArgs.at((args.count() - 1) / 2);

    const int centerLeft = args.count() / 2 - 1;
    return (sortedArgs.at(centerLeft) + sortedArgs.at(centerLeft + 1)) / HNumber(2);
}

HNumber function_min(Function* f, const Function::ArgumentList& args)
{
    ENSURE_POSITIVE_ARGUMENT_COUNT();
    return *std::min_element(args.begin(), args.end());
}

HNumber function_sum(Function* f, const Function::ArgumentList& args)
{
    ENSURE_POSITIVE_ARGUMENT_COUNT();
    return std::accumulate(args.begin(), args.end(), HNumber(0));
}

HNumber function_product(Function* f, const Function::ArgumentList& args)
{
    ENSURE_POSITIVE_ARGUMENT_COUNT();
    return std::accumulate(args.begin(), args.end(), HNumber(1), std::multiplies<HNumber>());
}

HNumber function_geomean(Function* f, const Function::ArgumentList& args)
{
    ENSURE_POSITIVE_ARGUMENT_COUNT();

    HNumber result = std::accumulate(args.begin(), args.end(), HNumber(1),
        std::multiplies<HNumber>());

    if (result <= HNumber(0))
        return HNumber("NaN");

    if (args.count() == 1)
        return result;

    if (args.count() == 2)
        return HMath::sqrt(result);

    return HMath::exp(HMath::ln(result) / HNumber(args.count()));
}

HNumber function_dec(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HNumber(args.at(0)).setFormat('g');
}

HNumber function_hex(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HNumber(args.at(0)).setFormat('h');
}

HNumber function_oct(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HNumber(args.at(0)).setFormat('o');
}

HNumber function_bin(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HNumber(args.at(0)).setFormat('b');
}

HNumber function_binompmf(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(3);
    return HMath::binomialPmf(args.at(0), args.at(1), args.at(2));
}

HNumber function_binomcdf(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(3);
    return HMath::binomialCdf(args.at(0), args.at(1), args.at(2));
}

HNumber function_binommean(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(2);
    return HMath::binomialMean(args.at(0), args.at(1));
}

HNumber function_binomvar(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(2);
    return HMath::binomialVariance(args.at(0), args.at(1));
}

HNumber function_hyperpmf(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(4);
    return HMath::hypergeometricPmf(args.at(0), args.at(1), args.at(2), args.at(3));
}

HNumber function_hypercdf(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(4);
    return HMath::hypergeometricCdf(args.at(0), args.at(1), args.at(2), args.at(3));
}

HNumber function_hypermean(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(3);
    return HMath::hypergeometricMean(args.at(0), args.at(1), args.at(2));
}

HNumber function_hypervar(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(3);
    return HMath::hypergeometricVariance(args.at(0), args.at(1), args.at(2));
}

HNumber function_poipmf(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(2);
    return HMath::poissonPmf(args.at(0), args.at(1));
}

HNumber function_poicdf(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(2);
    return HMath::poissonCdf(args.at(0), args.at(1));
}

HNumber function_poimean(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::poissonMean(args.at(0));
}

HNumber function_poivar(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::poissonVariance(args.at(0));
}

HNumber function_mask(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(2);
    return HMath::mask(args.at(0), args.at(1));
}

HNumber function_gmask(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(2);
    return HMath::gmask(args.at(0), args.at(1));
}

HNumber function_unmask(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(2);
    return HMath::sgnext(args.at(0), args.at(1));
}

HNumber function_not(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
	return ~args.at(0);
}

HNumber function_and(Function* f, const Function::ArgumentList& args)
{
    ENSURE_POSITIVE_ARGUMENT_COUNT();
    return std::accumulate(args.begin(), args.end(), HNumber(-1),
        std::mem_fun_ref(&HNumber::operator&));
}

HNumber function_or(Function* f, const Function::ArgumentList& args)
{
    ENSURE_POSITIVE_ARGUMENT_COUNT();
    return std::accumulate(args.begin(), args.end(), HNumber(0),
        std::mem_fun_ref(&HNumber::operator|));
}

HNumber function_xor(Function* f, const Function::ArgumentList& args)
{
    ENSURE_POSITIVE_ARGUMENT_COUNT();
    return std::accumulate(args.begin(), args.end(), HNumber(0),
        std::mem_fun_ref(&HNumber::operator^));
}

HNumber function_shl(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(2);
    return HMath::ashr(args.at(0), -args.at(1));
}

HNumber function_shr(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(2);
    return HMath::ashr(args.at(0), args.at(1));
}

HNumber function_idiv(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(2);
    return HMath::idiv(args.at(0), args.at(1));
}

HNumber function_mod(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(2);
    return args.at(0) % args.at(1);
}

void FunctionRepo::createFunctions()
{
    // Analysis.
    FUNCTION_INSERT(abs);
    FUNCTION_INSERT(absdev);
    FUNCTION_INSERT(average);
    FUNCTION_INSERT(bin);
    FUNCTION_INSERT(cbrt);
    FUNCTION_INSERT(ceil);
    FUNCTION_INSERT(dec);
    FUNCTION_INSERT(floor);
    FUNCTION_INSERT(frac);
    FUNCTION_INSERT(gamma);
    FUNCTION_INSERT(geomean);
    FUNCTION_INSERT(hex);
    FUNCTION_INSERT(int);
    FUNCTION_INSERT(lngamma);
    FUNCTION_INSERT(max);
    FUNCTION_INSERT(min);
    FUNCTION_INSERT(oct);
    FUNCTION_INSERT(product);
    FUNCTION_INSERT(round);
    FUNCTION_INSERT(sgn);
    FUNCTION_INSERT(sqrt);
    FUNCTION_INSERT(stddev);
    FUNCTION_INSERT(sum);
    FUNCTION_INSERT(trunc);
    FUNCTION_INSERT(variance);

    // Discrete.
    FUNCTION_INSERT(gcd);
    FUNCTION_INSERT(ncr);
    FUNCTION_INSERT(npr);

    // Probability.
    FUNCTION_INSERT(binomcdf);
    FUNCTION_INSERT(binommean);
    FUNCTION_INSERT(binompmf);
    FUNCTION_INSERT(binomvar);
    FUNCTION_INSERT(erf);
    FUNCTION_INSERT(erfc);
    FUNCTION_INSERT(hypercdf);
    FUNCTION_INSERT(hypermean);
    FUNCTION_INSERT(hyperpmf);
    FUNCTION_INSERT(hypervar);
    FUNCTION_INSERT(median);
    FUNCTION_INSERT(poicdf);
    FUNCTION_INSERT(poimean);
    FUNCTION_INSERT(poipmf);
    FUNCTION_INSERT(poivar);

    // Trigonometry.
    FUNCTION_INSERT(arccos);
    FUNCTION_INSERT(arcosh);
    FUNCTION_INSERT(arsinh);
    FUNCTION_INSERT(artanh);
    FUNCTION_INSERT(arcsin);
    FUNCTION_INSERT(arctan);
    FUNCTION_INSERT(cos);
    FUNCTION_INSERT(cosh);
    FUNCTION_INSERT(cot);
    FUNCTION_INSERT(csc);
    FUNCTION_INSERT(degrees);
    FUNCTION_INSERT(exp);
    FUNCTION_INSERT(lb);
    FUNCTION_INSERT(lg);
    FUNCTION_INSERT(ln);
    FUNCTION_INSERT(log);
    FUNCTION_INSERT(radians);
    FUNCTION_INSERT(sec);
    FUNCTION_INSERT(sin);
    FUNCTION_INSERT(sinh);
    FUNCTION_INSERT(tan);
    FUNCTION_INSERT(tanh);

    // Logic.
    FUNCTION_INSERT(mask);
    FUNCTION_INSERT(gmask);
    FUNCTION_INSERT(unmask);
    FUNCTION_INSERT(not);
    FUNCTION_INSERT(and);
    FUNCTION_INSERT(or);
    FUNCTION_INSERT(xor);
    FUNCTION_INSERT(shl);
    FUNCTION_INSERT(shr);
    FUNCTION_INSERT(idiv);
    FUNCTION_INSERT(mod);
}

FunctionRepo* FunctionRepo::instance()
{
    if (!s_FunctionRepoInstance) {
        s_FunctionRepoInstance = new FunctionRepo;
        qAddPostRoutine(s_deleteFunctions);
    }
    return s_FunctionRepoInstance;
}

FunctionRepo::FunctionRepo()
{
    createFunctions();
    setFunctionNames();
    setNonTranslatableFunctionUsages();
    setTranslatableFunctionUsages();
}

void FunctionRepo::insert(Function* function)
{
    if (!function)
        return;
    m_functions.insert(function->identifier().toUpper(), function);
}

Function* FunctionRepo::find(const QString& identifier) const
{
    if (identifier.isNull())
        return 0;
    return m_functions.value(identifier.toUpper(), 0);
}

QStringList FunctionRepo::getIdentifiers() const
{
    QStringList result = m_functions.keys();
    std::transform(result.begin(), result.end(), result.begin(),
        std::mem_fun_ref(&QString::toLower));
    return result;
}

void FunctionRepo::setNonTranslatableFunctionUsages()
{
    FUNCTION_USAGE(abs, "x");
    FUNCTION_USAGE(absdev, "x<sub>1</sub>; x<sub>2</sub>; ...");
    FUNCTION_USAGE(arccos, "x");
    FUNCTION_USAGE(and, "x<sub>1</sub>; x<sub>2</sub>; ...");
    FUNCTION_USAGE(arcosh, "x");
    FUNCTION_USAGE(arsinh, "x");
    FUNCTION_USAGE(artanh, "x");
    FUNCTION_USAGE(arcsin, "x");
    FUNCTION_USAGE(arctan, "x");
    FUNCTION_USAGE(average, "x<sub>1</sub>; x<sub>2</sub>; ...");
    FUNCTION_USAGE(bin, "n");
    FUNCTION_USAGE(cbrt, "x");
    FUNCTION_USAGE(ceil, "x");
    FUNCTION_USAGE(cos, "x");
    FUNCTION_USAGE(cosh, "x");
    FUNCTION_USAGE(cot, "x");
    FUNCTION_USAGE(csc, "x");
    FUNCTION_USAGE(dec, "x");
    FUNCTION_USAGE(degrees, "x");
    FUNCTION_USAGE(erf, "x");
    FUNCTION_USAGE(erfc, "x");
    FUNCTION_USAGE(exp, "x");
    FUNCTION_USAGE(floor, "x");
    FUNCTION_USAGE(frac, "x");
    FUNCTION_USAGE(gamma, "x");
    FUNCTION_USAGE(gcd, "n<sub>1</sub>; n<sub>2</sub>; ...");
    FUNCTION_USAGE(geomean, "x<sub>1</sub>; x<sub>2</sub>; ...");
    FUNCTION_USAGE(hex, "n");
    FUNCTION_USAGE(int, "x");
    FUNCTION_USAGE(lb, "x");
    FUNCTION_USAGE(lg, "x");
    FUNCTION_USAGE(ln, "x");
    FUNCTION_USAGE(lngamma, "x");
    FUNCTION_USAGE(max, "x<sub>1</sub>; x<sub>2</sub>; ...");
    FUNCTION_USAGE(median, "x<sub>1</sub>; x<sub>2</sub>; ...");
    FUNCTION_USAGE(min, "x<sub>1</sub>; x<sub>2</sub>; ...");
    FUNCTION_USAGE(ncr, "x<sub>1</sub>; x<sub>2</sub>");
    FUNCTION_USAGE(not, "n");
    FUNCTION_USAGE(npr, "x<sub>1</sub>; x<sub>2</sub>");
    FUNCTION_USAGE(oct, "n");
    FUNCTION_USAGE(or, "x<sub>1</sub>; x<sub>2</sub>; ...");
    FUNCTION_USAGE(product, "x<sub>1</sub>; x<sub>2</sub>; ...");
    FUNCTION_USAGE(radians, "x");
    FUNCTION_USAGE(round, "x");
    FUNCTION_USAGE(sec, "x)");
    FUNCTION_USAGE(sgn, "x");
    FUNCTION_USAGE(sin, "x");
    FUNCTION_USAGE(sinh, "x");
    FUNCTION_USAGE(sqrt, "x");
    FUNCTION_USAGE(stddev, "x<sub>1</sub>; x<sub>2</sub>; ...");
    FUNCTION_USAGE(sum, "x<sub>1</sub>; x<sub>2</sub>; ...");
    FUNCTION_USAGE(tan, "x");
    FUNCTION_USAGE(tanh, "x");
    FUNCTION_USAGE(trunc, "x");
    FUNCTION_USAGE(variance, "x<sub>1</sub>; x<sub>2</sub>; ...");
    FUNCTION_USAGE(xor, "x<sub>1</sub>; x<sub>2</sub>; ...");
}

void FunctionRepo::setTranslatableFunctionUsages()
{
    FUNCTION_USAGE_TR(binomcdf, tr("max; trials; probability"));
    FUNCTION_USAGE_TR(binommean, tr("trials; probability"));
    FUNCTION_USAGE_TR(binompmf, tr("hits; trials; probability"));
    FUNCTION_USAGE_TR(binomvar, tr("trials; probability"));
    FUNCTION_USAGE_TR(hypercdf, tr("max; total; hits; trials"));
    FUNCTION_USAGE_TR(hypermean, tr("total; hits; trials"));
    FUNCTION_USAGE_TR(hyperpmf, tr("count; total; hits; trials"));
    FUNCTION_USAGE_TR(hypervar, tr("total; hits; trials"));
    FUNCTION_USAGE_TR(idiv, tr("dividend; divisor"));
    FUNCTION_USAGE_TR(gmask, tr("n; bits group"));
    FUNCTION_USAGE_TR(log, tr("base; x"));
    FUNCTION_USAGE_TR(mask, tr("n; bits"));
    FUNCTION_USAGE_TR(mod, tr("value; modulo"));
    FUNCTION_USAGE_TR(poicdf, tr("events; average_events"));
    FUNCTION_USAGE_TR(poimean, tr("average_events"));
    FUNCTION_USAGE_TR(poipmf, tr("events; average_events"));
    FUNCTION_USAGE_TR(poivar, tr("average_events"));
    FUNCTION_USAGE_TR(shl, tr("n; bits"));
    FUNCTION_USAGE_TR(shr, tr("n; bits"));
    FUNCTION_USAGE_TR(unmask, tr("n; bits"));
}

void FunctionRepo::setFunctionNames()
{
    FUNCTION_NAME(abs, tr("Absolute Value"));
    FUNCTION_NAME(absdev, tr("Absolute Deviation"));
    FUNCTION_NAME(arccos, tr("Arc Cosine"));
    FUNCTION_NAME(and, tr("Logical AND"));
    FUNCTION_NAME(arcosh, tr("Area Hyperbolic Cosine"));
    FUNCTION_NAME(arsinh, tr("Area Hyperbolic Sine"));
    FUNCTION_NAME(artanh, tr("Area Hyperbolic Tangent"));
    FUNCTION_NAME(arcsin, tr("Arc Sine"));
    FUNCTION_NAME(arctan, tr("Arc Tangent"));
    FUNCTION_NAME(average, tr("Average (Arithmetic Mean)"));
    FUNCTION_NAME(bin, tr("Binary Representation"));
    FUNCTION_NAME(binomcdf, tr("Binomial Cumulative Distribution Function"));
    FUNCTION_NAME(binommean, tr("Binomial Distribution Mean"));
    FUNCTION_NAME(binompmf, tr("Binomial Probability Mass Function"));
    FUNCTION_NAME(binomvar, tr("Binomial Distribution Variance"));
    FUNCTION_NAME(cbrt, tr("Cube Root"));
    FUNCTION_NAME(ceil, tr("Ceiling"));
    FUNCTION_NAME(cos, tr("Cosine"));
    FUNCTION_NAME(cosh, tr("Hyperbolic Cosine"));
    FUNCTION_NAME(cot, tr("Cotangent"));
    FUNCTION_NAME(csc, tr("Cosecant"));
    FUNCTION_NAME(dec, tr("Decimal Representation"));
    FUNCTION_NAME(degrees, tr("Degrees of Arc"));
    FUNCTION_NAME(erf, tr("Error Function"));
    FUNCTION_NAME(erfc, tr("Complementary Error Function"));
    FUNCTION_NAME(exp, tr("Exponential"));
    FUNCTION_NAME(floor, tr("Floor"));
    FUNCTION_NAME(frac, tr("Fractional Part"));
    FUNCTION_NAME(gamma, tr("Extension of Factorials [= (x-1)!]"));
    FUNCTION_NAME(gcd, tr("Greatest Common Divisor"));
    FUNCTION_NAME(geomean, tr("Geometric Mean"));
    FUNCTION_NAME(hex, tr("Hexadecimal Representation"));
    FUNCTION_NAME(hypercdf, tr("Hypergeometric Cumulative Distribution Function"));
    FUNCTION_NAME(hypermean, tr("Hypergeometric Distribution Mean"));
    FUNCTION_NAME(hyperpmf, tr("Hypergeometric Probability Mass Function"));
    FUNCTION_NAME(hypervar, tr("Hypergeometric Distribution Variance"));
    FUNCTION_NAME(idiv, tr("Integer Quotient"));
    FUNCTION_NAME(gmask, tr("Mask to a bit group size"));
    FUNCTION_NAME(int, tr("Integer Part"));
    FUNCTION_NAME(lb, tr("Binary Logarithm"));
    FUNCTION_NAME(lg, tr("Common Logarithm"));
    FUNCTION_NAME(ln, tr("Natural Logarithm"));
    FUNCTION_NAME(lngamma, "ln(abs(Gamma))");
    FUNCTION_NAME(log, tr("Logarithm to Arbitrary Base"));
    FUNCTION_NAME(mask, tr("Mask to a bit size"));
    FUNCTION_NAME(max, tr("Maximum"));
    FUNCTION_NAME(median, tr("Median Value (50th Percentile)"));
    FUNCTION_NAME(min, tr("Minimum"));
    FUNCTION_NAME(mod, tr("Modulo"));
    FUNCTION_NAME(ncr, tr("Combination (Binomial Coefficient)"));
    FUNCTION_NAME(not, tr("Logical NOT"));
    FUNCTION_NAME(npr, tr("Permutation (Arrangement)"));
    FUNCTION_NAME(oct, tr("Octal Representation"));
    FUNCTION_NAME(or, tr("Logical OR"));
    FUNCTION_NAME(poicdf, tr("Poissonian Cumulative Distribution Function"));
    FUNCTION_NAME(poimean, tr("Poissonian Distribution Mean"));
    FUNCTION_NAME(poipmf, tr("Poissonian Probability Mass Function"));
    FUNCTION_NAME(poivar, tr("Poissonian Distribution Variance"));
    FUNCTION_NAME(product, tr("Product"));
    FUNCTION_NAME(radians, tr("Radians"));
    FUNCTION_NAME(round, tr("Rounding"));
    FUNCTION_NAME(sec, tr("Secant"));
    FUNCTION_NAME(shl, tr("Arithmetic Shift Left"));
    FUNCTION_NAME(shr, tr("Arithmetic Shift Right"));
    FUNCTION_NAME(sgn, tr("Signum"));
    FUNCTION_NAME(sin, tr("Sine"));
    FUNCTION_NAME(sinh, tr("Hyperbolic Sine"));
    FUNCTION_NAME(sqrt, tr("Square Root"));
    FUNCTION_NAME(stddev, tr("Standard Deviation (Square Root of Variance)"));
    FUNCTION_NAME(sum, tr("Sum"));
    FUNCTION_NAME(tan, tr("Tangent"));
    FUNCTION_NAME(tanh, tr("Hyperbolic Tangent"));
    FUNCTION_NAME(trunc, tr("Truncation"));
    FUNCTION_NAME(unmask, tr("Sign-extend a value"));
    FUNCTION_NAME(variance, tr("Variance"));
    FUNCTION_NAME(xor, tr("Logical XOR"));
}

void FunctionRepo::retranslateText()
{
    setFunctionNames();
    setTranslatableFunctionUsages();
}
