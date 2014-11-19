// This file is part of the SpeedCrunch project
// Copyright (C) 2004, 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2005, 2006 Johan Thelin <e8johan@gmail.com>
// Copyright (C) 2007-2014 Helder Correia <helder.pereira.correia@gmail.com>
// Copyright (C) 2011 Daniel Schäufele <git@schaeufele.org>
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

#include "gui/mainwindow.h"

#include "thirdparty/binreloc.h"
#include "core/constants.h"
#include "core/evaluator.h"
#include "core/functions.h"
#include "core/numberformatter.h"
#include "core/settings.h"
#include "gui/aboutbox.h"
#include "gui/bitfieldwidget.h"
#include "gui/bookdock.h"
#include "gui/constantsdock.h"
#include "gui/editor.h"
#include "gui/functionsdock.h"
#include "gui/historydock.h"
#include "gui/historywidget.h"
#include "gui/manualwindow.h"
#include "gui/resultdisplay.h"
#include "gui/syntaxhighlighter.h"
#include "gui/tipwidget.h"
#include "gui/variablesdock.h"
#include "gui/userfunctionsdock.h"
#include "math/floatconfig.h"

#include <QLatin1String>
#include <QLocale>
#include <QTextStream>
#include <QTimer>
#include <QTranslator>
#include <QUrl>
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QFont>
#include <QFontDialog>
#include <QInputDialog>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollBar>
#include <QStatusBar>
#include <QToolTip>
#include <QVBoxLayout>
#ifdef Q_WS_X11
#include <QX11Info>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#endif // Q_WS_X11

#ifdef Q_OS_WIN32
#include "Windows.h"
#endif // Q_OS_WIN32

QTranslator* MainWindow::createTranslator(const QString& langCode)
{
    QTranslator* translator = new QTranslator;
    QString locale = (langCode == "C") ? QLocale().name() : langCode;

    if (locale == "C")
        locale = "en";

    translator->load(QString(":/locale/") + locale);
    return translator;
}

void MainWindow::createUi()
{
    createActions();
    createActionGroups();
    createActionShortcuts();
    createMenus();
    createFixedWidgets();
    createFixedConnections();

    setWindowTitle("SpeedCrunch");
    setWindowIcon(QPixmap(":/speedcrunch.png"));

    m_copyWidget = m_widgets.editor;
}

void MainWindow::createActions()
{
    m_actions.sessionExportHtml = new QAction(this);
    m_actions.sessionExportPlainText = new QAction(this);
    m_actions.sessionImport = new QAction(this);
    m_actions.sessionLoad = new QAction(this);
    m_actions.sessionQuit = new QAction(this);
    m_actions.sessionSave = new QAction(this);
    m_actions.editClearExpression = new QAction(this);
    m_actions.editClearHistory = new QAction(this);
    m_actions.editCopyLastResult = new QAction(this);
    m_actions.editCopy = new QAction(this);
    m_actions.editPaste = new QAction(this);
    m_actions.editSelectExpression = new QAction(this);
    m_actions.viewConstants = new QAction(this);
    m_actions.viewFullScreenMode = new QAction(this);
    m_actions.viewFunctions = new QAction(this);
    m_actions.viewHistory = new QAction(this);
    m_actions.viewFormulaBook = new QAction(this);
#ifndef Q_OS_MAC
    if (shouldAllowHiddenMenuBar())
        m_actions.viewMenuBar = new QAction(this);
#endif
    m_actions.viewStatusBar = new QAction(this);
    m_actions.viewVariables = new QAction(this);
    m_actions.viewBitfield = new QAction(this);
    m_actions.viewUserFunctions = new QAction(this);
    m_actions.settingsAngleUnitDegree = new QAction(this);
    m_actions.settingsAngleUnitRadian = new QAction(this);
    m_actions.settingsBehaviorAlwaysOnTop = new QAction(this);
    m_actions.settingsBehaviorAutoAns = new QAction(this);
    m_actions.settingsBehaviorAutoCompletion = new QAction(this);
    m_actions.settingsBehaviorLeaveLastExpression = new QAction(this);
    m_actions.settingsBehaviorMinimizeToTray = new QAction(this);
    m_actions.settingsBehaviorPartialResults = new QAction(this);
    m_actions.settingsBehaviorSaveHistoryOnExit = new QAction(this);
    m_actions.settingsBehaviorSaveVariablesOnExit = new QAction(this);
    m_actions.settingsBehaviorSaveUserFunctionsOnExit = new QAction(this);
    m_actions.settingsBehaviorSaveWindowPositionOnExit = new QAction(this);
    m_actions.settingsBehaviorSyntaxHighlighting = new QAction(this);
    m_actions.settingsBehaviorDigitGroupingNone = new QAction(this);
    m_actions.settingsBehaviorDigitGroupingOneSpace = new QAction(this);
    m_actions.settingsBehaviorDigitGroupingTwoSpaces = new QAction(this);
    m_actions.settingsBehaviorDigitGroupingThreeSpaces = new QAction(this);
    m_actions.settingsBehaviorAutoResultToClipboard = new QAction(this);
    m_actions.settingsBehaviorParseAllRadixChar = new QAction(this);
    m_actions.settingsBehaviorStrictDigitGrouping = new QAction(this);
    m_actions.settingsDisplayColorSchemeStandard = new QAction(this);
    m_actions.settingsDisplayColorSchemeSublime = new QAction(this);
    m_actions.settingsDisplayColorSchemeTerminal = new QAction(this);
    m_actions.settingsDisplayFont = new QAction(this);
    m_actions.settingsLanguage = new QAction(this);
    m_actions.settingsRadixCharComma = new QAction(this);
    m_actions.settingsRadixCharDefault = new QAction(this);
    m_actions.settingsRadixCharDot = new QAction(this);
    m_actions.settingsResultFormat15Digits = new QAction(this);
    m_actions.settingsResultFormat2Digits = new QAction(this);
    m_actions.settingsResultFormat3Digits = new QAction(this);
    m_actions.settingsResultFormat50Digits = new QAction(this);
    m_actions.settingsResultFormat8Digits = new QAction(this);
    m_actions.settingsResultFormatAutoPrecision = new QAction(this);
    m_actions.settingsResultFormatBinary = new QAction(this);
    m_actions.settingsResultFormatEngineering = new QAction(this);
    m_actions.settingsResultFormatFixed = new QAction(this);
    m_actions.settingsResultFormatGeneral = new QAction(this);
    m_actions.settingsResultFormatHexadecimal = new QAction(this);
    m_actions.settingsResultFormatOctal = new QAction(this);
    m_actions.settingsResultFormatScientific = new QAction(this);
    m_actions.helpManual = new QAction(this);
    m_actions.helpUpdates = new QAction(this);
    m_actions.helpFeedback = new QAction(this);
    m_actions.helpCommunity = new QAction(this);
    m_actions.helpNews = new QAction(this);
    m_actions.helpAbout = new QAction(this);

    m_actions.settingsAngleUnitDegree->setCheckable(true);
    m_actions.settingsAngleUnitRadian->setCheckable(true);
    m_actions.settingsBehaviorAlwaysOnTop->setCheckable(true);
    m_actions.settingsBehaviorAutoAns->setCheckable(true);
    m_actions.settingsBehaviorAutoCompletion->setCheckable(true);
    m_actions.settingsBehaviorLeaveLastExpression->setCheckable(true);
    m_actions.settingsBehaviorMinimizeToTray->setCheckable(true);
    m_actions.settingsBehaviorPartialResults->setCheckable(true);
    m_actions.settingsBehaviorSaveHistoryOnExit->setCheckable(true);
    m_actions.settingsBehaviorSaveVariablesOnExit->setCheckable(true);
    m_actions.settingsBehaviorSaveUserFunctionsOnExit->setCheckable(true);
    m_actions.settingsBehaviorSaveWindowPositionOnExit->setCheckable(true);
    m_actions.settingsBehaviorSyntaxHighlighting->setCheckable(true);
    m_actions.settingsBehaviorDigitGroupingNone->setCheckable(true);
    m_actions.settingsBehaviorDigitGroupingNone->setData(0);
    m_actions.settingsBehaviorDigitGroupingOneSpace->setCheckable(true);
    m_actions.settingsBehaviorDigitGroupingOneSpace->setData(1);
    m_actions.settingsBehaviorDigitGroupingTwoSpaces->setCheckable(true);
    m_actions.settingsBehaviorDigitGroupingTwoSpaces->setData(2);
    m_actions.settingsBehaviorDigitGroupingThreeSpaces->setCheckable(true);
    m_actions.settingsBehaviorDigitGroupingThreeSpaces->setData(3);
    m_actions.settingsBehaviorAutoResultToClipboard->setCheckable(true);
    m_actions.settingsBehaviorParseAllRadixChar->setCheckable(true);
    m_actions.settingsBehaviorStrictDigitGrouping->setCheckable(true);
    m_actions.settingsDisplayColorSchemeStandard->setCheckable(true);
    m_actions.settingsDisplayColorSchemeSublime->setCheckable(true);
    m_actions.settingsDisplayColorSchemeTerminal->setCheckable(true);
    m_actions.settingsRadixCharComma->setCheckable(true);
    m_actions.settingsRadixCharDefault->setCheckable(true);
    m_actions.settingsRadixCharDot->setCheckable(true);
    m_actions.settingsResultFormat15Digits->setCheckable(true);
    m_actions.settingsResultFormat2Digits->setCheckable(true);
    m_actions.settingsResultFormat3Digits->setCheckable(true);
    m_actions.settingsResultFormat50Digits->setCheckable(true);
    m_actions.settingsResultFormat8Digits->setCheckable(true);
    m_actions.settingsResultFormatAutoPrecision->setCheckable(true);
    m_actions.settingsResultFormatBinary->setCheckable(true);
    m_actions.settingsResultFormatEngineering->setCheckable(true);
    m_actions.settingsResultFormatFixed->setCheckable(true);
    m_actions.settingsResultFormatGeneral->setCheckable(true);
    m_actions.settingsResultFormatHexadecimal->setCheckable(true);
    m_actions.settingsResultFormatOctal->setCheckable(true);
    m_actions.settingsResultFormatScientific->setCheckable(true);
    m_actions.viewConstants->setCheckable(true);
    m_actions.viewFullScreenMode->setCheckable(true);
    m_actions.viewFunctions->setCheckable(true);
    m_actions.viewHistory->setCheckable(true);
    m_actions.viewFormulaBook->setCheckable(true);
#ifndef Q_OS_MAC
    if (shouldAllowHiddenMenuBar())
        m_actions.viewMenuBar->setCheckable(true);
#endif
    m_actions.viewStatusBar->setCheckable(true);
    m_actions.viewVariables->setCheckable(true);
    m_actions.viewBitfield->setCheckable(true);
    m_actions.viewUserFunctions->setCheckable(true);

    m_actions.settingsDisplayColorSchemeStandard->setData(SyntaxHighlighter::Standard);
    m_actions.settingsDisplayColorSchemeSublime->setData(SyntaxHighlighter::Sublime);
    m_actions.settingsDisplayColorSchemeTerminal->setData(SyntaxHighlighter::Terminal);
}

void MainWindow::retranslateText()
{
    QTranslator* tr = 0;
    tr = createTranslator(m_settings->language);
    if (tr) {
        if (m_translator) {
            qApp->removeTranslator(m_translator);
            m_translator->deleteLater();
        }

        qApp->installTranslator(tr);
        m_translator = tr;
    } else {
        qApp->removeTranslator(m_translator);
        m_translator = 0;
    }

    setMenusText();
    setActionsText();
    setStatusBarText();
    setWidgetsDirection();
}

void MainWindow::setStatusBarText()
{
    if (m_status.angleUnit) {
        QString angleUnit = (m_settings->angleUnit == 'r') ?
            MainWindow::tr("Radian") : MainWindow::tr("Degree");

        QString format;
        switch (m_settings->resultFormat) {
            case 'b': format = MainWindow::tr("Binary"); break;
            case 'o': format = MainWindow::tr("Octal"); break;
            case 'h': format = MainWindow::tr("Hexadecimal"); break;
            case 'f': format = MainWindow::tr("Fixed decimal"); break;
            case 'n': format = MainWindow::tr("Engineering decimal"); break;
            case 'e': format = MainWindow::tr("Scientific decimal"); break;
            case 'g': format = MainWindow::tr("General decimal"); break;
            default : break;
        }

        m_status.angleUnit->setText(angleUnit);
        m_status.resultFormat->setText(format);

        m_status.angleUnit->setToolTip(MainWindow::tr("Angle unit"));
        m_status.resultFormat->setToolTip(MainWindow::tr("Result format"));
    }
}

#ifndef Q_OS_MAC
bool MainWindow::shouldAllowHiddenMenuBar()
{
#ifdef Q_WS_X11
    if (qgetenv("UBUNTU_MENUPROXY") == "libappmenu.so")
        return false;
#endif

    return true;
}
#endif

