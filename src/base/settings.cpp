/* This file is part of the SpeedCrunch project
   Copyright (C) 2004, 2005, 2007 Ariya Hidayat <ariya@kde.org>
                 2005-2006 Johan Thelin <e8johan@gmail.com>
                 2007 Helder Correia <helder.pereira.correia@gmail.com>

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

#include <base/settings.hxx>

#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QLocale> //refdp

#include <stdlib.h>
#include "floatconfig.h"

#define SETTINGSKEY "SpeedCrunch"

Settings* s_global_settings = 0;

QChar Settings::dot;                     //refdp
bool Settings::autoDetectDot;

static void deleteGlobalSettings()
{
    delete s_global_settings;
    s_global_settings = 0;
}

QChar Settings::decimalPoint() //refdp
{
  return self()->dot;
}

void Settings::setDecimalPoint(const QString& val) //refdp
{
  autoDetectDot = (val.length() != 1);
  dot = autoDetectDot? QLocale().decimalPoint() : val[0];
}


Settings::Settings()
{
//  angleMode       = "degree"; //refan
  angleMode       = Degree;
  saveHistory     = true;
  saveVariables   = true;
  autoComplete    = true;
  autoCalc        = true;
//  decimalPoint    = QString(); // refdp
  setDecimalPoint(QString());
  //language        = QString();
  minimizeToTray  = false;
  stayAlwaysOnTop = false;

  format        = 'g';
  decimalDigits = -1;

  showClearInputButton = true;
  showEvaluateButton   = true;
  showKeyPad           = true;
  showHistory          = false;
  showFunctions        = false;
  showVariables        = false;
  showConstants        = false;

  customAppearance       = false;
  customTextColor        = Qt::black;
  customBackgroundColor1 = Qt::white;
  customBackgroundColor2 = Qt::white;
  customErrorColor       = Qt::red;

  highlightNumberColor    = QColor(   0,   0, 127 );
  highlightFunctionColor  = QColor(  85,   0,   0 );
  highlightVariableColor  = QColor(   0,  85,   0 );
  matchedParenthesisColor = QColor( 255, 255, 183 );

  mainWindowSize        = QSize( 0, 0 );
  historyDockFloating   = false;
  historyDockLeft       = 0;
  historyDockTop        = 0;
  historyDockWidth      = 150;
  historyDockHeight     = 400;
  functionsDockFloating = false;
  functionsDockLeft     = 0;
  functionsDockTop      = 0;
  functionsDockWidth    = 150;
  functionsDockHeight   = 400;
  variablesDockFloating = false;
  variablesDockLeft     = 0;
  variablesDockTop      = 0;
  variablesDockWidth    = 150;
  variablesDockHeight   = 400;
  constantsDockFloating = false;
  constantsDockLeft     = 0;
  constantsDockTop      = 0;
  constantsDockWidth    = 150;
  constantsDockHeight   = 400;
}

void Settings::load()
{
  escape = "\\"; //reftbl
  QSettings settings( /*QSettings::IniFormat,*/ QSettings::UserScope, SETTINGSKEY, SETTINGSKEY );

  QString key = SETTINGSKEY;

