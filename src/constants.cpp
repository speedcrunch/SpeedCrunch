/* This file is part of the SpeedCrunch project
   Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
 */

#include "constants.h"

#include <QApplication>

Constants* s_global_constants = 0;

static void deleteGlobalConstants()
{
  delete s_global_constants;
  s_global_constants = 0;
}


Constants::Constants(): QObject( 0 )
{
  setObjectName( "Constants" );

  // http://en.wikipedia.org/wiki/Physical_constant#Table_of_universal_constants
  constantList += Constant( tr("characteristic impedance of vacuum"), "376.730313461", "ohm", tr("General Physics") );
  constantList += Constant( tr("electric constant"), "8.854187817e-12", "F/m", tr("General Physics") );
  constantList += Constant( tr("magnetic constant"), "1.256637061e-6", "N/A^2", tr("General Physics") );
  constantList += Constant( tr("gravitation constant"), "6.67428e-11", "m^3/(kgs^2)  ", tr("General Physics") );
  constantList += Constant( tr("Planck's constant"), "6.62606896e-34", "Js ", tr("General Physics") );
  constantList += Constant( tr("Dirac's constant"), "1.054571628e-34", "Js ", tr("General Physics") );
  constantList += Constant( tr("speed of light in vacuum"), "299792458", "m/s", tr("General Physics") );
      	
  // http://en.wikipedia.org/wiki/Physical_constant#Table_of_electromagnetic_constants
  constantList += Constant( tr("Bohr-Procopiu magneton"), "927.400949e-26", "J/T", tr("Electromagnetic") );
  constantList += Constant( tr("conductance quantum"), "7.748091733e-5", "S", tr("Electromagnetic") );
  constantList += Constant( tr("Coulomb's constant"), "8.987742438e9", "Nm^2/C^2", tr("Electromagnetic") );
  constantList += Constant( tr("elementary charge  "), "1.60217653e-19", "C", tr("Electromagnetic") );
  constantList += Constant( tr("Josephson constant"), "483597.879e9", "Hz/V", tr("Electromagnetic") );
  constantList += Constant( tr("magnetic flux quantum"), "2.06783372e-15", "Wb", tr("Electromagnetic") );
  constantList += Constant( tr("nuclear magneton"), "5.05078343e-27", "J/T", tr("Electromagnetic") );
  constantList += Constant( tr("resistance quantum "), "12906.403725", "ohm", tr("Electromagnetic") );
  constantList += Constant( tr("von Klitzing constant"), "25812.807449", "ohm", tr("Electromagnetic") );

  // http://en.wikipedia.org/wiki/Physical_constant#Table_of_atomic_and_nuclear_constants
  constantList += Constant( tr("Bohr radius"), "0.5291772108e-10", "m", tr("Atomic & Nuclear") );
  constantList += Constant( tr("Fermi coupling constant"), "1.16639e-5", "Ge/V^2", tr("Atomic & Nuclear") );
  constantList += Constant( tr("fine-structure constant"), "7.297352568e-3", QString(), tr("Atomic & Nuclear") );
  constantList += Constant( tr("Hartree energy"), "4.35974417e-18", "J", tr("Atomic & Nuclear") );
  constantList += Constant( tr("quantum of circulation"), "3.636947550e-4", "m^2/s", tr("Atomic & Nuclear") );
  constantList += Constant( tr("Rydberg constant"), "10973731.568525", "/m", tr("Atomic & Nuclear") );
  constantList += Constant( tr("Thomson cross section"), "0.665245873e-28", "m^2", tr("Atomic & Nuclear") );
  constantList += Constant( tr("weak mixing angle"), "0.22215", QString(), tr("Atomic & Nuclear") );

  // http://en.wikipedia.org/wiki/Physical_constant#Table_of_physico-chemical_constants
  constantList += Constant( tr("atomic mass unit"), "1.66053886e-27", "kg", tr("Physico-chemical") );
  constantList += Constant( tr("Avogadro's number"), "6.0221415e23", QString(), tr("Physico-chemical") );
  constantList += Constant( tr("Boltzmann constant"), "1.3806505e-23", "J/K", tr("Physico-chemical") );
  constantList += Constant( tr("Faraday constant"), "96485.3383", "C/mol", tr("Physico-chemical") );
  constantList += Constant( tr("first radiation constant"), "3.74177138e-16", "Wm^2", tr("Physico-chemical") );
  constantList += Constant( tr("Loschmidt constant"), "2.6867773e25", "/m^3", tr("Physico-chemical") );
  constantList += Constant( tr("gas constant"), "8.314472", " J/Kmol", tr("Physico-chemical") );
  constantList += Constant( tr("molar Planck constant"), "3.990312716e-10", "Js/mol-1", tr("Physico-chemical") );
  constantList += Constant( tr("second radiation constant"), "1.4387752e-2", "mK", tr("Physico-chemical") );
  constantList += Constant( tr("Stefan-Boltzmann constant"), "5.670400e-8", "W/m^2K^4", tr("Physico-chemical") );

  // http://www.astronomynotes.com/tables/tablesa.htm
  constantList += Constant( tr("astronomical unit"), "149597870.691", "km", tr("Astronomy") );
  constantList += Constant( tr("light year"), "9.460536207e12", "km", tr("Astronomy") );
  constantList += Constant( tr("parsec"), "3.08567802e13", "km", tr("Astronomy") );
  constantList += Constant( tr("sidereal year"), "365.2564", "days", tr("Astronomy") );
  constantList += Constant( tr("tropical year"), "365.2422", "days", tr("Astronomy") );
  constantList += Constant( tr("Gregorian year"), "365.2425", "days", tr("Astronomy") );
  constantList += Constant( tr("Earth mass"), "5.9736e24", "kg", tr("Astronomy") );
  constantList += Constant( tr("Sun mass"), "1.9891e30", "kg", tr("Astronomy") );
  constantList += Constant( tr("mean Earth radius"), "6371", "km", tr("Astronomy") );
  constantList += Constant( tr("Sun radius"), "6.96265e5", "km", tr("Astronomy") );
  constantList += Constant( tr("Sun luminosity"), "3.827e26", "W", tr("Astronomy") );

  for( int k = 0; k < constantList.count(); k++ )
  {
    QStringList cats = constantList[k].categories;
    for( int i = 0; i < cats.count(); i++ )
      if( !categoryList.contains( cats[i] ) )
        categoryList += cats[i];
  }
  categoryList.sort();
}

Constants* Constants::self()
{
  if( !s_global_constants )
  {
    s_global_constants = new Constants();
    qAddPostRoutine(deleteGlobalConstants);
  }
  return s_global_constants;
}

