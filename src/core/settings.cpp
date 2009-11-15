// This file is part of the SpeedCrunch project
// Copyright (C) 2004, 2005, 2007, 2008 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2005-2006 Johan Thelin <e8johan@gmail.com>
// Copyright (C) 2007-2009 Helder Correia <helder.pereira.correia@gmail.com>
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

#include "core/settings.hxx"

#include "3rdparty/binreloc/binreloc.h"
#include "math/floatconfig.h"

#include <QtCore/QDir>
#include <QtCore/QLocale>
#include <QtCore/QSettings>
#include <QtGui/QApplication>
#include <QtGui/QFont>

static Settings * s_settingsInstance = 0;
static char s_radixCharacter = 0;

static void s_deleteSettings()
{
    delete s_settingsInstance;
}

static QSettings * createQSettings( const QString & key );

Settings::Settings()
{
}

Settings * Settings::instance()
{
    if ( ! s_settingsInstance ) {
        s_settingsInstance = new Settings;
        s_settingsInstance->load();
        qAddPostRoutine( s_deleteSettings );
    }

    return s_settingsInstance;
}

void Settings::load()
{
    const char * KEY = "SpeedCrunch";

    QSettings * settings = createQSettings( KEY );
    if ( ! settings )
        return;

    QString key = QString::fromLatin1( KEY ) + QLatin1String("/General/");

    // angle mode special case
    QString angleUnitStr;
    angleUnitStr = settings->value( key + QLatin1String("AngleMode"), "r" ).toString();
    if ( angleUnitStr != QLatin1String("r") && angleUnitStr != QLatin1String("d") )
        angleUnit = 'r';
    else
        angleUnit = angleUnitStr.at( 0 ).toAscii();

    // radix character special case
    QString radixCharStr;
    radixCharStr = settings->value( key + QLatin1String("RadixCharacter"), 0 ).toString();
    setRadixCharacter( radixCharStr.at(0).toAscii() );

    autoCalc = settings->value( key + QLatin1String("AutoCalc"), true ).toBool();
    autoCompletion = settings->value( key + QLatin1String("AutoCompletion"), true ).toBool();
    historySave = settings->value( key + QLatin1String("HistorySave"), true ).toBool();
    language = settings->value( key + QLatin1String("Language"), "C" ).toString();
    variableSave = settings->value( key + QLatin1String("VariableSave"), true ).toBool();
    syntaxHighlighting = settings->value( key + QLatin1String("SyntaxHighlighting"),
                                          true ).toBool();
    systemTrayIconVisible = settings->value( key + QLatin1String("SystemTrayIconVisible"),
                                             false ).toBool();

    key = KEY + QLatin1String("/Format/");

    // format special case
    QString formatStr;
    formatStr = settings->value( key + QLatin1String("Type"), 'f' ).toString();
    if ( formatStr != "g" && formatStr != "f" && formatStr != "e" && formatStr != "n"
         && formatStr != "h" && formatStr != "o" && formatStr != "b" )
    {
        resultFormat = 'f';
    }
    else
        resultFormat = formatStr.at( 0 ).toAscii();

    resultPrecision = settings->value( key + QLatin1String("Precision"), -1 ).toInt();

    if ( resultPrecision > DECPRECISION )
        resultPrecision = DECPRECISION;

    key = KEY + QLatin1String("/Layout/");
    windowOnfullScreen = settings->value( key + QLatin1String("WindowOnFullScreen"),
                                          false ).toBool();
    historyDockVisible = settings->value( key + QLatin1String("HistoryDockVisible"),
                                          false ).toBool();
    keypadVisible = settings->value( key + QLatin1String("KeypadVisible"), true ).toBool();
    menuBarVisible = settings->value( key + QLatin1String("MenuBarVisible"), true ).toBool();
    statusBarVisible = settings->value( key + QLatin1String("StatusBarVisible"), false ).toBool();
    functionsDockVisible = settings->value( key + QLatin1String("FunctionsDockVisible"),
                                            false ).toBool();
    variablesDockVisible = settings->value( key + QLatin1String("VariablesDockVisible"),
                                            false ).toBool();
    mathBookDockVisible = settings->value( key + QLatin1String("MathBookDockVisible"),
                                           false ).toBool();
    constantsDockVisible = settings->value( key + QLatin1String("ConstantsDockVisible"),
                                            false ).toBool();
    windowAlwaysOnTop = settings->value( key + QLatin1String("WindowAlwaysOnTop"),
                                         false ).toBool();

    windowPosition = settings->value( key + QLatin1String("WindowPosition"), QPoint() ).toPoint();
    windowSize = settings->value( key + QLatin1String("WindowSize"), QSize(400, 300) ).toSize();
    windowState = settings->value( key + QLatin1String("State") ).toByteArray();
    displayFont = settings->value( key + QLatin1String("DisplayFont"), QFont().toString()
                                 ).toString();

    // load history
    key = KEY + QLatin1String("/History/");
    history.clear();
    int count = settings->value( key + QLatin1String("Count"), 0 ).toInt();
    for ( int i = 0; i < count; ++i ) {
        QString keyname = QString( key + QLatin1String("Expression%1") ).arg( i );
        QString str = settings->value( keyname ).toString();
        if ( ! str.isEmpty() ) {
            QString expr;
            for ( int c = 0; c < str.length(); c++ )
                if ( str.at(c) >= 32 )
                    expr.append( str.at(c) );
            history.append( expr );
        }
    }

    // load results
    historyResults.clear();
    for ( int i = 0; i < count; ++i ) {
        QString keyname = QString( key + QLatin1String("Expression%1Result") ).arg( i );
        QString str = settings->value( keyname ).toString();
        if ( ! str.isEmpty() )
            historyResults.append( str );
    }
    if ( history.count() != historyResults.count() ) {
        // avoid application crash because of new features with old settings files
        history.clear();
        historyResults.clear();
    }

    // load variables
    key = KEY + QLatin1String("/Variables/");
    variables.clear();
    settings->beginGroup( key );
    QStringList names = settings->childKeys();
    settings->endGroup();
    for ( int k = 0; k < names.count(); ++k ) {
        QString name = names.at( k );
        QString keyname = key + name;
        QString value = settings->value( keyname ).toString();
        // treat upper case escape code
        for ( int c = 0; c < name.length(); ++c ) {
            if ( name.at(c) == '_' ) {
                name.remove( c, 1 );
                if ( name.at(c) != '_' )
                    name[c] = name.at( c ).toUpper();
            }
        }
        // load
        if ( ! value.isEmpty()
             && name != QLatin1String("pi")
             && name != QLatin1String("phi")
             && name != QLatin1String("e") )
            variables.append( QString::fromLatin1("%1=%2").arg(name).arg(value) );
    }

    delete settings;
}