//  angleMode      = settings.value( key + "/General/AngleMode",      "degree"  ).toString(); //refan
  angleMode = Radian;
  if (settings.value( key + "/General/AngleMode",      "degree"  ).toString() == "degree")
    angleMode = Degree;
  saveHistory    = settings.value( key + "/General/SaveHistory",    true      ).toBool();
  saveVariables  = settings.value( key + "/General/SaveVariables",  true      ).toBool();
  autoComplete   = settings.value( key + "/General/AutoComplete",   true      ).toBool();
  autoCalc       = settings.value( key + "/General/AutoCalc",       true      ).toBool();
  setDecimalPoint(settings.value( key + "/General/DecimalPoint",   QString() ).toString());
  //language       = settings.value( key + "/General/Language",       QString() ).toString();
  minimizeToTray = settings.value( key + "/General/MinimizeToTray", false     ).toBool();

  QString formatStr = settings.value( key + "/View/Format" ).toString();
  if ( formatStr == "Fixed" )       format = 'f';
  if ( formatStr == "Engineering" ) format = 'n';
  if ( formatStr == "Exp" )         format = 'e';
  if ( formatStr == "General" )     format = 'g';
  if ( formatStr == "Hexadecimal" ) format = 'h';
  if ( formatStr == "Octal" )       format = 'o';
  if ( formatStr == "Binary" )      format = 'b';
  decimalDigits = settings.value( key + "/View/DecimalDigits", -1 ).toInt();
  if ( decimalDigits > DECPRECISION )         decimalDigits = DECPRECISION;

  showClearInputButton = settings.value( key + "/Appearance/ShowClearInputButton", true ).toBool();
  showEvaluateButton   = settings.value( key + "/Appearance/ShowEvaluateButton",   true ).toBool();
  showKeyPad           = settings.value( key + "/Appearance/ShowKeyPad",           true ).toBool();

  showHistory   = settings.value( key + "/Appearance/ShowHistory",   false ).toBool();
  showFunctions = settings.value( key + "/Appearance/ShowFunctions", false ).toBool();
  showVariables = settings.value( key + "/Appearance/ShowVariables", false ).toBool();
  showConstants = settings.value( key + "/Appearance/ShowConstants", false ).toBool();

  customAppearance = settings.value(                    key + "/Appearance/CustomAppearance",       false              ).toBool();
  customFont.fromString( settings.value(                key + "/Appearance/CustomFont",             QFont().toString() ).toString() );
  customTextColor.setNamedColor( settings.value(        key + "/Appearance/CustomTextColor",        "#000000"          ).toString() );
  customBackgroundColor1.setNamedColor( settings.value( key + "/Appearance/CustomBackgroundColor1", "#ffffff"          ).toString() );
  customBackgroundColor2.setNamedColor( settings.value( key + "/Appearance/CustomBackgroundColor2", "#eeeeee"          ).toString() );
  customErrorColor.setNamedColor( settings.value(       key + "/Appearance/CustomErrorColor",       "red"              ).toString() );
  mainWindowSize = QSize( settings.value( key + "/Appearance/WindowWidth",  0 ).toInt(),
                          settings.value( key + "/Appearance/WindowHeight", 0 ).toInt() );

  key = SETTINGSKEY;
  key += "/MainWindow/";
  mainWindowState       = settings.value( key + "State"                        ).toByteArray();
  stayAlwaysOnTop       = settings.value( key + "StayAlwaysOnTop",       false ).toBool();
  historyDockFloating   = settings.value( key + "HistoryDockFloating",   false ).toBool();
  historyDockTop        = settings.value( key + "HistoryDockTop",        0     ).toInt();
  historyDockLeft       = settings.value( key + "HistoryDockLeft",       0     ).toInt();
  historyDockWidth      = settings.value( key + "HistoryDockWidth",      150   ).toInt();
  historyDockHeight     = settings.value( key + "HistoryDockHeight",     400   ).toInt();
  functionsDockFloating = settings.value( key + "FunctionsDockFloating", false ).toBool();
  functionsDockTop      = settings.value( key + "FunctionsDockTop",      0     ).toInt();
  functionsDockLeft     = settings.value( key + "FunctionsDockLeft",     0     ).toInt();
  functionsDockWidth    = settings.value( key + "FunctionsDockWidth",    150   ).toInt();
  functionsDockHeight   = settings.value( key + "FunctionsDockHeight",   400   ).toInt();
  variablesDockFloating = settings.value( key + "VariablesDockFloating", false ).toBool();
  variablesDockTop      = settings.value( key + "VariablesDockTop",      0     ).toInt();
  variablesDockLeft     = settings.value( key + "VariablesDockLeft",     0     ).toInt();
  variablesDockWidth    = settings.value( key + "VariablesDockWidth",    150   ).toInt();
  variablesDockHeight   = settings.value( key + "VariablesDockHeight",   400   ).toInt();
  constantsDockFloating = settings.value( key + "ConstantsDockFloating", false ).toBool();
  constantsDockTop      = settings.value( key + "ConstantsDockTop",      0     ).toInt();
  constantsDockLeft     = settings.value( key + "ConstantsDockLeft",     0     ).toInt();
  constantsDockWidth    = settings.value( key + "ConstantsDockWidth",    150   ).toInt();
  constantsDockHeight   = settings.value( key + "ConstantsDockHeight",   400   ).toInt();

  key = SETTINGSKEY;
  key += "/SyntaxHighlight/";
  enableSyntaxHighlight = settings.value( key + "EnableSyntaxHighlight", true ).toBool();

  highlightNumberColor.setNamedColor(    settings.value( key + "NumberColor",             "#00007f" ).toString() );
  highlightFunctionColor.setNamedColor(  settings.value( key + "FunctionColor",           "#550000" ).toString() );
  highlightVariableColor.setNamedColor(  settings.value( key + "VariableColor",           "#005500" ).toString() );
  matchedParenthesisColor.setNamedColor( settings.value( key + "MatchedParenthesisColor", "#ffffb7" ).toString() );

  key = SETTINGSKEY;

  // load history
  history.clear();
  int count = settings.value( key + "/History/Count", 0 ).toInt();
  for ( int i = 0; i < count; i++ )
  {
    QString keyname = QString( "%1/History/Expression%2" ).arg( key ).arg( i );
    QString str = settings.value( keyname ).toString();
    if ( !str.isEmpty() )
    {
      QString expr;
      for ( int c = 0; c < str.length(); c++ )
        if ( str[c] >= 32 )
					expr.append( str[c] );
      history.append( expr );
    }
  }

  // load variables
  variables.clear();
  settings.beginGroup( key + "/Variables" );
  QStringList names = settings.childKeys();
  settings.endGroup();
  for ( int k = 0; k < names.count(); k++ )
  {
    QString keyname = QString( "%1/Variables/%2" ).arg( key ).arg( names[k] );
    QString value = settings.value( keyname ).toString();
    // treat upper case escape code
    QString name = names[k];
    int length = name.length();
    for ( int c = 0; c < length; c++ )
      if ( name[c] == '_' )
      {
	name.remove( c, 1 );
	name[c] = name[c].toUpper();
      }
    // load
    if ( ! value.isEmpty() )
      variables.append( QString( "%1=%2" ).arg( name ).arg( value ) );
  }
}

