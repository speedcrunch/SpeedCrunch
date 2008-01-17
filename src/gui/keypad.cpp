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


#include <base/settings.hxx>
#include <gui/keypad.hxx>

#include <QApplication>
#include <QGridLayout>
#include <QPushButton>
#include <QStyle>


class KeypadPrivate
{
  public:
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
    QPushButton * keyACos;
    QPushButton * keyAdd;
    QPushButton * keyAns;
    QPushButton * keyASin;
    QPushButton * keyATan;
    QPushButton * keyAvg;
    QPushButton * keyCos;
    QPushButton * keyDiv;
    QPushButton * keyDot;
    QPushButton * keyEE;
    QPushButton * keyExp;
    QPushButton * keyFact;
    QPushButton * keyLn;
    QPushButton * keyLParen;
    QPushButton * keyMul;
    QPushButton * keyPerc;
    QPushButton * keyPi;
    QPushButton * keyRaise;
    QPushButton * keyRParen;
    QPushButton * keySemic;
    QPushButton * keySin;
    QPushButton * keySqrt;
    QPushButton * keySub;
    QPushButton * keyTan;
    QPushButton * keyX;
    QPushButton * keyXEq;
};


void Keypad::createButtons()
{
  QChar dot = Settings::decimalPoint();

  d->key0      = new QPushButton( "0",    this );
  d->key1      = new QPushButton( "1",    this );
  d->key2      = new QPushButton( "2",    this );
  d->key3      = new QPushButton( "3",    this );
  d->key4      = new QPushButton( "4",    this );
  d->key5      = new QPushButton( "5",    this );
  d->key6      = new QPushButton( "6",    this );
  d->key7      = new QPushButton( "7",    this );
  d->key8      = new QPushButton( "8",    this );
  d->key9      = new QPushButton( "9",    this );
  d->keyACos   = new QPushButton( "acos", this );
  d->keyAdd    = new QPushButton( "+",    this );
  d->keyAns    = new QPushButton( "ans",  this );
  d->keyASin   = new QPushButton( "asin", this );
  d->keyATan   = new QPushButton( "atan", this );
  d->keyAvg    = new QPushButton(         this );
  d->keyCos    = new QPushButton( "cos",  this );
  d->keyDiv    = new QPushButton( "/",    this );
  d->keyDot    = new QPushButton( dot,    this );
  d->keyEE     = new QPushButton(         this );
  d->keyExp    = new QPushButton(         this );
  d->keyFact   = new QPushButton( "!",    this );
  d->keyLn     = new QPushButton( "ln",   this );
  d->keyLParen = new QPushButton( "(",    this );
  d->keyMul    = new QPushButton( "*",    this );
  d->keyPerc   = new QPushButton( "%",    this );
  d->keyPi     = new QPushButton(         this );
  d->keyRaise  = new QPushButton( "^",    this );
  d->keyRParen = new QPushButton( ")",    this );
  d->keySemic  = new QPushButton( ";",    this );
  d->keySin    = new QPushButton( "sin",  this );
  d->keySqrt   = new QPushButton(         this );
  d->keySub    = new QPushButton( "-",    this );
  d->keyTan    = new QPushButton( "tan",  this );
  d->keyXEq    = new QPushButton( "x=",   this );
  d->keyX      = new QPushButton( "x",    this );

  d->keyAvg->setIcon ( QPixmap( ":/average.png" ) );
  d->keyEE->setIcon  ( QPixmap( ":/ee.png"      ) );
  d->keyExp->setIcon ( QPixmap( ":/exp.png"     ) );
  d->keyPi->setIcon  ( QPixmap( ":/pi.png"      ) );
  d->keySqrt->setIcon( QPixmap( ":/sqrt.png"    ) );
}


