/* This file is part of the SpeedCrunch project
   Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
                 2005-2006 Johan Thelin <e8johan@gmail.com>
                 2007 Helder Correia <helder.pereira.correia@gmail.com>

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

#include "crunch.h"

#include "autohidelabel.h"
#include "constants.h"
#include "constantsdock.h"
#include "evaluator.h"
#include "historydock.h"
#include "hmath.h"
#include "editor.h"
#include "functions.h"
#include "functionsdock.h"
#include "result.h"
#include "settings.h"
#include "tipwidget.h"
#include "variablesdock.h"

#include "insertfunctiondlg.h"
#include "insertvardlg.h"
#include "deletevardlg.h"
#include "configdlg.h"
#include "aboutbox.h"
#include "keypad.h"

#include <stdlib.h> // rand()

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QRadioButton>
#include <QSystemTrayIcon>
#include <QTextStream>
#include <QTimer>
#include <QToolTip>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>

class CrunchActions
{
public:
  QAction* sessionSave;
  QAction* sessionQuit;
  QAction* editPaste;
  QAction* editCopy;
  QAction* editCopyResult;
  QAction* insertFunction;
  QAction* insertVariable;
  QAction* deleteVariable;
  QAction* clearInput;
  QAction* clearDisplay;
  QAction* clearHistory;
  QAction* clearVariables;
  QAction* viewGeneral;
  QAction* viewFixed;
  QAction* viewEngineering;
  QAction* viewScientific;
  QAction* viewHexadec;
  QAction* viewOctal;
  QAction* viewBinary;
  QAction* digitsAuto;
  QAction* digits2;
  QAction* digits3;
  QAction* digits8;
  QAction* digits15;
  QAction* digits50;
  QAction* focusAndSelectInput;
  QAction* showClearButton;
  QAction* showEvalButton;
  QAction* showKeyPad;
  QAction* showHistory;
  QAction* showFunctions;
  QAction* showVariables;
  QAction* showConstants;
  QAction* configure;
  QAction* helpTipOfTheDay;
  QAction* helpGotoWebsite;
  QAction* helpAboutQt;
  QAction* helpAbout;
};


class CrunchPrivate
{
public:
  CrunchActions* actions;
  QActionGroup *digitsGroup;
  Evaluator* eval;

  Editor *editor;
  AutoHideLabel* autoCalcLabel;
  TipWidget* tip;

  Result* result;
  KeyPad* keypad;
  QPushButton* clearInputButton;

  QPushButton* evalButton;
  QRadioButton* degButton;
  QRadioButton* radButton;

  QRadioButton* hexButton;
  QRadioButton* decButton;
  QRadioButton* octButton;
  QRadioButton* binButton;

  bool autoAns;

  QSystemTrayIcon* trayIcon;
  bool trayNotify;

  HistoryDock* historyDock;
  FunctionsDock* functionsDock;
  VariablesDock* variablesDock;
  ConstantsDock* constantsDock;

  ConfigDlg* configDlg;
  InsertFunctionDlg* insertFunctionDlg;
  InsertVariableDlg* insertVariableDlg;
  DeleteVariableDlg* deleteVariableDlg;
};

Crunch::Crunch(): QMainWindow()
{
  d = new CrunchPrivate;
  d->actions = new CrunchActions;

  d->eval = new Evaluator;

  d->autoAns = false;
  d->trayIcon = 0;
  d->trayNotify = true;

  // Outer widget and layout

  QWidget *box = new QWidget( this );
  setCentralWidget( box );

  QVBoxLayout *outerBoxLayout = new QVBoxLayout( box );
  outerBoxLayout->setMargin( 5 );
  outerBoxLayout->setSpacing( 0 );

  // Top layout for deg/rad buttons

  QHBoxLayout *topboxLayout = new QHBoxLayout();

  QWidget* radixBox = new QWidget( this );
  QHBoxLayout *radixLayout = new QHBoxLayout();
  radixLayout->setMargin( 0 );
  radixBox->setLayout( radixLayout );
  d->hexButton = new QRadioButton( tr( "Hex" ), radixBox );
  d->decButton = new QRadioButton( tr( "Dec" ), radixBox );
  d->octButton = new QRadioButton( tr( "Oct" ), radixBox );
  d->binButton = new QRadioButton( tr( "Bin" ), radixBox );
  d->hexButton->setFocusPolicy( Qt::ClickFocus );
  d->decButton->setFocusPolicy( Qt::ClickFocus );
  d->octButton->setFocusPolicy( Qt::ClickFocus );
  d->binButton->setFocusPolicy( Qt::ClickFocus );
  connect( d->hexButton, SIGNAL( toggled( bool ) ), SLOT( radixChanged() ) );
  connect( d->decButton, SIGNAL( toggled( bool ) ), SLOT( radixChanged() ) );
  connect( d->octButton, SIGNAL( toggled( bool ) ), SLOT( radixChanged() ) );
  connect( d->binButton, SIGNAL( toggled( bool ) ), SLOT( radixChanged() ) );
  radixLayout->addWidget( d->hexButton );
  radixLayout->addWidget( d->decButton );
  radixLayout->addWidget( d->octButton );
  radixLayout->addWidget( d->binButton );

  QSpacerItem *spacer = new QSpacerItem( 50, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
  topboxLayout->addWidget( radixBox );
  topboxLayout->addItem( spacer );

  QWidget* angleBox = new QWidget( this );
  d->degButton = new QRadioButton( tr( "&Degrees" ), angleBox );
  d->radButton = new QRadioButton( tr( "&Radians" ), angleBox );
  d->degButton->setFocusPolicy( Qt::ClickFocus );
  d->radButton->setFocusPolicy( Qt::ClickFocus );
  connect( d->degButton, SIGNAL( toggled( bool ) ), SLOT( angleModeChanged() ) );
  connect( d->radButton, SIGNAL( toggled( bool ) ), SLOT( angleModeChanged() ) );
  topboxLayout->addWidget( d->degButton );
  topboxLayout->addWidget( d->radButton );

  outerBoxLayout->addLayout( topboxLayout );



  // Result list

  d->result = new Result( box );
  outerBoxLayout->addWidget( d->result );

  // Layout for editor and evaluation button

  QHBoxLayout *inputBoxLayout = new QHBoxLayout();
  inputBoxLayout->setMargin( 5 );
  inputBoxLayout->setSpacing( 5 );

  d->clearInputButton = new QPushButton( box );
  d->clearInputButton->setMaximumWidth( 25 );
  d->clearInputButton->setFlat( true );
  d->clearInputButton->setIcon( QPixmap(":/clearinput.png") );
  d->clearInputButton->hide();
  d->clearInputButton->setToolTip( tr("Clear input line") );
  inputBoxLayout->addWidget( d->clearInputButton );

  d->editor = new Editor( d->eval, box );
  d->editor->setFocus();
  inputBoxLayout->addWidget( d->editor );

  d->evalButton = new QPushButton( box );
  d->evalButton->setText( tr("Evaluate" ) );
  d->evalButton->hide();
  inputBoxLayout->addWidget( d->evalButton );

  outerBoxLayout->addLayout( inputBoxLayout );


  // Keypad

  QHBoxLayout *keypadLayout = new QHBoxLayout();

  d->keypad = new KeyPad( box );
  d->keypad->setFocusPolicy( Qt::NoFocus );
  d->keypad->hide();

  keypadLayout->addStretch();
  keypadLayout->addWidget( d->keypad );
  keypadLayout->addStretch();

  outerBoxLayout->addLayout( keypadLayout );

  // Docks

  d->historyDock = new HistoryDock( this );
  d->historyDock->setObjectName( "HistoryWindow" );
  addDockWidget( Qt::RightDockWidgetArea, d->historyDock );

  d->functionsDock = new FunctionsDock( this );
  d->functionsDock->setObjectName( "FunctionsList" );
  addDockWidget( Qt::RightDockWidgetArea, d->functionsDock );

  d->variablesDock = new VariablesDock( this );
  d->variablesDock->setObjectName( "VariablesList" );
  addDockWidget( Qt::RightDockWidgetArea, d->variablesDock );

  d->constantsDock = new ConstantsDock( this );
  d->constantsDock->setObjectName( "ConstantsList" );
  d->constantsDock->update( Constants::self() );
  addDockWidget( Qt::RightDockWidgetArea, d->constantsDock );

  tabifyDockWidget( d->functionsDock, d->historyDock );
  tabifyDockWidget( d->functionsDock, d->variablesDock );
  tabifyDockWidget( d->functionsDock, d->constantsDock );
  d->historyDock->hide();
  d->functionsDock->hide();
  d->variablesDock->hide();
  d->constantsDock->hide();

  // for autocalc
  d->autoCalcLabel = new AutoHideLabel( this );
  d->autoCalcLabel->hide();

  // for tip of the day
  d->tip = new TipWidget( this );
  d->tip->hide();

  // Connect signals and slots

  connect( d->clearInputButton, SIGNAL( clicked() ), SLOT( clearInput() ) );
  connect( d->evalButton, SIGNAL( clicked() ), SLOT( returnPressed() ) );
  connect( d->editor, SIGNAL( returnPressed() ), SLOT( returnPressed() ) );
  connect( d->editor, SIGNAL( textChanged() ), SLOT( textChanged() ) );
  connect( d->editor, SIGNAL( autoCalcDeactivated() ), SLOT( hideAutoCalc() ) );
  connect( d->editor, SIGNAL( autoCalcActivated( const QString& ) ), SLOT( showAutoCalc( const QString& ) ) );
  connect( d->result, SIGNAL( textCopied( const QString& ) ), d->editor, SLOT( paste() ) );
  connect( d->result, SIGNAL( textCopied( const QString& ) ), d->editor, SLOT( setFocus() ) );
  connect( d->historyDock, SIGNAL( expressionSelected( const QString& ) ), SLOT( expressionSelected( const QString& ) ) );
  connect( d->functionsDock, SIGNAL( functionSelected( const QString& ) ), SLOT( functionSelected( const QString& ) ) );
  connect( d->variablesDock, SIGNAL( variableSelected( const QString& ) ), SLOT( variableSelected( const QString& ) ) );
  connect( d->constantsDock, SIGNAL( constantSelected( const QString& ) ), SLOT( constantSelected( const QString& ) ) );


  connect( d->keypad, SIGNAL( addText( const QString& ) ), SLOT( addKeyPadText( const QString& ) ) );

  // Initialize settings

  d->configDlg = 0;
  d->insertFunctionDlg = 0;
  d->insertVariableDlg = 0;
  d->deleteVariableDlg = 0;

  setWindowTitle( tr( "SpeedCrunch" ) );
  d->degButton->setChecked( true );
  createUI();
  applySettings();
  restoreDocks();

  QTimer::singleShot( 0, this, SLOT(activate()) );
}

Crunch::~Crunch()
{
  delete d->actions;
  delete d->eval;
  delete d;
}

void Crunch::createUI()
{
  // create all the actions
  d->actions->sessionSave = new QAction( tr("&Save..."), this );
    d->actions->sessionSave->setShortcut( Qt::CTRL + Qt::Key_S );
  d->actions->sessionQuit = new QAction( tr("&Quit"), this );
    d->actions->sessionQuit->setShortcut( Qt::CTRL + Qt::Key_Q );

  d->actions->focusAndSelectInput = new QAction( tr("&Select Input"), this );
    d->actions->focusAndSelectInput->setShortcut( Qt::Key_F6 );

  d->actions->editCopy = new QAction( tr("&Copy"), this );
    d->actions->editCopy->setShortcut( Qt::CTRL + Qt::Key_C );
  d->actions->editPaste = new QAction( tr("&Paste"), this );
    d->actions->editPaste->setShortcut( Qt::CTRL + Qt::Key_V );
  d->actions->editCopyResult = new QAction( tr("Copy &Result"), this );
    d->actions->editCopyResult->setShortcut( Qt::CTRL + Qt::Key_R );
  d->actions->insertFunction = new QAction( tr("Insert &Function..."), this );
    d->actions->insertFunction->setShortcut( Qt::CTRL + Qt::Key_F );
  d->actions->insertVariable = new QAction( tr("Insert &Variable..."), this );
    d->actions->insertVariable->setShortcut( Qt::CTRL + Qt::Key_I );
  d->actions->deleteVariable = new QAction( tr("D&elete Variable..."), this );
    d->actions->deleteVariable->setShortcut( Qt::CTRL + Qt::Key_D );

  d->actions->clearInput = new QAction( tr("Clear &Input" ), this );
    d->actions->clearInput->setShortcut( Qt::Key_Escape );
  d->actions->clearDisplay = new QAction( tr("Clear &Display" ), this );
  d->actions->clearHistory = new QAction( tr("Clear &History" ), this );
  d->actions->clearVariables = new QAction( tr("Clear V&ariables" ), this );

  d->actions->viewGeneral = new QAction( tr("&General"), 0 );
  d->actions->viewFixed = new QAction( tr("&Fixed Decimal"), 0 );
  d->actions->viewEngineering = new QAction( tr("&Engineering"), 0 );
  d->actions->viewScientific = new QAction( tr("&Scientific"), 0 );
  d->actions->viewHexadec = new QAction( tr("&Hexadecimal"), 0 );
  d->actions->viewOctal = new QAction( tr("&Octal"), 0 );
  d->actions->viewBinary = new QAction( tr("&Binary"), 0 );

  QActionGroup *formatGroup = new QActionGroup( this );
  formatGroup->addAction( d->actions->viewGeneral );
  formatGroup->addAction( d->actions->viewFixed );
  formatGroup->addAction( d->actions->viewEngineering );
  formatGroup->addAction( d->actions->viewScientific );
  formatGroup->addAction( d->actions->viewHexadec );
  formatGroup->addAction( d->actions->viewOctal );
  formatGroup->addAction( d->actions->viewBinary );

  d->actions->viewGeneral->setCheckable( true );
  d->actions->viewFixed->setCheckable( true );
  d->actions->viewEngineering->setCheckable( true );
  d->actions->viewScientific->setCheckable( true );
  d->actions->viewHexadec->setCheckable( true );
  d->actions->viewOctal->setCheckable( true );
  d->actions->viewBinary->setCheckable( true );

  d->actions->digitsAuto = new QAction( tr("&Automatic Precision"), 0 );
  d->actions->digits2 = new QAction( tr("&2 Decimal Digits"), 0 );
  d->actions->digits3 = new QAction( tr("&3 Decimal Digits"), 0 );
  d->actions->digits8 = new QAction( tr("&8 Decimal Digits"), 0 );
  d->actions->digits15 = new QAction( tr("&15 Decimal Digits"), 0 );
  d->actions->digits50 = new QAction( tr("&50 Decimal Digits"), 0 );

  d->digitsGroup = new QActionGroup( this );
  d->digitsGroup->addAction( d->actions->digitsAuto );
  d->digitsGroup->addAction( d->actions->digits2 );
  d->digitsGroup->addAction( d->actions->digits3 );
  d->digitsGroup->addAction( d->actions->digits8 );
  d->digitsGroup->addAction( d->actions->digits15 );
  d->digitsGroup->addAction( d->actions->digits50 );

  d->actions->digitsAuto->setCheckable( true );
  d->actions->digits2->setCheckable( true );
  d->actions->digits3->setCheckable( true );
  d->actions->digits8->setCheckable( true );
  d->actions->digits15->setCheckable( true );
  d->actions->digits50->setCheckable( true );

  d->actions->showClearButton = new QAction( tr("&Show Clear Button"), this );
  d->actions->showEvalButton = new QAction( tr("Show &Evaluate Button"), this );
  d->actions->showKeyPad = new QAction( tr("Show &Key Pad"), this );
  d->actions->showHistory = new QAction( tr("Show Expression &History"), this );
  d->actions->showFunctions = new QAction( tr("Show &Functions List"), this );
  d->actions->showVariables = new QAction( tr("Show &Variables List"), this );
  d->actions->showConstants = new QAction( tr("Show C&onstants List"), this );

  d->actions->showClearButton->setCheckable( true );
  d->actions->showEvalButton->setCheckable( true );
  d->actions->showKeyPad->setCheckable( true );
  d->actions->showHistory->setCheckable( true );
  d->actions->showFunctions->setCheckable( true );
  d->actions->showVariables->setCheckable( true );
  d->actions->showConstants->setCheckable( true );

  d->actions->configure = new QAction( tr("&Configure..."), this );

  d->actions->helpTipOfTheDay = new QAction( tr("&Tip of the Day"), this );
  d->actions->helpGotoWebsite = new QAction( tr("SpeedCrunch &Web Site..."), this );
  d->actions->helpAbout = new QAction( tr("&About"), this );
  d->actions->helpAboutQt = new QAction( tr("About &Qt"), this );

  // signal/slot
  connect( d->actions->sessionSave, SIGNAL( activated() ), this, SLOT( saveSession() ) );
  connect( d->actions->sessionQuit, SIGNAL( activated() ), this, SLOT( close() ) );
  connect( d->actions->editPaste, SIGNAL( activated() ), d->editor, SLOT( paste() ) );
  connect( d->actions->editCopy, SIGNAL( activated() ), d->editor, SLOT( copy() ) );
  connect( d->actions->editCopyResult, SIGNAL( activated() ), this, SLOT( copyResult() ) );
  connect( d->actions->focusAndSelectInput, SIGNAL( activated() ), this, SLOT( focusAndSelectInput() ) );
  connect( d->actions->clearInput, SIGNAL( activated() ), this, SLOT( clearInput() ) );
  connect( d->actions->clearDisplay, SIGNAL( activated() ), d->result, SLOT( clear() ) );
  connect( d->actions->clearHistory, SIGNAL( activated() ), d->editor, SLOT( clearHistory() ) );
  connect( d->actions->clearHistory, SIGNAL( activated() ), d->historyDock, SLOT( clear() ) );
  connect( d->actions->clearVariables, SIGNAL( activated() ), this, SLOT( clearVariables() ) );
  connect( d->actions->insertFunction, SIGNAL( activated() ), this, SLOT( insertFunction() ) );
  connect( d->actions->insertVariable, SIGNAL( activated() ), this, SLOT( insertVariable() ) );
  connect( d->actions->deleteVariable, SIGNAL( activated() ), this, SLOT( deleteVariable() ) );
  connect( d->actions->viewGeneral, SIGNAL( activated() ), this, SLOT( viewGeneral() ) );
  connect( d->actions->viewFixed, SIGNAL( activated() ), this, SLOT( viewFixed() ) );
  connect( d->actions->viewEngineering, SIGNAL( activated() ), this, SLOT( viewEngineering() ) );
  connect( d->actions->viewScientific, SIGNAL( activated() ), this, SLOT( viewScientific() ) );
  connect( d->actions->viewHexadec, SIGNAL( activated() ), this, SLOT( viewHexadec() ) );
  connect( d->actions->viewOctal, SIGNAL( activated() ), this, SLOT( viewOctal() ) );
  connect( d->actions->viewBinary, SIGNAL( activated() ), this, SLOT( viewBinary() ) );
  connect( d->actions->digitsAuto, SIGNAL( activated() ), this, SLOT( digitsAuto() ) );
  connect( d->actions->digits2, SIGNAL( activated() ), this, SLOT( digits2() ) );
  connect( d->actions->digits3, SIGNAL( activated() ), this, SLOT( digits3() ) );
  connect( d->actions->digits8, SIGNAL( activated() ), this, SLOT( digits8() ) );
  connect( d->actions->digits15, SIGNAL( activated() ), this, SLOT( digits15() ) );
  connect( d->actions->digits50, SIGNAL( activated() ), this, SLOT( digits50() ) );
  connect( d->actions->showClearButton, SIGNAL( toggled(bool) ), this, SLOT( showClearButton(bool) ) );
  connect( d->actions->showEvalButton, SIGNAL( toggled(bool) ), this, SLOT( showEvalButton(bool) ) );
  connect( d->actions->showKeyPad, SIGNAL( toggled(bool) ), this, SLOT( showKeyPad(bool) ) );
  connect( d->actions->showHistory, SIGNAL( toggled(bool) ), this, SLOT( showHistory(bool) ) );
  connect( d->actions->showFunctions, SIGNAL( toggled(bool) ), this, SLOT( showFunctions(bool) ) );
  connect( d->actions->showVariables, SIGNAL( toggled(bool) ), this, SLOT( showVariables(bool) ) );
  connect( d->actions->showConstants, SIGNAL( toggled(bool) ), this, SLOT( showConstants(bool) ) );
  connect( d->actions->configure, SIGNAL( activated() ), this, SLOT( configure() ) );
  connect( d->actions->helpTipOfTheDay, SIGNAL( activated() ), this, SLOT( showTip() ) );
  connect( d->actions->helpGotoWebsite, SIGNAL( activated() ), this, SLOT( gotoWebsite() ) );
  connect( d->actions->helpAbout, SIGNAL( activated() ), this, SLOT( about() ) );
  connect( d->actions->helpAboutQt, SIGNAL( activated() ), this, SLOT( aboutQt() ) );

  // synchronize dock actions
  connect( d->historyDock->toggleViewAction(), SIGNAL( toggled( bool ) ), d->actions->showHistory, SLOT( setChecked( bool ) ) );
  connect( d->functionsDock->toggleViewAction(), SIGNAL( toggled( bool ) ), d->actions->showFunctions, SLOT( setChecked( bool ) ) );
  connect( d->variablesDock->toggleViewAction(), SIGNAL( toggled( bool ) ), d->actions->showVariables, SLOT( setChecked( bool ) ) );
  connect( d->constantsDock->toggleViewAction(), SIGNAL( toggled( bool ) ), d->actions->showConstants, SLOT( setChecked( bool ) ) );

  // construct the menu

  QMenu *sessionMenu = new QMenu( tr("&Session"), this );
  menuBar()->addMenu( sessionMenu );
  sessionMenu->addAction( d->actions->sessionSave );
  sessionMenu->addAction( d->actions->sessionQuit );

  QMenu *editMenu = new QMenu( tr("&Edit"), this );
  menuBar()->addMenu( editMenu );
  editMenu->addAction( d->actions->editCopy );
  editMenu->addAction( d->actions->editCopyResult );
  editMenu->addAction( d->actions->editPaste );
  editMenu->addSeparator();
  editMenu->addAction( d->actions->insertFunction );
  editMenu->addAction( d->actions->insertVariable );
  editMenu->addSeparator();
  editMenu->addAction( d->actions->deleteVariable );
  editMenu->addSeparator();
  editMenu->addAction( d->actions->clearInput );
  editMenu->addAction( d->actions->clearDisplay );
  editMenu->addAction( d->actions->clearHistory );
  editMenu->addAction( d->actions->clearVariables );
  editMenu->addSeparator();
  editMenu->addAction( d->actions->focusAndSelectInput );

  QMenu *viewMenu = new QMenu( tr("&View"), this );
  menuBar()->addMenu( viewMenu );
  viewMenu->addAction( d->actions->viewGeneral );
  viewMenu->addAction( d->actions->viewFixed );
  viewMenu->addAction( d->actions->viewEngineering );
  viewMenu->addAction( d->actions->viewScientific );
  viewMenu->addAction( d->actions->viewHexadec );
  viewMenu->addAction( d->actions->viewOctal );
  viewMenu->addAction( d->actions->viewBinary );
  viewMenu->addSeparator();
  viewMenu->addAction( d->actions->digitsAuto );
  viewMenu->addAction( d->actions->digits2 );
  viewMenu->addAction( d->actions->digits3 );
  viewMenu->addAction( d->actions->digits8 );
  viewMenu->addAction( d->actions->digits15 );
  viewMenu->addAction( d->actions->digits50 );


  QMenu *settingsMenu = new QMenu( tr("Se&ttings"), this );
  menuBar()->addMenu( settingsMenu );
  settingsMenu->addAction( d->actions->showClearButton );
  settingsMenu->addAction( d->actions->showEvalButton );
  settingsMenu->addAction( d->actions->showKeyPad );
  settingsMenu->addSeparator();
  settingsMenu->addAction( d->actions->showHistory );
  settingsMenu->addAction( d->actions->showFunctions );
  settingsMenu->addAction( d->actions->showVariables );
  settingsMenu->addAction( d->actions->showConstants );
  settingsMenu->addSeparator();
  settingsMenu->addAction( d->actions->configure );

  QMenu *helpMenu = new QMenu( tr("&Help"), this );
  menuBar()->addMenu( helpMenu );
  helpMenu->addAction( d->actions->helpTipOfTheDay );
  helpMenu->addAction( d->actions->helpGotoWebsite );
  helpMenu->addSeparator();
  helpMenu->addAction( d->actions->helpAbout );
  helpMenu->addAction( d->actions->helpAboutQt );

  setWindowIcon( QPixmap( ":/crunch.png" ) );

  Settings::self()->load();
}

void Crunch::applySettings()
{
  Settings* settings = Settings::self();
  settings->load();

  if( settings->mainWindowSize != QSize( 0, 0 ) )
    resize( settings->mainWindowSize );

  d->editor->setAutoCompleteEnabled( settings->autoComplete );
  d->editor->setAutoCalcEnabled( settings->autoCalc );
  d->editor->setSyntaxHighlight( settings->enableSyntaxHighlight );
  d->editor->setHighlightColor( Editor::Number, settings->highlightNumberColor );
  d->editor->setHighlightColor( Editor::FunctionName, settings->highlightFunctionColor );
  d->editor->setHighlightColor( Editor::Variable, settings->highlightVariableColor );
  d->editor->setHighlightColor( Editor::MatchedPar, settings->matchedParenthesisColor );

  if( settings->angleMode == "degree" )
  {
    d->eval->setAngleMode( Evaluator::Degree );
    d->degButton->setChecked( true );
    d->radButton->setChecked( false );
  }

  if( settings->angleMode == "radian" )
  {
    d->eval->setAngleMode( Evaluator::Radian );
    d->degButton->setChecked( false );
    d->radButton->setChecked( true );
  }

  d->eval->setDecimalPoint( settings->decimalPoint );
  d->result->setDecimalPoint( settings->decimalPoint );

  d->historyDock->clear();
  if( settings->saveHistory )
  if( settings->history.count() )
  {
    d->editor->setHistory( settings->history );
    d->historyDock->setHistory( settings->history );
  }

  if( settings->saveVariables )
  {
    for( int k=0; k<settings->variables.count(); k++ )
    {
      d->eval->setExpression( settings->variables[k] );
      d->eval->eval();
    }
    d->variablesDock->updateList( d->eval );
  }

  switch( settings->format )
  {
    case 'h':
      d->hexButton->setChecked( true ); break;
    case 'o':
      d->octButton->setChecked( true ); break;
    case 'b':
      d->binButton->setChecked( true ); break;
    case 'f':
    case 'n':
    case 'e':
    case 'g':
    default:
      d->decButton->setChecked( true ); break;
  }

  d->result->setFormat( settings->format );
  d->result->setDecimalDigits( settings->decimalDigits );
  d->editor->setFormat( settings->format );
  d->editor->setDecimalDigits( settings->decimalDigits );

  if( settings->customAppearance )
  {
    d->result->setFont( settings->customFont );
    d->editor->setFont( settings->customFont );
	d->editor->adjustSize();
	d->editor->setFixedHeight( d->editor->sizeHint().height() );
  }
  else
  {
    d->result->setFont( QApplication::font( d->result ) );
    d->editor->setFont( QApplication::font( d->editor ) );
	d->editor->setFixedHeight( d->editor->sizeHint().height() );
  }

  d->result->setCustomAppearance( settings->customAppearance );
  d->result->setCustomTextColor( settings->customTextColor );
  d->result->setCustomBackgroundColor( settings->customBackgroundColor1,
    settings->customBackgroundColor2 );
  d->result->setCustomErrorColor( settings->customErrorColor );

  if( settings->format == 'g' ) d->actions->viewGeneral->setChecked( true );
  if( settings->format == 'f' ) d->actions->viewFixed->setChecked( true );
  if( settings->format == 'n' ) d->actions->viewEngineering->setChecked( true );
  if( settings->format == 'e' ) d->actions->viewScientific->setChecked( true );
  if( settings->format == 'h' ) d->actions->viewHexadec->setChecked( true );
  if( settings->format == 'o' ) d->actions->viewOctal->setChecked( true );
  if( settings->format == 'b' ) d->actions->viewBinary->setChecked( true );

  if( settings->decimalDigits < 0 ) d->actions->digitsAuto->setChecked( true );
  if( settings->decimalDigits == 2 ) d->actions->digits2->setChecked( true );
  if( settings->decimalDigits == 3 ) d->actions->digits3->setChecked( true );
  if( settings->decimalDigits == 8 ) d->actions->digits8->setChecked( true );
  if( settings->decimalDigits == 15 ) d->actions->digits15->setChecked( true );
  if( settings->decimalDigits == 50 ) d->actions->digits50->setChecked( true );

  if( settings->showClearInputButton )
    d->clearInputButton->show();
  else
    d->clearInputButton->hide();
  d->actions->showClearButton->setChecked( settings->showClearInputButton );

  if( settings->showEvaluateButton )
    d->evalButton->show();
  else
    d->evalButton->hide();
  d->actions->showEvalButton->setChecked( settings->showEvaluateButton );

  if( settings->showKeyPad )
    d->keypad->show();
  else
    d->keypad->hide();
  d->actions->showKeyPad->setChecked( settings->showKeyPad );

  d->actions->showHistory->setChecked( settings->showHistory );
  d->actions->showFunctions->setChecked( settings->showFunctions );
  d->actions->showVariables->setChecked( settings->showVariables );
  d->actions->showConstants->setChecked( settings->showConstants );

  d->historyDock->setVisible( settings->showHistory );
  d->functionsDock->setVisible( settings->showFunctions );
  d->variablesDock->setVisible( settings->showVariables );
  d->constantsDock->setVisible( settings->showConstants );

  if( settings->minimizeToTray )
  {
    if( !d->trayIcon && QSystemTrayIcon::isSystemTrayAvailable() )
    {
      d->trayNotify = true;
      d->trayIcon = new QSystemTrayIcon( this );
      d->trayIcon->setToolTip( tr("SpeedCrunch") );
      d->trayIcon->setIcon( QPixmap( ":/crunch.png" ) );
      connect( d->trayIcon, SIGNAL( activated(QSystemTrayIcon::ActivationReason) ),
        SLOT( trayIconActivated() ) );
    }
  }
  else
  {
    if( d->trayIcon )
	  delete d->trayIcon;
    d->trayIcon = 0;
  }

  // changed settings should trigger auto calc nor auto complete
  d->editor->stopAutoCalc();
  d->editor->stopAutoComplete();
}

bool Crunch::event( QEvent* e )
{
  if( e->type() == QEvent::WindowStateChange )
  {
    if( windowState() & Qt::WindowMinimized )
    if( Settings::self()->minimizeToTray )
      QTimer::singleShot( 100, this, SLOT(minimizeToTray()) );
  }

  return QMainWindow::event( e );
}


void Crunch::closeEvent( QCloseEvent* e )
{
  if(d->trayIcon)
    d->trayIcon->hide();
  saveSettings();
  saveDocks();
  QMainWindow::closeEvent( e );
}

void Crunch::saveSession()
{
  QString filters = tr( "Text Files (*.txt);; All Files (*.*)" );
  QString fname = QFileDialog::getSaveFileName( this, tr("Save Session"),
    QString::null, filters );
  if( fname.isEmpty() ) return;

  QFile file( fname );
  if( !file.open( QIODevice::WriteOnly ) )
  {
    QMessageBox::critical( this, tr("Error"), tr("Can't write to file %1").arg( fname ) );
    return;
  }

  QTextStream stream( &file );
  stream << d->result->asText();

  file.close();
}

void Crunch::saveSettings()
{
  Settings* settings = Settings::self();

  settings->mainWindowSize = size();

  if( d->eval->angleMode()== Evaluator::Degree )
    settings->angleMode = "degree";
  if( d->eval->angleMode()== Evaluator::Radian )
    settings->angleMode = "radian";

  settings->decimalPoint = d->eval->decimalPoint();

  if( settings->saveHistory )
    settings->history = d->editor->history();

  if( settings->saveVariables )
  {
    settings->variables.clear();
    QVector<Variable> vars = d->eval->variables();
    for( int i=0; i<vars.count(); i++ )
      if(    vars[i].name != "pi"
	  && vars[i].name != "phi" )
      {
        QString name;
	int length = vars[i].name.length();
	for ( int c = 0; c < length; c++ )
	{
	  QChar letter = vars[i].name[c];
	  if ( letter.isUpper() )
	    name += '_'; // escape code meaning upper case letter
	  name += vars[i].name[c].toLower();
	}
        char * value = HMath::formatFixed( vars[i].value, 100 );
        settings->variables.append(
	  QString("%1=%2").arg( name ).arg( QString( value ) ) );
        free( value );
      }
  }

  settings->save();
}

void Crunch::saveDocks()
{
  Settings* settings = Settings::self();

  settings->mainWindowState = saveState();

  settings->historyDockFloating = d->historyDock->isFloating();
  settings->historyDockLeft = d->historyDock->x();
  settings->historyDockTop = d->historyDock->y();
  settings->historyDockWidth = d->historyDock->width();
  settings->historyDockHeight = d->historyDock->height();

  settings->functionsDockFloating = d->functionsDock->isFloating();
  settings->functionsDockLeft = d->functionsDock->x();
  settings->functionsDockTop = d->functionsDock->y();
  settings->functionsDockWidth = d->functionsDock->width();
  settings->functionsDockHeight = d->functionsDock->height();

  settings->variablesDockFloating = d->variablesDock->isFloating();
  settings->variablesDockLeft = d->variablesDock->x();
  settings->variablesDockTop = d->variablesDock->y();
  settings->variablesDockWidth = d->variablesDock->width();
  settings->variablesDockHeight = d->variablesDock->height();

  settings->constantsDockFloating = d->constantsDock->isFloating();
  settings->constantsDockLeft = d->constantsDock->x();
  settings->constantsDockTop = d->constantsDock->y();
  settings->constantsDockWidth = d->constantsDock->width();
  settings->constantsDockHeight = d->constantsDock->height();

  settings->save();
}

void Crunch::restoreDocks()
{
  Settings* settings = Settings::self();

  restoreState( settings->mainWindowState );

  if( settings->showHistory )
  if( settings->historyDockFloating )
  if( !d->historyDock->isFloating() )
  {
    d->historyDock->hide();
    d->historyDock->setFloating( true );
    d->historyDock->move( settings->historyDockLeft, settings->historyDockTop );
    d->historyDock->resize( settings->historyDockWidth, settings->historyDockHeight );
    QTimer::singleShot(0, d->historyDock, SLOT(show()));
  }

  if( settings->showFunctions )
  if( settings->functionsDockFloating )
  if( !d->functionsDock->isFloating() )
  {
    d->functionsDock->hide();
    d->functionsDock->setFloating( true );
    d->functionsDock->move( settings->functionsDockLeft, settings->functionsDockTop );
    d->functionsDock->resize( settings->functionsDockWidth, settings->functionsDockHeight );
    QTimer::singleShot(0, d->functionsDock, SLOT(show()));
  }

  if( settings->showVariables )
  if( settings->variablesDockFloating )
  if( !d->variablesDock->isFloating() )
  {
    d->variablesDock->hide();
    d->variablesDock->setFloating( true );
    d->variablesDock->move( settings->variablesDockLeft, settings->variablesDockTop );
    d->variablesDock->resize( settings->variablesDockWidth, settings->variablesDockHeight );
    QTimer::singleShot(0, d->variablesDock, SLOT(show()));
  }

  if( settings->showConstants )
  if( settings->constantsDockFloating )
  if( !d->constantsDock->isFloating() )
  {
    d->constantsDock->hide();
    d->constantsDock->setFloating( true );
    d->constantsDock->move( settings->constantsDockLeft, settings->constantsDockTop );
    d->constantsDock->resize( settings->constantsDockWidth, settings->constantsDockHeight );
    QTimer::singleShot(0, d->constantsDock, SLOT(show()));
  }
}

void Crunch::activate()
{
  activateWindow();
  d->editor->setFocus();
}

void Crunch::minimizeToTray()
{
  if( d->trayIcon )
  {
    hide();
    d->trayIcon->show();
    if( d->trayNotify )
      QTimer::singleShot( 500, this, SLOT(showTrayMessage()) );
    d->trayNotify = false;
  }
}

void Crunch::showTrayMessage()
{
  if( d->trayIcon )
    d->trayIcon->showMessage( QString(),
      tr("SpeedCrunch is minimized. \n Click on the icon to reactivate it"),
      QSystemTrayIcon::NoIcon, 2000);
}

void Crunch::trayIconActivated()
{
  showNormal();
  activateWindow();
  d->editor->setFocus();
  QTimer::singleShot( 0, d->trayIcon, SLOT(hide()) );

  // work around docks does not reappear (under KDE/Linux)
#ifdef Q_OS_UNIX
  if( d->historyDock->isFloating() )
  {
    d->historyDock->hide();
    QTimer::singleShot( 0, d->historyDock, SLOT(show()) );
  }
  if( d->functionsDock->isFloating() )
  {
    d->functionsDock->hide();
    QTimer::singleShot( 0, d->functionsDock, SLOT(show()) );
  }
  if( d->variablesDock->isFloating() )
  {
    d->variablesDock->hide();
    QTimer::singleShot( 0, d->variablesDock, SLOT(show()) );
  }
  if( d->constantsDock->isFloating() )
  {
    d->constantsDock->hide();
    QTimer::singleShot( 0, d->constantsDock, SLOT(show()) );
  }
#endif
}

void Crunch::angleModeChanged()
{
  const QObject* s = sender();
  if( !s ) return;
  if( !s->inherits( "QRadioButton" ) ) return;

  blockSignals( true );
  if( s == static_cast<QObject*>( d->degButton ) )
    d->radButton->setChecked( !d->degButton->isChecked() );
  if( s == static_cast<QObject*>( d->radButton ) )
    d->degButton->setChecked( !d->radButton->isChecked() );
  blockSignals( false );

  if( d->degButton->isChecked() )
    d->eval->setAngleMode( Evaluator::Degree );
  if( d->radButton->isChecked() )
    d->eval->setAngleMode( Evaluator::Radian );

  QTimer::singleShot(0, d->editor, SLOT( setFocus() ) );
}

void Crunch::radixChanged()
{
  const QObject* s = sender();
  if( !s ) return;
  if( !s->inherits( "QRadioButton" ) ) return;

  Settings* settings = Settings::self();

  if( d->hexButton->isChecked() && settings->format != 'h')
  {
    d->digitsGroup->setDisabled(true);
    setView('h');
  }

  if( d->decButton->isChecked() && settings->format != 'g' &&
    settings->format != 'e'  && settings->format != 'f' &&
	settings->format != 'n')
  {
    d->digitsGroup->setDisabled(false);
    setView('g');
  }

  if( d->octButton->isChecked()  && settings->format != 'o')
  {
    d->digitsGroup->setDisabled(true);
    setView('o');
  }

  if( d->binButton->isChecked()  &&  settings->format != 'b')
  {
    d->digitsGroup->setDisabled(true);
    setView('b');
  }

  QTimer::singleShot(0, d->editor, SLOT( setFocus() ) );
}

void Crunch::returnPressed()
{
  QString str = Evaluator::autoFix( d->editor->text(), Settings::self()->decimalPoint );
  if( str.isEmpty() ) return;

  d->eval->setExpression( str );
  d->editor->appendHistory( str );
  d->historyDock->setHistory( d->editor->history() );

  HNumber result = d->eval->eval();
  if( !d->eval->error().isEmpty() )
    d->result->appendError( str, d->eval->error() );
  else
  {
    d->result->append( str, result );
    d->editor->setAnsAvailable( true );
    d->variablesDock->updateList( d->eval );
  }

  d->editor->setText( str );
  d->editor->selectAll();
  d->editor->stopAutoCalc();
  d->editor->stopAutoComplete();
  d->autoAns = true;

  QTimer::singleShot( 0, d->editor, SLOT(setFocus()) );

  if( !isActiveWindow () )
    activateWindow();
}

void Crunch::expressionSelected( const QString& e )
{
  d->editor->setText( e );
  returnPressed();

  if( !isActiveWindow () )
    activateWindow();
}

void Crunch::functionSelected( const QString& e )
{
  if( e.isEmpty() )
    return;
  d->editor->insert( e );
  d->editor->insert( "(" );

  QTimer::singleShot( 0, d->editor, SLOT(setFocus()) );

  if( !isActiveWindow () )
    activateWindow();
}

void Crunch::variableSelected( const QString& v )
{
  if( v.isEmpty() )
    return;
  d->editor->insert( v );

  QTimer::singleShot( 0, d->editor, SLOT(setFocus()) );

  if( !isActiveWindow () )
    activateWindow();
}

void Crunch::constantSelected( const QString& c )
{
  if( c.isEmpty() )
    return;
  d->editor->insert( c );

  QTimer::singleShot( 0, d->editor, SLOT(setFocus()) );

  if( !isActiveWindow () )
    activateWindow();
}

void Crunch::textChanged()
{
  if( d->autoAns )
  {
    QString expr = Evaluator::autoFix( d->editor->text(), Settings::self()->decimalPoint );
    if( expr.isEmpty() ) return;
    Tokens tokens = Evaluator::scan( expr, Settings::self()->decimalPoint );
    if( tokens.count() == 1 )
    if( ( tokens[0].asOperator() == Token::Plus ) ||
        ( tokens[0].asOperator() == Token::Minus ) ||
        ( tokens[0].asOperator() == Token::Asterisk ) ||
        ( tokens[0].asOperator() == Token::Slash ) ||
        ( tokens[0].asOperator() == Token::Caret ) )
     {
       d->autoAns = false;
       expr.prepend( "ans" );
       d->editor->setText( expr );
       d->editor->setCursorPosition( expr.length() );
     }
  }
}

void Crunch::copyResult()
{
  QClipboard *cb = QApplication::clipboard();
  HNumber num = d->eval->get("ans"); char *ss;
  switch (num.format())
  {
  case 'h':
     ss = HMath::formatHexadec( num );
     break;
  case 'o':
     ss = HMath::formatOctal( num );
     break;
  case 'b':
     ss = HMath::formatBinary( num );
     break;
  default:
     ss = HMath::formatFixed( num );
  }
  cb->setText( QString(ss), QClipboard::Clipboard );
  free( ss );
}

void Crunch::focusAndSelectInput()
{
  d->editor->selectAll();
  QTimer::singleShot(0, d->editor, SLOT( setFocus() ) );
}

void Crunch::clearInput()
{
  d->editor->clear();
  QTimer::singleShot(0, d->editor, SLOT( setFocus() ) );
}

void Crunch::clearVariables()
{
  d->eval->clearVariables();
  d->variablesDock->updateList( d->eval );
}

void Crunch::insertFunction()
{
  if( !d->insertFunctionDlg )
    d->insertFunctionDlg = new InsertFunctionDlg( this );
  //else    d->insertFunctionDlg->updateList();

  if( d->insertFunctionDlg->exec() == InsertFunctionDlg::Accepted )
  {
    QString fname = d->insertFunctionDlg->functionName();
    if( !fname.isEmpty() )
      d->editor->insert( fname );
  }
}

void Crunch::insertVariable()
{
  if( !d->insertVariableDlg )
    d->insertVariableDlg = new InsertVariableDlg( d->eval, this );
  else
    d->insertVariableDlg->updateList();

  if( d->insertVariableDlg->exec() == InsertVariableDlg::Accepted )
  {
    QString varname = d->insertVariableDlg->variableName();
    if( !varname.isEmpty() )
      d->editor->insert( varname );
  }
}

void Crunch::deleteVariable()
{
  if( !d->deleteVariableDlg )
    d->deleteVariableDlg = new DeleteVariableDlg( d->eval, this );
  else
    d->deleteVariableDlg->updateList();

  d->deleteVariableDlg->exec();
  d->variablesDock->updateList( d->eval );
}

void Crunch::setView(char c)
{
  Settings* settings = Settings::self();
  settings->format = c;
  saveSettings();
  applySettings();
}

void Crunch::viewGeneral()
{
  d->digitsGroup->setEnabled(true);
  blockSignals(true);
  d->decButton->setChecked(true);
  blockSignals(true);
  setView('g');
}

void Crunch::viewFixed()
{
  d->digitsGroup->setEnabled(true);
  d->decButton->setChecked(true);
  setView('f');
}

void Crunch::viewEngineering()
{
  d->digitsGroup->setEnabled(true);
  d->decButton->setChecked(true);
  setView('n');
}

void Crunch::viewScientific()
{
  d->digitsGroup->setEnabled(true);
  d->decButton->setChecked(true);
  setView('e');
}

void Crunch::viewHexadec()
{
  d->digitsGroup->setDisabled(true);
  d->hexButton->setChecked(true);
  setView('h');
}

void Crunch::viewOctal()
{
  d->digitsGroup->setDisabled(true);
  d->octButton->setChecked(true);
  setView('o');
}

void Crunch::viewBinary()
{
  d->digitsGroup->setDisabled(true);
  d->binButton->setChecked(true);
  setView('b');
}

void Crunch::setDigits(int i)
{
  Settings* settings = Settings::self();
  settings->decimalDigits = i;
  saveSettings();
  applySettings();
}

void Crunch::digitsAuto()
{
  setDigits(-1);
}

void Crunch::digits2()
{
  setDigits(2);
}

void Crunch::digits3()
{
  setDigits(3);
}

void Crunch::digits8()
{
  setDigits(8);
}

void Crunch::digits15()
{
  setDigits(15);
}

void Crunch::digits50()
{
  setDigits(50);
}

void Crunch::showClearButton( bool b )
{
  Settings* settings = Settings::self();
  settings->showClearInputButton = b;
  saveSettings();
  applySettings();
}

void Crunch::showEvalButton( bool b )
{
  Settings* settings = Settings::self();
  settings->showEvaluateButton = b;
  saveSettings();
  applySettings();
}

void Crunch::showKeyPad( bool b )
{
  Settings* settings = Settings::self();
  settings->showKeyPad = b;
  saveSettings();
  applySettings();
}

void Crunch::showHistory( bool b )
{
  Settings* settings = Settings::self();
  settings->showHistory = b;
  saveSettings();
  applySettings();
  d->historyDock->raise();
}

void Crunch::showFunctions( bool b)
{
  Settings* settings = Settings::self();
  settings->showFunctions = b;
  saveSettings();
  applySettings();
  d->functionsDock->raise();
}

void Crunch::showVariables( bool b)
{
  Settings* settings = Settings::self();
  settings->showVariables = b;
  saveSettings();
  applySettings();
  d->variablesDock->raise();
}

void Crunch::showConstants( bool b)
{
  Settings* settings = Settings::self();
  settings->showConstants = b;
  saveSettings();
  applySettings();
  d->constantsDock->raise();
}

void Crunch::configure()
{
  saveSettings();
  if( !d->configDlg )
  {
    d->configDlg = new ConfigDlg( this );
    connect( d->configDlg, SIGNAL( settingsChanged() ), SLOT( applySettings() ) );
  }
  d->configDlg->exec();
}

void Crunch::showTip()
{
  // DEBUG: test tip of the day
  QPoint p = mapFromGlobal( d->result->mapToGlobal( QPoint(0, 0) ) ) += QPoint(4,4);
  d->tip->move( p );

  int tipNo = rand() % 4;
  QString msg;
  switch( tipNo )
  {
    case 0:
      msg = tr("You can customize the syntax highlight colors. "
        "Use menu <i>Settings, Configure</i>, and then from the configuration dialog, "
        "choose tab <i>Syntax Highlight<i>." );
      break;
    case 1:
      msg = tr("To insert a function using keyboard, use Ctrl+F shorcut. "
        "From the dialog, you can choose the function you want to insert." );
      break;
    case 2:
      msg = tr("To insert a variable using keyboard, use Ctrl+I shorcut. "
        "From the dialog, you can choose the variable you want to insert." );
      break;
    case 3:
      msg = tr("Use variable <i>pi</i> to use pi constant." );
      break;
    default:
      break;
  }

  d->tip->showText( msg );
}

void Crunch::gotoWebsite()
{
  QDesktopServices::openUrl( QUrl(QString::fromLatin1("http://www.speedcrunch.org")) );
}

void Crunch::about()
{
  AboutBox* aboutBox = new AboutBox( this );
  aboutBox->exec();
  delete aboutBox;
}

void Crunch::aboutQt()
{
  QMessageBox::aboutQt( this, tr("About Qt") );
}

void Crunch::addKeyPadText( const QString& text )
{
  if( text == "<--" ) // Special case: backspace
    d->editor->doBackspace();
  else
  {
    bool wasAns = d->editor->toPlainText().startsWith( "ans", Qt::CaseInsensitive );
    d->editor->insertPlainText( text );
    if( !wasAns && d->editor->toPlainText().startsWith( "ans", Qt::CaseInsensitive ) )
      d->editor->setCursorPosition( d->editor->text().length() );
  }

  d->editor->setFocus();
}

void Crunch::hideAutoCalc()
{
  d->autoCalcLabel->hideText();
}

void Crunch::showAutoCalc( const QString& msg )
{
  QPoint p = d->editor->mapToParent( QPoint(0, 0) );
  d->autoCalcLabel->move( p );

  d->autoCalcLabel->showText( msg );
}
