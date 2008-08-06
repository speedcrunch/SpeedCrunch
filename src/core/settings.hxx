// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2005-2006 Johan Thelin <e8johan@gmail.com>
// Copyright (C) 2007-2008 Helder Correia <helder.pereira.correia@gmail.com>
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


#ifndef SETTINGS_HXX
#define SETTINGS_HXX


#include <QColor>
#include <QFont>
#include <QRect>
#include <QSize>
#include <QString>
#include <QStringList>

class Settings
{
  public:
    // do not expose internals to the interface.
    // Most clients just want to know the radix char and do not
    // care a lot whether it is retrieved from the OS or not.
    char getRadixChar() const;
    // extensions for classes maintaining the radix char field.
    // The value 0 means: use locale settings
    void setRadixCharacter( char c = 0 ) { radixChar = c; };
    bool isLocaleRadixChar() const { return radixChar == 0; };

    char angleMode; // 'r': radian; 'd': degree
    char resultFormat;    // see HMath documentation
    int  resultPrecision; // see HMath documentation

    bool autoCalc;
    bool autoCompletion;
    bool systemTrayIconVisible;
    bool historySave;
    bool variableSave;
    bool mathBookDockVisible;
    bool constantsDockVisible;
    bool windowOnfullScreen;
    bool functionsDockVisible;
    bool historyDockVisible;
    bool keypadVisible;
    bool menuBarVisible;
    bool statusBarVisible;
    bool variablesDockVisible;
    bool windowAlwaysOnTop;
    bool syntaxHighlighting;

    QStringList history;
    QStringList historyResults;

    QString language;

    QPoint      windowPosition;
    QSize       windowSize;
    QByteArray  windowState;

    QStringList variables;

    //static QString escape; //reftbl

    Settings();
    void load();
    void save();

/*  FIXME
    formats need frequent access to the current settings, which should offer
    their data publicly. IMO, a singleton serves this idea better than
    a hidden instance in MainWindow.
    Classes like editor, evaluator, format can update their knowledge
    about the settings (radix char) at the beginning of an operation,
    and, technically, do not need a real-time update by the Qt messaging system.
    Of course, this does not hold for GUI elements. (wl)
    This is a workaround. */
    static Settings* settings; // maintained by the MainWindow (awkward)

  private:
    char radixChar; // 0: locale (default);
    Settings( const Settings & );
    Settings & operator=( const Settings & );
};

#endif
