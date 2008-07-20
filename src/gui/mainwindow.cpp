// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2005-2006 Johan Thelin <e8johan@gmail.com>
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
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


#include "mainwindow.hxx"

#include "aboutbox.hxx"
#include "autohidelabel.hxx"
#include "bookdock.hxx"
#include "constantsdock.hxx"
#include "deletevardlg.hxx"
#include "editor.hxx"
#include "functionsdock.hxx"
#include "historydock.hxx"
#include "insertfunctiondlg.hxx"
#include "insertvardlg.hxx"
#include "keypad.hxx"
#include "resultdisplay.hxx"
#include "tipwidget.hxx"
#include "variablesdock.hxx"

#include "3rdparty/util/binreloc.h"
#include "base/constants.hxx"
#include "base/evaluator.hxx"
#include "base/functions.hxx"
#include "base/settings.hxx"
#include "math/hmath.hxx"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLocale>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QShortcut>
#include <QStatusBar>
#include <QSystemTrayIcon>
#include <QTextStream>
#include <QTimer>
#include <QToolTip>
#include <QTranslator>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>


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
  QAction * languageDefault;
  QAction * languageCa;
  QAction * languageCs;
  QAction * languageDe;
  QAction * languageEn;
  QAction * languageEs;
  QAction * languageEsAr;
  QAction * languageEu;
  QAction * languageFi;
  QAction * languageFr;
  QAction * languageHe;
  QAction * languageId;
  QAction * languageIt;
  QAction * languageNb;
  QAction * languageNl;
  QAction * languagePl;
  QAction * languagePt;
  QAction * languagePtBr;
  QAction * languageRo;
  QAction * languageRu;
  QAction * languageSv;
  QAction * languageTr;
  QAction * languageZhCn;

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
  QActionGroup * language;
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
  QMenu * language;
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
  QWidget *         root;
  ResultDisplay *   display;
  Editor *          editor;
  Keypad *          keypad;
  AutoHideLabel *   autoCalcLabel;
  TipWidget *       tip;
  QSystemTrayIcon * trayIcon;
};


struct Docks
{
  BookDock *      book;
  ConstantsDock * constants;
  FunctionsDock * functions;
  HistoryDock *   history;
  VariablesDock * variables;
};


struct Conditions
{
  bool notifyMenuBarHidden;
  bool trayNotify;
  bool autoAns;
};


struct Status
{
  QLabel * angleUnit;
  QLabel * resultFormat;
};


struct MainWindow::Private
{
  MainWindow *    p;
  QTranslator *   translator;
  Constants *     constants;
  Evaluator *     evaluator;
  Functions *     functions;
  Settings        settings;
  Actions         actions;
  ActionGroups    actionGroups;
  Menus           menus;
  Layouts         layouts;
  Widgets         widgets;
  Docks           docks;
  Conditions      conditions;
  Status          status;

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
  void checkInitialPrecision();
  void checkInitialLanguage();
  // THIS IS NOT NEEDED ANYMORE, BUT NEEDS TO BE TESTED WITH Qt 4.2
  //void restoreFloatingDocks();
  void restoreHistory();
  void restoreVariables();
  void deleteKeypad();
  void deleteStatusBar();
  void deleteBookDock();
  void deleteConstantsDock();
  void deleteFunctionsDock();
  void deleteHistoryDock();
  void deleteVariablesDock();
  // THIS IS NOT NEEDED ANYMORE, BUT NEEDS TO BE TESTED WITH Qt 4.2
  //void syncWindowStateToSettings();
  void saveSettings();
  void setActionsText();
  void setMenusText();
  void setStatusBarText();

  static QTranslator * createTranslator( const QString & langCode );
};


MainWindow::Private::Private()
{
  translator = 0;

  widgets.keypad   = 0;
  widgets.trayIcon = 0;

  menus.trayIcon = 0;

  conditions.autoAns             = false;
  conditions.trayNotify          = true;
  conditions.notifyMenuBarHidden = true;

  docks.book      = 0;
  docks.history   = 0;
  docks.constants = 0;
  docks.functions = 0;
  docks.variables = 0;

  status.angleUnit    = 0;
  status.resultFormat = 0;

  Settings::settings = &settings;
};


MainWindow::Private::~Private()
{
  if ( widgets.trayIcon ) widgets.trayIcon->hide();
  if ( docks.book       ) deleteBookDock();
  if ( docks.constants  ) deleteConstantsDock();
  if ( docks.variables  ) deleteVariablesDock();
  if ( docks.functions  ) deleteFunctionsDock();
  if ( docks.history    ) deleteHistoryDock();
  Settings::settings = 0;
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
  if ( br_init( & error ) == 0 && error != BR_INIT_ERROR_DISABLED )
  {
      qDebug( "Warning: BinReloc failed to initialize (error code %d)", error );
      qDebug( "Will fallback to hardcoded default path." );
  }

  char * dataDir = br_find_data_dir( 0 );
  QString localeDir = QString( dataDir ) + "/speedcrunch/locale";
  free( dataDir );
  if ( translator->load( localeFile, localeDir ) )
    return translator;
  else
    return 0;
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
  p->setWindowIcon( QPixmap( ":/speedcrunch.png" ) );
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

  actions.languageDefault = new QAction( p );
  actions.languageCa   = new QAction( QString::fromUtf8( "Català"              ), p );
  actions.languageCs   = new QAction( QString::fromUtf8( "Česky"               ), p );
  actions.languageDe   = new QAction( QString::fromUtf8( "Deutsch"             ), p );
  actions.languageEn   = new QAction( QString::fromUtf8( "English"             ), p );
  actions.languageEs   = new QAction( QString::fromUtf8( "Español"             ), p );
  actions.languageEsAr = new QAction( QString::fromUtf8( "Español Argentino"   ), p );
  actions.languageEu   = new QAction( QString::fromUtf8( "Euskara"             ), p );
  actions.languageFi   = new QAction( QString::fromUtf8( "Suomi"               ), p );
  actions.languageFr   = new QAction( QString::fromUtf8( "Français"            ), p );
  actions.languageHe   = new QAction( QString::fromUtf8( "עברית"               ), p );
  actions.languageId   = new QAction( QString::fromUtf8( "Bahasa Indonesia"    ), p );
  actions.languageIt   = new QAction( QString::fromUtf8( "Italiano"            ), p );
  actions.languageNb   = new QAction( QString::fromUtf8( "Norsk (Bokmål)"      ) + QChar( 0x200E ), p );
  actions.languageNl   = new QAction( QString::fromUtf8( "Nederlands"          ), p );
  actions.languagePl   = new QAction( QString::fromUtf8( "Polski"              ), p );
  actions.languagePt   = new QAction( QString::fromUtf8( "Português"           ), p );
  actions.languagePtBr = new QAction( QString::fromUtf8( "Português do Brasil" ), p );
  actions.languageRo   = new QAction( QString::fromUtf8( "Română"              ), p );
  actions.languageRu   = new QAction( QString::fromUtf8( "Русский"             ), p );
  actions.languageSv   = new QAction( QString::fromUtf8( "Svenska"             ), p );
  actions.languageTr   = new QAction( QString::fromUtf8( "Türkçe"              ), p );
  actions.languageZhCn = new QAction( QString::fromUtf8( "简化字"              ), p );

  actions.languageDefault->setData( QString( "C"     ) );
  actions.languageCa     ->setData( QString( "ca"    ) );
  actions.languageCs     ->setData( QString( "cs"    ) );
  actions.languageDe     ->setData( QString( "de"    ) );
  actions.languageEn     ->setData( QString( "en"    ) );
  actions.languageEs     ->setData( QString( "es"    ) );
  actions.languageEsAr   ->setData( QString( "es_AR" ) );
  actions.languageEu     ->setData( QString( "eu"    ) );
  actions.languageFi     ->setData( QString( "fi"    ) );
  actions.languageFr     ->setData( QString( "fr"    ) );
  actions.languageHe     ->setData( QString( "he"    ) );
  actions.languageId     ->setData( QString( "id"    ) );
  actions.languageIt     ->setData( QString( "it"    ) );
  actions.languageNb     ->setData( QString( "nb"    ) );
  actions.languageNl     ->setData( QString( "nl"    ) );
  actions.languagePl     ->setData( QString( "pl"    ) );
  actions.languagePt     ->setData( QString( "pt"    ) );
  actions.languagePtBr   ->setData( QString( "pt_BR" ) );
  actions.languageRo     ->setData( QString( "ro"    ) );
  actions.languageRu     ->setData( QString( "ru"    ) );
  actions.languageSv     ->setData( QString( "sv"    ) );
  actions.languageTr     ->setData( QString( "tr"    ) );
  actions.languageZhCn   ->setData( QString( "zh_CN" ) );

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

  actions.languageDefault->setCheckable( true );
  actions.languageCa     ->setCheckable( true );
  actions.languageCs     ->setCheckable( true );
  actions.languageDe     ->setCheckable( true );
  actions.languageEn     ->setCheckable( true );
  actions.languageEs     ->setCheckable( true );
  actions.languageEsAr   ->setCheckable( true );
  actions.languageEu     ->setCheckable( true );
  actions.languageFi     ->setCheckable( true );
  actions.languageFr     ->setCheckable( true );
  actions.languageHe     ->setCheckable( true );
  actions.languageId     ->setCheckable( true );
  actions.languageIt     ->setCheckable( true );
  actions.languageNb     ->setCheckable( true );
  actions.languageNl     ->setCheckable( true );
  actions.languagePl     ->setCheckable( true );
  actions.languagePt     ->setCheckable( true );
  actions.languagePtBr   ->setCheckable( true );
  actions.languageRo     ->setCheckable( true );
  actions.languageRu     ->setCheckable( true );
  actions.languageSv     ->setCheckable( true );
  actions.languageTr     ->setCheckable( true );
  actions.languageZhCn   ->setCheckable( true );
}


void MainWindow::setAllText()
{
  QTranslator * tr = 0;
  tr = d->createTranslator( d->settings.language );
  if ( tr )
  {
    if ( d->translator )
    {
      qApp->removeTranslator( d->translator );
      delete d->translator;
    }

    qApp->installTranslator( tr );
    d->translator = tr;
  }
  else
  {
    qApp->removeTranslator( d->translator );
    d->translator = 0;
  }

  d->setMenusText();
  d->setActionsText();
  d->setStatusBarText();

  d->functions->retranslateText(); // can't be signal/slot to ensure order
  d->constants->retranslateText(); // same as above

  if ( d->docks.book )
  {
    d->docks.book->setLanguage( d->settings.language );
    d->docks.book->setWindowTitle( MainWindow::tr( "Math Book" ) );
  }

  // layout direction
  setWidgetsDirection();
}


