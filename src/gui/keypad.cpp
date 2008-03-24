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


#include "keypad.hxx"

#include <QApplication>
#include <QGridLayout>
#include <QLocale>
#include <QPushButton>
#include <QStyle>


struct Keypad::Private
{
    // parent
    Keypad * p;

    // attributes
    char radixChar;

    // widgets
    QPushButton * key0;
    QPushButton * key1;
    QPushButton * key2;
    QPushButton * key3;
    QPushButton * key4;
    QPushButton * key5;
    QPushButton * key6;
    QPushButton * key7;
    QPushButton * key8;
    QPushButton * key9;
    QPushButton * keyAcos;
    QPushButton * keyAdd;
    QPushButton * keyAns;
    QPushButton * keyAsin;
    QPushButton * keyAtan;
    QPushButton * keyC;
    QPushButton * keyCos;
    QPushButton * keyDiv;
    QPushButton * keyDot;
    QPushButton * keyEE;
    QPushButton * keyEq;
    QPushButton * keyExp;
    QPushButton * keyFact;
    QPushButton * keyLn;
    QPushButton * keyLPar;
    QPushButton * keyMul;
    QPushButton * keyPerc;
    QPushButton * keyPi;
    QPushButton * keyRaise;
    QPushButton * keyRPar;
    QPushButton * keySin;
    QPushButton * keySqrt;
    QPushButton * keySub;
    QPushButton * keyTan;
    QPushButton * keyX;
    QPushButton * keyXEq;

    // methods
    void createButtons();
    void disableButtonFocus();
    void layoutButtons();
    void setButtonTooltips();
    void setUpButtonPressedSignal();
    void sizeButtons();
};


// public

Keypad::Keypad( char radixChar, QWidget * parent )
  : QWidget( parent ), d( new Keypad::Private )
{
  d->p = this;

  if ( radixChar == 'C' )
    d->radixChar = QLocale().decimalPoint().toAscii();
  else
    d->radixChar = radixChar;

  d->createButtons();
  d->sizeButtons();
  d->layoutButtons();
  d->setButtonTooltips();
  d->setUpButtonPressedSignal();
  d->disableButtonFocus();
}


Keypad::~Keypad()
{
  delete d;
}


char Keypad::radixChar() const
{
  return d->radixChar;
}


// public slots

void Keypad::setRadixChar( char c )
{
  if ( c == 'C' )
    c = QLocale().decimalPoint().toAscii();
  if ( d->radixChar != c )
  {
    d->radixChar = c;
    d->keyDot->setText( QString( c ) );
  }
}


void Keypad::retranslateText()
{
  d->setButtonTooltips();;
}


// protected slots

