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

  // http://en.wikipedia.org/wiki/Physical_constant#Table_of_atomic_and_nuclear_constants
  constantList += Constant( tr("Bohr radius"), "0.5291772108e-10", "m", tr("Atomic & Nuclear") );
  constantList += Constant( tr("Fermi coupling constant"), "1.16639e-5", "Ge/V^2", tr("Atomic & Nuclear") );
  constantList += Constant( tr("fine-structure constant"), "7.297352568e-3", QString(), tr("Atomic & Nuclear") );
  constantList += Constant( tr("Hartree energy"), "4.35974417e-18", "J", tr("Atomic & Nuclear") );
  constantList += Constant( tr("quantum of circulation"), "3.636947550e-4", "m^2/s", tr("Atomic & Nuclear") );
  constantList += Constant( tr("Rydberg constant"), "10973731.568525", "/m", tr("Atomic & Nuclear") );
  constantList += Constant( tr("Thomson cross section"), "0.665245873e-28", "m^2", tr("Atomic & Nuclear") );
  constantList += Constant( tr("weak mixing angle"), "0.22215", QString(), tr("Atomic & Nuclear") );

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

