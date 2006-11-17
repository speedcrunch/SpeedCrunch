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

#include "keypad.h"

#include <QPushButton>
#include <QGridLayout>
#include <QStyle>
#include <QApplication>

class KeyPadPrivate
{
  public:
    QPushButton* key0;
    QPushButton* key1;
    QPushButton* key2;
    QPushButton* key3;
    QPushButton* key4;
    QPushButton* key5;
    QPushButton* key6;
    QPushButton* key7;
    QPushButton* key8;
    QPushButton* key9;
    QPushButton* keyDot;
    QPushButton* keyEq;

    QPushButton* keySqrt;
    QPushButton* keyBackspace;
    QPushButton* keyLParen;
    QPushButton* keyRParen;
    QPushButton* keyAdd;
    QPushButton* keySub;
    QPushButton* keyMul;
    QPushButton* keyDiv;

    QPushButton* keyPi;
    QPushButton* keyAns;
    QPushButton* keyX;
    QPushButton* keyXEq;

    QPushButton* keyExp;
    QPushButton* keyLog;
    QPushButton* keySin;
    QPushButton* keyCos;
    QPushButton* keyTan;
    QPushButton* keyASin;
    QPushButton* keyACos;
    QPushButton* keyATan;
};

void KeyPad::createButtons()
{
  d->key0 = new QPushButton( tr("0"), this );
  d->key1 = new QPushButton( tr("1"), this );
  d->key2 = new QPushButton( tr("2"), this );
  d->key3 = new QPushButton( tr("3"), this );
  d->key4 = new QPushButton( tr("4"), this );
  d->key5 = new QPushButton( tr("5"), this );
  d->key6 = new QPushButton( tr("6"), this );
  d->key7 = new QPushButton( tr("7"), this );
  d->key8 = new QPushButton( tr("8"), this );
  d->key9 = new QPushButton( tr("9"), this );
  d->keyDot = new QPushButton( tr("."), this );
  d->keyEq = new QPushButton( tr("="), this );

  d->keySqrt = new QPushButton( this );
  d->keySqrt->setIcon( QPixmap( ":/math_sqrt.png" ) );
  d->keyBackspace = new QPushButton( this );
  d->keyBackspace->setIcon( QPixmap( ":/back.png" ) );
  d->keyLParen = new QPushButton( tr("("), this );
  d->keyRParen = new QPushButton( tr(")"), this );
  d->keyAdd = new QPushButton( tr("+"), this );
  d->keySub = new QPushButton( tr("-"), this );
  d->keyMul = new QPushButton( tr("*"), this );
  d->keyDiv = new QPushButton( tr("/"), this );

  d->keyPi = new QPushButton( tr("pi"), this );
  d->keyAns = new QPushButton( tr("ans"), this );
  d->keyX = new QPushButton( tr("x"), this );
  d->keyXEq = new QPushButton( tr("x="), this );

  d->keyExp = new QPushButton( tr("exp"), this );
  d->keyLog = new QPushButton( tr("log"), this );
  d->keySin = new QPushButton( tr("sin"), this );
  d->keyCos = new QPushButton( tr("cos"), this );
  d->keyTan = new QPushButton( tr("tan"), this );
  d->keyASin = new QPushButton( tr("asin"), this );
  d->keyACos = new QPushButton( tr("acos"), this );
  d->keyATan = new QPushButton( tr("atan"), this );
}

void KeyPad::polishButtons()
{
  d->key0->ensurePolished();
  d->key1->ensurePolished();
  d->key2->ensurePolished();
  d->key3->ensurePolished();
  d->key4->ensurePolished();
  d->key5->ensurePolished();
  d->key6->ensurePolished();
  d->key7->ensurePolished();
  d->key8->ensurePolished();
  d->key9->ensurePolished();
  d->keyDot->ensurePolished();
  d->keyEq->ensurePolished();

  d->keySqrt->ensurePolished();
  d->keyBackspace->ensurePolished();
  d->keyLParen->ensurePolished();
  d->keyRParen->ensurePolished();
  d->keyAdd->ensurePolished();
  d->keySub->ensurePolished();
  d->keyMul->ensurePolished();
  d->keyDiv->ensurePolished();

  d->keyPi->ensurePolished();
  d->keyAns->ensurePolished();
  d->keyX->ensurePolished();
  d->keyXEq->ensurePolished();

  d->keyExp->ensurePolished();
  d->keyLog->ensurePolished();
  d->keySin->ensurePolished();
  d->keyCos->ensurePolished();
  d->keyTan->ensurePolished();
  d->keyASin->ensurePolished();
  d->keyACos->ensurePolished();
  d->keyATan->ensurePolished();
}