void Settings::save()
{
    const QString KEY = QString::fromLatin1( "SpeedCrunch" );

    QSettings * settings = createQSettings( KEY );
    if ( ! settings )
        return;

    int k, i; // used on loops
    QString key = KEY + QLatin1String( "/General/" );

    settings->setValue( key + QLatin1String("HistorySave"), historySave );
    settings->setValue( key + QLatin1String("VariableSave"), variableSave );
    settings->setValue( key + QLatin1String("AutoCompletion"), autoCompletion );
    settings->setValue( key + QLatin1String("AutoCalc"), autoCalc );
    settings->setValue( key + QLatin1String("SystemTrayIconVisible"), systemTrayIconVisible );
    settings->setValue( key + QLatin1String("SyntaxHighlighting"), syntaxHighlighting );
    settings->setValue( key + QLatin1String("Language"), language );

    settings->setValue( key + QLatin1String("AngleMode"), QString(QChar(angleUnit)) );

    char c = 'C';
    if ( s_radixCharacter != 0 )
        c = s_radixCharacter;
    settings->setValue( key + QLatin1String("RadixCharacter"), QString(QChar(c)) );

    key = KEY + QLatin1String("/Format/");

    settings->setValue( key + QLatin1String("Type"), QString(QChar(resultFormat)) );
    settings->setValue( key + QLatin1String("Precision"), resultPrecision );

    key = KEY + QLatin1String("/Layout/");

    settings->setValue( key + QLatin1String("MathBookDockVisible"), mathBookDockVisible );
    settings->setValue( key + QLatin1String("ConstantsDockVisible"), constantsDockVisible );
    settings->setValue( key + QLatin1String("FunctionsDockVisible"), functionsDockVisible );
    settings->setValue( key + QLatin1String("HistoryDockVisible"), historyDockVisible );
    settings->setValue( key + QLatin1String("WindowOnFullScreen"), windowOnfullScreen );
    settings->setValue( key + QLatin1String("KeypadVisible"), keypadVisible );
    settings->setValue( key + QLatin1String("MenuBarVisible"), menuBarVisible );
    settings->setValue( key + QLatin1String("StatusBarVisible"), statusBarVisible );
    settings->setValue( key + QLatin1String("VariablesDockVisible"), variablesDockVisible );
    settings->setValue( key + QLatin1String("WindowPosition"), windowPosition );
    settings->setValue( key + QLatin1String("WindowSize"), windowSize );
    settings->setValue( key + QLatin1String("WindowAlwaysOnTop"), windowAlwaysOnTop );
    settings->setValue( key + QLatin1String("State"), windowState );
    settings->setValue( key + QLatin1String("DisplayFont"), displayFont );

    // save history
    if ( historySave ) {
        key = KEY + QLatin1String("/History/");
        QStringList realHistory = history;
        QStringList realHistoryResults = historyResults;

        if ( history.count() > 100 ) {
            realHistory.clear();
            realHistoryResults.clear();
            unsigned start = history.count() - 100;
            for ( int j = start; j < history.count(); j++ ) {
                realHistory.append( history.at(j) );
                realHistoryResults.append( historyResults.at(j) );
            }
        }

        settings->beginGroup( key );
        QStringList hkeys = settings->childKeys();
        settings->endGroup();

        for ( k = 0; k < hkeys.count(); k++ ) {
            settings->remove( (key + QLatin1String("Expression%1")).arg(k) );
            settings->remove( (key + QLatin1String("Expression%1Result")).arg(k) );
        }

        settings->setValue( key + QLatin1String("/Count/"), (int)history.count() );
        for ( i = 0; i < realHistory.count(); i++ ) {
            settings->setValue( (key + QLatin1String("Expression%1")).arg(i), realHistory.at(i) );
            settings->setValue( (key + QLatin1String("Expression%1Result")).arg(i),
                                realHistoryResults.at(i) );
        }
    }

    // save variables
    if ( variableSave ) {
        key = KEY + QLatin1String( "/Variables/" );
        settings->beginGroup( key );
        QStringList vkeys = settings->childKeys();
        settings->endGroup();

        for ( k = 0; k < vkeys.count(); ++k )
            //settings->remove( (key + QLatin1String("%1")).arg(vkeys.at(k)) );
            settings->remove( key + vkeys.at(k) );

        for ( i = 0; i < variables.count(); i++ ) {
            QStringList s = variables[i].split( '=' );
            if ( s.count() == 2
                 && s.at(0) != QLatin1String("pi")
                 && s.at(0) != QLatin1String("phi")
                 && s.at(0) != QLatin1String("e") )
            {
                QString name = "";
                QString value = s.at( 1 );
                int length = s.at( 0 ).length();
                for ( int c = 0; c < length; ++c ) {
                    if ( s.at(0).at(c).isUpper() || s.at(0).at(c) == '_' ) {
                        name += '_';
                        name += s.at( 0 ).at( c ).toLower();
                    } else
                        name += s.at( 0 ).at( c );
                }
                //settings->setValue( QString(key + "%1").arg(name), value );
                settings->setValue( key + name, value );
            }
        }
    }

    delete settings;
}

