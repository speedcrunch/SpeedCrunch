// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
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


#ifndef RESULT_HXX
#define RESULT_HXX


#include <QListWidget>
#include <QResizeEvent>


struct ResultPrivate;

class HNumber;

class QListWidgetItem;


class Result : public QListWidget
{
  Q_OBJECT

  public:
    Result( char radixChar = 'C', char format = 'g', int precision = -1,
            QWidget * parent = 0, const char * name = 0 );
    ~Result();

    void    append( const QString & expr, const HNumber & value );
    void    appendError( const QString & expr, const QString & msg );
    void    setCustomAppearance( bool );
    void    setCustomBackgroundColor( const QColor & bg1, const QColor & bg2 );
    void    setCustomErrorColor( const QColor & );
    void    setCustomTextColor( const QColor & );

    bool    customAppearance() const;
    char    format() const;
    char    radixChar() const;
    int     count() const;
    int     precision() const;
    QColor  customBackgroundColor1() const;
    QColor  customBackgroundColor2() const;
    QColor  customErrorColor() const;
    QColor  customTextColor() const;
    QString asText() const;
    QString formatNumber( const HNumber & ) const;

  signals:
    void textCopied( const QString & );

  public slots:
    void clear();
    void scrollEnd();
    void setFormat( char );
    void setPrecision( int );
    void setRadixChar( char );

  private slots:
    void copyToClipboard( QListWidgetItem * );
    void refresh();

  private:
    ResultPrivate * const d;
    Result( const Result & );
    Result & operator=( const Result & );
};


#endif
