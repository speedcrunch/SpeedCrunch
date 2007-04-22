/* This file is part of the SpeedCrunch project
   Copyright (C) 2004, 2005 Ariya Hidayat <ariya@kde.org>
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

#include "settings.h"

#include <qdir.h>
#include <qsettings.h>

#include <stdlib.h>

#define SETTINGSKEY "SpeedCrunch"

Settings* Settings::s_self = 0;

Settings::Settings()
{
  angleMode = "degree";
  saveHistory = true;
  saveVariables = true;
  autoComplete = true;
  autoCalc = true;
  decimalPoint = QString();
  minimizeToTray = false;

  format = 'g';
  decimalDigits = -1;

  showClearInputButton = true;
  showEvaluateButton = true;
  showKeyPad = true;
  showHistory = false;
  showFunctions = false;

  customAppearance = false;
  customTextColor = Qt::black;
  customBackgroundColor1 = Qt::white;
  customBackgroundColor2 = Qt::white;
  customErrorColor = Qt::red;

  highlightNumberColor = QColor(0,0,127);
  highlightFunctionColor = QColor(85,0,0);
  highlightVariableColor = QColor(0,85,0);
  matchedParenthesisColor = QColor(255,255,183);

  mainWindowSize = QSize( 0, 0 );
  historyDockFloating = false;
  historyDockLeft = 0;
  historyDockTop = 0;
  historyDockWidth = 150;
  historyDockHeight = 400;
  functionsDockFloating = false;
  functionsDockLeft = 0;
  functionsDockTop = 0;
  functionsDockWidth = 150;
  functionsDockHeight = 400;
}

void Settings::load()
{
  QSettings settings( QSettings::UserScope, SETTINGSKEY, SETTINGSKEY );

  QString key = SETTINGSKEY;

  angleMode = settings.readEntry( key + "/General/AngleMode", "degree" );
  saveHistory = settings.readBoolEntry( key + "/General/SaveHistory", true );
  saveVariables = settings.readBoolEntry( key + "/General/SaveVariables", true );
  autoComplete = settings.readBoolEntry( key + "/General/AutoComplete", true );
  autoCalc = settings.readBoolEntry( key + "/General/AutoCalc", true );
  decimalPoint = settings.readEntry( key + "/General/DecimalPoint", QString() );
  minimizeToTray = settings.readBoolEntry( key + "/General/MinimizeToTray", false );

  QString formatStr = settings.readEntry( key + "/View/Format" );
  if( formatStr == "Fixed" ) format = 'f';
  if( formatStr == "Exp" ) format = 'e';
  if( formatStr == "General" ) format = 'g';
  if( formatStr == "Hexadecimal" ) format = 'h';
  if( formatStr == "Octal" ) format = 'o';
  if( formatStr == "Binary" ) format = 'b';
  decimalDigits = settings.readNumEntry( key + "/View/DecimalDigits", -1 );
  if( decimalDigits > 70 ) decimalDigits = 70;

  showClearInputButton = settings.readBoolEntry( key + "/Appearance/ShowClearInputButton", true );
  showEvaluateButton = settings.readBoolEntry( key + "/Appearance/ShowEvaluateButton", true );
  showKeyPad = settings.readBoolEntry( key + "/Appearance/ShowKeyPad", true );
  showHistory = settings.readBoolEntry( key + "/Appearance/ShowHistory", false );
  showFunctions = settings.readBoolEntry( key + "/Appearance/ShowFunctions", false );

  customAppearance = settings.readBoolEntry( key + "/Appearance/CustomAppearance", false );
  customFont.fromString( settings.readEntry( key + "/Appearance/CustomFont", QFont().toString() ) );
  customTextColor.setNamedColor( settings.readEntry(
    key + "/Appearance/CustomTextColor", "#000000" ) );
  customBackgroundColor1.setNamedColor( settings.readEntry(
    key + "/Appearance/CustomBackgroundColor1", "#ffffff" ) );
  customBackgroundColor2.setNamedColor( settings.readEntry(
    key + "/Appearance/CustomBackgroundColor2", "#eeeeee" ) );
  customErrorColor.setNamedColor( settings.readEntry(
    key + "/Appearance/CustomErrorColor", "red" ) );
  mainWindowSize = QSize( settings.readNumEntry( key + "/Appearance/WindowWidth", 0 ),
                          settings.readNumEntry( key + "/Appearance/WindowHeight", 0 ) );

  key = SETTINGSKEY;
  key += "/MainWindow/";
  mainWindowState = settings.value( key + "State" ).toByteArray();
  historyDockFloating = settings.readBoolEntry( key + "HistoryDockFloating", false );
  historyDockTop = settings.readNumEntry( key + "HistoryDockTop", 0 );
  historyDockLeft = settings.readNumEntry( key + "HistoryDockLeft", 0 );
  historyDockWidth = settings.readNumEntry( key + "HistoryDockWidth", 150 );
  historyDockHeight = settings.readNumEntry( key + "HistoryDockHeight", 400 );
  functionsDockFloating = settings.readBoolEntry( key + "FunctionsDockFloating", false );
  functionsDockTop = settings.readNumEntry( key + "FunctionsDockTop", 0 );
  functionsDockLeft = settings.readNumEntry( key + "FunctionsDockLeft", 0 );
  functionsDockWidth = settings.readNumEntry( key + "FunctionsDockWidth", 150 );
  functionsDockHeight = settings.readNumEntry( key + "FunctionsDockHeight", 400 );

  key = SETTINGSKEY;
  key += "/SyntaxHighlight/";
  enableSyntaxHighlight = settings.readBoolEntry( key + "EnableSyntaxHighlight", true );

  highlightNumberColor.setNamedColor( settings.readEntry( key + "NumberColor", "#00007f" ) );
  highlightFunctionColor.setNamedColor( settings.readEntry( key + "FunctionColor", "#550000" ) );
  highlightVariableColor.setNamedColor( settings.readEntry( key + "VariableColor", "#005500" ) );
  matchedParenthesisColor.setNamedColor( settings.readEntry( key + "MatchedParenthesisColor", "#ffffb7" ) );

  key = SETTINGSKEY;

  history.clear();
  int count = settings.readNumEntry( key + "/History/Count", 0 );
  for( int i=0; i<count; i++ )
  {
    QString keyname = QString("%1/History/Expression%2").arg(key).arg(i);
    QString str = settings.readEntry( keyname );
    if( !str.isEmpty() )
    {
      QString expr;
      for( int c = 0; c < str.length(); c++ )
        if( str[c] >= 32 ) expr.append( str[c] );
      history.append( expr );
    }
  }

  variables.clear();
  QStringList names = settings.entryList( key + "/Variables" );
  for( int k=0; k<names.count(); k++ )
  {
    QString keyname = QString("%1/Variables/%2").arg(key).arg(names[k]);
    QString value = settings.readEntry( keyname );
    if( !value.isEmpty() )
      variables.append( QString("%1=%2").arg(names[k]).arg(value) );
  }
}

void Settings::save()
{
  QSettings settings( QSettings::UserScope, SETTINGSKEY, SETTINGSKEY );
  int k, i;

  QString key = SETTINGSKEY;

  settings.writeEntry( key + "/General/AngleMode", angleMode );
  settings.writeEntry( key + "/General/SaveHistory", saveHistory );
  settings.writeEntry( key + "/General/SaveVariables", saveVariables );
  settings.writeEntry( key + "/General/AutoComplete", autoComplete );
  settings.writeEntry( key + "/General/AutoCalc", autoCalc );
  settings.writeEntry( key + "/General/DecimalPoint", decimalPoint );
  settings.writeEntry( key + "/General/MinimizeToTray", minimizeToTray );

  QString formatStr;
  if( format == 'f' ) formatStr = "Fixed";
  if( format == 'e' ) formatStr = "Exp";
  if( format == 'g' ) formatStr = "General";
  if( format == 'h' ) formatStr = "Hexadecimal";
  if( format == 'o' ) formatStr = "Octal";
  if( format == 'b' ) formatStr = "Binary";
  settings.writeEntry( key + "/View/Format", formatStr );
  settings.writeEntry( key + "/View/DecimalDigits", decimalDigits );

  settings.writeEntry( key + "/Appearance/ShowClearInputButton", showClearInputButton );
  settings.writeEntry( key + "/Appearance/ShowEvaluateButton", showEvaluateButton );
  settings.writeEntry( key + "/Appearance/ShowKeyPad", showKeyPad );
  settings.writeEntry( key + "/Appearance/ShowHistory", showHistory );
  settings.writeEntry( key + "/Appearance/ShowFunctions", showFunctions );

  settings.writeEntry( key + "/Appearance/CustomAppearance", customAppearance );
  settings.writeEntry( key + "/Appearance/CustomFont", customFont.toString() );
  settings.writeEntry( key + "/Appearance/CustomTextColor", customTextColor.name() );
  settings.writeEntry( key + "/Appearance/CustomBackgroundColor1", customBackgroundColor1.name() );
  settings.writeEntry( key + "/Appearance/CustomBackgroundColor2", customBackgroundColor2.name() );
  settings.writeEntry( key + "/Appearance/CustomErrorColor", customErrorColor.name() );
  settings.writeEntry( key + "/Appearance/WindowWidth", mainWindowSize.width() ),
  settings.writeEntry( key + "/Appearance/WindowHeight", mainWindowSize.height() );

  settings.setValue( key + "/MainWindow/State", mainWindowState );
  settings.writeEntry( key + "/MainWindow/HistoryDockFloating", historyDockFloating );
  settings.writeEntry( key + "/MainWindow/HistoryDockTop", historyDockTop );
  settings.writeEntry( key + "/MainWindow/HistoryDockLeft", historyDockLeft );
  settings.writeEntry( key + "/MainWindow/HistoryDockWidth", historyDockWidth );
  settings.writeEntry( key + "/MainWindow/HistoryDockHeight", historyDockHeight );
  settings.writeEntry( key + "/MainWindow/FunctionsDockFloating", functionsDockFloating );
  settings.writeEntry( key + "/MainWindow/FunctionsDockTop", functionsDockTop );
  settings.writeEntry( key + "/MainWindow/FunctionsDockLeft", functionsDockLeft );
  settings.writeEntry( key + "/MainWindow/FunctionsDockWidth", functionsDockWidth );
  settings.writeEntry( key + "/MainWindow/FunctionsDockHeight", functionsDockHeight );

  key = SETTINGSKEY;
  key += "/SyntaxHighlight/";
  settings.writeEntry( key + "/EnableSyntaxHighlight", enableSyntaxHighlight );
  settings.writeEntry( key + "/NumberColor", highlightNumberColor.name() );
  settings.writeEntry( key + "/FunctionColor", highlightFunctionColor.name() );
  settings.writeEntry( key + "/VariableColor", highlightVariableColor.name() );
  settings.writeEntry( key + "/MatchedParenthesisColor", matchedParenthesisColor.name() );
  key = SETTINGSKEY;

  QStringList realHistory = history;
  if( history.count() > 100 )
  {
    realHistory.clear();
    unsigned start = history.count() - 100;
    for( int j=start; j<history.count(); j++ )
      realHistory.append( history[j] );
  }

  QStringList hkeys = settings.entryList( key + "/History" );
  for( k=0; k<hkeys.count(); k++ )
    settings.removeEntry( QString("%1/History/Expression%2").arg(key).arg(k) );
  settings.writeEntry( key + "/History/Count", (int)history.count() );
  for( i=0; i<realHistory.count(); i++ )
    settings.writeEntry( QString("%1/History/Expression%2").arg(key).arg(i),realHistory[i] );

  QStringList vkeys = settings.entryList( key + "/Variables" );
  for( k=0; k<vkeys.count(); k++ )
    settings.removeEntry( QString("%1/Variables/%2").arg(key).arg(vkeys[k]) );
  for( i=0; i<variables.count(); i++ )
  {
    QStringList s = QStringList::split( '=', variables[i] );
    if( s.count() == 2 )
      settings.writeEntry( QString("%1/Variables/%2").arg(key).arg(s[0]), s[1] );
  }

}

Settings* Settings::self()
{
  if( !s_self )
    s_self = new Settings();
  return s_self;
}
