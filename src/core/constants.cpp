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

    void createConstants();
};

void Constants::Private::createConstants()
{
}

Constants * Constants::instance()
{
    if ( ! s_constantsInstance ) {
        s_constantsInstance = new Constants;
        qAddPostRoutine( s_deleteConstants );
    }

    return s_constantsInstance;
}

constant_name_is::constant_name_is(const QString& name)
    : m_name(name)
{}

bool constant_name_is::operator()(const Constant& c) const {
    return c.name == m_name;
}

Constants::Constants()
    : d( new Constants::Private )
{
    setObjectName( "Constants" );
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
    d->list.clear();
    d->categories.clear();

    QString cat; // category translation

    // http://en.wikipedia.org/wiki/Physical_constant#Table_of_universal_constants
    cat = tr( "General Physics" );
    d->list += Constant( tr("Characteristic Impedance of Vacuum"), "376.730313461", "ohm",  cat );
    d->list += Constant( tr("Electric Constant"),        "8.854187817e-12", "F/m",          cat );
    d->list += Constant( tr("Magnetic Constant"),        "1.256637061e-6",  "N/A^2",        cat );
    d->list += Constant( tr("Gravitation Constant"),     "6.67428e-11",     "m^3/(kg s^2)", cat );
    d->list += Constant( tr("Planck's Constant"),        "6.62606896e-34",  "J s",          cat );
    d->list += Constant( tr("Dirac's Constant"),         "1.054571628e-34", "J s",          cat );
    d->list += Constant( tr("Speed of Light in Vacuum"), "299792458",       "m/s",          cat );

    // http://en.wikipedia.org/wiki/Physical_constant#Table_of_electromagnetic_constants
    cat = tr( "Electromagnetic" );
    d->list += Constant( tr("Bohr-Procopiu Magneton"), "927.400949e-26", "J/T",       cat );
    d->list += Constant( tr("Conductance Quantum"),    "7.748091733e-5", "S",         cat );
    d->list += Constant( tr("Coulomb's Constant"),     "8.987742438e9",  "N m^2/C^2", cat );
    d->list += Constant( tr("Elementary Charge"),      "1.60217653e-19", "C",         cat );
    d->list += Constant( tr("Josephson Constant"),     "483597.879e9",   "Hz/V",      cat );
    d->list += Constant( tr("Magnetic Flux Quantum"),  "2.06783372e-15", "Wb",        cat );
    d->list += Constant( tr("Nuclear Magneton"),       "5.05078343e-27", "J/T",       cat );
    d->list += Constant( tr("Resistance Quantum"),     "12906.403725",   "ohm",       cat );
    d->list += Constant( tr("von Klitzing Constant"),  "25812.807449",   "ohm",       cat );

    // http://en.wikipedia.org/wiki/Physical_constant#Table_of_atomic_and_nuclear_constants
    cat = tr( "Atomic & Nuclear" );
    d->list += Constant( tr("Bohr Radius"),             "0.5291772108e-10", "m",       cat );
    d->list += Constant( tr("Fermi Coupling Constant"), "1.16639e-5",       "Ge/V^2",  cat );
    d->list += Constant( tr("Fine-structure Constant"), "7.297352568e-3",   QString(), cat );
    d->list += Constant( tr("Hartree Energy"),          "4.35974417e-18",   "J",       cat );
    d->list += Constant( tr("Quantum of Circulation"),  "3.636947550e-4",   "m^2/s",   cat );
    d->list += Constant( tr("Rydberg Constant"),        "10973731.568525",  "m^(-1)",  cat );
    d->list += Constant( tr("Thomson Cross Section"),   "0.665245873e-28",  "m^2",     cat );
    d->list += Constant( tr("Weak Mixing Angle"),       "0.22215",          QString(), cat );

    // http://en.wikipedia.org/wiki/Physical_constant#Table_of_physico-chemical_constants
    cat = tr( "Physico-chemical" );
    d->list += Constant( tr("Atomic Mass Unit"),          "1.66053886e-27",  "kg",          cat );
    d->list += Constant( tr("Avogadro's Number"),         "6.0221415e23",    QString(),     cat );
    d->list += Constant( tr("Boltzmann Constant"),        "1.3806505e-23",   "J/K",         cat );
    d->list += Constant( tr("Faraday Constant"),          "96485.3383",      "C/mol",       cat );
    d->list += Constant( tr("First Radiation Constant"),  "3.74177138e-16",  "W m^2",       cat );
    d->list += Constant( tr("Loschmidt Constant"),        "2.6867773e25",    "m^(-3)",      cat );
    d->list += Constant( tr("Gas Constant"),              "8.314472",        "J/(K mol)",   cat );
    d->list += Constant( tr("Molar Planck Constant"),     "3.990312716e-10", "J s/mol",     cat );
    d->list += Constant( tr("Second Radiation Constant"), "1.4387752e-2",    "m K",         cat );
    d->list += Constant( tr("Stefan-Boltzmann Constant"), "5.670400e-8",     "W/(m^2 K^4)", cat );

    // http://www.astronomynotes.com/tables/tablesa.htm
    cat = tr( "Astronomy" );
    const QString days = tr( "days" );
    d->list += Constant( tr("Astronomical Unit"), "149597870691",       "m",  cat );
    d->list += Constant( tr("Light Year"),        "9.4607304725808e15", "m",  cat );
    d->list += Constant( tr("Parsec"),            "3.08567802e16",      "m",  cat );
    d->list += Constant( tr("Sidereal Year"),     "365.2564",           days, cat );
    d->list += Constant( tr("Tropical Year"),     "365.2422",           days, cat );
    d->list += Constant( tr("Gregorian Year"),    "365.2425",           days, cat );
    d->list += Constant( tr("Earth Mass"),        "5.9736e24",          "kg", cat );
    d->list += Constant( tr("Sun Mass"),          "1.9891e30",          "kg", cat );
    d->list += Constant( tr("Mean Earth Radius"), "6371000",            "m",  cat );
    d->list += Constant( tr("Sun Radius"),        "6.96265e8",          "m",  cat );
    d->list += Constant( tr("Sun Luminosity"),    "3.827e26",           "W",  cat );

    for ( int k = 0; k < d->list.count(); k++ ) {
        QStringList cats = d->list.at( k ).categories;
        for ( int i = 0; i < cats.count(); i++ )
            if ( ! d->categories.contains( cats.at(i) ) )
                d->categories += cats.at( i );
    }
    d->categories.sort();
}

