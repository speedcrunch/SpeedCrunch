// This file is part of the SpeedCrunch project
// Copyright (C) 2004-2006 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2007, 2009 Wolf Lammen
// Copyright (C) 2007-2009, 2013 Helder Correia <helder.pereira.correia@gmail.com>
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
#include <functional>
#include <cfloat>
#include <cmath>
#include <numeric>

#define ENSURE_POSITIVE_ARGUMENT_COUNT() \
    if (args.count() < 1) { \
        f->setError(InvalidParamCount); \
        return HMath::nan(); \
    }

#define ENSURE_ARGUMENT_COUNT(i) \
    if (args.count() != (i)) { \
        f->setError(InvalidParamCount); \
        return HMath::nan(); \
    } \

#define ENSURE_EITHER_ARGUMENT_COUNT(i, j) \
    if (args.count() != (i) && args.count() != (j)) { \
        f->setError(InvalidParamCount); \
        return HMath::nan(); \
    } \

static FunctionRepo* s_FunctionRepoInstance = 0;

// FIXME: destructor seems not to be called
static void s_deleteFunctions()
{
    delete s_FunctionRepoInstance;
}

static HNumber function_abs(Function*, const Function::ArgumentList&);
static HNumber function_absdev(Function*, const Function::ArgumentList&);
static HNumber function_acos(Function*, const Function::ArgumentList&);
static HNumber function_and(Function*, const Function::ArgumentList&);
static HNumber function_arcosh(Function*, const Function::ArgumentList&);
static HNumber function_arsinh(Function*, const Function::ArgumentList&);
static HNumber function_artanh(Function*, const Function::ArgumentList&);
static HNumber function_ashl(Function*, const Function::ArgumentList&);
static HNumber function_ashr(Function*, const Function::ArgumentList&);
static HNumber function_asin(Function*, const Function::ArgumentList&);
static HNumber function_atan(Function*, const Function::ArgumentList&);
static HNumber function_average(Function*, const Function::ArgumentList&);
static HNumber function_bin(Function*, const Function::ArgumentList&);
static HNumber function_binomcdf(Function*, const Function::ArgumentList&);
static HNumber function_binommean(Function*, const Function::ArgumentList&);
static HNumber function_binompmf(Function*, const Function::ArgumentList&);
static HNumber function_binomvar(Function*, const Function::ArgumentList&);
static HNumber function_cbrt(Function*, const Function::ArgumentList&);
static HNumber function_ceil(Function*, const Function::ArgumentList&);
static HNumber function_cos(Function*, const Function::ArgumentList&);
static HNumber function_cosh(Function*, const Function::ArgumentList&);
static HNumber function_cot(Function*, const Function::ArgumentList&);
static HNumber function_csc(Function*, const Function::ArgumentList&);
static HNumber function_dec(Function*, const Function::ArgumentList&);
static HNumber function_degrees(Function*, const Function::ArgumentList&);
static HNumber function_erfc(Function*, const Function::ArgumentList&);
static HNumber function_erf(Function*, const Function::ArgumentList&);
static HNumber function_exp(Function*, const Function::ArgumentList&);
static HNumber function_floor(Function*, const Function::ArgumentList&);
static HNumber function_frac(Function*, const Function::ArgumentList&);
static HNumber function_gamma(Function*, const Function::ArgumentList&);
static HNumber function_gcd(Function*, const Function::ArgumentList&);
static HNumber function_geomean(Function*, const Function::ArgumentList&);
static HNumber function_hex(Function*, const Function::ArgumentList&);
static HNumber function_hypercdf(Function*, const Function::ArgumentList&);
static HNumber function_hypermean(Function*, const Function::ArgumentList&);
static HNumber function_hyperpmf(Function*, const Function::ArgumentList&);
static HNumber function_hypervar(Function*, const Function::ArgumentList&);
static HNumber function_idiv(Function*, const Function::ArgumentList&);
static HNumber function_integer(Function*, const Function::ArgumentList&);
static HNumber function_lg(Function*, const Function::ArgumentList&);
static HNumber function_ln(Function*, const Function::ArgumentList&);
static HNumber function_lnGamma(Function*, const Function::ArgumentList&);
static HNumber function_log(Function*, const Function::ArgumentList&);
static HNumber function_mask(Function*, const Function::ArgumentList&);
static HNumber function_max(Function*, const Function::ArgumentList&);
static HNumber function_median(Function*, const Function::ArgumentList&);
static HNumber function_min(Function*, const Function::ArgumentList&);
static HNumber function_mod(Function*, const Function::ArgumentList&);
static HNumber function_ncr(Function*, const Function::ArgumentList&);
static HNumber function_not(Function*, const Function::ArgumentList&);
static HNumber function_npr(Function*, const Function::ArgumentList&);
static HNumber function_oct(Function*, const Function::ArgumentList&);
static HNumber function_or(Function*, const Function::ArgumentList&);
static HNumber function_poicdf(Function*, const Function::ArgumentList&);
static HNumber function_poimean(Function*, const Function::ArgumentList&);
static HNumber function_poipmf(Function*, const Function::ArgumentList&);
static HNumber function_poivar(Function*, const Function::ArgumentList&);
static HNumber function_product(Function*, const Function::ArgumentList&);
static HNumber function_radians(Function*, const Function::ArgumentList&);
static HNumber function_round(Function*, const Function::ArgumentList&);
static HNumber function_sec(Function*, const Function::ArgumentList&);
static HNumber function_sign(Function*, const Function::ArgumentList&);
static HNumber function_sin(Function*, const Function::ArgumentList&);
static HNumber function_sinh(Function*, const Function::ArgumentList&);
static HNumber function_sqrt(Function*, const Function::ArgumentList&);
static HNumber function_stddev(Function*, const Function::ArgumentList&);
static HNumber function_sum(Function*, const Function::ArgumentList&);
static HNumber function_tan(Function*, const Function::ArgumentList&);
static HNumber function_tanh(Function*, const Function::ArgumentList&);
static HNumber function_trunc(Function*, const Function::ArgumentList&);
static HNumber function_unmask(Function*, const Function::ArgumentList&);
static HNumber function_variance(Function*, const Function::ArgumentList&);
static HNumber function_xor(Function*, const Function::ArgumentList&);

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

