// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
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


#include <math/hmath.hxx>

#include <QStringList>
#include <QVector>


class Evaluator;
class Function;

class FunctionPrivate;
class FunctionRepositoryPrivate;


typedef QVector<HNumber> FunctionArguments;
typedef HNumber (*FunctionPtr)( const Evaluator *, Function *, const FunctionArguments & );


class Function
{
  public:
    Function( const QString & name, int argc, FunctionPtr ptr, const QString & desc );
    Function( const QString & name, FunctionPtr ptr, const QString & desc );
    QString description() const;
    QString error() const;
    HNumber exec( const Evaluator *, const FunctionArguments & args );
    QString name() const;
    void    setError( const QString & context, const QString & error );
    ~Function();

  private:
    FunctionPrivate * d;
    Function( const Function & );
    Function& operator=( const Function & );
};


class FunctionRepository
{
  public:
    FunctionRepository();
    static FunctionRepository * self();
    void        add( Function * function );
    Function *  function( const QString & name );
    QStringList functionNames() const;
    ~FunctionRepository();

  private:
    FunctionRepositoryPrivate * d;
    static FunctionRepository * s_self;
    FunctionRepository( const FunctionRepository & );
    FunctionRepository & operator=( const FunctionRepository & );
};


#endif