void MainWindow::setActionsText()
{
    m_actions.sessionExportHtml->setText(MainWindow::tr("&HTML"));
    m_actions.sessionExportPlainText->setText(MainWindow::tr("Plain &text"));
    m_actions.sessionImport->setText(MainWindow::tr("&Import..."));
    m_actions.sessionLoad->setText(MainWindow::tr("&Load..."));
    m_actions.sessionQuit->setText(MainWindow::tr("&Quit"));
    m_actions.sessionSave->setText(MainWindow::tr("&Save..."));

    m_actions.editClearExpression->setText(MainWindow::tr("Clear E&xpression"));
    m_actions.editClearHistory->setText(MainWindow::tr("Clear &History"));
    m_actions.editCopyLastResult->setText(MainWindow::tr("Copy Last &Result"));
    m_actions.editCopy->setText(MainWindow::tr("&Copy"));
    m_actions.editPaste->setText(MainWindow::tr("&Paste"));
    m_actions.editSelectExpression->setText(MainWindow::tr("&Select Expression"));

    m_actions.viewConstants->setText(MainWindow::tr("&Constants"));
    m_actions.viewFullScreenMode->setText(MainWindow::tr("F&ull Screen Mode"));
    m_actions.viewFunctions->setText(MainWindow::tr("&Functions"));
    m_actions.viewHistory->setText(MainWindow::tr("&History"));
    m_actions.viewFormulaBook->setText(MainWindow::tr("Formula &Book"));
#ifndef Q_OS_MAC
    if (shouldAllowHiddenMenuBar())
        m_actions.viewMenuBar->setText(MainWindow::tr("&Menu Bar"));
#endif
    m_actions.viewStatusBar->setText(MainWindow::tr("&Status Bar"));
    m_actions.viewVariables->setText(MainWindow::tr("&Variables"));
    m_actions.viewBitfield->setText(MainWindow::tr("Bitfield"));
    m_actions.viewUserFunctions->setText(MainWindow::tr("Use&r Functions"));

    m_actions.settingsAngleUnitDegree->setText(MainWindow::tr("&Degree"));
    m_actions.settingsAngleUnitRadian->setText(MainWindow::tr("&Radian"));
    m_actions.settingsBehaviorAlwaysOnTop->setText(MainWindow::tr("Always On &Top"));
    m_actions.settingsBehaviorAutoAns->setText(MainWindow::tr("Automatic Result &Reuse"));
    m_actions.settingsBehaviorAutoCompletion->setText(MainWindow::tr("Automatic &Completion"));
    m_actions.settingsBehaviorMinimizeToTray->setText(MainWindow::tr("&Minimize To System Tray"));
    m_actions.settingsBehaviorPartialResults->setText(MainWindow::tr("&Partial Results"));
    m_actions.settingsBehaviorSaveHistoryOnExit->setText(MainWindow::tr("Save &History on Exit"));
    m_actions.settingsBehaviorSaveVariablesOnExit->setText(MainWindow::tr("Save &Variables on Exit"));
    m_actions.settingsBehaviorSaveUserFunctionsOnExit->setText(MainWindow::tr("Save User &Functions on Exit"));
    m_actions.settingsBehaviorSaveWindowPositionOnExit->setText(MainWindow::tr("Save &Window Positon on Exit"));
    m_actions.settingsBehaviorSyntaxHighlighting->setText(MainWindow::tr("Syntax &Highlighting"));
    m_actions.settingsBehaviorDigitGroupingNone->setText(MainWindow::tr("Disabled"));
    m_actions.settingsBehaviorDigitGroupingOneSpace->setText(MainWindow::tr("Small Space"));
    m_actions.settingsBehaviorDigitGroupingTwoSpaces->setText(MainWindow::tr("Medium Space"));
    m_actions.settingsBehaviorDigitGroupingThreeSpaces->setText(MainWindow::tr("Large Space"));
    m_actions.settingsBehaviorLeaveLastExpression->setText(MainWindow::tr("Leave &Last Expression"));
    m_actions.settingsBehaviorAutoResultToClipboard->setText(MainWindow::tr("Automatic &Result to Clipboard"));
    m_actions.settingsBehaviorParseAllRadixChar->setText(MainWindow::tr("Detect &All Radix Characters"));
    m_actions.settingsBehaviorStrictDigitGrouping->setText(MainWindow::tr("&Strict Digit Groups Detection"));
    m_actions.settingsRadixCharComma->setText(MainWindow::tr("&Comma"));
    m_actions.settingsRadixCharDefault->setText(MainWindow::tr("&System Default"));
    m_actions.settingsRadixCharDot->setText(MainWindow::tr("&Dot"));
    m_actions.settingsResultFormat15Digits->setText(MainWindow::tr("&15 Decimal Digits"));
    m_actions.settingsResultFormat2Digits->setText(MainWindow::tr("&2 Decimal Digits"));
    m_actions.settingsResultFormat3Digits->setText(MainWindow::tr("&3 Decimal Digits"));
    m_actions.settingsResultFormat50Digits->setText(MainWindow::tr("&50 Decimal Digits"));
    m_actions.settingsResultFormat8Digits->setText(MainWindow::tr("&8 Decimal Digits"));
    m_actions.settingsResultFormatAutoPrecision->setText(MainWindow::tr("&Automatic"));
    m_actions.settingsResultFormatBinary->setText(MainWindow::tr("&Binary"));
    m_actions.settingsResultFormatEngineering->setText(MainWindow::tr("&Engineering"));
    m_actions.settingsResultFormatFixed->setText(MainWindow::tr("&Fixed Decimal"));
    m_actions.settingsResultFormatGeneral->setText(MainWindow::tr("&General"));
    m_actions.settingsResultFormatHexadecimal->setText(MainWindow::tr("&Hexadecimal"));
    m_actions.settingsResultFormatOctal->setText(MainWindow::tr("&Octal"));
    m_actions.settingsResultFormatScientific->setText(MainWindow::tr("&Scientific"));
    m_actions.settingsDisplayColorSchemeStandard->setText(MainWindow::tr("Standard"));
    m_actions.settingsDisplayColorSchemeSublime->setText(MainWindow::tr("Sublime"));
    m_actions.settingsDisplayColorSchemeTerminal->setText(MainWindow::tr("Terminal"));
    m_actions.settingsDisplayFont->setText(MainWindow::tr("&Font..."));
    m_actions.settingsLanguage->setText(MainWindow::tr("&Language..."));

    m_actions.helpManual->setText(MainWindow::tr("User &Manual"));
    m_actions.helpUpdates->setText(MainWindow::tr("Check &Updates"));
    m_actions.helpFeedback->setText(MainWindow::tr("Send &Feedback"));
    m_actions.helpCommunity->setText(MainWindow::tr("Join &Community"));
    m_actions.helpNews->setText(MainWindow::tr("&News Feed"));
    m_actions.helpAbout->setText(MainWindow::tr("About &SpeedCrunch"));
}

void MainWindow::createActionGroups()
{
    m_actionGroups.resultFormat = new QActionGroup(this);
    m_actionGroups.resultFormat->addAction(m_actions.settingsResultFormatBinary);
    m_actionGroups.resultFormat->addAction(m_actions.settingsResultFormatGeneral);
    m_actionGroups.resultFormat->addAction(m_actions.settingsResultFormatFixed);
    m_actionGroups.resultFormat->addAction(m_actions.settingsResultFormatEngineering);
    m_actionGroups.resultFormat->addAction(m_actions.settingsResultFormatScientific);
    m_actionGroups.resultFormat->addAction(m_actions.settingsResultFormatOctal);
    m_actionGroups.resultFormat->addAction(m_actions.settingsResultFormatHexadecimal);

    m_actionGroups.radixChar = new QActionGroup(this);
    m_actionGroups.radixChar->addAction(m_actions.settingsRadixCharDefault);
    m_actionGroups.radixChar->addAction(m_actions.settingsRadixCharDot);
    m_actionGroups.radixChar->addAction(m_actions.settingsRadixCharComma);

    m_actionGroups.digits = new QActionGroup(this);
    m_actionGroups.digits->addAction(m_actions.settingsResultFormatAutoPrecision);
    m_actionGroups.digits->addAction(m_actions.settingsResultFormat2Digits);
    m_actionGroups.digits->addAction(m_actions.settingsResultFormat3Digits);
    m_actionGroups.digits->addAction(m_actions.settingsResultFormat8Digits);
    m_actionGroups.digits->addAction(m_actions.settingsResultFormat15Digits);
    m_actionGroups.digits->addAction(m_actions.settingsResultFormat50Digits);

    m_actionGroups.angle = new QActionGroup(this);
    m_actionGroups.angle->addAction(m_actions.settingsAngleUnitRadian);
    m_actionGroups.angle->addAction(m_actions.settingsAngleUnitDegree);

    m_actionGroups.colorScheme = new QActionGroup(this);
    m_actionGroups.colorScheme->addAction(m_actions.settingsDisplayColorSchemeStandard);
    m_actionGroups.colorScheme->addAction(m_actions.settingsDisplayColorSchemeSublime);
    m_actionGroups.colorScheme->addAction(m_actions.settingsDisplayColorSchemeTerminal);

    m_actionGroups.digitGrouping = new QActionGroup(this);
    m_actionGroups.digitGrouping->addAction(m_actions.settingsBehaviorDigitGroupingNone);
    m_actionGroups.digitGrouping->addAction(m_actions.settingsBehaviorDigitGroupingOneSpace);
    m_actionGroups.digitGrouping->addAction(m_actions.settingsBehaviorDigitGroupingTwoSpaces);
    m_actionGroups.digitGrouping->addAction(m_actions.settingsBehaviorDigitGroupingThreeSpaces);
}

void MainWindow::createActionShortcuts()
{
    m_actions.sessionLoad->setShortcut(Qt::CTRL + Qt::Key_L);
    m_actions.sessionQuit->setShortcut(Qt::CTRL + Qt::Key_Q);
    m_actions.sessionSave->setShortcut(Qt::CTRL + Qt::Key_S);
    m_actions.editClearExpression->setShortcut(Qt::Key_Escape);
    m_actions.editClearHistory->setShortcut(Qt::CTRL + Qt::Key_N);
    m_actions.editCopyLastResult->setShortcut(Qt::CTRL + Qt::Key_R);
    m_actions.editCopy->setShortcut(Qt::CTRL + Qt::Key_C);
    m_actions.editPaste->setShortcut(Qt::CTRL + Qt::Key_V);
    m_actions.editSelectExpression->setShortcut(Qt::CTRL + Qt::Key_A);
    m_actions.viewBitfield->setShortcut(Qt::CTRL + Qt::Key_6);
    m_actions.viewConstants->setShortcut(Qt::CTRL + Qt::Key_2);
    m_actions.viewFullScreenMode->setShortcut(Qt::Key_F11);
    m_actions.viewFunctions->setShortcut(Qt::CTRL + Qt::Key_3);
    m_actions.viewHistory->setShortcut(Qt::CTRL + Qt::Key_7);
    m_actions.viewFormulaBook->setShortcut(Qt::CTRL + Qt::Key_1);
#ifndef Q_OS_MAC
    if (shouldAllowHiddenMenuBar())
        m_actions.viewMenuBar->setShortcut(Qt::CTRL + Qt::Key_M);
#endif
    m_actions.viewStatusBar->setShortcut(Qt::CTRL + Qt::Key_B);
    m_actions.viewVariables->setShortcut(Qt::CTRL + Qt::Key_4);
    m_actions.viewUserFunctions->setShortcut(Qt::CTRL + Qt::Key_5);
    m_actions.settingsAngleUnitDegree->setShortcut(Qt::Key_F10);
    m_actions.settingsAngleUnitRadian->setShortcut(Qt::Key_F9);
    m_actions.settingsResultFormatGeneral->setShortcut(Qt::Key_F1);
    m_actions.settingsResultFormatFixed->setShortcut(Qt::Key_F2);
    m_actions.settingsResultFormatEngineering->setShortcut(Qt::Key_F3);
    m_actions.settingsResultFormatScientific->setShortcut(Qt::Key_F4);
    m_actions.settingsResultFormatBinary->setShortcut(Qt::Key_F5);
    m_actions.settingsResultFormatOctal->setShortcut(Qt::Key_F6);
    m_actions.settingsResultFormatHexadecimal->setShortcut(Qt::Key_F7);
    m_actions.settingsRadixCharDot->setShortcut(Qt::CTRL + Qt::Key_Period);
    m_actions.settingsRadixCharComma->setShortcut(Qt::CTRL + Qt::Key_Comma);
}

