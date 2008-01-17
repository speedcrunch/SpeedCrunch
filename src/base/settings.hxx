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


#include <qcolor.h>
#include <qfont.h>
#include <qrect.h>
#include <QSize>
#include <qstring.h>
#include <qstringlist.h>


class Settings
{
  public:
    typedef enum { Degree, Radian } AngleMode; //refan

    AngleMode    angleMode;
    bool         autoCalc;
    bool         autoComplete;
    static bool  autoDetectDot;
    bool         constantsDockFloating;
    int          constantsDockTop;
    int          constantsDockLeft;
    int          constantsDockWidth;
    int          constantsDockHeight;
    bool         customAppearance;
    QColor       customBackgroundColor1;
    QColor       customBackgroundColor2;
    QColor       customErrorColor;
    QFont        customFont;
    QColor       customTextColor;
    int          decimalDigits;
    bool         enableSyntaxHighlight;
    QString      escape; //reftbl
    char         format;
    bool         functionsDockFloating;
    int          functionsDockHeight;
    int          functionsDockLeft;
    int          functionsDockTop;
    int          functionsDockWidth;
    QColor       highlightFunctionColor;
    QColor       highlightNumberColor;
    QColor       highlightVariableColor;
    QStringList  history;
    bool         historyDockFloating;
    int          historyDockHeight;
    int          historyDockLeft;
    int          historyDockTop;
    int          historyDockWidth;
    QStringList  historyResults;
    //QString     language;
    QSize        mainWindowSize;
    QByteArray   mainWindowState;
    QColor       matchedParenthesisColor;
    bool         minimizeToTray;
    bool         saveSession;
    bool         saveVariables;
    bool         showClearInputButton;
    bool         showConstants;
    bool         showEvaluateButton;
    bool         showFunctions;
    bool         showHistory;
    bool         showKeypad;
    bool         showVariables;
    bool         stayAlwaysOnTop;
    QStringList  variables;
    bool         variablesDockFloating;
    int          variablesDockTop;
    int          variablesDockHeight;
    int          variablesDockLeft;
    int          variablesDockWidth;

    static QChar      decimalPoint();
    void              load();
    void              save();
    static Settings * self();
    void              setDecimalPoint( const QString & val);

  private:
    static QChar dot; //refdp

    Settings();
    Settings( const Settings & );
    Settings & operator=( const Settings & );
};


#endif
