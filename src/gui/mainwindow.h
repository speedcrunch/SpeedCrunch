// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2005, 2006 Johan Thelin <e8johan@gmail.com>
// Copyright (C) 2007-2013 Helder Correia <helder.pereira.correia@gmail.com>
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

class AutoHideLabel;
class BookDock;
class Constants;
class ConstantsDock;
class Editor;
class Evaluator;
class Functions;
class FunctionsDock;
class HistoryDock;
class ResultDisplay;
class Settings;
class TextEdit;
class TipWidget;
class VariablesDock;

class QActionGroup;
class QHBoxLayout;
class QPushButton;
class QTranslator;
class QVBoxLayout;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

signals:
    void angleUnitChanged();
    void colorSchemeChanged();
    void languageChanged();
    void radixCharacterChanged();
    void resultFormatChanged();
    void resultPrecisionChanged();
    void syntaxHighlightingChanged();

public slots:
    void copy();
    void raiseWindow();

private slots:
    void activate();
    void applySelectedColorScheme();
    void clearEditor();
    void clearHistory();
    void copyResultToClipboard();
    void cycleAngleUnits();
    void cycleResultFormats();
    void deleteVariables();
    void evaluateEditorExpression();
    void exportHtml();
    void exportPlainText();
    void handleCopyAvailable(bool copyAvailable);
    void handleEditorTextChange();
    void handleKeypadButtonPress(Keypad::Button);
    void handleSystemTrayIconActivation(QSystemTrayIcon::ActivationReason);
    void hideAutoCalcTip();
    void insertConstantIntoEditor(const QString&);
    void insertFunctionIntoEditor(const QString&);
    void insertTextIntoEditor(const QString&);
    void insertVariableIntoEditor(const QString&);
    void minimizeToSystemTray();
    void retranslateText();
    void saveSession();
    void scrollDown();
    void scrollUp();
    void selectEditorExpression();
    void setAlwaysOnTopEnabled(bool);
    void setAngleModeDegree();
    void setAngleModeRadian();
    void setAutoAnsEnabled(bool);
    void setAutoCalcEnabled(bool);
    void setAutoCompletionEnabled(bool);
    void setConstantsDockVisible(bool);
    void setFormulaBookDockVisible(bool);
    void setFullScreenEnabled(bool);
    void setFunctionsDockVisible(bool);
    void setHistoryDockVisible(bool);
    void setHistorySaveEnabled(bool);
    void setKeypadVisible(bool);
    void setLeaveLastExpressionEnabled(bool);
    void setMenuBarVisible(bool);
    void setRadixCharacterAutomatic();
    void setRadixCharacter(char);
    void setRadixCharacterComma();
    void setRadixCharacterDot();
    void setResultFormatBinary();
    void setResultFormat(char);
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
    void setResultPrecision(int);
    void setStatusBarVisible(bool);
    void setSyntaxHighlightingEnabled(bool);
    void setAutoResultToClipboardEnabled(bool);
    void setSystemTrayIconEnabled(bool);
    void setVariableSaveEnabled(bool);
    void setVariablesDockVisible(bool);
    void setWidgetsDirection();
    void showAboutDialog();
    void showAutoCalcTip(const QString&);
    void showFontDialog();
    void showLanguageChooserDialog();
    void showMenuBarTip();
    void showResultFormatContextMenu(const QPoint&);
    void showSessionImportDialog();
    void showSessionLoadDialog();
    void showSystemTrayMessage();
    void zoomIn();
    void zoomOut();

protected:
    virtual void closeEvent(QCloseEvent*);
    virtual bool event(QEvent*);
    virtual bool eventFilter(QObject*, QEvent*);

