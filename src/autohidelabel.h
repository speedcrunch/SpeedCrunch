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

#ifndef AUTOHIDELABEL
#define AUTOHIDELABEL

#include <QLabel>

class AutoHideLabelPrivate;

class AutoHideLabel: public QLabel
{
  Q_OBJECT

public:
  AutoHideLabel( QWidget* parent );
  ~AutoHideLabel();

public slots:
  void showText( const QString& msg );
  void hideText();

private slots:
  void fade();

private:
  AutoHideLabelPrivate* d;
  AutoHideLabel( const AutoHideLabel& );
  AutoHideLabel& operator=( const AutoHideLabel& );
};

#endif // AUTOHIDELABEL

