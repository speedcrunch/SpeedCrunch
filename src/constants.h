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

#ifndef CONSTANTS
#define CONSTANTS

#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>

class Constant
{
public:
  QString name;
  QString value;
  QString unit;
  QStringList categories;

  Constant( const QString& n, const QString& v, const QString& u, const QStringList& cat ):
    name( n ), value( v ), unit( u ), categories( cat ) {}

  Constant( const QString& n, const QString& v, const QString& u, const QString& cat ):
    name( n ), value( v ), unit( u ) { categories << cat; }

  Constant( const QString& n, const QString& v, const QString& u, const QString& cat1,   const QString& cat2):
    name( n ), value( v ), unit( u ) {  categories << cat1; categories << cat2; }

};

// Dummy placeholder to hold all constants

class Constants : public QObject
{
  Q_OBJECT

  public:
    static Constants* self();

    QList<Constant> constantList;
    QStringList categoryList;

  private:
    explicit Constants();
    Constants( const Constants& );
    Constants& operator= ( const Constants& );
};

#endif // CONSTANTS
