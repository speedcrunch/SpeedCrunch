// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2005-2006 Johan Thelin <e8johan@gmail.com>
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
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

#include "3rdparty/binreloc/binreloc.h"
#include "core/constants.hxx"
#include "core/evaluator.hxx"
#include "core/functions.hxx"
#include "core/settings.hxx"
#include "gui/aboutbox.hxx"
#include "gui/autohidelabel.hxx"
#include "gui/bookdock.hxx"
#include "gui/constantsdock.hxx"
#include "gui/deletevardlg.hxx"
#include "gui/editor.hxx"
#include "gui/functionsdock.hxx"
#include "gui/historydock.hxx"
#include "gui/historywidget.hxx"
#include "gui/insertfunctiondlg.hxx"
#include "gui/insertvardlg.hxx"
#include "gui/mainwindow.hxx"
#include "gui/resultdisplay.hxx"
#include "gui/tipwidget.hxx"
#include "gui/variablesdock.hxx"
#include "math/floatconfig.h"

#include <QtCore/QTextStream>
#include <QtCore/QTimer>
#include <QtCore/QTranslator>
#include <QtCore/QUrl>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QCloseEvent>
#include <QtGui/QDesktopServices>
#include <QtGui/QDesktopWidget>
#include <QtGui/QFileDialog>
#include <QtGui/QInputDialog>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QMessageBox>
#include <QtGui/QScrollBar>
#include <QtGui/QStatusBar>
#include <QtGui/QVBoxLayout>
#ifdef Q_WS_X11
#include <QtGui/QX11Info>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#endif // Q_WS_X11

#ifdef Q_OS_WIN32
#include "Windows.h"
#endif // Q_OS_WIN32

struct Actions
{
    // session
    QAction * sessionLoad;
    QAction * sessionSave;
    QAction * sessionImport;
    QAction * sessionExport;
    QAction * sessionQuit;

    // edit
    QAction * editCopy;
    QAction * editCopyLastResult;
    QAction * editPaste;
    QAction * editSelectExpression;
    QAction * editInsertFunction;
    QAction * editInsertVariable;
    QAction * editDeleteVariable;
    QAction * editDeleteAllVariables;
    QAction * editClearExpression;
    QAction * editClearHistory;

    // view
    QAction * viewKeypad;
    QAction * viewMathBook;
    QAction * viewConstants;
    QAction * viewFunctions;
    QAction * viewVariables;
    QAction * viewHistory;
    QAction * viewMenuBar;
    QAction * viewStatusBar;
    QAction * viewFullScreenMode;

    // settings / result format / decimal
    QAction * settingsResultFormatGeneral;
    QAction * settingsResultFormatFixed;
    QAction * settingsResultFormatEngineering;
    QAction * settingsResultFormatScientific;
    QAction * settingsResultFormatAutoPrecision;
    QAction * settingsResultFormat2Digits;
    QAction * settingsResultFormat3Digits;
    QAction * settingsResultFormat8Digits;
    QAction * settingsResultFormat15Digits;
    QAction * settingsResultFormat50Digits;
    // settings / result format
    QAction * settingsResultFormatBinary;
    QAction * settingsResultFormatOctal;
    QAction * settingsResultFormatHexadecimal;
    // settings / angle unit
    QAction * settingsAngleUnitRadian;
    QAction * settingsAngleUnitDegree;
    // settings / behavior
    QAction * settingsBehaviorSaveHistoryOnExit;
    QAction * settingsBehaviorSaveVariablesOnExit;
    QAction * settingsBehaviorPartialResults;
    QAction * settingsBehaviorAutoCompletion;
    QAction * settingsBehaviorSyntaxHilite;
    QAction * settingsBehaviorAlwaysOnTop;
    QAction * settingsBehaviorMinimizeToTray;
    // settings / radix character
    QAction * settingsRadixCharDefault;
    QAction * settingsRadixCharDot;
    QAction * settingsRadixCharComma;
    // settings / language
    QAction * settingsLanguage;

    // help
    QAction * helpTipOfTheDay;
    QAction * helpWebsite;
    QAction * helpAbout;
    QAction * helpAboutQt;

    // shortcuts
    QAction * scrollDown;
    QAction * scrollUp;
};

struct ActionGroups
{
    QActionGroup * angle;
    QActionGroup * digits;
    QActionGroup * resultFormat;
    QActionGroup * radixChar;
};

struct Menus
{
    QMenu * angleUnit;
    QMenu * behavior;
    QMenu * decimal;
    QMenu * edit;
    QMenu * resultFormat;
    QMenu * help;
    QMenu * precision;
    QMenu * radixChar;
    QMenu * session;
    QMenu * settings;
    QMenu * trayIcon;
    QMenu * view;
};

struct Layouts
{
    QVBoxLayout * root;
    QHBoxLayout * keypad;
};

struct Widgets
{
    AutoHideLabel * autoCalcTip;
    ResultDisplay * display;
    Editor * editor;
    Keypad * keypad;
    QWidget * root;
    TipWidget * tip;
    QSystemTrayIcon * trayIcon;
};

struct Docks
{
  BookDock * book;
  ConstantsDock * constants;
  FunctionsDock * functions;
  HistoryDock * history;
  VariablesDock * variables;
};

struct Conditions
{
  bool notifyMenuBarHidden;
  bool trayNotify;
  bool autoAns;
};

struct StatusBar
{
  QLabel * angleUnit;
  QLabel * resultFormat;
};

struct MainWindow::Private
{
    Actions actions;
    ActionGroups actionGroups;
    Conditions conditions;
    Constants * constants;
    Docks docks;
    Evaluator * evaluator;
    Functions * functions;
    Layouts layouts;
    Menus menus;
    MainWindow * p;
    Settings * settings;
    StatusBar status;
    QTranslator * translator;
    Widgets widgets;

    Private();
    ~Private();
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

    static QTranslator * createTranslator( const QString & langCode );
};

MainWindow::Private::Private()
{
    translator = 0;
    settings = Settings::instance();

    widgets.keypad   = 0;
    widgets.trayIcon = 0;

    menus.trayIcon = 0;

    conditions.autoAns = false;
    conditions.trayNotify = true;
    conditions.notifyMenuBarHidden = true;

    docks.book = 0;
    docks.history = 0;
    docks.constants = 0;
    docks.functions = 0;
    docks.variables = 0;

    status.angleUnit = 0;
    status.resultFormat = 0;
};

MainWindow::Private::~Private()
{
    if ( widgets.trayIcon ) widgets.trayIcon->hide();
    if ( docks.book       ) deleteBookDock();
    if ( docks.constants  ) deleteConstantsDock();
    if ( docks.variables  ) deleteVariablesDock();
    if ( docks.functions  ) deleteFunctionsDock();
    if ( docks.history    ) deleteHistoryDock();
}

QTranslator * MainWindow::Private::createTranslator( const QString & langCode )
{
    QTranslator * translator = new QTranslator;
    QString localeFile = (langCode == "C") ? QLocale().name() : langCode;

    // fallback to English
    if ( localeFile == "C" )
        localeFile = "en";

#if defined(Q_OS_WIN32) || defined(Q_OS_MAC)
    QString appPath = QApplication::applicationFilePath();
    int ii = appPath.lastIndexOf( '/' );
    if ( ii > 0 )
        appPath.remove( ii, appPath.length() );
    QString fname = appPath + '/' + QString( "locale/" ) + localeFile;
    translator->load( fname );
    return translator;
#else

    BrInitError error;
    if ( br_init( & error ) == 0 && error != BR_INIT_ERROR_DISABLED ) {
        qDebug( "Warning: BinReloc failed to initialize (error code %d)", error );
        qDebug( "Will fallback to hardcoded default path." );
    }

    char * dataDir = br_find_data_dir( 0 );
    QString localeDir = QString( dataDir ) + "/speedcrunch/locale";
    free( dataDir );
    return translator->load( localeFile, localeDir ) ? translator : 0;
#endif
}

void MainWindow::Private::createUi()
{
    createActions();
    createActionGroups();
    createActionShortcuts();
    createMenus();
    createFixedWidgets();
    createFixedConnections();

    p->setWindowTitle( "SpeedCrunch" );
    p->setWindowIcon( QPixmap(":/speedcrunch.png") );
}

void MainWindow::Private::createActions()
{
    actions.sessionExport = new QAction( p );
    actions.sessionImport = new QAction( p );
    actions.sessionLoad   = new QAction( p );
    actions.sessionQuit   = new QAction( p );
    actions.sessionSave   = new QAction( p );

    actions.editClearExpression    = new QAction( p );
    actions.editClearHistory       = new QAction( p );
    actions.editCopyLastResult     = new QAction( p );
    actions.editCopy               = new QAction( p );
    actions.editDeleteAllVariables = new QAction( p );
    actions.editDeleteVariable     = new QAction( p );
    actions.editInsertFunction     = new QAction( p );
    actions.editInsertVariable     = new QAction( p );
    actions.editPaste              = new QAction( p );
    actions.editSelectExpression   = new QAction( p );

    actions.viewConstants      = new QAction( p );
    actions.viewFullScreenMode = new QAction( p );
    actions.viewFunctions      = new QAction( p );
    actions.viewHistory        = new QAction( p );
    actions.viewKeypad         = new QAction( p );
    actions.viewMathBook       = new QAction( p );
    actions.viewMenuBar        = new QAction( p );
    actions.viewStatusBar      = new QAction( p );
    actions.viewVariables      = new QAction( p );

    actions.settingsAngleUnitDegree = new QAction( p );
    actions.settingsAngleUnitRadian = new QAction( p );

    actions.settingsBehaviorAlwaysOnTop         = new QAction( p );
    actions.settingsBehaviorAutoCompletion      = new QAction( p );
    actions.settingsBehaviorMinimizeToTray      = new QAction( p );
    actions.settingsBehaviorPartialResults      = new QAction( p );
    actions.settingsBehaviorSaveHistoryOnExit   = new QAction( p );
    actions.settingsBehaviorSaveVariablesOnExit = new QAction( p );
    actions.settingsBehaviorSyntaxHilite        = new QAction( p );

    actions.settingsRadixCharComma   = new QAction( p );
    actions.settingsRadixCharDefault = new QAction( p );
    actions.settingsRadixCharDot     = new QAction( p );

    actions.settingsResultFormat15Digits      = new QAction( p );
    actions.settingsResultFormat2Digits       = new QAction( p );
    actions.settingsResultFormat3Digits       = new QAction( p );
    actions.settingsResultFormat50Digits      = new QAction( p );
    actions.settingsResultFormat8Digits       = new QAction( p );
    actions.settingsResultFormatAutoPrecision = new QAction( p );
    actions.settingsResultFormatBinary        = new QAction( p );
    actions.settingsResultFormatEngineering   = new QAction( p );
    actions.settingsResultFormatFixed         = new QAction( p );
    actions.settingsResultFormatGeneral       = new QAction( p );
    actions.settingsResultFormatHexadecimal   = new QAction( p );
    actions.settingsResultFormatOctal         = new QAction( p );
    actions.settingsResultFormatScientific    = new QAction( p );

    actions.helpAbout       = new QAction( p );
    actions.helpAboutQt     = new QAction( p );
    actions.helpTipOfTheDay = new QAction( p );
    actions.helpWebsite     = new QAction( p );

    actions.scrollDown = new QAction( p );
    actions.scrollUp   = new QAction( p );

    actions.settingsLanguage = new QAction( p );

    actions.settingsAngleUnitDegree->setCheckable( true );
    actions.settingsAngleUnitRadian->setCheckable( true );

    actions.settingsBehaviorAlwaysOnTop        ->setCheckable( true );
    actions.settingsBehaviorAutoCompletion     ->setCheckable( true );
    actions.settingsBehaviorMinimizeToTray     ->setCheckable( true );
    actions.settingsBehaviorPartialResults     ->setCheckable( true );
    actions.settingsBehaviorSaveHistoryOnExit  ->setCheckable( true );
    actions.settingsBehaviorSaveVariablesOnExit->setCheckable( true );
    actions.settingsBehaviorSyntaxHilite       ->setCheckable( true );

    actions.settingsRadixCharComma  ->setCheckable( true );
    actions.settingsRadixCharDefault->setCheckable( true );
    actions.settingsRadixCharDot    ->setCheckable( true );

    actions.settingsResultFormat15Digits     ->setCheckable( true );
    actions.settingsResultFormat2Digits      ->setCheckable( true );
    actions.settingsResultFormat3Digits      ->setCheckable( true );
    actions.settingsResultFormat50Digits     ->setCheckable( true );
    actions.settingsResultFormat8Digits      ->setCheckable( true );
    actions.settingsResultFormatAutoPrecision->setCheckable( true );
    actions.settingsResultFormatBinary       ->setCheckable( true );
    actions.settingsResultFormatEngineering  ->setCheckable( true );
    actions.settingsResultFormatFixed        ->setCheckable( true );
    actions.settingsResultFormatGeneral      ->setCheckable( true );
    actions.settingsResultFormatHexadecimal  ->setCheckable( true );
    actions.settingsResultFormatOctal        ->setCheckable( true );
    actions.settingsResultFormatScientific   ->setCheckable( true );

    actions.viewConstants     ->setCheckable( true );
    actions.viewFullScreenMode->setCheckable( true );
    actions.viewFunctions     ->setCheckable( true );
    actions.viewHistory       ->setCheckable( true );
    actions.viewKeypad        ->setCheckable( true );
    actions.viewMathBook      ->setCheckable( true );
    actions.viewMenuBar       ->setCheckable( true );
    actions.viewStatusBar     ->setCheckable( true );
    actions.viewVariables     ->setCheckable( true );
}