void MainWindow::Private::setStatusBarText()
{
  if ( status.angleUnit )
  {
    QString angleUnit = (settings.angleMode == 'r') ?
      MainWindow::tr( "Radian" ) : MainWindow::tr( "Degree" );

    QString format;
    switch ( settings.resultFormat )
    {
      case 'b': format = MainWindow::tr( "Binary"              ); break;
      case 'o': format = MainWindow::tr( "Octal"               ); break;
      case 'h': format = MainWindow::tr( "Hexadecimal"         ); break;
      case 'f': format = MainWindow::tr( "Fixed decimal"       ); break;
      case 'n': format = MainWindow::tr( "Engineering decimal" ); break;
      case 'e': format = MainWindow::tr( "Scientific decimal"  ); break;
      case 'g': format = MainWindow::tr( "General decimal"     ); break;
      default : break;
    }

    status.angleUnit   ->setText( angleUnit );
    status.resultFormat->setText( format    );

    status.angleUnit   ->setToolTip( MainWindow::tr( "Angle unit"    ) );
    status.resultFormat->setToolTip( MainWindow::tr( "Result format" ) );
  }
}


void MainWindow::Private::setActionsText()
{
  actions.editClearExpression                ->setText( MainWindow::tr( "Clear E&xpression"        ) );
  actions.editClearHistory                   ->setText( MainWindow::tr( "Clear &History"           ) );
  actions.editCopyLastResult                 ->setText( MainWindow::tr( "Copy Last &Result"        ) );
  actions.editCopy                           ->setText( MainWindow::tr( "&Copy"                    ) );
  actions.editDeleteAllVariables             ->setText( MainWindow::tr( "Delete All V&ariables"    ) );
  actions.editDeleteVariable                 ->setText( MainWindow::tr( "D&elete Variable..."      ) );
  actions.editInsertFunction                 ->setText( MainWindow::tr( "Insert &Function..."      ) );
  actions.editInsertVariable                 ->setText( MainWindow::tr( "Insert &Variable..."      ) );
  actions.editPaste                          ->setText( MainWindow::tr( "&Paste"                   ) );
  actions.editSelectExpression               ->setText( MainWindow::tr( "&Select Expression"       ) );
  actions.helpAboutQt                        ->setText( MainWindow::tr( "About &Qt"                ) );
  actions.helpAbout                          ->setText( MainWindow::tr( "&About"                   ) );
  actions.helpTipOfTheDay                    ->setText( MainWindow::tr( "&Tip of the Day"          ) );
  actions.helpWebsite                        ->setText( MainWindow::tr( "SpeedCrunch &Web Site..." ) );
  actions.sessionExport                      ->setText( MainWindow::tr( "&Export..."               ) );
  actions.sessionImport                      ->setText( MainWindow::tr( "&Import..."               ) );
  actions.sessionLoad                        ->setText( MainWindow::tr( "&Load..."                 ) );
  actions.sessionQuit                        ->setText( MainWindow::tr( "&Quit"                    ) );
  actions.sessionSave                        ->setText( MainWindow::tr( "&Save..."                 ) );
  actions.settingsAngleUnitDegree            ->setText( MainWindow::tr( "&Degree"                  ) );
  actions.settingsAngleUnitRadian            ->setText( MainWindow::tr( "&Radian"                  ) );
  actions.settingsBehaviorAlwaysOnTop        ->setText( MainWindow::tr( "Always On &Top"           ) );
  actions.settingsBehaviorAutoCompletion     ->setText( MainWindow::tr( "Automatic &Completion"    ) );
  actions.settingsBehaviorMinimizeToTray     ->setText( MainWindow::tr( "&Minimize To System Tray" ) );
  actions.settingsBehaviorPartialResults     ->setText( MainWindow::tr( "&Partial Results"         ) );
  actions.settingsBehaviorSaveHistoryOnExit  ->setText( MainWindow::tr( "Save &History on Exit"    ) );
  actions.settingsBehaviorSaveVariablesOnExit->setText( MainWindow::tr( "Save &Variables on Exit"  ) );
  actions.settingsBehaviorSyntaxHilite       ->setText( MainWindow::tr( "Syntax &Highlighting"     ) );
  actions.settingsRadixCharComma             ->setText( MainWindow::tr( "&Comma"                   ) );
  actions.settingsRadixCharDefault           ->setText( MainWindow::tr( "&System Default"          ) );
  actions.settingsRadixCharDot               ->setText( MainWindow::tr( "&Dot"                     ) );
  actions.settingsResultFormat15Digits       ->setText( MainWindow::tr( "&15 Decimal Digits"       ) );
  actions.settingsResultFormat2Digits        ->setText( MainWindow::tr( "&2 Decimal Digits"        ) );
  actions.settingsResultFormat3Digits        ->setText( MainWindow::tr( "&3 Decimal Digits"        ) );
  actions.settingsResultFormat50Digits       ->setText( MainWindow::tr( "&50 Decimal Digits"       ) );
  actions.settingsResultFormat8Digits        ->setText( MainWindow::tr( "&8 Decimal Digits"        ) );
  actions.settingsResultFormatAutoPrecision  ->setText( MainWindow::tr( "&Automatic Precision"     ) );
  actions.settingsResultFormatBinary         ->setText( MainWindow::tr( "&Binary"                  ) );
  actions.settingsResultFormatEngineering    ->setText( MainWindow::tr( "&Engineering"             ) );
  actions.settingsResultFormatFixed          ->setText( MainWindow::tr( "&Fixed Decimal"           ) );
  actions.settingsResultFormatGeneral        ->setText( MainWindow::tr( "&General"                 ) );
  actions.settingsResultFormatHexadecimal    ->setText( MainWindow::tr( "&Hexadecimal"             ) );
  actions.settingsResultFormatOctal          ->setText( MainWindow::tr( "&Octal"                   ) );
  actions.settingsResultFormatScientific     ->setText( MainWindow::tr( "&Scientific"              ) );
  actions.viewConstants                      ->setText( MainWindow::tr( "&Constants"               ) );
  actions.viewFullScreenMode                 ->setText( MainWindow::tr( "F&ull Screen Mode"        ) );
  actions.viewFunctions                      ->setText( MainWindow::tr( "&Functions"               ) );
  actions.viewHistory                        ->setText( MainWindow::tr( "&History"                 ) );
  actions.viewKeypad                         ->setText( MainWindow::tr( "&Keypad"                  ) );
  actions.viewMathBook                       ->setText( MainWindow::tr( "Math &Book"               ) );
  actions.viewMenuBar                        ->setText( MainWindow::tr( "&Menu Bar"                ) );
  actions.viewStatusBar                      ->setText( MainWindow::tr( "&Status Bar"              ) );
  actions.viewVariables                      ->setText( MainWindow::tr( "&Variables"               ) );

  actions.scrollDown                         ->setText( MainWindow::tr( "Scroll Display Down"      ) );
  actions.scrollUp                           ->setText( MainWindow::tr( "Scroll Display Up"        ) );

  actions.languageDefault                    ->setText( MainWindow::tr( "System &Default"          ) );
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

  actionGroups.radixChar = new QActionGroup( p );
  actionGroups.radixChar->addAction( actions.settingsRadixCharDefault  );
  actionGroups.radixChar->addAction( actions.settingsRadixCharDot   );
  actionGroups.radixChar->addAction( actions.settingsRadixCharComma );

  actionGroups.language = new QActionGroup( p );
  actionGroups.language->addAction( actions.languageDefault );
  actionGroups.language->addAction( actions.languageCa );
  actionGroups.language->addAction( actions.languageCs );
  actionGroups.language->addAction( actions.languageDe );
  actionGroups.language->addAction( actions.languageEn );
  actionGroups.language->addAction( actions.languageEs );
  actionGroups.language->addAction( actions.languageEsAr );
  actionGroups.language->addAction( actions.languageEu );
  actionGroups.language->addAction( actions.languageFi );
  actionGroups.language->addAction( actions.languageFr );
  actionGroups.language->addAction( actions.languageHe );
  actionGroups.language->addAction( actions.languageId );
  actionGroups.language->addAction( actions.languageIt );
  actionGroups.language->addAction( actions.languageNb );
  actionGroups.language->addAction( actions.languageNl );
  actionGroups.language->addAction( actions.languagePl );
  actionGroups.language->addAction( actions.languagePt );
  actionGroups.language->addAction( actions.languagePtBr );
  actionGroups.language->addAction( actions.languageRo );
  actionGroups.language->addAction( actions.languageRu );
  actionGroups.language->addAction( actions.languageSv );
  actionGroups.language->addAction( actions.languageTr );
  actionGroups.language->addAction( actions.languageZhCn );
}


void MainWindow::Private::createActionShortcuts()
{
  actions.editClearExpression            ->setShortcut( Qt::Key_Escape                 );
  actions.editClearHistory               ->setShortcut( Qt::CTRL + Qt::ALT + Qt::Key_N );
  actions.editCopyLastResult             ->setShortcut( Qt::CTRL + Qt::Key_R           );
  actions.editCopy                       ->setShortcut( Qt::CTRL + Qt::Key_C           );
  actions.editDeleteVariable             ->setShortcut( Qt::CTRL + Qt::Key_D           );
  actions.editInsertFunction             ->setShortcut( Qt::CTRL + Qt::Key_F           );
  actions.editInsertVariable             ->setShortcut( Qt::CTRL + Qt::Key_I           );
  actions.editPaste                      ->setShortcut( Qt::CTRL + Qt::Key_V           );
  actions.editSelectExpression           ->setShortcut( Qt::CTRL + Qt::Key_A           );
  actions.helpTipOfTheDay                ->setShortcut( Qt::CTRL + Qt::Key_T           );
  actions.scrollDown                     ->setShortcut( Qt::SHIFT + Qt::Key_PageDown   );
  actions.scrollUp                       ->setShortcut( Qt::SHIFT + Qt::Key_PageUp     );
  actions.sessionLoad                    ->setShortcut( Qt::CTRL + Qt::Key_L           );
  actions.sessionQuit                    ->setShortcut( Qt::CTRL + Qt::Key_Q           );
  actions.sessionSave                    ->setShortcut( Qt::CTRL + Qt::Key_S           );
  actions.settingsAngleUnitDegree        ->setShortcut( Qt::Key_F10                    );
  actions.settingsAngleUnitRadian        ->setShortcut( Qt::Key_F9                     );
  actions.settingsResultFormatBinary     ->setShortcut( Qt::Key_F5                     );
  actions.settingsResultFormatGeneral    ->setShortcut( Qt::Key_F7                     );
  actions.settingsResultFormatHexadecimal->setShortcut( Qt::Key_F8                     );
  actions.settingsResultFormatOctal      ->setShortcut( Qt::Key_F6                     );
  actions.viewConstants                  ->setShortcut( Qt::CTRL + Qt::Key_2           );
  actions.viewFullScreenMode             ->setShortcut( Qt::Key_F11                    );
  actions.viewFunctions                  ->setShortcut( Qt::CTRL + Qt::Key_3           );
  actions.viewHistory                    ->setShortcut( Qt::CTRL + Qt::Key_5           );
  actions.viewKeypad                     ->setShortcut( Qt::CTRL + Qt::ALT + Qt::Key_K );
  actions.viewMathBook                   ->setShortcut( Qt::CTRL + Qt::Key_1           );
#ifndef Q_OS_MAC
  actions.viewMenuBar                    ->setShortcut( Qt::CTRL + Qt::ALT + Qt::Key_M );
#endif
  actions.viewStatusBar                  ->setShortcut( Qt::CTRL + Qt::ALT + Qt::Key_B );
  actions.viewVariables                  ->setShortcut( Qt::CTRL + Qt::Key_4           );
}