void MainWindow::createMenus()
{
    m_menus.session = new QMenu("", this);
    menuBar()->addMenu(m_menus.session);
    m_menus.session->addAction(m_actions.sessionLoad);
    m_menus.session->addAction(m_actions.sessionSave);
    m_menus.session->addSeparator();
    m_menus.session->addAction(m_actions.sessionImport);
    m_menus.sessionExport = m_menus.session->addMenu("");
    m_menus.sessionExport->addAction(m_actions.sessionExportPlainText);
    m_menus.sessionExport->addAction(m_actions.sessionExportHtml);
    m_menus.session->addSeparator();
    m_menus.session->addAction(m_actions.sessionQuit);

    m_menus.edit = new QMenu("", this);
    menuBar()->addMenu(m_menus.edit);
    m_menus.edit->addAction(m_actions.editCopy);
    m_menus.edit->addAction(m_actions.editCopyLastResult);
    m_menus.edit->addAction(m_actions.editPaste);
    m_menus.edit->addAction(m_actions.editSelectExpression);
    m_menus.edit->addAction(m_actions.editClearExpression);
    m_menus.edit->addAction(m_actions.editClearHistory);

    m_menus.view = new QMenu("", this);
    menuBar()->addMenu(m_menus.view);
    m_menus.view->addAction(m_actions.viewFormulaBook);
    m_menus.view->addAction(m_actions.viewConstants);
    m_menus.view->addAction(m_actions.viewFunctions);
    m_menus.view->addAction(m_actions.viewVariables);
    m_menus.view->addAction(m_actions.viewUserFunctions);
    m_menus.view->addAction(m_actions.viewBitfield);
    m_menus.view->addAction(m_actions.viewHistory);
    m_menus.view->addSeparator();
    m_menus.view->addAction(m_actions.viewStatusBar);
#ifndef Q_OS_MAC
    if (shouldAllowHiddenMenuBar())
        m_menus.view->addAction(m_actions.viewMenuBar);
#endif
    m_menus.view->addSeparator();
    m_menus.view->addAction(m_actions.viewFullScreenMode);

    m_menus.settings = new QMenu("", this);
    menuBar()->addMenu(m_menus.settings);

    m_menus.resultFormat = m_menus.settings->addMenu("");

    m_menus.decimal = m_menus.resultFormat->addMenu("");
    m_menus.decimal->addAction(m_actions.settingsResultFormatGeneral);
    m_menus.decimal->addAction(m_actions.settingsResultFormatFixed);
    m_menus.decimal->addAction(m_actions.settingsResultFormatEngineering);
    m_menus.decimal->addAction(m_actions.settingsResultFormatScientific);
    m_menus.decimal->addSeparator();

    m_menus.precision = m_menus.decimal->addMenu("");
    m_menus.precision->addAction(m_actions.settingsResultFormatAutoPrecision);
    m_menus.precision->addAction(m_actions.settingsResultFormat2Digits);
    m_menus.precision->addAction(m_actions.settingsResultFormat3Digits);
    m_menus.precision->addAction(m_actions.settingsResultFormat8Digits);
    m_menus.precision->addAction(m_actions.settingsResultFormat15Digits);
    m_menus.precision->addAction(m_actions.settingsResultFormat50Digits);

    m_menus.resultFormat->addSeparator();

    m_menus.resultFormat->addAction(m_actions.settingsResultFormatBinary);
    m_menus.resultFormat->addAction(m_actions.settingsResultFormatOctal);
    m_menus.resultFormat->addAction(m_actions.settingsResultFormatHexadecimal);

    m_menus.resultFormat->addSeparator();

    m_menus.radixChar = m_menus.resultFormat->addMenu("");
    m_menus.radixChar->addAction(m_actions.settingsRadixCharDefault);
    m_menus.radixChar->addSeparator();
    m_menus.radixChar->addAction(m_actions.settingsRadixCharDot);
    m_menus.radixChar->addAction(m_actions.settingsRadixCharComma);

    m_menus.angleUnit = m_menus.settings->addMenu("");
    m_menus.angleUnit->addAction(m_actions.settingsAngleUnitRadian);
    m_menus.angleUnit->addAction(m_actions.settingsAngleUnitDegree);

    m_menus.behavior = m_menus.settings->addMenu("");
    m_menus.behavior->addAction(m_actions.settingsBehaviorSaveHistoryOnExit);
    m_menus.behavior->addAction(m_actions.settingsBehaviorSaveVariablesOnExit);
    m_menus.behavior->addAction(m_actions.settingsBehaviorSaveUserFunctionsOnExit);
    m_menus.behavior->addAction(m_actions.settingsBehaviorSaveWindowPositionOnExit);
    m_menus.behavior->addSeparator();
    m_menus.behavior->addAction(m_actions.settingsBehaviorPartialResults);
    m_menus.behavior->addAction(m_actions.settingsBehaviorAutoAns);
    m_menus.behavior->addAction(m_actions.settingsBehaviorAutoCompletion);
    m_menus.behavior->addAction(m_actions.settingsBehaviorSyntaxHighlighting);

    m_menus.digitGrouping = m_menus.behavior->addMenu("");
    m_menus.digitGrouping->addAction(m_actions.settingsBehaviorDigitGroupingNone);
    m_menus.digitGrouping->addAction(m_actions.settingsBehaviorDigitGroupingOneSpace);
    m_menus.digitGrouping->addAction(m_actions.settingsBehaviorDigitGroupingTwoSpaces);
    m_menus.digitGrouping->addAction(m_actions.settingsBehaviorDigitGroupingThreeSpaces);

    m_menus.behavior->addAction(m_actions.settingsBehaviorLeaveLastExpression);
    m_menus.behavior->addAction(m_actions.settingsBehaviorParseAllRadixChar);
    m_menus.behavior->addAction(m_actions.settingsBehaviorStrictDigitGrouping);
    m_menus.behavior->addSeparator();
    m_menus.behavior->addAction(m_actions.settingsBehaviorAlwaysOnTop);
    m_menus.behavior->addAction(m_actions.settingsBehaviorMinimizeToTray);
    m_menus.behavior->addAction(m_actions.settingsBehaviorAutoResultToClipboard);

    m_menus.display = m_menus.settings->addMenu("");
    m_menus.colorScheme = m_menus.display->addMenu("");
    m_menus.colorScheme->addAction(m_actions.settingsDisplayColorSchemeStandard);
    m_menus.colorScheme->addAction(m_actions.settingsDisplayColorSchemeSublime);
    m_menus.colorScheme->addAction(m_actions.settingsDisplayColorSchemeTerminal);
    m_menus.display->addAction(m_actions.settingsDisplayFont);

    m_menus.settings->addAction(m_actions.settingsLanguage);

    m_menus.help = new QMenu("", this);
    menuBar()->addMenu(m_menus.help);
    m_menus.help->addAction(m_actions.helpManual);
    m_menus.help->addSeparator();
    m_menus.help->addAction(m_actions.helpUpdates);
    m_menus.help->addAction(m_actions.helpFeedback);
    m_menus.help->addAction(m_actions.helpCommunity);
    m_menus.help->addAction(m_actions.helpNews);
    m_menus.help->addSeparator();
    m_menus.help->addAction(m_actions.helpAbout);

    addActions(menuBar()->actions());
}

void MainWindow::setMenusText()
{
    m_menus.session->setTitle(MainWindow::tr("&Session"));
    m_menus.sessionExport->setTitle(MainWindow::tr("&Export"));
    m_menus.edit->setTitle(MainWindow::tr("&Edit"));
    m_menus.view->setTitle(MainWindow::tr("&View"));
    m_menus.settings->setTitle(MainWindow::tr("Se&ttings"));
    m_menus.resultFormat->setTitle(MainWindow::tr("Result &Format"));
    m_menus.radixChar->setTitle(MainWindow::tr("Radix &Character"));
    m_menus.decimal->setTitle(MainWindow::tr("&Decimal"));
    m_menus.precision->setTitle(MainWindow::tr("&Precision"));
    m_menus.angleUnit->setTitle(MainWindow::tr("&Angle Unit"));
    m_menus.behavior->setTitle(MainWindow::tr("&Behavior"));
    m_menus.display->setTitle(MainWindow::tr("&Display"));
    m_menus.colorScheme->setTitle(MainWindow::tr("Color Scheme"));
    m_menus.help->setTitle(MainWindow::tr("&Help"));
    m_menus.digitGrouping->setTitle(MainWindow::tr("Digit Grouping"));
}

void MainWindow::createStatusBar()
{
    QStatusBar* bar = statusBar();

    m_status.angleUnit = new QPushButton(bar);
    m_status.resultFormat = new QPushButton(bar);

    m_status.angleUnit->setFlat(true);
    m_status.resultFormat->setFlat(true);

    m_status.angleUnit->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_status.angleUnit->addAction(m_actions.settingsAngleUnitRadian);
    m_status.angleUnit->addAction(m_actions.settingsAngleUnitDegree);

    m_status.resultFormat->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_status.resultFormat, SIGNAL(customContextMenuRequested(const QPoint&)),
        SLOT(showResultFormatContextMenu(const QPoint&)));

    connect(m_status.angleUnit, SIGNAL(clicked()), SLOT(cycleAngleUnits()));
    connect(m_status.resultFormat, SIGNAL(clicked()), SLOT(cycleResultFormats()));

    bar->addWidget(m_status.angleUnit);
    bar->addWidget(m_status.resultFormat);

    setStatusBarText();
}

void MainWindow::createFixedWidgets()
{
    m_constants = Constants::instance();
    m_evaluator = Evaluator::instance();
    m_functions = FunctionRepo::instance();

    m_widgets.root = new QWidget(this);
    setCentralWidget(m_widgets.root);

    m_layouts.root = new QVBoxLayout(m_widgets.root);
    m_layouts.root->setSpacing(0);
    m_layouts.root->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout* displayLayout = new QHBoxLayout();
    displayLayout->setSpacing(0);
    m_widgets.display = new ResultDisplay(m_widgets.root);
    m_widgets.display->setFrameStyle(QFrame::NoFrame);
    displayLayout->addWidget(m_widgets.display);
    m_layouts.root->addLayout(displayLayout);

    QHBoxLayout* editorLayout = new QHBoxLayout();
    editorLayout->setSpacing(0);
    m_widgets.editor = new Editor(m_widgets.root);
    m_widgets.editor->setFrameStyle(QFrame::NoFrame);
    m_widgets.editor->setFocus();
    editorLayout->addWidget(m_widgets.editor);
    m_layouts.root->addLayout(editorLayout);

    m_widgets.state = new QLabel(this);
    m_widgets.state->setPalette(QToolTip::palette());
    m_widgets.state->setAutoFillBackground(true);
    m_widgets.state->setFrameShape(QFrame::Box);
    m_widgets.state->hide();

#ifndef Q_OS_MAC
    if (shouldAllowHiddenMenuBar()) {
        m_widgets.tip = new TipWidget(this);
        m_widgets.tip->hide();
    }
#endif // Q_OS_MAC
}

void MainWindow::createBitField() {
    m_widgets.bitField = new BitFieldWidget(m_widgets.root);
    m_layouts.root->addWidget(m_widgets.bitField);
    m_widgets.bitField->show();
    m_widgets.display->scrollToBottom();
    connect(m_widgets.bitField, SIGNAL(bitsChanged(const QString&)), SLOT(handleBitsChanged(const QString&)));
    m_settings->bitfieldVisible = true;
}

void MainWindow::createBookDock()
{
    m_docks.book = new BookDock(this);
    m_docks.book->setObjectName("BookDock");
    m_docks.book->installEventFilter(this);
    m_docks.book->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::RightDockWidgetArea, m_docks.book);

    connect(m_docks.book, SIGNAL(expressionSelected(const QString&)),
        SLOT(insertTextIntoEditor(const QString&)));

    if (m_docks.functions)
        tabifyDockWidget(m_docks.functions, m_docks.book);
    else if (m_docks.variables)
        tabifyDockWidget(m_docks.variables, m_docks.book);
    else if (m_docks.userFunctions)
        tabifyDockWidget(m_docks.userFunctions, m_docks.book);
    else if (m_docks.history)
        tabifyDockWidget(m_docks.history, m_docks.book);
    else if (m_docks.constants)
        tabifyDockWidget(m_docks.constants, m_docks.book);

    m_docks.book->show();
    m_docks.book->raise();

    m_settings->formulaBookDockVisible = true;
}

void MainWindow::createConstantsDock()
{
    m_docks.constants = new ConstantsDock(this);
    m_docks.constants->setObjectName("ConstantsDock");
    m_docks.constants->installEventFilter(this);
    m_docks.constants->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::RightDockWidgetArea, m_docks.constants);

    connect(m_docks.constants->widget(), SIGNAL(constantSelected(const QString&)),
        SLOT(insertConstantIntoEditor(const QString&)));
    connect(this, SIGNAL(radixCharacterChanged()), m_docks.constants->widget(),
        SLOT(handleRadixCharacterChange()));

    if (m_docks.functions)
        tabifyDockWidget(m_docks.functions, m_docks.constants);
    else if (m_docks.variables)
        tabifyDockWidget(m_docks.variables, m_docks.constants);
    else if (m_docks.userFunctions)
        tabifyDockWidget(m_docks.userFunctions, m_docks.constants);
    else if (m_docks.history)
        tabifyDockWidget(m_docks.history, m_docks.constants);
    else if (m_docks.book)
        tabifyDockWidget(m_docks.book, m_docks.constants);

    m_docks.constants->show();
    m_docks.constants->raise();

    m_settings->constantsDockVisible = true;
}

void MainWindow::createFunctionsDock()
{
    m_docks.functions = new FunctionsDock(this);
    m_docks.functions->setObjectName("FunctionsDock");
    m_docks.functions->installEventFilter(this);
    m_docks.functions->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::RightDockWidgetArea, m_docks.functions);

    connect(m_docks.functions->widget(), SIGNAL(functionSelected(const QString&)), SLOT(insertFunctionIntoEditor(const QString&)));

    if (m_docks.history)
        tabifyDockWidget(m_docks.history, m_docks.functions);
    else if (m_docks.variables)
        tabifyDockWidget(m_docks.variables, m_docks.functions);
    else if (m_docks.userFunctions)
        tabifyDockWidget(m_docks.userFunctions, m_docks.functions);
    else if (m_docks.constants)
        tabifyDockWidget(m_docks.constants, m_docks.functions);
    else if (m_docks.book)
        tabifyDockWidget(m_docks.book, m_docks.functions);

    m_docks.functions->show();
    m_docks.functions->raise();

    m_settings->functionsDockVisible = true;
}

void MainWindow::createHistoryDock()
{
    m_docks.history = new HistoryDock(this);
    m_docks.history->setObjectName("HistoryDock");
    m_docks.history->installEventFilter(this);
    m_docks.history->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::RightDockWidgetArea, m_docks.history);

    HistoryWidget* history = qobject_cast<HistoryWidget*>(m_docks.history->widget());
    connect(history, SIGNAL(expressionSelected(const QString&)), SLOT(insertTextIntoEditor(const QString&)));

    HistoryWidget* historyWidget = qobject_cast<HistoryWidget *>(m_docks.history->widget());
    historyWidget->setHistory(m_widgets.editor->history());

    if (m_docks.functions)
        tabifyDockWidget(m_docks.functions, m_docks.history);
    else if (m_docks.variables)
        tabifyDockWidget(m_docks.variables, m_docks.history);
    else if (m_docks.userFunctions)
        tabifyDockWidget(m_docks.userFunctions, m_docks.history);
    else if (m_docks.constants)
        tabifyDockWidget(m_docks.constants, m_docks.history);
    else if (m_docks.book)
        tabifyDockWidget(m_docks.book, m_docks.history);

    m_docks.history->show();
    m_docks.history->raise();

    m_settings->historyDockVisible = true;
}