void MainWindow::retranslateText()
{
    QTranslator * tr = 0;
    tr = d->createTranslator( d->settings->language );
    if ( tr ) {
        if ( d->translator ) {
            qApp->removeTranslator( d->translator );
            d->translator->deleteLater();
        }

        qApp->installTranslator( tr );
        d->translator = tr;
    } else {
        qApp->removeTranslator( d->translator );
        d->translator = 0;
    }

    d->setMenusText();
    d->setActionsText();
    d->setStatusBarText();

    d->functions->retranslateText(); // can't be signal/slot to ensure order
    d->constants->retranslateText(); // same as above

    if ( d->docks.book )
        d->docks.book->setWindowTitle( MainWindow::tr("Math Book") );

    // layout direction
    setWidgetsDirection();
}

void MainWindow::Private::setStatusBarText()
{
    if ( status.angleUnit ) {
        QString angleUnit = (settings->angleUnit == 'r') ?
                            MainWindow::tr("Radian") : MainWindow::tr("Degree");

        QString format;
        switch ( settings->resultFormat ) {
            case 'b': format = MainWindow::tr( "Binary" ); break;
            case 'o': format = MainWindow::tr( "Octal" ); break;
            case 'h': format = MainWindow::tr( "Hexadecimal" ); break;
            case 'f': format = MainWindow::tr( "Fixed decimal" ); break;
            case 'n': format = MainWindow::tr( "Engineering decimal" ); break;
            case 'e': format = MainWindow::tr( "Scientific decimal" ); break;
            case 'g': format = MainWindow::tr( "General decimal" ); break;
            default : break;
        }

        status.angleUnit->setText( angleUnit );
        status.resultFormat->setText( format );

        status.angleUnit->setToolTip( MainWindow::tr("Angle unit") );
        status.resultFormat->setToolTip( MainWindow::tr("Result format") );
    }
}

void MainWindow::Private::setActionsText()
{
    actions.sessionExport->setText( MainWindow::tr("&Export...") );
    actions.sessionImport->setText( MainWindow::tr("&Import...") );
    actions.sessionLoad  ->setText( MainWindow::tr("&Load...") );
    actions.sessionQuit  ->setText( MainWindow::tr("&Quit") );
    actions.sessionSave  ->setText( MainWindow::tr("&Save...") );

    actions.editClearExpression   ->setText( MainWindow::tr("Clear E&xpression") );
    actions.editClearHistory      ->setText( MainWindow::tr("Clear &History") );
    actions.editCopyLastResult    ->setText( MainWindow::tr("Copy Last &Result") );
    actions.editCopy              ->setText( MainWindow::tr("&Copy") );
    actions.editDeleteAllVariables->setText( MainWindow::tr("Delete All V&ariables") );
    actions.editDeleteVariable    ->setText( MainWindow::tr("D&elete Variable...") );
    actions.editInsertFunction    ->setText( MainWindow::tr("Insert &Function...") );
    actions.editInsertVariable    ->setText( MainWindow::tr("Insert &Variable...") );
    actions.editPaste             ->setText( MainWindow::tr("&Paste") );
    actions.editSelectExpression  ->setText( MainWindow::tr("&Select Expression") );

    actions.viewConstants     ->setText( MainWindow::tr("&Constants") );
    actions.viewFullScreenMode->setText( MainWindow::tr("F&ull Screen Mode") );
    actions.viewFunctions     ->setText( MainWindow::tr("&Functions") );
    actions.viewHistory       ->setText( MainWindow::tr("&History") );
    actions.viewKeypad        ->setText( MainWindow::tr("&Keypad") );
    actions.viewMathBook      ->setText( MainWindow::tr("Math &Book") );
    actions.viewMenuBar       ->setText( MainWindow::tr("&Menu Bar") );
    actions.viewStatusBar     ->setText( MainWindow::tr("&Status Bar") );
    actions.viewVariables     ->setText( MainWindow::tr("&Variables") );

    actions.settingsAngleUnitDegree            ->setText( MainWindow::tr("&Degree") );
    actions.settingsAngleUnitRadian            ->setText( MainWindow::tr("&Radian") );
    actions.settingsBehaviorAlwaysOnTop        ->setText( MainWindow::tr("Always On &Top") );
    actions.settingsBehaviorAutoCompletion     ->setText( MainWindow::tr("Automatic &Completion") );
    actions.settingsBehaviorMinimizeToTray     ->setText( MainWindow::tr("&Minimize To System Tray") );
    actions.settingsBehaviorPartialResults     ->setText( MainWindow::tr("&Partial Results") );
    actions.settingsBehaviorSaveHistoryOnExit  ->setText( MainWindow::tr("Save &History on Exit") );
    actions.settingsBehaviorSaveVariablesOnExit->setText( MainWindow::tr("Save &Variables on Exit") );
    actions.settingsBehaviorSyntaxHilite       ->setText( MainWindow::tr("Syntax &Highlighting") );
    actions.settingsRadixCharComma             ->setText( MainWindow::tr("&Comma") );
    actions.settingsRadixCharDefault           ->setText( MainWindow::tr("&System Default") );
    actions.settingsRadixCharDot               ->setText( MainWindow::tr("&Dot") );
    actions.settingsResultFormat15Digits       ->setText( MainWindow::tr("&15 Decimal Digits") );
    actions.settingsResultFormat2Digits        ->setText( MainWindow::tr("&2 Decimal Digits") );
    actions.settingsResultFormat3Digits        ->setText( MainWindow::tr("&3 Decimal Digits") );
    actions.settingsResultFormat50Digits       ->setText( MainWindow::tr("&50 Decimal Digits") );
    actions.settingsResultFormat8Digits        ->setText( MainWindow::tr("&8 Decimal Digits") );
    actions.settingsResultFormatAutoPrecision  ->setText( MainWindow::tr("&Automatic") );
    actions.settingsResultFormatBinary         ->setText( MainWindow::tr("&Binary") );
    actions.settingsResultFormatEngineering    ->setText( MainWindow::tr("&Engineering") );
    actions.settingsResultFormatFixed          ->setText( MainWindow::tr("&Fixed Decimal") );
    actions.settingsResultFormatGeneral        ->setText( MainWindow::tr("&General") );
    actions.settingsResultFormatHexadecimal    ->setText( MainWindow::tr("&Hexadecimal") );
    actions.settingsResultFormatOctal          ->setText( MainWindow::tr("&Octal") );
    actions.settingsResultFormatScientific     ->setText( MainWindow::tr("&Scientific") );
    actions.settingsLanguage                   ->setText( MainWindow::tr("&Language...") );

    actions.helpAboutQt    ->setText( MainWindow::tr("About &Qt") );
    actions.helpAbout      ->setText( MainWindow::tr("&About") );
    actions.helpTipOfTheDay->setText( MainWindow::tr("&Tip of the Day") );
    actions.helpWebsite    ->setText( MainWindow::tr("SpeedCrunch &Web Site...") );

    actions.scrollDown->setText( MainWindow::tr("Scroll Display Down") );
    actions.scrollUp  ->setText( MainWindow::tr("Scroll Display Up") );
}

void MainWindow::Private::createActionGroups()
{
    actionGroups.resultFormat = new QActionGroup( p );
    actionGroups.resultFormat->addAction( actions.settingsResultFormatBinary );
    actionGroups.resultFormat->addAction( actions.settingsResultFormatGeneral );
    actionGroups.resultFormat->addAction( actions.settingsResultFormatFixed );
    actionGroups.resultFormat->addAction( actions.settingsResultFormatEngineering );
    actionGroups.resultFormat->addAction( actions.settingsResultFormatScientific );
    actionGroups.resultFormat->addAction( actions.settingsResultFormatOctal );
    actionGroups.resultFormat->addAction( actions.settingsResultFormatHexadecimal );

    actionGroups.radixChar = new QActionGroup( p );
    actionGroups.radixChar->addAction( actions.settingsRadixCharDefault );
    actionGroups.radixChar->addAction( actions.settingsRadixCharDot );
    actionGroups.radixChar->addAction( actions.settingsRadixCharComma );

    actionGroups.digits = new QActionGroup( p );
    actionGroups.digits->addAction( actions.settingsResultFormatAutoPrecision );
    actionGroups.digits->addAction( actions.settingsResultFormat2Digits );
    actionGroups.digits->addAction( actions.settingsResultFormat3Digits );
    actionGroups.digits->addAction( actions.settingsResultFormat8Digits );
    actionGroups.digits->addAction( actions.settingsResultFormat15Digits );
    actionGroups.digits->addAction( actions.settingsResultFormat50Digits );

    actionGroups.angle = new QActionGroup( p );
    actionGroups.angle->addAction( actions.settingsAngleUnitRadian );
    actionGroups.angle->addAction( actions.settingsAngleUnitDegree );
}