void Keypad::key0Pressed()     { emit buttonPressed( Keypad::Key0         ); };
void Keypad::key1Pressed()     { emit buttonPressed( Keypad::Key1         ); };
void Keypad::key2Pressed()     { emit buttonPressed( Keypad::Key2         ); };
void Keypad::key3Pressed()     { emit buttonPressed( Keypad::Key3         ); };
void Keypad::key4Pressed()     { emit buttonPressed( Keypad::Key4         ); };
void Keypad::key5Pressed()     { emit buttonPressed( Keypad::Key5         ); };
void Keypad::key6Pressed()     { emit buttonPressed( Keypad::Key6         ); };
void Keypad::key7Pressed()     { emit buttonPressed( Keypad::Key7         ); };
void Keypad::key8Pressed()     { emit buttonPressed( Keypad::Key8         ); };
void Keypad::key9Pressed()     { emit buttonPressed( Keypad::Key9         ); };
void Keypad::keyAcosPressed()  { emit buttonPressed( Keypad::KeyAcos      ); };
void Keypad::keyAddPressed()   { emit buttonPressed( Keypad::KeyPlus      ); };
void Keypad::keyAnsPressed()   { emit buttonPressed( Keypad::KeyAns       ); };
void Keypad::keyAsinPressed()  { emit buttonPressed( Keypad::KeyAsin      ); };
void Keypad::keyAtanPressed()  { emit buttonPressed( Keypad::KeyAtan      ); };
void Keypad::keyCosPressed()   { emit buttonPressed( Keypad::KeyCos       ); };
void Keypad::keyCPressed()     { emit buttonPressed( Keypad::KeyClear     ); };
void Keypad::keyDivPressed()   { emit buttonPressed( Keypad::KeyDivide    ); };
void Keypad::keyDotPressed()   { emit buttonPressed( Keypad::KeyRadixChar ); };
void Keypad::keyEEPressed()    { emit buttonPressed( Keypad::KeyEE        ); };
void Keypad::keyEqPressed()    { emit buttonPressed( Keypad::KeyEquals    ); };
void Keypad::keyExpPressed()   { emit buttonPressed( Keypad::KeyExp       ); };
void Keypad::keyFactPressed()  { emit buttonPressed( Keypad::KeyFactorial ); };
void Keypad::keyLnPressed()    { emit buttonPressed( Keypad::KeyLn        ); };
void Keypad::keyLParPressed()  { emit buttonPressed( Keypad::KeyLeftPar   ); };
void Keypad::keyMulPressed()   { emit buttonPressed( Keypad::KeyTimes     ); };
void Keypad::keyPercPressed()  { emit buttonPressed( Keypad::KeyPercent   ); };
void Keypad::keyPiPressed()    { emit buttonPressed( Keypad::KeyPi        ); };
void Keypad::keyRaisePressed() { emit buttonPressed( Keypad::KeyRaise     ); };
void Keypad::keyRParPressed()  { emit buttonPressed( Keypad::KeyRightPar  ); };
void Keypad::keySinPressed()   { emit buttonPressed( Keypad::KeySin       ); };
void Keypad::keySqrtPressed()  { emit buttonPressed( Keypad::KeySqrt      ); };
void Keypad::keySubPressed()   { emit buttonPressed( Keypad::KeyMinus     ); };
void Keypad::keyTanPressed()   { emit buttonPressed( Keypad::KeyTan       ); };
void Keypad::keyXPressed()     { emit buttonPressed( Keypad::KeyX         ); };
void Keypad::keyXEqPressed()   { emit buttonPressed( Keypad::KeyXEquals   ); };


// private

void Keypad::Private::createButtons()
{
  key0      = new QPushButton( "0",    p );
  key1      = new QPushButton( "1",    p );
  key2      = new QPushButton( "2",    p );
  key3      = new QPushButton( "3",    p );
  key4      = new QPushButton( "4",    p );
  key5      = new QPushButton( "5",    p );
  key6      = new QPushButton( "6",    p );
  key7      = new QPushButton( "7",    p );
  key8      = new QPushButton( "8",    p );
  key9      = new QPushButton( "9",    p );
  keyAcos   = new QPushButton( "acos", p );
  keyAdd    = new QPushButton( "+",    p );
  keyAns    = new QPushButton( "ans",  p );
  keyAsin   = new QPushButton( "asin", p );
  keyAtan   = new QPushButton( "atan", p );
  keyC      = new QPushButton( "C",    p );
  keyCos    = new QPushButton( "cos",  p );
  keyDiv    = new QPushButton( "/",    p );
  keyEE     = new QPushButton( "E",    p );
  keyEq     = new QPushButton( "=",    p );
  keyExp    = new QPushButton( "exp",  p );
  keyFact   = new QPushButton( "!",    p );
  keyLn     = new QPushButton( "ln",   p );
  keyLPar   = new QPushButton( "(",    p );
  keyMul    = new QPushButton( "*",    p );
  keyPerc   = new QPushButton( "%",    p );
  keyRaise  = new QPushButton( "^",    p );
  keyRPar   = new QPushButton( ")",    p );
  keySin    = new QPushButton( "sin",  p );
  keySub    = new QPushButton( "-",    p );
  keyTan    = new QPushButton( "tan",  p );
  keyXEq    = new QPushButton( "x=",   p );
  keyX      = new QPushButton( "x",    p );

  keyDot    = new QPushButton( QString( radixChar ),     p );
  keyPi     = new QPushButton( QString::fromUtf8( "π" ), p );
  keySqrt   = new QPushButton( QString::fromUtf8( "√" ), p );
}