HNumber function_integer(Function* f, const Function::ArgumentList& args)
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

HNumber function_log(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::log(args.at(0));
}

HNumber function_lg(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::lg(args.at(0));
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

HNumber function_asin(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    HNumber result = HMath::asin(args.at(0));
    if (Settings::instance()->angleUnit == 'd')
        result = HMath::rad2deg(result);
    return result;
}

HNumber function_acos(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    HNumber result = HMath::acos(args.at(0));
    if (Settings::instance()->angleUnit == 'd')
        result = HMath::rad2deg(result);
    return result;
}

HNumber function_atan(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    HNumber result = HMath::atan(args.at(0));
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

HNumber function_lnGamma(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::lnGamma(args.at(0));
}

HNumber function_sign(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(1);
    return HMath::sign(args.at(0));
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

HNumber function_average(Function* f, const Function::ArgumentList& args)
{
    ENSURE_POSITIVE_ARGUMENT_COUNT();
    return std::accumulate(args.begin(), args.end(), HNumber(0)) / HNumber(args.count());
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

HNumber function_unmask(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(2);
    return HMath::sgnext(args.at(0), args.at(1));
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

HNumber function_ashl(Function* f, const Function::ArgumentList& args)
{
    ENSURE_ARGUMENT_COUNT(2);
    return HMath::ashr(args.at(0), -args.at(1));
}

HNumber function_ashr(Function* f, const Function::ArgumentList& args)
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
    insert(new Function("abs", function_abs, this));
    insert(new Function("absdev", function_absdev, this));
    insert(new Function("average", function_average, this));
    insert(new Function("bin", function_bin, this));
    insert(new Function("cbrt", function_cbrt, this));
    insert(new Function("ceil", function_ceil, this));
    insert(new Function("dec", function_dec, this));
    insert(new Function("floor", function_floor, this));
    insert(new Function("frac", function_frac, this));
    insert(new Function("gamma", function_gamma, this));
    insert(new Function("geomean", function_geomean, this));
    insert(new Function("hex", function_hex, this));
    insert(new Function("int", function_integer, this));
    insert(new Function("lngamma", function_lnGamma, this));
    insert(new Function("max", function_max, this));
    insert(new Function("min", function_min, this));
    insert(new Function("oct", function_oct, this));
    insert(new Function("product", function_product, this));
    insert(new Function("round", function_round, this));
    insert(new Function("sign", function_sign, this));
    insert(new Function("sqrt", function_sqrt, this));
    insert(new Function("stddev", function_stddev, this));
    insert(new Function("sum", function_sum, this));
    insert(new Function("trunc", function_trunc, this));
    insert(new Function("variance", function_variance, this));

    // Discrete.
    insert(new Function("gcd", function_gcd, this));
    insert(new Function("ncr", function_ncr, this));
    insert(new Function("npr", function_npr, this));

    // Probability.
    insert(new Function("binomcdf", function_binomcdf, this));
    insert(new Function("binommean", function_binommean, this));
    insert(new Function("binompmf", function_binompmf, this));
    insert(new Function("binomvar", function_binomvar, this));
    insert(new Function("erf", function_erf, this));
    insert(new Function("erfc", function_erfc, this));
    insert(new Function("hypercdf", function_hypercdf, this));
    insert(new Function("hypermean", function_hypermean, this));
    insert(new Function("hyperpmf", function_hyperpmf, this));
    insert(new Function("hypervar", function_hypervar, this));
    insert(new Function("median", function_median, this));
    insert(new Function("poicdf", function_poicdf, this));
    insert(new Function("poimean", function_poimean, this));
    insert(new Function("poipmf", function_poipmf, this));
    insert(new Function("poivar", function_poivar, this));

    // Trigonometry.
    insert(new Function("acos", function_acos, this));
    insert(new Function("arcosh", function_arcosh, this));
    insert(new Function("arsinh", function_arsinh, this));
    insert(new Function("artanh", function_artanh, this));
    insert(new Function("asin", function_asin, this));
    insert(new Function("atan", function_atan, this));
    insert(new Function("cos", function_cos, this));
    insert(new Function("cosh", function_cosh, this));
    insert(new Function("cot", function_cot, this));
    insert(new Function("csc", function_csc, this));
    insert(new Function("degrees", function_degrees, this));
    insert(new Function("exp", function_exp, this));
    insert(new Function("lg", function_lg, this));
    insert(new Function("ln", function_ln, this));
    insert(new Function("log", function_log, this));
    insert(new Function("radians", function_radians, this));
    insert(new Function("sec", function_sec, this));
    insert(new Function("sin", function_sin, this));
    insert(new Function("sinh", function_sinh, this));
    insert(new Function("tan", function_tan, this));
    insert(new Function("tanh", function_tanh, this));

    // Logic.
    insert(new Function("mask", function_mask, this));
    insert(new Function("unmask", function_unmask, this));
    insert(new Function("not", function_not, this));
    insert(new Function("and", function_and, this));
    insert(new Function("or", function_or, this));
    insert(new Function("xor", function_xor, this));
    insert(new Function("shl", function_ashl, this));
    insert(new Function("shr", function_ashr, this));
    insert(new Function("idiv", function_idiv, this));
    insert(new Function("mod", function_mod, this));
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
    find("abs")->setUsage(QString::fromLatin1("x"));
    find("absdev")->setUsage(QString::fromUtf8("x₁; x₂ ; ..."));
    find("acos")->setUsage(QString::fromLatin1("x"));
    find("and")->setUsage(QString::fromUtf8("x₁; x₂ ; ..."));
    find("arcosh")->setUsage(QString::fromLatin1("x"));
    find("arsinh")->setUsage(QString::fromLatin1("x"));
    find("artanh")->setUsage(QString::fromLatin1("x"));
    find("asin")->setUsage(QString::fromLatin1("x"));
    find("atan")->setUsage(QString::fromLatin1("x"));
    find("average")->setUsage(QString::fromUtf8("x₁; x₂ ; ..."));
    find("bin")->setUsage(QString::fromLatin1("n"));
    find("cbrt")->setUsage(QString::fromLatin1("x"));
    find("ceil")->setUsage(QString::fromLatin1("x"));
    find("cos")->setUsage(QString::fromLatin1("x"));
    find("cosh")->setUsage(QString::fromLatin1("x"));
    find("cot")->setUsage(QString::fromLatin1("x"));
    find("csc")->setUsage(QString::fromLatin1("x"));
    find("dec")->setUsage(QString::fromLatin1("x"));
    find("degrees")->setUsage(QString::fromLatin1("x"));
    find("erf" )->setUsage(QString::fromLatin1("x"));
    find("erfc")->setUsage(QString::fromLatin1("x"));
    find("exp")->setUsage(QString::fromLatin1("x"));
    find("floor")->setUsage(QString::fromLatin1("x"));
    find("frac")->setUsage(QString::fromLatin1("x"));
    find("gamma")->setUsage(QString::fromLatin1("x"));
    find("gcd")->setUsage(QString::fromUtf8("n₁; n₂ ; ..."));
    find("geomean")->setUsage(QString::fromUtf8("x₁; x₂ ; ..."));
    find("hex")->setUsage(QString::fromLatin1("n"));
    find("int")->setUsage(QString::fromLatin1("x"));
    find("lg")->setUsage(QString::fromLatin1("x"));
    find("ln")->setUsage(QString::fromLatin1("x"));
    find("lngamma")->setUsage(QString::fromLatin1("x"));
    find("log")->setUsage(QString::fromLatin1("x"));
    find("max")->setUsage(QString::fromUtf8("x₁; x₂ ; ..."));
    find("median")->setUsage(QString::fromUtf8("x₁; x₂ ; ..."));
    find("min")->setUsage(QString::fromUtf8("x₁; x₂ ; ..."));
    find("ncr")->setUsage(QString::fromUtf8("x₁; x₂"));
    find("not")->setUsage(QString::fromLatin1("n"));
    find("npr")->setUsage(QString::fromUtf8("x₁; x₂"));
    find("oct")->setUsage(QString::fromLatin1("n"));
    find("or")->setUsage(QString::fromUtf8("x₁; x₂ ; ..."));
    find("product")->setUsage(QString::fromUtf8("x₁; x₂ ; ..."));
    find("radians")->setUsage(QString::fromLatin1("x"));
    find("round")->setUsage(QString::fromLatin1("x"));
    find("sec")->setUsage(QString::fromLatin1("x)"));
    find("sign")->setUsage(QString::fromLatin1("x"));
    find("sin")->setUsage(QString::fromLatin1("x"));
    find("sinh")->setUsage(QString::fromLatin1("x"));
    find("sqrt")->setUsage(QString::fromLatin1("x"));
    find("stddev")->setUsage(QString::fromUtf8("x₁; x₂ ; ..."));
    find("sum")->setUsage(QString::fromUtf8("x₁; x₂ ; ..."));
    find("tan")->setUsage(QString::fromLatin1("x"));
    find("tanh")->setUsage(QString::fromLatin1("x"));
    find("trunc")->setUsage(QString::fromLatin1("x"));
    find("variance")->setUsage(QString::fromUtf8("x₁; x₂ ; ..."));
    find("xor")->setUsage(QString::fromUtf8("x₁; x₂ ; ..."));
}

void FunctionRepo::setTranslatableFunctionUsages()
{
    find("binomcdf")->setUsage(tr("max; trials; probability"));
    find("binommean")->setUsage(tr("trials; probability"));
    find("binompmf")->setUsage(tr("hits; trials; probability"));
    find("binomvar")->setUsage(tr("trials; probability"));
    find("hypercdf")->setUsage(tr("max; total; hits; trials"));
    find("hypermean")->setUsage(tr("total; hits; trials"));
    find("hyperpmf")->setUsage(tr("count; total; hits; trials"));
    find("hypervar")->setUsage(tr("total; hits; trials"));
    find("idiv")->setUsage(tr("dividend; divisor"));
    find("mask")->setUsage(tr("n; bits"));
    find("mod")->setUsage(tr("value; modulo"));
    find("poicdf")->setUsage(tr("events; average_events"));
    find("poimean")->setUsage(tr("average_events"));
    find("poipmf")->setUsage(tr("events; average_events"));
    find("poivar")->setUsage(tr("average_events"));
    find("shl")->setUsage(tr("n; bits"));
    find("shr")->setUsage(tr("n; bits"));
    find("unmask")->setUsage(tr("n; bits"));
}

void FunctionRepo::setFunctionNames()
{
    find("abs")->setName(Function::tr("Absolute Value"));
    find("absdev")->setName(Function::tr("Absolute Deviation"));
    find("acos")->setName(Function::tr("Arc Cosine"));
    find("and")->setName(Function::tr("Logical AND"));
    find("arcosh")->setName(Function::tr("Area Hyperbolic Cosine"));
    find("arsinh")->setName(Function::tr("Area Hyperbolic Sine"));
    find("artanh")->setName(Function::tr("Area Hyperbolic Tangent"));
    find("asin")->setName(Function::tr("Arc Sine"));
    find("atan")->setName(Function::tr("Arc Tangent"));
    find("average")->setName(Function::tr("Average (Arithmetic Mean)"));
    find("bin")->setName(Function::tr("Binary Representation"));
    find("binomcdf")->setName(Function::tr("Binomial Cumulative Distribution Function"));
    find("binommean")->setName(Function::tr("Binomial Distribution Mean"));
    find("binompmf")->setName(Function::tr("Binomial Probability Mass Function"));
    find("binomvar")->setName(Function::tr("Binomial Distribution Variance"));
    find("cbrt")->setName(Function::tr("Cube Root"));
    find("ceil")->setName(Function::tr("Ceiling"));
    find("cos")->setName(Function::tr("Cosine"));
    find("cosh")->setName(Function::tr("Hyperbolic Cosine"));
    find("cot")->setName(Function::tr("Cotangent"));
    find("csc")->setName(Function::tr("Cosecant"));
    find("dec")->setName(Function::tr("Decimal Representation"));
    find("degrees")->setName(Function::tr("Degrees of Arc"));
    find("erf" )->setName(Function::tr("Error Function"));
    find("erfc")->setName(Function::tr("Complementary Error Function"));
    find("exp")->setName(Function::tr("Exponential"));
    find("floor")->setName(Function::tr("Floor"));
    find("frac")->setName(Function::tr("Fractional Part"));
    find("gamma")->setName(Function::tr("Extension of Factorials [= (x-1)!]"));
    find("gcd")->setName(Function::tr("Greatest Common Divisor"));
    find("geomean")->setName(Function::tr("Geometric Mean"));
    find("hex")->setName(Function::tr("Hexadecimal Representation"));
    find("hypercdf")->setName(Function::tr("Hypergeometric Cumulative Distribution Function"));
    find("hypermean")->setName(Function::tr("Hypergeometric Distribution Mean"));
    find("hyperpmf")->setName(Function::tr("Hypergeometric Probability Mass Function"));
    find("hypervar")->setName(Function::tr("Hypergeometric Distribution Variance"));
    find("idiv")->setName(Function::tr("Integer Quotient"));
    find("int")->setName(Function::tr("Integer Part"));
    find("lg")->setName(Function::tr("Base-2 Logarithm"));
    find("ln")->setName(Function::tr("Natural Logarithm"));
    find("lngamma")->setName("ln(abs(Gamma))");
    find("log")->setName(Function::tr("Base-10 Logarithm"));
    find("mask")->setName(Function::tr("Mask to a bit size"));
    find("max")->setName(Function::tr("Maximum"));
    find("median")->setName(Function::tr("Median Value (50th Percentile)"));
    find("min")->setName(Function::tr("Minimum"));
    find("mod")->setName(Function::tr("Modulo"));
    find("ncr")->setName(Function::tr("Combination (Binomial Coefficient)"));
    find("not")->setName(Function::tr("Logical NOT"));
    find("npr")->setName(Function::tr("Permutation (Arrangement)"));
    find("oct")->setName(Function::tr("Octal Representation"));
    find("or")->setName(Function::tr("Logical OR"));
    find("poicdf")->setName(Function::tr("Poissonian Cumulative Distribution Function"));
    find("poimean")->setName(Function::tr("Poissonian Distribution Mean"));
    find("poipmf")->setName(Function::tr("Poissonian Probability Mass Function"));
    find("poivar")->setName(Function::tr("Poissonian Distribution Variance"));
    find("product")->setName(Function::tr("Product"));
    find("radians")->setName(Function::tr("Radians"));
    find("round")->setName(Function::tr("Rounding"));
    find("sec")->setName(Function::tr("Secant"));
    find("shl")->setName(Function::tr("Arithmetic Shift Left"));
    find("shr")->setName(Function::tr("Arithmetic Shift Right"));
    find("sign")->setName(Function::tr("Signum"));
    find("sin")->setName(Function::tr("Sine"));
    find("sinh")->setName(Function::tr("Hyperbolic Sine"));
    find("sqrt")->setName(Function::tr("Square Root"));
    find("stddev")->setName(Function::tr("Standard Deviation (Square Root of Variance)"));
    find("sum")->setName(Function::tr("Sum"));
    find("tan")->setName(Function::tr("Tangent"));
    find("tanh")->setName(Function::tr("Hyperbolic Tangent"));
    find("trunc")->setName(Function::tr("Truncation"));
    find("unmask")->setName(Function::tr("Sign-extent a value"));
    find("variance")->setName(Function::tr("Variance"));
    find("xor")->setName(Function::tr("Logical XOR"));
}

void FunctionRepo::retranslateText()
{
    setFunctionNames();
    setTranslatableFunctionUsages();
}