void MainWindow::createVariablesDock()
{
    m_docks.variables = new VariablesDock(this);
    m_docks.variables->setObjectName("VariablesDock");
    m_docks.variables->installEventFilter(this);
    m_docks.variables->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::RightDockWidgetArea, m_docks.variables);

    connect(m_docks.variables, SIGNAL(variableSelected(const QString&)), SLOT(insertVariableIntoEditor(const QString&)));
    connect(this, SIGNAL(radixCharacterChanged()), m_docks.variables, SLOT(handleRadixCharacterChange()));

    m_docks.variables->updateList();

    if (m_docks.functions)
        tabifyDockWidget(m_docks.functions, m_docks.variables);
    else if (m_docks.userFunctions)
        tabifyDockWidget(m_docks.userFunctions, m_docks.variables);
    else if (m_docks.history)
        tabifyDockWidget(m_docks.history, m_docks.variables);
    else if (m_docks.constants)
        tabifyDockWidget(m_docks.constants, m_docks.variables);
    else if (m_docks.book)
        tabifyDockWidget(m_docks.book, m_docks.variables);

    m_docks.variables->show();
    m_docks.variables->raise();

    m_settings->variablesDockVisible = true;
}

void MainWindow::createUserFunctionsDock()
{
    m_docks.userFunctions = new UserFunctionsDock(this);
    m_docks.userFunctions->setObjectName("UserFunctionsDock");
    m_docks.userFunctions->installEventFilter(this);
    m_docks.userFunctions->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::RightDockWidgetArea, m_docks.userFunctions);

    connect(m_docks.userFunctions, SIGNAL(userFunctionSelected(const QString&)), SLOT(insertUserFunctionIntoEditor(const QString&)));
    connect(m_docks.userFunctions, SIGNAL(userFunctionEdited(const QString&)), SLOT(insertUserFunctionIntoEditor(const QString&)));
    connect(this, SIGNAL(radixCharacterChanged()), m_docks.userFunctions, SLOT(handleRadixCharacterChange()));

    m_docks.userFunctions->updateList();

    if (m_docks.functions)
        tabifyDockWidget(m_docks.functions, m_docks.userFunctions);
    else if (m_docks.variables)
        tabifyDockWidget(m_docks.variables, m_docks.userFunctions);
    else if (m_docks.history)
        tabifyDockWidget(m_docks.history, m_docks.userFunctions);
    else if (m_docks.constants)
        tabifyDockWidget(m_docks.constants, m_docks.userFunctions);
    else if (m_docks.book)
        tabifyDockWidget(m_docks.book, m_docks.userFunctions);

    m_docks.userFunctions->show();
    m_docks.userFunctions->raise();

    m_settings->userFunctionsDockVisible = true;
}

void MainWindow::createFixedConnections()
{
    connect(m_actions.sessionExportHtml, SIGNAL(triggered()), SLOT(exportHtml()));
    connect(m_actions.sessionExportPlainText, SIGNAL(triggered()), SLOT(exportPlainText()));
    connect(m_actions.sessionImport, SIGNAL(triggered()), SLOT(showSessionImportDialog()));
    connect(m_actions.sessionLoad, SIGNAL(triggered()), SLOT(showSessionLoadDialog()));
    connect(m_actions.sessionQuit, SIGNAL(triggered()), SLOT(close()));
    connect(m_actions.sessionSave, SIGNAL(triggered()), SLOT(saveSession()));

    connect(m_actions.editClearExpression, SIGNAL(triggered()), SLOT(clearEditor()));
    connect(m_actions.editClearHistory, SIGNAL(triggered()), SLOT(clearHistory()));
    connect(m_actions.editCopyLastResult, SIGNAL(triggered()), SLOT(copyResultToClipboard()));
    connect(m_actions.editCopy, SIGNAL(triggered()), SLOT(copy()));
    connect(m_actions.editPaste, SIGNAL(triggered()), m_widgets.editor, SLOT(paste()));
    connect(m_actions.editSelectExpression, SIGNAL(triggered()), SLOT(selectEditorExpression()));

    connect(m_actions.viewConstants, SIGNAL(toggled(bool)), SLOT(setConstantsDockVisible(bool)));
    connect(m_actions.viewFullScreenMode, SIGNAL(toggled(bool)), SLOT(setFullScreenEnabled(bool)));
    connect(m_actions.viewFunctions, SIGNAL(toggled(bool)), SLOT(setFunctionsDockVisible(bool)));
    connect(m_actions.viewHistory, SIGNAL(toggled(bool)), SLOT(setHistoryDockVisible(bool)));
    connect(m_actions.viewFormulaBook, SIGNAL(toggled(bool)), SLOT(setFormulaBookDockVisible(bool)));
#ifndef Q_OS_MAC
    if (shouldAllowHiddenMenuBar())
        connect(m_actions.viewMenuBar, SIGNAL(toggled(bool)), SLOT(setMenuBarVisible(bool)));
#endif
    connect(m_actions.viewStatusBar, SIGNAL(toggled(bool)), SLOT(setStatusBarVisible(bool)));
    connect(m_actions.viewVariables, SIGNAL(toggled(bool)), SLOT(setVariablesDockVisible(bool)));
    connect(m_actions.viewBitfield, SIGNAL(toggled(bool)), SLOT(setBitfieldVisible(bool)));
    connect(m_actions.viewUserFunctions, SIGNAL(toggled(bool)), SLOT(setUserFunctionsDockVisible(bool)));

    connect(m_actions.settingsAngleUnitDegree, SIGNAL(triggered()), SLOT(setAngleModeDegree()));
    connect(m_actions.settingsAngleUnitRadian, SIGNAL(triggered()), SLOT(setAngleModeRadian()));

    connect(m_actions.settingsBehaviorAlwaysOnTop, SIGNAL(toggled(bool)), SLOT(setAlwaysOnTopEnabled(bool)));
    connect(m_actions.settingsBehaviorAutoCompletion, SIGNAL(toggled(bool)), SLOT(setAutoCompletionEnabled(bool)));
    connect(m_actions.settingsBehaviorMinimizeToTray, SIGNAL(toggled(bool)), SLOT(setSystemTrayIconEnabled(bool)));
    connect(m_actions.settingsBehaviorAutoAns, SIGNAL(toggled(bool)), SLOT(setAutoAnsEnabled(bool)));
    connect(m_actions.settingsBehaviorPartialResults, SIGNAL(toggled(bool)), SLOT(setAutoCalcEnabled(bool)));
    connect(m_actions.settingsBehaviorSaveHistoryOnExit, SIGNAL(toggled(bool)), SLOT(setHistorySaveEnabled(bool)));
    connect(m_actions.settingsBehaviorSaveVariablesOnExit, SIGNAL(toggled(bool)), SLOT(setVariableSaveEnabled(bool)));
    connect(m_actions.settingsBehaviorSaveUserFunctionsOnExit, SIGNAL(toggled(bool)), SLOT(setUserFunctionSaveEnabled(bool)));
    connect(m_actions.settingsBehaviorSaveWindowPositionOnExit, SIGNAL(toggled(bool)), SLOT(setWindowPositionSaveEnabled(bool)));
    connect(m_actions.settingsBehaviorSyntaxHighlighting, SIGNAL(toggled(bool)), SLOT(setSyntaxHighlightingEnabled(bool)));
    connect(m_actionGroups.digitGrouping, SIGNAL(triggered(QAction*)), SLOT(setDigitGrouping(QAction*)));
    connect(m_actions.settingsBehaviorLeaveLastExpression, SIGNAL(toggled(bool)), SLOT(setLeaveLastExpressionEnabled(bool)));
    connect(m_actions.settingsBehaviorAutoResultToClipboard, SIGNAL(toggled(bool)), SLOT(setAutoResultToClipboardEnabled(bool)));
    connect(m_actions.settingsBehaviorParseAllRadixChar, SIGNAL(toggled(bool)), SLOT(setParseAllRadixChar(bool)));
    connect(m_actions.settingsBehaviorStrictDigitGrouping, SIGNAL(toggled(bool)), SLOT(setStrictDigitGrouping(bool)));

    connect(m_actions.settingsRadixCharComma, SIGNAL(triggered()), SLOT(setRadixCharacterComma()));
    connect(m_actions.settingsRadixCharDefault, SIGNAL(triggered()), SLOT(setRadixCharacterAutomatic()));
    connect(m_actions.settingsRadixCharDot, SIGNAL(triggered()), SLOT(setRadixCharacterDot()));

    connect(m_actions.settingsResultFormat15Digits, SIGNAL(triggered()), SLOT(setResultPrecision15Digits()));
    connect(m_actions.settingsResultFormat2Digits, SIGNAL(triggered()), SLOT(setResultPrecision2Digits()));
    connect(m_actions.settingsResultFormat3Digits, SIGNAL(triggered()), SLOT(setResultPrecision3Digits()));
    connect(m_actions.settingsResultFormat50Digits, SIGNAL(triggered()), SLOT(setResultPrecision50Digits()));
    connect(m_actions.settingsResultFormat8Digits, SIGNAL(triggered()), SLOT(setResultPrecision8Digits()));
    connect(m_actions.settingsResultFormatAutoPrecision, SIGNAL(triggered()), SLOT(setResultPrecisionAutomatic()));
    connect(m_actions.settingsResultFormatBinary, SIGNAL(triggered()), SLOT(setResultFormatBinary()));
    connect(m_actions.settingsResultFormatEngineering, SIGNAL(triggered()), SLOT(setResultFormatEngineering()));
    connect(m_actions.settingsResultFormatFixed, SIGNAL(triggered()), SLOT(setResultFormatFixed()));
    connect(m_actions.settingsResultFormatGeneral, SIGNAL(triggered()), SLOT(setResultFormatGeneral()));
    connect(m_actions.settingsResultFormatHexadecimal, SIGNAL(triggered()), SLOT(setResultFormatHexadecimal()));
    connect(m_actions.settingsResultFormatOctal, SIGNAL(triggered()), SLOT(setResultFormatOctal()));
    connect(m_actions.settingsResultFormatScientific, SIGNAL(triggered()), SLOT(setResultFormatScientific()));

    connect(m_actions.settingsLanguage, SIGNAL(triggered()), SLOT(showLanguageChooserDialog()));

    connect(m_actions.helpManual, SIGNAL(triggered()), SLOT(showManualWindow()));
    connect(m_actions.helpUpdates, SIGNAL(triggered()), SLOT(openUpdatesURL()));
    connect(m_actions.helpFeedback, SIGNAL(triggered()), SLOT(openFeedbackURL()));
    connect(m_actions.helpCommunity, SIGNAL(triggered()), SLOT(openCommunityURL()));
    connect(m_actions.helpNews, SIGNAL(triggered()), SLOT(openNewsURL()));
    connect(m_actions.helpAbout, SIGNAL(triggered()), SLOT(showAboutDialog()));

    connect(m_widgets.editor, SIGNAL(autoCalcDisabled()), SLOT(hideStateLabel()));
    connect(m_widgets.editor, SIGNAL(autoCalcEnabled(const QString&)), SLOT(showStateLabel(const QString&)));
    connect(m_widgets.editor, SIGNAL(returnPressed()), SLOT(evaluateEditorExpression()));
    connect(m_widgets.editor, SIGNAL(shiftDownPressed()), SLOT(decreaseDisplayFontPointSize()));
    connect(m_widgets.editor, SIGNAL(shiftUpPressed()), SLOT(increaseDisplayFontPointSize()));
    connect(m_widgets.editor, SIGNAL(controlPageUpPressed()), m_widgets.display, SLOT(scrollToTop()));
    connect(m_widgets.editor, SIGNAL(controlPageDownPressed()), m_widgets.display, SLOT(scrollToBottom()));
    connect(m_widgets.editor, SIGNAL(shiftPageUpPressed()), m_widgets.display, SLOT(scrollLineUp()));
    connect(m_widgets.editor, SIGNAL(shiftPageDownPressed()), m_widgets.display, SLOT(scrollLineDown()));
    connect(m_widgets.editor, SIGNAL(pageUpPressed()), m_widgets.display, SLOT(scrollPageUp()));
    connect(m_widgets.editor, SIGNAL(pageDownPressed()), m_widgets.display, SLOT(scrollPageDown()));
    connect(m_widgets.editor, SIGNAL(textChanged()), SLOT(handleEditorTextChange()));
    connect(m_widgets.editor, SIGNAL(copyAvailable(bool)), SLOT(handleCopyAvailable(bool)));
    connect(m_widgets.editor, SIGNAL(copySequencePressed()), SLOT(copy()));
    connect(m_widgets.editor, SIGNAL(selectionChanged()), SLOT(handleEditorSelectionChange()));

    connect(m_widgets.display, SIGNAL(copyAvailable(bool)), SLOT(handleCopyAvailable(bool)));
    connect(m_widgets.display, SIGNAL(expressionSelected(const QString&)), SLOT(insertTextIntoEditor(const QString&)));
    connect(m_widgets.display, SIGNAL(selectionChanged()), SLOT(handleDisplaySelectionChange()));
    connect(m_widgets.display, SIGNAL(shiftWheelUp()), SLOT(increaseDisplayFontPointSize()));
    connect(m_widgets.display, SIGNAL(shiftWheelDown()), SLOT(decreaseDisplayFontPointSize()));
    connect(m_widgets.display, SIGNAL(shiftControlWheelDown()), SLOT(decreaseOpacity()));
    connect(m_widgets.display, SIGNAL(shiftControlWheelUp()), SLOT(increaseOpacity()));

    connect(this, SIGNAL(radixCharacterChanged()), m_widgets.display, SLOT(refresh()));
    connect(this, SIGNAL(resultFormatChanged()), m_widgets.display, SLOT(refresh()));
    connect(this, SIGNAL(resultPrecisionChanged()), m_widgets.display, SLOT(refresh()));
    connect(this, SIGNAL(colorSchemeChanged()), m_widgets.display, SLOT(rehighlight()));
    connect(this, SIGNAL(colorSchemeChanged()), m_widgets.editor, SLOT(rehighlight()));
    connect(this, SIGNAL(syntaxHighlightingChanged()), m_widgets.display, SLOT(rehighlight()));
    connect(this, SIGNAL(syntaxHighlightingChanged()), m_widgets.editor, SLOT(rehighlight()));

    connect(m_actions.settingsDisplayFont, SIGNAL(triggered()), SLOT(showFontDialog()));

    QList<QAction*> colorSchemeActions = m_actionGroups.colorScheme->actions();
    for (int i = 0; i < colorSchemeActions.size(); ++i)
        connect(colorSchemeActions.at(i), SIGNAL(triggered()), SLOT(applySelectedColorScheme()));

    connect(this, SIGNAL(languageChanged()), SLOT(retranslateText()));
}

