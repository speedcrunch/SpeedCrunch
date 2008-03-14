// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2005-2006 Johan Thelin <e8johan@gmail.com>
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


#include "resultdisplay.hxx"

#include "base/functions.hxx"
#include "math/hmath.hxx"

#include <QApplication>
#include <QClipboard>
#include <QLocale>
#include <QResizeEvent>
#include <QScrollBar>
#include <QTimer>


struct ResultDisplay::Private
{
  QStringList contents;
  int         count;
  bool        customAppearance;
  QColor      customBackgroundColor1;
  QColor      customBackgroundColor2;
  QColor      customErrorColor;
  QColor      customTextColor;
  char        format;
  int         precision;
  char        radixChar;
};


class BaseItem : public QListWidgetItem
{
  public:
    BaseItem( ResultDisplay * r ) : QListWidgetItem( r ) {}
    virtual void updateItem() {}
};


class ExprItem : public BaseItem
{
  public:
    ExprItem( ResultDisplay * r, int i, const QString & e )
      : BaseItem( r )
    {
      expr = e;
      result = r;
      index = i;
      setTextAlignment( Qt::AlignLeft );
      setText( e );
      //printf( "%s\n", qPrintable(e) );
      updateItem();
    }

    void updateItem()
    {
      if ( result->customAppearance() )
      {
        setForeground( result->customTextColor() );
        setBackground( (index & 1) ? result->customBackgroundColor1()
                                   : result->customBackgroundColor2() );
      }
      else
      {
        const QPalette & pal = result->palette();
        setForeground( pal.text() );
        setBackground( (index & 1) ? pal.base() : pal.alternateBase() );
      }
    }

  private:
    QString         expr;
    int             index;
    ResultDisplay * result;
};


class ResultDisplayItem : public BaseItem
{
  public:
    ResultDisplayItem( ResultDisplay * r, int i, const HNumber & v )
      : BaseItem( r )
    {
      value = v;
      result = r;
      index = i;
      setTextAlignment( Qt::AlignRight );
      setText( result->formatNumber( value ) );
      //printf( "%s\n", qPrintable(text()) );
      updateItem();
    }

    void updateItem()
    {
      setText( result->formatNumber( value ) );

      if ( result->customAppearance() )
      {
        setForeground( result->customTextColor() );
        setBackground( (index & 1) ? result->customBackgroundColor1() :
        result->customBackgroundColor2() );
      }
      else
      {
        const QPalette & pal = result->palette();
        setForeground( pal.text() );
        setBackground( (index & 1) ? pal.base() : pal.alternateBase() );
      }

    }

  private:
    int             index;
    HNumber         value;
    ResultDisplay * result;
};


class ErrorItem: public BaseItem
{
  public:
    ErrorItem( ResultDisplay * r, int i, const QString& m )
      : BaseItem( r )
    {
      msg = m;
      result = r;
      index = i;
      setTextAlignment( Qt::AlignRight );
      setText( m );
      //printf( "%s\n", qPrintable(msg) );
      updateItem();
    }

    void updateItem()
    {
      if( result->customAppearance() )
      {
        setForeground( result->customErrorColor() );
        setBackground( (index & 1) ? result->customBackgroundColor1()
                                   : result->customBackgroundColor2() );
      }
      else
      {
        const QPalette & pal = result->palette();
        setForeground( Qt::red );
        setBackground( (index & 1) ? pal.base() : pal.alternateBase() );
      }
    }

  private:
    int             index;
    QString         msg;
    ResultDisplay * result;
};


// public

ResultDisplay::ResultDisplay( char radixChar, char format, int precision,
                              QWidget * parent, const char * name )
  : QListWidget( parent ), d( new ResultDisplay::Private )
{
  if ( radixChar == 'C' )
    d->radixChar = QLocale().decimalPoint().toAscii();
  else
    d->radixChar = radixChar;

  d->format    = format;
  d->precision = precision;

  d->customAppearance = false;
  d->count = 0;

  d->customTextColor        = QColor( 255, 255, 255 );
  d->customBackgroundColor1 = QColor(   0,   0,   0 );
  d->customBackgroundColor2 = QColor(  21,  21,  21 );
  d->customErrorColor       = QColor( 255,   0,   0 );

  setObjectName( name );
  setBackgroundRole( QPalette::Base );

  setSelectionMode( NoSelection );
  setMinimumWidth( 150 );

  connect( this, SIGNAL( itemClicked( QListWidgetItem * ) ),
           this, SLOT( copyToClipboard( QListWidgetItem * ) ) );
  connect( this, SIGNAL( itemDoubleClicked( QListWidgetItem * ) ),
           this, SLOT( selectItem( QListWidgetItem * ) ) );

  setFocusPolicy( Qt::NoFocus );

  setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );
  setVerticalScrollMode  ( QAbstractItemView::ScrollPerPixel );

  setResizeMode( QListView::Adjust );

  // ensure scrollbars background get system background color
  QString colorName = QApplication::palette().background().color().name();
  verticalScrollBar()->setStyleSheet  ( QString("QVerticalScrollBar   { background: %1; }").arg( colorName ) );
  horizontalScrollBar()->setStyleSheet( QString("QHorizontalScrollBar { background: %1; }").arg( colorName ) );
}


