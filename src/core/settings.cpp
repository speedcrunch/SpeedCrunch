// This file is part of the SpeedCrunch project
// Copyright (C) 2004, 2005, 2007, 2008 Ariya Hidayat <ariya@kde.org>
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

#include "core/settings.hxx"

#include "3rdparty/util/binreloc.h"
#include "math/floatconfig.h"

#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QLocale>

static Settings * sSettings = 0;
static int sCounter = 0;
static char sRadixCharacter = 0;
static QSettings * createQSettings( const QString & key );

Settings::Settings()
{
}

Settings::~Settings()
{
}

Settings * Settings::instance()
{
  if ( ! sSettings )
    sSettings = new Settings;
  ++sCounter;
  return sSettings;
}

void Settings::release()
{
  if ( sCounter == 1 )
  {
    sSettings = 0;
    sCounter = 0;
    delete sSettings;
  }
  else
    --sCounter;
}

void Settings::load()
{
  const QString KEY = "SpeedCrunch";

  QSettings * settings = createQSettings( KEY );
  if ( ! settings )
    return;

  QString key;

  key = KEY + "/General/";

  // angle mode special case
  QString angleUnitStr;
  angleUnitStr = settings->value( key + "AngleMode", "r" ).toString();
  if ( angleUnitStr != "r" && angleUnitStr != "d" )
    angleUnit = 'r';
  else
    angleUnit = angleUnitStr[0].toAscii();

  // radix character special case
  QString radixCharStr;
  radixCharStr = settings->value( key + "RadixCharacter", 0 ).toString();
  //if ( radixCharStr != "," && radixCharStr != "." )
  //  sRadixCharacter = 0;
  //else
  //  sRadixCharacter = radixCharStr[0].toAscii();
  setRadixCharacter( radixCharStr[0].toAscii() );

  historySave           = settings->value( key + "HistorySave",           true  ).toBool();
  variableSave          = settings->value( key + "VariableSave",          true  ).toBool();
  autoCompletion        = settings->value( key + "AutoCompletion",        true  ).toBool();
  autoCalc              = settings->value( key + "AutoCalc",              true  ).toBool();
  systemTrayIconVisible = settings->value( key + "SystemTrayIconVisible", false ).toBool();
  syntaxHighlighting    = settings->value( key + "SyntaxHighlighting",    true  ).toBool();
  language              = settings->value( key + "Language",              "C"   ).toString();

  key = KEY + "/Format/";

  // format special case
  QString formatStr;
  formatStr = settings->value( key + "Type", 'g' ).toString();
  if (    formatStr != "g" && formatStr != "f" && formatStr != "e"
       && formatStr != "n" && formatStr != "h" && formatStr != "o"
       && formatStr != "b" )
    resultFormat = 'g';
  else
    resultFormat = formatStr[0].toAscii();

  resultPrecision = settings->value( key + "Precision", -1  ).toInt();

  if ( resultPrecision > DECPRECISION )
    resultPrecision = DECPRECISION;

  key = KEY + "/Layout/";
  windowOnfullScreen   = settings->value( key + "WindowOnFullScreen",   false ).toBool();
  keypadVisible        = settings->value( key + "KeypadVisible",        true  ).toBool();
  menuBarVisible       = settings->value( key + "MenuBarVisible",       true  ).toBool();
  statusBarVisible     = settings->value( key + "StatusBarVisible",     false ).toBool();
  historyDockVisible   = settings->value( key + "HistoryDockVisible",   false ).toBool();
  functionsDockVisible = settings->value( key + "FunctionsDockVisible", false ).toBool();
  variablesDockVisible = settings->value( key + "VariablesDockVisible", false ).toBool();
  mathBookDockVisible  = settings->value( key + "MathBookDockVisible",  false ).toBool();
  constantsDockVisible = settings->value( key + "ConstantsDockVisible", false ).toBool();
  windowAlwaysOnTop    = settings->value( key + "WindowAlwaysOnTop",    false ).toBool();

  windowSize           = settings->value( key + "WindowSize",      QSize( 400, 300 ) ).toSize(),
  windowPosition       = settings->value( key + "WindowPosition",  QPoint()          ).toPoint();
  windowState          = settings->value( key + "State"                              ).toByteArray();

  // load history
  key = KEY + "/History/";
  history.clear();
  int count = settings->value( key + "Count", 0 ).toInt();
  for ( int i = 0; i < count; i++ )
  {
    QString keyname = QString( key + "Expression%1" ).arg( i );
    QString str = settings->value( keyname ).toString();
    if ( ! str.isEmpty() )
    {
      QString expr;
      for ( int c = 0; c < str.length(); c++ )
        if ( str[c] >= 32 )
          expr.append( str[c] );
      history.append( expr );
    }
  }
  // load results
  historyResults.clear();
  for ( int i = 0; i < count; i++ )
  {
    QString keyname = QString( key + "Expression%1Result" ).arg( i );
    QString str = settings->value( keyname ).toString();
    if ( ! str.isEmpty() )
      historyResults.append( str );
  }
  if ( history.count() != historyResults.count() )
  {
    // avoid application crash because of new features with old settings files
    history.clear();
    historyResults.clear();
  }

  // load variables
  key = KEY + "/Variables/";
  variables.clear();
  settings->beginGroup( key );
  QStringList names = settings->childKeys();
  settings->endGroup();
  for ( int k = 0; k < names.count(); k++ )
  {
    QString name = names[k];
    QString keyname = QString( key + "%1" ).arg( name );
    QString value = settings->value( keyname ).toString();
    // treat upper case escape code
    int length = name.length();
    for ( int c = 0; c < length; c++ )
    {
      if ( name[c] == '_' )
      {
        name.remove( c, 1 );
        if ( name[c] != '_' )
          name[c] = name[c].toUpper();
      }
    }
    // load
    if ( ! value.isEmpty() && name != "pi" && name != "phi" )
      variables.append( QString( "%1=%2" ).arg( name ).arg( value ) );
  }

  delete settings;
}