void MainWindow::applySettings()
{
    emit languageChanged();

    m_actions.viewFormulaBook->setChecked(m_settings->formulaBookDockVisible);
    m_actions.viewConstants->setChecked(m_settings->constantsDockVisible);
    m_actions.viewFunctions->setChecked(m_settings->functionsDockVisible);
    m_actions.viewHistory->setChecked(m_settings->historyDockVisible);
    m_actions.viewVariables->setChecked(m_settings->variablesDockVisible);
    m_actions.viewBitfield->setChecked(m_settings->bitfieldVisible);
    m_actions.viewUserFunctions->setChecked(m_settings->userFunctionsDockVisible);

    resize(m_settings->windowSize);

    if (m_settings->maximized)
        showMaximized();
    else if (m_settings->windowPosition.isNull()) {
        QDesktopWidget* desktop = QApplication::desktop();
        QRect screen = desktop->availableGeometry(this);
        move(screen.center() - rect().center());
    } else
        move(m_settings->windowPosition);

    restoreState(m_settings->windowState);

    m_actions.viewFullScreenMode->setChecked(m_settings->windowOnfullScreen);
    m_actions.settingsBehaviorAlwaysOnTop->setChecked(m_settings->windowAlwaysOnTop);

    if (m_settings->angleUnit == 'r')
        m_actions.settingsAngleUnitRadian->setChecked(true);
    else if (m_settings->angleUnit == 'd')
        m_actions.settingsAngleUnitDegree->setChecked(true);

    if (m_settings->historySave) {
        m_actions.settingsBehaviorSaveHistoryOnExit->setChecked(true);
        restoreHistory();
    }

    m_actions.settingsBehaviorLeaveLastExpression->setChecked(m_settings->leaveLastExpression);
    m_actions.settingsBehaviorSaveWindowPositionOnExit->setChecked(m_settings->windowPositionSave);

    if (m_settings->variableSave) {
        m_actions.settingsBehaviorSaveVariablesOnExit->setChecked(true);
        restoreVariables();
    }

    if (m_settings->userFunctionSave) {
        m_actions.settingsBehaviorSaveUserFunctionsOnExit->setChecked(true);
        restoreUserFunctions();
    }

    checkInitialResultFormat();
    checkInitialResultPrecision();

    if (m_settings->isRadixCharacterAuto())
        m_actions.settingsRadixCharDefault->setChecked(true);
    else if (m_settings->radixCharacter() == '.')
        m_actions.settingsRadixCharDot->setChecked(true);
    else if (m_settings->radixCharacter() == ',')
        m_actions.settingsRadixCharComma->setChecked(true);

    if (m_settings->autoAns)
        m_actions.settingsBehaviorAutoAns->setChecked(true);
    else
        setAutoAnsEnabled(false);

    if (m_settings->autoCalc)
        m_actions.settingsBehaviorPartialResults->setChecked(true);
    else
        setAutoCalcEnabled(false);

    if (m_settings->autoCompletion)
        m_actions.settingsBehaviorAutoCompletion->setChecked(true);
    else
        setAutoCompletionEnabled(false);

    m_actions.settingsBehaviorMinimizeToTray->setChecked(m_settings->systemTrayIconVisible);

    checkInitialDigitGrouping();

    if (m_settings->syntaxHighlighting)
        m_actions.settingsBehaviorSyntaxHighlighting->setChecked(true);
    else
        setSyntaxHighlightingEnabled(false);

    if (m_settings->autoResultToClipboard)
        m_actions.settingsBehaviorAutoResultToClipboard->setChecked(true);
    else
        setAutoResultToClipboardEnabled(false);

    m_actions.settingsBehaviorParseAllRadixChar->setChecked(m_settings->parseAllRadixChar);
    m_actions.settingsBehaviorStrictDigitGrouping->setChecked(m_settings->strictDigitGrouping);

    QFont font;
    font.fromString(m_settings->displayFont);
    m_widgets.display->setFont(font);
    m_widgets.editor->setFont(font);

    if (m_settings->colorScheme == SyntaxHighlighter::Standard)
        m_actions.settingsDisplayColorSchemeStandard->setChecked(true);
    else if (m_settings->colorScheme == SyntaxHighlighter::Sublime)
        m_actions.settingsDisplayColorSchemeSublime->setChecked(true);
    else if (m_settings->colorScheme == SyntaxHighlighter::Terminal)
        m_actions.settingsDisplayColorSchemeTerminal->setChecked(true);

    m_actions.viewStatusBar->setChecked(m_settings->statusBarVisible);

#ifndef Q_OS_MAC
    if (shouldAllowHiddenMenuBar()) {
        m_actions.viewMenuBar->setChecked(m_settings->menuBarVisible);
        menuBar()->setVisible(m_settings->menuBarVisible);
    }
#endif

    if (m_widgets.display->isEmpty())
        QTimer::singleShot(0, this, SLOT(showReadyMessage()));

    QTimer::singleShot(100, m_widgets.editor, SLOT(setFocus()));
}

void MainWindow::showManualWindow()
{
    if (m_widgets.manual) {
        m_widgets.manual->raise();
        m_widgets.manual->activateWindow();
        return;
    }

    m_widgets.manual = new ManualWindow();
    m_widgets.manual->show();
    connect(m_widgets.manual, SIGNAL(windowClosed()), SLOT(handleManualClosed()));
}

void MainWindow::showReadyMessage()
{
    showStateLabel(tr("Type an expression here"));
}

void MainWindow::checkInitialResultFormat()
{
    switch (m_settings->resultFormat) {
        case 'g': m_actions.settingsResultFormatGeneral->setChecked(true); break;
        case 'n': m_actions.settingsResultFormatEngineering->setChecked(true); break;
        case 'e': m_actions.settingsResultFormatScientific->setChecked(true); break;
        case 'h': m_actions.settingsResultFormatHexadecimal->setChecked(true); break;
        case 'o': m_actions.settingsResultFormatOctal->setChecked(true); break;
        case 'b': m_actions.settingsResultFormatBinary->setChecked(true); break;
        default : m_actions.settingsResultFormatFixed->setChecked(true);
    }
}

void MainWindow::checkInitialResultPrecision()
{
    switch (m_settings->resultPrecision) {
        case 2: m_actions.settingsResultFormat2Digits->setChecked(true); break;
        case 3: m_actions.settingsResultFormat3Digits->setChecked(true); break;
        case 8: m_actions.settingsResultFormat8Digits->setChecked(true); break;
        case 15: m_actions.settingsResultFormat15Digits->setChecked(true); break;
        case 50: m_actions.settingsResultFormat50Digits->setChecked(true); break;
        default: m_actions.settingsResultFormatAutoPrecision->setChecked(true);
    }
}

void MainWindow::checkInitialDigitGrouping()
{
    switch (m_settings->digitGrouping) {
        case 1: m_actions.settingsBehaviorDigitGroupingOneSpace->setChecked(true); break;
        case 2: m_actions.settingsBehaviorDigitGroupingTwoSpaces->setChecked(true); break;
        case 3: m_actions.settingsBehaviorDigitGroupingThreeSpaces->setChecked(true); break;
        default:
        case 0: m_actions.settingsBehaviorDigitGroupingNone->setChecked(true); break;
    }
}

void MainWindow::saveSettings()
{
    if (m_settings->historySave) {
        m_settings->history = m_widgets.editor->history();
        m_settings->historyResults = m_widgets.editor->historyResults();
    } else {
        m_settings->history.clear();
        m_settings->historyResults.clear();
    }

    if (m_settings->variableSave) {
        m_settings->variables.clear();
        QList<Evaluator::Variable> variables = m_evaluator->getUserDefinedVariablesPlusAns();
        for (int i = 0; i < variables.count(); ++i) {
            QString name = variables.at(i).name;
            char* value = HMath::format(variables.at(i).value, 'e', DECPRECISION);
            m_settings->variables.append(QString("%1=%2").arg(name).arg(value));
            free(value);
        }
    }

    if (m_settings->userFunctionSave) {
        QList<Evaluator::UserFunctionDescr> userFunctions = m_evaluator->getUserFunctions();
        for (int i = 0; i < userFunctions.count(); ++i) {
            Evaluator::UserFunctionDescr descr = userFunctions.at(i);
            QStringList funcParts;
            funcParts << descr.name << descr.arguments << descr.expression;
            m_settings->userFunctions.append(funcParts);
        }
    }

    m_settings->windowPosition = m_settings->windowPositionSave ? pos() : QPoint(0, 0);
    m_settings->windowSize = size();
    m_settings->windowState = saveState();
    m_settings->displayFont = m_widgets.display->font().toString();
    m_settings->maximized = isMaximized();

    m_settings->save();
}

MainWindow::MainWindow()
    : QMainWindow()
{
    m_translator = 0;
    m_settings = Settings::instance();

    m_widgets.trayIcon = 0;
    m_widgets.tip = 0;
    m_widgets.manual = 0;

    m_menus.trayIcon = 0;

    m_conditions.autoAns = false;
    m_conditions.trayNotify = true;
#ifndef Q_OS_MAC
    m_conditions.notifyMenuBarHidden = true;
#endif

    m_docks.book = 0;
    m_docks.history = 0;
    m_docks.constants = 0;
    m_docks.functions = 0;
    m_docks.variables = 0;
    m_docks.userFunctions = 0;

    m_status.angleUnit = 0;
    m_status.resultFormat = 0;

    m_copyWidget = 0;

    createUi();
    applySettings();
    QTimer::singleShot(0, m_widgets.editor, SLOT(setFocus()));
}

MainWindow::~MainWindow()
{
    if (m_widgets.trayIcon)
        m_widgets.trayIcon->hide();
    if (m_docks.book)
        deleteBookDock();
    if (m_docks.constants)
        deleteConstantsDock();
    if (m_docks.variables)
        deleteVariablesDock();
    if (m_docks.userFunctions)
        deleteUserFunctionsDock();
    if (m_docks.functions)
        deleteFunctionsDock();
    if (m_docks.history)
        deleteHistoryDock();
}

bool MainWindow::event(QEvent* event)
{
    bool conditionsToMinimize =
        (event->type() == QEvent::WindowStateChange)
        && (windowState() & Qt::WindowMinimized)
        && (m_settings->systemTrayIconVisible);

    if (conditionsToMinimize) {
        QTimer::singleShot(100, this, SLOT(minimizeToSystemTray()));
        return true;
    }

    return QMainWindow::event(event);
}

void MainWindow::showAboutDialog()
{
    AboutBox dialog(this);
    dialog.resize(480, 640);
    dialog.exec();
}

void MainWindow::clearHistory()
{
    m_widgets.display->clear();
    m_widgets.editor->clearHistory();
    if (m_settings->historyDockVisible) {
        HistoryWidget* history = qobject_cast<HistoryWidget *>(m_docks.history->widget());
        history->clear();
    }
    m_settings->history.clear();
    m_settings->historyResults.clear();
    QTimer::singleShot(0, m_widgets.editor, SLOT(setFocus()));
}

void MainWindow::clearEditor()
{
    m_widgets.editor->clear();
    QTimer::singleShot(0, m_widgets.editor, SLOT(setFocus()));
}

void MainWindow::copyResultToClipboard()
{
    QClipboard* cb = QApplication::clipboard();
    HNumber num = m_evaluator->getVariable(QLatin1String("ans")).value;
    char* strToCopy = HMath::format(num, m_settings->resultFormat, m_settings->resultPrecision);
    QString final(strToCopy);
    if (m_settings->radixCharacter() == ',')
        final.replace('.', ',');
    cb->setText(final, QClipboard::Clipboard);
    free(strToCopy);
}

void MainWindow::setAngleModeDegree()
{
    if (m_settings->angleUnit == 'd')
        return;

    m_settings->angleUnit = 'd';

    if (m_status.angleUnit)
        m_status.angleUnit->setText(tr("Degree"));

    emit angleUnitChanged();
}

void MainWindow::decreaseOpacity()
{
    if (windowOpacity() > 0.4)
        setWindowOpacity(windowOpacity() - 0.1);
}

void MainWindow::increaseOpacity()
{
    if (windowOpacity() < 1.0)
        setWindowOpacity(windowOpacity() + 0.1);
}