void Keypad::polishButtons()
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
  d->keyEE->ensurePolished();
  d->keySqrt->ensurePolished();
  d->keyRaise->ensurePolished();
  d->keyPerc->ensurePolished();
  d->keyFact->ensurePolished();
  d->keyLParen->ensurePolished();
  d->keySemic->ensurePolished();
  d->keyRParen->ensurePolished();
  d->keyAvg->ensurePolished();
  d->keyAdd->ensurePolished();
  d->keySub->ensurePolished();
  d->keyMul->ensurePolished();
  d->keyDiv->ensurePolished();
  d->keyPi->ensurePolished();
  d->keyAns->ensurePolished();
  d->keyX->ensurePolished();
  d->keyXEq->ensurePolished();
  d->keyExp->ensurePolished();
  d->keyLn->ensurePolished();
  d->keySin->ensurePolished();
  d->keyCos->ensurePolished();
  d->keyTan->ensurePolished();
  d->keyASin->ensurePolished();
  d->keyACos->ensurePolished();
  d->keyATan->ensurePolished();
}


void Keypad::sizeButtons()
{
  // The same font in all buttons, so just pick one
  QFontMetrics fm = d->key0->fontMetrics();

  int maxWidth = 0;
  int textHeight = qMax(fm.lineSpacing(), 14);
//  int marginSize = d->key0->frameWidth() * 2;

  // Find the widest text
  if( fm.width( d->key0->text()      ) > maxWidth ) maxWidth = fm.width( d->key0->text()      );
  if( fm.width( d->key1->text()      ) > maxWidth ) maxWidth = fm.width( d->key1->text()      );
  if( fm.width( d->key2->text()      ) > maxWidth ) maxWidth = fm.width( d->key2->text()      );
  if( fm.width( d->key3->text()      ) > maxWidth ) maxWidth = fm.width( d->key3->text()      );
  if( fm.width( d->key4->text()      ) > maxWidth ) maxWidth = fm.width( d->key4->text()      );
  if( fm.width( d->key5->text()      ) > maxWidth ) maxWidth = fm.width( d->key5->text()      );
  if( fm.width( d->key6->text()      ) > maxWidth ) maxWidth = fm.width( d->key6->text()      );
  if( fm.width( d->key7->text()      ) > maxWidth ) maxWidth = fm.width( d->key7->text()      );
  if( fm.width( d->key8->text()      ) > maxWidth ) maxWidth = fm.width( d->key8->text()      );
  if( fm.width( d->key9->text()      ) > maxWidth ) maxWidth = fm.width( d->key9->text()      );
  if( fm.width( d->keyDot->text()    ) > maxWidth ) maxWidth = fm.width( d->keyDot->text()    );
  if( fm.width( d->keySqrt->text()   ) > maxWidth ) maxWidth = fm.width( d->keySqrt->text()   );
  if( fm.width( d->keyRaise->text()  ) > maxWidth ) maxWidth = fm.width( d->keyRaise->text()  );
  if( fm.width( d->keyPerc->text()   ) > maxWidth ) maxWidth = fm.width( d->keyPerc->text()   );
  if( fm.width( d->keyFact->text()   ) > maxWidth ) maxWidth = fm.width( d->keyFact->text()   );
  if( fm.width( d->keyEE->text()     ) > maxWidth ) maxWidth = fm.width( d->keyEE->text()     );
  if( fm.width( d->keyLParen->text() ) > maxWidth ) maxWidth = fm.width( d->keyLParen->text() );
  if( fm.width( d->keySemic->text()  ) > maxWidth ) maxWidth = fm.width( d->keySemic->text()  );
  if( fm.width( d->keyRParen->text() ) > maxWidth ) maxWidth = fm.width( d->keyRParen->text() );
  if( fm.width( d->keyAvg->text()    ) > maxWidth ) maxWidth = fm.width( d->keyAvg->text()    );
  if( fm.width( d->keyAdd->text()    ) > maxWidth ) maxWidth = fm.width( d->keyAdd->text()    );
  if( fm.width( d->keySub->text()    ) > maxWidth ) maxWidth = fm.width( d->keySub->text()    );
  if( fm.width( d->keyMul->text()    ) > maxWidth ) maxWidth = fm.width( d->keyMul->text()    );
  if( fm.width( d->keyDiv->text()    ) > maxWidth ) maxWidth = fm.width( d->keyDiv->text()    );
  if( fm.width( d->keyPi->text()     ) > maxWidth ) maxWidth = fm.width( d->keyPi->text()     );
  if( fm.width( d->keyAns->text()    ) > maxWidth ) maxWidth = fm.width( d->keyAns->text()    );
  if( fm.width( d->keyX->text()      ) > maxWidth ) maxWidth = fm.width( d->keyX->text()      );
  if( fm.width( d->keyXEq->text()    ) > maxWidth ) maxWidth = fm.width( d->keyXEq->text()    );
  if( fm.width( d->keyExp->text()    ) > maxWidth ) maxWidth = fm.width( d->keyExp->text()    );
  if( fm.width( d->keyLn->text()     ) > maxWidth ) maxWidth = fm.width( d->keyLn->text()     );
  if( fm.width( d->keySin->text()    ) > maxWidth ) maxWidth = fm.width( d->keySin->text()    );
  if( fm.width( d->keyCos->text()    ) > maxWidth ) maxWidth = fm.width( d->keyCos->text()    );
  if( fm.width( d->keyTan->text()    ) > maxWidth ) maxWidth = fm.width( d->keyTan->text()    );
  if( fm.width( d->keyASin->text()   ) > maxWidth ) maxWidth = fm.width( d->keyASin->text()   );
  if( fm.width( d->keyACos->text()   ) > maxWidth ) maxWidth = fm.width( d->keyACos->text()   );
  if( fm.width( d->keyATan->text()   ) > maxWidth ) maxWidth = fm.width( d->keyATan->text()   );

  // Limit the size of the buttons
  d->key0->setMaximumSize(
#ifdef Q_WS_MAC
      d->key0->style()->sizeFromContents( QStyle::CT_PushButton,
#else
      d->key0->style()->sizeFromContents( QStyle::CT_ToolButton,
#endif
                                          0,
                                          QSize( int(maxWidth*1.0), int(textHeight*1.0) ).expandedTo(QApplication::globalStrut()), d->key0 ) );

  d->key1->setMaximumSize     ( d->key0->maximumSize() ); d->key1->setMinimumSize     ( d->key0->maximumSize() );
  d->key2->setMaximumSize     ( d->key0->maximumSize() ); d->key2->setMinimumSize     ( d->key0->maximumSize() );
  d->key3->setMaximumSize     ( d->key0->maximumSize() ); d->key3->setMinimumSize     ( d->key0->maximumSize() );
  d->key4->setMaximumSize     ( d->key0->maximumSize() ); d->key4->setMinimumSize     ( d->key0->maximumSize() );
  d->key5->setMaximumSize     ( d->key0->maximumSize() ); d->key5->setMinimumSize     ( d->key0->maximumSize() );
  d->key6->setMaximumSize     ( d->key0->maximumSize() ); d->key6->setMinimumSize     ( d->key0->maximumSize() );
  d->key7->setMaximumSize     ( d->key0->maximumSize() ); d->key7->setMinimumSize     ( d->key0->maximumSize() );
  d->key8->setMaximumSize     ( d->key0->maximumSize() ); d->key8->setMinimumSize     ( d->key0->maximumSize() );
  d->key9->setMaximumSize     ( d->key0->maximumSize() ); d->key9->setMinimumSize     ( d->key0->maximumSize() );
  d->keyDot->setMaximumSize   ( d->key0->maximumSize() ); d->keyDot->setMinimumSize   ( d->key0->maximumSize() );
  d->keySqrt->setMaximumSize  ( d->key0->maximumSize() ); d->keySqrt->setMinimumSize  ( d->key0->maximumSize() );
  d->keyRaise->setMaximumSize ( d->key0->maximumSize() ); d->keyRaise->setMinimumSize ( d->key0->maximumSize() );
  d->keyPerc->setMaximumSize  ( d->key0->maximumSize() ); d->keyPerc->setMinimumSize  ( d->key0->maximumSize() );
  d->keyFact->setMaximumSize  ( d->key0->maximumSize() ); d->keyFact->setMinimumSize  ( d->key0->maximumSize() );
  d->keyEE->setMaximumSize    ( d->key0->maximumSize() ); d->keyEE->setMinimumSize    ( d->key0->maximumSize() );
  d->keyLParen->setMaximumSize( d->key0->maximumSize() ); d->keyLParen->setMinimumSize( d->key0->maximumSize() );
  d->keySemic->setMaximumSize ( d->key0->maximumSize() ); d->keySemic->setMinimumSize ( d->key0->maximumSize() );
  d->keyRParen->setMaximumSize( d->key0->maximumSize() ); d->keyRParen->setMinimumSize( d->key0->maximumSize() );
  d->keyAvg->setMaximumSize   ( d->key0->maximumSize() ); d->keyAvg->setMinimumSize   ( d->key0->maximumSize() );
  d->keyAdd->setMaximumSize   ( d->key0->maximumSize() ); d->keyAdd->setMinimumSize   ( d->key0->maximumSize() );
  d->keySub->setMaximumSize   ( d->key0->maximumSize() ); d->keySub->setMinimumSize   ( d->key0->maximumSize() );
  d->keyMul->setMaximumSize   ( d->key0->maximumSize() ); d->keyMul->setMinimumSize   ( d->key0->maximumSize() );
  d->keyDiv->setMaximumSize   ( d->key0->maximumSize() ); d->keyDiv->setMinimumSize   ( d->key0->maximumSize() );
  d->keyPi->setMaximumSize    ( d->key0->maximumSize() ); d->keyPi->setMinimumSize    ( d->key0->maximumSize() );
  d->keyAns->setMaximumSize   ( d->key0->maximumSize() ); d->keyAns->setMinimumSize   ( d->key0->maximumSize() );
  d->keyX->setMaximumSize     ( d->key0->maximumSize() ); d->keyX->setMinimumSize     ( d->key0->maximumSize() );
  d->keyXEq->setMaximumSize   ( d->key0->maximumSize() ); d->keyXEq->setMinimumSize   ( d->key0->maximumSize() );
  d->keyExp->setMaximumSize   ( d->key0->maximumSize() ); d->keyExp->setMinimumSize   ( d->key0->maximumSize() );
  d->keyLn->setMaximumSize    ( d->key0->maximumSize() ); d->keyLn->setMinimumSize    ( d->key0->maximumSize() );
  d->keySin->setMaximumSize   ( d->key0->maximumSize() ); d->keySin->setMinimumSize   ( d->key0->maximumSize() );
  d->keyCos->setMaximumSize   ( d->key0->maximumSize() ); d->keyCos->setMinimumSize   ( d->key0->maximumSize() );
  d->keyTan->setMaximumSize   ( d->key0->maximumSize() ); d->keyTan->setMinimumSize   ( d->key0->maximumSize() );
  d->keyASin->setMaximumSize  ( d->key0->maximumSize() ); d->keyASin->setMinimumSize  ( d->key0->maximumSize() );
  d->keyACos->setMaximumSize  ( d->key0->maximumSize() ); d->keyACos->setMinimumSize  ( d->key0->maximumSize() );
  d->keyATan->setMaximumSize  ( d->key0->maximumSize() ); d->keyATan->setMinimumSize  ( d->key0->maximumSize() );
}


void Keypad::dontFocusButtons()
{
  d->key0->setFocusPolicy     ( Qt::NoFocus );
  d->key1->setFocusPolicy     ( Qt::NoFocus );
  d->key2->setFocusPolicy     ( Qt::NoFocus );
  d->key3->setFocusPolicy     ( Qt::NoFocus );
  d->key4->setFocusPolicy     ( Qt::NoFocus );
  d->key5->setFocusPolicy     ( Qt::NoFocus );
  d->key6->setFocusPolicy     ( Qt::NoFocus );
  d->key7->setFocusPolicy     ( Qt::NoFocus );
  d->key8->setFocusPolicy     ( Qt::NoFocus );
  d->key9->setFocusPolicy     ( Qt::NoFocus );
  d->keyDot->setFocusPolicy   ( Qt::NoFocus );
  d->keySqrt->setFocusPolicy  ( Qt::NoFocus );
  d->keyRaise->setFocusPolicy ( Qt::NoFocus );
  d->keyPerc->setFocusPolicy  ( Qt::NoFocus );
  d->keyFact->setFocusPolicy  ( Qt::NoFocus );
  d->keyEE->setFocusPolicy    ( Qt::NoFocus );
  d->keyLParen->setFocusPolicy( Qt::NoFocus );
  d->keySemic->setFocusPolicy ( Qt::NoFocus );
  d->keyRParen->setFocusPolicy( Qt::NoFocus );
  d->keyAvg->setFocusPolicy   ( Qt::NoFocus );
  d->keyAdd->setFocusPolicy   ( Qt::NoFocus );
  d->keySub->setFocusPolicy   ( Qt::NoFocus );
  d->keyMul->setFocusPolicy   ( Qt::NoFocus );
  d->keyDiv->setFocusPolicy   ( Qt::NoFocus );
  d->keyPi->setFocusPolicy    ( Qt::NoFocus );
  d->keyAns->setFocusPolicy   ( Qt::NoFocus );
  d->keyX->setFocusPolicy     ( Qt::NoFocus );
  d->keyXEq->setFocusPolicy   ( Qt::NoFocus );
  d->keyExp->setFocusPolicy   ( Qt::NoFocus );
  d->keyLn->setFocusPolicy    ( Qt::NoFocus );
  d->keySin->setFocusPolicy   ( Qt::NoFocus );
  d->keyCos->setFocusPolicy   ( Qt::NoFocus );
  d->keyTan->setFocusPolicy   ( Qt::NoFocus );
  d->keyASin->setFocusPolicy  ( Qt::NoFocus );
  d->keyACos->setFocusPolicy  ( Qt::NoFocus );
  d->keyATan->setFocusPolicy  ( Qt::NoFocus );
}


void Keypad::layoutButtons()
{

  QGridLayout* layout = new QGridLayout( this );
  layout->setMargin( 3 );
  layout->setSpacing( 3 );

  //   7    8    9     /   sqrt    ^     pi   exp   ln
  //   4    5    6     *     (     )    ans   sin  asin
  //   1    2    3     -     ;    avg    x    cos  acos
  //   0    .    E     +     %     !     x=   tan  atan

  layout->addWidget( d->key7,         0, 0 );
  layout->addWidget( d->key4,         1, 0 );
  layout->addWidget( d->key1,         2, 0 );
  layout->addWidget( d->key0,         3, 0 );
  layout->addWidget( d->key8,         0, 1 );
  layout->addWidget( d->key5,         1, 1 );
  layout->addWidget( d->key2,         2, 1 );
  layout->addWidget( d->keyDot,       3, 1 );
  layout->addWidget( d->key9,         0, 2 );
  layout->addWidget( d->key6,         1, 2 );
  layout->addWidget( d->key3,         2, 2 );
  layout->addWidget( d->keyEE,        3, 2 );
  layout->addWidget( d->keyDiv,       0, 3 );
  layout->addWidget( d->keyMul,       1, 3 );
  layout->addWidget( d->keySub,       2, 3 );
  layout->addWidget( d->keyAdd,       3, 3 );
  layout->addWidget( d->keySqrt,      0, 4 );
  layout->addWidget( d->keyLParen,    1, 4 );
  layout->addWidget( d->keySemic,     2, 4 );
  layout->addWidget( d->keyPerc,      3, 4 );
  layout->addWidget( d->keyRaise,     0, 5 );
  layout->addWidget( d->keyRParen,    1, 5 );
  layout->addWidget( d->keyAvg,       2, 5 );
  layout->addWidget( d->keyFact,      3, 5 );
  layout->addWidget( d->keyPi,        0, 6 );
  layout->addWidget( d->keyAns,       1, 6 );
  layout->addWidget( d->keyX,         2, 6 );
  layout->addWidget( d->keyXEq,       3, 6 );
  layout->addWidget( d->keyExp,       0, 7 );
  layout->addWidget( d->keySin,       1, 7 );
  layout->addWidget( d->keyCos,       2, 7 );
  layout->addWidget( d->keyTan,       3, 7 );
  layout->addWidget( d->keyLn,        0, 8 );
  layout->addWidget( d->keyASin,      1, 8 );
  layout->addWidget( d->keyACos,      2, 8 );
  layout->addWidget( d->keyATan,      3, 8 );
}


void Keypad::connectButtons()
{
  connect( d->key0,         SIGNAL(clicked()), SLOT(clicked0())      );
  connect( d->key1,         SIGNAL(clicked()), SLOT(clicked1())      );
  connect( d->key2,         SIGNAL(clicked()), SLOT(clicked2())      );
  connect( d->key3,         SIGNAL(clicked()), SLOT(clicked3())      );
  connect( d->key4,         SIGNAL(clicked()), SLOT(clicked4())      );
  connect( d->key5,         SIGNAL(clicked()), SLOT(clicked5())      );
  connect( d->key6,         SIGNAL(clicked()), SLOT(clicked6())      );
  connect( d->key7,         SIGNAL(clicked()), SLOT(clicked7())      );
  connect( d->key8,         SIGNAL(clicked()), SLOT(clicked8())      );
  connect( d->key9,         SIGNAL(clicked()), SLOT(clicked9())      );
  connect( d->keyDot,       SIGNAL(clicked()), SLOT(clickedDot())    );
  connect( d->keySqrt,      SIGNAL(clicked()), SLOT(clickedSqrt())   );
  connect( d->keyRaise,     SIGNAL(clicked()), SLOT(clickedRaise())  );
  connect( d->keyPerc,      SIGNAL(clicked()), SLOT(clickedPerc())   );
  connect( d->keyFact,      SIGNAL(clicked()), SLOT(clickedFact())   );
  connect( d->keyEE,        SIGNAL(clicked()), SLOT(clickedEE())     );
  connect( d->keyLParen,    SIGNAL(clicked()), SLOT(clickedLParen()) );
  connect( d->keySemic,     SIGNAL(clicked()), SLOT(clickedSemic())  );
  connect( d->keyRParen,    SIGNAL(clicked()), SLOT(clickedRParen()) );
  connect( d->keyAvg,       SIGNAL(clicked()), SLOT(clickedAvg())    );
  connect( d->keyAdd,       SIGNAL(clicked()), SLOT(clickedAdd())    );
  connect( d->keySub,       SIGNAL(clicked()), SLOT(clickedSub())    );
  connect( d->keyMul,       SIGNAL(clicked()), SLOT(clickedMul())    );
  connect( d->keyDiv,       SIGNAL(clicked()), SLOT(clickedDiv())    );
  connect( d->keyPi,        SIGNAL(clicked()), SLOT(clickedPi())     );
  connect( d->keyAns,       SIGNAL(clicked()), SLOT(clickedAns())    );
  connect( d->keyX,         SIGNAL(clicked()), SLOT(clickedX())      );
  connect( d->keyXEq,       SIGNAL(clicked()), SLOT(clickedXEq())    );
  connect( d->keyExp,       SIGNAL(clicked()), SLOT(clickedExp())    );
  connect( d->keyLn,        SIGNAL(clicked()), SLOT(clickedLn())     );
  connect( d->keySin,       SIGNAL(clicked()), SLOT(clickedSin())    );
  connect( d->keyCos,       SIGNAL(clicked()), SLOT(clickedCos())    );
  connect( d->keyTan,       SIGNAL(clicked()), SLOT(clickedTan())    );
  connect( d->keyASin,      SIGNAL(clicked()), SLOT(clickedASin())   );
  connect( d->keyACos,      SIGNAL(clicked()), SLOT(clickedACos())   );
  connect( d->keyATan,      SIGNAL(clicked()), SLOT(clickedATan())   );
}


Keypad::Keypad( QWidget* parent ) : QWidget( parent )
{
  d = new KeypadPrivate;

  // Setup buttons
  createButtons();
  polishButtons();
  sizeButtons();
  dontFocusButtons();
  layoutButtons();
  connectButtons();

  // Add tool tips
  d->keyAns->setToolTip ( tr("The result from the last expression") );
  d->keyLn->setToolTip  ( tr("Natural logarithm") );
  d->keyExp->setToolTip ( tr("Exponential") );
  d->keyX->setToolTip   ( tr("The variable X") );
  d->keyASin->setToolTip( tr("Inverse sine") );
  d->keyACos->setToolTip( tr("Inverse cosine") );
  d->keyATan->setToolTip( tr("Inverse tangent") );
  d->keyPi->setToolTip  ( tr("The number pi") );
  d->keySin->setToolTip ( tr("Sine") );
  d->keyCos->setToolTip ( tr("Cosine") );
  d->keyTan->setToolTip ( tr("Tangent") );
  d->keySqrt->setToolTip( tr("Square root") );
}


Keypad::~Keypad()
{
  delete d;
}


void Keypad::clicked0()      { emit addText( "0"        ); }
void Keypad::clicked1()      { emit addText( "1"        ); }
void Keypad::clicked2()      { emit addText( "2"        ); }
void Keypad::clicked3()      { emit addText( "3"        ); }
void Keypad::clicked4()      { emit addText( "4"        ); }
void Keypad::clicked5()      { emit addText( "5"        ); }
void Keypad::clicked6()      { emit addText( "6"        ); }
void Keypad::clicked7()      { emit addText( "7"        ); }
void Keypad::clicked8()      { emit addText( "8"        ); }
void Keypad::clicked9()      { emit addText( "9"        ); }
void Keypad::clickedSqrt()   { emit addText( "sqrt("    ); }
void Keypad::clickedRaise()  { emit addText( "^"        ); }
void Keypad::clickedPerc()   { emit addText( "%"        ); }
void Keypad::clickedFact()   { emit addText( "!"        ); }
void Keypad::clickedEE()     { emit addText( "e"        ); }
void Keypad::clickedLParen() { emit addText( "("        ); }
void Keypad::clickedSemic()  { emit addText( ";"        ); }
void Keypad::clickedRParen() { emit addText( ")"        ); }
void Keypad::clickedAvg()    { emit addText( "average(" ); }
void Keypad::clickedAdd()    { emit addText( "+"        ); }
void Keypad::clickedSub()    { emit addText( "-"        ); }
void Keypad::clickedMul()    { emit addText( "*"        ); }
void Keypad::clickedDiv()    { emit addText( "/"        ); }
void Keypad::clickedPi()     { emit addText( "pi"       ); }
void Keypad::clickedAns()    { emit addText( "ans"      ); }
void Keypad::clickedX()      { emit addText( "x"        ); }
void Keypad::clickedXEq()    { emit addText( "x="       ); }
void Keypad::clickedExp()    { emit addText( "exp("     ); }
void Keypad::clickedLn()     { emit addText( "ln("      ); }
void Keypad::clickedSin()    { emit addText( "sin("     ); }
void Keypad::clickedCos()    { emit addText( "cos("     ); }
void Keypad::clickedTan()    { emit addText( "tan("     ); }
void Keypad::clickedASin()   { emit addText( "asin("    ); }
void Keypad::clickedACos()   { emit addText( "acos("    ); }
void Keypad::clickedATan()   { emit addText( "atan("    ); }
void Keypad::clickedDot()    { emit addText( d->keyDot->text().toLatin1() ); }


void Keypad::settingsChanged()
{
  d->keyDot->setText( Settings::decimalPoint() );
}
