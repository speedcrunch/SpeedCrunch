/* This file is part of the SpeedCrunch project
   Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2007 Helder Correia <helder.pereira.correia@gmail.com>

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

#include <constants.hxx>

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
  constantList += Constant( tr("Characteristic Impedance of Vacuum"), "376.730313461",   "ohm",          tr("General Physics") );
  constantList += Constant( tr("Electric Constant"                 ), "8.854187817e-12", "F/m",          tr("General Physics") );
  constantList += Constant( tr("Magnetic Constant"                 ), "1.256637061e-6",  "N/A^2",        tr("General Physics") );
  constantList += Constant( tr("Gravitation Constant"              ), "6.67428e-11",     "m^3/(kg s^2)", tr("General Physics") );
  constantList += Constant( tr("Planck's Constant"                 ), "6.62606896e-34",  "J s",          tr("General Physics") );
  constantList += Constant( tr("Dirac's Constant"                  ), "1.054571628e-34", "J s",          tr("General Physics") );
  constantList += Constant( tr("Speed of Light in Vacuum"          ), "299792458",       "m/s",          tr("General Physics") );

  // http://en.wikipedia.org/wiki/Physical_constant#Table_of_electromagnetic_constants
  constantList += Constant( tr("Bohr-Procopiu Magneton"), "927.400949e-26", "J/T",       tr("Electromagnetic") );
  constantList += Constant( tr("Conductance Quantum"   ), "7.748091733e-5", "S",         tr("Electromagnetic") );
  constantList += Constant( tr("Coulomb's Constant"    ), "8.987742438e9",  "N m^2/C^2", tr("Electromagnetic") );
  constantList += Constant( tr("Elementary Charge"     ), "1.60217653e-19", "C",         tr("Electromagnetic") );
  constantList += Constant( tr("Josephson Constant"    ), "483597.879e9",   "Hz/V",      tr("Electromagnetic") );
  constantList += Constant( tr("Magnetic Flux Quantum" ), "2.06783372e-15", "Wb",        tr("Electromagnetic") );
  constantList += Constant( tr("Nuclear Magneton"      ), "5.05078343e-27", "J/T",       tr("Electromagnetic") );
  constantList += Constant( tr("Resistance Quantum"    ), "12906.403725",   "ohm",       tr("Electromagnetic") );
  constantList += Constant( tr("von Klitzing Constant" ), "25812.807449",   "ohm",       tr("Electromagnetic") );

  // http://en.wikipedia.org/wiki/Physical_constant#Table_of_atomic_and_nuclear_constants
  constantList += Constant( tr("Bohr Radius"            ), "0.5291772108e-10", "m",       tr("Atomic & Nuclear") );
  constantList += Constant( tr("Fermi Coupling Constant"), "1.16639e-5",       "Ge/V^2",  tr("Atomic & Nuclear") );
  constantList += Constant( tr("Fine-structure Constant"), "7.297352568e-3",   QString(), tr("Atomic & Nuclear") );
  constantList += Constant( tr("Hartree Energy"         ), "4.35974417e-18",   "J",       tr("Atomic & Nuclear") );
  constantList += Constant( tr("Quantum of Circulation" ), "3.636947550e-4",   "m^2/s",   tr("Atomic & Nuclear") );
  constantList += Constant( tr("Rydberg Constant"       ), "10973731.568525",  "m^(-1)",  tr("Atomic & Nuclear") );
  constantList += Constant( tr("Thomson Cross Section"  ), "0.665245873e-28",  "m^2",     tr("Atomic & Nuclear") );
  constantList += Constant( tr("Weak Mixing Angle"      ), "0.22215",          QString(), tr("Atomic & Nuclear") );

  // http://en.wikipedia.org/wiki/Physical_constant#Table_of_physico-chemical_constants
  constantList += Constant( tr("Atomic Mass Unit"         ), "1.66053886e-27",  "kg",          tr("Physico-chemical") );
  constantList += Constant( tr("Avogadro's Number"        ), "6.0221415e23",    QString(),     tr("Physico-chemical") );
  constantList += Constant( tr("Boltzmann Constant"       ), "1.3806505e-23",   "J/K",         tr("Physico-chemical") );
  constantList += Constant( tr("Faraday Constant"         ), "96485.3383",      "C/mol",       tr("Physico-chemical") );
  constantList += Constant( tr("First Radiation Constant" ), "3.74177138e-16",  "W m^2",       tr("Physico-chemical") );
  constantList += Constant( tr("Loschmidt Constant"       ), "2.6867773e25",    "m^(-3)",      tr("Physico-chemical") );
  constantList += Constant( tr("Gas Constant"             ), "8.314472",        "J/(K mol)",   tr("Physico-chemical") );
  constantList += Constant( tr("Molar Planck Constant"    ), "3.990312716e-10", "J s/mol",     tr("Physico-chemical") );
  constantList += Constant( tr("Second Radiation Constant"), "1.4387752e-2",    "m K",         tr("Physico-chemical") );
  constantList += Constant( tr("Stefan-Boltzmann Constant"), "5.670400e-8",     "W/(m^2 K^4)", tr("Physico-chemical") );

  // http://www.astronomynotes.com/tables/tablesa.htm
  constantList += Constant( tr("Astronomical Unit"), "149597870691",       "m",        tr("Astronomy") );
  constantList += Constant( tr("Light Year"       ), "9.4607304725808e15", "m",        tr("Astronomy") );
  constantList += Constant( tr("Parsec"           ), "3.08567802e16",      "m",        tr("Astronomy") );
  constantList += Constant( tr("Sidereal Year"    ), "365.2564",           tr("days"), tr("Astronomy") );
  constantList += Constant( tr("Tropical Year"    ), "365.2422",           tr("days"), tr("Astronomy") );
  constantList += Constant( tr("Gregorian Year"   ), "365.2425",           tr("days"), tr("Astronomy") );
  constantList += Constant( tr("Earth Mass"       ), "5.9736e24",          "kg",       tr("Astronomy") );
  constantList += Constant( tr("Sun Mass"         ), "1.9891e30",          "kg",       tr("Astronomy") );
  constantList += Constant( tr("Mean Earth Radius"), "6371000",            "m",        tr("Astronomy") );
  constantList += Constant( tr("Sun Radius"       ), "6.96265e8",          "m",        tr("Astronomy") );
  constantList += Constant( tr("Sun Luminosity"   ), "3.827e26",           "W",        tr("Astronomy") );

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