void MainWindow::Private::createActionShortcuts()
{
    actions.sessionLoad->setShortcut( Qt::CTRL + Qt::Key_L );
    actions.sessionQuit->setShortcut( Qt::CTRL + Qt::Key_Q );
    actions.sessionSave->setShortcut( Qt::CTRL + Qt::Key_S );

    actions.editClearExpression ->setShortcut( Qt::Key_Escape );
    actions.editClearHistory    ->setShortcut( Qt::CTRL + Qt::ALT + Qt::Key_N );
    actions.editCopyLastResult  ->setShortcut( Qt::CTRL + Qt::Key_R );
    actions.editCopy            ->setShortcut( Qt::CTRL + Qt::Key_C );
    actions.editDeleteVariable  ->setShortcut( Qt::CTRL + Qt::Key_D );
    actions.editInsertFunction  ->setShortcut( Qt::CTRL + Qt::Key_F );
    actions.editInsertVariable  ->setShortcut( Qt::CTRL + Qt::Key_I );
    actions.editPaste           ->setShortcut( Qt::CTRL + Qt::Key_V );
    actions.editSelectExpression->setShortcut( Qt::CTRL + Qt::Key_A );

    actions.viewConstants     ->setShortcut( Qt::CTRL + Qt::Key_2 );
    actions.viewFullScreenMode->setShortcut( Qt::Key_F11 );
    actions.viewFunctions     ->setShortcut( Qt::CTRL + Qt::Key_3 );
    actions.viewHistory       ->setShortcut( Qt::CTRL + Qt::Key_5 );
    actions.viewKeypad        ->setShortcut( Qt::CTRL + Qt::ALT + Qt::Key_K );
    actions.viewMathBook      ->setShortcut( Qt::CTRL + Qt::Key_1 );
#ifndef Q_OS_MAC
    actions.viewMenuBar       ->setShortcut( Qt::CTRL + Qt::ALT + Qt::Key_M );
#endif
    actions.viewStatusBar     ->setShortcut( Qt::CTRL + Qt::ALT + Qt::Key_B );
    actions.viewVariables     ->setShortcut( Qt::CTRL + Qt::Key_4           );

    actions.settingsAngleUnitDegree        ->setShortcut( Qt::Key_F10 );
    actions.settingsAngleUnitRadian        ->setShortcut( Qt::Key_F9 );
    actions.settingsResultFormatBinary     ->setShortcut( Qt::Key_F5 );
    actions.settingsResultFormatGeneral    ->setShortcut( Qt::Key_F7 );
    actions.settingsResultFormatHexadecimal->setShortcut( Qt::Key_F8 );
    actions.settingsResultFormatOctal      ->setShortcut( Qt::Key_F6 );
    actions.settingsRadixCharDot           ->setShortcut( Qt::CTRL + Qt::Key_Period );
    actions.settingsRadixCharComma         ->setShortcut( Qt::CTRL + Qt::Key_Comma );

    actions.helpTipOfTheDay->setShortcut( Qt::CTRL + Qt::Key_T );

    actions.scrollDown->setShortcut( Qt::SHIFT + Qt::Key_PageDown );
    actions.scrollUp  ->setShortcut( Qt::SHIFT + Qt::Key_PageUp );
}

void MainWindow::Private::createMenus()
{
    menus.session = new QMenu( "", p );
    p->menuBar()->addMenu( menus.session );
    menus.session->addAction( actions.sessionLoad );
    menus.session->addAction( actions.sessionSave );
    menus.session->addSeparator();
    menus.session->addAction( actions.sessionImport );
    menus.session->addAction( actions.sessionExport );
    menus.session->addSeparator();
    menus.session->addAction( actions.sessionQuit );

    menus.edit = new QMenu( "", p );
    p->menuBar()->addMenu( menus.edit );
    menus.edit->addAction( actions.editCopy );
    menus.edit->addAction( actions.editCopyLastResult );
    menus.edit->addAction( actions.editPaste );
    menus.edit->addAction( actions.editSelectExpression );
    menus.edit->addSeparator();
    menus.edit->addAction( actions.editInsertFunction );
    menus.edit->addAction( actions.editInsertVariable );
    menus.edit->addSeparator();
    menus.edit->addAction( actions.editDeleteVariable );
    menus.edit->addAction( actions.editDeleteAllVariables );
    menus.edit->addSeparator();
    menus.edit->addAction( actions.editClearExpression );
    menus.edit->addAction( actions.editClearHistory );

    menus.view = new QMenu( "", p );
    p->menuBar()->addMenu( menus.view );
    menus.view->addAction( actions.viewKeypad );
    menus.view->addSeparator();
    menus.view->addAction( actions.viewMathBook );
    menus.view->addAction( actions.viewConstants );
    menus.view->addAction( actions.viewFunctions );
    menus.view->addAction( actions.viewVariables );
    menus.view->addAction( actions.viewHistory );
    menus.view->addSeparator();
    menus.view->addAction( actions.viewStatusBar );
#ifndef Q_OS_MAC
    menus.view->addAction( actions.viewMenuBar );
#endif
    menus.view->addSeparator();
    menus.view->addAction( actions.viewFullScreenMode );

    menus.settings = new QMenu( "", p );
    p->menuBar()->addMenu( menus.settings );

    menus.resultFormat = menus.settings->addMenu( "" );

    menus.decimal = menus.resultFormat->addMenu( "" );
    menus.decimal->addAction( actions.settingsResultFormatGeneral );
    menus.decimal->addAction( actions.settingsResultFormatFixed );
    menus.decimal->addAction( actions.settingsResultFormatEngineering );
    menus.decimal->addAction( actions.settingsResultFormatScientific );
    menus.decimal->addSeparator();
    menus.precision = menus.decimal->addMenu( "" );
    menus.precision->addAction( actions.settingsResultFormatAutoPrecision );
    menus.precision->addAction( actions.settingsResultFormat2Digits );
    menus.precision->addAction( actions.settingsResultFormat3Digits );
    menus.precision->addAction( actions.settingsResultFormat8Digits );
    menus.precision->addAction( actions.settingsResultFormat15Digits );
    menus.precision->addAction( actions.settingsResultFormat50Digits );

    menus.resultFormat->addSeparator();

    menus.resultFormat->addAction( actions.settingsResultFormatBinary );
    menus.resultFormat->addAction( actions.settingsResultFormatOctal );
    menus.resultFormat->addAction( actions.settingsResultFormatHexadecimal );

    menus.resultFormat->addSeparator();

    menus.radixChar = menus.resultFormat->addMenu( "" );
    menus.radixChar->addAction( actions.settingsRadixCharDefault );
    menus.radixChar->addSeparator();
    menus.radixChar->addAction( actions.settingsRadixCharDot );
    menus.radixChar->addAction( actions.settingsRadixCharComma );

    menus.angleUnit = menus.settings->addMenu( "" );
    menus.angleUnit->addAction( actions.settingsAngleUnitRadian );
    menus.angleUnit->addAction( actions.settingsAngleUnitDegree );

    menus.behavior = menus.settings->addMenu( "" );
    menus.behavior->addAction( actions.settingsBehaviorSaveHistoryOnExit );
    menus.behavior->addAction( actions.settingsBehaviorSaveVariablesOnExit );
    menus.behavior->addSeparator();
    menus.behavior->addAction( actions.settingsBehaviorPartialResults );
    menus.behavior->addAction( actions.settingsBehaviorAutoCompletion );
    menus.behavior->addAction( actions.settingsBehaviorSyntaxHilite );
    menus.behavior->addSeparator();
    menus.behavior->addAction( actions.settingsBehaviorAlwaysOnTop );
    menus.behavior->addAction( actions.settingsBehaviorMinimizeToTray );

    menus.settings->addAction( actions.settingsLanguage );

    menus.help = new QMenu( "", p );
    p->menuBar()->addMenu( menus.help );
    menus.help->addAction( actions.helpTipOfTheDay );
    menus.help->addAction( actions.helpWebsite );
    menus.help->addSeparator();
    menus.help->addAction( actions.helpAbout );
    menus.help->addAction( actions.helpAboutQt );

    p->addActions( p->menuBar()->actions() );
    p->addAction( actions.scrollDown );
    p->addAction( actions.scrollUp );
}

void MainWindow::Private::setMenusText()
{
    menus.session     ->setTitle( MainWindow::tr("&Session") );
    menus.edit        ->setTitle( MainWindow::tr("&Edit") );
    menus.view        ->setTitle( MainWindow::tr("&View") );
    menus.settings    ->setTitle( MainWindow::tr("Se&ttings") );
    menus.resultFormat->setTitle( MainWindow::tr("Result &Format") );
    menus.radixChar   ->setTitle( MainWindow::tr("Radix &Character") );
    menus.decimal     ->setTitle( MainWindow::tr("&Decimal") );
    menus.precision   ->setTitle( MainWindow::tr("&Precision") );
    menus.angleUnit   ->setTitle( MainWindow::tr("&Angle Unit") );
    menus.behavior    ->setTitle( MainWindow::tr("&Behavior") );
    menus.help        ->setTitle( MainWindow::tr("&Help") );
}

void MainWindow::Private::createStatusBar()
{
    QStatusBar * bar = p->statusBar();

    status.angleUnit = new QLabel( bar );
    status.resultFormat = new QLabel( bar );

    status.angleUnit->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    status.resultFormat->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );

    bar->addWidget( status.angleUnit );
    bar->addWidget( status.resultFormat );

    setStatusBarText();
}

void MainWindow::Private::createFixedWidgets()
{
    // necessary objects
    constants = Constants::instance();
    evaluator = Evaluator::instance();
    functions = Functions::instance();

    // outer widget and layout
    widgets.root = new QWidget( p );
    p->setCentralWidget( widgets.root );

    layouts.root = new QVBoxLayout( widgets.root );
    layouts.root->setMargin( 0 );
    layouts.root->setSpacing( 0 );

    // display
    QHBoxLayout * displayLayout = new QHBoxLayout();
    displayLayout->setMargin( 5 );
    widgets.display = new ResultDisplay( widgets.root );
    displayLayout->addWidget( widgets.display );
    layouts.root->addLayout( displayLayout );

    // editor
    QHBoxLayout * editorLayout = new QHBoxLayout();
    editorLayout->setMargin( 5 );
    widgets.editor = new Editor( widgets.root );
    widgets.editor->setFocus();
    QString editorStyle( "QTextEdit { font: bold %1pt }" );
    const int editorPointSize = widgets.editor->font().pointSize();
    widgets.editor->setStyleSheet( editorStyle.arg(editorPointSize + 3) );
    widgets.editor->setFixedHeight( widgets.editor->sizeHint().height() );
    editorLayout->addWidget( widgets.editor );
    layouts.root->addLayout( editorLayout );

    // for autocalc
    widgets.autoCalcTip = new AutoHideLabel( p );
    widgets.autoCalcTip->hide();

    // for tip of the day and menu bar hiding message
    widgets.tip = new TipWidget( p );
    widgets.tip->hide();
}

void MainWindow::Private::createKeypad()
{
    widgets.keypad = new Keypad( widgets.root );
    widgets.keypad->setFocusPolicy( Qt::NoFocus );

    connect( widgets.keypad, SIGNAL(buttonPressed(Keypad::Button)),
             p, SLOT( handleKeypadButtonPress(Keypad::Button)) );
    connect( p, SIGNAL(radixCharacterChanged()),
             widgets.keypad, SLOT(handleRadixCharacterChange()) );

    layouts.keypad = new QHBoxLayout();
    layouts.keypad->addStretch();
    layouts.keypad->addWidget( widgets.keypad );
    layouts.keypad->addStretch();
    layouts.root->addLayout( layouts.keypad );

    widgets.keypad->show();
    widgets.display->scrollEnd();
}

