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
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
    void digitsAuto();
    void digits2();
    void digits3();
    void digits8();
    void digits15();
    void digits50();
    void showClearButton();
    void showEvalButton();
    void showKeyPad();
    void configure();
    void about();
    void aboutQt();
    void addKeyPadText( const QString& );

  protected slots:
    void returnPressed();
    void textChanged();
    void angleModeChanged();

  protected:
    virtual void closeEvent( QCloseEvent* );

  private:
    void createUI();
    void saveSettings();

    CrunchPrivate* d;
    Crunch( const Crunch& );
    Crunch& operator=( const Crunch& );
};

#endif // CRUNCH
