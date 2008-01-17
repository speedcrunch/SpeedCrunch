// This file is part of the SpeedCrunch project
// Copyright (C) 2005-2006 Johan Thelin <e8johan@gmail.com>
// Copyright (C) 2007-2008 Helder Correia <helder.pereira.correia@gmail.com>
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


#ifndef KEYPAD_HXX
#define KEYPAD_HXX


#include <QWidget>


class KeypadPrivate;


class Keypad : public QWidget
{
  Q_OBJECT

  public:
    explicit Keypad( QWidget * parent );
    ~Keypad();

  signals:
    void addText( const QString & );
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
    void clickedACos();
    void clickedAdd();
    void clickedAns();
    void clickedASin();
    void clickedATan();
    void clickedAvg();
    void clickedCos();
    void clickedDiv();
    void clickedDot();
    void clickedEE();
    void clickedExp();
    void clickedFact();
    void clickedLn();
    void clickedLParen();
    void clickedMul();
    void clickedPerc();
    void clickedPi();
    void clickedRaise();
    void clickedRParen();
    void clickedSemic();
    void clickedSin();
    void clickedSqrt();
    void clickedSub();
    void clickedTan();
    void clickedX();
    void clickedXEq();

  private:
    void connectButtons();
    void createButtons();
    void deactivateButtonFocus();
    void layoutButtons();
    void polishButtons();
    void sizeButtons();

    KeypadPrivate * d;
    Keypad( const Keypad & );
    Keypad & operator=( const Keypad & );
};


#endif
