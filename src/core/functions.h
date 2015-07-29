// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2008-2009, 2013 Helder Correia <helder.pereira.correia@gmail.com>
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

#ifndef CORE_FUNCTION_H
#define CORE_FUNCTION_H

#include "core/errors.h"
#include "math/hmath.h"

#include <QHash>
#include <QObject>
#include <QStringList>
#include <QVector>

class Function;

class Function : public QObject {
    Q_OBJECT
public:
    typedef QVector<HNumber> ArgumentList;
    typedef HNumber (*FunctionImpl)(Function*, const ArgumentList&);

    Function(const QString& identifier, FunctionImpl ptr, QObject* parent = 0)
        : QObject(parent)
        , m_identifier(identifier)
        , m_ptr(ptr)
    { }

    const QString& identifier() const { return m_identifier; }
    const QString& name() const { return m_name; }
    const QString& usage() const { return m_usage; }
    Error error() const { return m_error; }
    HNumber exec(const ArgumentList&);

    void setName(const QString& name) { m_name = name; }
    void setUsage(const QString& usage) { m_usage = usage; }
    void setError(Error error) { m_error = error; }

private:
    Q_DISABLE_COPY(Function)
    Function();

    QString m_identifier;
    QString m_name;
    QString m_usage;
    Error m_error;
    FunctionImpl m_ptr;
};

class FunctionRepo : public QObject {
    Q_OBJECT
public:
    static FunctionRepo* instance();

    void insert(Function*);
    QStringList getIdentifiers() const;
    Function* find(const QString& identifier) const;

public slots:
    void retranslateText();

private:
    Q_DISABLE_COPY(FunctionRepo)
    FunctionRepo();

    void createFunctions();
    void setFunctionNames();
    void setNonTranslatableFunctionUsages();
    void setTranslatableFunctionUsages();

    QHash<QString, Function*> m_functions;
};

#endif // CORE_FUNCTION_H