void MainWindow::deleteVariables()
{
    m_evaluator->unsetAllUserDefinedVariables();

    if (m_settings->variablesDockVisible)
        m_docks.variables->updateList();
}

void MainWindow::deleteUserFunctions()
{
    m_evaluator->unsetAllUserFunctions();

    if (m_settings->userFunctionsDockVisible)
        m_docks.userFunctions->updateList();
}

void MainWindow::setResultPrecision2Digits()
{
    setResultPrecision(2);
}

void MainWindow::setResultPrecision3Digits()
{
    setResultPrecision(3);
}

void MainWindow::setResultPrecision8Digits()
{
    setResultPrecision(8);
}

void MainWindow::setResultPrecision15Digits()
{
    setResultPrecision(15);
}

void MainWindow::setResultPrecision50Digits()
{
    setResultPrecision(50);
}

void MainWindow::setResultPrecisionAutomatic()
{
    setResultPrecision(-1);
}

void MainWindow::applySelectedColorScheme()
{
    m_settings->colorScheme = m_actionGroups.colorScheme->checkedAction()->data().toInt();
    emit colorSchemeChanged();
}

void MainWindow::selectEditorExpression()
{
    activateWindow();
    m_widgets.editor->selectAll();
    m_widgets.editor->setFocus();
}

void MainWindow::hideStateLabel()
{
    m_widgets.state->hide();
}

void MainWindow::showSessionLoadDialog()
{
    QString errMsg  = tr("File %1 is not a valid session");
    QString filters = tr("SpeedCrunch Sessions (*.sch);;All Files (*)");
    QString fname = QFileDialog::getOpenFileName(this, tr("Load Session"), QString::null, filters);
    if (fname.isEmpty())
        return;

    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Can't read from file %1").arg(fname));
        return;
    }

    QTextStream stream(&file);

    // Version of the format.
    QString version = stream.readLine();
    if (version != "0.10" && version != "0.12") {
        QMessageBox::critical(this, tr("Error"), errMsg.arg(fname));
        return;
    }

    // Number of calculations.
    bool ok;
    int noCalcs = stream.readLine().toInt(&ok);
    if (ok == false || noCalcs < 0) {
        QMessageBox::critical(this, tr("Error"), errMsg.arg(fname));
        return;
    }

    // Ask for merge with current session.
    QString mergeMsg = tr(
        "Merge session being loaded with current session?\n"
        "If no, current variables and display will be cleared."
    );

    QMessageBox::StandardButton button =
        QMessageBox::question(this, tr("Question"), mergeMsg,
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);

    if (button == QMessageBox::Cancel)
        return;

    if (button == QMessageBox::No) {
        m_widgets.display->clear();
        deleteVariables();
        deleteUserFunctions();
        clearHistory();
    }

    // Expressions and results.
    QStringList expList;
    QStringList resList;
    for (int i = 0; i < noCalcs; ++i) {
        QString exp = stream.readLine();
        QString res = stream.readLine();
        if (exp.isNull() || res.isNull()) {
            QMessageBox::critical(this, tr("Error"), errMsg.arg(fname));
            return;
        }
        expList.append(exp);
        resList.append(res);
    }

    // FIXME: This keeps the history in three (finally even four) different places.
    // There should be just one central history and result list, and the clients
    // editor, display, settings and historyDock should retrieve their data from there.
    // FIXME: Actually there should be three lists: expressions, values and formats.
    m_widgets.display->appendHistory(expList, resList);
    m_widgets.editor->appendHistory(expList, resList);

    if (m_settings->historyDockVisible) {
        HistoryWidget* history = qobject_cast<HistoryWidget *>(m_docks.history->widget());
        history->appendHistory(expList);
    }

    // Variables.
    int noVars = stream.readLine().toInt(&ok);
    if (ok == false || noVars < 0) {
        QMessageBox::critical(this, tr("Error"), errMsg.arg(fname));
        return;
    }
    for (int i = 0; i < noVars; i++) {
        QString var = stream.readLine();
        QString val = stream.readLine();
        if (var.isNull() || val.isNull()) {
            QMessageBox::critical(this, tr("Error"), errMsg.arg(fname));
            return;
        }

        // Only allow the "ans" built-in variable to be set.
        Evaluator::Variable::Type type = m_evaluator->isBuiltInVariable(var) ?
            Evaluator::Variable::BuiltIn : Evaluator::Variable::UserDefined;
        if (type == Evaluator::Variable::BuiltIn && var != "ans")
            continue;

        HNumber num(val.toLatin1().data());
        if (num != HMath::nan())
            m_evaluator->setVariable(var, num, type);
    }

    if (m_settings->variablesDockVisible)
        m_docks.variables->updateList();

    if (version == "0.12") {
        // User functions.
        int noUsrFuncs = stream.readLine().toInt(&ok);
        if (ok == false || noUsrFuncs < 0) {
            QMessageBox::critical(this, tr("Error"), errMsg.arg(fname));
            return;
        }
        for (int i = 0; i < noUsrFuncs; i++) {
            QString name = stream.readLine();
            QString args = stream.readLine();
            QString expr = stream.readLine();
            if (name.isNull() || args.isNull() || expr.isNull()) {
                QMessageBox::critical(this, tr("Error"), errMsg.arg(fname));
                return;
            }
            Evaluator::UserFunctionDescr descr(name, args.split(";"), expr);
            m_evaluator->setUserFunction(descr);
        }

        if (m_settings->userFunctionsDockVisible)
            m_docks.userFunctions->updateList();
    }

    file.close();

}

void MainWindow::showSessionImportDialog()
{
    QString filters = tr("All Files (*)");
    QString fname = QFileDialog::getOpenFileName(this, tr("Import Session"), QString::null, filters);
    if (fname.isEmpty())
        return;

    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Can't read from file %1").arg(fname));
        return;
    }

    // Ask for merge with current session.
    QString mergeMsg = tr(
        "Merge session being imported with current session?\n"
        "If no, current variables and display will be cleared."
    );

    QMessageBox::StandardButton button =
        QMessageBox::question(this, tr("Question"), mergeMsg,
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);

    if (button == QMessageBox::Cancel)
        return;
    if (button == QMessageBox::No) {
        m_widgets.display->clear();
        deleteVariables();
        deleteUserFunctions();
        clearHistory();
    }

    QTextStream stream(&file);
    QString exp = stream.readLine();
    bool ignoreAll = false;
    while (!exp.isNull()) {
        m_widgets.editor->setText(exp);

        QString str = m_evaluator->autoFix(exp);

        m_evaluator->setExpression(str);

        HNumber result = m_evaluator->evalUpdateAns();
        if (!m_evaluator->error().isEmpty()) {
            if (!ignoreAll) {
                QMessageBox::StandardButton button =
                    QMessageBox::warning(this, tr("Error"), tr("Ignore error?") + "\n" + m_evaluator->error(),
                        QMessageBox::Yes | QMessageBox::YesToAll
                        | QMessageBox::Cancel, QMessageBox::Yes);

                if (button == QMessageBox::Cancel)
                    return;
                if (button == QMessageBox::YesToAll)
                    ignoreAll = true;
            }
        } else {
            m_widgets.display->append(str, result);
            if (result.isNan()) {
                m_widgets.editor->appendHistory(str, "");
            } else {
                char* num = HMath::format(result, 'e', DECPRECISION);
                m_widgets.editor->appendHistory(str, num);
                free(num);
                m_widgets.editor->setAnsAvailable(true);
            }

            if (m_settings->variablesDockVisible)
                m_docks.variables->updateList();
            if (m_settings->userFunctionsDockVisible)
                m_docks.userFunctions->updateList();
            if (m_settings->historyDockVisible) {
                HistoryWidget* history = qobject_cast<HistoryWidget*>(m_docks.history->widget());
                history->append(str);
            }

            m_widgets.editor->setText(str);
            m_widgets.editor->selectAll();
            m_widgets.editor->stopAutoCalc();
            m_widgets.editor->stopAutoComplete();
            if(!result.isNan())
                m_conditions.autoAns = true;
        }

        exp = stream.readLine();
    }

    file.close();

    QTimer::singleShot(0, m_widgets.editor, SLOT(setFocus()));

    if (!isActiveWindow())
        activateWindow();
}

void MainWindow::setAlwaysOnTopEnabled(bool b)
{
    m_settings->windowAlwaysOnTop = b;

    QPoint cur = mapToGlobal(QPoint(0, 0));
    if (b)
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    else
        setWindowFlags(windowFlags() & (~ Qt::WindowStaysOnTopHint));
    move(cur);
    show();
}

void MainWindow::setAutoAnsEnabled(bool b)
{
    m_settings->autoAns = b;
}

void MainWindow::setAutoCalcEnabled(bool b)
{
    m_settings->autoCalc = b;
    m_widgets.editor->setAutoCalcEnabled(b);
}

void MainWindow::setHistorySaveEnabled(bool b)
{
    m_settings->historySave = b;
}

void MainWindow::setLeaveLastExpressionEnabled(bool b)
{
    m_settings->leaveLastExpression = b;
}

void MainWindow::setVariableSaveEnabled(bool b)
{
    m_settings->variableSave = b;
}

void MainWindow::setUserFunctionSaveEnabled(bool b)
{
    m_settings->userFunctionSave = b;
}

void MainWindow::setWindowPositionSaveEnabled(bool b)
{
    m_settings->windowPositionSave = b;
}

void MainWindow::setAutoCompletionEnabled(bool b)
{
    m_settings->autoCompletion = b;
    m_widgets.editor->setAutoCompletionEnabled(b);
}

void MainWindow::setBitfieldVisible(bool b)
{
    if (b)
        createBitField();
    else
        deleteBitField();
}

void MainWindow::setSystemTrayIconEnabled(bool b)
{
    if (b && !m_widgets.trayIcon && QSystemTrayIcon::isSystemTrayAvailable()) {
        m_conditions.trayNotify = true;
        m_widgets.trayIcon = new QSystemTrayIcon(this);
        m_widgets.trayIcon->setToolTip("SpeedCrunch");
        m_widgets.trayIcon->setIcon(QPixmap(":/speedcrunch.png"));

        m_menus.trayIcon = new QMenu(this);
        m_menus.trayIcon->addAction(m_actions.editCopyLastResult);
        m_menus.trayIcon->addSeparator();
        m_menus.trayIcon->addAction(m_actions.sessionQuit);

        m_widgets.trayIcon->setContextMenu(m_menus.trayIcon);
        connect(m_widgets.trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            SLOT(handleSystemTrayIconActivation(QSystemTrayIcon::ActivationReason)));
    } else {
        if (m_widgets.trayIcon)
            m_widgets.trayIcon->deleteLater();
        m_widgets.trayIcon = 0;
    }

    m_settings->systemTrayIconVisible = b;
}

void MainWindow::setSyntaxHighlightingEnabled(bool b)
{
    m_settings->syntaxHighlighting = b;
    m_menus.digitGrouping->setEnabled(b);
    emit syntaxHighlightingChanged();
}

void MainWindow::setDigitGrouping(QAction *action)
{
    m_settings->digitGrouping = action->data().toInt();
    emit syntaxHighlightingChanged();
}

void MainWindow::setAutoResultToClipboardEnabled(bool b)
{
    m_settings->autoResultToClipboard = b;
}

void MainWindow::setParseAllRadixChar(bool b)
{
    m_settings->parseAllRadixChar = b;
    emit radixCharacterChanged();
}

void MainWindow::setStrictDigitGrouping(bool b)
{
    m_settings->strictDigitGrouping = b;
    emit radixCharacterChanged();   // FIXME?
}

void MainWindow::setAngleModeRadian()
{
    if (m_settings->angleUnit == 'r')
        return;

    m_settings->angleUnit = 'r';

    if (m_status.angleUnit)
        m_status.angleUnit->setText(tr("Radian"));

    emit angleUnitChanged();
}

void MainWindow::saveSession()
{
    QString filters = tr("SpeedCrunch Sessions (*.sch);;All Files (*)");
    QString fname = QFileDialog::getSaveFileName(this, tr("Save Session"), QString::null, filters);
    if (fname.isEmpty())
        return;

    QFile file(fname);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Can't write to file %1").arg(fname));
        return;
    }

    QTextStream stream(&file);

    // Format version.
    stream << "0.12" << "\n";

#if 1
    QStringList history = m_widgets.editor->history();
    QStringList historyResults = m_widgets.editor->historyResults();

    Q_ASSERT(history.count() == historyResults.count());

    // Number of calculations.
    stream << history.count() << "\n";

    // Expressions and results.
    for (int i = 0; i < history.count(); ++i) {
        stream << history.at(i) << QLatin1String("\n");
        stream << historyResults.at(i) << QLatin1String("\n");
    }
#else
    // Number of calculations.
    stream << m_widgets.display->count() << "\n";

    // Expressions and results.
    QStringList historyLines = m_widgets.display->toPlainText().split(QLatin1String("\n"));
    int exprCount = 0;
    for (int i = 0; i < historyLines.count(); ++i) {
        if (historyLines.at(i).isEmpty())
            continue;

        // If the expression has a result, just remove the equal sign prefix,
        // otherwise, export the result as an empty line.
        if (i + 1 < historyLines.count() && !historyLines.at(i + 1).isEmpty()) {
            // Expression.
            stream << historyLines.at(i) << QLatin1String("\n");
            ++i;
            // Result.
            const QString &result = historyLines.at(i);
            if (result.startsWith(QLatin1String("= ")))
                stream << result.mid(2) << QLatin1String("\n");
            else
                stream << result << QLatin1String("\n");
        } else {
            // User function assignment, so there is no result.
            stream << historyLines.at(i) << QLatin1String("\n\n");
        }
        ++exprCount;
    }

    Q_ASSERT(exprCount == m_widgets.display->count());