void MainWindow::Private::createBookDock()
{
    QString booksDir;

#if defined(Q_OS_WIN32) || defined(Q_OS_MAC)
    QString appPath = QApplication::applicationFilePath();
    int ii = appPath.lastIndexOf( '/' );
    if ( ii > 0 )
        appPath.remove( ii, appPath.length() );
    booksDir = appPath + '/' + QString( "books/" );
#else
    BrInitError error;
    if ( br_init( & error ) == 0 && error != BR_INIT_ERROR_DISABLED ) {
        qDebug( "Warning: BinReloc failed to initialize (error code %d)", error );
        qDebug( "Will fallback to hardcoded default path." );
    }

    char * dataDir = br_find_data_dir( 0 );
    booksDir = QString( dataDir ) + "/speedcrunch/books/";
    free( dataDir );
#endif

    docks.book = new BookDock( booksDir, "math_index.html", p );
    docks.book->setObjectName( "BookDock" );
    docks.book->setTitle( MainWindow::tr("Math Book") );
    docks.book->installEventFilter( p );
    docks.book->setAllowedAreas( Qt::AllDockWidgetAreas );
    p->addDockWidget( Qt::RightDockWidgetArea, docks.book );

    connect( docks.book, SIGNAL(expressionSelected(const QString &)),
             p, SLOT(insertTextIntoEditor(const QString &)) );

    if ( docks.functions )
        p->tabifyDockWidget( docks.functions, docks.book );
    else if ( docks.variables )
        p->tabifyDockWidget( docks.variables, docks.book );
    else if ( docks.history )
        p->tabifyDockWidget( docks.history, docks.book );
    else if ( docks.constants )
        p->tabifyDockWidget( docks.constants, docks.book );

    docks.book->show();
    docks.book->raise();

    settings->mathBookDockVisible = true;
}

void MainWindow::Private::createConstantsDock()
{
    docks.constants = new ConstantsDock( p );
    docks.constants->setObjectName( "ConstantsDock" );
    docks.constants->installEventFilter( p );
    docks.constants->setAllowedAreas( Qt::AllDockWidgetAreas );
    p->addDockWidget( Qt::RightDockWidgetArea, docks.constants );

    connect( docks.constants->widget(), SIGNAL(constantSelected(const QString &)),
             p, SLOT(insertConstantIntoEditor(const QString &)) );
    connect( p, SIGNAL(radixCharacterChanged()),
             docks.constants->widget(), SLOT(handleRadixCharacterChange()) );

    if ( docks.functions )
        p->tabifyDockWidget( docks.functions, docks.constants );
    else if ( docks.variables )
        p->tabifyDockWidget( docks.variables, docks.constants );
    else if ( docks.history )
        p->tabifyDockWidget( docks.history, docks.constants );
    else if ( docks.book )
        p->tabifyDockWidget( docks.book, docks.constants );

    docks.constants->show();
    docks.constants->raise();

    settings->constantsDockVisible = true;
}

void MainWindow::Private::createFunctionsDock()
{
    docks.functions = new FunctionsDock( p );
    docks.functions->setObjectName( "FunctionsDock" );
    docks.functions->installEventFilter( p );
    docks.functions->setAllowedAreas( Qt::AllDockWidgetAreas );
    p->addDockWidget( Qt::RightDockWidgetArea, docks.functions );

    connect( docks.functions->widget(), SIGNAL(functionSelected(const QString &)),
             p, SLOT(insertFunctionIntoEditor(const QString &)) );

    if ( docks.history )
        p->tabifyDockWidget( docks.history, docks.functions );
    else if ( docks.variables )
        p->tabifyDockWidget( docks.variables, docks.functions );
    else if ( docks.constants )
        p->tabifyDockWidget( docks.constants, docks.functions );
    else if ( docks.book )
        p->tabifyDockWidget( docks.book, docks.functions );

    docks.functions->show();
    docks.functions->raise();

    settings->functionsDockVisible = true;
}

void MainWindow::Private::createHistoryDock()
{
    docks.history = new HistoryDock( p );
    docks.history->setObjectName( "HistoryDock" );
    docks.history->installEventFilter( p );
    docks.history->setAllowedAreas( Qt::AllDockWidgetAreas );
    p->addDockWidget( Qt::RightDockWidgetArea, docks.history );

    HistoryWidget * history = qobject_cast<HistoryWidget *>( docks.history->widget() );
    connect( history, SIGNAL(expressionSelected(const QString &)),
             p, SLOT(insertTextIntoEditor(const QString &)) );
    connect( p, SIGNAL(languageChanged()), docks.history, SLOT(retranslateText()) );

    HistoryWidget * historyWidget = qobject_cast<HistoryWidget *>( docks.history->widget() );
    historyWidget->setHistory( widgets.editor->history() );

    if ( docks.functions )
        p->tabifyDockWidget( docks.functions, docks.history );
    else if ( docks.variables )
        p->tabifyDockWidget( docks.variables, docks.history );
    else if ( docks.constants )
        p->tabifyDockWidget( docks.constants, docks.history );
    else if ( docks.book )
        p->tabifyDockWidget( docks.book, docks.history );

    docks.history->show();
    docks.history->raise();

    settings->historyDockVisible = true;
}

void MainWindow::Private::createVariablesDock()
{
    docks.variables = new VariablesDock( p );
    docks.variables->setObjectName( "VariablesDock" );
    docks.variables->installEventFilter( p );
    docks.variables->setAllowedAreas( Qt::AllDockWidgetAreas );
    p->addDockWidget( Qt::RightDockWidgetArea, docks.variables );

    connect( docks.variables, SIGNAL(variableSelected(const QString &)),
             p, SLOT(insertVariableIntoEditor(const QString &)) );
    connect( p, SIGNAL(radixCharacterChanged()),
            docks.variables, SLOT(handleRadixCharacterChange()));
    connect( p, SIGNAL(languageChanged()), docks.variables, SLOT(retranslateText()) );

    docks.variables->updateList();

    if ( docks.functions )
        p->tabifyDockWidget( docks.functions, docks.variables );
    else if ( docks.history )
        p->tabifyDockWidget( docks.history, docks.variables );
    else if ( docks.constants )
        p->tabifyDockWidget( docks.constants, docks.variables );
    else if ( docks.book )
        p->tabifyDockWidget( docks.book, docks.variables );

    docks.variables->show();
    docks.variables->raise();

    settings->variablesDockVisible = true;
}

void MainWindow::Private::createFixedConnections()
{
    connect( actions.sessionExport, SIGNAL(triggered()), p, SLOT(showSessionExportDialog()) );
    connect( actions.sessionImport, SIGNAL(triggered()), p, SLOT(showSessionImportDialog()) );
    connect( actions.sessionLoad,   SIGNAL(triggered()), p, SLOT(showSessionLoadDialog()) );
    connect( actions.sessionQuit,   SIGNAL(triggered()), p, SLOT(close()) );
    connect( actions.sessionSave,   SIGNAL(triggered()), p, SLOT(saveSession()) );

    connect( actions.editClearExpression, SIGNAL(triggered()), p, SLOT(clearEditor()) );
    connect( actions.editClearHistory, SIGNAL(triggered()), p, SLOT(clearHistory()) );
    connect( actions.editCopyLastResult, SIGNAL(triggered()), p, SLOT(copyResultToClipboard()) );
    connect( actions.editCopy, SIGNAL(triggered()), widgets.editor, SLOT(copy()) );
    connect( actions.editDeleteAllVariables, SIGNAL(triggered()), p, SLOT(deleteAllVariables()) );
    connect( actions.editDeleteVariable, SIGNAL(triggered()), p,
             SLOT(showVariableDeletionDialog()) );
    connect( actions.editInsertFunction, SIGNAL(triggered()), p,
             SLOT(showFunctionInsertionDialog()) );
    connect( actions.editInsertVariable, SIGNAL(triggered()), p,
             SLOT(showVariableInsertionDialog()) );
    connect( actions.editPaste, SIGNAL(triggered()), widgets.editor, SLOT(paste()) );
    connect( actions.editSelectExpression, SIGNAL(triggered()), p, SLOT(selectEditorExpression()) );

    connect( actions.viewConstants, SIGNAL(toggled(bool)), p, SLOT(setConstantsDockVisible(bool)) );
    connect( actions.viewFullScreenMode, SIGNAL(toggled(bool)), p,
             SLOT(setFullScreenEnabled(bool)) );
    connect( actions.viewFunctions, SIGNAL(toggled(bool)), p, SLOT(setFunctionsDockVisible(bool)) );
    connect( actions.viewHistory, SIGNAL(toggled(bool)), p, SLOT(setHistoryDockVisible(bool)) );
    connect( actions.viewKeypad, SIGNAL(toggled(bool)), p, SLOT(setKeypadVisible(bool)) );
    connect( actions.viewMathBook, SIGNAL(toggled(bool)), p, SLOT(setMathBookDockVisible(bool)) );
    connect( actions.viewMenuBar, SIGNAL(toggled(bool)), p, SLOT(setMenuBarVisible(bool)) );
    connect( actions.viewStatusBar, SIGNAL(toggled(bool)), p, SLOT(setStatusBarVisible(bool)) );
    connect( actions.viewVariables, SIGNAL(toggled(bool)), p,
             SLOT(setVariablesDockVisible(bool)) );

    connect( actions.settingsAngleUnitDegree, SIGNAL(triggered()), p, SLOT(setAngleModeDegree()) );
    connect( actions.settingsAngleUnitRadian, SIGNAL(triggered()), p, SLOT(setAngleModeRadian()) );

    connect( actions.settingsBehaviorAlwaysOnTop, SIGNAL(toggled(bool)), p,
             SLOT(setAlwaysOnTopEnabled(bool)) );
    connect( actions.settingsBehaviorAutoCompletion, SIGNAL(toggled(bool)), p,
             SLOT(setAutoCompletionEnabled(bool)) );
    connect( actions.settingsBehaviorMinimizeToTray, SIGNAL(toggled(bool)), p,
             SLOT(setSystemTrayIconEnabled(bool)) );
    connect( actions.settingsBehaviorPartialResults, SIGNAL(toggled(bool)), p,
             SLOT(setAutoCalcEnabled(bool)) );
    connect( actions.settingsBehaviorSaveHistoryOnExit, SIGNAL(toggled(bool)), p,
             SLOT(setHistorySaveEnabled(bool)) );
    connect( actions.settingsBehaviorSaveVariablesOnExit, SIGNAL(toggled(bool)), p,
             SLOT(setVariableSaveEnabled(bool)) );
    connect( actions.settingsBehaviorSyntaxHilite, SIGNAL(toggled(bool)), p,
             SLOT(setSyntaxHighlightingEnabled(bool)) );

    connect( actions.settingsRadixCharComma, SIGNAL(triggered()), p,
             SLOT(setRadixCharacterComma()) );
    connect( actions.settingsRadixCharDefault, SIGNAL(triggered()), p,
             SLOT(setRadixCharacterAutomatic()) );
    connect( actions.settingsRadixCharDot, SIGNAL(triggered()), p, SLOT(setRadixCharacterDot()) );

    connect( actions.settingsResultFormat15Digits, SIGNAL(triggered()), p,
             SLOT(setResultPrecision15Digits()) );
    connect( actions.settingsResultFormat2Digits, SIGNAL(triggered()), p,
             SLOT(setResultPrecision2Digits()) );
    connect( actions.settingsResultFormat3Digits, SIGNAL(triggered()), p,
             SLOT(setResultPrecision3Digits()) );
    connect( actions.settingsResultFormat50Digits, SIGNAL(triggered()), p,
             SLOT(setResultPrecision50Digits()) );
    connect( actions.settingsResultFormat8Digits, SIGNAL(triggered()), p,
             SLOT(setResultPrecision8Digits()) );
    connect( actions.settingsResultFormatAutoPrecision, SIGNAL(triggered()), p,
             SLOT(setResultPrecisionAutomatic()) );
    connect( actions.settingsResultFormatBinary, SIGNAL(triggered()), p,
             SLOT(setResultFormatBinary()) );
    connect( actions.settingsResultFormatEngineering, SIGNAL(triggered()), p,
             SLOT(setResultFormatEngineering()) );
    connect( actions.settingsResultFormatFixed, SIGNAL(triggered()), p,
             SLOT(setResultFormatFixed()) );
    connect( actions.settingsResultFormatGeneral, SIGNAL(triggered()), p,
             SLOT(setResultFormatGeneral()) );
    connect( actions.settingsResultFormatHexadecimal, SIGNAL(triggered()), p,
             SLOT(setResultFormatHexadecimal()) );
    connect( actions.settingsResultFormatOctal, SIGNAL(triggered()), p,
             SLOT(setResultFormatOctal()) );
    connect( actions.settingsResultFormatScientific, SIGNAL(triggered()), p,
             SLOT(setResultFormatScientific()) );

    connect( actions.settingsLanguage, SIGNAL(triggered()), p, SLOT(showLanguageChooserDialog()) );

    connect( actions.helpAboutQt, SIGNAL(triggered()), p, SLOT(showAboutQtDialog()) );
    connect( actions.helpAbout, SIGNAL(triggered()), p, SLOT(showAboutDialog()) );
    connect( actions.helpTipOfTheDay, SIGNAL(triggered()), p, SLOT(showTipOfTheDay()) );
    connect( actions.helpWebsite, SIGNAL(triggered()), p, SLOT(openHomePageInSystemBrowser()) );

    connect( widgets.display, SIGNAL(textSelected(const QString &)), widgets.editor,
             SLOT(paste()) );
    connect( widgets.display, SIGNAL(textSelected(const QString &)), widgets.editor,
             SLOT(setFocus()) );

    connect( widgets.editor, SIGNAL(autoCalcDisabled()), p, SLOT(hideAutoCalcTip()) );
    connect( widgets.editor, SIGNAL(autoCalcEnabled(const QString &)), p,
             SLOT(showAutoCalcTip(const QString &)) );
    connect( widgets.editor, SIGNAL(returnPressed()), p, SLOT( evaluateEditorExpression()) );
    connect( widgets.editor, SIGNAL(textChanged()), p, SLOT( handleEditorTextChange()) );

    connect( p, SIGNAL(radixCharacterChanged()), widgets.display,
             SLOT(handleRadixCharacterChange()) );
    connect( p, SIGNAL(resultFormatChanged()), widgets.display, SLOT(handleResultFormatChange()) );
    connect( p, SIGNAL(resultPrecisionChanged()), widgets.display,
             SLOT(handleResultPrecisionChange()) );

    connect( p, SIGNAL(languageChanged()), p, SLOT(retranslateText()) );

    connect( actions.scrollDown, SIGNAL(triggered()), p, SLOT(scrollDown()) );
    connect( actions.scrollUp,   SIGNAL(triggered()), p, SLOT(scrollUp()) );
}

