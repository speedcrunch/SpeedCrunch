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

#ifndef CRUNCH
#define CRUNCH

#include <QMainWindow>
#include <QCloseEvent>

class CrunchPrivate;

class Crunch : public QMainWindow
{
  Q_OBJECT

  public:
    Crunch();
    ~Crunch();
    bool event( QEvent* );

  public slots:
    void applySettings();
    void saveSession();
    void copyResult();
    void clearInput();
    void clearVariables();
    void insertVariable();
    void deleteVariable();
    void insertFunction();
    void viewGeneral();
    void viewFixed();
    void viewExponential();
    void viewHexadec();
    void viewOctal();
    void viewBinary();
    void digitsAuto();
    void digits2();
    void digits3();
    void digits8();
    void digits15();
    void digits50();
    void showClearButton(bool);
    void showEvalButton(bool);
    void showKeyPad(bool);
    void showHistory(bool);
    void showFunctions(bool);
    void showVariables(bool);
    void showConstants(bool);
    void configure();
    void showTip();
    void gotoWebsite();
    void about();
    void aboutQt();
    void addKeyPadText( const QString& );
    void hideAutoCalc();
    void showAutoCalc( const QString& );

  protected slots:
    void restoreDocks();
    void activate();
    void returnPressed();
    void expressionSelected( const QString & );
    void functionSelected( const QString & );
    void variableSelected( const QString & );
    void constantSelected( const QString & );
    void textChanged();
    void angleModeChanged();
    void radixChanged();
    void minimizeToTray();
    void showTrayMessage();
    void trayIconActivated();

  protected:
    void saveDocks();
    virtual void closeEvent( QCloseEvent* );

  private:
    void createUI();
    void saveSettings();
    void setView(char c = 'g');
    void setDigits(int i = -1);

    CrunchPrivate* d;
    Crunch( const Crunch& );
    Crunch& operator=( const Crunch& );
};

#endif // CRUNCH