void Settings::save()
{
  const QString KEY = "SpeedCrunch";

  QSettings * settings = createQSettings( KEY );
  if ( ! settings )
    return;

  QString key;
  int k, i;

  key = KEY + "/General/";

  settings->setValue( key + "HistorySave",           historySave           );
  settings->setValue( key + "VariableSave",          variableSave          );
  settings->setValue( key + "AutoCompletion",        autoCompletion        );
  settings->setValue( key + "AutoCalc",              autoCalc              );
  settings->setValue( key + "SystemTrayIconVisible", systemTrayIconVisible );
  settings->setValue( key + "SyntaxHighlighting",    syntaxHighlighting    );
  settings->setValue( key + "Language",              language              );

  settings->setValue( key + "AngleMode", QString( QChar( angleUnit ) ) );

  char c = 'C';
  if ( sRadixCharacter != 0 )
    c = sRadixCharacter;
  settings->setValue( key + "RadixCharacter", QString( QChar( c ) ) );

  key = KEY + "/Format/";

  settings->setValue( key + "Type",      QString( QChar( resultFormat ) ) );
  settings->setValue( key + "Precision", resultPrecision );

  key = KEY + "/Layout/";

  settings->setValue( key + "MathBookDockVisible",  mathBookDockVisible  );
  settings->setValue( key + "ConstantsDockVisible", constantsDockVisible );
  settings->setValue( key + "FunctionsDockVisible", functionsDockVisible );
  settings->setValue( key + "HistoryDockVisible",   historyDockVisible   );
  settings->setValue( key + "WindowOnFullScreen",   windowOnfullScreen   );
  settings->setValue( key + "KeypadVisible",        keypadVisible        );
  settings->setValue( key + "MenuBarVisible",       menuBarVisible       );
  settings->setValue( key + "StatusBarVisible",     statusBarVisible     );
  settings->setValue( key + "VariablesDockVisible", variablesDockVisible );
  settings->setValue( key + "WindowPosition",       windowPosition       );
  settings->setValue( key + "WindowSize",           windowSize           );
  settings->setValue( key + "WindowAlwaysOnTop",    windowAlwaysOnTop    );
  settings->setValue( key + "State",                windowState          );

  // save history
  if ( historySave )
  {
    key = KEY + "/History/";
    QStringList realHistory = history;
    QStringList realHistoryResults = historyResults;

    if ( history.count() > 100 )
    {
      realHistory.clear();
      realHistoryResults.clear();
      unsigned start = history.count() - 100;
      for ( int j = start; j < history.count(); j++ )
      {
        realHistory.append( history[j] );
        realHistoryResults.append( historyResults[j] );
      }
    }

    settings->beginGroup( key );
    QStringList hkeys = settings->childKeys();
    settings->endGroup();

    for ( k = 0; k < hkeys.count(); k++ )
    {
      settings->remove( QString( key + "Expression%1" ).arg( k )       );
      settings->remove( QString( key + "Expression%1Result" ).arg( k ) );
    }

    settings->setValue( key + "/Count/", (int) history.count() );
    for ( i = 0; i < realHistory.count(); i++ )
    {
      settings->setValue( QString( key + "Expression%1" ).arg( i ),       realHistory[i]        );
      settings->setValue( QString( key + "Expression%1Result" ).arg( i ), realHistoryResults[i] );
    }
  }

  // save variables
  if ( variableSave )
  {
    key = KEY + "/Variables/";
    settings->beginGroup( key );
    QStringList vkeys = settings->childKeys();
    settings->endGroup();

    for ( k = 0; k < vkeys.count(); k++ )
      settings->remove( QString( key + "%1" ).arg( vkeys[k] ) );

    for ( i = 0; i < variables.count(); i++ )
    {
      QStringList s = variables[i].split( '=' );

      if ( s.count() == 2 && s[0] != "pi" && s[0] != "phi" )
      {
        QString name  = "";
        QString value = s[1];
        int length = s[0].length();
        for ( int c = 0; c < length; c++ )
        {
          if ( s[0].at(c).isUpper() || s[0].at(c) == '_' )
          {
            name += '_';
            name += s[0][c].toLower();
          }
          else
          {
            name += s[0][c];
          }
        }
        settings->setValue( QString( key + "%1").arg( name ), value );
      }
    }
  }

  delete settings;
}

