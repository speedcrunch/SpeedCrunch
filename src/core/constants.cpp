// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2007, 2009, 2011 Helder Correia <helder.pereira.correia@gmail.com>
// Copyright (C) 2009 Andreas Scherer <andreas_coder@freenet.de>
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

#include "core/constants.h"

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

    // General Physics.
    PUSH_CONSTANT("Characteristic Impedance of Vacuum", QLatin1String("376.730313461"), QString::fromUtf8("Ω"));
    PUSH_CONSTANT("Dirac's Constant", QLatin1String("1.054571628e-34"), QString::fromUtf8("J·s"));
    PUSH_CONSTANT("Electric Constant", QLatin1String("8.854187817e-12"), QLatin1String("F/m"));
    PUSH_CONSTANT("Gravitation Constant", QLatin1String("6.67428e-11"), QString::fromUtf8("m³/(kg·s²)"));
    PUSH_CONSTANT("Magnetic Constant", QLatin1String("1.256637061e-6"), QString::fromUtf8("N/A²"));
    PUSH_CONSTANT("Planck's Constant", QLatin1String("6.62606896e-34"), QString::fromUtf8("J·s"));
    PUSH_CONSTANT("Speed of Light in Vacuum", QLatin1String("299792458"), QLatin1String("m/s"));

    // Electromagnetic.
    PUSH_CONSTANT("Bohr-Procopiu Magneton", QLatin1String("927.400949e-26"), QLatin1String("J/T"));
    PUSH_CONSTANT("Conductance Quantum", QLatin1String("7.748091733e-5"), QLatin1String("S"));
    PUSH_CONSTANT("Coulomb's Constant", QLatin1String("8.987742438e9"), QString::fromUtf8("N·m²/C²"));
    PUSH_CONSTANT("Elementary Charge", QLatin1String("1.60217653e-19"), QLatin1String("C"));
    PUSH_CONSTANT("Josephson Constant", QLatin1String("483597.879e9"), QLatin1String("Hz/V"));
    PUSH_CONSTANT("Magnetic Flux Quantum", QLatin1String("2.06783372e-15"), QLatin1String("Wb"));
    PUSH_CONSTANT("Nuclear Magneton", QLatin1String("5.05078343e-27"), QLatin1String("J/T"));
    PUSH_CONSTANT("Resistance Quantum", QLatin1String("12906.403725"), QString::fromUtf8("Ω"));
    PUSH_CONSTANT("von Klitzing Constant", QLatin1String("25812.807449"), QString::fromUtf8("Ω"));

    // Atomic & Nuclear.
    PUSH_CONSTANT("Bohr Radius", QLatin1String("0.5291772108e-10"), QLatin1String("m"));
    PUSH_CONSTANT("Fermi Coupling Constant", QLatin1String("1.16639e-5"), QString::fromUtf8("Ge/V²"));
    PUSH_CONSTANT("Fine-structure Constant", QLatin1String("7.297352568e-3"), QLatin1String(""));
    PUSH_CONSTANT("Hartree Energy", QLatin1String("4.35974417e-18"), QLatin1String("J"));
    PUSH_CONSTANT("Quantum of Circulation", QLatin1String("3.636947550e-4"), QString::fromUtf8("m²/s"));
    PUSH_CONSTANT("Rydberg Constant", QLatin1String("10973731.568525"), QLatin1String("1/m"));
    PUSH_CONSTANT("Thomson Cross Section", QLatin1String("0.665245873e-28"), QString::fromUtf8("m²"));
    PUSH_CONSTANT("Weak Mixing Angle", QLatin1String("0.22215"), QLatin1String(""));

    // Physico-chemical.
    PUSH_CONSTANT("Atomic Mass Unit", QLatin1String("1.66053886e-27"), QLatin1String("kg"));
    PUSH_CONSTANT("Avogadro's Number", QLatin1String("6.0221415e23"), QLatin1String("1/mol"));
    PUSH_CONSTANT("Boltzmann Constant", QLatin1String("1.3806505e-23"), QLatin1String("J/K"));
    PUSH_CONSTANT("Faraday, CONSTANT", QLatin1String("96485.3383"), QLatin1String("C/mol"));
    PUSH_CONSTANT("First Radiation Constant", QLatin1String("3.74177138e-16"), QString::fromUtf8("W·m²"));
    PUSH_CONSTANT("Gas Constant", QLatin1String("8.314472"), QString::fromUtf8("J/(K·mol)"));
    PUSH_CONSTANT("Loschmidt Constant", QLatin1String("2.6867773e25"), QString::fromUtf8("1/m³"));
    PUSH_CONSTANT("Molar Planck Constant", QLatin1String("3.990312716e-10"), QString::fromUtf8("J·s/mol"));
    PUSH_CONSTANT("Second Radiation Constant", QLatin1String("1.4387752e-2"), QString::fromUtf8("m·K"));
    PUSH_CONSTANT("Stefan-Boltzmann Constant", QLatin1String("5.670400e-8"), QString::fromUtf8("W/(m²·K⁴)"));

    // Astronomy.
    PUSH_CONSTANT("Astronomical Unit", QLatin1String("149597870691"), QLatin1String("m"));
    PUSH_CONSTANT("Light Year", QLatin1String("9.4607304725808e15"), QLatin1String("m"));
    PUSH_CONSTANT("Parsec", QLatin1String("3.08567802e16"), QLatin1String("m"));
    PUSH_CONSTANT_DAYS("Gregorian Year", QLatin1String("365.2425"));
    PUSH_CONSTANT_DAYS("Julian Year", QLatin1String("365.25"));
    PUSH_CONSTANT_DAYS("Sidereal Year", QLatin1String("365.2564"));
    PUSH_CONSTANT_DAYS("Tropical Year", QLatin1String("365.2422"));
    PUSH_CONSTANT("Earth Mass", QLatin1String("5.9736e24"), QLatin1String("kg"));
    PUSH_CONSTANT("Mean Earth Radius", QLatin1String("6371000"), QLatin1String("m"));
    PUSH_CONSTANT("Sun Mass", QLatin1String("1.9891e30"), QLatin1String("kg"));
    PUSH_CONSTANT("Sun Radius", QLatin1String("6.96265e8"), QLatin1String("m"));
    PUSH_CONSTANT("Sun Luminosity", QLatin1String("3.827e26"), QLatin1String("W"));

    // Molar Masses
    PUSH_CONSTANT("Aluminium", QLatin1String("26.9815386"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Antimony", QLatin1String("121.760"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Argon", QLatin1String("39.948"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Arsenic", QLatin1String("74.92160"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Barium", QLatin1String("137.327"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Beryllium", QLatin1String("9.012182"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Bismuth", QLatin1String("208.98040"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Boron", QLatin1String("10.811"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Bromine", QLatin1String("79.904"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Cadmium", QLatin1String("112.411"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Caesium", QLatin1String("132.9054519"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Calcium", QLatin1String("40.078"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Carbon", QLatin1String("12.0107"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Cerium", QLatin1String("140.116"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Chlorine", QLatin1String("35.453"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Chromium", QLatin1String("51.9961"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Cobalt", QLatin1String("58.933195"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Copper", QLatin1String("63.546"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Dysprosium", QLatin1String("162.500"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Erbium", QLatin1String("167.259"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Europium", QLatin1String("151.964"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Fluorine", QLatin1String("18.9984032"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Gadolinium", QLatin1String("157.25"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Gallium", QLatin1String("69.723"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Germanium", QLatin1String("72.64"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Gold", QLatin1String("196.966569"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Hafnium", QLatin1String("178.49"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Helium", QLatin1String("4.002602"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Holmium", QLatin1String("164.93032"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Hydrogen", QLatin1String("1.00794"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Indium", QLatin1String("114.818"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Iodine", QLatin1String("126.90447"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Iridium", QLatin1String("192.217"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Iron", QLatin1String("55.845"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Krypton", QLatin1String("83.798"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Lanthanum", QLatin1String("138.90547"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Lead", QLatin1String("207.2"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Lithium", QLatin1String("6.941"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Lutetium", QLatin1String("174.9668"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Magnesium", QLatin1String("24.3050"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Manganese", QLatin1String("54.938045"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Mercury", QLatin1String("200.59"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Molybdenum", QLatin1String("95.96"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Neodymium", QLatin1String("144.242"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Neon", QLatin1String("20.1797"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Nickel", QLatin1String("58.6934"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Niobium", QLatin1String("92.90638"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Nitrogen", QLatin1String("14.0067"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Osmium", QLatin1String("190.23"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Oxygen", QLatin1String("15.9994"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Palladium", QLatin1String("106.42"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Phosphorus", QLatin1String("30.973762"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Platinum", QLatin1String("192.084"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Potassium", QLatin1String("39.0983"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Praseodymium", QLatin1String("140.90765"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Protactinium", QLatin1String("231.03588"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Rhenium", QLatin1String("186.207"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Rubidium", QLatin1String("85.4678"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Ruthenium", QLatin1String("101.07"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Samarium", QLatin1String("150.36"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Scandium", QLatin1String("44.955912"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Selenium", QLatin1String("78.96"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Silicon", QLatin1String("28.0855"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Silver", QLatin1String("107.8682"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Sodium", QLatin1String("22.98976928"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Strontium", QLatin1String("87.62"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Sulfur", QLatin1String("32.065"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Tantalum", QLatin1String("180.94788"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Tellurium", QLatin1String("127.60"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Terbium", QLatin1String("158.92535"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Thallium", QLatin1String("204.3833"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Thorium", QLatin1String("232.03806"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Thulium", QLatin1String("168.93421"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Tin", QLatin1String("118.710"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Titanium", QLatin1String("47.867"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Tungsten", QLatin1String("183.84"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Uranium", QLatin1String("238.02891"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Vanadium", QLatin1String("51.9961"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Xenon", QLatin1String("131.293"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Ytterbium", QLatin1String("173.054"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Yttrium", QLatin1String("88.90585"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Zinc", QLatin1String("65.38"), QLatin1String("g/mol"));
    PUSH_CONSTANT("Zirconium", QLatin1String("91.224"), QLatin1String("g/mol"));
}

void Constants::Private::retranslateText()
{
    QList<Constant>::iterator i = list.begin();
    QString cat;

    // http://en.wikipedia.org/wiki/Physical_constant#Table_of_universal_constants
    cat = Constants::tr("General Physics");

    I18N_CONSTANT(Constants::tr("Characteristic Impedance of Vacuum"));
    I18N_CONSTANT(Constants::tr("Dirac's Constant"));
    I18N_CONSTANT(Constants::tr("Electric Constant"));
    I18N_CONSTANT(Constants::tr("Gravitation Constant"));
    I18N_CONSTANT(Constants::tr("Magnetic Constant"));
    I18N_CONSTANT(Constants::tr("Planck's Constant"));
    I18N_CONSTANT(Constants::tr("Speed of Light in Vacuum"));

    // http://en.wikipedia.org/wiki/Physical_constant#Table_of_electromagnetic_constants
    cat = Constants::tr("Electromagnetic");

    I18N_CONSTANT(Constants::tr("Bohr-Procopiu Magneton"));
    I18N_CONSTANT(Constants::tr("Conductance Quantum"));
    I18N_CONSTANT(Constants::tr("Coulomb's Constant"));
    I18N_CONSTANT(Constants::tr("Elementary Charge"));
    I18N_CONSTANT(Constants::tr("Josephson Constant"));
    I18N_CONSTANT(Constants::tr("Magnetic Flux Quantum"));
    I18N_CONSTANT(Constants::tr("Nuclear Magneton"));
    I18N_CONSTANT(Constants::tr("Resistance Quantum"));
    I18N_CONSTANT(Constants::tr("von Klitzing Constant"));

    // http://en.wikipedia.org/wiki/Physical_constant#Table_of_atomic_and_nuclear_constants
    cat = Constants::tr("Atomic & Nuclear");

    I18N_CONSTANT(Constants::tr("Bohr Radius"));
    I18N_CONSTANT(Constants::tr("Fermi Coupling Constant"));
    I18N_CONSTANT(Constants::tr("Fine-structure Constant"));
    I18N_CONSTANT(Constants::tr("Hartree Energy"));
    I18N_CONSTANT(Constants::tr("Quantum of Circulation"));
    I18N_CONSTANT(Constants::tr("Rydberg Constant"));
    I18N_CONSTANT(Constants::tr("Thomson Cross Section"));
    I18N_CONSTANT(Constants::tr("Weak Mixing Angle"));

    // http://en.wikipedia.org/wiki/Physical_constant#Table_of_physico-chemical_constants
    cat = Constants::tr("Physico-chemical");

    I18N_CONSTANT(Constants::tr("Atomic Mass Unit"));
    I18N_CONSTANT(Constants::tr("Avogadro's Number"));
    I18N_CONSTANT(Constants::tr("Boltzmann Constant"));
    I18N_CONSTANT(Constants::tr("Faraday Constant"));
    I18N_CONSTANT(Constants::tr("First Radiation Constant"));
    I18N_CONSTANT(Constants::tr("Gas Constant"));
    I18N_CONSTANT(Constants::tr("Loschmidt Constant"));
    I18N_CONSTANT(Constants::tr("Molar Planck Constant"));
    I18N_CONSTANT(Constants::tr("Second Radiation Constant"));
    I18N_CONSTANT(Constants::tr("Stefan-Boltzmann Constant"));

    // http://www.astronomynotes.com/tables/tablesa.htm
    cat = Constants::tr("Astronomy");

    I18N_CONSTANT(Constants::tr("Astronomical Unit"));
    I18N_CONSTANT(Constants::tr("Light Year"));
    I18N_CONSTANT(Constants::tr("Parsec"));

    const QString days = Constants::tr("days");

    I18N_CONSTANT_DAYS(Constants::tr("Gregorian Year"));
    I18N_CONSTANT_DAYS(Constants::tr("Julian Year"));
    I18N_CONSTANT_DAYS(Constants::tr("Sidereal Year"));
    I18N_CONSTANT_DAYS(Constants::tr("Tropical Year"));

    I18N_CONSTANT(Constants::tr("Earth Mass"));
    I18N_CONSTANT(Constants::tr("Mean Earth Radius"));
    I18N_CONSTANT(Constants::tr("Sun Mass"));
    I18N_CONSTANT(Constants::tr("Sun Radius"));
    I18N_CONSTANT(Constants::tr("Sun Luminosity"));

    // http://www.ptable.com/
    // Only known constants of accuracy more than an integer are included.
    cat = Constants::tr("Molar Mass");

    I18N_CONSTANT(Constants::tr("Aluminium"));
    I18N_CONSTANT(Constants::tr("Antimony"));
    I18N_CONSTANT(Constants::tr("Argon"));
    I18N_CONSTANT(Constants::tr("Arsenic"));
    I18N_CONSTANT(Constants::tr("Barium"));
    I18N_CONSTANT(Constants::tr("Beryllium"));
    I18N_CONSTANT(Constants::tr("Bismuth"));
    I18N_CONSTANT(Constants::tr("Boron"));
    I18N_CONSTANT(Constants::tr("Bromine"));
    I18N_CONSTANT(Constants::tr("Cadmium"));
    I18N_CONSTANT(Constants::tr("Caesium"));
    I18N_CONSTANT(Constants::tr("Calcium"));
    I18N_CONSTANT(Constants::tr("Carbon"));
    I18N_CONSTANT(Constants::tr("Cerium"));
    I18N_CONSTANT(Constants::tr("Chlorine"));
    I18N_CONSTANT(Constants::tr("Chromium"));
    I18N_CONSTANT(Constants::tr("Cobalt"));
    I18N_CONSTANT(Constants::tr("Copper"));
    I18N_CONSTANT(Constants::tr("Dysprosium"));
    I18N_CONSTANT(Constants::tr("Erbium"));
    I18N_CONSTANT(Constants::tr("Europium"));
    I18N_CONSTANT(Constants::tr("Fluorine"));
    I18N_CONSTANT(Constants::tr("Gadolinium"));
    I18N_CONSTANT(Constants::tr("Gallium"));
    I18N_CONSTANT(Constants::tr("Germanium"));
    I18N_CONSTANT(Constants::tr("Gold"));
    I18N_CONSTANT(Constants::tr("Hafnium"));
    I18N_CONSTANT(Constants::tr("Helium"));
    I18N_CONSTANT(Constants::tr("Holmium"));
    I18N_CONSTANT(Constants::tr("Hydrogen"));
    I18N_CONSTANT(Constants::tr("Indium"));
    I18N_CONSTANT(Constants::tr("Iodine"));
    I18N_CONSTANT(Constants::tr("Iridium"));
    I18N_CONSTANT(Constants::tr("Iron"));
    I18N_CONSTANT(Constants::tr("Krypton"));
    I18N_CONSTANT(Constants::tr("Lanthanum"));
    I18N_CONSTANT(Constants::tr("Lead"));
    I18N_CONSTANT(Constants::tr("Lithium"));
    I18N_CONSTANT(Constants::tr("Lutetium"));
    I18N_CONSTANT(Constants::tr("Magnesium"));
    I18N_CONSTANT(Constants::tr("Manganese"));
    I18N_CONSTANT(Constants::tr("Mercury"));
    I18N_CONSTANT(Constants::tr("Molybdenum"));
    I18N_CONSTANT(Constants::tr("Neodymium"));
    I18N_CONSTANT(Constants::tr("Neon"));
    I18N_CONSTANT(Constants::tr("Nickel"));
    I18N_CONSTANT(Constants::tr("Niobium"));
    I18N_CONSTANT(Constants::tr("Nitrogen"));
    I18N_CONSTANT(Constants::tr("Osmium"));
    I18N_CONSTANT(Constants::tr("Oxygen"));
    I18N_CONSTANT(Constants::tr("Palladium"));
    I18N_CONSTANT(Constants::tr("Phosphorus"));
    I18N_CONSTANT(Constants::tr("Platinum"));
    I18N_CONSTANT(Constants::tr("Potassium"));
    I18N_CONSTANT(Constants::tr("Praseodymium"));
    I18N_CONSTANT(Constants::tr("Protactinium"));
    I18N_CONSTANT(Constants::tr("Rhenium"));
    I18N_CONSTANT(Constants::tr("Rubidium"));
    I18N_CONSTANT(Constants::tr("Ruthenium"));
    I18N_CONSTANT(Constants::tr("Samarium"));
    I18N_CONSTANT(Constants::tr("Scandium"));
    I18N_CONSTANT(Constants::tr("Selenium"));
    I18N_CONSTANT(Constants::tr("Silicon"));
    I18N_CONSTANT(Constants::tr("Silver"));
    I18N_CONSTANT(Constants::tr("Sodium"));
    I18N_CONSTANT(Constants::tr("Strontium"));
    I18N_CONSTANT(Constants::tr("Sulfur"));
    I18N_CONSTANT(Constants::tr("Tantalum"));
    I18N_CONSTANT(Constants::tr("Tellurium"));
    I18N_CONSTANT(Constants::tr("Terbium"));
    I18N_CONSTANT(Constants::tr("Thallium"));
    I18N_CONSTANT(Constants::tr("Thorium"));
    I18N_CONSTANT(Constants::tr("Thulium"));
    I18N_CONSTANT(Constants::tr("Tin"));
    I18N_CONSTANT(Constants::tr("Titanium"));
    I18N_CONSTANT(Constants::tr("Tungsten"));
    I18N_CONSTANT(Constants::tr("Uranium"));
    I18N_CONSTANT(Constants::tr("Vanadium"));
    I18N_CONSTANT(Constants::tr("Xenon"));
    I18N_CONSTANT(Constants::tr("Ytterbium"));
    I18N_CONSTANT(Constants::tr("Yttrium"));
    I18N_CONSTANT(Constants::tr("Zinc"));
    I18N_CONSTANT(Constants::tr("Zirconium"));

    categories.clear();
    for (int k = 0; k < list.count(); ++k)
        if (!categories.contains(list.at(k).category))
            categories += list.at(k).category;
    categories.sort();
}

Constants * Constants::instance()
{
    if (!s_constantsInstance) {
        s_constantsInstance = new Constants;
        qAddPostRoutine(s_deleteConstants);
    }
    return s_constantsInstance;
}

constant_name_is::constant_name_is(const QString &name)
    : m_name(name)
{
}

bool constant_name_is::operator()(const Constant &c) const
{
    return c.name == m_name;
}

Constants::Constants()
    : d(new Constants::Private)
{
    setObjectName("Constants");
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
