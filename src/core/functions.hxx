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

#ifndef CORE_FUNCTIONS_HXX
#define CORE_FUNCTIONS_HXX

#include "math/hmath.hxx"

#include <QObject>
#include <QStringList>
#include <QVector>

#include <memory>

class Function;
class Functions;

typedef HNumber (*FunctionPtr)( Function *, const QVector<HNumber> & );

class Function
{
  public:
    Function( const QString & name, int argc, FunctionPtr ptr,
              Functions * parent );
    Function( const QString & name, FunctionPtr ptr, Functions * parent );

    QString     description() const;
    void        setDescription( const QString & );
    QString     error() const;
    HNumber     exec( const QVector<HNumber> & args );
    QString     name() const;
    Functions * functions() const;
    void        setError( const QString & context, const QString & error );

  private:
    struct Private;
    const std::auto_ptr<Private> d;
    Function();
    Function( const Function & );
    Function & operator=( const Function & );
};

class Functions : public QObject
{
  Q_OBJECT

  public:
    Functions( QObject * parent = 0 );
    ~Functions();

    void        add( Function * );
    Function *  function( const QString & ) const;
    QStringList functionNames() const;

  public slots:
    void retranslateText();

  private:
    struct Private;
    const std::auto_ptr<Private> d;
    Functions( const Functions & );
    Functions & operator=( const Functions & );
};

#endif

