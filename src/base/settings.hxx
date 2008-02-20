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
    char        radixChar; // 'C': locale (default);
    char        angleMode; // 'r': radian; 'd': degree
    char        format;    // see HMath documentation
    int         precision; // see HMath documentation

    bool        autoCalc;
    bool        autoComplete;
    bool        minimizeToTray;
    bool        saveSession;
    bool        saveVariables;
    bool        showBook;
    bool        showConstants;
    bool        showFullScreen;
    bool        showFunctions;
    bool        showHistory;
    bool        showKeypad;
    bool        showMenuBar;
    bool        showVariables;
    bool        stayAlwaysOnTop;
    bool        hiliteSyntax;

    bool        bookDockFloating;
    int         bookDockTop;
    int         bookDockLeft;
    int         bookDockWidth;
    int         bookDockHeight;
    bool        constantsDockFloating;
    int         constantsDockTop;
    int         constantsDockLeft;
    int         constantsDockWidth;
    int         constantsDockHeight;
    bool        functionsDockFloating;
    int         functionsDockHeight;
    int         functionsDockLeft;
    int         functionsDockTop;
    int         functionsDockWidth;
    bool        historyDockFloating;
    int         historyDockHeight;
    int         historyDockLeft;
    int         historyDockTop;
    int         historyDockWidth;
    bool        variablesDockFloating;
    int         variablesDockTop;
    int         variablesDockHeight;
    int         variablesDockLeft;
    int         variablesDockWidth;

    QStringList history;
    QStringList historyResults;

    //QString     language;
    //
    QSize       mainWindowSize;
    QByteArray  mainWindowState;
    QStringList variables;

    //static QString escape; //reftbl

    Settings();
    void load();
    void save();

  private:
    Settings( const Settings & );
    Settings & operator=( const Settings & );
};


#endif
