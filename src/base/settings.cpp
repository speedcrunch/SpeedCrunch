// This file is part of the SpeedCrunch project
// Copyright (C) 2004, 2005, 2007 Ariya Hidayat <ariya@kde.org>
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


#include <base/settings.hxx>
#include <math/floatconfig.h>

#include <QApplication>
#include <QDir>
#include <QLocale>
#include <QSettings>

#include <stdlib.h>


#define SETTINGSKEY "SpeedCrunch"


Settings * s_global_settings = 0;
QChar      Settings::dot;
bool       Settings::autoDetectDot;


static void deleteGlobalSettings()
{
    delete s_global_settings;
    s_global_settings = 0;
}


// public

QChar Settings::decimalPoint()
{
  return self()->dot;
}


void Settings::load()
{
  QSettings settings( /*QSettings::IniFormat,*/ QSettings::UserScope, SETTINGSKEY, SETTINGSKEY );

  QString key = SETTINGSKEY;

  angleMode = Radian;
  if ( settings.value( key + "/General/AngleMode", "Radian" ).toString() == "Degree")
    angleMode = Degree;
  saveSession    = settings.value( key + "/General/RestoreLastSession", true      ).toBool();
  saveVariables  = settings.value( key + "/General/SaveVariables",      true      ).toBool();
  autoComplete   = settings.value( key + "/General/AutoComplete",       true      ).toBool();
  autoCalc       = settings.value( key + "/General/AutoCalc",           true      ).toBool();
  setDecimalPoint( settings.value( key + "/General/DecimalPoint",       QString() ).toString());
  //language       = settings.value( key + "/General/Language",           QString() ).toString();
  minimizeToTray = settings.value( key + "/General/MinimizeToTray",     false     ).toBool();

  QString formatStr = settings.value( key + "/View/Format" ).toString();
  if ( formatStr == "Fixed" )       format = 'f';
  if ( formatStr == "Engineering" ) format = 'n';
  if ( formatStr == "Scientific" )  format = 'e';
  if ( formatStr == "General" )     format = 'g';
  if ( formatStr == "Hexadecimal" ) format = 'h';
  if ( formatStr == "Octal" )       format = 'o';
  if ( formatStr == "Binary" )      format = 'b';
  decimalDigits = settings.value( key + "/View/DecimalDigits", -1 ).toInt();
  if ( decimalDigits > DECPRECISION )
    decimalDigits = DECPRECISION;

  showFullScreen       = settings.value( key + "/Appearance/ShowFullScreen", false ).toBool();
  showKeypad           = settings.value( key + "/Appearance/ShowKeypad",     true  ).toBool();
  showMenuBar          = settings.value( key + "/Appearance/ShowMenuBar",    true  ).toBool();

  showHistory   = settings.value( key + "/Appearance/ShowHistory",   false ).toBool();
  showFunctions = settings.value( key + "/Appearance/ShowFunctions", false ).toBool();
  showVariables = settings.value( key + "/Appearance/ShowVariables", false ).toBool();
  showConstants = settings.value( key + "/Appearance/ShowConstants", false ).toBool();

  customAppearance = settings.value                   ( key + "/Appearance/CustomAppearance",       true               ).toBool();
  customFont.fromString( settings.value               ( key + "/Appearance/CustomFont",             QFont().toString() ).toString() );
  customTextColor.setNamedColor( settings.value       ( key + "/Appearance/CustomTextColor",        "#9acd32"          ).toString() );
  customBackgroundColor1.setNamedColor( settings.value( key + "/Appearance/CustomBackgroundColor1", "#000000"          ).toString() );
  customBackgroundColor2.setNamedColor( settings.value( key + "/Appearance/CustomBackgroundColor2", "#111417"          ).toString() );
  customErrorColor.setNamedColor( settings.value      ( key + "/Appearance/CustomErrorColor",       "#ff8c00"          ).toString() );
  mainWindowSize = QSize( settings.value              ( key + "/Appearance/WindowWidth",            350                ).toInt(),
                          settings.value              ( key + "/Appearance/WindowHeight",           400                ).toInt() );

  key = SETTINGSKEY;
  key += "/MainWindow/";
  mainWindowState       = settings.value( key + "State"                        ).toByteArray();
  stayAlwaysOnTop       = settings.value( key + "StayAlwaysOnTop",       false ).toBool();
  historyDockFloating   = settings.value( key + "HistoryDockFloating",   false ).toBool();
  historyDockTop        = settings.value( key + "HistoryDockTop",        0     ).toInt();
  historyDockLeft       = settings.value( key + "HistoryDockLeft",       0     ).toInt();
  historyDockWidth      = settings.value( key + "HistoryDockWidth",      150   ).toInt();
  historyDockHeight     = settings.value( key + "HistoryDockHeight",     350   ).toInt();
  functionsDockFloating = settings.value( key + "FunctionsDockFloating", false ).toBool();
  functionsDockTop      = settings.value( key + "FunctionsDockTop",      0     ).toInt();
  functionsDockLeft     = settings.value( key + "FunctionsDockLeft",     0     ).toInt();
  functionsDockWidth    = settings.value( key + "FunctionsDockWidth",    150   ).toInt();
  functionsDockHeight   = settings.value( key + "FunctionsDockHeight",   350   ).toInt();
  variablesDockFloating = settings.value( key + "VariablesDockFloating", false ).toBool();
  variablesDockTop      = settings.value( key + "VariablesDockTop",      0     ).toInt();
  variablesDockLeft     = settings.value( key + "VariablesDockLeft",     0     ).toInt();
  variablesDockWidth    = settings.value( key + "VariablesDockWidth",    150   ).toInt();
  variablesDockHeight   = settings.value( key + "VariablesDockHeight",   350   ).toInt();
  constantsDockFloating = settings.value( key + "ConstantsDockFloating", false ).toBool();
  constantsDockTop      = settings.value( key + "ConstantsDockTop",      0     ).toInt();
  constantsDockLeft     = settings.value( key + "ConstantsDockLeft",     0     ).toInt();
  constantsDockWidth    = settings.value( key + "ConstantsDockWidth",    150   ).toInt();
  constantsDockHeight   = settings.value( key + "ConstantsDockHeight",   350   ).toInt();

  key = SETTINGSKEY;
  key += "/SyntaxHighlight/";
  enableSyntaxHighlight = settings.value( key + "EnableSyntaxHighlight", true ).toBool();

  highlightNumberColor.setNamedColor   ( settings.value( key + "NumberColor",             "#00007f" ).toString() );
  highlightFunctionColor.setNamedColor ( settings.value( key + "FunctionColor",           "#550000" ).toString() );
  highlightVariableColor.setNamedColor ( settings.value( key + "VariableColor",           "#005500" ).toString() );
  matchedParenthesisColor.setNamedColor( settings.value( key + "MatchedParenthesisColor", "#ffffb7" ).toString() );

  key = SETTINGSKEY;

  // load history
  history.clear();
  int count = settings.value( key + "/History/Count", 0 ).toInt();
  for ( int i = 0; i < count; i++ )
  {
    QString keyname = QString( key + "/History/Expression%1" ).arg( i );
    QString str = settings.value( keyname ).toString();
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
    QString keyname = QString( key + "/History/Expression%1Result" ).arg( i );
    QString str = settings.value( keyname ).toString();
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
  variables.clear();
  settings.beginGroup( key + "/Variables" );
  QStringList names = settings.childKeys();
  settings.endGroup();
  for ( int k = 0; k < names.count(); k++ )
  {
    QString name = names[k];
    QString keyname = QString( key + "/Variables/%1" ).arg( name );
    QString value = settings.value( keyname ).toString();
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
}


void Settings::save()
{
  QSettings settings( /*QSettings::IniFormat,*/ QSettings::UserScope, SETTINGSKEY, SETTINGSKEY );
  int k, i;

  QString key = SETTINGSKEY;

  QString angleStr;
  if      ( angleMode == Settings::Radian ) angleStr = "Radian";
  else if ( angleMode == Settings::Degree ) angleStr = "Degree";
  settings.setValue( key + "/General/AngleMode",          angleStr      );
  settings.setValue( key + "/General/RestoreLastSession", saveSession   );
  settings.setValue( key + "/General/SaveVariables",      saveVariables );
  settings.setValue( key + "/General/AutoComplete",       autoComplete  );
  settings.setValue( key + "/General/AutoCalc",           autoCalc      );
  settings.setValue( key + "/General/DecimalPoint",       autoDetectDot? QString(): decimalPoint() );
  //settings.setValue( key + "/General/Language",           language      );
  settings.setValue( key + "/General/MinimizeToTray",     minimizeToTray );

  QString formatStr;
  if      ( format == 'f' ) formatStr = "Fixed";
  else if ( format == 'n' ) formatStr = "Engineering";
  else if ( format == 'e' ) formatStr = "Scientific";
  else if ( format == 'g' ) formatStr = "General";
  else if ( format == 'h' ) formatStr = "Hexadecimal";
  else if ( format == 'o' ) formatStr = "Octal";
  else if ( format == 'b' ) formatStr = "Binary";
  settings.setValue( key + "/View/Format",        formatStr     );
  settings.setValue( key + "/View/DecimalDigits", decimalDigits );

  settings.setValue( key + "/Appearance/ShowConstants",        showConstants        );
  settings.setValue( key + "/Appearance/ShowFunctions",        showFunctions        );
  settings.setValue( key + "/Appearance/ShowHistory",          showHistory          );
  settings.setValue( key + "/Appearance/ShowFullScreen",       showFullScreen       );
  settings.setValue( key + "/Appearance/ShowKeypad",           showKeypad           );
  settings.setValue( key + "/Appearance/ShowMenuBar",          showMenuBar          );
  settings.setValue( key + "/Appearance/ShowVariables",        showVariables        );

  settings.setValue( key + "/Appearance/CustomAppearance",       customAppearance              );
  settings.setValue( key + "/Appearance/CustomBackgroundColor1", customBackgroundColor1.name() );
  settings.setValue( key + "/Appearance/CustomBackgroundColor2", customBackgroundColor2.name() );
  settings.setValue( key + "/Appearance/CustomErrorColor",       customErrorColor.name()       );
  settings.setValue( key + "/Appearance/CustomFont",             customFont.toString()         );
  settings.setValue( key + "/Appearance/CustomTextColor",        customTextColor.name()        );
  settings.setValue( key + "/Appearance/WindowHeight",           mainWindowSize.height()       );
  settings.setValue( key + "/Appearance/WindowWidth",            mainWindowSize.width()        ),

  settings.setValue( key + "/MainWindow/ConstantsDockFloating", constantsDockFloating );
  settings.setValue( key + "/MainWindow/ConstantsDockHeight",   constantsDockHeight   );
  settings.setValue( key + "/MainWindow/ConstantsDockLeft",     constantsDockLeft     );
  settings.setValue( key + "/MainWindow/ConstantsDockTop",      constantsDockTop      );
  settings.setValue( key + "/MainWindow/ConstantsDockWidth",    constantsDockWidth    );
  settings.setValue( key + "/MainWindow/FunctionsDockFloating", functionsDockFloating );
  settings.setValue( key + "/MainWindow/FunctionsDockHeight",   functionsDockHeight   );
  settings.setValue( key + "/MainWindow/FunctionsDockLeft",     functionsDockLeft     );
  settings.setValue( key + "/MainWindow/FunctionsDockTop",      functionsDockTop      );
  settings.setValue( key + "/MainWindow/FunctionsDockWidth",    functionsDockWidth    );
  settings.setValue( key + "/MainWindow/HistoryDockFloating",   historyDockFloating   );
  settings.setValue( key + "/MainWindow/HistoryDockHeight",     historyDockHeight     );
  settings.setValue( key + "/MainWindow/HistoryDockLeft",       historyDockLeft       );
  settings.setValue( key + "/MainWindow/HistoryDockTop",        historyDockTop        );
  settings.setValue( key + "/MainWindow/HistoryDockWidth",      historyDockWidth      );
  settings.setValue( key + "/MainWindow/State",                 mainWindowState       );
  settings.setValue( key + "/MainWindow/StayAlwaysOnTop",       stayAlwaysOnTop       );
  settings.setValue( key + "/MainWindow/VariablesDockFloating", variablesDockFloating );
  settings.setValue( key + "/MainWindow/VariablesDockHeight",   variablesDockHeight   );
  settings.setValue( key + "/MainWindow/VariablesDockLeft",     variablesDockLeft     );
  settings.setValue( key + "/MainWindow/VariablesDockTop",      variablesDockTop      );
  settings.setValue( key + "/MainWindow/VariablesDockWidth",    variablesDockWidth    );

  key = SETTINGSKEY;
  key += "/SyntaxHighlight/";
  settings.setValue( key + "/EnableSyntaxHighlight",   enableSyntaxHighlight          );
  settings.setValue( key + "/FunctionColor",           highlightFunctionColor.name()  );
  settings.setValue( key + "/MatchedParenthesisColor", matchedParenthesisColor.name() );
  settings.setValue( key + "/NumberColor",             highlightNumberColor.name()    );
  settings.setValue( key + "/VariableColor",           highlightVariableColor.name()  );
  key = SETTINGSKEY;

  // save history
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

  settings.beginGroup( key + "/History" );
  QStringList hkeys = settings.childKeys();
  settings.endGroup();

  for ( k = 0; k < hkeys.count(); k++ )
  {
    settings.remove( QString( key + "/History/Expression%1" ).arg( k ) );
    settings.remove( QString( key + "/History/Expression%1Result" ).arg( k ) );
  }

  settings.setValue( key + "/History/Count", (int) history.count() );
  for ( i = 0; i < realHistory.count(); i++ )
  {
    settings.setValue( QString( key + "/History/Expression%1" ).arg( i ), realHistory[i] );
    settings.setValue( QString( key + "/History/Expression%1Result" ).arg( i ), realHistoryResults[i] );
  }

  // save variables
  settings.beginGroup( key + "/Variables" );
  QStringList vkeys = settings.childKeys();
  settings.endGroup();

  for ( k = 0; k < vkeys.count(); k++ )
    settings.remove( QString( key + "/Variables/%1" ).arg( vkeys[k] ) );

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
        // bug in QT 4.2: subscript operator[] returns a reference type that
        // does not know about isUpper(). Use at() here, the better choice anyway.
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
      settings.setValue( QString( key + "/Variables/%1").arg( name ), value );
    }
  }
}


