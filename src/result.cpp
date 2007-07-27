/* This file is part of the SpeedCrunch project
   Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
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
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
 */

#include "result.h"
#include "hmath.h"
#include "functions.h"

#include <QApplication>
#include <QClipboard>
#include <QResizeEvent>
#include <QScrollBar>
#include <QTimer>

class ResultPrivate
{
public:
  int count;
  QStringList contents;
  char format;
  int decimalDigits;
  QString decimalPoint;
  bool customAppearance;
  QColor customTextColor;
  QColor customBackgroundColor1;
  QColor customBackgroundColor2;
  QColor customErrorColor;
};

class BaseItem: public QListWidgetItem
{
  public:
    BaseItem( Result* r ): QListWidgetItem( r ) {}
    virtual void updateItem() {}
};

class ExprItem: public BaseItem
{
  public:
    ExprItem( Result* r, int i, const QString& e ): BaseItem( r )
    {
      expr = e;
      result = r;
      index = i;
      setTextAlignment( Qt::AlignLeft );
      setText( e );
      printf("%s\n", qPrintable(e));
      updateItem();
    }

    void updateItem()
    {
      if( result->customAppearance() )
      {
        setForeground( result->customTextColor() );
        setBackground( (index&1) ? result->customBackgroundColor1() :
          result->customBackgroundColor2() );
      }
      else
      {
        const QPalette& pal = result->palette();
        setForeground( pal.text() );
        setBackground( (index&1) ? pal.base() : pal.alternateBase() );
      }
    }
  private:
    Result* result;
    int index;
    QString expr;
};

class ResultItem: public BaseItem
{
  public:
    ResultItem( Result* r, int i,  const HNumber& v ): BaseItem( r )
    {
      value = v;
      result = r;
      index = i;
      setTextAlignment( Qt::AlignRight );
      setText( result->formatNumber( value ) );
      printf("%s\n", qPrintable(text()));
      updateItem();
    }

    void updateItem()
    {
      setText( result->formatNumber( value ) );
    {
      if( result->customAppearance() )
      {
        setForeground( result->customTextColor() );
        setBackground( (index&1) ? result->customBackgroundColor1() :
          result->customBackgroundColor2() );
      }
      else
      {
        const QPalette& pal = result->palette();
        setForeground( pal.text() );
        setBackground( (index&1) ? pal.base() : pal.alternateBase() );
      }
    }
    }
  private:
    Result* result;
    int index;
    HNumber value;
};

class ErrorItem: public BaseItem
{
  public:
    ErrorItem( Result* r, int i, const QString& m ): BaseItem( r )
    {
      msg = m;
      result = r;
      index = i;
      setTextAlignment( Qt::AlignRight );
      setText( m );
      printf("%s\n", qPrintable(msg));
      updateItem();
    }

    void updateItem()
    {
      if( result->customAppearance() )
      {
        setForeground( result->customErrorColor() );
        setBackground( (index&1) ? result->customBackgroundColor1() :
          result->customBackgroundColor2() );
      }
      else
      {
        const QPalette& pal = result->palette();
        setForeground( Qt::red );
        setBackground( (index&1) ? pal.base() : pal.alternateBase() );
      }
    }

  private:
    Result* result;
    int index;
    QString msg;
};


Result::Result( QWidget* parent, const char* name ): QListWidget( parent )
{
  d = new ResultPrivate;
  d->format = 'g';
  d->decimalDigits = -1;
  d->decimalPoint = QString();
  d->customAppearance = false;
  d->count = 0;

  d->customTextColor = Qt::black;
  d->customBackgroundColor1 = QColor( 255, 255, 255 );
  d->customBackgroundColor2 = QColor( 200, 200, 200 );
  d->customErrorColor = Qt::red;

  setObjectName( name );
  setBackgroundRole(QPalette::Base);

  setSelectionMode( NoSelection );
  setMinimumWidth( 150 );

  connect( this, SIGNAL( itemClicked( QListWidgetItem* ) ),
    this, SLOT( copyToClipboard( QListWidgetItem* ) ) );
}

Result::~Result()
{
  delete d;
}

void Result::append( const QString& expr, const HNumber& value )
{
  d->count++;

  new ExprItem( this, d->count, expr );
  new ResultItem( this, d->count, value );

  QTimer::singleShot( 0, this, SLOT(scrollEnd()) );
}

void Result::appendError( const QString& expr, const QString&  msg )
{
  d->count++;

  new ExprItem( this, d->count, expr );
  new ErrorItem( this, d->count, msg );

  QTimer::singleShot( 0, this, SLOT(scrollEnd()) );
}

QString Result::asText() const
{
  return d->contents.join( "\n" );
}

void Result::clear()
{
  d->count = 0;
  d->contents.clear();
  QListWidget::clear();
}

void Result::copyToClipboard( QListWidgetItem* item )
{
 if ( !item ) return;
 QClipboard *cb = QApplication::clipboard();
 cb->setText( item->text(), QClipboard::Clipboard );
 emit textCopied( item->text() );
}

void Result::resizeEvent( QResizeEvent* )
{
  triggerUpdate();
}

void Result::setFormat( char format )
{
  d->format = format;
  triggerUpdate();
}

char Result::format() const
{
  return d->format;
}

void Result::setDecimalDigits( int digits )
{
  d->decimalDigits = digits;
  triggerUpdate();
}

int Result::decimalDigits() const
{
  return d->decimalDigits;
}

void Result::setDecimalPoint( const QString& dp )
{
  d->decimalPoint = dp;
  triggerUpdate();
}

QString Result::decimalPoint() const
{
  return d->decimalPoint;
}

QString Result::formatNumber( const HNumber& value ) const
{
  char* str = HMath::format( value, value.format() ? value.format() : d->format, d->decimalDigits );
  QString s = QString::fromLatin1( str );
  free( str );
  if( d->decimalPoint.length() == 1 )
    for( int i = 0; i < s.length(); i++ )
      if( s[i] == '.' )
        s[i] = d->decimalPoint[0];
  return s;
}

void Result::triggerUpdate()
{
  for(int c = 0; c < count(); c++)
    dynamic_cast<BaseItem*>(item(c))->updateItem();
}

void Result::scrollEnd()
{
  scrollToItem( item( count()-1 ) );

  // this should work even on right-to-left layout
  QScrollBar* bar = horizontalScrollBar();
  bar->setValue( bar->maximum() );
}

void Result::setCustomAppearance( bool custom )
{
  d->customAppearance = custom;
  QColor bgcolor = QApplication::palette().base().color();
  if( custom ) bgcolor = customBackgroundColor1();

  QPalette pal = palette();
  pal.setColor( QPalette::Background, bgcolor);
  setPalette( pal );

  triggerUpdate();
}

bool Result::customAppearance() const
{
  return d->customAppearance;
}

void Result::setCustomTextColor( const QColor& c )
{
  d->customTextColor = c;
  triggerUpdate();
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
  {
    QPalette pal = palette();
    pal.setColor( QPalette::Background, bg1);
    setPalette( pal );
  }

  triggerUpdate();
}

QColor Result::customBackgroundColor1() const
{
  return d->customBackgroundColor1;
}

QColor Result::customBackgroundColor2() const
{
  return d->customBackgroundColor2;
}

void Result::setCustomErrorColor( const QColor& e )
{
  d->customErrorColor = e;
  triggerUpdate();
};

QColor Result::customErrorColor() const
{
  return d->customErrorColor;
}

