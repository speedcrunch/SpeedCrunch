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


#include <QCloseEvent>
#include <QMainWindow>


class CrunchPrivate;


class Crunch : public QMainWindow
{
  Q_OBJECT

  public:
    Crunch();
    bool event( QEvent * );
    ~Crunch();

  signals:
    void adaptToLanguageChange();
    void quitApplication();

  public slots:
    void about();
    void aboutQt();
    void addKeypadText( const QString & );
    void applySettings();
    void clearHistory();
    void clearExpression();
    void configure();
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
    void radian();
    void saveSession();
    void setWidgetsLayoutAccordingToLanguageDirection();
    void showAutoCalc( const QString & );
    void showConstants( bool );
    void showFunctions( bool );
    void showHistory( bool );
    void showKeypad( bool );
    void showTip();
    void showVariables( bool );
    void scrollDown();
    void scrollUp();
    void viewBinary();
    void viewEngineering();
    void viewFixed();
    void viewGeneral();
    void viewHexadec();
    void viewOctal();
    void viewScientific();

  protected slots:
    void activate();
    void constantSelected( const QString & );
    void expressionSelected( const QString & );
    void functionSelected( const QString & );
    void minimizeToTray();
    void restoreDocks();
    void restoreLastSession();
    void returnPressed();
    void showTrayMessage();
    void textChanged();
    void trayIconActivated();
    void variableSelected( const QString & );

  protected:
    virtual void closeEvent( QCloseEvent * );

  private:
    void createUI();
    void saveSettings();
    void setDigits( int i = -1 );
    void setView( char c = 'g' );

    CrunchPrivate * d;
    Crunch( const Crunch & );
    Crunch & operator=( const Crunch & );
};


#endif