void Keypad::Private::disableButtonFocus()
{
  key0->setFocusPolicy    ( Qt::NoFocus );
  key1->setFocusPolicy    ( Qt::NoFocus );
  key2->setFocusPolicy    ( Qt::NoFocus );
  key3->setFocusPolicy    ( Qt::NoFocus );
  key4->setFocusPolicy    ( Qt::NoFocus );
  key5->setFocusPolicy    ( Qt::NoFocus );
  key6->setFocusPolicy    ( Qt::NoFocus );
  key7->setFocusPolicy    ( Qt::NoFocus );
  key8->setFocusPolicy    ( Qt::NoFocus );
  key9->setFocusPolicy    ( Qt::NoFocus );
  keyAcos->setFocusPolicy ( Qt::NoFocus );
  keyAdd->setFocusPolicy  ( Qt::NoFocus );
  keyAns->setFocusPolicy  ( Qt::NoFocus );
  keyAsin->setFocusPolicy ( Qt::NoFocus );
  keyAtan->setFocusPolicy ( Qt::NoFocus );
  keyCos->setFocusPolicy  ( Qt::NoFocus );
  keyC->setFocusPolicy    ( Qt::NoFocus );
  keyDiv->setFocusPolicy  ( Qt::NoFocus );
  keyDot->setFocusPolicy  ( Qt::NoFocus );
  keyEE->setFocusPolicy   ( Qt::NoFocus );
  keyEq->setFocusPolicy   ( Qt::NoFocus );
  keyExp->setFocusPolicy  ( Qt::NoFocus );
  keyFact->setFocusPolicy ( Qt::NoFocus );
  keyLn->setFocusPolicy   ( Qt::NoFocus );
  keyLPar->setFocusPolicy ( Qt::NoFocus );
  keyMul->setFocusPolicy  ( Qt::NoFocus );
  keyPerc->setFocusPolicy ( Qt::NoFocus );
  keyPi->setFocusPolicy   ( Qt::NoFocus );
  keyRaise->setFocusPolicy( Qt::NoFocus );
  keyRPar->setFocusPolicy ( Qt::NoFocus );
  keySin->setFocusPolicy  ( Qt::NoFocus );
  keySqrt->setFocusPolicy ( Qt::NoFocus );
  keySub->setFocusPolicy  ( Qt::NoFocus );
  keyTan->setFocusPolicy  ( Qt::NoFocus );
  keyXEq->setFocusPolicy  ( Qt::NoFocus );
  keyX->setFocusPolicy    ( Qt::NoFocus );
}


void Keypad::Private::layoutButtons()
{
  QGridLayout * layout = new QGridLayout( p );
  layout->setMargin( 3 );
  layout->setSpacing( 3 );

  layout->addWidget( key0,      3, 0 );
  layout->addWidget( key1,      2, 0 );
  layout->addWidget( key2,      2, 1 );
  layout->addWidget( key3,      2, 2 );
  layout->addWidget( key4,      1, 0 );
  layout->addWidget( key5,      1, 1 );
  layout->addWidget( key6,      1, 2 );
  layout->addWidget( key7,      0, 0 );
  layout->addWidget( key8,      0, 1 );
  layout->addWidget( key9,      0, 2 );
  layout->addWidget( keyAcos,   2, 8 );
  layout->addWidget( keyAdd,    3, 3 );
  layout->addWidget( keyAns,    1, 6 );
  layout->addWidget( keyAsin,   1, 8 );
  layout->addWidget( keyAtan,   3, 8 );
  layout->addWidget( keyC,      0, 4 );
  layout->addWidget( keyCos,    2, 7 );
  layout->addWidget( keyDiv,    0, 3 );
  layout->addWidget( keyDot,    3, 1 );
  layout->addWidget( keyEE,     1, 4 );
  layout->addWidget( keyEq,     3, 2 );
  layout->addWidget( keyExp,    0, 7 );
  layout->addWidget( keyFact,   3, 5 );
  layout->addWidget( keyLn,     0, 8 );
  layout->addWidget( keyLPar,   2, 4 );
  layout->addWidget( keyMul,    1, 3 );
  layout->addWidget( keyPerc,   3, 4 );
  layout->addWidget( keyPi,     0, 6 );
  layout->addWidget( keyRaise,  1, 5 );
  layout->addWidget( keyRPar,   2, 5 );
  layout->addWidget( keySin,    1, 7 );
  layout->addWidget( keySqrt,   0, 5 );
  layout->addWidget( keySub,    2, 3 );
  layout->addWidget( keyTan,    3, 7 );
  layout->addWidget( keyX,      2, 6 );
  layout->addWidget( keyXEq,    3, 6 );
}


