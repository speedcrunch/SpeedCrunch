// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
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

#ifndef GUI_RESULTDISPLAY_HXX
#define GUI_RESULTDISPLAY_HXX

#include <QtGui/QListWidget>

#include <memory>

class HNumber;
class QListWidgetItem;

class ResultDisplay : public QListWidget
{
    Q_OBJECT

public:
    ResultDisplay( QWidget * parent = 0, const char * name = 0 );
    ~ResultDisplay();

    void    append( const QString & expr, const HNumber & value );
    void    appendHistory( const QStringList & history, const QStringList & results );
    QString asText() const;
    int     count() const;
    bool    customAppearance() const;
    QColor  customBackgroundColor() const;
    QColor  customBackgroundAlternateColor() const;
    QColor  customTextColor() const;
    void    setCustomAppearance( bool );
    void    setCustomBackgroundColor( const QColor & base, const QColor & alternate );
    void    setCustomTextColor( const QColor & );

    static QString formatNumber( const HNumber & );

signals:
    void textCopied( const QString & );
    void textSelected( const QString & );

public slots:
    void clear();
    void scrollEnd();
    void handleResultFormatChange();
    void handleResultPrecisionChange();
    void handleRadixCharacterChange();

private slots:
    void copyToClipboard( QListWidgetItem * );
    void selectItem( QListWidgetItem * );
    void refresh();

private:
    struct Private;
    const std::auto_ptr<Private> d;

    ResultDisplay( const ResultDisplay & );
    ResultDisplay & operator=( const ResultDisplay & );
};

#endif