void KeyPad::sizeButtons()
{
  // The same font in all buttons, so just pick one
  QFontMetrics fm = d->key0->fontMetrics();

  int maxWidth = 0;
  int textHeight = QMAX(fm.lineSpacing(), 14);
//  int marginSize = d->key0->frameWidth() * 2;

  // Find the widest text
  if( fm.width( d->key0->text() ) > maxWidth ) maxWidth = fm.width( d->key0->text() );
  if( fm.width( d->key1->text() ) > maxWidth ) maxWidth = fm.width( d->key1->text() );
  if( fm.width( d->key2->text() ) > maxWidth ) maxWidth = fm.width( d->key2->text() );
  if( fm.width( d->key3->text() ) > maxWidth ) maxWidth = fm.width( d->key3->text() );
  if( fm.width( d->key4->text() ) > maxWidth ) maxWidth = fm.width( d->key4->text() );
  if( fm.width( d->key5->text() ) > maxWidth ) maxWidth = fm.width( d->key5->text() );
  if( fm.width( d->key6->text() ) > maxWidth ) maxWidth = fm.width( d->key6->text() );
  if( fm.width( d->key7->text() ) > maxWidth ) maxWidth = fm.width( d->key7->text() );
  if( fm.width( d->key8->text() ) > maxWidth ) maxWidth = fm.width( d->key8->text() );
  if( fm.width( d->key9->text() ) > maxWidth ) maxWidth = fm.width( d->key9->text() );
  if( fm.width( d->keyDot->text() ) > maxWidth ) maxWidth = fm.width( d->keyDot->text() );
  if( fm.width( d->keyEq->text() ) > maxWidth ) maxWidth = fm.width( d->keyEq->text() );

  if( fm.width( d->keySqrt->text() ) > maxWidth ) maxWidth = fm.width( d->keySqrt->text() );
  if( fm.width( d->keyBackspace->text() ) > maxWidth ) maxWidth = fm.width( d->keyBackspace->text() );
  if( fm.width( d->keyLParen->text() ) > maxWidth ) maxWidth = fm.width( d->keyLParen->text() );
  if( fm.width( d->keyRParen->text() ) > maxWidth ) maxWidth = fm.width( d->keyRParen->text() );
  if( fm.width( d->keyAdd->text() ) > maxWidth ) maxWidth = fm.width( d->keyAdd->text() );
  if( fm.width( d->keySub->text() ) > maxWidth ) maxWidth = fm.width( d->keySub->text() );
  if( fm.width( d->keyMul->text() ) > maxWidth ) maxWidth = fm.width( d->keyMul->text() );
  if( fm.width( d->keyDiv->text() ) > maxWidth ) maxWidth = fm.width( d->keyDiv->text() );

  if( fm.width( d->keyPi->text() ) > maxWidth ) maxWidth = fm.width( d->keyPi->text() );
  if( fm.width( d->keyAns->text() ) > maxWidth ) maxWidth = fm.width( d->keyAns->text() );
  if( fm.width( d->keyX->text() ) > maxWidth ) maxWidth = fm.width( d->keyX->text() );
  if( fm.width( d->keyXEq->text() ) > maxWidth ) maxWidth = fm.width( d->keyXEq->text() );
		
  if( fm.width( d->keyExp->text() ) > maxWidth ) maxWidth = fm.width( d->keyExp->text() );
  if( fm.width( d->keyLog->text() ) > maxWidth ) maxWidth = fm.width( d->keyLog->text() );
  if( fm.width( d->keySin->text() ) > maxWidth ) maxWidth = fm.width( d->keySin->text() );
  if( fm.width( d->keyCos->text() ) > maxWidth ) maxWidth = fm.width( d->keyCos->text() );
  if( fm.width( d->keyTan->text() ) > maxWidth ) maxWidth = fm.width( d->keyTan->text() );
  if( fm.width( d->keyASin->text() ) > maxWidth ) maxWidth = fm.width( d->keyASin->text() );
  if( fm.width( d->keyACos->text() ) > maxWidth ) maxWidth = fm.width( d->keyACos->text() );
  if( fm.width( d->keyATan->text() ) > maxWidth ) maxWidth = fm.width( d->keyATan->text() );

  // Limit the size of the buttons
  
  d->key0->setMaximumSize(
  d->key0->style()->sizeFromContents( QStyle::CT_ToolButton,
                                      0,
                                      QSize( int(maxWidth*1.5), int(textHeight*1.5) ).expandedTo(QApplication::globalStrut()), d->key0 ) );
  d->key1->setMaximumSize( d->key0->maximumSize() );
  d->key2->setMaximumSize( d->key0->maximumSize() );
  d->key3->setMaximumSize( d->key0->maximumSize() );
  d->key4->setMaximumSize( d->key0->maximumSize() );
  d->key5->setMaximumSize( d->key0->maximumSize() );
  d->key6->setMaximumSize( d->key0->maximumSize() );
  d->key7->setMaximumSize( d->key0->maximumSize() );
  d->key8->setMaximumSize( d->key0->maximumSize() );
  d->key9->setMaximumSize( d->key0->maximumSize() );
  d->keyDot->setMaximumSize( d->key0->maximumSize() );
  d->keyEq->setMaximumSize( d->key0->maximumSize() );

  d->keySqrt->setMaximumSize( d->key0->maximumSize() );
  d->keyBackspace->setMaximumSize( d->key0->maximumSize() );
  d->keyLParen->setMaximumSize( d->key0->maximumSize() );
  d->keyRParen->setMaximumSize( d->key0->maximumSize() );
  d->keyAdd->setMaximumSize( d->key0->maximumSize() );
  d->keySub->setMaximumSize( d->key0->maximumSize() );
  d->keyMul->setMaximumSize( d->key0->maximumSize() );
  d->keyDiv->setMaximumSize( d->key0->maximumSize() );

  d->keyPi->setMaximumSize( d->key0->maximumSize() );
  d->keyAns->setMaximumSize( d->key0->maximumSize() );
  d->keyX->setMaximumSize( d->key0->maximumSize() );
  d->keyXEq->setMaximumSize( d->key0->maximumSize() );
		
  d->keyExp->setMaximumSize( d->key0->maximumSize() );
  d->keyLog->setMaximumSize( d->key0->maximumSize() );
  d->keySin->setMaximumSize( d->key0->maximumSize() );
  d->keyCos->setMaximumSize( d->key0->maximumSize() );
  d->keyTan->setMaximumSize( d->key0->maximumSize() );
  d->keyASin->setMaximumSize( d->key0->maximumSize() );
  d->keyACos->setMaximumSize( d->key0->maximumSize() );
  d->keyATan->setMaximumSize( d->key0->maximumSize() );
}

