/* This file is part of the SpeedCrunch project
   Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
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
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "result.h"
#include "hmath.h"
#include "functions.h"

#include <qapplication.h>
#include <qclipboard.h>
#include <q3listbox.h>
#include <qpainter.h>
#include <qstring.h>
#include <qtimer.h>
//Added by qt3to4:
#include <QResizeEvent>

class ResultPrivate
{
public:
  int count;
  QStringList contents;
  char format;
  int decimalDigits;
  bool customAppearance;
  QColor customTextColor;
  QColor customBackgroundColor1;
  QColor customBackgroundColor2;
};

class ExprItem: public Q3ListBoxText
{
  public:
    ExprItem( Result*, int index, const QString& );
  protected:
    void paint( QPainter* p );
    virtual int width( const Q3ListBox* ) const;
    virtual int height( const Q3ListBox* ) const;
  private:
    Result* result;
    int index;
    QString expr;
};

class ResultItem: public Q3ListBoxText
{
  public:
    ResultItem( Result*, int index, const HNumber& );
    QString formattedText() const;
  protected:
    void paint( QPainter* p );
    virtual int width( const Q3ListBox* ) const;
    virtual int height( const Q3ListBox* ) const;
  private:
    Result* result;
    int index;
    HNumber value;
};

class ErrorItem: public Q3ListBoxText
{
  public:
    ErrorItem( Result*, int index, const QString& );
  protected:
    void paint( QPainter* p );
    virtual int width( const Q3ListBox* ) const;
    virtual int height( const Q3ListBox* ) const;
  private:
    Result* result;
    int index;
    QString msg;
};

ExprItem::ExprItem( Result* r, int i, const QString& e ):
  Q3ListBoxText ( r, e )
{
  result = r;
  index = i;
  expr = e;
}

void ExprItem::paint( QPainter* painter )
{
  painter->save();

  QColor bgcolor = result->colorGroup().base();
  if( index&1 ) bgcolor = bgcolor.dark( 120 );

  if( result->customAppearance() )
  {
    painter->setPen( result->customTextColor() );
    bgcolor = result->customBackgroundColor1();
    if( index&1 ) bgcolor = result->customBackgroundColor2();
  }

  QRect r ( 0, 0, listBox()->contentsWidth(), height( listBox() ) );
  painter->fillRect( r, bgcolor );

  int tf = Qt::NoAccel | Qt::SingleLine | Qt::AlignVCenter;
  r.addCoords( 5, 0, -5, 0 );
  painter->drawText( r, tf, expr );

  painter->restore();
}

int ExprItem::width( const Q3ListBox* lb ) const
{
  QFont font = lb->font();
  return QFontMetrics( font ).width( expr ) + 10;
}

int ExprItem::height( const Q3ListBox* lb ) const
{
  QFont font = lb->font();
  return QFontMetrics( font ).height() + 4;
}


ResultItem::ResultItem( Result* r, int i, const HNumber& v ):
  Q3ListBoxText ( r )
{
  result = r;
  index = i;
  value = v;
  setText( formattedText() );
}

QString ResultItem::formattedText() const
{
  return result->formatNumber( value );
}

void ResultItem::paint( QPainter* painter )
{
  painter->save();

  QColor bgcolor = result->colorGroup().base();
  if( index&1 ) bgcolor = bgcolor.dark( 120 );

  if( result->customAppearance() )
  {
    painter->setPen( result->customTextColor() );
    bgcolor = result->customBackgroundColor1();
    if( index&1 ) bgcolor = result->customBackgroundColor2();
  }

  QRect r ( 0, 0, listBox()->contentsWidth(), height( listBox() ) );
  painter->fillRect( r, bgcolor );

  int tf = Qt::NoAccel | Qt::SingleLine | Qt::AlignVCenter| Qt::AlignRight;
  QFont font = painter->font();
  font.setBold( true );
  painter->setFont( font );
  r.addCoords( 5, 0, -5, 0 );
  painter->drawText( r, tf, formattedText() );

  painter->restore();
}

int ResultItem::width( const Q3ListBox* lb ) const
{
  QFont font = lb->font();
  font.setBold( true );
  return QFontMetrics( font ).width( formattedText() ) + 10;
}

int ResultItem::height( const Q3ListBox* lb ) const
{
  QFont font = lb->font();
  font.setBold( true );
  return QFontMetrics( font ).height() + 4;
}

ErrorItem::ErrorItem( Result* r, int i, const QString& m ):
  Q3ListBoxText ( r, m )
{
  result = r;
  index = i;
  msg = m;
}

void ErrorItem::paint( QPainter* painter )
{
  painter->save();

  QColor bgcolor = result->colorGroup().base();
  if( index&1 ) bgcolor = bgcolor.dark( 120 );

  if( result->customAppearance() )
  {
    painter->setPen( result->customTextColor() );
    bgcolor = result->customBackgroundColor1();
    if( index&1 ) bgcolor = result->customBackgroundColor2();
  }

  QRect r ( 0, 0, listBox()->contentsWidth(), height( listBox() ) );
  painter->fillRect( r, bgcolor );

  int tf = Qt::NoAccel | Qt::SingleLine | Qt::AlignVCenter| Qt::AlignRight;
  QFont font = painter->font();
  font.setBold( true );

  QFontMetrics fm(font);
  QString dispMsg(msg);
  // as long as we can't dispaly msg (length > 4 is saveguard)
  while ( fm.width( dispMsg ) + 10 > r.width() && dispMsg.length() > 4 )
  {
    // move to last real char (!= '.') with i
    int i = dispMsg.length() - 1;
    while ( dispMsg[ i ] == '.' && i > 0 )
      --i;
    // create new dispMsg without that last char and add ...
    dispMsg = dispMsg.left( i ) + "...";
  }
  painter->setFont( font );
  r.addCoords( 5, 0, -5, 0 );
  painter->setPen( Qt::red );
  painter->drawText( r, tf, dispMsg );

  painter->restore();
}

int ErrorItem::width( const Q3ListBox* lb ) const
{
 // @todo: method can be removed allthougether, default implementation will do
       // error items have the tendency to grow to large in width
       // here we return the minimum with, in ErrorItem::paint() we'll
       // implement a routine that only uses the with given to display string
  return Q3ListBoxItem::width( lb );
}

int ErrorItem::height( const Q3ListBox* lb ) const
{
  QFont font = lb->font();
  font.setBold( true );
  return QFontMetrics( font ).height() + 4;
}


Result::Result( QWidget* parent, const char* name ):
  Q3ListBox( parent, name )
{
  d = new ResultPrivate;
  d->format = 'g';
  d->decimalDigits = -1;
  d->customAppearance = false;
  d->count = 0;

  setSelectionMode( NoSelection );
  setVariableWidth( true );
  setMinimumWidth( 150 );

  connect( this, SIGNAL( clicked( Q3ListBoxItem* ) ),
    this, SLOT( itemClicked( Q3ListBoxItem* ) ) );
}

Result::~Result()
{
  delete d;
}

void Result::append( const QString& expr, const HNumber& value )
{
  d->count++;

  new ExprItem( this, d->count, expr );
  ResultItem* item = new ResultItem( this, d->count, value );

  d->contents.append( expr );
  d->contents.append( item->formattedText() );

  QTimer::singleShot( 100, this, SLOT( scrollEnd() ) );

  printf( "%s\n", expr.latin1() );
  printf( "%s\n", item->formattedText().latin1() );
}

void Result::appendError( const QString& expr, const QString&  msg )
{
  d->count++;

  new ExprItem( this, d->count, expr );
  new ErrorItem( this, d->count, msg );

  d->contents.append( expr );
  d->contents.append( msg );

  QTimer::singleShot( 100, this, SLOT( scrollEnd() ) );

  printf( "%s\n", expr.latin1() );
  printf( "%s\n", msg.latin1() );
}

QString Result::asText() const
{
  return d->contents.join( "\n" );
}

void Result::clear()
{
  d->contents.clear();
  Q3ListBox::clear();
}

void Result::itemClicked( Q3ListBoxItem* item )
{
 if ( !item ) return;
 QClipboard *cb = QApplication::clipboard();
 cb->setText( item->text(), QClipboard::Clipboard );
 emit textCopied( item->text() );
}

void Result::scrollEnd()
{
  if( layoutDirection() == Qt::LeftToRight )
    ensureVisible( contentsWidth()-1, contentsHeight()-1 );
  else  
    ensureVisible( 0, contentsHeight()-1 );
}


void Result::resizeEvent( QResizeEvent* )
{
  triggerUpdate( true );
}

void Result::setFormat( char format )
{
  d->format = format;
  triggerUpdate( true );
}

char Result::format() const
{
  return d->format;
}

void Result::setDecimalDigits( int digits )
{
  d->decimalDigits = digits;
  triggerUpdate( true );
}

int Result::decimalDigits() const
{
  return d->decimalDigits;
}

QString Result::formatNumber( const HNumber& value ) const
{
  char* str = HMath::format( value, d->format, d->decimalDigits );
  QString s = QString::fromLatin1( str );
  free( str );
  return s;
}

void Result::setCustomAppearance( bool custom )
{
  d->customAppearance = custom;
  QColor bgcolor = QApplication::palette().active().base();
  if( custom ) bgcolor = customBackgroundColor1();
  setPaletteBackgroundColor( bgcolor );
  triggerUpdate( false );
}

bool Result::customAppearance() const
{
  return d->customAppearance;
}

void Result::setCustomTextColor( const QColor& c )
{
  d->customTextColor = c;
  triggerUpdate( false );
}

QColor Result::customTextColor() const
{
  return d->customTextColor;
}

void Result::setCustomBackgroundColor( const QColor& bg1, const QColor& bg2 )
{
  d->customBackgroundColor1 = bg1;
  d->customBackgroundColor2 = bg2;
  if( d->customAppearance )
    setPaletteBackgroundColor( bg1 );
  triggerUpdate( false );
}

QColor Result::customBackgroundColor1() const
{
  return d->customBackgroundColor1;
}

QColor Result::customBackgroundColor2() const
{
  return d->customBackgroundColor2;
}

