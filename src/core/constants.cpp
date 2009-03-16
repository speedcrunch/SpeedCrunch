// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2007-2009 Helder Correia <helder.pereira.correia@gmail.com>
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

#include "core/constants.hxx"

#include <QtCore/QCoreApplication>

#include <algorithm>

static Constants * s_constantsInstance = 0;

static void s_deleteConstants()
{
    delete s_constantsInstance;
}

struct Constants::Private
{
    QStringList categories;
    QList<Constant> list;

    void populate();
    void retranslateText();
};

#define PUSH_CONSTANT(NAME,VALUE,UNIT) \
    c.value = VALUE; \
    c.unit = UNIT; \
    list << c;

#define PUSH_CONSTANT_DAYS(NAME,VALUE) \
    c.value = VALUE; \
    list << c;

#define I18N_CONSTANT(NAME) \
    i->name = NAME; \
    i->category = cat; \
    ++i;

#define I18N_CONSTANT_DAYS(NAME) \
    i->unit = days; \
    I18N_CONSTANT(NAME)

void Constants::Private::populate()
{
    Constant c;

    PUSH_CONSTANT("Characteristic Impedance of Vacuum",
            QLatin1String("376.730313461"), QString::fromUtf8("Ω"));

    PUSH_CONSTANT("Dirac's Constant",
            QLatin1String("1.054571628e-34"), QString::fromUtf8("J·s"));

    PUSH_CONSTANT("Electric Constant",
            QLatin1String("8.854187817e-12"), QLatin1String("F/m"));

    PUSH_CONSTANT("Gravitation Constant",
            QLatin1String("6.67428e-11"), QString::fromUtf8("m³/(kg·s²)"));

    PUSH_CONSTANT("Magnetic Constant",
            QLatin1String("1.256637061e-6"), QString::fromUtf8("N/A²"));

    PUSH_CONSTANT("Planck's Constant",
            QLatin1String("6.62606896e-34"), QString::fromUtf8("J·s"));

    PUSH_CONSTANT("Speed of Light in Vacuum",
            QLatin1String("299792458"), QLatin1String("m/s"));

    PUSH_CONSTANT("Bohr-Procopiu Magneton",
            QLatin1String("927.400949e-26"), QLatin1String("J/T"));

    PUSH_CONSTANT("Conductance Quantum",
            QLatin1String("7.748091733e-5"), QLatin1String("S"));

    PUSH_CONSTANT("Coulomb's Constant",
            QLatin1String("8.987742438e9"), QString::fromUtf8("N·m²/C²"));

    PUSH_CONSTANT("Elementary Charge",
            QLatin1String("1.60217653e-19"), QLatin1String("C"));

    PUSH_CONSTANT("Josephson Constant",
            QLatin1String("483597.879e9"), QLatin1String("Hz/V"));

    PUSH_CONSTANT("Magnetic Flux Quantum",
            QLatin1String("2.06783372e-15"), QLatin1String("Wb"));

    PUSH_CONSTANT("Nuclear Magneton",
            QLatin1String("5.05078343e-27"), QLatin1String("J/T"));

    PUSH_CONSTANT("Resistance Quantum",
            QLatin1String("12906.403725"), QString::fromUtf8("Ω"));

    PUSH_CONSTANT("von Klitzing Constant",
            QLatin1String("25812.807449"), QString::fromUtf8("Ω"));

    PUSH_CONSTANT("Bohr Radius",
            QLatin1String("0.5291772108e-10"), QLatin1String("m"));

    PUSH_CONSTANT("Fermi Coupling Constant",
            QLatin1String("1.16639e-5"), QString::fromUtf8("Ge/V²"));

    PUSH_CONSTANT("Fine-structure Constant",
            QLatin1String("7.297352568e-3"), QLatin1String(""));

    PUSH_CONSTANT("Hartree Energy",
            QLatin1String("4.35974417e-18"), QLatin1String("J"));

    PUSH_CONSTANT("Quantum of Circulation",
            QLatin1String("3.636947550e-4"), QString::fromUtf8("m²/s"));

    PUSH_CONSTANT("Rydberg Constant",
            QLatin1String("10973731.568525"), QLatin1String("1/m"));

    PUSH_CONSTANT("Thomson Cross Section",
            QLatin1String("0.665245873e-28"), QString::fromUtf8("m²"));

    PUSH_CONSTANT("Weak Mixing Angle",
            QLatin1String("0.22215"), QLatin1String(""));

    PUSH_CONSTANT("Atomic Mass Unit",
            QLatin1String("1.66053886e-27"), QLatin1String("kg"));

    PUSH_CONSTANT("Avogadro's Number",
            QLatin1String("6.0221415e23"), QLatin1String("1/mol"));

    PUSH_CONSTANT("Boltzmann Constant",
            QLatin1String("1.3806505e-23"), QLatin1String("J/K"));

    PUSH_CONSTANT("Faraday, CONSTANT",
            QLatin1String("96485.3383"), QLatin1String("C/mol"));

    PUSH_CONSTANT("First Radiation Constant",
            QLatin1String("3.74177138e-16"), QString::fromUtf8("W·m²"));

    PUSH_CONSTANT("Gas Constant",
            QLatin1String("8.314472"), QString::fromUtf8("J/(K·mol)"));

    PUSH_CONSTANT("Loschmidt Constant",
            QLatin1String("2.6867773e25"), QString::fromUtf8("1/m³"));

    PUSH_CONSTANT("Molar Planck Constant",
            QLatin1String("3.990312716e-10"), QString::fromUtf8("J·s/mol"));

    PUSH_CONSTANT("Second Radiation Constant",
            QLatin1String("1.4387752e-2"), QString::fromUtf8("m·K"));

    PUSH_CONSTANT("Stefan-Boltzmann Constant",
            QLatin1String("5.670400e-8"), QString::fromUtf8("W/(m²·K⁴)"));

    PUSH_CONSTANT("Astronomical Unit",
            QLatin1String("149597870691"), QLatin1String("m"));

    PUSH_CONSTANT("Light Year",
            QLatin1String("9.4607304725808e15"), QLatin1String("m"));

    PUSH_CONSTANT("Parsec",
            QLatin1String("3.08567802e16"), QLatin1String("m"));

    PUSH_CONSTANT_DAYS("Gregorian Year", QLatin1String("365.2425"));
    PUSH_CONSTANT_DAYS("Julian Year",    QLatin1String("365.25"));
    PUSH_CONSTANT_DAYS("Sidereal Year",  QLatin1String("365.2564"));
    PUSH_CONSTANT_DAYS("Tropical Year",  QLatin1String("365.2422"));

    PUSH_CONSTANT("Earth Mass",
            QLatin1String("5.9736e24"), QLatin1String("kg"));

    PUSH_CONSTANT("Mean Earth Radius",
            QLatin1String("6371000"), QLatin1String("m"));

    PUSH_CONSTANT("Sun Mass",
            QLatin1String("1.9891e30"), QLatin1String("kg"));

    PUSH_CONSTANT("Sun Radius",
            QLatin1String("6.96265e8"), QLatin1String("m"));

    PUSH_CONSTANT("Sun Luminosity",
            QLatin1String("3.827e26"), QLatin1String("W"));
}