private:
    MainWindow(const MainWindow&);
    MainWindow & operator=(const MainWindow&);

    void createUi();
    void createActions();
    void createActionGroups();
    void createActionShortcuts();
    void createMenus();
    void createStatusBar();
    void createFixedWidgets();
    void createKeypad();
    void createBookDock();
    void createConstantsDock();
    void createFunctionsDock();
    void createHistoryDock();
    void createVariablesDock();
    void createFixedConnections();
    void applySettings();
    void checkInitialResultFormat();
    void checkInitialResultPrecision();
    void checkInitialLanguage();
    void restoreHistory();
    void restoreVariables();
    void deleteKeypad();
    void deleteStatusBar();
    void deleteBookDock();
    void deleteConstantsDock();
    void deleteFunctionsDock();
    void deleteHistoryDock();
    void deleteVariablesDock();
    void saveSettings();
    void setActionsText();
    void setMenusText();
    void setStatusBarText();

    static QTranslator* createTranslator(const QString& langCode);

    struct Actions {
        QAction* sessionLoad;
        QAction* sessionSave;
        QAction* sessionImport;
        QAction* sessionExportHtml;
        QAction* sessionExportPlainText;
        QAction* sessionQuit;
        QAction* editCopy;
        QAction* editCopyLastResult;
        QAction* editPaste;
        QAction* editSelectExpression;
        QAction* editInsertFunction;
        QAction* editInsertVariable;
        QAction* editDeleteVariable;
        QAction* editClearExpression;
        QAction* editClearHistory;
        QAction* viewKeypad;
        QAction* viewFormulaBook;
        QAction* viewConstants;
        QAction* viewFunctions;
        QAction* viewVariables;
        QAction* viewHistory;
        QAction* viewMenuBar;
        QAction* viewStatusBar;
        QAction* viewFullScreenMode;
        QAction* settingsResultFormatGeneral;
        QAction* settingsResultFormatFixed;
        QAction* settingsResultFormatEngineering;
        QAction* settingsResultFormatScientific;
        QAction* settingsResultFormatAutoPrecision;
        QAction* settingsResultFormat2Digits;
        QAction* settingsResultFormat3Digits;
        QAction* settingsResultFormat8Digits;
        QAction* settingsResultFormat15Digits;
        QAction* settingsResultFormat50Digits;
        QAction* settingsResultFormatBinary;
        QAction* settingsResultFormatOctal;
        QAction* settingsResultFormatHexadecimal;
        QAction* settingsAngleUnitRadian;
        QAction* settingsAngleUnitDegree;
        QAction* settingsBehaviorSaveHistoryOnExit;
        QAction* settingsBehaviorSaveVariablesOnExit;
        QAction* settingsBehaviorPartialResults;
        QAction* settingsBehaviorAutoCompletion;
        QAction* settingsBehaviorSyntaxHighlighting;
        QAction* settingsBehaviorAutoAns;
        QAction* settingsBehaviorLeaveLastExpression;
        QAction* settingsBehaviorAlwaysOnTop;
        QAction* settingsBehaviorMinimizeToTray;
        QAction* settingsBehaviorAutoResultToClipboard;
        QAction* settingsDisplayZoomIn;
        QAction* settingsDisplayZoomOut;
        QAction* settingsDisplayFont;
        QAction* settingsDisplayColorSchemeStandard;
        QAction* settingsDisplayColorSchemeSublime;
        QAction* settingsDisplayColorSchemeTerminal;
        QAction* settingsRadixCharDefault;
        QAction* settingsRadixCharDot;
        QAction* settingsRadixCharComma;
        QAction* settingsLanguage;
        QAction* helpTipOfTheDay;
        QAction* helpAbout;
        QAction* helpAboutQt;
        // Shortcuts.
        QAction* scrollDown;
        QAction* scrollUp;
    };

    struct ActionGroups {
        QActionGroup* angle;
        QActionGroup* colorScheme;
        QActionGroup* digits;
        QActionGroup* resultFormat;
        QActionGroup* radixChar;
    };

    struct Menus {
        QMenu* angleUnit;
        QMenu* behavior;
        QMenu* colorScheme;
        QMenu* decimal;
        QMenu* display;
        QMenu* edit;
        QMenu* resultFormat;
        QMenu* help;
        QMenu* precision;
        QMenu* radixChar;
        QMenu* session;
        QMenu* sessionExport;
        QMenu* settings;
        QMenu* trayIcon;
        QMenu* view;
    };

    struct Layouts {
        QVBoxLayout* root;
        QHBoxLayout* keypad;
    };

    struct Widgets {
        AutoHideLabel* autoCalcTip;
        ResultDisplay* display;
        Editor* editor;
        Keypad* keypad;
        QWidget* root;
        TipWidget* tip;
        QSystemTrayIcon* trayIcon;
    };

    struct Docks {
        BookDock* book;
        ConstantsDock* constants;
        FunctionsDock* functions;
        HistoryDock* history;
        VariablesDock* variables;
    };

    struct Conditions {
        bool notifyMenuBarHidden;
        bool trayNotify;
        bool autoAns;
    };

    struct StatusBar {
        QPushButton* angleUnit;
        QPushButton* resultFormat;
    };

    Actions m_actions;
    ActionGroups m_actionGroups;
    Conditions m_conditions;
    Constants* m_constants;
    Docks m_docks;
    Evaluator* m_evaluator;
    Functions* m_functions;
    Layouts m_layouts;
    Menus m_menus;
    Settings* m_settings;
    StatusBar m_status;
    QTranslator* m_translator;
    Widgets m_widgets;
    TextEdit* m_copyWidget;
};

#endif