void MainWindow::Private::createMenus()
{
  // session
  menus.session = new QMenu( "", p );
  p->menuBar()->addMenu( menus.session );
  menus.session->addAction( actions.sessionLoad );
  menus.session->addAction( actions.sessionSave );
  menus.session->addSeparator();
  menus.session->addAction( actions.sessionImport );
  menus.session->addAction( actions.sessionExport );
  menus.session->addSeparator();
  menus.session->addAction( actions.sessionQuit );

  // edit
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

  // view
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

  // settings
  menus.settings = new QMenu( "", p );
  p->menuBar()->addMenu( menus.settings );

  // settings / result format
  menus.resultFormat = menus.settings->addMenu( "" );
  menus.resultFormat->addAction( actions.settingsResultFormatBinary );
  menus.resultFormat->addAction( actions.settingsResultFormatOctal );

  // settings / result format / decimal
  menus.decimal = menus.resultFormat->addMenu( "" );
  menus.decimal->addAction( actions.settingsResultFormatGeneral );
  menus.decimal->addAction( actions.settingsResultFormatFixed );
  menus.decimal->addAction( actions.settingsResultFormatEngineering );
  menus.decimal->addAction( actions.settingsResultFormatScientific );
  menus.decimal->addSeparator();
  menus.decimal->addAction( actions.settingsResultFormatAutoPrecision );
  menus.decimal->addAction( actions.settingsResultFormat2Digits );
  menus.decimal->addAction( actions.settingsResultFormat3Digits );
  menus.decimal->addAction( actions.settingsResultFormat8Digits );
  menus.decimal->addAction( actions.settingsResultFormat15Digits );
  menus.decimal->addAction( actions.settingsResultFormat50Digits );

  // settings / result format (continued)
  menus.resultFormat->addAction( actions.settingsResultFormatHexadecimal );

  // settings / angle mode
  menus.angleUnit = menus.settings->addMenu( "" );
  menus.angleUnit->addAction( actions.settingsAngleUnitRadian );
  menus.angleUnit->addAction( actions.settingsAngleUnitDegree );

  // settings / behavior
  menus.behavior = menus.settings->addMenu( "" );
  menus.behavior->addAction( actions.settingsBehaviorSaveHistoryOnExit   );
  menus.behavior->addAction( actions.settingsBehaviorSaveVariablesOnExit );
  menus.behavior->addSeparator();
  menus.behavior->addAction( actions.settingsBehaviorPartialResults            );
  menus.behavior->addAction( actions.settingsBehaviorAutoCompletion      );
  menus.behavior->addAction( actions.settingsBehaviorSyntaxHilite        );
  menus.behavior->addSeparator();
  menus.behavior->addAction( actions.settingsBehaviorAlwaysOnTop         );
  menus.behavior->addAction( actions.settingsBehaviorMinimizeToTray      );

  // settings / radix character
  menus.radixChar = menus.settings->addMenu( "" );
  menus.radixChar->addAction( actions.settingsRadixCharDefault  );
  menus.radixChar->addSeparator();
  menus.radixChar->addAction( actions.settingsRadixCharDot   );
  menus.radixChar->addAction( actions.settingsRadixCharComma );

  // settings / language
  menus.language = menus.settings->addMenu( "" );
  menus.language->addAction( actions.languageDefault );
  menus.language->addSeparator();
  menus.language->addAction( actions.languageCa   );
  menus.language->addAction( actions.languageCs   );
  menus.language->addAction( actions.languageDe   );
  menus.language->addAction( actions.languageEn   );
  menus.language->addAction( actions.languageEs   );
  menus.language->addAction( actions.languageEsAr );
  menus.language->addAction( actions.languageEu   );
  menus.language->addAction( actions.languageFi   );
  menus.language->addAction( actions.languageFr   );
  menus.language->addAction( actions.languageHe   );
  menus.language->addAction( actions.languageId   );
  menus.language->addAction( actions.languageIt   );
  menus.language->addAction( actions.languageNb   );
  menus.language->addAction( actions.languageNl   );
  menus.language->addAction( actions.languagePl   );
  menus.language->addAction( actions.languagePt   );
  menus.language->addAction( actions.languagePtBr );
  menus.language->addAction( actions.languageRo   );
  menus.language->addAction( actions.languageRu   );
  menus.language->addAction( actions.languageSv   );
  menus.language->addAction( actions.languageTr   );
  menus.language->addAction( actions.languageZhCn );

  // help
  menus.help = new QMenu( "", p );
  p->menuBar()->addMenu( menus.help );
  menus.help->addAction( actions.helpTipOfTheDay );
  menus.help->addAction( actions.helpWebsite );
  menus.help->addSeparator();
  menus.help->addAction( actions.helpAbout );
  menus.help->addAction( actions.helpAboutQt );

  // necessary after hiding the menu bar, so shortcuts still recognized
  p->addActions( p->menuBar()->actions() );
  p->addAction( actions.scrollDown );
  p->addAction( actions.scrollUp );
}


void MainWindow::Private::setMenusText()
{
  menus.session     ->setTitle( MainWindow::tr( "&Session"         ) );
  menus.edit        ->setTitle( MainWindow::tr( "&Edit"            ) );
  menus.view        ->setTitle( MainWindow::tr( "&View"            ) );
  menus.settings    ->setTitle( MainWindow::tr( "Se&ttings"        ) );
  menus.resultFormat->setTitle( MainWindow::tr( "Result &Format"   ) );
  menus.decimal     ->setTitle( MainWindow::tr( "&Decimal"         ) );
  menus.angleUnit   ->setTitle( MainWindow::tr( "&Angle Unit"      ) );
  menus.behavior    ->setTitle( MainWindow::tr( "&Behavior"        ) );
  menus.radixChar   ->setTitle( MainWindow::tr( "Radix &Character" ) );
  menus.language    ->setTitle( MainWindow::tr( "&Language"        ) );
  menus.help        ->setTitle( MainWindow::tr( "&Help"            ) );
}


void MainWindow::Private::createStatusBar()
{
  QStatusBar * bar = p->statusBar();

  status.angleUnit    = new QLabel( bar );
  status.resultFormat = new QLabel( bar );

  status.angleUnit   ->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
  status.resultFormat->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );

  bar->addWidget( status.angleUnit    );
  bar->addWidget( status.resultFormat );

  setStatusBarText();
}


void MainWindow::Private::createFixedWidgets()
{
  // necessary objects
  constants = new Constants( p );
  functions = new Functions( settings.angleMode, p );
  evaluator = new Evaluator( functions, settings.getRadixChar(), p );

  // outer widget and layout
  widgets.root = new QWidget( p );
  p->setCentralWidget( widgets.root );

  layouts.root = new QVBoxLayout( widgets.root );
  layouts.root->setMargin( 0 );
  layouts.root->setSpacing( 0 );

  // display
  QHBoxLayout * displayLayout = new QHBoxLayout();
  displayLayout->setMargin( 5 );
  widgets.display = new ResultDisplay( settings.getRadixChar(), settings.resultFormat,
                                       settings.resultPrecision, widgets.root );
  displayLayout->addWidget( widgets.display );
  layouts.root->addLayout( displayLayout );

  // editor
  QHBoxLayout * editorLayout = new QHBoxLayout();
  editorLayout->setMargin( 5 );
  widgets.editor = new Editor( evaluator, functions, constants, settings.resultFormat,
                               settings.resultPrecision, settings.getRadixChar(),
                               widgets.root );
  widgets.editor->setFocus();
  QString editorStyle( "QTextEdit { font: bold %1pt }" );
  int editorPointSize = widgets.editor->font().pointSize();
  widgets.editor->setStyleSheet( editorStyle.arg( editorPointSize + 3 ) );
  widgets.editor->setFixedHeight( widgets.editor->sizeHint().height() );
  editorLayout->addWidget( widgets.editor );
  layouts.root->addLayout( editorLayout );

  // for autocalc
  widgets.autoCalcLabel = new AutoHideLabel( p );
  widgets.autoCalcLabel->hide();

  // for tip of the day and menu bar hiding message
  widgets.tip = new TipWidget( p );
  widgets.tip->hide();
}


void MainWindow::Private::createKeypad()
{
  widgets.keypad = new Keypad( settings.getRadixChar(), widgets.root );
  widgets.keypad->setFocusPolicy( Qt::NoFocus );

  connect( widgets.keypad, SIGNAL( buttonPressed( Keypad::Button ) ),
           p, SLOT( keypadButtonPressed( Keypad::Button ) ) );
  connect( p, SIGNAL( radixCharChanged( char ) ),
           widgets.keypad, SLOT( setRadixChar( char ) ) );
  connect( p, SIGNAL( languageChanged() ),
           widgets.keypad, SLOT( retranslateText() ) );

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
  if ( br_init( & error ) == 0 && error != BR_INIT_ERROR_DISABLED )
  {
      qDebug( "Warning: BinReloc failed to initialize (error code %d)", error );
      qDebug( "Will fallback to hardcoded default path." );
  }

  char * dataDir = br_find_data_dir( 0 );
  booksDir = QString( dataDir ) + "/speedcrunch/books/";
  free( dataDir );
#endif

  docks.book = new BookDock( booksDir, "math_index.html", settings.language,
                             p );
  docks.book->setObjectName( "BookDock" );
  docks.book->setTitle( MainWindow::tr( "Math Book" ) );
  docks.book->installEventFilter( p );
  docks.book->setAllowedAreas( Qt::AllDockWidgetAreas );
  p->addDockWidget( Qt::RightDockWidgetArea, docks.book );

  connect( docks.book, SIGNAL( expressionSelected( const QString & ) ),
           p, SLOT( expressionSelected( const QString & ) ) );

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

  settings.showBook = true;
}


void MainWindow::Private::createConstantsDock()
{
  docks.constants = new ConstantsDock( constants, settings.getRadixChar(), p );
  docks.constants->setObjectName( "ConstantsDock" );
  docks.constants->installEventFilter( p );
  docks.constants->setAllowedAreas( Qt::AllDockWidgetAreas );
  p->addDockWidget( Qt::RightDockWidgetArea, docks.constants );

  connect( docks.constants, SIGNAL( constantSelected( const QString & ) ),
           p, SLOT( constantSelected( const QString & ) ) );
  connect( p, SIGNAL( radixCharChanged( char ) ),
           docks.constants, SLOT( setRadixChar( char ) ) );
  connect( p, SIGNAL( languageChanged() ),
           docks.constants, SLOT( retranslateText() ) );

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

  settings.showConstants = true;
}


