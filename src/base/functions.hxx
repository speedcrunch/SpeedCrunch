// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2008 Helder Correia <helder.pereira.correia@gmail.com>
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


#ifndef FUNCTIONS_HXX
#define FUNCTIONS_HXX


#include "math/hmath.hxx"

#include <QStringList>
#include <QVector>


class Evaluator;
class Function;

class FunctionPrivate;
class FunctionsPrivate;


typedef HNumber (*FunctionPtr)( const Evaluator *, Function *, const QVector<HNumber> & );


class Function
{
  public:
    Function( const QString & name, int argc, FunctionPtr ptr, const QString & desc );
    Function( const QString & name, FunctionPtr ptr, const QString & desc );
    ~Function();
    QString description() const;
    QString error() const;
    HNumber exec( const Evaluator *, const QVector<HNumber> & args );
    QString name() const;
    void    setError( const QString & context, const QString & error );

  private:
    FunctionPrivate * d;
    Function( const Function & );
    Function & operator=( const Function & );
};


class Functions
{
  public:
    Functions();
    ~Functions();

    void        add( Function * );
    Function *  function( const QString & );
    QStringList functionNames() const;

    static Functions * self();

  //public slots:
  //  setAngleMode

  private:
    FunctionsPrivate * d;
    static Functions * s_self;
    Functions( const Functions & );
    Functions & operator=( const Functions & );
};


#endif