#endif

    // Number of variables.
    QList<Evaluator::Variable> variables = m_evaluator->getUserDefinedVariablesPlusAns();
    stream << variables.count() << "\n";

    // Variables.
    for (int i = 0; i < variables.count(); ++i) {
        Evaluator::Variable var = variables.at(i);
        char* value = HMath::format(var.value);
        stream << var.name << "\n" << value << "\n";
        free(value);
    }

    // Number of user functions.
    QList<Evaluator::UserFunctionDescr> userFunctions = m_evaluator->getUserFunctions();
    stream << userFunctions.count() << "\n";

    // User functions.
    for (int i = 0; i < userFunctions.count(); ++i) {
        Evaluator::UserFunctionDescr descr = userFunctions.at(i);
        stream << descr.name << "\n" << descr.arguments.join(";") << "\n" << descr.expression << "\n";
    }

    file.close();
}

inline static QString documentsLocation()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
    return QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
#endif
}

void MainWindow::exportHtml()
{
    QString fname = QFileDialog::getSaveFileName(this, tr("Export session as HTML"),
        documentsLocation());

    if (fname.isEmpty())
        return;

    QFile file(fname);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Can't write to file %1").arg(fname));
        return;
    }

    QTextStream stream(& file);
    stream.setCodec("UTF-8");
    stream << m_widgets.display->document()->toHtml("utf-8");

    file.close();
}

void MainWindow::exportPlainText()
{
    QString fname = QFileDialog::getSaveFileName(this, tr("Export session as plain text"),
        documentsLocation());

    if (fname.isEmpty())
        return;

    QFile file(fname);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Can't write to file %1").arg(fname));
        return;
    }

    QByteArray text;
    QTextStream stream(&text, QIODevice::WriteOnly | QIODevice::Text);
    stream.setCodec("UTF-8");
    stream << m_widgets.display->document()->toPlainText();
    stream.flush();

    file.write(text);
    file.close();
}

void MainWindow::setWidgetsDirection()
{
    QLocale::Language lang = QLocale().language();
    bool rtlSystem = (lang == QLocale::Hebrew || lang == QLocale::Arabic);

    QString code = m_settings->language;
    bool rtlCustom = (code.contains("he") || code.contains("ar"));

    if ((m_settings->language == "C" && rtlSystem) || rtlCustom)
        qApp->setLayoutDirection(Qt::RightToLeft);
    else
        qApp->setLayoutDirection(Qt::LeftToRight);
}

void MainWindow::showFontDialog()
{
    bool ok;
    QFont f = QFontDialog::getFont(&ok, m_widgets.display->font(), this, tr("Display font"));
    if (!ok)
        return;
    m_widgets.display->setFont(f);
    m_widgets.editor->setFont(f);
}

#ifndef Q_OS_MAC
void MainWindow::setMenuBarVisible(bool b)
{
    menuBar()->setVisible(b);
    m_settings->menuBarVisible = b;

    if (!b && m_conditions.notifyMenuBarHidden) {
        showMenuBarTip();
        m_conditions.notifyMenuBarHidden = false;
    }
}
#endif

void MainWindow::setStatusBarVisible(bool b)
{
    b ? createStatusBar() : deleteStatusBar();
    m_settings->statusBarVisible = b;
}

void MainWindow::showStateLabel(const QString& msg)
{
    m_widgets.state->setText(msg);
    m_widgets.state->adjustSize();
    m_widgets.state->show();
    m_widgets.state->raise();
    const int height = m_widgets.state->height();
    QPoint pos = mapFromGlobal(m_widgets.editor->mapToGlobal(QPoint(0, -height)));
    m_widgets.state->move(pos);
}

void MainWindow::setFullScreenEnabled(bool b)
{
    m_settings->windowOnfullScreen = b;
    b ? showFullScreen() : showNormal();
}

bool MainWindow::eventFilter(QObject* o, QEvent* e)
{
    if (o == m_docks.book) {
        if (e->type() == QEvent::Close) {
            deleteBookDock();
            return true;
        }
        return false;
    }

    if (o == m_docks.constants) {
        if (e->type() == QEvent::Close) {
            deleteConstantsDock();
            return true;
        }
        return false;
    }

    if (o == m_docks.functions) {
        if (e->type() == QEvent::Close) {
            deleteFunctionsDock();
            return true;
        }
        return false;
    }

    if (o == m_docks.history) {
        if (e->type() == QEvent::Close) {
            deleteHistoryDock();
            return true;
        }
        return false;
    }

    if (o == m_docks.variables) {
        if (e->type() == QEvent::Close) {
            deleteVariablesDock();
            return true;
        }
        return false;
    }

    if (o == m_docks.userFunctions) {
        if (e->type() == QEvent::Close) {
            deleteUserFunctionsDock();
            return true;
        }
        return false;
    }

    return QMainWindow::eventFilter(o, e);
}

void MainWindow::deleteStatusBar()
{
    m_status.angleUnit->deleteLater();
    m_status.angleUnit = 0;

    m_status.resultFormat->deleteLater();
    m_status.resultFormat = 0;

    setStatusBar(0);
}

void MainWindow::deleteBitField()
{
    m_widgets.bitField->hide();
    m_layouts.root->removeWidget(m_widgets.bitField);
    disconnect(m_widgets.bitField);
    m_widgets.bitField->deleteLater();
    m_widgets.bitField = 0;
    m_settings->bitfieldVisible = false;
}

void MainWindow::deleteBookDock()
{
    Q_ASSERT(m_docks.book);

    removeDockWidget(m_docks.book);
    disconnect(m_docks.book);
    m_docks.book->deleteLater();
    m_docks.book = 0;
    m_actions.viewFormulaBook->blockSignals(true);
    m_actions.viewFormulaBook->setChecked(false);
    m_actions.viewFormulaBook->blockSignals(false);
    m_settings->formulaBookDockVisible = false;
}

void MainWindow::deleteConstantsDock()
{
    Q_ASSERT(m_docks.constants);

    removeDockWidget(m_docks.constants);
    disconnect(m_docks.constants);
    m_docks.constants->deleteLater();
    m_docks.constants = 0;
    m_actions.viewConstants->blockSignals(true);
    m_actions.viewConstants->setChecked(false);
    m_actions.viewConstants->blockSignals(false);
    m_settings->constantsDockVisible = false;
}

void MainWindow::deleteFunctionsDock()
{
    Q_ASSERT(m_docks.functions);

    removeDockWidget(m_docks.functions);
    disconnect(m_docks.functions);
    m_docks.functions->deleteLater();
    m_docks.functions = 0;
    m_actions.viewFunctions->blockSignals(true);
    m_actions.viewFunctions->setChecked(false);
    m_actions.viewFunctions->blockSignals(false);
    m_settings->functionsDockVisible = false;
}

void MainWindow::deleteHistoryDock()
{
    Q_ASSERT(m_docks.history);

    removeDockWidget(m_docks.history);
    disconnect(m_docks.history);
    m_docks.history->deleteLater();
    m_docks.history = 0;
    m_actions.viewHistory->blockSignals(true);
    m_actions.viewHistory->setChecked(false);
    m_actions.viewHistory->blockSignals(false);
    m_settings->historyDockVisible = false;
}

void MainWindow::deleteVariablesDock()
{
    Q_ASSERT(m_docks.variables);

    removeDockWidget(m_docks.variables);
    disconnect(m_docks.variables);
    m_docks.variables->deleteLater();
    m_docks.variables = 0;
    m_actions.viewVariables->blockSignals(true);
    m_actions.viewVariables->setChecked(false);
    m_actions.viewVariables->blockSignals(false);
    m_settings->variablesDockVisible = false;
}

void MainWindow::deleteUserFunctionsDock()
{
    Q_ASSERT(m_docks.userFunctions);

    removeDockWidget(m_docks.userFunctions);
    disconnect(m_docks.userFunctions);
    m_docks.userFunctions->deleteLater();
    m_docks.userFunctions = 0;
    m_actions.viewUserFunctions->blockSignals(true);
    m_actions.viewUserFunctions->setChecked(false);
    m_actions.viewUserFunctions->blockSignals(false);
    m_settings->userFunctionsDockVisible = false;
}

void MainWindow::setFunctionsDockVisible(bool b)
{
    if (b)
        createFunctionsDock();
    else
        deleteFunctionsDock();
}

void MainWindow::setFormulaBookDockVisible(bool b)
{
    if (b)
        createBookDock();
    else
        deleteBookDock();
}

void MainWindow::setConstantsDockVisible(bool b)
{
    if (b)
        createConstantsDock();
    else
        deleteConstantsDock();
}

void MainWindow::setHistoryDockVisible(bool b)
{
    if (b)
        createHistoryDock();
    else
        deleteHistoryDock();
}

void MainWindow::setVariablesDockVisible(bool b)
{
    if (b)
        createVariablesDock();
    else
        deleteVariablesDock();
}

void MainWindow::setUserFunctionsDockVisible(bool b)
{
    if (b)
        createUserFunctionsDock();
    else
        deleteUserFunctionsDock();
}

#ifndef Q_OS_MAC
void MainWindow::showMenuBarTip()
{
    QString msg = tr("The menu bar is now hidden. To make it visible again, press Ctrl+M.");

    m_widgets.tip->move(0, 0);
    m_widgets.tip->showText(msg, tr("Warning"));
}
#endif

void MainWindow::setResultFormatBinary()
{
    m_actionGroups.digits->setDisabled(true);
    setResultFormat('b');

    if (m_status.resultFormat)
        m_status.resultFormat->setText(tr("Binary"));
}

void MainWindow::setResultFormatEngineering()
{
    m_actionGroups.digits->setEnabled(true);
    setResultFormat('n');

    if (m_status.resultFormat)
        m_status.resultFormat->setText(tr("Engineering decimal"));
}

void MainWindow::setResultFormatFixed()
{
    m_actionGroups.digits->setEnabled(true);
    setResultFormat('f');

    if (m_status.resultFormat)
        m_status.resultFormat->setText(tr("Fixed decimal"));
}

void MainWindow::setResultFormatGeneral()
{
    m_actionGroups.digits->setEnabled(true);
    setResultFormat('g');

    if (m_status.resultFormat)
        m_status.resultFormat->setText(tr("General decimal"));
}

void MainWindow::setResultFormatHexadecimal()
{
    m_actionGroups.digits->setDisabled(true);
    setResultFormat('h');

    if (m_status.resultFormat)
        m_status.resultFormat->setText(tr("Hexadecimal"));
}

void MainWindow::setResultFormatOctal()
{
    m_actionGroups.digits->setDisabled(true);
    setResultFormat('o');

    if (m_status.resultFormat)
        m_status.resultFormat->setText(tr("Octal"));
}

void MainWindow::setResultFormatScientific()
{
    m_actionGroups.digits->setEnabled(true);
    setResultFormat('e');

    if (m_status.resultFormat)
        m_status.resultFormat->setText(tr("Scientific decimal"));
}

void MainWindow::activate()
{
    show();
    raise();
    activateWindow();
    m_widgets.editor->setFocus();
}

void MainWindow::insertConstantIntoEditor(const QString& c)
{
    if (c.isEmpty())
        return;

    QString s = c;
    s.replace('.', m_settings->radixCharacter());
    insertTextIntoEditor(s);
}

void MainWindow::insertTextIntoEditor(const QString& s)
{
    if (s.isEmpty())
        return;

    bool shouldAutoComplete = m_widgets.editor->isAutoCompletionEnabled();
    m_widgets.editor->setAutoCompletionEnabled(false);
    m_widgets.editor->insert(s);
    m_widgets.editor->setAutoCompletionEnabled(shouldAutoComplete);

    if (!isActiveWindow())
        activateWindow();
    QTimer::singleShot(0, m_widgets.editor, SLOT(setFocus()));
}

void MainWindow::insertFunctionIntoEditor(const QString& f)
{
    if (f.isEmpty())
        return;
    insertTextIntoEditor(f + "()");
    QTextCursor cursor = m_widgets.editor->textCursor();
    cursor.movePosition(QTextCursor::PreviousCharacter);
    m_widgets.editor->setTextCursor(cursor);
}

void MainWindow::minimizeToSystemTray()
{
    if (!m_widgets.trayIcon)
        return;
    hide();
    m_widgets.trayIcon->show();
    if (m_conditions.trayNotify)
        QTimer::singleShot(500, this, SLOT(showSystemTrayMessage()));
    m_conditions.trayNotify = false;
}

void MainWindow::openUpdatesURL()
{
    QDesktopServices::openUrl(QUrl(QString::fromLatin1("http://speedcrunch.org")));
}

void MainWindow::openFeedbackURL()
{
    QDesktopServices::openUrl(QUrl(QString::fromLatin1("https://code.google.com/p/speedcrunch/issues/entry")));
}

void MainWindow::openCommunityURL()
{
    QDesktopServices::openUrl(QUrl(QString::fromLatin1("https://groups.google.com/group/speedcrunch/")));
}

void MainWindow::openNewsURL()
{
    QDesktopServices::openUrl(QUrl(QString::fromLatin1("http://speedcrunch.blogspot.com/")));
}

void MainWindow::copy()
{
    m_copyWidget->copy();
}

