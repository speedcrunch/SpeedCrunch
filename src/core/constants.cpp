// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
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


#include "constants.hxx"

#include <QApplication>


struct Constants::Private
{
  QList<Constant> constantList;
  QStringList     categoryList;

  void createConstants();
};


void Constants::Private::createConstants()
{
  constantList.clear();
  categoryList.clear();

  // http://en.wikipedia.org/wiki/Physical_constant#Table_of_universal_constants
  constantList += Constant( Constants::tr( "Characteristic Impedance of Vacuum" ), "376.730313461",   "ohm",          Constants::tr( "General Physics" ) );
  constantList += Constant( Constants::tr( "Electric Constant"                  ), "8.854187817e-12", "F/m",          Constants::tr( "General Physics" ) );
  constantList += Constant( Constants::tr( "Magnetic Constant"                  ), "1.256637061e-6",  "N/A^2",        Constants::tr( "General Physics" ) );
  constantList += Constant( Constants::tr( "Gravitation Constant"               ), "6.67428e-11",     "m^3/(kg s^2)", Constants::tr( "General Physics" ) );
  constantList += Constant( Constants::tr( "Planck's Constant"                  ), "6.62606896e-34",  "J s",          Constants::tr( "General Physics" ) );
  constantList += Constant( Constants::tr( "Dirac's Constant"                   ), "1.054571628e-34", "J s",          Constants::tr( "General Physics" ) );
  constantList += Constant( Constants::tr( "Speed of Light in Vacuum"           ), "299792458",       "m/s",          Constants::tr( "General Physics" ) );

  // http://en.wikipedia.org/wiki/Physical_constant#Table_of_electromagnetic_constants
  constantList += Constant( Constants::tr( "Bohr-Procopiu Magneton" ), "927.400949e-26", "J/T",       Constants::tr( "Electromagnetic" ) );
  constantList += Constant( Constants::tr( "Conductance Quantum"    ), "7.748091733e-5", "S",         Constants::tr( "Electromagnetic" ) );
  constantList += Constant( Constants::tr( "Coulomb's Constant"     ), "8.987742438e9",  "N m^2/C^2", Constants::tr( "Electromagnetic" ) );
  constantList += Constant( Constants::tr( "Elementary Charge"      ), "1.60217653e-19", "C",         Constants::tr( "Electromagnetic" ) );
  constantList += Constant( Constants::tr( "Josephson Constant"     ), "483597.879e9",   "Hz/V",      Constants::tr( "Electromagnetic" ) );
  constantList += Constant( Constants::tr( "Magnetic Flux Quantum"  ), "2.06783372e-15", "Wb",        Constants::tr( "Electromagnetic" ) );
  constantList += Constant( Constants::tr( "Nuclear Magneton"       ), "5.05078343e-27", "J/T",       Constants::tr( "Electromagnetic" ) );
  constantList += Constant( Constants::tr( "Resistance Quantum"     ), "12906.403725",   "ohm",       Constants::tr( "Electromagnetic" ) );
  constantList += Constant( Constants::tr( "von Klitzing Constant"  ), "25812.807449",   "ohm",       Constants::tr( "Electromagnetic" ) );

  // http://en.wikipedia.org/wiki/Physical_constant#Table_of_atomic_and_nuclear_constants
  constantList += Constant( Constants::tr( "Bohr Radius"             ), "0.5291772108e-10", "m",       Constants::tr( "Atomic & Nuclear" ) );
  constantList += Constant( Constants::tr( "Fermi Coupling Constant" ), "1.16639e-5",       "Ge/V^2",  Constants::tr( "Atomic & Nuclear" ) );
  constantList += Constant( Constants::tr( "Fine-structure Constant" ), "7.297352568e-3",   QString(), Constants::tr( "Atomic & Nuclear" ) );
  constantList += Constant( Constants::tr( "Hartree Energy"          ), "4.35974417e-18",   "J",       Constants::tr( "Atomic & Nuclear" ) );
  constantList += Constant( Constants::tr( "Quantum of Circulation"  ), "3.636947550e-4",   "m^2/s",   Constants::tr( "Atomic & Nuclear" ) );
  constantList += Constant( Constants::tr( "Rydberg Constant"        ), "10973731.568525",  "m^(-1)",  Constants::tr( "Atomic & Nuclear" ) );
  constantList += Constant( Constants::tr( "Thomson Cross Section"   ), "0.665245873e-28",  "m^2",     Constants::tr( "Atomic & Nuclear" ) );
  constantList += Constant( Constants::tr( "Weak Mixing Angle"       ), "0.22215",          QString(), Constants::tr( "Atomic & Nuclear" ) );

  // http://en.wikipedia.org/wiki/Physical_constant#Table_of_physico-chemical_constants
  constantList += Constant( Constants::tr( "Atomic Mass Unit"          ), "1.66053886e-27",  "kg",          Constants::tr( "Physico-chemical" ) );
  constantList += Constant( Constants::tr( "Avogadro's Number"         ), "6.0221415e23",    QString(),     Constants::tr( "Physico-chemical" ) );
  constantList += Constant( Constants::tr( "Boltzmann Constant"        ), "1.3806505e-23",   "J/K",         Constants::tr( "Physico-chemical" ) );
  constantList += Constant( Constants::tr( "Faraday Constant"          ), "96485.3383",      "C/mol",       Constants::tr( "Physico-chemical" ) );
  constantList += Constant( Constants::tr( "First Radiation Constant"  ), "3.74177138e-16",  "W m^2",       Constants::tr( "Physico-chemical" ) );
  constantList += Constant( Constants::tr( "Loschmidt Constant"        ), "2.6867773e25",    "m^(-3)",      Constants::tr( "Physico-chemical" ) );
  constantList += Constant( Constants::tr( "Gas Constant"              ), "8.314472",        "J/(K mol)",   Constants::tr( "Physico-chemical" ) );
  constantList += Constant( Constants::tr( "Molar Planck Constant"     ), "3.990312716e-10", "J s/mol",     Constants::tr( "Physico-chemical" ) );
  constantList += Constant( Constants::tr( "Second Radiation Constant" ), "1.4387752e-2",    "m K",         Constants::tr( "Physico-chemical" ) );
  constantList += Constant( Constants::tr( "Stefan-Boltzmann Constant" ), "5.670400e-8",     "W/(m^2 K^4)", Constants::tr( "Physico-chemical" ) );

  // http://www.astronomynotes.com/tables/tablesa.htm
  constantList += Constant( Constants::tr( "Astronomical Unit" ), "149597870691",       "m",                     Constants::tr( "Astronomy" ) );
  constantList += Constant( Constants::tr( "Light Year"        ), "9.4607304725808e15", "m",                     Constants::tr( "Astronomy" ) );
  constantList += Constant( Constants::tr( "Parsec"            ), "3.08567802e16",      "m",                     Constants::tr( "Astronomy" ) );
  constantList += Constant( Constants::tr( "Sidereal Year"     ), "365.2564",           Constants::tr( "days" ), Constants::tr( "Astronomy" ) );
  constantList += Constant( Constants::tr( "Tropical Year"     ), "365.2422",           Constants::tr( "days" ), Constants::tr( "Astronomy" ) );
  constantList += Constant( Constants::tr( "Gregorian Year"    ), "365.2425",           Constants::tr( "days" ), Constants::tr( "Astronomy" ) );
  constantList += Constant( Constants::tr( "Earth Mass"        ), "5.9736e24",          "kg",                    Constants::tr( "Astronomy" ) );
  constantList += Constant( Constants::tr( "Sun Mass"          ), "1.9891e30",          "kg",                    Constants::tr( "Astronomy" ) );
  constantList += Constant( Constants::tr( "Mean Earth Radius" ), "6371000",            "m",                     Constants::tr( "Astronomy" ) );
  constantList += Constant( Constants::tr( "Sun Radius"        ), "6.96265e8",          "m",                     Constants::tr( "Astronomy" ) );
  constantList += Constant( Constants::tr( "Sun Luminosity"    ), "3.827e26",           "W",                     Constants::tr( "Astronomy" ) );

  for ( int k = 0; k < constantList.count(); k++ )
  {
    QStringList cats = constantList[k].categories;
    for ( int i = 0; i < cats.count(); i++ )
      if ( ! categoryList.contains( cats[i] ) )
        categoryList += cats[i];
  }
  categoryList.sort();
}


Constants::Constants( QObject * parent )
  : QObject( parent ), d( new Constants::Private )
{
  setObjectName( "Constants" );
  d->createConstants();
}


Constants::~Constants()
{
  delete d;
}


QList<Constant> Constants::constantList() const
{
  return d->constantList;
}


QStringList Constants::categoryList() const
{
  return d->categoryList;
}


// public slots

void Constants::retranslateText()
{
  d->createConstants();
}