QString ResultDisplay::asText() const
{
  return d->contents.join( "\n" );
}


void ResultDisplay::append( const QString & expr, const HNumber & value )
{
  ++d->count;

  new ExprItem(   this, d->count, expr );
  new ResultDisplayItem( this, d->count, value );

  d->contents.append( expr );
  d->contents.append( HMath::format( value ) );

  QTimer::singleShot( 0, this, SLOT( scrollEnd() ) );
}


void ResultDisplay::appendError( const QString & expr, const QString & msg )
{
  ++d->count;

  new ExprItem(  this, d->count, expr );
  new ErrorItem( this, d->count, msg );

  d->contents.append( expr );
  d->contents.append( msg );

  QTimer::singleShot( 0, this, SLOT( scrollEnd() ) );
}


void ResultDisplay::appendHistory( const QStringList & history,
                            const QStringList & results )
{
  for ( int i = 0 ; i < history.count(); i++ )
  {
    const char * resultStr = results[i].toAscii().data();
    HNumber result( resultStr );
    if ( results[i] == "NaN" || ! result.isNan() )
      append( history[i], result );
    else
      appendError( history[i], resultStr );
  }
}


int ResultDisplay::count() const
{
  return d->count;
}


bool ResultDisplay::customAppearance() const
{
  return d->customAppearance;
}


char ResultDisplay::format() const
{
  return d->format;
}


char ResultDisplay::radixChar() const
{
  return d->radixChar;
}


int ResultDisplay::precision() const
{
  return d->precision;
}


QColor ResultDisplay::customBackgroundColor1() const
{
  return d->customBackgroundColor1;
}


QColor ResultDisplay::customBackgroundColor2() const
{
  return d->customBackgroundColor2;
}


QColor ResultDisplay::customErrorColor() const
{
  return d->customErrorColor;
}


QColor ResultDisplay::customTextColor() const
{
  return d->customTextColor;
}


QString ResultDisplay::formatNumber( const HNumber & value ) const
{
  char * str = HMath::format( value, value.format() ?
                                       value.format() : d->format, d->precision );
  QString s = QString::fromLatin1( str );
  free( str );
  if ( d->radixChar == ',' )
    s.replace( '.', ',' );
  return s;
}


void ResultDisplay::setCustomAppearance( bool custom )
{
  d->customAppearance = custom;
  QColor bgcolor = QApplication::palette().base().color();
  if ( custom )
    bgcolor = customBackgroundColor1();

  QPalette pal = palette();
  pal.setColor( QPalette::Background, bgcolor );
  setPalette( pal );

  refresh();
}


void ResultDisplay::setCustomBackgroundColor( const QColor & bg1, const QColor & bg2 )
{
  d->customBackgroundColor1 = bg1;
  d->customBackgroundColor2 = bg2;
  if( d->customAppearance )
  {
    QPalette pal = palette();
    pal.setColor( QPalette::Base, bg1 );
    setPalette( pal );
  }
  refresh();
}


void ResultDisplay::setCustomErrorColor( const QColor & e )
{
  d->customErrorColor = e;
  refresh();
};


void ResultDisplay::setCustomTextColor( const QColor & c )
{
  d->customTextColor = c;
  refresh();
}


ResultDisplay::~ResultDisplay()
{
  delete d;
}


// public slots

void ResultDisplay::clear()
{
  d->count = 0;
  d->contents.clear();
  QListWidget::clear();
}


void ResultDisplay::scrollEnd()
{
  QScrollBar * hBar = horizontalScrollBar();
  hBar->setValue( hBar->maximum() );
  scrollToBottom();
}


void ResultDisplay::setFormat( char c )
{
  if ( d->format != c )
  {
    d->format = c;
    refresh();
  }
}


void ResultDisplay::setPrecision( int p )
{
  if ( d->precision != p )
  {
    d->precision = p;
    refresh();
  }
}


void ResultDisplay::setRadixChar( char c )
{
  if ( c == 'C' )
    c = QLocale().decimalPoint().toAscii();
  if ( d->radixChar != c )
  {
    d->radixChar = c;
  }
}


// private slots

void ResultDisplay::copyToClipboard( QListWidgetItem * item )
{
  if ( ! item )
    return;

  QClipboard * cb = QApplication::clipboard();
  cb->setText( item->text(), QClipboard::Clipboard );
  emit textCopied( item->text() );
}


void ResultDisplay::selectItem( QListWidgetItem * item )
{
  if ( ! item )
    return;

  copyToClipboard( item );
  emit textSelected( item->text() );
}


void ResultDisplay::refresh()
{
  for ( int c = 0; c < d->count * 2; c++ )
    dynamic_cast<BaseItem *>( item(c) )->updateItem();
}