void MainWindow::Private::createFunctionsDock()
{
  docks.functions = new FunctionsDock( functions, p );
  docks.functions->setObjectName( "FunctionsDock" );
  docks.functions->installEventFilter( p );
  docks.functions->setAllowedAreas( Qt::AllDockWidgetAreas );
  p->addDockWidget( Qt::RightDockWidgetArea, docks.functions );

  connect( docks.functions, SIGNAL( functionSelected( const QString & ) ),
           p, SLOT( functionSelected( const QString & ) ) );
  connect( p, SIGNAL( languageChanged() ),
           docks.functions, SLOT( retranslateText() ) );

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

  settings.showFunctions = true;
}


void MainWindow::Private::createHistoryDock()
{
  docks.history = new HistoryDock( p );
  docks.history->setObjectName( "HistoryDock" );
  docks.history->installEventFilter( p );
  docks.history->setAllowedAreas( Qt::AllDockWidgetAreas );
  p->addDockWidget( Qt::RightDockWidgetArea, docks.history );

  connect( docks.history, SIGNAL( expressionSelected( const QString & ) ),
           p, SLOT( expressionSelected( const QString & ) ) );
  connect( p, SIGNAL( languageChanged() ),
           docks.history, SLOT( retranslateText() ) );

  docks.history->setHistory( widgets.editor->history() );

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

  settings.showHistory = true;
}


void MainWindow::Private::createVariablesDock()
{
  docks.variables = new VariablesDock( settings.getRadixChar(), p );
  docks.variables->setObjectName( "VariablesDock" );
  docks.variables->installEventFilter( p );
  docks.variables->setAllowedAreas( Qt::AllDockWidgetAreas );
  p->addDockWidget( Qt::RightDockWidgetArea, docks.variables );

  connect( docks.variables, SIGNAL( variableSelected( const QString & ) ),
           p, SLOT( variableSelected( const QString & ) ) );
  connect( p, SIGNAL( radixCharChanged( char ) ),
           docks.variables, SLOT( setRadixChar( char ) ) );
  connect( p, SIGNAL( languageChanged() ),
           docks.variables, SLOT( retranslateText() ) );

  docks.variables->updateList( evaluator );

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

  settings.showVariables = true;
}


void MainWindow::Private::createFixedConnections()
{
  connect( actions.editClearExpression,    SIGNAL( triggered() ), p,              SLOT( clearExpression()    ) );
  connect( actions.editClearHistory,       SIGNAL( triggered() ), p,              SLOT( clearHistory()       ) );
  connect( actions.editCopyLastResult,     SIGNAL( triggered() ), p,              SLOT( copyResult()         ) );
  connect( actions.editCopy,               SIGNAL( triggered() ), widgets.editor, SLOT( copy()               ) );
  connect( actions.editDeleteAllVariables, SIGNAL( triggered() ), p,              SLOT( deleteAllVariables() ) );
  connect( actions.editDeleteVariable,     SIGNAL( triggered() ), p,              SLOT( deleteVariable()     ) );
  connect( actions.editInsertFunction,     SIGNAL( triggered() ), p,              SLOT( insertFunction()     ) );
  connect( actions.editInsertVariable,     SIGNAL( triggered() ), p,              SLOT( insertVariable()     ) );
  connect( actions.editPaste,              SIGNAL( triggered() ), widgets.editor, SLOT( paste()              ) );
  connect( actions.editSelectExpression,   SIGNAL( triggered() ), p,              SLOT( selectExpression()   ) );

  connect( actions.helpAboutQt,     SIGNAL( triggered() ), p, SLOT( aboutQt()         ) );
  connect( actions.helpAbout,       SIGNAL( triggered() ), p, SLOT( about()           ) );
  connect( actions.helpTipOfTheDay, SIGNAL( triggered() ), p, SLOT( showTipOfTheDay() ) );
  connect( actions.helpWebsite,     SIGNAL( triggered() ), p, SLOT( gotoWebsite()     ) );

  connect( actions.sessionExport, SIGNAL( triggered() ), p, SLOT( exportSession() ) );
  connect( actions.sessionImport, SIGNAL( triggered() ), p, SLOT( importSession() ) );
  connect( actions.sessionLoad,   SIGNAL( triggered() ), p, SLOT( loadSession()   ) );
  connect( actions.sessionQuit,   SIGNAL( triggered() ), p, SLOT( close()         ) );
  connect( actions.sessionSave,   SIGNAL( triggered() ), p, SLOT( saveSession()   ) );

  connect( actions.settingsAngleUnitDegree, SIGNAL( triggered() ), p, SLOT( degree() ) );
  connect( actions.settingsAngleUnitRadian, SIGNAL( triggered() ), p, SLOT( radian() ) );

  connect( actions.settingsBehaviorAlwaysOnTop,         SIGNAL( toggled( bool )                    ), p,                     SLOT( alwaysOnTopToggled( bool )         ) );
  connect( actions.settingsBehaviorAutoCompletion,      SIGNAL( toggled( bool )                    ), p,                     SLOT( autoCompletionToggled( bool )      ) );
  connect( actions.settingsBehaviorMinimizeToTray,      SIGNAL( toggled( bool )                    ), p,                     SLOT( minimizeToTrayToggled( bool )      ) );
  connect( actions.settingsBehaviorPartialResults,      SIGNAL( toggled( bool )                    ), p,                     SLOT( autoCalcToggled( bool )            ) );
  connect( actions.settingsBehaviorSaveHistoryOnExit,   SIGNAL( toggled( bool )                    ), p,                     SLOT( saveHistoryOnExitToggled( bool )   ) );
  connect( actions.settingsBehaviorSaveVariablesOnExit, SIGNAL( toggled( bool )                    ), p,                     SLOT( saveVariablesOnExitToggled( bool ) ) );
  connect( actions.settingsBehaviorSyntaxHilite,        SIGNAL( toggled( bool )                    ), p,                     SLOT( hiliteSyntaxToggled( bool )        ) );

  connect( actions.settingsRadixCharComma,   SIGNAL( triggered() ), p, SLOT( radixCharCommaTriggered() ) );
  connect( actions.settingsRadixCharDefault, SIGNAL( triggered() ), p, SLOT( radixCharAutoTriggered()  ) );
  connect( actions.settingsRadixCharDot,     SIGNAL( triggered() ), p, SLOT( radixCharDotTriggered()   ) );

  connect( actions.settingsResultFormat15Digits,      SIGNAL( triggered() ), p, SLOT( digits15()          ) );
  connect( actions.settingsResultFormat2Digits,       SIGNAL( triggered() ), p, SLOT( digits2()           ) );
  connect( actions.settingsResultFormat3Digits,       SIGNAL( triggered() ), p, SLOT( digits3()           ) );
  connect( actions.settingsResultFormat50Digits,      SIGNAL( triggered() ), p, SLOT( digits50()          ) );
  connect( actions.settingsResultFormat8Digits,       SIGNAL( triggered() ), p, SLOT( digits8()           ) );
  connect( actions.settingsResultFormatAutoPrecision, SIGNAL( triggered() ), p, SLOT( digitsAuto()        ) );
  connect( actions.settingsResultFormatBinary,        SIGNAL( triggered() ), p, SLOT( formatBinary()      ) );
  connect( actions.settingsResultFormatEngineering,   SIGNAL( triggered() ), p, SLOT( formatEngineering() ) );
  connect( actions.settingsResultFormatFixed,         SIGNAL( triggered() ), p, SLOT( formatFixed()       ) );
  connect( actions.settingsResultFormatGeneral,       SIGNAL( triggered() ), p, SLOT( formatGeneral()     ) );
  connect( actions.settingsResultFormatHexadecimal,   SIGNAL( triggered() ), p, SLOT( formatHexadec()     ) );
  connect( actions.settingsResultFormatOctal,         SIGNAL( triggered() ), p, SLOT( formatOctal()       ) );
  connect( actions.settingsResultFormatScientific,    SIGNAL( triggered() ), p, SLOT( formatScientific()  ) );

  connect( actions.viewConstants,      SIGNAL( toggled( bool ) ), p, SLOT( showConstants( bool )    ) );
  connect( actions.viewFullScreenMode, SIGNAL( toggled( bool ) ), p, SLOT( showInFullScreen( bool ) ) );
  connect( actions.viewFunctions,      SIGNAL( toggled( bool ) ), p, SLOT( showFunctions( bool )    ) );
  connect( actions.viewHistory,        SIGNAL( toggled( bool ) ), p, SLOT( showHistory( bool )      ) );
  connect( actions.viewKeypad,         SIGNAL( toggled( bool ) ), p, SLOT( showKeypad( bool )       ) );
  connect( actions.viewMathBook,       SIGNAL( toggled( bool ) ), p, SLOT( showBook( bool )         ) );
  connect( actions.viewMenuBar,        SIGNAL( toggled( bool ) ), p, SLOT( showMenuBar( bool )      ) );
  connect( actions.viewStatusBar,      SIGNAL( toggled( bool ) ), p, SLOT( showStatusBar( bool )    ) );
  connect( actions.viewVariables,      SIGNAL( toggled( bool ) ), p, SLOT( showVariables( bool )    ) );

  connect( actions.languageDefault, SIGNAL( triggered() ), p, SLOT( changeLanguage() ) );
  connect( actions.languageCa,      SIGNAL( triggered() ), p, SLOT( changeLanguage() ) );
  connect( actions.languageCs,      SIGNAL( triggered() ), p, SLOT( changeLanguage() ) );
  connect( actions.languageDe,      SIGNAL( triggered() ), p, SLOT( changeLanguage() ) );
  connect( actions.languageEn,      SIGNAL( triggered() ), p, SLOT( changeLanguage() ) );
  connect( actions.languageEs,      SIGNAL( triggered() ), p, SLOT( changeLanguage() ) );
  connect( actions.languageEsAr,    SIGNAL( triggered() ), p, SLOT( changeLanguage() ) );
  connect( actions.languageEu,      SIGNAL( triggered() ), p, SLOT( changeLanguage() ) );
  connect( actions.languageFi,      SIGNAL( triggered() ), p, SLOT( changeLanguage() ) );
  connect( actions.languageFr,      SIGNAL( triggered() ), p, SLOT( changeLanguage() ) );
  connect( actions.languageHe,      SIGNAL( triggered() ), p, SLOT( changeLanguage() ) );
  connect( actions.languageId,      SIGNAL( triggered() ), p, SLOT( changeLanguage() ) );
  connect( actions.languageIt,      SIGNAL( triggered() ), p, SLOT( changeLanguage() ) );
  connect( actions.languageNb,      SIGNAL( triggered() ), p, SLOT( changeLanguage() ) );
  connect( actions.languageNl,      SIGNAL( triggered() ), p, SLOT( changeLanguage() ) );
  connect( actions.languagePl,      SIGNAL( triggered() ), p, SLOT( changeLanguage() ) );
  connect( actions.languagePt,      SIGNAL( triggered() ), p, SLOT( changeLanguage() ) );
  connect( actions.languagePtBr,    SIGNAL( triggered() ), p, SLOT( changeLanguage() ) );
  connect( actions.languageRo,      SIGNAL( triggered() ), p, SLOT( changeLanguage() ) );
  connect( actions.languageRu,      SIGNAL( triggered() ), p, SLOT( changeLanguage() ) );
  connect( actions.languageSv,      SIGNAL( triggered() ), p, SLOT( changeLanguage() ) );
  connect( actions.languageTr,      SIGNAL( triggered() ), p, SLOT( changeLanguage() ) );
  connect( actions.languageZhCn,    SIGNAL( triggered() ), p, SLOT( changeLanguage() ) );

  connect( actions.scrollDown, SIGNAL( triggered() ), p, SLOT( scrollDown() ) );
  connect( actions.scrollUp,   SIGNAL( triggered() ), p, SLOT( scrollUp()   ) );

  connect( widgets.display, SIGNAL( textSelected( const QString & ) ), widgets.editor, SLOT( paste()                            ) );
  connect( widgets.display, SIGNAL( textSelected( const QString & ) ), widgets.editor, SLOT( setFocus()                         ) );

  connect( widgets.editor, SIGNAL( autoCalcDisabled()                 ), p, SLOT( hideAutoCalc()                  ) );
  connect( widgets.editor, SIGNAL( autoCalcEnabled( const QString & ) ), p, SLOT( showAutoCalc( const QString & ) ) );
  connect( widgets.editor, SIGNAL( returnPressed()                    ), p, SLOT( returnPressed()                 ) );
  connect( widgets.editor, SIGNAL( textChanged()                      ), p, SLOT( textChanged()                   ) );

  connect( p, SIGNAL( resultFormatChanged( char )   ), widgets.editor,  SLOT( setFormat( char )    ) );
  connect( p, SIGNAL( resultPrecisionChanged( int ) ), widgets.editor,  SLOT( setPrecision( int )  ) );
  connect( p, SIGNAL( radixCharChanged( char )      ), widgets.editor,  SLOT( setRadixChar( char ) ) );
  connect( p, SIGNAL( resultFormatChanged( char )   ), widgets.display, SLOT( setFormat( char )    ) );
  connect( p, SIGNAL( resultPrecisionChanged( int ) ), widgets.display, SLOT( setPrecision( int )  ) );
  connect( p, SIGNAL( radixCharChanged( char )      ), widgets.display, SLOT( setRadixChar( char ) ) );
  connect( p, SIGNAL( radixCharChanged( char )      ), evaluator,       SLOT( setRadixChar( char ) ) );
  connect( p, SIGNAL( angleUnitChanged( char )      ), functions,       SLOT( setAngleMode( char ) ) );
  connect( p, SIGNAL( languageChanged()             ), p,               SLOT( setAllText()         ) );
}