void KeyPad::dontFocusButtons()
{
  d->key0->setFocusPolicy( Qt::NoFocus );
  d->key1->setFocusPolicy( Qt::NoFocus );
  d->key2->setFocusPolicy( Qt::NoFocus );
  d->key3->setFocusPolicy( Qt::NoFocus );
  d->key4->setFocusPolicy( Qt::NoFocus );
  d->key5->setFocusPolicy( Qt::NoFocus );
  d->key6->setFocusPolicy( Qt::NoFocus );
  d->key7->setFocusPolicy( Qt::NoFocus );
  d->key8->setFocusPolicy( Qt::NoFocus );
  d->key9->setFocusPolicy( Qt::NoFocus );
  d->keyDot->setFocusPolicy( Qt::NoFocus );
  d->keyEq->setFocusPolicy( Qt::NoFocus );

  d->keySqrt->setFocusPolicy( Qt::NoFocus );
  d->keyBackspace->setFocusPolicy( Qt::NoFocus );
  d->keyLParen->setFocusPolicy( Qt::NoFocus );
  d->keyRParen->setFocusPolicy( Qt::NoFocus );
  d->keyAdd->setFocusPolicy( Qt::NoFocus );
  d->keySub->setFocusPolicy( Qt::NoFocus );
  d->keyMul->setFocusPolicy( Qt::NoFocus );
  d->keyDiv->setFocusPolicy( Qt::NoFocus );

  d->keyPi->setFocusPolicy( Qt::NoFocus );
  d->keyAns->setFocusPolicy( Qt::NoFocus );
  d->keyX->setFocusPolicy( Qt::NoFocus );
  d->keyXEq->setFocusPolicy( Qt::NoFocus );

  d->keyExp->setFocusPolicy( Qt::NoFocus );
  d->keyLog->setFocusPolicy( Qt::NoFocus );
  d->keySin->setFocusPolicy( Qt::NoFocus );
  d->keyCos->setFocusPolicy( Qt::NoFocus );
  d->keyTan->setFocusPolicy( Qt::NoFocus );
  d->keyASin->setFocusPolicy( Qt::NoFocus );
  d->keyACos->setFocusPolicy( Qt::NoFocus );
  d->keyATan->setFocusPolicy( Qt::NoFocus );
}

