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

#ifndef TIPWIDGET
#define TIPWIDGET

#include <QFrame>

class TipWidgetPrivate;

class TipWidget: public QFrame
{
  Q_OBJECT

public:
  explicit TipWidget( QWidget* parent );
  ~TipWidget();

  QSize sizeHint() const;

public slots:
  void showText( const QString& msg );
  void hideText();

private slots:
  void appear();
  void disappear();
  void animateFade( int );

private:
  TipWidgetPrivate* d;
  TipWidget( const TipWidget& );
  TipWidget& operator=( const TipWidget& );
};

#endif // TIPWIDGET