Settings* Settings::self()
{
  if ( ! s_global_settings )
  {
    s_global_settings = new Settings();
    qAddPostRoutine( deleteGlobalSettings );
  }
  return s_global_settings;
}


void Settings::setDecimalPoint( const QString & val )
{
  autoDetectDot = (val.length() != 1);
  dot = autoDetectDot? QLocale().decimalPoint() : val[0];
}


// private

Settings::Settings()
{
  angleMode       = Radian;
  autoCalc        = true;
  autoComplete    = true;
  minimizeToTray  = false;
  saveSession     = true;
  saveVariables   = true;
  setDecimalPoint( QString() );
  stayAlwaysOnTop = false;

  format        = 'g';
  decimalDigits = -1;

  customAppearance = true;
  showConstants    = false;
  showFunctions    = false;
  showHistory      = false;
  showFullScreen   = false;
  showKeypad       = true;
  showMenuBar      = true;
  showVariables    = false;

  customBackgroundColor1  = QColor(   0,   0,   0 );
  customBackgroundColor2  = QColor(  17,  20,  23 );
  customErrorColor        = QColor( 255, 140,   0 );
  customTextColor         = QColor( 154, 205,  50 );
  highlightFunctionColor  = QColor(  85,   0,   0 );
  highlightNumberColor    = QColor(   0,   0, 127 );
  highlightVariableColor  = QColor(   0,  85,   0 );
  matchedParenthesisColor = QColor( 255, 255, 183 );

  constantsDockFloating = false;
  constantsDockHeight   = 350;
  constantsDockLeft     = 0;
  constantsDockTop      = 0;
  constantsDockWidth    = 200;
  functionsDockFloating = false;
  functionsDockHeight   = 350;
  functionsDockLeft     = 0;
  functionsDockTop      = 0;
  functionsDockWidth    = 200;
  historyDockFloating   = false;
  historyDockHeight     = 350;
  historyDockLeft       = 0;
  historyDockTop        = 0;
  historyDockWidth      = 200;
  mainWindowSize        = QSize( 0, 0 );
  variablesDockFloating = false;
  variablesDockHeight   = 350;
  variablesDockLeft     = 0;
  variablesDockTop      = 0;
  variablesDockWidth    = 200;

  escape = "\\"; //reftbl
}