void MainWindow::Private::applySettings()
{
    // window state
    //
    actions.viewMathBook ->setChecked( settings->mathBookDockVisible  );
    actions.viewConstants->setChecked( settings->constantsDockVisible );
    actions.viewFunctions->setChecked( settings->functionsDockVisible );
    actions.viewHistory  ->setChecked( settings->historyDockVisible   );
    actions.viewVariables->setChecked( settings->variablesDockVisible );
    //
    p->resize( settings->windowSize );
    //
    if ( settings->windowPosition.isNull() ) {
        QDesktopWidget * desktop = QApplication::desktop();
        QRect screen = desktop->availableGeometry( p );
        p->move( screen.center() - p->rect().center() );
    } else
        p->move( settings->windowPosition );
    //
    p->restoreState( settings->windowState );

    // full screen
    actions.viewFullScreenMode->setChecked( settings->windowOnfullScreen );

    // always-on-top
    actions.settingsBehaviorAlwaysOnTop->setChecked( settings->windowAlwaysOnTop);

    // angle mode
    if ( settings->angleUnit == 'r' )
        actions.settingsAngleUnitRadian->setChecked( true );
    else if ( settings->angleUnit == 'd' )
        actions.settingsAngleUnitDegree->setChecked( true );

    // history
    if ( settings->historySave ) {
        actions.settingsBehaviorSaveHistoryOnExit->setChecked( true );
        restoreHistory();
    }

    // variables
    if ( settings->variableSave ) {
        actions.settingsBehaviorSaveVariablesOnExit->setChecked( true );
        restoreVariables();
    }

    // resultFormat
    checkInitialResultFormat();

    // precision
    checkInitialResultPrecision();

    // radix character
    if ( settings->isRadixCharacterAuto() )
        actions.settingsRadixCharDefault->setChecked( true );
    else if ( settings->radixCharacter() == '.' )
        actions.settingsRadixCharDot->setChecked( true );
    else if ( settings->radixCharacter() == ',' )
        actions.settingsRadixCharComma->setChecked( true );

    // keypad
    actions.viewKeypad->setChecked( settings->keypadVisible );

    // autocalc
    if ( settings->autoCalc )
        actions.settingsBehaviorPartialResults->setChecked( true );
    else
        p->setAutoCalcEnabled( false );

    // autocomplete
    if ( settings->autoCompletion )
        actions.settingsBehaviorAutoCompletion->setChecked( true );
    else
        p->setAutoCompletionEnabled( false );

    // minimize to system tray
    actions.settingsBehaviorMinimizeToTray->setChecked( settings->systemTrayIconVisible );

    // syntax hilite
    if ( settings->syntaxHighlighting )
        actions.settingsBehaviorSyntaxHilite->setChecked( true );
    else
        p->setSyntaxHighlightingEnabled( false );

    // status bar
    actions.viewStatusBar->setChecked( settings->statusBarVisible );

    // menu bar
#ifndef Q_OS_MAC
    actions.viewMenuBar->setChecked( settings->menuBarVisible );
    p->menuBar()->setVisible( settings->menuBarVisible );
#endif

    emit p->languageChanged();
}

void MainWindow::Private::checkInitialResultFormat()
{
    switch ( settings->resultFormat ) {
        case 'f': actions.settingsResultFormatFixed      ->setChecked( true ); break;
        case 'n': actions.settingsResultFormatEngineering->setChecked( true ); break;
        case 'e': actions.settingsResultFormatScientific ->setChecked( true ); break;
        case 'h': actions.settingsResultFormatHexadecimal->setChecked( true ); break;
        case 'o': actions.settingsResultFormatOctal      ->setChecked( true ); break;
        case 'b': actions.settingsResultFormatBinary     ->setChecked( true ); break;
        default : actions.settingsResultFormatGeneral    ->setChecked( true );
    }
}

void MainWindow::Private::checkInitialResultPrecision()
{
    switch ( settings->resultPrecision ) {
        case 2:  actions.settingsResultFormat2Digits      ->setChecked( true ); break;
        case 3:  actions.settingsResultFormat3Digits      ->setChecked( true ); break;
        case 8:  actions.settingsResultFormat8Digits      ->setChecked( true ); break;
        case 15: actions.settingsResultFormat15Digits     ->setChecked( true ); break;
        case 50: actions.settingsResultFormat50Digits     ->setChecked( true ); break;
        default: actions.settingsResultFormatAutoPrecision->setChecked( true );
    }
}

void MainWindow::Private::saveSettings()
{
    // history
    if ( settings->historySave ) {
        settings->history = widgets.editor->history();
        settings->historyResults = widgets.editor->historyResults();
    } else {
        settings->history.clear();
        settings->historyResults.clear();
    }

    // variables
    if ( settings->variableSave ) {
        settings->variables.clear();
        QVector<Variable> vars = evaluator->variables();
        for ( int i = 0; i < vars.count(); i++ ) {
            QString name = vars.at( i ).name;
            char * value = HMath::format( vars.at(i).value, 'e', DECPRECISION );
            settings->variables.append( QString("%1=%2").arg(name).arg(value) );
            free( value );
        }
    }

    // window
    settings->windowPosition = p->pos();
    settings->windowSize = p->size();
    settings->windowState = p->saveState();

    settings->save();
}

MainWindow::MainWindow()
    : QMainWindow(), d( new MainWindow::Private )
{
    d->p = this;
    d->createUi();
    d->applySettings();
    QTimer::singleShot( 0, this, SLOT(activate()) );
}

MainWindow::~MainWindow()
{
}

bool MainWindow::event( QEvent * e )
{
    if ( e->type() == QEvent::WindowStateChange )
        if ( windowState() & Qt::WindowMinimized )
            if ( d->settings->systemTrayIconVisible ) {
                QTimer::singleShot( 100, this, SLOT(minimizeToSystemTray()) );
                return true;
            }

    return QMainWindow::event( e );
}

void MainWindow::showAboutDialog()
{
    AboutBox dlg;
    dlg.exec();
}

void MainWindow::showAboutQtDialog()
{
    QMessageBox::aboutQt( this, tr("About Qt") );
}

void MainWindow::clearHistory()
{
    d->widgets.display->clear();
    d->widgets.editor->clearHistory();
    if ( d->settings->historyDockVisible ) {
        HistoryWidget * history = qobject_cast<HistoryWidget *>( d->docks.history->widget() );
        history->clear();
    }
    d->settings->history.clear();
    d->settings->historyResults.clear();
    QTimer::singleShot( 0, d->widgets.editor, SLOT(setFocus()) );
}

void MainWindow::clearEditor()
{
    d->widgets.editor->clear();
    QTimer::singleShot( 0, d->widgets.editor, SLOT(setFocus()) );
}

void MainWindow::copyResultToClipboard()
{
    QClipboard * cb = QApplication::clipboard();
    HNumber num = d->evaluator->get( "ans" );
    char * strToCopy = HMath::format( num, d->settings->resultFormat,
                                      d->settings->resultPrecision );
    QString final( strToCopy );
    if ( d->settings->radixCharacter() == ',' )
        final.replace( '.', ',' );
    cb->setText( final, QClipboard::Clipboard );
    free( strToCopy );
}

void MainWindow::setAngleModeDegree()
{
    if ( d->settings->angleUnit == 'd' )
        return;

    d->settings->angleUnit = 'd';

    if ( d->status.angleUnit )
        d->status.angleUnit->setText( tr("Degree") );

    emit angleUnitChanged();
}

void MainWindow::deleteAllVariables()
{
    d->evaluator->clearVariables();

    if ( d->settings->variablesDockVisible )
        d->docks.variables->updateList();
}

void MainWindow::showVariableDeletionDialog()
{
    DeleteVariableDlg dlg;
    dlg.exec();

    if ( d->settings->variablesDockVisible )
        d->docks.variables->updateList();
}

void MainWindow::setResultPrecision2Digits()
{
    setResultPrecision( 2 );
}

void MainWindow::setResultPrecision3Digits()
{
    setResultPrecision( 3 );
}

void MainWindow::setResultPrecision8Digits()
{
    setResultPrecision( 8 );
}