void Settings::save()
{
  QSettings settings( /*QSettings::IniFormat,*/ QSettings::UserScope, SETTINGSKEY, SETTINGSKEY );
  int k, i;

  QString key = SETTINGSKEY;

  settings.setValue( key + "/General/AngleMode",      angleMode      );
  settings.setValue( key + "/General/SaveHistory",    saveHistory    );
  settings.setValue( key + "/General/SaveVariables",  saveVariables  );
  settings.setValue( key + "/General/AutoComplete",   autoComplete   );
  settings.setValue( key + "/General/AutoCalc",       autoCalc       );
  settings.setValue( key + "/General/DecimalPoint",   autoDetectDot? QString(): decimalPoint() );
  //settings.setValue( key + "/General/Language",       language       );
  settings.setValue( key + "/General/MinimizeToTray", minimizeToTray );

  QString formatStr;
  if ( format == 'f' ) formatStr = "Fixed";
  if ( format == 'n' ) formatStr = "Engineering";
  if ( format == 'e' ) formatStr = "Exp";
  if ( format == 'g' ) formatStr = "General";
  if ( format == 'h' ) formatStr = "Hexadecimal";
  if ( format == 'o' ) formatStr = "Octal";
  if ( format == 'b' ) formatStr = "Binary";
  settings.setValue( key + "/View/Format",        formatStr     );
  settings.setValue( key + "/View/DecimalDigits", decimalDigits );

  settings.setValue( key + "/Appearance/ShowClearInputButton", showClearInputButton );
  settings.setValue( key + "/Appearance/ShowEvaluateButton",   showEvaluateButton   );
  settings.setValue( key + "/Appearance/ShowKeyPad",           showKeyPad           );
  settings.setValue( key + "/Appearance/ShowHistory",          showHistory          );
  settings.setValue( key + "/Appearance/ShowFunctions",        showFunctions        );
  settings.setValue( key + "/Appearance/ShowVariables",        showVariables        );
  settings.setValue( key + "/Appearance/ShowConstants",        showConstants        );

  settings.setValue( key + "/Appearance/CustomAppearance",       customAppearance              );
  settings.setValue( key + "/Appearance/CustomFont",             customFont.toString()         );
  settings.setValue( key + "/Appearance/CustomTextColor",        customTextColor.name()        );
  settings.setValue( key + "/Appearance/CustomBackgroundColor1", customBackgroundColor1.name() );
  settings.setValue( key + "/Appearance/CustomBackgroundColor2", customBackgroundColor2.name() );
  settings.setValue( key + "/Appearance/CustomErrorColor",       customErrorColor.name()       );
  settings.setValue( key + "/Appearance/WindowWidth",            mainWindowSize.width()        ),
  settings.setValue( key + "/Appearance/WindowHeight",           mainWindowSize.height()       );

  settings.setValue( key + "/MainWindow/State",                 mainWindowState       );
  settings.setValue( key + "/MainWindow/StayAlwaysOnTop",       stayAlwaysOnTop       );
  settings.setValue( key + "/MainWindow/HistoryDockFloating",   historyDockFloating   );
  settings.setValue( key + "/MainWindow/HistoryDockTop",        historyDockTop        );
  settings.setValue( key + "/MainWindow/HistoryDockLeft",       historyDockLeft       );
  settings.setValue( key + "/MainWindow/HistoryDockWidth",      historyDockWidth      );
  settings.setValue( key + "/MainWindow/HistoryDockHeight",     historyDockHeight     );
  settings.setValue( key + "/MainWindow/FunctionsDockFloating", functionsDockFloating );
  settings.setValue( key + "/MainWindow/FunctionsDockTop",      functionsDockTop      );
  settings.setValue( key + "/MainWindow/FunctionsDockLeft",     functionsDockLeft     );
  settings.setValue( key + "/MainWindow/FunctionsDockWidth",    functionsDockWidth    );
  settings.setValue( key + "/MainWindow/FunctionsDockHeight",   functionsDockHeight   );
  settings.setValue( key + "/MainWindow/VariablesDockFloating", variablesDockFloating );
  settings.setValue( key + "/MainWindow/VariablesDockTop",      variablesDockTop      );
  settings.setValue( key + "/MainWindow/VariablesDockLeft",     variablesDockLeft     );
  settings.setValue( key + "/MainWindow/VariablesDockWidth",    variablesDockWidth    );
  settings.setValue( key + "/MainWindow/VariablesDockHeight",   variablesDockHeight   );
  settings.setValue( key + "/MainWindow/ConstantsDockFloating", constantsDockFloating );
  settings.setValue( key + "/MainWindow/ConstantsDockTop",      constantsDockTop      );
  settings.setValue( key + "/MainWindow/ConstantsDockLeft",     constantsDockLeft     );
  settings.setValue( key + "/MainWindow/ConstantsDockWidth",    constantsDockWidth    );
  settings.setValue( key + "/MainWindow/ConstantsDockHeight",   constantsDockHeight   );

  key = SETTINGSKEY;
  key += "/SyntaxHighlight/";
  settings.setValue( key + "/EnableSyntaxHighlight",   enableSyntaxHighlight          );
  settings.setValue( key + "/NumberColor",             highlightNumberColor.name()    );
  settings.setValue( key + "/FunctionColor",           highlightFunctionColor.name()  );
  settings.setValue( key + "/VariableColor",           highlightVariableColor.name()  );
  settings.setValue( key + "/MatchedParenthesisColor", matchedParenthesisColor.name() );
  key = SETTINGSKEY;

  // save history
  QStringList realHistory = history;
  if ( history.count() > 100 )
  {
    realHistory.clear();
    unsigned start = history.count() - 100;
    for ( int j = start; j < history.count(); j++ )
      realHistory.append( history[j] );
  }

  settings.beginGroup( key + "/History" );
  QStringList hkeys = settings.childKeys();
  settings.endGroup();

  for ( k = 0; k < hkeys.count(); k++ )
    settings.remove( QString("%1/History/Expression%2").arg(key).arg(k) );

  settings.setValue( key + "/History/Count", (int)history.count() );
  for ( i = 0; i < realHistory.count(); i++ )
    settings.setValue( QString("%1/History/Expression%2").arg(key).arg(i),
                       realHistory[i] );

  // save variables
  settings.beginGroup( key + "/Variables" );
  QStringList vkeys = settings.childKeys();
  settings.endGroup();

  for ( k = 0; k < vkeys.count(); k++ )
    settings.remove( QString("%1/Variables/%2").arg(key).arg(vkeys[k]) );

  for ( i = 0; i < variables.count(); i++ )
  {
    QStringList s = variables[i].split( '=' );

    if ( s.count() == 2 )
      settings.setValue( QString("%1/Variables/%2").arg(key).arg(s[0]), s[1] );
  }
}

Settings* Settings::self()
{
  if ( !s_global_settings )
  {
    s_global_settings = new Settings();
    qAddPostRoutine( deleteGlobalSettings );
  }
  return s_global_settings;
}
