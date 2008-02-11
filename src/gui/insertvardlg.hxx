// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
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


#ifndef INSERTVARDLG_HXX
#define INSERTVARDLG_HXX


#include <QDialog>


class Evaluator;


class InsertVariableDlg : public QDialog
{
  Q_OBJECT

  public:
    InsertVariableDlg( Evaluator * eval, char radixChar, QWidget * parent = 0 );
    ~InsertVariableDlg();

    void    updateList();
    char    radixChar() const;
    QString variableName() const;

  public slots:
    void setRadixChar( char );

  private:
    struct Private;
    Private * const d;
    InsertVariableDlg();
    InsertVariableDlg( const InsertVariableDlg & );
    InsertVariableDlg & operator=( const InsertVariableDlg & );
};


#endif