void MainWindow::setResultPrecision15Digits()
{
    setResultPrecision( 15 );
}

void MainWindow::setResultPrecision50Digits()
{
    setResultPrecision( 50 );
}

void MainWindow::setResultPrecisionAutomatic()
{
    setResultPrecision( -1 );
}

void MainWindow::selectEditorExpression()
{
    activateWindow();
    d->widgets.editor->selectAll();
    d->widgets.editor->setFocus();
}

void MainWindow::openHomePageInSystemBrowser()
{
    QDesktopServices::openUrl( QUrl(QString::fromLatin1("http://www.speedcrunch.org")) );
}

void MainWindow::hideAutoCalcTip()
{
    d->widgets.autoCalcTip->hideText();
}

void MainWindow::showFunctionInsertionDialog()
{
    InsertFunctionDlg dlg;

    if ( dlg.exec() == InsertFunctionDlg::Accepted )
        insertFunctionIntoEditor( dlg.selectedFunctionName() );
}

void MainWindow::showVariableInsertionDialog()
{
    InsertVariableDlg dlg;

    if ( dlg.exec() == InsertVariableDlg::Accepted ) {
        QString varName = dlg.variableName();
        if ( ! varName.isEmpty() )
            d->widgets.editor->insert( varName );
    }
}

void MainWindow::showSessionLoadDialog()
{
    QString errMsg  = tr( "File %1 is not a valid session" );
    QString filters = tr( "SpeedCrunch Sessions (*.sch);;All Files (*)" );
    QString fname = QFileDialog::getOpenFileName( this, tr("Load Session"),
                                                  QString::null, filters );
    if ( fname.isEmpty() )
        return;

    QFile file( fname );
    if ( ! file.open( QIODevice::ReadOnly ) ) {
        QMessageBox::critical( this, tr("Error"), tr("Can't read from file %1").arg(fname) );
        return;
    }

    QTextStream stream( &file );

    // version of the format
    QString version = stream.readLine();
    if ( version != "0.10" ) {
        QMessageBox::critical( this, tr("Error"), errMsg.arg(fname) );
        return;
    }

    // number of calculations
    bool ok;
    int noCalcs = stream.readLine().toInt( &ok );
    if ( ok == false || noCalcs < 0 ) {
        QMessageBox::critical( this, tr("Error"), errMsg.arg(fname) );
        return;
    }

    // ask for merge with current session
    QString mergeMsg = tr( "Merge session being loaded with current session?\n"
                           "If no, current variables and display will be "
                           "cleared." );
    QMessageBox::StandardButton but =
        QMessageBox::question( this, tr("Question"), mergeMsg,
                               QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                               QMessageBox::Yes );

    if ( but == QMessageBox::Cancel )
        return;
    else if ( but == QMessageBox::No ) {
        d->widgets.display->clear();
        deleteAllVariables();
        clearHistory();
    }

    // expressions and results
    QStringList expLs;
    QStringList resLs;
    for ( int i = 0; i < noCalcs; i++ ) {
        QString exp = stream.readLine();
        QString res = stream.readLine();
        if ( exp.isNull() || res.isNull() ) {
            QMessageBox::critical( this, tr("Error"), errMsg.arg(fname) );
            return;
        }
        expLs.append( exp );
        resLs.append( res );
    }

    // FIXME: this keeps the history in three (finally even four) different places.
    // There should be just one central history and result list, and the clients
    // editor, display, settings and historyDock should retrieve their data from there.
    // FIXME: Actually there should be three lists: expressions, values and formats.
    d->widgets.display->appendHistory( expLs, resLs );
    d->widgets.editor->appendHistory( expLs, resLs );

    if ( d->settings->historyDockVisible ) {
        HistoryWidget * history = qobject_cast<HistoryWidget *>( d->docks.history->widget() );
        history->appendHistory( expLs );
    }

    // variables
    int noVars = stream.readLine().toInt( &ok );
    if ( ok == false || noVars < 0 ) {
        QMessageBox::critical( this, tr("Error"), errMsg.arg(fname) );
        return;
    }
    for ( int i = 0; i < noVars; i++ ) {
        QString var = stream.readLine();
        QString val = stream.readLine();
        if ( var.isNull() || val.isNull() ) {
            QMessageBox::critical( this, tr("Error"), errMsg.arg(fname) );
            return;
        }
        HNumber num( val.toAscii().data() );
        if (  num != HMath::nan() )
            d->evaluator->set( var, num );
    }

    file.close();
}

void MainWindow::showSessionImportDialog()
{
    QString filters = tr( "All Files (*)" );
    QString fname = QFileDialog::getOpenFileName( this, tr("Import Session"),
                                                  QString::null, filters );
    if ( fname.isEmpty() )
        return;

    QFile file( fname );
    if ( ! file.open( QIODevice::ReadOnly ) ) {
        QMessageBox::critical( this, tr("Error"), tr("Can't read from file %1").arg(fname) );
        return;
    }

    // ask for merge with current session
    QString mergeMsg = tr( "Merge session being imported with current session?\n"
                           "If no, current variables and display will be "
                           "cleared." );
    QMessageBox::StandardButton but
        = QMessageBox::question( this, tr("Question"), mergeMsg,
                                 QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                 QMessageBox::Yes );

    if ( but == QMessageBox::Cancel )
        return;
    else if ( but == QMessageBox::No ) {
        d->widgets.display->clear();
        deleteAllVariables();
        clearHistory();
    }

    QTextStream stream( &file );
    QString exp = stream.readLine();
    bool ignoreAll = false;
    while ( ! exp.isNull() ) {
        d->widgets.editor->setText( exp );

        QString str = d->evaluator->autoFix( exp );

        d->evaluator->setExpression( str );

        HNumber result = d->evaluator->evalUpdateAns();
        if ( ! d->evaluator->error().isEmpty() ) {
            if ( ! ignoreAll ) {
                QMessageBox::StandardButton but
                    = QMessageBox::warning( this, tr("Error"),
                                            tr("Ignore error?") + "\n" + d->evaluator->error(),
                                            QMessageBox::Yes | QMessageBox::YesToAll
                                            | QMessageBox::Cancel, QMessageBox::Yes );

                if ( but == QMessageBox::Cancel )
                    return;
                else if ( but == QMessageBox::YesToAll )
                    ignoreAll = true;
            }
        } else {
            d->widgets.display->append( str, result );
            char * num = HMath::format( result, 'e', DECPRECISION );
            d->widgets.editor->appendHistory( str, num );
            free( num );
            d->widgets.editor->setAnsAvailable( true );
            if ( d->settings->variablesDockVisible )
                d->docks.variables->updateList();
            if ( d->settings->historyDockVisible ) {
                HistoryWidget * history
                    = qobject_cast<HistoryWidget *>( d->docks.history->widget() );
                history->append( str );
            }

            d->widgets.editor->setText( str );
            d->widgets.editor->selectAll();
            d->widgets.editor->stopAutoCalc();
            d->widgets.editor->stopAutoComplete();
            d->conditions.autoAns = true;
        }

        exp = stream.readLine();
    }

    file.close();

    QTimer::singleShot( 0, d->widgets.editor, SLOT(setFocus()) );

    if ( ! isActiveWindow() )
        activateWindow();
}

void MainWindow::setAlwaysOnTopEnabled( bool b )
{
    d->settings->windowAlwaysOnTop = b;

    QPoint cur = mapToGlobal( QPoint(0, 0) );
    if ( b )
        setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint );
    else
        setWindowFlags( windowFlags() & (~ Qt::WindowStaysOnTopHint) );
    move( cur );
    show();
}

void MainWindow::setAutoCalcEnabled( bool b )
{
    d->settings->autoCalc = b;
    d->widgets.editor->setAutoCalcEnabled( b );
}

void MainWindow::setHistorySaveEnabled( bool b )
{
    d->settings->historySave = b;
}

void MainWindow::setVariableSaveEnabled( bool b )
{
    d->settings->variableSave = b;
}

void MainWindow::setAutoCompletionEnabled( bool b )
{
    d->settings->autoCompletion = b;
    d->widgets.editor->setAutoCompletionEnabled( b );
}

void MainWindow::setSystemTrayIconEnabled( bool b )
{
    if ( b && ! d->widgets.trayIcon && QSystemTrayIcon::isSystemTrayAvailable() ) {
        d->conditions.trayNotify = true;
        d->widgets.trayIcon = new QSystemTrayIcon( this );
        d->widgets.trayIcon->setToolTip( "SpeedCrunch" );
        d->widgets.trayIcon->setIcon( QPixmap(":/speedcrunch.png") );

        d->menus.trayIcon = new QMenu( this );
        d->menus.trayIcon->addAction( d->actions.editCopyLastResult );
        d->menus.trayIcon->addSeparator();
        d->menus.trayIcon->addAction( d->actions.sessionQuit    );

        d->widgets.trayIcon->setContextMenu( d->menus.trayIcon );
        connect( d->widgets.trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                 SLOT(handleSystemTrayIconActivation(QSystemTrayIcon::ActivationReason)) );
    } else {
        if ( d->widgets.trayIcon )
            d->widgets.trayIcon->deleteLater();
        d->widgets.trayIcon = 0;
    }

    d->settings->systemTrayIconVisible = b;
}

void MainWindow::setSyntaxHighlightingEnabled( bool b )
{
    d->widgets.editor->setSyntaxHighlightingEnabled( b );
    d->settings->syntaxHighlighting = b;
}

void MainWindow::setAngleModeRadian()
{
    if ( d->settings->angleUnit == 'r' )
        return;

    d->settings->angleUnit = 'r';

    if ( d->status.angleUnit )
        d->status.angleUnit->setText( tr("Radian") );

    emit angleUnitChanged();
}

void MainWindow::saveSession()
{
    QString filters = tr( "SpeedCrunch Sessions (*.sch);;All Files (*)" );
    QString fname = QFileDialog::getSaveFileName( this, tr("Save Session"),
                    QString::null, filters );
    if ( fname.isEmpty() )
        return;

    QFile file( fname );
    if ( ! file.open(QIODevice::WriteOnly) ) {
        QMessageBox::critical( this, tr("Error"), tr("Can't write to file %1").arg(fname) );
        return;
    }

    QTextStream stream( &file );

    // version of the format
    stream << "0.10" << "\n";

    // number of calculations
    stream << d->widgets.display->count() << "\n";

    // expressions and results
    stream << d->widgets.display->asText() << "\n";

    // number of variables
    int noVars = d->evaluator->variables().count();
    stream << noVars - 2 << "\n"; // exclude pi and phi

    // variables
    for ( int i = 0; i < noVars; i++ ) {
        Variable var = d->evaluator->variables().at( i );
        if ( var.name != "pi" && var.name != "phi" ) {
            char * value = HMath::format( var.value );
            stream << var.name << "\n" << value << "\n";
            free( value );
        }
    }

    file.close();
}

void MainWindow::showSessionExportDialog()
{
    QString filters = tr( "All Files (*)" );
    QString fname = QFileDialog::getSaveFileName( this, tr("Export Session"),
                                                  QString::null, filters );
    if ( fname.isEmpty() )
        return;

    QFile file( fname );
    if ( ! file.open(QIODevice::WriteOnly) ) {
        QMessageBox::critical( this, tr("Error"), tr("Can't write to file %1").arg(fname) );
        return;
    }

    QTextStream stream( &file );

    // expressions and results
    stream << d->widgets.display->asText() << "\n";

    file.close();
}

