// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
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

#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H

#include "gui/keypad.h"

#include <QtGui/QMainWindow>
#include <QtGui/QSystemTrayIcon>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

signals:
    void angleUnitChanged();
    void languageChanged();
    void radixCharacterChanged();
    void resultFormatChanged();
    void resultPrecisionChanged();

public slots:
    void copy();
    void raiseWindow();

private slots:
    void activate();
    void clearEditor();
    void clearHistory();
    void copyResultToClipboard();
    void cycleAngleUnits();
    void cycleResultFormats();
    void deleteAllVariables();
    void evaluateEditorExpression();
    void exportHtml();
    void exportPlainText();
    void handleCopyAvailable( bool yes );
    void handleEditorTextChange();
    void handleKeypadButtonPress( Keypad::Button );
    void handleSystemTrayIconActivation( QSystemTrayIcon::ActivationReason );
    void hideAutoCalcTip();
    void insertConstantIntoEditor( const QString & );
    void insertFunctionIntoEditor( const QString & );
    void insertTextIntoEditor( const QString & );
    void insertVariableIntoEditor( const QString & );
    void minimizeToSystemTray();
    void openHomePageInSystemBrowser();
    void retranslateText();
    void saveSession();
    void scrollDown();
    void scrollUp();
    void selectEditorExpression();
    void setAlwaysOnTopEnabled( bool );
    void setAngleModeDegree();
    void setAngleModeRadian();
    void setAutoAnsEnabled( bool );
    void setAutoCalcEnabled( bool );
    void setAutoCompletionEnabled( bool );
    void setConstantsDockVisible( bool );
    void setFullScreenEnabled( bool );
    void setFunctionsDockVisible( bool );
    void setHistoryDockVisible( bool );
    void setHistorySaveEnabled( bool );
    void setKeypadVisible( bool );
    void setMathBookDockVisible( bool );
    void setMenuBarVisible( bool );
    void setRadixCharacterAutomatic();
    void setRadixCharacter( char );
    void setRadixCharacterComma();
    void setRadixCharacterDot();
    void setResultFormatBinary();
    void setResultFormat( char );
    void setResultFormatEngineering();
    void setResultFormatFixed();
    void setResultFormatGeneral();
    void setResultFormatHexadecimal();
    void setResultFormatOctal();
    void setResultFormatScientific();
    void setResultPrecision15Digits();
    void setResultPrecision2Digits();
    void setResultPrecision3Digits();
    void setResultPrecision50Digits();
    void setResultPrecision8Digits();
    void setResultPrecisionAutomatic();
    void setResultPrecision( int );
    void setStatusBarVisible( bool );
    void setSyntaxHighlightingEnabled( bool );
    void setAutoResultToClipboardEnabled( bool );
    void setSystemTrayIconEnabled( bool );
    void setVariableSaveEnabled( bool );
    void setVariablesDockVisible( bool );
    void setWidgetsDirection();
    void showAboutDialog();
    void showAboutQtDialog();
    void showAutoCalcTip( const QString & );
    void showFontDialog();
    void showFunctionInsertionDialog();
    void showLanguageChooserDialog();
    void showMenuBarTip();
    void showResultFormatContextMenu( const QPoint & );
    void showSessionImportDialog();
    void showSessionLoadDialog();
    void showSystemTrayMessage();
    void showTipOfTheDay();
    void showVariableDeletionDialog();
    void showVariableInsertionDialog();
    void zoomIn();
    void zoomOut();

protected:
    virtual void closeEvent( QCloseEvent * );
    virtual bool event( QEvent * );
    virtual bool eventFilter( QObject *, QEvent * );

private:
    struct Private;
    const std::auto_ptr<Private> d;

    MainWindow( const MainWindow & );
    MainWindow & operator=( const MainWindow & );
};

#endif

