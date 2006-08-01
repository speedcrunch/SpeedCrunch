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

class KeyPad::Private
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
    QPushButton* keyDiv;
    QPushButton* keyAns;
    QPushButton* keyLn;
    QPushButton* keyLog;
    QPushButton* keyExp;
    QPushButton* keyMul;
    QPushButton* keyX;
    QPushButton* keySinh;
    QPushButton* keyCosh;
    QPushButton* keyTanh;
    QPushButton* keySub;
    QPushButton* keyPi;
    QPushButton* keySin;
    QPushButton* keyCos;
    QPushButton* keyTan;
    QPushButton* keyAdd;
    QPushButton* keyLParen;
    QPushButton* keyRParen;
    QPushButton* keySqrt;
    QPushButton* keyBackspace;
};

KeyPad::KeyPad( QWidget* parent ) : QWidget( parent )
{
  d = new Private;

  QGridLayout* layout = new QGridLayout( this );
  layout->setMargin( 5 );
  layout->setSpacing( 5 );

  // Create buttons

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
  d->keyDiv = new QPushButton( tr("/"), this );
  d->keyAns = new QPushButton( tr("ans"), this );
  d->keyLn = new QPushButton( tr("ln"), this );
  d->keyLog = new QPushButton( tr("log"), this );
  d->keyExp = new QPushButton( tr("exp"), this );
  d->keyMul = new QPushButton( tr("*"), this );
  d->keyX = new QPushButton( tr("X"), this );
  d->keySinh = new QPushButton( tr("sinh"), this );
  d->keyCosh = new QPushButton( tr("cosh"), this );
  d->keyTanh = new QPushButton( tr("tanh"), this );
  d->keySub = new QPushButton( tr("-"), this );
  d->keyPi = new QPushButton( tr("pi"), this );
  d->keySin = new QPushButton( tr("sin"), this );
  d->keyCos = new QPushButton( tr("cos"), this );
  d->keyTan = new QPushButton( tr("tan"), this );
  d->keyAdd = new QPushButton( tr("+"), this );
  d->keyLParen = new QPushButton( tr("("), this );
  d->keyRParen = new QPushButton( tr(")"), this );
  d->keySqrt = new QPushButton( tr("sqrt"), this );
  d->keyBackspace = new QPushButton( tr("<--"), this );

  // Create a size hint for the buttons

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
  d->keyDiv->ensurePolished();
  d->keyAns->ensurePolished();
  d->keyLn->ensurePolished();
  d->keyLog->ensurePolished();
  d->keyExp->ensurePolished();
  d->keyMul->ensurePolished();
  d->keyX->ensurePolished();
  d->keySinh->ensurePolished();
  d->keyCosh->ensurePolished();
  d->keyTanh->ensurePolished();
  d->keySub->ensurePolished();
  d->keyPi->ensurePolished();
  d->keySin->ensurePolished();
  d->keyCos->ensurePolished();
  d->keyTan->ensurePolished();
  d->keyAdd->ensurePolished();
  d->keyLParen->ensurePolished();
  d->keyRParen->ensurePolished();
  d->keySqrt->ensurePolished();
  d->keyBackspace->ensurePolished();

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
  if( fm.width( d->keyDiv->text() ) > maxWidth ) maxWidth = fm.width( d->keyDiv->text() );
  if( fm.width( d->keyAns->text() ) > maxWidth ) maxWidth = fm.width( d->keyAns->text() );
  if( fm.width( d->keyLn->text() ) > maxWidth ) maxWidth = fm.width( d->keyLn->text() );
  if( fm.width( d->keyLog->text() ) > maxWidth ) maxWidth = fm.width( d->keyLog->text() );
  if( fm.width( d->keyExp->text() ) > maxWidth ) maxWidth = fm.width( d->keyExp->text() );
  if( fm.width( d->keyMul->text() ) > maxWidth ) maxWidth = fm.width( d->keyMul->text() );
  if( fm.width( d->keyX->text() ) > maxWidth ) maxWidth = fm.width( d->keyX->text() );
  if( fm.width( d->keySinh->text() ) > maxWidth ) maxWidth = fm.width( d->keySinh->text() );
  if( fm.width( d->keyCosh->text() ) > maxWidth ) maxWidth = fm.width( d->keyCosh->text() );
  if( fm.width( d->keyTanh->text() ) > maxWidth ) maxWidth = fm.width( d->keyTanh->text() );
  if( fm.width( d->keySub->text() ) > maxWidth ) maxWidth = fm.width( d->keySub->text() );
  if( fm.width( d->keyPi->text() ) > maxWidth ) maxWidth = fm.width( d->keyPi->text() );
  if( fm.width( d->keySin->text() ) > maxWidth ) maxWidth = fm.width( d->keySin->text() );
  if( fm.width( d->keyCos->text() ) > maxWidth ) maxWidth = fm.width( d->keyCos->text() );
  if( fm.width( d->keyTan->text() ) > maxWidth ) maxWidth = fm.width( d->keyTan->text() );
  if( fm.width( d->keyAdd->text() ) > maxWidth ) maxWidth = fm.width( d->keyAdd->text() );
  if( fm.width( d->keyLParen->text() ) > maxWidth ) maxWidth = fm.width( d->keyLParen->text() );
  if( fm.width( d->keyRParen->text() ) > maxWidth ) maxWidth = fm.width( d->keyRParen->text() );
  if( fm.width( d->keySqrt->text() ) > maxWidth ) maxWidth = fm.width( d->keySqrt->text() );
  if( fm.width( d->keyBackspace->text() ) > maxWidth ) maxWidth = fm.width( d->keyBackspace->text() );

  d->key0->setMaximumSize(
      d->key0->style()->sizeFromContents( QStyle::CT_PushButton,
                                          0,
                                          QSize( maxWidth*1.5, textHeight*1.5 ).expandedTo(QApplication::globalStrut()), d->key0 ) );
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
  d->keyDiv->setMaximumSize( d->key0->maximumSize() );
  d->keyAns->setMaximumSize( d->key0->maximumSize() );
  d->keyLn->setMaximumSize( d->key0->maximumSize() );
  d->keyLog->setMaximumSize( d->key0->maximumSize() );
  d->keyExp->setMaximumSize( d->key0->maximumSize() );
  d->keyMul->setMaximumSize( d->key0->maximumSize() );
  d->keyX->setMaximumSize( d->key0->maximumSize() );
  d->keySinh->setMaximumSize( d->key0->maximumSize() );
  d->keyCosh->setMaximumSize( d->key0->maximumSize() );
  d->keyTanh->setMaximumSize( d->key0->maximumSize() );
  d->keySub->setMaximumSize( d->key0->maximumSize() );
  d->keyPi->setMaximumSize( d->key0->maximumSize() );
  d->keySin->setMaximumSize( d->key0->maximumSize() );
  d->keyCos->setMaximumSize( d->key0->maximumSize() );
  d->keyTan->setMaximumSize( d->key0->maximumSize() );
  d->keyAdd->setMaximumSize( d->key0->maximumSize() );
  d->keyLParen->setMaximumSize( d->key0->maximumSize() );
  d->keyRParen->setMaximumSize( d->key0->maximumSize() );
  d->keySqrt->setMaximumSize( d->key0->maximumSize() );
  d->keyBackspace->setMaximumSize( d->key0->maximumSize() );

  // Add tool tips

  d->keyEq->setToolTip( tr("The assignment operator<br>Usage: <i>x = expression</i>") );
  d->keyAns->setToolTip( tr("The result from the last expression") );
  d->keyLn->setToolTip( tr("Natural logarithm") );
  d->keyLog->setToolTip( tr("Logarithm") );
  d->keyExp->setToolTip( tr("Exponential") );
  d->keyX->setToolTip( tr("The variable X") );
  d->keySinh->setToolTip( tr("Hyperbolic sine") );
  d->keyCosh->setToolTip( tr("Hyperbolic cosine") );
  d->keyTanh->setToolTip( tr("Hyperbolic tangens") );
  d->keyPi->setToolTip( tr("The number pi") );
  d->keySin->setToolTip( tr("Sine") );
  d->keyCos->setToolTip( tr("Cosine") );
  d->keyTan->setToolTip( tr("Tangens") );
  d->keySqrt->setToolTip( tr("Square root") );
  d->keyBackspace->setToolTip( tr("Backspace") );

  // Do not accept focus

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
  d->keyDiv->setFocusPolicy( Qt::NoFocus );
  d->keyAns->setFocusPolicy( Qt::NoFocus );
  d->keyLn->setFocusPolicy( Qt::NoFocus );
  d->keyLog->setFocusPolicy( Qt::NoFocus );
  d->keyExp->setFocusPolicy( Qt::NoFocus );
  d->keyMul->setFocusPolicy( Qt::NoFocus );
  d->keyX->setFocusPolicy( Qt::NoFocus );
  d->keySinh->setFocusPolicy( Qt::NoFocus );
  d->keyCosh->setFocusPolicy( Qt::NoFocus );
  d->keyTanh->setFocusPolicy( Qt::NoFocus );
  d->keySub->setFocusPolicy( Qt::NoFocus );
  d->keyPi->setFocusPolicy( Qt::NoFocus );
  d->keySin->setFocusPolicy( Qt::NoFocus );
  d->keyCos->setFocusPolicy( Qt::NoFocus );
  d->keyTan->setFocusPolicy( Qt::NoFocus );
  d->keyAdd->setFocusPolicy( Qt::NoFocus );
  d->keyLParen->setFocusPolicy( Qt::NoFocus );
  d->keyRParen->setFocusPolicy( Qt::NoFocus );
  d->keySqrt->setFocusPolicy( Qt::NoFocus );
  d->keyBackspace->setFocusPolicy( Qt::NoFocus );

  // Add to layout

  /*    7    8    9    +    (    ) sqrt   <- */
  /*    4    5    6    -   pi  sin  cos  tan */
  /*    1    2    3    *    x sinh cosh tanh */
  /*    0    .    =    /  ans   ln  log  exp */

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
  layout->addWidget( d->keyDiv, 3, 4 );
  layout->addWidget( d->keyAns, 3, 5 );
  layout->addWidget( d->keyLn, 3, 6 );
  layout->addWidget( d->keyLog, 3, 7 );
  layout->addWidget( d->keyExp, 3, 8 );
  layout->addWidget( d->keyMul, 2, 4 );
  layout->addWidget( d->keyX, 2, 5 );
  layout->addWidget( d->keySinh, 2, 6 );
  layout->addWidget( d->keyCosh, 2, 7 );
  layout->addWidget( d->keyTanh, 2, 8 );
  layout->addWidget( d->keySub, 1, 4 );
  layout->addWidget( d->keyPi, 1, 5 );
  layout->addWidget( d->keySin, 1, 6 );
  layout->addWidget( d->keyCos, 1, 7 );
  layout->addWidget( d->keyTan, 1, 8 );
  layout->addWidget( d->keyAdd, 0, 4 );
  layout->addWidget( d->keyLParen, 0, 5 );
  layout->addWidget( d->keyRParen, 0, 6 );
  layout->addWidget( d->keySqrt, 0, 7 );
  layout->addWidget( d->keyBackspace, 0, 8 );

  // Setup connections

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
  connect( d->keyDiv, SIGNAL(clicked()), SLOT(clickedDiv()) );
  connect( d->keyAns, SIGNAL(clicked()), SLOT(clickedAns()) );
  connect( d->keyLn, SIGNAL(clicked()), SLOT(clickedLn()) );
  connect( d->keyLog, SIGNAL(clicked()), SLOT(clickedLog()) );
  connect( d->keyExp, SIGNAL(clicked()), SLOT(clickedExp()) );
  connect( d->keyMul, SIGNAL(clicked()), SLOT(clickedMul()) );
  connect( d->keyX, SIGNAL(clicked()), SLOT(clickedX()) );
  connect( d->keySinh, SIGNAL(clicked()), SLOT(clickedSinh()) );
  connect( d->keyCosh, SIGNAL(clicked()), SLOT(clickedCosh()) );
  connect( d->keyTanh, SIGNAL(clicked()), SLOT(clickedTanh()) );
  connect( d->keySub, SIGNAL(clicked()), SLOT(clickedSub()) );
  connect( d->keyPi, SIGNAL(clicked()), SLOT(clickedPi()) );
  connect( d->keySin, SIGNAL(clicked()), SLOT(clickedSin()) );
  connect( d->keyCos, SIGNAL(clicked()), SLOT(clickedCos()) );
  connect( d->keyTan, SIGNAL(clicked()), SLOT(clickedTan()) );
  connect( d->keyAdd, SIGNAL(clicked()), SLOT(clickedAdd()) );
  connect( d->keyLParen, SIGNAL(clicked()), SLOT(clickedLParen()) );
  connect( d->keyRParen, SIGNAL(clicked()), SLOT(clickedRParen()) );
  connect( d->keySqrt, SIGNAL(clicked()), SLOT(clickedSqrt()) );
  connect( d->keyBackspace, SIGNAL(clicked()), SLOT(clickedBackspace()) );
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
void KeyPad::clickedEq() { emit addText( "=" ); }
void KeyPad::clickedDiv() { emit addText( "/" ); }
void KeyPad::clickedAns() { emit addText( "ans" ); }
void KeyPad::clickedLn() { emit addText( "ln(" ); }
void KeyPad::clickedLog() { emit addText( "log(" ); }
void KeyPad::clickedExp() { emit addText( "exp(" ); }
void KeyPad::clickedMul() { emit addText( "*" ); }
void KeyPad::clickedX() { emit addText( "X" ); }
void KeyPad::clickedSinh() { emit addText( "sinh(" ); }
void KeyPad::clickedCosh() { emit addText( "cosh(" ); }
void KeyPad::clickedTanh() { emit addText( "tanh(" ); }
void KeyPad::clickedSub() { emit addText( "-" ); }
void KeyPad::clickedPi() { emit addText( "pi" ); }
void KeyPad::clickedSin() { emit addText( "sin(" ); }
void KeyPad::clickedCos() { emit addText( "cos(" ); }
void KeyPad::clickedTan() { emit addText( "tan(" ); }
void KeyPad::clickedAdd() { emit addText( "+" ); }
void KeyPad::clickedLParen() { emit addText( "(" ); }
void KeyPad::clickedRParen() { emit addText( ")" ); }
void KeyPad::clickedSqrt() { emit addText( "sqrt(" ); }
void KeyPad::clickedBackspace() { emit addText( "<--" ); }
