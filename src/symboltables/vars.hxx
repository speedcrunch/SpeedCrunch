/* This file is part of the SpeedCrunch project
   Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2007 Wolf Lammen ookami1 <at> gmx <dot> de

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

#ifndef _VARS_H
#define _VARS_H

class Variable
{
  public:
    QString name;
    HNumber value;
};

class VarTablePrivate;

class VarTable
{
  public:
    typedef enum {vtPredef, vtGlobal, vtAnyScope} t_scope;

    static VarTable* getTable(t_scope scope);

    HNumber get( const QString& id );
    void set( const QString& id, HNumber value );

    bool has( const QString& id );
    QVector<Variable> variables() const;
    void clearVariables();
    void remove( const QString& id );

  private:
    VarTablePrivate* d;
};

#endif /* _VARS_H */