void MainWindow::Private::applySettings()
{
  // window state
  //
  actions.viewMathBook ->setChecked( settings.showBook      );
  actions.viewConstants->setChecked( settings.showConstants );
  actions.viewFunctions->setChecked( settings.showFunctions );
  actions.viewHistory  ->setChecked( settings.showHistory   );
  actions.viewVariables->setChecked( settings.showVariables );
  // THIS IS NOT NEEDED ANYMORE, BUT NEEDS TO BE TESTED WITH Qt 4.2
  //restoreFloatingDocks();
  //
  p->resize( settings.windowSize );
  //
  if ( settings.windowPosition.isNull() )
  {
      QDesktopWidget * desktop = QApplication::desktop();
      QRect screen = desktop->availableGeometry( p );
      p->move( screen.center() - p->rect().center() );
  }
  else
      p->move( settings.windowPosition );
  //
  p->restoreState( settings.windowState );

  // full screen
  actions.viewFullScreenMode->setChecked( settings.showFullScreen );

  // always-on-top
  actions.settingsBehaviorAlwaysOnTop->setChecked( settings.stayAlwaysOnTop );

  // angle mode
  if ( settings.angleMode == 'r' )
    actions.settingsAngleUnitRadian->setChecked( true );
  else if ( settings.angleMode == 'd' )
    actions.settingsAngleUnitDegree->setChecked( true );

  // language
  checkInitialLanguage();

  // history
  if ( settings.saveHistory )
  {
    actions.settingsBehaviorSaveHistoryOnExit->setChecked( true );
    restoreHistory();
  }

  // variables
  if ( settings.saveVariables )
  {
    actions.settingsBehaviorSaveVariablesOnExit->setChecked( true );
    restoreVariables();
  }

  // resultFormat
  checkInitialResultFormat();

  // precision
  checkInitialPrecision();

  // radix character
  if ( settings.isLocaleRadixChar() )
      actions.settingsRadixCharDefault->setChecked( true );
  else if ( settings.getRadixChar() == '.' )
    actions.settingsRadixCharDot->setChecked( true );
  else if ( settings.getRadixChar() == ',' )
    actions.settingsRadixCharComma->setChecked( true );

  // keypad
  actions.viewKeypad->setChecked( settings.showKeypad );

  // autocalc
  if ( settings.autoCalc )
    actions.settingsBehaviorPartialResults->setChecked( true );
  else
    p->autoCalcToggled( false );

  // autocomplete
  if ( settings.autoComplete )
    actions.settingsBehaviorAutoCompletion->setChecked( true );
  else
    p->autoCompletionToggled( false );

  // minimize to system tray
  actions.settingsBehaviorMinimizeToTray->setChecked( settings.minimizeToTray );

  // syntax hilite
  if ( settings.hiliteSyntax )
    actions.settingsBehaviorSyntaxHilite->setChecked( true );
  else
    p->hiliteSyntaxToggled( false );

  // status bar
  actions.viewStatusBar->setChecked( settings.showStatusBar );

  // menu bar
#ifndef Q_OS_MAC
  actions.viewMenuBar->setChecked( settings.showMenuBar );
  p->menuBar()->setVisible( settings.showMenuBar );
#endif
}


void MainWindow::Private::checkInitialResultFormat()
{
  switch ( settings.resultFormat )
  {
    case 'f': actions.settingsResultFormatFixed      ->setChecked( true ); break;
    case 'n': actions.settingsResultFormatEngineering->setChecked( true ); break;
    case 'e': actions.settingsResultFormatScientific ->setChecked( true ); break;
    case 'h': actions.settingsResultFormatHexadecimal->setChecked( true ); break;
    case 'o': actions.settingsResultFormatOctal      ->setChecked( true ); break;
    case 'b': actions.settingsResultFormatBinary     ->setChecked( true ); break;
    default : actions.settingsResultFormatGeneral    ->setChecked( true );
  }
}


void MainWindow::Private::checkInitialPrecision()
{
  switch ( settings.resultPrecision )
  {
    case 2 : actions.settingsResultFormat2Digits      ->setChecked( true ); break;
    case 3 : actions.settingsResultFormat3Digits      ->setChecked( true ); break;
    case 8 : actions.settingsResultFormat8Digits      ->setChecked( true ); break;
    case 15: actions.settingsResultFormat15Digits     ->setChecked( true ); break;
    case 50: actions.settingsResultFormat50Digits     ->setChecked( true ); break;
    default: actions.settingsResultFormatAutoPrecision->setChecked( true );
  }
}


void MainWindow::Private::checkInitialLanguage()
{
  if ( settings.language == "ca" )
    actions.languageCa->setChecked( true );
  else if ( settings.language == "cs" )
    actions.languageCs->setChecked( true );
  else if ( settings.language == "de" )
    actions.languageDe->setChecked( true );
  else if ( settings.language == "en" )
    actions.languageEn->setChecked( true );
  else if ( settings.language == "es" )
    actions.languageEs->setChecked( true );
  else if ( settings.language == "es_AR" )
    actions.languageEsAr->setChecked( true );
  else if ( settings.language == "eu" )
    actions.languageEu->setChecked( true );
  else if ( settings.language == "fi" )
    actions.languageFi->setChecked( true );
  else if ( settings.language == "fr" )
    actions.languageFr->setChecked( true );
  else if ( settings.language == "he" )
    actions.languageHe->setChecked( true );
  else if ( settings.language == "id" )
    actions.languageId->setChecked( true );
  else if ( settings.language == "it" )
    actions.languageIt->setChecked( true );
  else if ( settings.language == "nb" )
    actions.languageNb->setChecked( true );
  else if ( settings.language == "nl" )
    actions.languageNl->setChecked( true );
  else if ( settings.language == "pl" )
    actions.languagePl->setChecked( true );
  else if ( settings.language == "pt" )
    actions.languagePt->setChecked( true );
  else if ( settings.language == "pt_BR" )
    actions.languagePtBr->setChecked( true );
  else if ( settings.language == "ro" )
    actions.languageRo->setChecked( true );
  else if ( settings.language == "ru" )
    actions.languageRu->setChecked( true );
  else if ( settings.language == "sv" )
    actions.languageSv->setChecked( true );
  else if ( settings.language == "tr" )
    actions.languageTr->setChecked( true );
  else if ( settings.language == "zn_CN" )
    actions.languageZhCn->setChecked( true );
  else
    actions.languageDefault->setChecked( true );
}


void MainWindow::Private::saveSettings()
{
  // history
  if ( settings.saveHistory )
  {
    settings.history        = widgets.editor->history();
    settings.historyResults = widgets.editor->historyResults();
  }
  else
  {
    settings.history.clear();
    settings.historyResults.clear();
  }

  // variables
  if ( settings.saveVariables )
  {
    settings.variables.clear();
    QVector<Variable> vars = evaluator->variables();
    for ( int i = 0; i < vars.count(); i++ )
    {
      QString name = vars[i].name;
      char * value = HMath::formatScientific( vars[i].value, DECPRECISION );
      settings.variables.append( QString( "%1=%2" ).arg( name )
                                                   .arg( QString( value ) ) );
      free( value );
    }
  }

  // window
  settings.windowPosition = p->pos();
  settings.windowSize     = p->size();
  settings.windowState    = p->saveState();

  // THIS IS NOT NEEDED ANYMORE, BUT NEEDS TO BE TESTED WITH Qt 4.2
  //syncWindowStateToSettings();

  settings.save();
}