void MainWindow::raiseWindow()
{
    activate();

#ifdef Q_OS_WIN
    SetWindowPos((HWND)winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    SetWindowPos((HWND)winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
#endif // Q_OS_WIN

#ifdef Q_WS_X11
    static Atom NET_ACTIVE_WINDOW = XInternAtom(QX11Info::display(), "_NET_ACTIVE_WINDOW", False);

    XClientMessageEvent xev;
    xev.type = ClientMessage;
    xev.window = winId();
    xev.message_type = NET_ACTIVE_WINDOW;
    xev.format = 32;
    xev.data.l[0] = 2;
    xev.data.l[1] = CurrentTime;
    xev.data.l[2] = 0;
    xev.data.l[3] = 0;
    xev.data.l[4] = 0;

    XSendEvent(QX11Info::display(), QX11Info::appRootWindow(), False,
        (SubstructureNotifyMask | SubstructureRedirectMask), (XEvent*)&xev);
#endif // Q_WS_X11
}

void MainWindow::restoreVariables()
{
    for (int k = 0; k < m_settings->variables.count(); ++k) {
        m_evaluator->setExpression(m_settings->variables.at(k));
        m_evaluator->eval();
        QStringList list = m_settings->variables.at(k).split("=");
        Evaluator::Variable::Type type = Evaluator::Variable::UserDefined;
        if (list.at(0) == QLatin1String("ans"))
            type = Evaluator::Variable::BuiltIn;
        m_evaluator->setVariable(list.at(0), HNumber(list.at(1).toLatin1().data()), type);
    }

    if (m_docks.variables)
        m_docks.variables->updateList();

    m_settings->variables.clear();
}

void MainWindow::restoreUserFunctions()
{
    for (int k = 0; k < m_settings->userFunctions.count(); ++k) {
        QStringList funcParts = m_settings->userFunctions.at(k);
        Evaluator::UserFunctionDescr descr(funcParts.first(), funcParts.mid(1, funcParts.size() - 2), funcParts.last());
        m_evaluator->setUserFunction(descr);
    }

    if (m_docks.userFunctions)
        m_docks.userFunctions->updateList();

    m_settings->userFunctions.clear();
}

void MainWindow::restoreHistory()
{
    if (m_settings->historyResults.count() != m_settings->history.count()) {
        clearHistory();
        return;
    }

    m_widgets.editor->setHistory(m_settings->history);
    m_widgets.editor->setHistoryResults(m_settings->historyResults);
    m_widgets.display->appendHistory(m_settings->history, m_settings->historyResults);

    if (m_docks.history) {
        HistoryWidget* history = qobject_cast<HistoryWidget*>(m_docks.history->widget());
        history->setHistory(m_widgets.editor->history());
    }

    // Free some useless memory.
    m_settings->history.clear();
    m_settings->historyResults.clear();
}

void MainWindow::evaluateEditorExpression()
{
    QString expr = m_evaluator->autoFix(m_widgets.editor->text());

    if (expr.isEmpty())
        return;

    m_evaluator->setExpression(expr);
    HNumber result = m_evaluator->evalUpdateAns();

    if (!m_evaluator->error().isEmpty()) {
        showStateLabel(m_evaluator->error());
        return;
    }

    if (m_evaluator->isUserFunctionAssign()) {
        result = HMath::nan();
    } else if (result.isNan())
        return;

    m_widgets.display->append(expr, result);
    m_widgets.display->scrollToBottom();

    if (result.isNan()) {
        m_widgets.editor->appendHistory(expr, "");
    } else {
        const char format = result.format() != 0 ? result.format() : 'e';
        char* num = HMath::format(result, format, DECPRECISION);
        m_widgets.editor->appendHistory(expr, num);
        free(num);
        m_widgets.editor->setAnsAvailable(true);
    }

    if (m_settings->bitfieldVisible)
        m_widgets.bitField->updateBits(result);

    if (m_settings->variablesDockVisible)
        m_docks.variables->updateList();

    if (m_settings->userFunctionsDockVisible)
        m_docks.userFunctions->updateList();

    if (m_settings->historyDockVisible) {
        HistoryWidget* history = qobject_cast<HistoryWidget*>(m_docks.history->widget());
        history->append(expr);
    }

    if (m_settings->autoResultToClipboard)
        copyResultToClipboard();

    if (m_settings->leaveLastExpression)
        m_widgets.editor->selectAll();
    else
        m_widgets.editor->clear();

    m_widgets.editor->stopAutoCalc();
    m_widgets.editor->stopAutoComplete();
    if (!result.isNan())
        m_conditions.autoAns = true;
}

void MainWindow::showSystemTrayMessage()
{
    QString msg = tr("SpeedCrunch is minimized.\nLeft click the icon to restore it or right click for options.");
    if (menuBar()->layoutDirection() == Qt::RightToLeft)
        msg += QChar(0x200E);
    if (m_widgets.trayIcon)
        m_widgets.trayIcon->showMessage(QString(), msg, QSystemTrayIcon::NoIcon, 4000);
}

void MainWindow::clearTextEditSelection(QPlainTextEdit* edit)
{
    QTextCursor cursor = edit->textCursor();
    if (cursor.hasSelection()) {
        cursor.clearSelection();
        edit->setTextCursor(cursor);
    }
}

void MainWindow::handleManualClosed()
{
    disconnect(m_widgets.manual);
    m_widgets.manual->deleteLater();
    m_widgets.manual = 0;
}

void MainWindow::handleDisplaySelectionChange()
{
    clearTextEditSelection(m_widgets.editor);
}

void MainWindow::handleEditorSelectionChange()
{
    clearTextEditSelection(m_widgets.display);
}

void MainWindow::handleCopyAvailable(bool copyAvailable)
{
    if (!copyAvailable)
        return;
    QPlainTextEdit* const textEdit = static_cast<QPlainTextEdit*>(sender());
    if (textEdit)
        m_copyWidget = textEdit;
}

void MainWindow::handleBitsChanged(const QString& str)
{
    clearEditor();
    HNumber num(str.toLatin1().data());
    insertTextIntoEditor(QString(HMath::format(num, 'h')));
    showStateLabel(QString("Current value: %1").arg(NumberFormatter::format(num)));
}

void MainWindow::handleEditorTextChange()
{
    clearTextEditSelection(m_widgets.display);
    if (m_conditions.autoAns && m_settings->autoAns) {
        QString expr = m_evaluator->autoFix(m_widgets.editor->text());
        if (expr.isEmpty())
            return;

        Tokens tokens = m_evaluator->scan(expr, Evaluator::NoAutoFix);
        if (tokens.count() == 1) {
            bool operatorCondition =
                tokens.at(0).asOperator() == Token::Plus
                || tokens.at(0).asOperator() == Token::Minus
                || tokens.at(0).asOperator() == Token::Asterisk
                || tokens.at(0).asOperator() == Token::Slash
                || tokens.at(0).asOperator() == Token::Caret;
            if (operatorCondition) {
                m_conditions.autoAns = false;
                expr.prepend("ans");
                m_widgets.editor->setText(expr);
                m_widgets.editor->setCursorPosition(expr.length());
            }
        }
    }
}

void MainWindow::handleSystemTrayIconActivation(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Context)
        m_menus.trayIcon->show();
    else {
        showNormal();
        activateWindow();
        m_widgets.editor->setFocus();
        QTimer::singleShot(0, m_widgets.trayIcon, SLOT(hide()));

        // Work around docks do not reappear if floating.
        if (m_docks.history && m_docks.history->isFloating()) {
            m_docks.history->hide();
            m_docks.history->show();
        }
        if (m_docks.functions && m_docks.functions->isFloating()) {
            m_docks.functions->hide();
            m_docks.functions->show();
        }
        if (m_docks.variables && m_docks.variables->isFloating()) {
            m_docks.variables->hide();
            m_docks.variables->show();
        }
        if (m_docks.userFunctions && m_docks.userFunctions->isFloating()) {
            m_docks.userFunctions->hide();
            m_docks.userFunctions->show();
        }
        if (m_docks.constants && m_docks.constants->isFloating()) {
            m_docks.constants->hide();
            m_docks.constants->show();
        }
    }
}

void MainWindow::insertVariableIntoEditor(const QString& v)
{
    insertTextIntoEditor(v);
}

void MainWindow::insertUserFunctionIntoEditor(const QString& v)
{
    insertTextIntoEditor(v);
}

void MainWindow::setRadixCharacterAutomatic()
{
    setRadixCharacter(0);
}

void MainWindow::setRadixCharacterDot()
{
    setRadixCharacter('.');
}

void MainWindow::setRadixCharacterComma()
{
    setRadixCharacter(',');
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    saveSettings();
    e->accept();
}

void MainWindow::setResultPrecision(int p)
{
    if (m_settings->resultPrecision == p)
        return;

    m_settings->resultPrecision = p;
    emit resultPrecisionChanged();
}

void MainWindow::setResultFormat(char c)
{
    if (m_settings->resultFormat == c)
        return;

    m_settings->resultFormat = c;
    emit resultFormatChanged();
}

void MainWindow::setRadixCharacter(char c)
{
    m_settings->setRadixCharacter(c);
    emit radixCharacterChanged();
}

void MainWindow::increaseDisplayFontPointSize()
{
    m_widgets.display->increaseFontPointSize();
    m_widgets.editor->increaseFontPointSize();
}

void MainWindow::decreaseDisplayFontPointSize()
{
    m_widgets.display->decreaseFontPointSize();
    m_widgets.editor->decreaseFontPointSize();
}

void MainWindow::showLanguageChooserDialog()
{
    QMap<QString, QString> map;

    map.insert(QLatin1String("American English"), QLatin1String("en_US"));
    map.insert(QLatin1String("Bahasa Indonesia"), QLatin1String("id_ID"));
    map.insert(QLatin1String("British English"), QLatin1String("en_GB"));
    map.insert(QString::fromUtf8("Català"), QLatin1String("ca_ES"));
    map.insert(QLatin1String("Deutsch"), QLatin1String("de_DE"));
    map.insert(QLatin1String("Eesti"), QLatin1String("et_EE"));
    map.insert(QString::fromUtf8("Español"), QLatin1String("es_ES"));
    map.insert(QString::fromUtf8("Español Argentino"), QLatin1String("es_AR"));
    map.insert(QLatin1String("Euskara"), QLatin1String("eu_ES"));
    map.insert(QString::fromUtf8("Français"), QLatin1String("fr_FR"));
    map.insert(QLatin1String("Italiano"), QLatin1String("it_IT"));
    map.insert(QString::fromUtf8("Latviešu"), QLatin1String("lv_LV"));
    map.insert(QLatin1String("Magyar"), QLatin1String("hu_HU"));
    map.insert(QLatin1String("Nederlands"), QLatin1String("nl_NL"));
    map.insert(QString::fromUtf8("Norsk (Bokmål)") + QChar(0x200E), QLatin1String("nb_NO"));
    map.insert(QLatin1String("Polski"), QLatin1String("pl_PL"));
    map.insert(QString::fromUtf8("Português do Brasil"), QLatin1String("pt_BR"));
    map.insert(QString::fromUtf8("Português Europeu"), QLatin1String("pt_PT"));
    map.insert(QString::fromUtf8("Română"), QLatin1String("ro_RO"));
    map.insert(QLatin1String("Suomi"), QLatin1String("fi_FI"));
    map.insert(QLatin1String("Svenska"), QLatin1String("sv_SE"));
    map.insert(QString::fromUtf8("Česky"), QLatin1String("cs_CZ"));
    map.insert(QString::fromUtf8("한국어"), QLatin1String("ko_KR"));
    map.insert(QString::fromUtf8("Tiếng Việt"), QLatin1String("vi_VN"));
    map.insert(QString::fromUtf8("Türkçe"), QLatin1String("tr_TR"));
    map.insert(QString::fromUtf8("العربية"), QLatin1String("ar_JO"));
    map.insert(QString::fromUtf8("עברית"), QLatin1String("he_IL"));
    map.insert(QString::fromUtf8("Русский"), QLatin1String("ru_RU"));
    map.insert(QString::fromUtf8("日本語"), QLatin1String("ja_JP"));
    map.insert(QString::fromUtf8("简化字"), QLatin1String("zh_CN"));

    int current = map.values().indexOf(m_settings->language) + 1;

    QString defaultKey = tr("System Default");
    QStringList keys(QStringList() << defaultKey << map.keys());

    bool ok;
    QString langName = QInputDialog::getItem(this, tr("Language"), tr("Select the language:"),
        keys, current, false, &ok);
    if (ok && !langName.isEmpty()) {
        QString value = (langName == defaultKey) ? QLatin1String("C") : map.value(langName);
        if (m_settings->language != value) {
            m_settings->language = value;
            emit languageChanged();
        }
    }
}

void MainWindow::showResultFormatContextMenu(const QPoint& point)
{
    m_menus.resultFormat->popup(m_status.resultFormat->mapToGlobal(point));
}

void MainWindow::cycleAngleUnits()
{
    if (m_actions.settingsAngleUnitDegree->isChecked())
        m_actions.settingsAngleUnitRadian->trigger();
    else if (m_actions.settingsAngleUnitRadian->isChecked())
        m_actions.settingsAngleUnitDegree->trigger();
}

void MainWindow::cycleResultFormats()
{
  if (m_actions.settingsResultFormatGeneral->isChecked())
      m_actions.settingsResultFormatFixed->trigger();
  else if (m_actions.settingsResultFormatFixed->isChecked())
      m_actions.settingsResultFormatEngineering->trigger();
  else if (m_actions.settingsResultFormatEngineering->isChecked())
      m_actions.settingsResultFormatScientific->trigger();
  else if (m_actions.settingsResultFormatScientific->isChecked())
      m_actions.settingsResultFormatBinary->trigger();
  else if (m_actions.settingsResultFormatBinary->isChecked())
      m_actions.settingsResultFormatOctal->trigger();
  else if (m_actions.settingsResultFormatOctal->isChecked())
      m_actions.settingsResultFormatHexadecimal->trigger();
  else if (m_actions.settingsResultFormatHexadecimal->isChecked())
      m_actions.settingsResultFormatGeneral->trigger();
}