void MainWindow::setWidgetsDirection()
{
    QLocale::Language lang = QLocale().language();
    bool rtlSystem = (lang == QLocale::Hebrew || lang == QLocale::Arabic);

    QString code = d->settings->language;
    bool rtlCustom = (code.contains("he") || code.contains("ar"));

    if ( (d->settings->language == "C" && rtlSystem) || rtlCustom )
        qApp->setLayoutDirection( Qt::RightToLeft );
    else
        qApp->setLayoutDirection( Qt::LeftToRight );
}

void MainWindow::scrollDown()
{
    QScrollBar * sb = d->widgets.display->verticalScrollBar();
    int value = sb->value() + 40;
    sb->setValue( value );
}

void MainWindow::scrollUp()
{
    QScrollBar * sb = d->widgets.display->verticalScrollBar();
    int value = sb->value() - 40;
    sb->setValue( value );
}

void MainWindow::setMenuBarVisible( bool b )
{
    menuBar()->setVisible( b );
    d->settings->menuBarVisible = b;

    if ( ! b && d->conditions.notifyMenuBarHidden ) {
        showMenuBarTip();
        d->conditions.notifyMenuBarHidden = false;
    }

    d->widgets.display->scrollEnd();
}

void MainWindow::setStatusBarVisible( bool b )
{
    b ? d->createStatusBar() : d->deleteStatusBar();
    d->settings->statusBarVisible = b;
}

void MainWindow::showAutoCalcTip( const QString & msg )
{
    d->widgets.autoCalcTip->showText( msg );
    d->widgets.autoCalcTip->adjustSize();
    const int h = d->widgets.autoCalcTip->height();
    QPoint p = d->widgets.editor->mapToGlobal( QPoint(0, -h) );
    p = mapFromGlobal( p );
    d->widgets.autoCalcTip->move( p );
}

void MainWindow::setFullScreenEnabled( bool b )
{
    d->settings->windowOnfullScreen = b;
    b ? showFullScreen() : showNormal();
}

bool MainWindow::eventFilter( QObject * o, QEvent * e )
{
    if ( o == d->docks.book ) {
        if ( e->type() == QEvent::Close ) {
            d->deleteBookDock();
            return true;
        }
        return false;
    }

    if ( o == d->docks.constants ) {
        if ( e->type() == QEvent::Close ) {
            d->deleteConstantsDock();
            return true;
        }
        return false;
    }

    if ( o == d->docks.functions ) {
        if ( e->type() == QEvent::Close ) {
            d->deleteFunctionsDock();
            return true;
        }
        return false;
    }

    if ( o == d->docks.history ) {
        if ( e->type() == QEvent::Close ) {
            d->deleteHistoryDock();
            return true;
        }
        return false;
    }

    if ( o == d->docks.variables ) {
        if ( e->type() == QEvent::Close ) {
            d->deleteVariablesDock();
            return true;
        }
        return false;
    }

    return QMainWindow::eventFilter( o, e );
}

void MainWindow::Private::deleteKeypad()
{
    p->disconnect( widgets.keypad );
    widgets.keypad->deleteLater();
    widgets.keypad = 0;

    layouts.root->removeItem( layouts.keypad );
    layouts.keypad->deleteLater();
    layouts.keypad = 0;
}

void MainWindow::Private::deleteStatusBar()
{
    status.angleUnit->deleteLater();
    status.angleUnit = 0;

    status.resultFormat->deleteLater();
    status.resultFormat = 0;

    p->setStatusBar( 0 );
}

void MainWindow::Private::deleteBookDock()
{
    Q_ASSERT( docks.book );

    p->removeDockWidget( docks.book );
    p->disconnect( docks.book );
    docks.book->deleteLater();
    docks.book = 0;
    actions.viewMathBook->blockSignals( true );
    actions.viewMathBook->setChecked( false );
    actions.viewMathBook->blockSignals( false );
    settings->mathBookDockVisible = false;
}

void MainWindow::Private::deleteConstantsDock()
{
    Q_ASSERT( docks.constants );

    p->removeDockWidget( docks.constants );
    p->disconnect( docks.constants );
    docks.constants->deleteLater();
    docks.constants = 0;
    actions.viewConstants->blockSignals( true );
    actions.viewConstants->setChecked( false );
    actions.viewConstants->blockSignals( false );
    settings->constantsDockVisible = false;
}

void MainWindow::Private::deleteFunctionsDock()
{
    Q_ASSERT( docks.functions );

    p->removeDockWidget( docks.functions );
    p->disconnect( docks.functions );
    docks.functions->deleteLater();
    docks.functions = 0;
    actions.viewFunctions->blockSignals( true );
    actions.viewFunctions->setChecked( false );
    actions.viewFunctions->blockSignals( false );
    settings->functionsDockVisible = false;
}

void MainWindow::Private::deleteHistoryDock()
{
    Q_ASSERT( docks.history );

    p->removeDockWidget( docks.history );
    p->disconnect( docks.history );
    docks.history->deleteLater();
    docks.history = 0;
    actions.viewHistory->blockSignals( true );
    actions.viewHistory->setChecked( false );
    actions.viewHistory->blockSignals( false );
    settings->historyDockVisible = false;
}

void MainWindow::Private::deleteVariablesDock()
{
    Q_ASSERT( docks.variables );

    p->removeDockWidget( docks.variables );
    p->disconnect( docks.variables );
    docks.variables->deleteLater();
    docks.variables = 0;
    actions.viewVariables->blockSignals( true );
    actions.viewVariables->setChecked( false );
    actions.viewVariables->blockSignals( false );
    settings->variablesDockVisible = false;
}

void MainWindow::setFunctionsDockVisible( bool b )
{
    if ( b )
        d->createFunctionsDock();
    else
        d->deleteFunctionsDock();
}

void MainWindow::setMathBookDockVisible( bool b )
{
    if ( b )
        d->createBookDock();
    else
        d->deleteBookDock();
}

void MainWindow::setConstantsDockVisible( bool b )
{
    if ( b )
        d->createConstantsDock();
    else
        d->deleteConstantsDock();
}

void MainWindow::setHistoryDockVisible( bool b )
{
    if ( b )
        d->createHistoryDock();
    else
        d->deleteHistoryDock();
}

void MainWindow::setVariablesDockVisible( bool b )
{
    if ( b )
        d->createVariablesDock();
    else
        d->deleteVariablesDock();
}

void MainWindow::setKeypadVisible( bool b )
{
    d->settings->keypadVisible = b;
    if ( b )
        d->createKeypad();
    else
        d->deleteKeypad();
}

void MainWindow::showMenuBarTip()
{
    QString msg = tr( "The menu bar is now hidden. "
                      "To make it visible again, press Ctrl+Alt+M." );

    d->widgets.tip->move( 0, 0 );
    d->widgets.tip->showText( msg, tr("Warning") );
}

void MainWindow::showTipOfTheDay()
{
    d->widgets.tip->move( 0, 0 );

    int tipNo = qrand() % 4;
    QString msg;
    switch ( tipNo ) {
        case 0:
            msg = tr( "To insert a function using keyboard, use Ctrl+F shorcut. "
                      "From the dialog, you can choose the function you want to "
                      "insert." );
            break;
        case 1:
            msg = tr( "To insert a variable using keyboard, use Ctrl+I shorcut. "
                      "From the dialog, you can choose the variable you want to "
                      "insert." );
            break;
        case 2:
            msg = tr( "Use variable <i>pi</i> to use pi constant." );
            break;
        case 3:
            msg = tr( "Use <i>;</i> (semicolon) to separate the parameters in "
                      "functions." );
            break;
        default:
            break;
    }

    d->widgets.tip->showText( msg, tr("Tip of the day") );
}

void MainWindow::setResultFormatBinary()
{
    d->actionGroups.digits->setDisabled( true );
    setResultFormat( 'b' );

    if ( d->status.resultFormat )
        d->status.resultFormat->setText( tr("Binary") );
}

void MainWindow::setResultFormatEngineering()
{
    d->actionGroups.digits->setEnabled( true );
    setResultFormat( 'n' );

    if ( d->status.resultFormat )
        d->status.resultFormat->setText( tr("Engineering decimal") );
}

void MainWindow::setResultFormatFixed()
{
    d->actionGroups.digits->setEnabled( true );
    setResultFormat( 'f' );

    if ( d->status.resultFormat )
        d->status.resultFormat->setText( tr("Fixed decimal") );
}

void MainWindow::setResultFormatGeneral()
{
    d->actionGroups.digits->setEnabled( true );
    setResultFormat( 'g' );

    if ( d->status.resultFormat )
        d->status.resultFormat->setText( tr("General decimal") );
}

void MainWindow::setResultFormatHexadecimal()
{
    d->actionGroups.digits->setDisabled( true );
    setResultFormat( 'h' );

    if ( d->status.resultFormat )
        d->status.resultFormat->setText( tr("Hexadecimal") );
}

void MainWindow::setResultFormatOctal()
{
    d->actionGroups.digits->setDisabled( true );
    setResultFormat( 'o' );

    if ( d->status.resultFormat )
        d->status.resultFormat->setText( tr("Octal") );
}

void MainWindow::setResultFormatScientific()
{
    d->actionGroups.digits->setEnabled( true );
    setResultFormat( 'e' );

    if ( d->status.resultFormat )
        d->status.resultFormat->setText( tr("Scientific decimal") );
}

void MainWindow::activate()
{
    show();
    raise();
    activateWindow();
    d->widgets.editor->setFocus();
}

void MainWindow::insertConstantIntoEditor( const QString & c )
{
    if ( c.isEmpty() )
        return;

    QString s = c;
    s.replace( '.', d->settings->radixCharacter() );
    insertTextIntoEditor( s );
}

void MainWindow::insertTextIntoEditor( const QString & s )
{
    if ( s.isEmpty() )
        return;

    d->widgets.editor->blockSignals( true );
    d->widgets.editor->insert( s );
    d->widgets.editor->blockSignals( false );

    if ( ! isActiveWindow () )
        activateWindow();
    QTimer::singleShot( 0, d->widgets.editor, SLOT(setFocus()) );
}

void MainWindow::insertFunctionIntoEditor( const QString & f )
{
    if ( ! f.isEmpty() )
        insertTextIntoEditor( f + "(" );
}