// THIS IS NOT NEEDED ANYMORE, BUT NEEDS TO BE TESTED WITH Qt 4.2
//void MainWindow::Private::syncWindowStateToSettings()
//{
//   docks
//  if ( docks.book )
//  {
//    settings.bookDockFloating = docks.book->isFloating();
//    settings.bookDockLeft     = docks.book->x();
//    settings.bookDockTop      = docks.book->y();
//    settings.bookDockWidth    = docks.book->width();
//    settings.bookDockHeight   = docks.book->height();
//  }
//  if ( docks.history )
//  {
//    settings.historyDockFloating = docks.history->isFloating();
//    settings.historyDockLeft     = docks.history->x();
//    settings.historyDockTop      = docks.history->y();
//    settings.historyDockWidth    = docks.history->width();
//    settings.historyDockHeight   = docks.history->height();
//  }
//  if ( docks.functions )
//  {
//    settings.functionsDockFloating = docks.functions->isFloating();
//    settings.functionsDockLeft     = docks.functions->x();
//    settings.functionsDockTop      = docks.functions->y();
//    settings.functionsDockWidth    = docks.functions->width();
//    settings.functionsDockHeight   = docks.functions->height();
//  }
//  if ( docks.variables )
//  {
//    settings.variablesDockFloating = docks.variables->isFloating();
//    settings.variablesDockLeft     = docks.variables->x();
//    settings.variablesDockTop      = docks.variables->y();
//    settings.variablesDockWidth    = docks.variables->width();
//    settings.variablesDockHeight   = docks.variables->height();
//  }
//  if ( docks.constants )
//  {
//    settings.constantsDockFloating = docks.constants->isFloating();
//    settings.constantsDockLeft     = docks.constants->x();
//    settings.constantsDockTop      = docks.constants->y();
//    settings.constantsDockWidth    = docks.constants->width();
//    settings.constantsDockHeight   = docks.constants->height();
//  }
//}


// public

MainWindow::MainWindow()
  : QMainWindow(), d( new MainWindow::Private )
{
  d->p = this;
  d->settings.load();
  d->createUi();
  d->applySettings();
  emit languageChanged();
  QTimer::singleShot( 0, this, SLOT( activate() ) );
}


MainWindow::~MainWindow()
{
  delete d;
}


bool MainWindow::event( QEvent * e )
{
  if ( e->type() == QEvent::WindowStateChange )
    if ( windowState() & Qt::WindowMinimized )
      if ( d->settings.minimizeToTray )
      {
        QTimer::singleShot( 100, this, SLOT( minimizeToTray() ) );
        return true;
      }

  return QMainWindow::event( e );
}


// public slots

void MainWindow::about()
{
  AboutBox dlg;
  dlg.exec();
}


void MainWindow::aboutQt()
{
  QMessageBox::aboutQt( this, tr( "About Qt" ) );
}


void MainWindow::clearHistory()
{
  d->widgets.display->clear();
  d->widgets.editor->clearHistory();
  if ( d->settings.showHistory )
    d->docks.history->clear();
  d->settings.history.clear();
  d->settings.historyResults.clear();
  QTimer::singleShot( 0, d->widgets.editor, SLOT( setFocus() ) );
}


void MainWindow::clearExpression()
{
  d->widgets.editor->clear();
  QTimer::singleShot( 0, d->widgets.editor, SLOT( setFocus() ) );
}


void MainWindow::copyResult()
{
  QClipboard * cb = QApplication::clipboard();
  HNumber num = d->evaluator->get( "ans" );
  char * strToCopy = HMath::format( num, d->settings.resultFormat,
                                    d->settings.resultPrecision );
  QString final( strToCopy );
  if ( d->widgets.display->radixChar() == ',' )
    final.replace( '.', ',' );
  cb->setText( final, QClipboard::Clipboard );
  free( strToCopy );
}


void MainWindow::degree()
{
  if ( d->settings.angleMode == 'd' )
    return;

  d->settings.angleMode = 'd';

  if ( d->status.angleUnit )
    d->status.angleUnit->setText( tr( "Degree" ) );

  emit angleUnitChanged( 'd' );
}


void MainWindow::deleteAllVariables()
{
  d->evaluator->clearVariables();

  if ( d->settings.showVariables )
    d->docks.variables->updateList( d->evaluator );
}


void MainWindow::deleteVariable()
{
  DeleteVariableDlg dlg( d->evaluator );
  dlg.exec();

  if ( d->settings.showVariables )
    d->docks.variables->updateList( d->evaluator );
}


void MainWindow::digits2()
{
  setResultPrecision( 2 );
}


void MainWindow::digits3()
{
  setResultPrecision( 3 );
}


void MainWindow::digits8()
{
  setResultPrecision( 8 );
}


void MainWindow::digits15()
{
  setResultPrecision( 15 );
}


void MainWindow::digits50()
{
  setResultPrecision( 50 );
}


void MainWindow::digitsAuto()
{
  setResultPrecision( -1 );
}


void MainWindow::selectExpression()
{
  activateWindow();
  d->widgets.editor->selectAll();
  d->widgets.editor->setFocus();
}


void MainWindow::gotoWebsite()
{
  QDesktopServices::openUrl(
    QUrl( QString::fromLatin1( "http://www.speedcrunch.org" ) ) );
}


void MainWindow::hideAutoCalc()
{
  d->widgets.autoCalcLabel->hideText();
}


void MainWindow::insertFunction()
{
  InsertFunctionDlg dlg( d->functions );

  if ( dlg.exec() == InsertFunctionDlg::Accepted )
  {
    QString funcName = dlg.functionName();
    if ( ! funcName.isEmpty() )
      d->widgets.editor->insert( funcName );
  }
}


void MainWindow::insertVariable()
{
  InsertVariableDlg dlg( d->evaluator, d->settings.getRadixChar() );

  if ( dlg.exec() == InsertVariableDlg::Accepted )
  {
    QString varName = dlg.variableName();
    if ( ! varName.isEmpty() )
      d->widgets.editor->insert( varName );
  }
}


