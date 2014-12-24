// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2005, 2006 Johan Thelin <e8johan@gmail.com>
// Copyright (C) 2007-2014 Helder Correia <helder.pereira.correia@gmail.com>
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

#include <QMainWindow>
#include <QSystemTrayIcon>

class AutoHideLabel;
class BitFieldWidget;
class BookDock;
class Constants;
class ConstantsDock;
class Editor;
class Evaluator;
class FunctionRepo;
class FunctionsDock;
class HistoryDock;
class ManualWindow;
class ResultDisplay;
class Settings;
class TipWidget;
class VariablesDock;
class UserFunctionsDock;

class QActionGroup;
class QHBoxLayout;
class QLabel;
class QPlainTextEdit;
class QPushButton;
class QTranslator;
class QVBoxLayout;

class MainWindow : public QMainWindow {
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

private slots:
    void activate();
    void applySelectedColorScheme();
    void clearEditor();
    void clearHistory();
    void copyResultToClipboard();
    void cycleAngleUnits();
    void cycleResultFormats();
    void decreaseOpacity();
    void deleteVariables();
    void deleteUserFunctions();
    void evaluateEditorExpression();
    void exportHtml();
    void exportPlainText();
    void handleCopyAvailable(bool copyAvailable);
    void handleBitsChanged(const QString&str);
    void handleEditorTextChange();
    void handleDisplaySelectionChange();
    void handleEditorSelectionChange();
    void handleManualClosed();
    void handleSystemTrayIconActivation(QSystemTrayIcon::ActivationReason);
    void hideStateLabel();
    void increaseOpacity();
    void insertConstantIntoEditor(const QString&);
    void insertFunctionIntoEditor(const QString&);
    void insertTextIntoEditor(const QString&);
    void insertVariableIntoEditor(const QString&);
    void insertUserFunctionIntoEditor(const QString&);
    void minimizeToSystemTray();
    void openUpdatesURL();
    void openFeedbackURL();
    void openCommunityURL();
    void openNewsURL();
    void retranslateText();
    void saveSession();
    void selectEditorExpression();
    void setAlwaysOnTopEnabled(bool);
    void setAngleModeDegree();
    void setAngleModeRadian();
    void setAutoAnsEnabled(bool);
    void setAutoCalcEnabled(bool);
    void setAutoCompletionEnabled(bool);
    void setBitfieldVisible(bool);
    void setConstantsDockVisible(bool);
    void setFormulaBookDockVisible(bool);
    void setFullScreenEnabled(bool);
    void setFunctionsDockVisible(bool);
    void setHistoryDockVisible(bool);
    void setHistorySaveEnabled(bool);
    void setLeaveLastExpressionEnabled(bool);
#ifndef Q_OS_MAC
    void setMenuBarVisible(bool);
#endif
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
    void setDigitGrouping(QAction*);
    void setAutoResultToClipboardEnabled(bool);
    void setParseAllRadixChar(bool);
    void setStrictDigitGrouping(bool);
    void setSystemTrayIconEnabled(bool);
    void setVariableSaveEnabled(bool);
    void setUserFunctionSaveEnabled(bool b);
    void setVariablesDockVisible(bool);
    void setUserFunctionsDockVisible(bool);
    void setWindowPositionSaveEnabled(bool);
    void setWidgetsDirection();
    void showAboutDialog();
    void showStateLabel(const QString&);
    void showFontDialog();
    void showLanguageChooserDialog();
    void showManualWindow();
#ifndef Q_OS_MAC
    void showMenuBarTip();
#endif
    void showReadyMessage();
    void showResultFormatContextMenu(const QPoint&);
    void showSessionImportDialog();
    void showSessionLoadDialog();
    void showSystemTrayMessage();
    void increaseDisplayFontPointSize();
    void decreaseDisplayFontPointSize();

protected:
    virtual void closeEvent(QCloseEvent*);
    virtual bool event(QEvent*);
    virtual bool eventFilter(QObject*, QEvent*);

private:
    Q_DISABLE_COPY(MainWindow)

    void clearTextEditSelection(QPlainTextEdit*);
    void createUi();
    void createActions();
    void createActionGroups();
    void createActionShortcuts();
    void createMenus();
    void createStatusBar();
    void createFixedWidgets();
    void createBitField();
    void createBookDock();
    void createConstantsDock();
    void createFunctionsDock();
    void createHistoryDock();
    void createVariablesDock();
    void createUserFunctionsDock();
    void createFixedConnections();
    void applySettings();
    void checkInitialResultFormat();
    void checkInitialResultPrecision();
    void checkInitialLanguage();
    void checkInitialDigitGrouping();
    void restoreHistory();
    void restoreVariables();
    void restoreUserFunctions();
    void deleteStatusBar();
    void deleteBitField();
    void deleteBookDock();
    void deleteConstantsDock();
    void deleteFunctionsDock();
    void deleteHistoryDock();
    void deleteVariablesDock();
    void deleteUserFunctionsDock();
    void saveSettings();
    void setActionsText();
    void setMenusText();
    void setStatusBarText();
#ifndef Q_OS_MAC
    bool shouldAllowHiddenMenuBar();
#endif

