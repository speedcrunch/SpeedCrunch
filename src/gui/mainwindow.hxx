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


#ifndef CRUNCH_HXX
#define CRUNCH_HXX


#include "keypad.hxx"

#include <QCloseEvent>
#include <QMainWindow>
#include <QSystemTrayIcon>


class Keypad;


class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
    MainWindow();
    ~MainWindow();

  signals:
    void adaptToLanguageChange();
    void angleModeChanged( char );
    void formatChanged( char );
    void radixCharChanged( char );
    void precisionChanged( int );
    void quitApplication();

  public slots:
    void about();
    void aboutQt();
    void clearHistory();
    void clearExpression();
    void copyResult();
    void degree();
    void deleteAllVariables();
    void deleteVariable();
    void digits2();
    void digits3();
    void digits8();
    void digits15();
    void digits50();
    void digitsAuto();
    void selectExpression();
    void gotoWebsite();
    void hideAutoCalc();
    void insertFunction();
    void insertVariable();
    void loadSession();
    void alwaysOnTopToggled( bool );
    void autoCalcToggled( bool );
    void autoCompletionToggled( bool );
    void minimizeToTrayToggled( bool );
    void radian();
    void saveSession();
    void setWidgetsLayoutAccordingToLanguageDirection();
    void showAutoCalc( const QString & );
    void showConstants( bool );
    void showInFullScreen( bool );
    void showFunctions( bool );
    void showHistory( bool );
    void showKeypad( bool );
    void showMenuBarTip();
    void showTipOfTheDay();
    void showVariables( bool );
    void scrollDown();
    void scrollUp();
    void showMenuBar();
    void formatBinary();
    void formatEngineering();
    void formatFixed();
    void formatGeneral();
    void formatHexadec();
    void formatOctal();
    void formatScientific();
    void radixCharAutoActivated();
    void radixCharDotActivated();
    void radixCharCommaActivated();

  protected slots:
    void activate();
    void constantSelected( const QString & );
    void expressionSelected( const QString & );
    void functionSelected( const QString & );
    void keypadButtonPressed( Keypad::Button );
    void minimizeToTray();
    void returnPressed();
    void showTrayMessage();
    void textChanged();
    void trayIconActivated( QSystemTrayIcon::ActivationReason );
    void variableSelected( const QString & );
    void setPrecision( int );
    void setFormat( char );
    void setRadixChar( char );

  protected:
    virtual void closeEvent( QCloseEvent * );
    virtual bool event( QEvent * );
    bool         eventFilter( QObject *, QEvent * );

  private:
    struct Private;
    Private * const d;
    MainWindow( const MainWindow & );
    MainWindow & operator=( const MainWindow & );
};


#endif
