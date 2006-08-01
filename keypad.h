/* This file is part of the SpeedCrunch project
   Copyright (C) 2005-2006 Johan Thelin <e8johan@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef KEYPAD
#define KEYPAD

#include <QWidget>

class KeyPad : public QWidget
{
  Q_OBJECT

  public:
    KeyPad( QWidget* parent );
    ~KeyPad();

  protected slots:
    void clicked0();
    void clicked1();
    void clicked2();
    void clicked3();
    void clicked4();
    void clicked5();
    void clicked6();
    void clicked7();
    void clicked8();
    void clicked9();
    void clickedDot();
    void clickedEq();
    void clickedDiv();
    void clickedAns();
    void clickedLn();
    void clickedLog();
    void clickedExp();
    void clickedMul();
    void clickedX();
    void clickedSinh();
    void clickedCosh();
    void clickedTanh();
    void clickedSub();
    void clickedPi();
    void clickedSin();
    void clickedCos();
    void clickedTan();
    void clickedAdd();
    void clickedLParen();
    void clickedRParen();
    void clickedSqrt();
    void clickedBackspace();

  signals:
    void addText( const QString& );
    void evalFunction( const QString& );
    void evalLine( const QString& );
    void clearText();
    void clearAll();

  private:
    class Private;
    Private*d;
};

#endif // KEYPAD
