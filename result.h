/* This file is part of the SpeedCrunch project
   Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>

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


#ifndef RESULT
#define RESULT

#include "hmath.h"
//Added by qt3to4:
#include <QResizeEvent>

class QWidget;
#include <qcolor.h>
#include <q3listbox.h>
class Q3ListBoxItem;

class ResultPrivate;

class Result : public Q3ListBox
{
  Q_OBJECT
  
  public:
    Result( QWidget* parent = 0, const char* name = 0 );
    ~Result();
    
    void append( const QString& expr, const HNumber& value );    
    void appendError( const QString& expr, const QString& msg );
    QString asText() const;
    
    void setFormat( char format );
    char format() const;
    void setDecimalDigits( int digits );
    int decimalDigits() const;
    QString formatNumber( const HNumber& value ) const;
    
    void setCustomAppearance( bool custom );
    bool customAppearance() const;
    void setCustomTextColor( const QColor& textColor );
    QColor customTextColor() const;
    void setCustomBackgroundColor( const QColor& bg1, const QColor& bg2 );
    QColor customBackgroundColor1() const;
    QColor customBackgroundColor2() const;
    
  signals:
    void textCopied( const QString& text );
    
  public slots:
    void clear();
    
  protected:  
    virtual void resizeEvent( QResizeEvent* );
    
  private slots:
    void scrollEnd();
    void itemClicked( Q3ListBoxItem* );
    
  private:
    ResultPrivate* d;
    Result( const Result& );
    Result& operator=( const Result& );
};

#endif // RESULT