void Keypad::Private::setButtonTooltips()
{
  keyAcos->setToolTip( Keypad::tr( "Inverse cosine"      ) );
  keyAns->setToolTip ( Keypad::tr( "The last result"     ) );
  keyAsin->setToolTip( Keypad::tr( "Inverse sine"        ) );
  keyAtan->setToolTip( Keypad::tr( "Inverse tangent"     ) );
  keyC->setToolTip   ( Keypad::tr( "Clear expression"    ) );
  keyCos->setToolTip ( Keypad::tr( "Cosine"              ) );
  keyEE->setToolTip  ( Keypad::tr( "Scientific notation" ) );
  keyExp->setToolTip ( Keypad::tr( "Exponential"         ) );
  keyLn->setToolTip  ( Keypad::tr( "Natural logarithm"   ) );
  keySin->setToolTip ( Keypad::tr( "Sine"                ) );
  keySqrt->setToolTip( Keypad::tr( "Square root"         ) );
  keyTan->setToolTip ( Keypad::tr( "Tangent"             ) );
  keyXEq->setToolTip ( Keypad::tr( "Assign variable x"   ) );
  keyX->setToolTip   ( Keypad::tr( "The variable x"      ) );
}


void Keypad::Private::setUpButtonPressedSignal()
{
  connect( key0,     SIGNAL( clicked() ), p, SLOT( key0Pressed()    ) );
  connect( key1,     SIGNAL( clicked() ), p, SLOT( key1Pressed()    ) );
  connect( key2,     SIGNAL( clicked() ), p, SLOT( key2Pressed()    ) );
  connect( key3,     SIGNAL( clicked() ), p, SLOT( key3Pressed()    ) );
  connect( key4,     SIGNAL( clicked() ), p, SLOT( key4Pressed()    ) );
  connect( key5,     SIGNAL( clicked() ), p, SLOT( key5Pressed()    ) );
  connect( key6,     SIGNAL( clicked() ), p, SLOT( key6Pressed()    ) );
  connect( key7,     SIGNAL( clicked() ), p, SLOT( key7Pressed()    ) );
  connect( key8,     SIGNAL( clicked() ), p, SLOT( key8Pressed()    ) );
  connect( key9,     SIGNAL( clicked() ), p, SLOT( key9Pressed()    ) );
  connect( keyAcos,  SIGNAL( clicked() ), p, SLOT( keyAcosPressed() ) );
  connect( keyAdd,   SIGNAL( clicked() ), p, SLOT( keyAddPressed()  ) );
  connect( keyAns,   SIGNAL( clicked() ), p, SLOT( keyAnsPressed()  ) );
  connect( keyAsin,  SIGNAL( clicked() ), p, SLOT( keyAsinPressed() ) );
  connect( keyAtan,  SIGNAL( clicked() ), p, SLOT( keyAtanPressed() ) );
  connect( keyC,     SIGNAL( clicked() ), p, SLOT( keyCPressed()    ) );
  connect( keyCos,   SIGNAL( clicked() ), p, SLOT( keyCosPressed()  ) );
  connect( keyDiv,   SIGNAL( clicked() ), p, SLOT( keyDivPressed()  ) );
  connect( keyEE,    SIGNAL( clicked() ), p, SLOT( keyEEPressed()   ) );
  connect( keyEq,    SIGNAL( clicked() ), p, SLOT( keyEqPressed()   ) );
  connect( keyExp,   SIGNAL( clicked() ), p, SLOT( keyExpPressed()  ) );
  connect( keyFact,  SIGNAL( clicked() ), p, SLOT( keyFactPressed() ) );
  connect( keyLn,    SIGNAL( clicked() ), p, SLOT( keyLnPressed()   ) );
  connect( keyLPar,  SIGNAL( clicked() ), p, SLOT( keyLParPressed() ) );
  connect( keyMul,   SIGNAL( clicked() ), p, SLOT( keyMulPressed()  ) );
  connect( keyPerc,  SIGNAL( clicked() ), p, SLOT( keyPercPressed() ) );
  connect( keyRaise, SIGNAL( clicked() ), p, SLOT( keyRaisePressed()) );
  connect( keyRPar,  SIGNAL( clicked() ), p, SLOT( keyRParPressed() ) );
  connect( keySin,   SIGNAL( clicked() ), p, SLOT( keySinPressed()  ) );
  connect( keySub,   SIGNAL( clicked() ), p, SLOT( keySubPressed()  ) );
  connect( keyTan,   SIGNAL( clicked() ), p, SLOT( keyTanPressed()  ) );
  connect( keyXEq,   SIGNAL( clicked() ), p, SLOT( keyXEqPressed()  ) );
  connect( keyX,     SIGNAL( clicked() ), p, SLOT( keyXPressed()    ) );
  connect( keyDot,   SIGNAL( clicked() ), p, SLOT( keyDotPressed()  ) );
  connect( keyPi,    SIGNAL( clicked() ), p, SLOT( keyPiPressed()   ) );
  connect( keySqrt,  SIGNAL( clicked() ), p, SLOT( keySqrtPressed() ) );
}