void Constants::Private::retranslateText()
{
    QList<Constant>::iterator i = list.begin();
    QString cat;

    // http://en.wikipedia.org/wiki/Physical_constant#Table_of_universal_constants
    cat = tr("General Physics");

    I18N_CONSTANT(tr("Characteristic Impedance of Vacuum"));
    I18N_CONSTANT(tr("Dirac's Constant"));
    I18N_CONSTANT(tr("Electric Constant"));
    I18N_CONSTANT(tr("Gravitation Constant"));
    I18N_CONSTANT(tr("Magnetic Constant"));
    I18N_CONSTANT(tr("Planck's Constant"));
    I18N_CONSTANT(tr("Speed of Light in Vacuum"));

    // http://en.wikipedia.org/wiki/Physical_constant#Table_of_electromagnetic_constants
    cat = tr("Electromagnetic");

    I18N_CONSTANT(tr("Bohr-Procopiu Magneton"));
    I18N_CONSTANT(tr("Conductance Quantum"));
    I18N_CONSTANT(tr("Coulomb's Constant"));
    I18N_CONSTANT(tr("Elementary Charge"));
    I18N_CONSTANT(tr("Josephson Constant"));
    I18N_CONSTANT(tr("Magnetic Flux Quantum"));
    I18N_CONSTANT(tr("Nuclear Magneton"));
    I18N_CONSTANT(tr("Resistance Quantum"));
    I18N_CONSTANT(tr("von Klitzing Constant"));

    // http://en.wikipedia.org/wiki/Physical_constant#Table_of_atomic_and_nuclear_constants
    cat = tr("Atomic & Nuclear");

    I18N_CONSTANT(tr("Bohr Radius"));
    I18N_CONSTANT(tr("Fermi Coupling Constant"));
    I18N_CONSTANT(tr("Fine-structure Constant"));
    I18N_CONSTANT(tr("Hartree Energy"));
    I18N_CONSTANT(tr("Quantum of Circulation"));
    I18N_CONSTANT(tr("Rydberg Constant"));
    I18N_CONSTANT(tr("Thomson Cross Section"));
    I18N_CONSTANT(tr("Weak Mixing Angle"));

    // http://en.wikipedia.org/wiki/Physical_constant#Table_of_physico-chemical_constants
    cat = tr("Physico-chemical");

    I18N_CONSTANT(tr("Atomic Mass Unit"));
    I18N_CONSTANT(tr("Avogadro's Number"));
    I18N_CONSTANT(tr("Boltzmann Constant"));
    I18N_CONSTANT(tr("Faraday Constant"));
    I18N_CONSTANT(tr("First Radiation Constant"));
    I18N_CONSTANT(tr("Gas Constant"));
    I18N_CONSTANT(tr("Loschmidt Constant"));
    I18N_CONSTANT(tr("Molar Planck Constant"));
    I18N_CONSTANT(tr("Second Radiation Constant"));
    I18N_CONSTANT(tr("Stefan-Boltzmann Constant"));

    // http://www.astronomynotes.com/tables/tablesa.htm
    cat = tr("Astronomy");

    I18N_CONSTANT(tr("Astronomical Unit"));
    I18N_CONSTANT(tr("Light Year"));
    I18N_CONSTANT(tr("Parsec"));

    const QString days = tr( "days" );

    I18N_CONSTANT_DAYS(tr("Gregorian Year"));
    I18N_CONSTANT_DAYS(tr("Julian Year"));
    I18N_CONSTANT_DAYS(tr("Sidereal Year"));
    I18N_CONSTANT_DAYS(tr("Tropical Year"));

    I18N_CONSTANT(tr("Earth Mass"));
    I18N_CONSTANT(tr("Mean Earth Radius"));
    I18N_CONSTANT(tr("Sun Mass"));
    I18N_CONSTANT(tr("Sun Radius"));
    I18N_CONSTANT(tr("Sun Luminosity"));

    categories.clear();
    for ( int k = 0; k < list.count(); ++k )
        if ( ! categories.contains(list.at(k).category) )
            categories += list.at( k ).category;
    categories.sort();
}

Constants * Constants::instance()
{
    if ( ! s_constantsInstance ) {
        s_constantsInstance = new Constants;
        qAddPostRoutine( s_deleteConstants );
    }

    return s_constantsInstance;
}

constant_name_is::constant_name_is( const QString & name )
    : m_name(name)
{
}

bool constant_name_is::operator()( const Constant & c ) const
{
    return c.name == m_name;
}

Constants::Constants()
    : d( new Constants::Private )
{
    setObjectName( "Constants" );
    d->populate();
    d->retranslateText();
}

Constants::~Constants()
{
}

const QList<Constant> & Constants::list() const
{
    return d->list;
}

const QStringList & Constants::categories() const
{
    return d->categories;
}

void Constants::retranslateText()
{
    d->retranslateText();
}
