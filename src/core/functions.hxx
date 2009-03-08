// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2008-2009 Helder Correia <helder.pereira.correia@gmail.com>
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

#include <QtCore/QStringList>
#include <QtCore/QVector>

#include <memory>

class Function;
class Functions;
class HNumber;

typedef HNumber (*FunctionPtr)( Function *, const QVector<HNumber> & );

class Function : public QObject
{
    Q_OBJECT

public:
    Function( const QString & name, int argc, FunctionPtr ptr, QObject * parent = 0 );
    Function( const QString & name, FunctionPtr ptr, QObject * parent = 0  );
    ~Function();

    QString description() const;
    QString error() const;
    HNumber exec( const QVector<HNumber> & args );
    Functions * functions() const;
    QString name() const;
    void setError( const QString & context, const QString & error );
    void setTitle( const QString & );

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
    static Functions * instance();
    ~Functions();

    void add( Function * );
    Function * function( const QString & ) const;
    QStringList names() const;

public slots:
    void retranslateText();

private:
    struct Private;
    const std::auto_ptr<Private> d;

    Functions();
    Functions( const Functions & );
    Functions & operator=( const Functions & );
};

#endif