void KeyPad::layoutButtons()
{

  QGridLayout* layout = new QGridLayout( this );
  layout->setMargin( 5 );
  layout->setSpacing( 5 );

  /*    7    8    9        sqrt  <--         pi        exp  log  */
  /*    4    5    6         (      )         ans       sin  asin */
  /*    1    2    3         +      -         x         cos  acos */
  /*    0    .    =         *      /         x=        tan  atan */

  layout->addWidget( d->key0, 3, 0 );
  layout->addWidget( d->key1, 2, 0 );
  layout->addWidget( d->key2, 2, 1 );
  layout->addWidget( d->key3, 2, 2 );
  layout->addWidget( d->key4, 1, 0 );
  layout->addWidget( d->key5, 1, 1 );
  layout->addWidget( d->key6, 1, 2 );
  layout->addWidget( d->key7, 0, 0 );
  layout->addWidget( d->key8, 0, 1 );
  layout->addWidget( d->key9, 0, 2 );
  layout->addWidget( d->keyDot, 3, 1 );
  layout->addWidget( d->keyEq, 3, 2 );

  layout->setColumnMinimumWidth( 3, 10 );

  layout->addWidget( d->keySqrt, 0, 4 );
  layout->addWidget( d->keyBackspace, 0, 5 );
  layout->addWidget( d->keyLParen, 1, 4 );
  layout->addWidget( d->keyRParen, 1, 5 );
  layout->addWidget( d->keyAdd, 2, 4 );
  layout->addWidget( d->keySub, 2, 5 );
  layout->addWidget( d->keyMul, 3, 4 );
  layout->addWidget( d->keyDiv, 3, 5 );

  layout->setColumnMinimumWidth( 6, 10 );

  layout->addWidget( d->keyPi, 0, 7 );
  layout->addWidget( d->keyAns, 1, 7 );
  layout->addWidget( d->keyX, 2, 7 );
  layout->addWidget( d->keyXEq, 3, 7 );

  layout->setColumnMinimumWidth( 8, 10 );

  layout->addWidget( d->keyExp, 0, 9 );
  layout->addWidget( d->keyLog, 0, 10 );
  layout->addWidget( d->keySin, 1, 9 );
  layout->addWidget( d->keyCos, 2, 9 );
  layout->addWidget( d->keyTan, 3, 9 );
  layout->addWidget( d->keyASin, 1, 10 );
  layout->addWidget( d->keyACos, 2, 10 );
  layout->addWidget( d->keyATan, 3, 10 );
}

void KeyPad::connectButtons()
{
  connect( d->key0, SIGNAL(clicked()), SLOT(clicked0()) );
  connect( d->key1, SIGNAL(clicked()), SLOT(clicked1()) );
  connect( d->key2, SIGNAL(clicked()), SLOT(clicked2()) );
  connect( d->key3, SIGNAL(clicked()), SLOT(clicked3()) );
  connect( d->key4, SIGNAL(clicked()), SLOT(clicked4()) );
  connect( d->key5, SIGNAL(clicked()), SLOT(clicked5()) );
  connect( d->key6, SIGNAL(clicked()), SLOT(clicked6()) );
  connect( d->key7, SIGNAL(clicked()), SLOT(clicked7()) );
  connect( d->key8, SIGNAL(clicked()), SLOT(clicked8()) );
  connect( d->key9, SIGNAL(clicked()), SLOT(clicked9()) );
  connect( d->keyDot, SIGNAL(clicked()), SLOT(clickedDot()) );
  connect( d->keyEq, SIGNAL(clicked()), SLOT(clickedEq()) );

  connect( d->keySqrt, SIGNAL(clicked()), SLOT(clickedSqrt()) );
  connect( d->keyBackspace, SIGNAL(clicked()), SLOT(clickedBackspace()) );
  connect( d->keyLParen, SIGNAL(clicked()), SLOT(clickedLParen()) );
  connect( d->keyRParen, SIGNAL(clicked()), SLOT(clickedRParen()) );
  connect( d->keyAdd, SIGNAL(clicked()), SLOT(clickedAdd()) );
  connect( d->keySub, SIGNAL(clicked()), SLOT(clickedSub()) );
  connect( d->keyMul, SIGNAL(clicked()), SLOT(clickedMul()) );
  connect( d->keyDiv, SIGNAL(clicked()), SLOT(clickedDiv()) );

  connect( d->keyPi, SIGNAL(clicked()), SLOT(clickedPi()) );
  connect( d->keyAns, SIGNAL(clicked()), SLOT(clickedAns()) );
  connect( d->keyX, SIGNAL(clicked()), SLOT(clickedX()) );
  connect( d->keyXEq, SIGNAL(clicked()), SLOT(clickedXEq()) );

  connect( d->keyExp, SIGNAL(clicked()), SLOT(clickedExp()) );
  connect( d->keyLog, SIGNAL(clicked()), SLOT(clickedLog()) );
  connect( d->keySin, SIGNAL(clicked()), SLOT(clickedSin()) );
  connect( d->keyCos, SIGNAL(clicked()), SLOT(clickedCos()) );
  connect( d->keyTan, SIGNAL(clicked()), SLOT(clickedTan()) );
  connect( d->keyASin, SIGNAL(clicked()), SLOT(clickedASin()) );
  connect( d->keyACos, SIGNAL(clicked()), SLOT(clickedACos()) );
  connect( d->keyATan, SIGNAL(clicked()), SLOT(clickedATan()) );
}