char Settings::radixCharacter() const
{
    return s_radixCharacter == 0 ? QLocale().decimalPoint().toAscii() : s_radixCharacter;
}

bool Settings::isRadixCharacterAuto() const
{
    return s_radixCharacter == 0;
}

void Settings::setRadixCharacter( char c )
{
    s_radixCharacter = (c != ',' && c != '.') ? 0 : c;
};

QSettings * createQSettings( const QString & KEY )
{
    QSettings * settings = 0;

#ifdef Q_WS_WIN
#ifdef SPEEDCRUNCH_PORTABLE
    // Portable Windows version: settings are from INI file in same directory
    QString appPath = QApplication::applicationFilePath();
    int ii = appPath.lastIndexOf( '/' );
    if ( ii > 0)
        appPath.remove( ii, appPath.length() );
    QString iniFile = appPath + '/' + KEY + ".ini";
    settings = new QSettings( iniFile, QSettings::IniFormat );
#else
    // Regular Windows version:
    //   settings are from the registry HKEY_CURRENT_USER\Software\SpeedCrunch
    settings = new QSettings( QSettings::NativeFormat, QSettings::UserScope, KEY, KEY );
#endif // SPEEDCRUNCH_PORTABLE
#endif // Q_WS_WIN


#ifdef Q_WS_MAC
    settings = new QSettings( QSettings::NativeFormat, QSettings::UserScope, KEY, KEY );
#endif // Q_WS_MAC


#if defined (Q_WS_X11) || defined (Q_WS_QWS)
#ifdef SPEEDCRUNCH_PORTABLE
    // Portable X11 version: settings are from INI file in the same directory
    BrInitError error;
    if ( br_init( & error ) == 0 && error != BR_INIT_ERROR_DISABLED ) {
        qDebug( "Warning: BinReloc failed to initialize (error code %d)", error );
        qDebug( "Will fallback to hardcoded default path." );
    }

    const char * prefix = br_find_prefix( 0 );
    QString iniFile = QString( prefix ) + '/' + KEY + QLatin1String( ".ini" );
    free( prefix );
    settings = new QSettings( iniFile, QSettings::IniFormat );
#else
    // Regular Unix or Embedded Linux (not Mac) version: settings from $HOME/.conf/SpeedCrunch
    settings = new QSettings( QSettings::NativeFormat, QSettings::UserScope, KEY, KEY );
#endif // SPEEDCRUNCH_PORTABLE
#endif // Q_WS_X11 || Q_WS_QWS

    return settings;
}