void MainWindow::loadSession()
{
  QString errMsg  = tr( "File %1 is not a valid session" );
  QString filters = tr( "SpeedCrunch Sessions (*.sch);;All Files (*)" );
  QString fname   = QFileDialog::getOpenFileName( this, tr( "Load Session" ),
                                                  QString::null, filters );
  if ( fname.isEmpty() )
    return;

  QFile file( fname );
  if ( ! file.open( QIODevice::ReadOnly ) )
  {
    QMessageBox::critical( this, tr( "Error" ),
                           tr( "Can't read from file %1" ).arg( fname ) );
    return;
  }

  QTextStream stream( & file );

  // version of the format
  QString version = stream.readLine();
  if ( version != "0.10" )
  {
    QMessageBox::critical( this, tr( "Error" ), errMsg.arg( fname ) );
    return;
  }

  // number of calculations
  bool ok;
  int noCalcs = stream.readLine().toInt( &ok );
  if ( ok == false || noCalcs < 0 )
  {
    QMessageBox::critical( this, tr( "Error" ), errMsg.arg( fname ) );
    return;
  }

  // ask for merge with current session
  QString mergeMsg = tr( "Merge session being loaded with current session?\n"
                         "If no, current variables and display will be "
                         "cleared." );
  QMessageBox::StandardButton but
    = QMessageBox::question( this, tr( "Question" ), mergeMsg,
                             QMessageBox::Yes | QMessageBox::No
                             | QMessageBox::Cancel, QMessageBox::Yes );

  if ( but == QMessageBox::Cancel )
  {
    return;
  }
  else if ( but == QMessageBox::No )
  {
    d->widgets.display->clear();
    deleteAllVariables();
    clearHistory();
  }

  // expressions and results
  QStringList expLs;
  QStringList resLs;
  for ( int i = 0; i < noCalcs; i++ )
  {
    QString exp = stream.readLine();
    QString res = stream.readLine();
    if ( exp.isNull() || res.isNull() )
    {
      QMessageBox::critical( this, tr( "Error" ), errMsg.arg( fname ) );
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

  if ( d->settings.showHistory )
    d->docks.history->appendHistory( expLs );

  // variables
  int noVars = stream.readLine().toInt( &ok );
  if ( ok == false || noVars < 0 )
  {
    QMessageBox::critical( this, tr( "Error" ), errMsg.arg( fname ) );
    return;
  }
  for ( int i = 0; i < noVars; i++ )
  {
    QString var = stream.readLine();
    QString val = stream.readLine();
    if ( var.isNull() || val.isNull() )
    {
      QMessageBox::critical( this, tr( "Error" ), errMsg.arg( fname ) );
      return;
    }
    HNumber num( val.toAscii().data() );
    if (  num != HNumber::nan() )
      d->evaluator->set( var, num );
  }

  file.close();
}


void MainWindow::importSession()
{
  QString filters = tr( "All Files (*)" );
  QString fname = QFileDialog::getOpenFileName( this, tr( "Import Session" ),
                                                QString::null, filters );
  if ( fname.isEmpty() )
    return;

  QFile file( fname );
  if ( ! file.open( QIODevice::ReadOnly ) )
  {
    QMessageBox::critical( this, tr( "Error" ),
                           tr( "Can't read from file %1" ).arg( fname ) );
    return;
  }

  // ask for merge with current session
  QString mergeMsg = tr( "Merge session being imported with current session?\n"
                         "If no, current variables and display will be "
                         "cleared." );
  QMessageBox::StandardButton but
    = QMessageBox::question( this, tr( "Question" ), mergeMsg,
                             QMessageBox::Yes | QMessageBox::No
                             | QMessageBox::Cancel, QMessageBox::Yes );

  if ( but == QMessageBox::Cancel )
  {
    return;
  }
  else if ( but == QMessageBox::No )
  {
    d->widgets.display->clear();
    deleteAllVariables();
    clearHistory();
  }

  QTextStream stream( & file );
  QString exp = stream.readLine();
  bool ignoreAll = false;
  while ( ! exp.isNull() )
  {
    d->widgets.editor->setText( exp );

    QString str = d->evaluator->autoFix( exp );

    d->evaluator->setExpression( str );

    HNumber result = d->evaluator->evalUpdateAns();
    if ( ! d->evaluator->error().isEmpty() )
    {
      if ( ! ignoreAll )
      {
        QMessageBox::StandardButton but
          = QMessageBox::warning( this, tr( "Error" ),
              tr( "Ignore error?" ) + "\n" + d->evaluator->error(),
              QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::Cancel,
              QMessageBox::Yes );

        if ( but == QMessageBox::Cancel )
          return;
        else if ( but == QMessageBox::YesToAll )
          ignoreAll = true;
      }
    }
    else
    {
      d->widgets.display->append( str, result );
      char * num = HMath::formatScientific( result, DECPRECISION );
      d->widgets.editor->appendHistory( str, num );
      free( num );
      d->widgets.editor->setAnsAvailable( true );
      if ( d->settings.showVariables )
        d->docks.variables->updateList( d->evaluator );
      if ( d->settings.showHistory )
        d->docks.history->append( str );

      d->widgets.editor->setText( str );
      d->widgets.editor->selectAll();
      d->widgets.editor->stopAutoCalc();
      d->widgets.editor->stopAutoComplete();
      d->conditions.autoAns = true;
    }

    exp = stream.readLine();
  }

  file.close();

  QTimer::singleShot( 0, d->widgets.editor, SLOT( setFocus() ) );

  if ( ! isActiveWindow() )
    activateWindow();
}


void MainWindow::alwaysOnTopToggled( bool b )
{
  d->settings.stayAlwaysOnTop = b;

  QPoint cur = mapToGlobal( QPoint(0, 0) );
  if ( b )
    setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint );
  else
    setWindowFlags( windowFlags() & (~ Qt::WindowStaysOnTopHint) );
  move( cur );
  show();
}


void MainWindow::autoCalcToggled( bool b )
{
  d->settings.autoCalc = b;
  d->widgets.editor->setAutoCalcEnabled( b );
}


void MainWindow::saveHistoryOnExitToggled( bool b )
{
  d->settings.saveHistory = b;
}


void MainWindow::saveVariablesOnExitToggled( bool b )
{
  d->settings.saveVariables = b;
}


void MainWindow::autoCompletionToggled( bool b )
{
  d->settings.autoComplete = b;
  d->widgets.editor->setAutoCompleteEnabled( b );
}


void MainWindow::minimizeToTrayToggled( bool b )
{
  if ( b && ! d->widgets.trayIcon && QSystemTrayIcon::isSystemTrayAvailable() )
  {
    d->conditions.trayNotify = true;
    d->widgets.trayIcon = new QSystemTrayIcon( this );
    d->widgets.trayIcon->setToolTip( "SpeedCrunch" );
    d->widgets.trayIcon->setIcon( QPixmap( ":/speedcrunch.png" ) );

    d->menus.trayIcon = new QMenu( this );
    d->menus.trayIcon->addAction( d->actions.editCopyLastResult );
    d->menus.trayIcon->addSeparator();
    d->menus.trayIcon->addAction( d->actions.sessionQuit    );

    d->widgets.trayIcon->setContextMenu( d->menus.trayIcon );
    connect( d->widgets.trayIcon,
             SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
             this,
             SLOT( trayIconActivated( QSystemTrayIcon::ActivationReason ) ) );
  }
  else
  {
    if ( d->widgets.trayIcon )
      delete d->widgets.trayIcon;
    d->widgets.trayIcon = 0;
  }

  d->settings.minimizeToTray = b;
}


void MainWindow::hiliteSyntaxToggled( bool b )
{
  d->widgets.editor->setSyntaxHighlight( b );
  d->settings.hiliteSyntax = b;
}


void MainWindow::radian()
{
  if ( d->settings.angleMode == 'r' )
    return;

  d->settings.angleMode = 'r';

  if ( d->status.angleUnit )
    d->status.angleUnit->setText( tr( "Radian" ) );

  emit angleUnitChanged( 'r' );
}


void MainWindow::saveSession()
{
  QString filters = tr( "SpeedCrunch Sessions (*.sch);;All Files (*)" );
  QString fname = QFileDialog::getSaveFileName( this, tr( "Save Session" ),
                                                QString::null, filters );
  if ( fname.isEmpty() )
    return;

  QFile file( fname );
  if ( ! file.open( QIODevice::WriteOnly ) )
  {
    QMessageBox::critical( this, tr( "Error" ),
                           tr( "Can't write to file %1" ).arg( fname ) );
    return;
  }

  QTextStream stream( & file );

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
  for ( int i = 0; i < noVars; i++ )
  {
    Variable var = d->evaluator->variables()[i];
    if ( var.name != "pi" && var.name != "phi" )
    {
      char * value = HMath::format( var.value );
      stream << var.name << "\n" << value << "\n";
      free( value );
    }
  }

  file.close();
}


void MainWindow::exportSession()
{
  QString filters = tr( "All Files (*)" );
  QString fname = QFileDialog::getSaveFileName( this, tr( "Export Session" ),
                                                QString::null, filters );
  if ( fname.isEmpty() )
    return;

  QFile file( fname );
  if ( ! file.open( QIODevice::WriteOnly ) )
  {
    QMessageBox::critical( this, tr( "Error" ),
                           tr( "Can't write to file %1" ).arg( fname ) );
    return;
  }

  QTextStream stream( & file );

  // expressions and results
  stream << d->widgets.display->asText() << "\n";

  file.close();
}


void MainWindow::setWidgetsDirection()
{
  if ( (d->settings.language == "C" && QLocale().language() == QLocale::Hebrew)
       || d->settings.language == "he" )
  {
    qApp->setLayoutDirection( Qt::RightToLeft );
  }
  else
  {
    qApp->setLayoutDirection( Qt::LeftToRight );
  }
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


void MainWindow::showMenuBar( bool b )
{
  menuBar()->setVisible( b );
  d->settings.showMenuBar = b;

  if ( ! b && d->conditions.notifyMenuBarHidden )
  {
    showMenuBarTip();
    d->conditions.notifyMenuBarHidden = false;
  }

  d->widgets.display->scrollEnd();
}


void MainWindow::showStatusBar( bool b )
{
  b ? d->createStatusBar() : d->deleteStatusBar();
  d->settings.showStatusBar = b;
}


void MainWindow::showAutoCalc( const QString & msg )
{
  QPoint p = d->widgets.editor->mapToParent( QPoint(0, 0) );
  d->widgets.autoCalcLabel->move( p );
  d->widgets.autoCalcLabel->showText( msg );
}


void MainWindow::showInFullScreen( bool b )
{
  d->settings.showFullScreen = b;
  b ? showFullScreen() : showNormal();
}


bool MainWindow::eventFilter( QObject * o, QEvent * e )
{
  if ( o == d->docks.book )
  {
    if ( e->type() == QEvent::Close )
    {
      d->deleteBookDock();
      return true;
    }
    return false;
  }

  if ( o == d->docks.constants )
  {
    if ( e->type() == QEvent::Close )
    {
      d->deleteConstantsDock();
      return true;
    }
    return false;
  }

  if ( o == d->docks.functions )
  {
    if ( e->type() == QEvent::Close )
    {
      d->deleteFunctionsDock();
      return true;
    }
    return false;
  }

  if ( o == d->docks.history )
  {
    if ( e->type() == QEvent::Close )
    {
      d->deleteHistoryDock();
      return true;
    }
    return false;
  }

  if ( o == d->docks.variables )
  {
    if ( e->type() == QEvent::Close )
    {
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
  delete widgets.keypad;
  widgets.keypad = 0;

  layouts.root->removeItem( layouts.keypad );
  delete layouts.keypad;
  layouts.keypad = 0;
}


void MainWindow::Private::deleteStatusBar()
{
  delete status.angleUnit;
  status.angleUnit = 0;

  delete status.resultFormat;
  status.resultFormat = 0;

  p->setStatusBar( 0 );
}


void MainWindow::Private::deleteBookDock()
{
  Q_ASSERT( docks.book );

  p->removeDockWidget( docks.book );
  p->disconnect( docks.book );
  delete docks.book;
  docks.book = 0;
  actions.viewMathBook->blockSignals( true );
  actions.viewMathBook->setChecked( false );
  actions.viewMathBook->blockSignals( false );
  settings.showBook = false;
}


void MainWindow::Private::deleteConstantsDock()
{
  Q_ASSERT( docks.constants );

  p->removeDockWidget( docks.constants );
  p->disconnect( docks.constants );
  delete docks.constants;
  docks.constants = 0;
  actions.viewConstants->blockSignals( true );
  actions.viewConstants->setChecked( false );
  actions.viewConstants->blockSignals( false );
  settings.showConstants = false;
}


void MainWindow::Private::deleteFunctionsDock()
{
  Q_ASSERT( docks.functions );

  p->removeDockWidget( docks.functions );
  p->disconnect( docks.functions );
  delete docks.functions;
  docks.functions = 0;
  actions.viewFunctions->blockSignals( true );
  actions.viewFunctions->setChecked( false );
  actions.viewFunctions->blockSignals( false );
  settings.showFunctions = false;
}


void MainWindow::Private::deleteHistoryDock()
{
  Q_ASSERT( docks.history );

  p->removeDockWidget( docks.history );
  p->disconnect( docks.history );
  delete docks.history;
  docks.history = 0;
  actions.viewHistory->blockSignals( true );
  actions.viewHistory->setChecked( false );
  actions.viewHistory->blockSignals( false );
  settings.showHistory = false;
}


void MainWindow::Private::deleteVariablesDock()
{
  Q_ASSERT( docks.variables );

  p->removeDockWidget( docks.variables );
  p->disconnect( docks.variables );
  delete docks.variables;
  docks.variables = 0;
  actions.viewVariables->blockSignals( true );
  actions.viewVariables->setChecked( false );
  actions.viewVariables->blockSignals( false );
  settings.showVariables = false;
}


void MainWindow::showFunctions( bool b )
{
  if ( b )
    d->createFunctionsDock();
  else
    d->deleteFunctionsDock();
}


void MainWindow::showBook( bool b )
{
  if ( b )
    d->createBookDock();
  else
    d->deleteBookDock();
}


void MainWindow::showConstants( bool b )
{
  if ( b )
    d->createConstantsDock();
  else
    d->deleteConstantsDock();
}


void MainWindow::showHistory( bool b )
{
  if ( b )
    d->createHistoryDock();
  else
    d->deleteHistoryDock();
}


void MainWindow::showVariables( bool b )
{
  if ( b )
    d->createVariablesDock();
  else
    d->deleteVariablesDock();
}


void MainWindow::showKeypad( bool b )
{
  d->settings.showKeypad = b;
  if ( b )
    d->createKeypad();
  else
    d->deleteKeypad();
}


void MainWindow::showMenuBarTip()
{
  QString msg = tr( "The menu bar is now hidden. "
                    "To make it visible again, press Ctrl+Alt+M." );

  //QPoint p = mapFromGlobal( d->widgets.display->mapToGlobal( QPoint(0, 0) ) )
  //                          += QPoint(5, 5);
  d->widgets.tip->move( 0, 0 );
  d->widgets.tip->showText( msg, tr( "Warning" ) );
}


void MainWindow::showTipOfTheDay()
{
  //QPoint p = mapFromGlobal( d->widgets.display->mapToGlobal( QPoint(0, 0) ) )
  //                          += QPoint(5, 5);
  d->widgets.tip->move( 0, 0 );

  int tipNo = qrand() % 4;
  QString msg;
  switch ( tipNo )
  {
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

  d->widgets.tip->showText( msg, tr( "Tip of the day" ) );
}


void MainWindow::formatBinary()
{
  d->actionGroups.digits->setDisabled( true );
  setResultFormat( 'b' );

  if ( d->status.resultFormat )
    d->status.resultFormat->setText( tr( "Binary" ) );
}


void MainWindow::formatEngineering()
{
  d->actionGroups.digits->setEnabled( true );
  setResultFormat( 'n' );

  if ( d->status.resultFormat )
    d->status.resultFormat->setText( tr( "Engineering decimal" ) );
}


void MainWindow::formatFixed()
{
  d->actionGroups.digits->setEnabled( true );
  setResultFormat( 'f' );

  if ( d->status.resultFormat )
    d->status.resultFormat->setText( tr( "Fixed decimal" ) );
}


void MainWindow::formatGeneral()
{
  d->actionGroups.digits->setEnabled( true );
  setResultFormat( 'g' );

  if ( d->status.resultFormat )
    d->status.resultFormat->setText( tr( "General decimal" ) );
}


void MainWindow::formatHexadec()
{
  d->actionGroups.digits->setDisabled( true );
  setResultFormat( 'h' );

  if ( d->status.resultFormat )
    d->status.resultFormat->setText( tr( "Hexadecimal" ) );
}


void MainWindow::formatOctal()
{
  d->actionGroups.digits->setDisabled( true );
  setResultFormat( 'o' );

  if ( d->status.resultFormat )
    d->status.resultFormat->setText( tr( "Octal" ) );
}


void MainWindow::formatScientific()
{
  d->actionGroups.digits->setEnabled( true );
  setResultFormat( 'e' );

  if ( d->status.resultFormat )
    d->status.resultFormat->setText( tr( "Scientific decimal" ) );
}


// protected slots

void MainWindow::activate()
{
  activateWindow();
  raise();
  d->widgets.editor->setFocus();
}


void MainWindow::constantSelected( const QString & c )
{
  if ( c.isEmpty() )
    return;

  QString str( c );
  str.replace( '.', d->widgets.editor->radixChar() );
  d->widgets.editor->insert( str );

  QTimer::singleShot( 0, d->widgets.editor, SLOT( setFocus() ) );

  if ( ! isActiveWindow () )
    activateWindow();
}


void MainWindow::expressionSelected( const QString & expr )
{
  QTextCursor cursor = d->widgets.editor->textCursor();
  d->widgets.editor->blockSignals( true );
  cursor.insertText( expr );
  d->widgets.editor->blockSignals( false );
  QTimer::singleShot( 0, d->widgets.editor, SLOT( setFocus() ) );
  d->widgets.editor->setTextCursor( cursor );

  if ( ! isActiveWindow () )
    activateWindow();
}


void MainWindow::functionSelected( const QString & e )
{
  if ( e.isEmpty() )
    return;

  d->widgets.editor->insert( e );
  d->widgets.editor->insert( "(" );

  QTimer::singleShot( 0, d->widgets.editor, SLOT( setFocus() ) );

  if ( ! isActiveWindow () )
    activateWindow();
}


void MainWindow::keypadButtonPressed( Keypad::Button b )
{
  d->widgets.editor->blockSignals( true ); // prevent completion
  switch ( b )
  {
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
      d->widgets.editor->insert( QString( d->widgets.keypad->radixChar() ) );
      break;

    case Keypad::KeyClear:
      clearExpression();
      break;

    default: break;
  }

  QTimer::singleShot( 0, d->widgets.editor, SLOT( setFocus() ) );
  d->widgets.editor->blockSignals( false );

  if ( b == Keypad::KeyEquals)
    d->widgets.editor->evaluate();
}


void MainWindow::minimizeToTray()
{
  if ( d->widgets.trayIcon )
  {
    hide();
    d->widgets.trayIcon->show();
    if ( d->conditions.trayNotify )
      QTimer::singleShot( 500, this, SLOT( showTrayMessage() ) );
    d->conditions.trayNotify = false;
  }
}


void MainWindow::Private::restoreVariables()
{
  for ( int k = 0; k < settings.variables.count(); k++ )
  {
    evaluator->setExpression( settings.variables[k] );
    evaluator->eval();
    QStringList list = settings.variables[k].split( "=" );
    evaluator->set( list[0], HNumber( list[1].toAscii().data() ) );
  }

  if ( docks.variables )
    docks.variables->updateList( evaluator );

  // free memory
  settings.variables.clear();
}


// THIS IS NOT NEEDED ANYMORE, BUT NEEDS TO BE TESTED WITH Qt 4.2
//void MainWindow::Private::restoreFloatingDocks()
//{
//  if ( settings.showBook && settings.bookDockFloating
//       && ! docks.book->isFloating() )
//  {
//    docks.book->hide();
//    docks.book->setFloating( true );
//    docks.book->move( settings.bookDockLeft, settings.bookDockTop );
//    docks.book->resize( settings.bookDockWidth, settings.bookDockHeight );
//    docks.book->show();
//  }
//
//  if ( settings.showHistory && settings.historyDockFloating
//       && ! docks.history->isFloating() )
//  {
//    docks.history->hide();
//    docks.history->setFloating( true );
//    docks.history->move( settings.historyDockLeft, settings.historyDockTop );
//    docks.history->resize( settings.historyDockWidth,
//                           settings.historyDockHeight );
//    docks.history->show();
//  }
//
//  if ( settings.showFunctions && settings.functionsDockFloating
//       && ! docks.functions->isFloating() )
//  {
//    docks.functions->hide();
//    docks.functions->setFloating( true );
//    docks.functions->move( settings.functionsDockLeft,
//                           settings.functionsDockTop );
//    docks.functions->resize( settings.functionsDockWidth,
//                             settings.functionsDockHeight );
//    docks.functions->show();
//  }
//
//  if ( settings.showVariables && settings.variablesDockFloating
//       && ! docks.variables->isFloating() )
//  {
//    docks.variables->hide();
//    docks.variables->setFloating( true );
//    docks.variables->move( settings.variablesDockLeft,
//                           settings.variablesDockTop );
//    docks.variables->resize( settings.variablesDockWidth,
//                             settings.variablesDockHeight );
//    docks.variables->show();
//  }
//
//  if ( settings.showConstants && settings.constantsDockFloating
//       && ! docks.constants->isFloating() )
//  {
//    docks.constants->hide();
//    docks.constants->setFloating( true );
//    docks.constants->move( settings.constantsDockLeft,
//                           settings.constantsDockTop );
//    docks.constants->resize( settings.constantsDockWidth,
//                             settings.constantsDockHeight );
//    docks.constants->show();
//  }
//}


void MainWindow::Private::restoreHistory()
{
  if ( settings.historyResults.count() != settings.history.count() )
  {
    p->clearHistory();
    return;
  }

  widgets.editor->setHistory( settings.history );
  widgets.editor->setHistoryResults( settings.historyResults );
  widgets.display->appendHistory( settings.history, settings.historyResults );

  if ( docks.history )
    docks.history->setHistory( widgets.editor->history() );

  // free some useless memory
  settings.history.clear();
  settings.historyResults.clear();
}


void MainWindow::returnPressed()
{
  QString str = d->evaluator->autoFix( d->widgets.editor->text() );
  if ( str.isEmpty() )
    return;

  d->evaluator->setExpression( str );

  HNumber result = d->evaluator->evalUpdateAns();
  if ( ! d->evaluator->error().isEmpty() )
  {
    QMessageBox::warning( this, tr( "Error" ), d->evaluator->error() );
  }
  else
  {
    d->widgets.display->append( str, result );
    char * num = HMath::formatScientific( result, DECPRECISION );
    d->widgets.editor->appendHistory( str, num );
    free( num );
    d->widgets.editor->setAnsAvailable( true );
    if ( d->settings.showVariables )
      d->docks.variables->updateList( d->evaluator );
    if ( d->settings.showHistory )
      d->docks.history->append( str );
  }

  d->widgets.editor->setText( str );
  d->widgets.editor->selectAll();
  d->widgets.editor->stopAutoCalc();
  d->widgets.editor->stopAutoComplete();
  d->conditions.autoAns = true;

  QTimer::singleShot( 0, d->widgets.editor, SLOT( setFocus() ) );

  if ( ! isActiveWindow() )
    activateWindow();
}


void MainWindow::showTrayMessage()
{
  QString msg = tr( "SpeedCrunch is minimized.\nLeft click the icon to "
                    "restore it or right click for options." );
  if ( menuBar()->layoutDirection() == Qt::RightToLeft )
    msg += QChar( 0x200E );
  if ( d->widgets.trayIcon )
    d->widgets.trayIcon->showMessage( QString(), msg, QSystemTrayIcon::NoIcon,
                                      4000 );
}


void MainWindow::textChanged()
{
  if ( d->conditions.autoAns )
  {
    QString expr = d->evaluator->autoFix( d->widgets.editor->text() );
    if ( expr.isEmpty() )
      return;

    Tokens tokens = d->evaluator->scan( expr );
    if ( tokens.count() == 1 )
    {
      if (    tokens[0].asOperator() == Token::Plus
           || tokens[0].asOperator() == Token::Minus
           || tokens[0].asOperator() == Token::Asterisk
           || tokens[0].asOperator() == Token::Slash
           || tokens[0].asOperator() == Token::Caret )
      {
        d->conditions.autoAns = false;
        expr.prepend( "ans" );
        d->widgets.editor->setText( expr );
        d->widgets.editor->setCursorPosition( expr.length() );
      }
    }
  }
}


void MainWindow::trayIconActivated( QSystemTrayIcon::ActivationReason r )
{
  if ( r == QSystemTrayIcon::Context )
  {
    d->menus.trayIcon->show();
  }
  else
  {
    showNormal();
    activateWindow();
    d->widgets.editor->setFocus();
    QTimer::singleShot( 0, d->widgets.trayIcon, SLOT( hide() ) );

    // work around docks do not reappear if floating
    if ( d->docks.history )
      if ( d->docks.history->isFloating() )
      {
        d->docks.history->hide();
        d->docks.history->show();
      }
    if ( d->docks.functions )
      if ( d->docks.functions->isFloating() )
      {
        d->docks.functions->hide();
        d->docks.functions->show();
      }
    if ( d->docks.variables )
      if ( d->docks.variables->isFloating() )
      {
        d->docks.variables->hide();
        d->docks.variables->show();
      }
    if ( d->docks.constants )
      if ( d->docks.constants->isFloating() )
      {
        d->docks.constants->hide();
        d->docks.constants->show();
      }
  }
}


void MainWindow::variableSelected( const QString & v )
{
  if ( v.isEmpty() )
    return;

  d->widgets.editor->blockSignals( true );
  d->widgets.editor->insert( v );
  d->widgets.editor->blockSignals( false );

  QTimer::singleShot( 0, d->widgets.editor, SLOT( setFocus() ) );

  if ( ! isActiveWindow () )
    activateWindow();
}


void MainWindow::radixCharAutoTriggered()
{
  setRadixChar( 0 );
}


void MainWindow::radixCharDotTriggered()
{
  setRadixChar( '.' );
}


void MainWindow::radixCharCommaTriggered()
{
  setRadixChar( ',' );
}


// protected

void MainWindow::closeEvent( QCloseEvent * e )
{
  d->saveSettings();
  e->accept();
}


void MainWindow::setResultPrecision( int p )
{
  if ( d->settings.resultPrecision == p )
    return;

  d->settings.resultPrecision = p;
  emit resultPrecisionChanged( p );
}


void MainWindow::setResultFormat( char c )
{
  if ( d->settings.resultFormat == c )
    return;

  d->settings.resultFormat = c;
  emit resultFormatChanged( c );
}


void MainWindow::setRadixChar( char c )
{
  char oldRadixChar = d->settings.getRadixChar();
  d->settings.setRadixChar( c );
  c = d->settings.getRadixChar();
  if ( oldRadixChar != c )
    emit radixCharChanged( c );
}


void MainWindow::changeLanguage()
{
  QAction * a = d->actionGroups.language->checkedAction();
  QString lang = a->data().toString();
  if ( d->settings.language != lang )
    d->settings.language = lang;

  emit languageChanged();
}