void MainWindow::handleKeypadButtonPress( Keypad::Button b )
{
    d->widgets.editor->blockSignals( true ); // prevent completion
    switch ( b ) {
        case Keypad::Key0: d->widgets.editor->insert( "0" ); break;
        case Keypad::Key1: d->widgets.editor->insert( "1" ); break;
        case Keypad::Key2: d->widgets.editor->insert( "2" ); break;
        case Keypad::Key3: d->widgets.editor->insert( "3" ); break;
        case Keypad::Key4: d->widgets.editor->insert( "4" ); break;
        case Keypad::Key5: d->widgets.editor->insert( "5" ); break;
        case Keypad::Key6: d->widgets.editor->insert( "6" ); break;
        case Keypad::Key7: d->widgets.editor->insert( "7" ); break;
        case Keypad::Key8: d->widgets.editor->insert( "8" ); break;
        case Keypad::Key9: d->widgets.editor->insert( "9" ); break;

        case Keypad::KeyPlus  : d->widgets.editor->insert( "+" ); break;
        case Keypad::KeyMinus : d->widgets.editor->insert( "-" ); break;
        case Keypad::KeyTimes : d->widgets.editor->insert( "*" ); break;
        case Keypad::KeyDivide: d->widgets.editor->insert( "/" ); break;

        case Keypad::KeyEE       : d->widgets.editor->insert( "e" ); break;
        case Keypad::KeyLeftPar  : d->widgets.editor->insert( "(" ); break;
        case Keypad::KeyRightPar : d->widgets.editor->insert( ")" ); break;
        case Keypad::KeyRaise    : d->widgets.editor->insert( "^" ); break;
        case Keypad::KeyPercent  : d->widgets.editor->insert( "%" ); break;
        case Keypad::KeyFactorial: d->widgets.editor->insert( "!" ); break;

        case Keypad::KeyX      : d->widgets.editor->insert( "x"   ); break;
        case Keypad::KeyXEquals: d->widgets.editor->insert( "x="  ); break;
        case Keypad::KeyPi     : d->widgets.editor->insert( "pi"  ); break;
        case Keypad::KeyAns    : d->widgets.editor->insert( "ans" ); break;

        case Keypad::KeySqrt: d->widgets.editor->insert( "sqrt(" ); break;
        case Keypad::KeyLn  : d->widgets.editor->insert( "ln("   ); break;
        case Keypad::KeyExp : d->widgets.editor->insert( "exp("  ); break;
        case Keypad::KeySin : d->widgets.editor->insert( "sin("  ); break;
        case Keypad::KeyCos : d->widgets.editor->insert( "cos("  ); break;
        case Keypad::KeyTan : d->widgets.editor->insert( "tan("  ); break;
        case Keypad::KeyAcos: d->widgets.editor->insert( "acos(" ); break;
        case Keypad::KeyAtan: d->widgets.editor->insert( "atan(" ); break;
        case Keypad::KeyAsin: d->widgets.editor->insert( "asin(" ); break;

        case Keypad::KeyRadixChar:
            d->widgets.editor->insert( QString(d->settings->radixCharacter()) );
            break;

        case Keypad::KeyClear: clearEditor(); break;

        default: break;
  }

    QTimer::singleShot( 0, d->widgets.editor, SLOT(setFocus()) );
    d->widgets.editor->blockSignals( false );

    if ( b == Keypad::KeyEquals )
        d->widgets.editor->evaluate();
}

void MainWindow::minimizeToSystemTray()
{
    if ( d->widgets.trayIcon ) {
        hide();
        d->widgets.trayIcon->show();
        if ( d->conditions.trayNotify )
            QTimer::singleShot( 500, this, SLOT(showSystemTrayMessage()) );
        d->conditions.trayNotify = false;
    }
}

void MainWindow::raiseWindow()
{
    activate();

#ifdef Q_OS_WIN
    SetWindowPos( winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
    SetWindowPos( winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
#endif // Q_OS_WIN

#ifdef Q_WS_X11
    static Atom NET_ACTIVE_WINDOW = XInternAtom( QX11Info::display(), "_NET_ACTIVE_WINDOW",
                                                 False );

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

    XSendEvent( QX11Info::display(), QX11Info::appRootWindow(), False,
                (SubstructureNotifyMask | SubstructureRedirectMask), (XEvent *)&xev );
#endif // Q_WS_X11
}

void MainWindow::Private::restoreVariables()
{
    for ( int k = 0; k < settings->variables.count(); k++ ) {
        evaluator->setExpression( settings->variables.at(k) );
        evaluator->eval();
        QStringList list = settings->variables.at(k).split( "=" );
        evaluator->set( list.at(0), HNumber( list.at(1).toAscii().data() ) );
    }

    if ( docks.variables )
        docks.variables->updateList();

    // free memory
    settings->variables.clear();
}

void MainWindow::Private::restoreHistory()
{
    if ( settings->historyResults.count() != settings->history.count() ) {
        p->clearHistory();
        return;
    }

    widgets.editor->setHistory( settings->history );
    widgets.editor->setHistoryResults( settings->historyResults );
    widgets.display->appendHistory( settings->history, settings->historyResults );

    if ( docks.history ) {
        HistoryWidget * history = qobject_cast<HistoryWidget *>( docks.history->widget() );
        history->setHistory( widgets.editor->history() );
    }

    // free some useless memory
    settings->history.clear();
    settings->historyResults.clear();
}

void MainWindow::evaluateEditorExpression()
{
    QString str = d->evaluator->autoFix( d->widgets.editor->text() );
    if ( str.isEmpty() )
        return;

    d->evaluator->setExpression( str );

    HNumber result = d->evaluator->evalUpdateAns();
    if ( ! d->evaluator->error().isEmpty() )
        showAutoCalcTip( d->evaluator->error() );
    else {
        d->widgets.display->append( str, result );
        char * num = HMath::format( result, 'e', DECPRECISION );
        d->widgets.editor->appendHistory( str, num );
        free( num );
        d->widgets.editor->setAnsAvailable( true );
        if ( d->settings->variablesDockVisible )
            d->docks.variables->updateList();
        if ( d->settings->historyDockVisible ) {
            HistoryWidget * history = qobject_cast<HistoryWidget *>( d->docks.history->widget() );
            history->append( str );
        }

        d->widgets.editor->selectAll();
        d->widgets.editor->stopAutoCalc();
        d->widgets.editor->stopAutoComplete();
        d->conditions.autoAns = true;
    }
}

void MainWindow::showSystemTrayMessage()
{
    QString msg = tr( "SpeedCrunch is minimized.\nLeft click the icon to "
                      "restore it or right click for options." );
    if ( menuBar()->layoutDirection() == Qt::RightToLeft )
        msg += QChar( 0x200E );
    if ( d->widgets.trayIcon )
        d->widgets.trayIcon->showMessage( QString(), msg, QSystemTrayIcon::NoIcon, 4000 );
}

void MainWindow::handleEditorTextChange()
{
    if ( d->conditions.autoAns ) {
        QString expr = d->evaluator->autoFix( d->widgets.editor->text() );
        if ( expr.isEmpty() )
            return;

        Tokens tokens = d->evaluator->scan( expr );
        if ( tokens.count() == 1 ) {
            if ( tokens.at(0).asOperator() == Token::Plus
                 || tokens.at(0).asOperator() == Token::Minus
                 || tokens.at(0).asOperator() == Token::Asterisk
                 || tokens.at(0).asOperator() == Token::Slash
                 || tokens.at(0).asOperator() == Token::Caret )
            {
                d->conditions.autoAns = false;
                expr.prepend( "ans" );
                d->widgets.editor->setText( expr );
                d->widgets.editor->setCursorPosition( expr.length() );
            }
        }
    }
}

void MainWindow::handleSystemTrayIconActivation( QSystemTrayIcon::ActivationReason r )
{
    if ( r == QSystemTrayIcon::Context )
        d->menus.trayIcon->show();
    else {
        showNormal();
        activateWindow();
        d->widgets.editor->setFocus();
        QTimer::singleShot( 0, d->widgets.trayIcon, SLOT(hide()) );

        // work around docks do not reappear if floating
        if ( d->docks.history )
            if ( d->docks.history->isFloating() ) {
                d->docks.history->hide();
                d->docks.history->show();
            }
        if ( d->docks.functions )
            if ( d->docks.functions->isFloating() ) {
                d->docks.functions->hide();
                d->docks.functions->show();
            }
        if ( d->docks.variables )
            if ( d->docks.variables->isFloating() ) {
                d->docks.variables->hide();
                d->docks.variables->show();
            }
        if ( d->docks.constants )
            if ( d->docks.constants->isFloating() ) {
                d->docks.constants->hide();
                d->docks.constants->show();
            }
    }
}

void MainWindow::insertVariableIntoEditor( const QString & v )
{
    insertTextIntoEditor( v );
}

void MainWindow::setRadixCharacterAutomatic()
{
    setRadixCharacter( 0 );
}

void MainWindow::setRadixCharacterDot()
{
    setRadixCharacter( '.' );
}

void MainWindow::setRadixCharacterComma()
{
    setRadixCharacter( ',' );
}

void MainWindow::closeEvent( QCloseEvent * e )
{
    d->saveSettings();
    e->accept();
}

void MainWindow::setResultPrecision( int p )
{
    if ( d->settings->resultPrecision == p )
        return;

    d->settings->resultPrecision = p;
    emit resultPrecisionChanged();
}

void MainWindow::setResultFormat( char c )
{
    if ( d->settings->resultFormat == c )
        return;

    d->settings->resultFormat = c;
    emit resultFormatChanged();
}

void MainWindow::setRadixCharacter( char c )
{
    d->settings->setRadixCharacter( c );
    emit radixCharacterChanged();
}

void MainWindow::showLanguageChooserDialog()
{
    QMap<QString, QString> map;

    map.insert( QLatin1String("American English"), QLatin1String("en_US") );
    map.insert( QLatin1String("Bahasa Indonesia"), QLatin1String("id_ID") );
    map.insert( QLatin1String("British English"), QLatin1String("en_GB") );
    map.insert( QString::fromUtf8("Català"), QLatin1String("ca_ES") );
    map.insert( QString::fromUtf8("Česky"), QLatin1String("cs_CZ") );
    map.insert( QLatin1String("Deutsch"), QLatin1String("de_DE") );
    map.insert( QString::fromUtf8("Español"), QLatin1String("es_ES") );
    map.insert( QString::fromUtf8("Español Argentino"), QLatin1String("es_AR") );
    map.insert( QLatin1String("Euskara"), QLatin1String("eu_ES") );
    map.insert( QString::fromUtf8("Français"), QLatin1String("fr_FR") );
    map.insert( QLatin1String("Italiano"), QLatin1String("it_IT") );
    map.insert( QLatin1String("Nederlands"), QLatin1String("nl_NL") );
    map.insert( QString::fromUtf8("Norsk (Bokmål)") + QChar(0x200E), QLatin1String("nb_NO") );
    map.insert( QLatin1String("Polski"), QLatin1String("pl_PL") );
    map.insert( QString::fromUtf8("Português do Brasil"), QLatin1String("pt_BR") );
    map.insert( QString::fromUtf8("Português Europeu"), QLatin1String("pt_PT") );
    map.insert( QString::fromUtf8("Română"), QLatin1String("ro_RO") );
    map.insert( QLatin1String("Suomi"), QLatin1String("fi_FI") );
    map.insert( QLatin1String("Svenska"), QLatin1String("sv_SE") );
    map.insert( QString::fromUtf8("Türkçe"), QLatin1String("tr_TR") );
    map.insert( QString::fromUtf8("العربية"), QLatin1String("ar_JO") );
    map.insert( QString::fromUtf8("עברית"), QLatin1String("he_IL") );
    map.insert( QString::fromUtf8("Русский"), QLatin1String("ru_RU") );
    map.insert( QString::fromUtf8("简化字"), QLatin1String("zh_CN") );

    int current = map.values().indexOf( d->settings->language ) + 1;

    QString defaultKey = tr( "System Default" );
    QStringList keys( QStringList() << defaultKey << map.keys() );

    bool ok;
    QString langName = QInputDialog::getItem( this, tr("Language"), tr("Select the language:"),
                                              keys, current, false, &ok );
    if ( ok && ! langName.isEmpty() ) {
        QString value = (langName == defaultKey) ? QLatin1String( "C" ) : map.value( langName );
        if ( d->settings->language != value ) {
            d->settings->language = value;
            emit languageChanged();
        }
    }
}

