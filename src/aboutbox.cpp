/* This file is part of the SpeedCrunch project
   Copyright (C) 2004-2006 Ariya Hidayat <ariya@kde.org>
                 2005-2006 Johan Thelin <e8johan@gmail.com>

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

#include "aboutbox.h"

#include <QtGui>

#define MAKE_STRING(a) DO_MAKE_STRING(a)
#define DO_MAKE_STRING(a) # a

class MarqueeTextPrivate
{
public:
  QTextEdit* editor;
  QTextDocument* doc;
  QPixmap* buffer;
  unsigned pos;
  unsigned scrollStep;
  unsigned scrollTick;
  unsigned sideMargin;
};

MarqueeText::MarqueeText( QWidget* parent, const char* name ):
QFrame( parent )
{
  d = new MarqueeTextPrivate;
  d->scrollStep = 4;
  d->scrollTick = 200;
  d->sideMargin = 20;

  d->buffer = new QPixmap;
  d->editor = new QTextEdit( this );
  d->editor->hide();
  d->doc = d->editor->document();
  d->pos = 50;

  setFrameStyle( Panel | Sunken );
  setLineWidth( 1 );

  setMinimumSize( 300, 150 );

  QTimer* scrollTimer = new QTimer( this );
  connect( scrollTimer, SIGNAL( timeout() ), this, SLOT( scroll() ) );
  scrollTimer->start( d->scrollTick );
  
  setBackgroundRole( QPalette::Base );
}

MarqueeText::~MarqueeText()
{
  delete d->buffer;
  delete d;
}

QSizePolicy MarqueeText::sizePolicy() const
{
  return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}

void MarqueeText::setText( const QString& text )
{
  d->doc->clear();
  d->doc->setHtml( text );
  layout();
  d->pos = height();
  update();
}

void MarqueeText::paintEvent( QPaintEvent *e )
{
  QPainter painter( this );
  painter.fillRect( this->rect(), painter.background().color() );
  painter.end();
  
  QFrame::paintEvent( e );
  
  if( d->buffer )
  {
    QPainter p( this );
    QPixmap pix = d->buffer->copy();
    QRegion clip( 2, 2, width()-4, height()-4 );
    p.setClipRegion( clip );
    p.drawPixmap( 0, d->pos, pix );
  }

}

void MarqueeText::resizeEvent( QResizeEvent *e )
{
  layout();
  e->accept();
  update();
}

void MarqueeText::scroll()
{
  d->pos -= d->scrollStep;
  int offset = d->pos + d->buffer->height();

  if( offset < 0 )
    d->pos = height() + 2*d->scrollStep;

  update();

}

void MarqueeText::layout()
{
  QList<QTextLayout*> layouts;

  int leading = fontMetrics().leading();
  double hh = 10;
  double lineWidth = width()-d->sideMargin;
  int c = 0;

  // layouting lines, also to find total height
  QTextBlock block = d->doc->begin();
  while( block.isValid() )
  {
    QTextLayout* textLayout = new QTextLayout( block );
    layouts.append( textLayout );

    textLayout->beginLayout();
    for( ; ; )
    {
      QTextLine line = textLayout->createLine();
      if( !line.isValid() )
        break;

      line.setLineWidth( lineWidth );
      double x = (lineWidth - line.naturalTextWidth())/2;
      line.setPosition( QPointF(x-3, hh) );
      hh += line.height()/2;
      hh += leading;
      c++;
    }
    textLayout->endLayout();

    block = block.next();
    hh += 10; // paragraph-spacing
  }

  // recreate off-screen buffer

  QPixmap* oldbuf = d->buffer;
  d->buffer = 0;
  delete oldbuf;

  QBrush bgbrush( palette().base() );
  d->buffer = new QPixmap( QSize(int(lineWidth+5), int(2*hh+50) ) );
  d->buffer->fill( bgbrush.color() );

  // paint the lines

  QPainter painter( d->buffer );
  double ypos = 0;
  for( int k = 0; k < layouts.count(); k++ )
  {
    QTextLayout* textLayout = layouts[k];
    if( !textLayout ) continue;
    for( int i = 0; i < textLayout->lineCount(); ++i)
    {
      QTextLine line = textLayout->lineAt( i );
      QPointF pos = QPointF( d->sideMargin/2, ypos );
      line.draw( &painter, pos, 0 );
      ypos +=line.height()/2;
      ypos += leading;
    }
    ypos += 10; // paragraph-spacing
  }
}

AboutBox::AboutBox( QWidget* parent ):
  QDialog( parent )
{
  setObjectName( "AboutBox" );

  QString website = tr( "http://speedcrunch.digitalfanatics.org");

  QLabel* infoLabel = new QLabel( this );
  QString info = "<b>";
  info += tr("SpeedCrunch version %1").arg( MAKE_STRING(SPEEDCRUNCH_VERSION) );
  info += "</b><br>";
  info += tr("Copyright (C) 2004-2005 Ariya Hidayat (ariya@kde.org)");
  info += "<br>";
  info += tr("Copyright (C) 2005-2006 Johan Thelin (e8johan@gmail.com)");
  info += "<br>";
  info += QString("<a href=\"%1\">%2</a>").arg(website).arg(website);
  infoLabel->setText( info );
  infoLabel->setOpenExternalLinks(true);

  QLabel* iconLabel = new QLabel( this );
  iconLabel->setPixmap( QPixmap( ":/crunch.png" ) );
  iconLabel->setAlignment( Qt::AlignVCenter | Qt::AlignRight );

  MarqueeText* marqueeText = new MarqueeText( this );
  QString msg = "<center>";
  msg += "<p><b>";
  msg += tr( "SpeedCrunch version %1").arg( MAKE_STRING(SPEEDCRUNCH_VERSION) );
  msg += "</b></p>";
  msg += QString( "<p>%1:<br>%2" ).arg( tr("Author") ).arg( "Ariya Hidayat" );
  msg += QString( "<p>%1:<br>%2" ).arg( tr("Maintainer") ).arg( "Johan Thelin" );
  msg += QString( "<p>%1:<br>%2" ).arg( tr("Based on idea from") ).arg( "Roberto Alsina" );
  msg += QString( "<p>%1:<br>%2" ).arg( tr("Special thanks to") ).arg( "Michael Pyne" );
  msg += QString( "<p>%1:<br>%2" ).arg( tr("Artworks are courtesy of") ).arg( "Kuswanto (aka Zeus)" );
  msg += QString( "<p>%1<br>" ).arg( tr("Thanks to:") );
  msg += "Damir Perisa<br>"
         "Melchior Franz<br>"
         "Maciek Borowka<br>"
         "Bernhard Schiffner<br>"
         "Mohamed Eldesoky<br>"
         "Anders Lund<br>"
         "Thomas Nagy<br>"
         "Helder Correia<br>"
         "Pieter Pareit<br>"
         "Thomas Luebking<br>"
         "Jean-Remy Falleri<br>"
         "Gary Cramblitt<br>"
         "Philippe Fremy<br>"
         "Vladimir Pouzanov<br>"
         "Derek O'Connor<br>"
         ;
  msg += QString( "<br>%1</p>" ).arg( tr("and many others.") );

  msg += QString( "<p>%1<br>" ).arg( tr("Translators:") );

  msg += QString( "%1: %2<br>" ).arg( tr("Czech") ).arg( "Blondak (blondak@neser.cz)" );
  msg += QString( "%1: %2<br>" ).arg( tr("French") ).arg( "Vibet Alexis (bevi@netcourrier.com)" );
  msg += QString( "%1: %2<br>" ).arg( tr("German") ).arg( "Damir Perisa (damir.perisa@solnet.ch)" );
  msg += QString( "%1: %2<br>" ).arg( tr("Indonesian") ).arg( "Yolla Indria (yolla.indria@gmail.com)" );
  msg += QString( "%1: %2<br>" ).arg( tr("Portuguese") ).arg( "Helder Correia (helder.pereira.correia@gmail.com)" );
  msg += QString( "%1: %2<br>" ).arg( tr("Portuguese BR") ).arg( "Henrique Pinto (henrique.pinto@kdemail.net)" );
  msg += QString( "%1: %2<br>" ).arg( tr("Russian") ).arg( "Alexey Kouznetsov (kouznetsov@phtf.stu.neva.ru)" );
  msg += QString( "%1: %2<br>" ).arg( tr("Italian") ).arg( "Francesco di Cugno (fdicugno@gmail.com)" );
  msg += QString( "%1: %2<br>" ).arg( tr("Norwegian") ).arg( "Lars Ivar Igesund (larsivar@igesund.net)" );
  msg += QString( "%1: %2<br>" ).arg( tr("Polish") ).arg( "Witold Wysota (wysota@wysota.eu.org)" );
  msg += QString( "%1: %2<br>" ).arg( tr("Spanish") ).arg( "Alejandro Villarreal (alexv86@gmail.com)" );
  msg += QString( "</p>" );

  msg += "<p>";
  msg += tr("This program is free software; you can redistribute it and/or "
    "modify it under the terms of the GNU General Public License "
    "as published by the Free Software Foundation; either version 2 "
    "of the License, or (at your option) any later version" );
  msg += "</p>";
  msg += "<p>";
  msg += tr("This program is distributed in the hope that it will be useful, "
    "but WITHOUT ANY WARRANTY; without even the implied warranty of "
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
    "GNU General Public License for more details.");
  msg += "</p>";
  msg +=  "<p>&nbsp;</p>";
  msg += QString( "<p>%1</p>" ).arg( tr("Visit http://speedcrunch.digitalfanatics.org for more information!") );
  marqueeText->setText( msg );

  QPushButton* okButton = new QPushButton(this);
  okButton->setObjectName( "OKButton" );
  okButton->setText( tr("OK") );
  QObject::connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

  QSpacerItem* buttonSpacer = new QSpacerItem( 20, 20,
    QSizePolicy::Expanding, QSizePolicy::Minimum);

  // --- layout handling ---

  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  QHBoxLayout* topLayout = new QHBoxLayout();
  QHBoxLayout* buttonLayout = new QHBoxLayout();

  mainLayout->addLayout( topLayout );
  mainLayout->addWidget( marqueeText );
  mainLayout->addLayout( buttonLayout );
  mainLayout->setStretchFactor( marqueeText, 1 );

  topLayout->addWidget( infoLabel );
  topLayout->addWidget( iconLabel );

  buttonLayout->addItem( buttonSpacer );
  buttonLayout->addWidget( okButton );


  setWindowTitle( tr("About SpeedCrunch") );
  resize( QSize(350, 200).expandedTo(minimumSizeHint()) );
  setMinimumSize( 300, 150 );
}



