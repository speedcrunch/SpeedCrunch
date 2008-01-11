/* This file is part of the SpeedCrunch project
   Copyright (C) 2005-2006 Johan Thelin <e8johan@gmail.com>
   Copyright (C) 2007-2008 Helder Correia <helder.pereira.correia@gmail.com>

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

#ifndef KEYPAD
#define KEYPAD

#include <QWidget>

class KeyPadPrivate;

class KeyPad : public QWidget
{
  Q_OBJECT

  public:
    explicit KeyPad( QWidget* parent );
    ~KeyPad();

  signals:
    void addText( const QString& );
    void evaluate();

  public slots:
    void settingsChanged();

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
    void clickedSqrt();
    void clickedRaise();
    void clickedPerc();
    void clickedFact();
    void clickedEE();
    void clickedLParen();
    void clickedSemic();
    void clickedRParen();
    void clickedAvg();
    void clickedAdd();
    void clickedSub();
    void clickedMul();
    void clickedDiv();
    void clickedPi();
    void clickedAns();
    void clickedX();
    void clickedXEq();
    void clickedExp();
    void clickedLn();
    void clickedSin();
    void clickedCos();
    void clickedTan();
    void clickedASin();
    void clickedACos();
    void clickedATan();

  private:
    void createButtons();
    void polishButtons();
    void sizeButtons();
    void dontFocusButtons();
    void layoutButtons();
    void connectButtons();

    KeyPadPrivate*d;
    KeyPad( const KeyPad& );
    KeyPad& operator=( const KeyPad& );
};

#endif // KEYPAD
