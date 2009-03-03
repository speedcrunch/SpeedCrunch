// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2005-2006 Johan Thelin <e8johan@gmail.com>
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2008-2009 Helder Correia <helder.pereira.correia@gmail.com>
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

#include "3rdparty/flickcharm/flickcharm.h"
#include "gui/resultdisplay.hxx"

#include "core/functions.hxx"
#include "core/settings.hxx"
#include "math/hmath.hxx"
#include "math/floatconfig.h"

#include <QApplication>
#include <QClipboard>
#include <QScrollBar>
#include <QTimer>

struct ResultDisplay::Private
{
  QStringList contents;
  int         count;
  bool        customAppearance;
  QColor      customBackgroundColor;
  QColor      customBackgroundAlternateColor;
  QColor      customTextColor;
  FlickCharm  flickCharm;
};

class BaseItem : public QListWidgetItem
{
  public:
    BaseItem( ResultDisplay * r ) : QListWidgetItem( r ) {}
    virtual void updateItem() {}
};

class ExpressionItem : public BaseItem
{
  public:
    ExpressionItem( ResultDisplay * r, int i, const QString & e )
      : BaseItem( r ), expr( e ), index( i ), result( r )
    {
      setTextAlignment( Qt::AlignLeft );
      QFont f = font();
      f.setItalic( true );
      f.setPointSize( f.pointSize() + 1 );
      setFont( f );
      setText( expr );
      updateItem();
    }

    void updateItem()
    {
      if ( result->customAppearance() ) {
        setForeground( result->customTextColor() );
        setBackground( result->customBackgroundColor());
      } else {
        const QPalette & pal = result->palette();
        setForeground( pal.text() );
        setBackground( pal.base() );
      }
    }

  private:
    QString         expr;
    int             index;
    ResultDisplay * result;
};

class ResultItem : public BaseItem
{
  public:
    ResultItem( ResultDisplay * r, int i, const HNumber & v )
      : BaseItem( r ), index( i ), result( r ), value( v )
    {
      setTextAlignment( Qt::AlignLeft );
      QFont f = font();
      f.setBold( true );
      f.setPointSize( f.pointSize() + 2 );
      setFont( f );
      updateItem();
    }

    void updateItem()
    {
      setText( QString( "    =  " ) + ResultDisplay::formatNumber( value ) );
      if ( result->customAppearance() ) {
        setForeground( result->customTextColor() );
        setBackground( result->customBackgroundColor() );
      } else {
        const QPalette & pal = result->palette();
        setForeground( pal.text() );
        setBackground( pal.alternateBase() );
      }
    }

  private:
    int             index;
    ResultDisplay * result;
    HNumber         value;
};

ResultDisplay::ResultDisplay( QWidget * parent, const char * name )
  : QListWidget( parent ), d( new ResultDisplay::Private )
{
  d->customAppearance = false;
  d->count = 0;
  d->customTextColor = QColor( 255, 255, 255 );
  d->customBackgroundColor = QColor( 0, 0, 0 );
  d->customBackgroundAlternateColor = QColor( 21,  21,  21 );
  d->flickCharm.activateOn( this );

  setBackgroundRole( QPalette::Base );
  setCursor( QCursor(Qt::PointingHandCursor) );
  setFocusPolicy( Qt::NoFocus );
  setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );
  setLayoutDirection( Qt::LeftToRight );
  setMinimumWidth( 150 );
  setObjectName( name );
  setResizeMode( QListView::Adjust );
  setSelectionMode( NoSelection );
  setSpacing( 5 );
  setVerticalScrollMode( QAbstractItemView::ScrollPerPixel );

  connect( this, SIGNAL( itemClicked( QListWidgetItem * ) ),
           SLOT( copyToClipboard( QListWidgetItem * ) ) );
  connect( this, SIGNAL( itemDoubleClicked( QListWidgetItem * ) ),
           SLOT( selectItem( QListWidgetItem * ) ) );
}

QString ResultDisplay::asText() const
{
  return d->contents.join( "\n" );
}

void ResultDisplay::append( const QString & expr, const HNumber & value )
{
  ++d->count;

  new ExpressionItem( this, d->count, expr );
  new ResultItem( this, d->count, value );

  d->contents.append( expr );
  char* str = HMath::format( value, 'e', DECPRECISION );
  d->contents.append( str );
  free(str);

  QTimer::singleShot( 0, this, SLOT( scrollEnd() ) );
}

void ResultDisplay::appendHistory( const QStringList & history, const QStringList & results )
{
  for ( int i = 0 ; i < history.count(); i++ )
  {
    QByteArray a = results.at(i).toLatin1();
    const char * resultStr = a.constData();

    HNumber result( resultStr );
    if ( results.at(i) == "NaN" || ! result.isNan() )
      append( history.at(i), result );
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

QColor ResultDisplay::customBackgroundColor() const
{
  return d->customBackgroundColor;
}

QColor ResultDisplay::customBackgroundAlternateColor() const
{
  return d->customBackgroundAlternateColor;
}

QColor ResultDisplay::customTextColor() const
{
  return d->customTextColor;
}

QString ResultDisplay::formatNumber( const HNumber & value )
{
  Settings * settings = Settings::instance();
  char format = value.format() ? value.format() : settings->resultFormat;
  char * str = HMath::format( value, format, settings->resultPrecision );
  QString s = QString::fromLatin1( str );
  free( str );
  if ( settings->radixCharacter() != '.' )
    s.replace( '.', settings->radixCharacter() );
  return s;
}

void ResultDisplay::setCustomAppearance( bool custom )
{
  d->customAppearance = custom;
  QColor bgcolor = QApplication::palette().base().color();
  if ( custom )
    bgcolor = customBackgroundColor();

  QPalette pal = palette();
  pal.setColor( QPalette::Background, bgcolor );
  setPalette( pal );

  refresh();
}

void ResultDisplay::setCustomBackgroundColor( const QColor & base, const QColor & alternate )
{
  d->customBackgroundColor = base;
  d->customBackgroundAlternateColor = alternate;
  if( d->customAppearance )
  {
    QPalette pal = palette();
    pal.setColor( QPalette::Base, base );
    setPalette( pal );
  }
  refresh();
}

void ResultDisplay::setCustomTextColor( const QColor & c )
{
  d->customTextColor = c;
  refresh();
}

ResultDisplay::~ResultDisplay()
{
}

void ResultDisplay::clear()
{
  d->count = 0;
  d->contents.clear();
  QListWidget::clear();
}

void ResultDisplay::scrollEnd()
{
  scrollToBottom();
}

void ResultDisplay::handleResultFormatChange()
{
  refresh();
}

void ResultDisplay::handleResultPrecisionChange()
{
  refresh();
}

void ResultDisplay::handleRadixCharacterChange()
{
  refresh();
}

void ResultDisplay::copyToClipboard( QListWidgetItem * item )
{
  if ( ! item )
    return;

  QClipboard * cb = QApplication::clipboard();
  QString text = item->text().remove("=").trimmed();
  cb->setText( text, QClipboard::Clipboard );
  emit textCopied( text );
}


void ResultDisplay::selectItem( QListWidgetItem * item )
{
  if ( ! item )
    return;

  QString text = item->text().trimmed();
  copyToClipboard( item );
  emit textSelected( text );
}


void ResultDisplay::refresh()
{
  for ( int c = 0; c < d->count * 2; c++ )
    dynamic_cast<BaseItem *>( item(c) )->updateItem();
}