    static QTranslator* createTranslator(const QString& langCode);

    struct {
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
        QAction* editInsertUserFunction;
        QAction* editDeleteUserFunction;
        QAction* editClearExpression;
        QAction* editClearHistory;
        QAction* viewFormulaBook;
        QAction* viewConstants;
        QAction* viewFunctions;
        QAction* viewVariables;
        QAction* viewUserFunctions;
        QAction* viewHistory;
#ifndef Q_OS_MAC
        QAction* viewMenuBar;
#endif
        QAction* viewStatusBar;
        QAction* viewFullScreenMode;
        QAction* viewBitfield;
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
        QAction* settingsBehaviorSaveUserFunctionsOnExit;
        QAction* settingsBehaviorSaveWindowPositionOnExit;
        QAction* settingsBehaviorPartialResults;
        QAction* settingsBehaviorAutoCompletion;
        QAction* settingsBehaviorSyntaxHighlighting;
        QAction* settingsBehaviorDigitGroupingNone;
        QAction* settingsBehaviorDigitGroupingOneSpace;
        QAction* settingsBehaviorDigitGroupingTwoSpaces;
        QAction* settingsBehaviorDigitGroupingThreeSpaces;
        QAction* settingsBehaviorAutoAns;
        QAction* settingsBehaviorLeaveLastExpression;
        QAction* settingsBehaviorAlwaysOnTop;
        QAction* settingsBehaviorMinimizeToTray;
        QAction* settingsBehaviorAutoResultToClipboard;
        QAction* settingsBehaviorParseAllRadixChar;
        QAction* settingsBehaviorStrictDigitGrouping;
        QAction* settingsDisplayZoomIn;
        QAction* settingsDisplayZoomOut;
        QAction* settingsDisplayFont;
        QAction* settingsDisplayColorSchemeStandard;
        QAction* settingsDisplayColorSchemeSublime;
        QAction* settingsDisplayColorSchemeTerminal;
        QAction* settingsDisplayColorSchemeSolarizedDark;
        QAction* settingsDisplayColorSchemeSolarizedLight;
        QAction* settingsRadixCharDefault;
        QAction* settingsRadixCharDot;
        QAction* settingsRadixCharComma;
        QAction* settingsLanguage;
        QAction* helpTipOfTheDay;
        QAction* helpManual;
        QAction* helpUpdates;
        QAction* helpFeedback;
        QAction* helpCommunity;
        QAction* helpNews;
        QAction* helpAbout;
    } m_actions;

    struct {
        QActionGroup* angle;
        QActionGroup* colorScheme;
        QActionGroup* digits;
        QActionGroup* resultFormat;
        QActionGroup* radixChar;
        QActionGroup* digitGrouping;
    } m_actionGroups;

    struct {
        QMenu* angleUnit;
        QMenu* behavior;
        QMenu* colorScheme;
        QMenu* decimal;
        QMenu* digitGrouping;
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
    } m_menus;

    struct {
        QVBoxLayout* root;
    } m_layouts;

    struct {
        QLabel* state;
        ResultDisplay* display;
        Editor* editor;
        QWidget* root;
        TipWidget* tip;
        QSystemTrayIcon* trayIcon;
        ManualWindow* manual;
        BitFieldWidget* bitField;
    } m_widgets;

    struct {
        BookDock* book;
        ConstantsDock* constants;
        FunctionsDock* functions;
        HistoryDock* history;
        VariablesDock* variables;
        UserFunctionsDock* userFunctions;
    } m_docks;

    struct {
#ifndef Q_OS_MAC
        bool notifyMenuBarHidden;
#endif
        bool trayNotify;
        bool autoAns;
    } m_conditions;

    struct {
        QPushButton* angleUnit;
        QPushButton* resultFormat;
    } m_status;

    Constants* m_constants;
    Evaluator* m_evaluator;
    FunctionRepo* m_functions;
    Settings* m_settings;
    QTranslator* m_translator;
    QPlainTextEdit* m_copyWidget;
};

#endif // GUI_MAINWINDOW_H
