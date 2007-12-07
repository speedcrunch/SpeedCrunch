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
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
 */


#ifndef SETTINGS
#define SETTINGS

#include <qcolor.h>
#include <qfont.h>
#include <qrect.h>
#include <qstring.h>
#include <qstringlist.h>
#include <QSize>

class Settings
{
  public:

    QString angleMode;
//    QString decimalPoint; //refdp
    static QChar decimalPoint();
    void setDecimalPoint(const QString& val);

    bool saveHistory;
    bool saveVariables;
    bool autoComplete;
    bool autoCalc;
    //QString language;
    bool minimizeToTray;

    char format;
    int decimalDigits;

    bool showClearInputButton;
    bool showEvaluateButton;
    bool showKeyPad;
    bool showHistory;
    bool showFunctions;
    bool showVariables;
    bool showConstants;
    bool stayAlwaysOnTop;

    bool customAppearance;
    QFont customFont;
    QColor customTextColor;
    QColor customBackgroundColor1;
    QColor customBackgroundColor2;
    QColor customErrorColor;

    bool enableSyntaxHighlight;
    QColor highlightNumberColor;
    QColor highlightFunctionColor;
    QColor highlightVariableColor;
    QColor matchedParenthesisColor;

    QStringList history;
    QStringList variables;

    QSize mainWindowSize;
    QByteArray mainWindowState;
    bool historyDockFloating;
    int historyDockTop;
    int historyDockLeft;
    int historyDockWidth;
    int historyDockHeight;
    bool functionsDockFloating;
    int functionsDockTop;
    int functionsDockLeft;
    int functionsDockWidth;
    int functionsDockHeight;
    bool variablesDockFloating;
    int variablesDockTop;
    int variablesDockLeft;
    int variablesDockWidth;
    int variablesDockHeight;
    bool constantsDockFloating;
    int constantsDockTop;
    int constantsDockLeft;
    int constantsDockWidth;
    int constantsDockHeight;

    static Settings* self();

    void load();
    void save();

  private:
    bool autoPoint;
    QChar point; //refdp
    Settings();
    Settings( const Settings& );
    Settings& operator=( const Settings& );
};

#endif // SETTINGS