void Keypad::Private::sizeButtons()
{
  // The same font in all buttons, so just pick one
  QFontMetrics fm = key0->fontMetrics();

  int maxWidth = fm.width( keyAcos->text() );
  int textHeight = qMax( fm.lineSpacing(), 14 );
  QSize size =
#ifdef Q_WS_MAC
               key0->style()->sizeFromContents( QStyle::CT_PushButton,
#else
               key0->style()->sizeFromContents( QStyle::CT_ToolButton,
#endif
                 0, QSize( int(maxWidth), int(textHeight) ).expandedTo( QApplication::globalStrut() ), keyAcos );

  // limit the size of the buttons
  key0->setMaximumSize     ( size ); key0->setMinimumSize     ( size );
  key1->setMaximumSize     ( size ); key1->setMinimumSize     ( size );
  key2->setMaximumSize     ( size ); key2->setMinimumSize     ( size );
  key3->setMaximumSize     ( size ); key3->setMinimumSize     ( size );
  key4->setMaximumSize     ( size ); key4->setMinimumSize     ( size );
  key5->setMaximumSize     ( size ); key5->setMinimumSize     ( size );
  key6->setMaximumSize     ( size ); key6->setMinimumSize     ( size );
  key7->setMaximumSize     ( size ); key7->setMinimumSize     ( size );
  key8->setMaximumSize     ( size ); key8->setMinimumSize     ( size );
  key9->setMaximumSize     ( size ); key9->setMinimumSize     ( size );
  keyAcos->setMaximumSize  ( size ); keyAcos->setMinimumSize  ( size );
  keyAdd->setMaximumSize   ( size ); keyAdd->setMinimumSize   ( size );
  keyAns->setMaximumSize   ( size ); keyAns->setMinimumSize   ( size );
  keyAsin->setMaximumSize  ( size ); keyAsin->setMinimumSize  ( size );
  keyAtan->setMaximumSize  ( size ); keyAtan->setMinimumSize  ( size );
  keyC->setMaximumSize     ( size ); keyC->setMinimumSize     ( size );
  keyCos->setMaximumSize   ( size ); keyCos->setMinimumSize   ( size );
  keyDiv->setMaximumSize   ( size ); keyDiv->setMinimumSize   ( size );
  keyDot->setMaximumSize   ( size ); keyDot->setMinimumSize   ( size );
  keyEE->setMaximumSize    ( size ); keyEE->setMinimumSize    ( size );
  keyEq->setMaximumSize    ( size ); keyEq->setMinimumSize    ( size );
  keyExp->setMaximumSize   ( size ); keyExp->setMinimumSize   ( size );
  keyFact->setMaximumSize  ( size ); keyFact->setMinimumSize  ( size );
  keyLn->setMaximumSize    ( size ); keyLn->setMinimumSize    ( size );
  keyLPar->setMaximumSize  ( size ); keyLPar->setMinimumSize  ( size );
  keyMul->setMaximumSize   ( size ); keyMul->setMinimumSize   ( size );
  keyPerc->setMaximumSize  ( size ); keyPerc->setMinimumSize  ( size );
  keyPi->setMaximumSize    ( size ); keyPi->setMinimumSize    ( size );
  keyRaise->setMaximumSize ( size ); keyRaise->setMinimumSize ( size );
  keyRPar->setMaximumSize  ( size ); keyRPar->setMinimumSize  ( size );
  keySin->setMaximumSize   ( size ); keySin->setMinimumSize   ( size );
  keySqrt->setMaximumSize  ( size ); keySqrt->setMinimumSize  ( size );
  keySub->setMaximumSize   ( size ); keySub->setMinimumSize   ( size );
  keyTan->setMaximumSize   ( size ); keyTan->setMinimumSize   ( size );
  keyXEq->setMaximumSize   ( size ); keyXEq->setMinimumSize   ( size );
  keyX->setMaximumSize     ( size ); keyX->setMinimumSize     ( size );
}
