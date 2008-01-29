// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2008 Helder Correia <helder.pereira.correia@gmail.com>
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


#include "tipwidget.hxx"

#include <QLabel>
#include <QPushButton>
#include <QResizeEvent>
#include <QTimeLine>
#include <QTimer>
#include <QToolTip>


struct TipWidget::Private
{
  QTimeLine *   fadeTimeLine;
  QTimer *      hideTimer;
  QPushButton * nextButton;
  QLabel *      tipLabel;
};


TipWidget::TipWidget( QWidget * parent ) : QFrame( parent ),
  d( new TipWidget::Private )
{
  setObjectName( "TipWidget" );

  d->tipLabel = new QLabel( this );
  d->tipLabel->setWordWrap( true );
//  d->tipLabel->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );

  d->nextButton = new QPushButton( this );
//  d->nextButton->setText( tr("Next") );
  d->nextButton->hide();

  d->hideTimer = new QTimer( this );
  d->hideTimer->setInterval( 4000 );
  d->hideTimer->setSingleShot( true );
  connect( d->hideTimer, SIGNAL( timeout() ), SLOT( disappear() ) );

  d->fadeTimeLine = new QTimeLine( 750, this );
  d->fadeTimeLine->setFrameRange( 0, 100 );
  //d->fadeTimeLine->setUpdateInterval( 25 );
  d->fadeTimeLine->setCurveShape( QTimeLine::EaseInCurve );
  connect( d->fadeTimeLine, SIGNAL( frameChanged( int ) ),
           SLOT( animateFade(int) ) );

//  QPalette normal = palette();
  setPalette( QToolTip::palette() );

//  d->nextButton->setPalette( normal );
//  d->tipLabel->setPalette( yellow );
//  d->tipLabel->setAutoFillBackground( true );

  setFrameShape( QFrame::Box );
  setAutoFillBackground( true );

  setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
}


TipWidget::~TipWidget()
{
  delete d;
}


QSize TipWidget::sizeHint() const
{
  return d->tipLabel->sizeHint() + QSize(100, 0);
}


void TipWidget::hideText()
{
  if ( d->fadeTimeLine->state() == QTimeLine::NotRunning )
  {
    d->hideTimer->stop();
    QTimer::singleShot( 0, this, SLOT( disappear() ) );
  }
}


void TipWidget::showText( const QString & msg, const QString & title )
{
  setPalette( QToolTip::palette() );

  QString text = tr("<b>%1</b><br>%2").arg( title ).arg( msg );
  d->tipLabel->setText( text );
  d->tipLabel->adjustSize();
  d->tipLabel->resize( width(), d->tipLabel->sizeHint().height() );

  QTimer::singleShot( 0, this, SLOT( appear() ) );
}


void TipWidget::animateFade( int v )
{
  resize( width(), d->fadeTimeLine->endFrame() - v );
  d->tipLabel->move( 10, -v );

  int a = qMax( 0, 240 - v * 4 );

  QPalette pal = QToolTip::palette();
  QColor c1 = pal.window().color();
  QColor c2 = pal.windowText().color();

  c1.setAlpha( a );
  c2.setAlpha( a );

  pal.setBrush( QPalette::Window,     c1 );
  pal.setBrush( QPalette::WindowText, c2 );
  setPalette( pal );
}


void TipWidget::appear()
{
  resize( width(), sizeHint().height() );

  show();
  raise();

  d->fadeTimeLine->stop();
  d->fadeTimeLine->setFrameRange( 0, height() );
  animateFade( 0 );

  d->hideTimer->stop();
  d->hideTimer->start();
}


void TipWidget::disappear()
{
  d->fadeTimeLine->setFrameRange( 0, height() );
  d->fadeTimeLine->start();
}