KeyPad::KeyPad( QWidget* parent ) : QWidget( parent )
{
  d = new KeyPadPrivate;

  // Setup buttons

  createButtons();
  polishButtons();
  sizeButtons();
  dontFocusButtons();
  layoutButtons();
  connectButtons();

  // Add tool tips

  d->keyEq->setToolTip( tr("Evaluate the expression") );
  d->keyAns->setToolTip( tr("The result from the last expression") );
  d->keyLog->setToolTip( tr("Logarithm") );
  d->keyExp->setToolTip( tr("Exponential") );
  d->keyX->setToolTip( tr("The variable X") );
  d->keyASin->setToolTip( tr("Inverse sine") );
  d->keyACos->setToolTip( tr("Inverse cosine") );
  d->keyATan->setToolTip( tr("Inverse tangens") );
  d->keyPi->setToolTip( tr("The number pi") );
  d->keySin->setToolTip( tr("Sine") );
  d->keyCos->setToolTip( tr("Cosine") );
  d->keyTan->setToolTip( tr("Tangens") );
  d->keySqrt->setToolTip( tr("Square root") );
  d->keyBackspace->setToolTip( tr("Backspace") );
}

KeyPad::~KeyPad()
{
  delete d;
}

void KeyPad::clicked0() { emit addText( "0" ); }
void KeyPad::clicked1() { emit addText( "1" ); }
void KeyPad::clicked2() { emit addText( "2" ); }
void KeyPad::clicked3() { emit addText( "3" ); }
void KeyPad::clicked4() { emit addText( "4" ); }
void KeyPad::clicked5() { emit addText( "5" ); }
void KeyPad::clicked6() { emit addText( "6" ); }
void KeyPad::clicked7() { emit addText( "7" ); }
void KeyPad::clicked8() { emit addText( "8" ); }
void KeyPad::clicked9() { emit addText( "9" ); }
void KeyPad::clickedDot() { emit addText( "." ); }
void KeyPad::clickedEq() { emit evaluate(); }

void KeyPad::clickedSqrt() { emit addText( "sqrt(" ); }
void KeyPad::clickedBackspace() { emit addText( "<--" ); }
void KeyPad::clickedLParen() { emit addText( "(" ); }
void KeyPad::clickedRParen() { emit addText( ")" ); }
void KeyPad::clickedAdd() { emit addText( "+" ); }
void KeyPad::clickedSub() { emit addText( "-" ); }
void KeyPad::clickedMul() { emit addText( "*" ); }
void KeyPad::clickedDiv() { emit addText( "/" ); }

void KeyPad::clickedPi() { emit addText( "pi" ); }
void KeyPad::clickedAns() { emit addText( "ans" ); }
void KeyPad::clickedX() { emit addText( "x" ); }
void KeyPad::clickedXEq() { emit addText( "x=" ); }

void KeyPad::clickedExp() { emit addText( "exp(" ); }
void KeyPad::clickedLog() { emit addText( "log(" ); }
void KeyPad::clickedSin() { emit addText( "sin(" ); }
void KeyPad::clickedCos() { emit addText( "cos(" ); }
void KeyPad::clickedTan() { emit addText( "tan(" ); }
void KeyPad::clickedASin() { emit addText( "asin(" ); }
void KeyPad::clickedACos() { emit addText( "acos(" ); }
void KeyPad::clickedATan() { emit addText( "atan(" ); }
