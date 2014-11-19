// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2005-2006 Johan Thelin <e8johan@gmail.com>
// Copyright (C) 2007-2013 Helder Correia <helder.pereira.correia@gmail.com>
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

#ifndef CORE_SETTINGS_H
#define CORE_SETTINGS_H

#include <QtCore/QPoint>
#include <QtCore/QSize>
#include <QtCore/QStringList>
#include <QtCore/QList>

// current application version (format to be comparable lexicographically) 
#define APP_VERSION_FULL "00.12.00.00"
// milestones for compatibility checks
#define APP_VERSION_00120000 "00.12.00.00"
// unknown format (always lexicographically <APP_VERSION_FULL) 
#define APP_VERSION_UNKN "00.00.00.00"

class Settings {
public:
    static Settings* instance();

    void load();
    void save();

    QString appVersion; // "00.00.00.00" format

    char radixCharacter() const; // 0: Automatic.
    void setRadixCharacter(char c = 0);
    bool isRadixCharacterAuto() const;

    bool parseAllRadixChar;
    bool strictDigitGrouping;

    char angleUnit; // 'r': radian; 'd': degree.

    char resultFormat; // See HMath documentation.
    int resultPrecision; // Ditto.

    bool autoAns;
    bool autoCalc;
    bool autoCompletion;
    int digitGrouping;
    bool historySave;
    bool leaveLastExpression;
    bool syntaxHighlighting;
    bool systemTrayIconVisible;
    bool variableSave;
    bool userFunctionSave;
    bool windowAlwaysOnTop;
    bool autoResultToClipboard;
    bool windowPositionSave;

    bool constantsDockVisible;
    bool functionsDockVisible;
    bool historyDockVisible;
    bool formulaBookDockVisible;
    bool menuBarVisible;
    bool statusBarVisible;
    bool variablesDockVisible;
    bool userFunctionsDockVisible;
    bool windowOnfullScreen;
    bool bitfieldVisible;

    int colorScheme;
    QString displayFont;

    QString language;

    QStringList history;
    QStringList historyResults;
    QStringList variables;
    QList<QStringList> userFunctions;

    QPoint windowPosition;
    QSize windowSize;
    QByteArray windowState;
    bool maximized;

private:
    Settings();
    Q_DISABLE_COPY(Settings);
};

#endif
