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
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "autohidelabel.h"

#include <QLabel>
#include <QTimer>
#include <QToolTip>

#define TIMER_INTERVAL 50
#define FADE_STEP 16

class AutoHideLabelPrivate
{
public:
  QTimer* fadeTimer;
  int alpha;
};

AutoHideLabel::AutoHideLabel( QWidget* parent ): QLabel( parent )
{
  d = new AutoHideLabelPrivate;

  d->fadeTimer = new QTimer( this );
  d->fadeTimer->setInterval( TIMER_INTERVAL );
  connect( d->fadeTimer, SIGNAL( timeout() ), SLOT( fade() ) );

  d->alpha = 255;

  setPalette( QToolTip::palette() );
  setAutoFillBackground( true );
  setFrameShape( QFrame::Box );
}

AutoHideLabel::~AutoHideLabel()
{
  delete d;
}

void AutoHideLabel::showText( const QString& msg )
{
  setText( msg );
  adjustSize();
  show();
  raise();

  // 5 seconds display + fading
  d->alpha = 5*(1000/TIMER_INTERVAL*FADE_STEP) + 300;

  d->fadeTimer->stop();
  d->fadeTimer->start();
  fade();
}

void AutoHideLabel::hideText()
{
  d->fadeTimer->stop();
  hide();
}

void AutoHideLabel::fade()
{
  if( d->alpha <= 0)
  {
    d->fadeTimer->stop();
    hide();
  }
  else
  {
    d->alpha = qMax( 0, d->alpha-FADE_STEP );
    int a = qMin( 255, d->alpha );

    QPalette pal = QToolTip::palette();
    QColor c1 = pal.window().color();
    QColor c2 = pal.windowText().color();

    c1.setAlpha(a);
    c2.setAlpha(a);

    pal.setBrush( QPalette::Window, c1 );
    pal.setBrush( QPalette::WindowText, c2 );
    setPalette(pal);
  }
}