char Settings::radixCharacter() const
{
  if ( sRadixCharacter == 0 )
    return QLocale().decimalPoint().toAscii();
  else
    return sRadixCharacter;
}

void Settings::setRadixCharacter( char c )
{
  if ( c != ',' && c != '.' )
    sRadixCharacter = 0;
  else
    sRadixCharacter = c;
};

bool Settings::isRadixCharacterAuto() const
{
  return sRadixCharacter == 0;
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
        appPath.remove(ii, appPath.length());
    QString iniFile = appPath + '/' + KEY + ".ini";
    settings = new QSettings( iniFile, QSettings::IniFormat );
  #else
    // Regular Windows version:
    //   settings are from the registry HKEY_CURRENT_USER\Software\SpeedCrunch
    settings = new QSettings( QSettings::NativeFormat, QSettings::UserScope,
                              KEY, KEY );
  #endif // SPEEDCRUNCH_PORTABLE
  #endif // Q_WS_WIN


  #ifdef Q_WS_MAC
    settings = new QSettings( QSettings::NativeFormat, QSettings::UserScope,
                              KEY, KEY );
  #endif // Q_WS_MAC


  #ifdef Q_WS_X11
  #ifdef SPEEDCRUNCH_PORTABLE
    // Portable X11 version: settings are from INI file in the same directory
    BrInitError error;
    if ( br_init( & error ) == 0 && error != BR_INIT_ERROR_DISABLED )
    {
        qDebug( "Warning: BinReloc failed to initialize (error code %d)",
                error );
        qDebug( "Will fallback to hardcoded default path." );
    }

    char * prefix = br_find_prefix( 0 );
    QString iniFile = QString( prefix ) + '/' + KEY + ".ini";
    free( prefix );
    settings = new QSettings( iniFile, QSettings::IniFormat );
  #else
    // Regular Unix (not Mac) version: settings from $HOME/.conf/SpeedCrunch
    settings = new QSettings( QSettings::NativeFormat, QSettings::UserScope,
                              KEY, KEY );
  #endif // SPEEDCRUNCH_PORTABLE
  #endif // Q_WS_X11

  return settings;
}

