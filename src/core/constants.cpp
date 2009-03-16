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
};

void Constants::Private::populate()
{
    Constant c;

    c.value = QLatin1String("376.730313461");
    c.unit = QString::fromUtf8("Ω");
    list << c;

    c.value = QLatin1String("8.854187817e-12");
    c.unit = QLatin1String("F/m");
    list << c;

    c.value = QLatin1String("1.256637061e-6");
    c.unit = QString::fromUtf8("N/A²");
    list << c;

    c.value = QLatin1String("6.67428e-11");
    c.unit = QString::fromUtf8("m³/(kg·s²)");
    list << c;

    c.value = QLatin1String("6.62606896e-34");
    c.unit = QString::fromUtf8("J·s");
    list << c;

    c.value = QLatin1String("1.054571628e-34");
    c.unit = QString::fromUtf8("J·s");
    list << c;

    c.value = QLatin1String("299792458");
    c.unit = QLatin1String("m/s");
    list << c;

    c.value = QLatin1String("927.400949e-26");
    c.unit = QLatin1String("J/T");
    list << c;

    c.value = QLatin1String("7.748091733e-5");
    c.unit = QLatin1String("S");
    list << c;

    c.value = QLatin1String("8.987742438e9");
    c.unit = QString::fromUtf8("N·m²/C²");
    list << c;

    c.value = QLatin1String("1.60217653e-19");
    c.unit = QLatin1String("C");
    list << c;

    c.value = QLatin1String("483597.879e9");
    c.unit = QLatin1String("Hz/V");
    list << c;

    c.value = QLatin1String("2.06783372e-15");
    c.unit = QLatin1String("Wb");
    list << c;

    c.value = QLatin1String("5.05078343e-27");
    c.unit = QLatin1String("J/T");
    list << c;

    c.value = QLatin1String("12906.403725");
    c.unit = QString::fromUtf8("Ω");
    list << c;

    c.value = QLatin1String("25812.807449");
    c.unit = QString::fromUtf8("Ω");
    list << c;

    c.value = QLatin1String("0.5291772108e-10");
    c.unit = QLatin1String("m");
    list << c;

    c.value = QLatin1String("1.16639e-5");
    c.unit = QString::fromUtf8("Ge/V²");
    list << c;

    c.value = QLatin1String("7.297352568e-3");
    c.unit = QLatin1String("");
    list << c;

    c.value = QLatin1String("4.35974417e-18");
    c.unit = QLatin1String("J");
    list << c;

    c.value = QLatin1String("3.636947550e-4");
    c.unit = QString::fromUtf8("m²/s");
    list << c;

    c.value = QLatin1String("10973731.568525");
    c.unit = QLatin1String("1/m");
    list << c;

    c.value = QLatin1String("0.665245873e-28");
    c.unit = QString::fromUtf8("m²");
    list << c;

    c.value = QLatin1String("0.22215");
    c.unit = QLatin1String("");
    list << c;

    c.value = QLatin1String("1.66053886e-27");
    c.unit = QLatin1String("kg");
    list << c;

    c.value = QLatin1String("6.0221415e23");
    c.unit = QLatin1String("1/mol");
    list << c;

    c.value = QLatin1String("1.3806505e-23");
    c.unit = QLatin1String("J/K");
    list << c;

    c.value = QLatin1String("96485.3383");
    c.unit = QLatin1String("C/mol");
    list << c;

    c.value = QLatin1String("3.74177138e-16");
    c.unit = QString::fromUtf8("W·m²");
    list << c;

    c.value = QLatin1String("2.6867773e25");
    c.unit = QString::fromUtf8("1/m³");
    list << c;

    c.value = QLatin1String("8.314472");
    c.unit = QString::fromUtf8("J/(K·mol)");
    list << c;

    c.value = QLatin1String("3.990312716e-10");
    c.unit = QString::fromUtf8("J·s/mol");
    list << c;

    c.value = QLatin1String("1.4387752e-2");
    c.unit = QString::fromUtf8("m·K");
    list << c;

    c.value = QLatin1String("5.670400e-8");
    c.unit = QString::fromUtf8("W/(m²·K⁴)");
    list << c;

    c.value = QLatin1String("149597870691");
    c.unit = QLatin1String("m");
    list << c;

    c.value = QLatin1String("9.4607304725808e15");
    c.unit = QLatin1String("m");
    list << c;

    c.value = QLatin1String("3.08567802e16");
    c.unit = QLatin1String("m");
    list << c;

    c.value = QLatin1String("365.2564");
    list << c;

    c.value = QLatin1String("365.2422");
    list << c;

    c.value = QLatin1String("365.2425");
    list << c;

    c.value = QLatin1String("365.25");
    list << c;

    c.value = QLatin1String("5.9736e24");
    c.unit = QLatin1String("kg");
    list << c;

    c.value = QLatin1String("1.9891e30");
    c.unit = QLatin1String("kg");
    list << c;

    c.value = QLatin1String("6371000");
    c.unit = QLatin1String("m");
    list << c;

    c.value = QLatin1String("6.96265e8");
    c.unit = QLatin1String("m");
    list << c;

    c.value = QLatin1String("3.827e26");
    c.unit = QLatin1String("W");
    list << c;
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
    retranslateText();
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
    QList<Constant>::iterator i = d->list.begin();
    QString cat;

    // http://en.wikipedia.org/wiki/Physical_constant#Table_of_universal_constants
    cat = tr("General Physics");

    i->category = cat;
    i->name = tr("Characteristic Impedance of Vacuum");
    ++i;

    i->category = cat;
    i->name = tr("Electric Constant");
    ++i;

    i->category = cat;
    i->name = tr("Magnetic Constant");
    ++i;

    i->category = cat;
    i->name = tr("Gravitation Constant");
    ++i;

    i->category = cat;
    i->name = tr("Planck's Constant");
    ++i;

    i->category = cat;
    i->name = tr("Dirac's Constant");
    ++i;

    i->category = cat;
    i->name = tr("Speed of Light in Vacuum");
    ++i;

    // http://en.wikipedia.org/wiki/Physical_constant#Table_of_electromagnetic_constants
    cat = tr("Electromagnetic");

    i->category = cat;
    i->name = tr("Bohr-Procopiu Magneton");
    ++i;

    i->category = cat;
    i->name = tr("Conductance Quantum");
    ++i;

    i->category = cat;
    i->name = tr("Coulomb's Constant");
    ++i;

    i->category = cat;
    i->name = tr("Elementary Charge");
    ++i;

    i->category = cat;
    i->name = tr("Josephson Constant");
    ++i;

    i->category = cat;
    i->name = tr("Magnetic Flux Quantum");
    ++i;

    i->category = cat;
    i->name = tr("Nuclear Magneton");
    ++i;

    i->category = cat;
    i->name = tr("Resistance Quantum");
    ++i;

    i->category = cat;
    i->name = tr("von Klitzing Constant");
    ++i;

    // http://en.wikipedia.org/wiki/Physical_constant#Table_of_atomic_and_nuclear_constants
    cat = tr("Atomic & Nuclear");

    i->category = cat;
    i->name = tr("Bohr Radius");
    ++i;

    i->category = cat;
    i->name = tr("Fermi Coupling Constant");
    ++i;

    i->category = cat;
    i->name = tr("Fine-structure Constant");
    ++i;

    i->category = cat;
    i->name = tr("Hartree Energy");
    ++i;

    i->category = cat;
    i->name = tr("Quantum of Circulation");
    ++i;

    i->category = cat;
    i->name = tr("Rydberg Constant");
    ++i;

    i->category = cat;
    i->name = tr("Thomson Cross Section");
    ++i;

    i->category = cat;
    i->name = tr("Weak Mixing Angle");
    ++i;

    // http://en.wikipedia.org/wiki/Physical_constant#Table_of_physico-chemical_constants
    cat = tr("Physico-chemical");

    i->category = cat;
    i->name = tr("Atomic Mass Unit");
    ++i;

    i->category = cat;
    i->name = tr("Avogadro's Number");
    ++i;

    i->category = cat;
    i->name = tr("Boltzmann Constant");
    ++i;

    i->category = cat;
    i->name = tr("Faraday Constant");
    ++i;

    i->category = cat;
    i->name = tr("First Radiation Constant");
    ++i;

    i->category = cat;
    i->name = tr("Loschmidt Constant");
    ++i;

    i->category = cat;
    i->name = tr("Gas Constant");
    ++i;

    i->category = cat;
    i->name = tr("Molar Planck Constant");
    ++i;

    i->category = cat;
    i->name = tr("Second Radiation Constant");
    ++i;

    i->category = cat;
    i->name = tr("Stefan-Boltzmann Constant");
    ++i;

    // http://www.astronomynotes.com/tables/tablesa.htm
    cat = tr("Astronomy");

    i->category = cat;
    i->name = tr("Astronomical Unit");
    ++i;

    i->category = cat;
    i->name = tr("Light Year");
    ++i;

    i->category = cat;
    i->name = tr("Parsec");
    ++i;

    const QString days = tr( "days" );

    i->category = cat;
    i->name = tr("Sidereal Year");
    i->unit = days;
    ++i;

    i->category = cat;
    i->name = tr("Tropical Year");
    i->unit = days;
    ++i;

    i->category = cat;
    i->name = tr("Gregorian Year");
    i->unit = days;
    ++i;

    i->category = cat;
    i->name = tr("Julian Year");
    i->unit = days;
    ++i;

    i->category = cat;
    i->name = tr("Earth Mass");
    ++i;

    i->category = cat;
    i->name = tr("Sun Mass");
    ++i;

    i->category = cat;
    i->name = tr("Mean Earth Radius");
    ++i;

    i->category = cat;
    i->name = tr("Sun Radius");
    ++i;

    i->category = cat;
    i->name = tr("Sun Luminosity");

    d->categories.clear();
    for ( int k = 0; k < d->list.count(); ++k )
        if ( ! d->categories.contains(d->list.at(k).category) )
            d->categories += d->list.at( k ).category;
    d->categories.sort();
}

