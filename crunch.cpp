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

#include "crunch.h"
#include "evaluator.h"
#include "historydock.h"
#include "hmath.h"
#include "editor.h"
#include "functions.h"
#include "functionsdock.h"
#include "result.h"
#include "settings.h"
#include "insertfunctiondlg.h"
#include "insertvardlg.h"
#include "deletevardlg.h"
#include "configdlg.h"
#include "aboutbox.h"
#include "keypad.h"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QRadioButton>
#include <QTextStream>
#include <QTimer>
#include <QToolTip>
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
  QAction* viewExponential;
  QAction* viewHexadec;
  QAction* viewOctal;
  QAction* viewBinary;
  QAction* digitsAuto;
  QAction* digits2;
  QAction* digits3;
  QAction* digits8;
  QAction* digits15;
  QAction* digits50;
  QAction* showClearButton;
  QAction* showEvalButton;
  QAction* showKeyPad;
  QAction* configure;
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

  HistoryDock* historyDock;
  FunctionsDock* functionsDock;

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

  QSpacerItem *spacer = new QSpacerItem( 50, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
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
  d->clearInputButton->setIconSet( QPixmap(":/clearinput.png") );
  d->clearInputButton->hide();
  QToolTip::add( d->clearInputButton, tr("Clear input line") );
  inputBoxLayout->addWidget( d->clearInputButton );

  d->editor = new Editor( d->eval, box );
  d->editor->setFocus();
  inputBoxLayout->addWidget( d->editor );

  d->evalButton = new QPushButton( box );
  d->evalButton->setText( tr("Evaluate" ) );
  d->evalButton->hide();
  inputBoxLayout->addWidget( d->evalButton );

  outerBoxLayout->addLayout( inputBoxLayout );

  d->clearInputButton->setMaximumHeight( d->editor->sizeHint().height() );
  d->editor->setMaximumHeight( d->editor->sizeHint().height() );

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
  addDockWidget( Qt::RightDockWidgetArea, d->historyDock );

  d->functionsDock = new FunctionsDock( this );
  addDockWidget( Qt::RightDockWidgetArea, d->functionsDock );

  // Connect signals and slots

  connect( d->clearInputButton, SIGNAL( clicked() ), SLOT( clearInput() ) );
  connect( d->evalButton, SIGNAL( clicked() ), SLOT( returnPressed() ) );
  connect( d->editor, SIGNAL( returnPressed() ), SLOT( returnPressed() ) );
  connect( d->editor, SIGNAL( textChanged() ), SLOT( textChanged() ) );
  connect( d->result, SIGNAL( textCopied( const QString& ) ), d->editor, SLOT( paste() ) );
  connect( d->result, SIGNAL( textCopied( const QString& ) ), d->editor, SLOT( setFocus() ) );
  connect( d->historyDock, SIGNAL( expressionSelected( const QString& ) ), SLOT( expressionSelected( const QString& ) ) );
  connect( d->functionsDock, SIGNAL( functionSelected( const QString& ) ), SLOT( functionSelected( const QString& ) ) );

  connect( d->keypad, SIGNAL( addText( const QString& ) ), SLOT( addKeyPadText( const QString& ) ) );

  // Initialize settings

  d->configDlg = 0;
  d->insertFunctionDlg = 0;
  d->insertVariableDlg = 0;
  d->deleteVariableDlg = 0;

  setCaption( tr( "SpeedCrunch" ) );
  d->degButton->setChecked( true );
  createUI();
  applySettings();
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
  d->actions->clearDisplay = new QAction( tr("Clear &Display" ), this );
  d->actions->clearHistory = new QAction( tr("Clear &History" ), this );
  d->actions->clearVariables = new QAction( tr("Clear V&ariables" ), this );

  d->actions->viewGeneral = new QAction( tr("&General"), 0 );
  d->actions->viewFixed = new QAction( tr("&Fixed Decimal"), 0 );
  d->actions->viewExponential = new QAction( tr("&Exponential"), 0 );
  d->actions->viewHexadec = new QAction( tr("&Hexadecimal"), 0 );
  d->actions->viewOctal = new QAction( tr("&Octal"), 0 );
  d->actions->viewBinary = new QAction( tr("&Binary"), 0 );

  QActionGroup *formatGroup = new QActionGroup( this );
  formatGroup->addAction( d->actions->viewGeneral );
  formatGroup->addAction( d->actions->viewFixed );
  formatGroup->addAction( d->actions->viewExponential );
  formatGroup->addAction( d->actions->viewHexadec );
  formatGroup->addAction( d->actions->viewOctal );
  formatGroup->addAction( d->actions->viewBinary );

  d->actions->viewGeneral->setToggleAction( true );
  d->actions->viewFixed->setToggleAction( true );
  d->actions->viewExponential->setToggleAction( true );
  d->actions->viewHexadec->setToggleAction( true );
  d->actions->viewOctal->setToggleAction( true );
  d->actions->viewBinary->setToggleAction( true );

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

  d->actions->digitsAuto->setToggleAction( true );
  d->actions->digits2->setToggleAction( true );
  d->actions->digits3->setToggleAction( true );
  d->actions->digits8->setToggleAction( true );
  d->actions->digits15->setToggleAction( true );
  d->actions->digits50->setToggleAction( true );

  d->actions->showClearButton = new QAction( tr("&Show Clear Button"), this );
  d->actions->showEvalButton = new QAction( tr("Show &Evaluate Button"), this );
  d->actions->showKeyPad = new QAction( tr("Show &Key Pad"), this );
  d->actions->showClearButton->setToggleAction( true );
  d->actions->showEvalButton->setToggleAction( true );
  d->actions->showKeyPad->setToggleAction( true );

  d->actions->configure = new QAction( tr("&Configure..."), this );

  d->actions->helpAbout = new QAction( tr("&About"), this );
  d->actions->helpAboutQt = new QAction( tr("About &Qt"), this );

  // signal/slot
  connect( d->actions->sessionSave, SIGNAL( activated() ), this, SLOT( saveSession() ) );
  connect( d->actions->sessionQuit, SIGNAL( activated() ), this, SLOT( close() ) );
  connect( d->actions->editPaste, SIGNAL( activated() ), d->editor, SLOT( paste() ) );
  connect( d->actions->editCopy, SIGNAL( activated() ), d->editor, SLOT( copy() ) );
  connect( d->actions->editCopyResult, SIGNAL( activated() ), this, SLOT( copyResult() ) );
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
  connect( d->actions->viewExponential, SIGNAL( activated() ), this, SLOT( viewExponential() ) );
  connect( d->actions->viewHexadec, SIGNAL( activated() ), this, SLOT( viewHexadec() ) );
  connect( d->actions->viewOctal, SIGNAL( activated() ), this, SLOT( viewOctal() ) );
  connect( d->actions->viewBinary, SIGNAL( activated() ), this, SLOT( viewBinary() ) );
  connect( d->actions->digitsAuto, SIGNAL( activated() ), this, SLOT( digitsAuto() ) );
  connect( d->actions->digits2, SIGNAL( activated() ), this, SLOT( digits2() ) );
  connect( d->actions->digits3, SIGNAL( activated() ), this, SLOT( digits3() ) );
  connect( d->actions->digits8, SIGNAL( activated() ), this, SLOT( digits8() ) );
  connect( d->actions->digits15, SIGNAL( activated() ), this, SLOT( digits15() ) );
  connect( d->actions->digits50, SIGNAL( activated() ), this, SLOT( digits50() ) );
  connect( d->actions->showClearButton, SIGNAL( activated() ), this, SLOT( showClearButton() ) );
  connect( d->actions->showEvalButton, SIGNAL( activated() ), this, SLOT( showEvalButton() ) );
  connect( d->actions->showKeyPad, SIGNAL( activated() ), this, SLOT( showKeyPad() ) );
  connect( d->actions->configure, SIGNAL( activated() ), this, SLOT( configure() ) );
  connect( d->actions->helpAbout, SIGNAL( activated() ), this, SLOT( about() ) );
  connect( d->actions->helpAboutQt, SIGNAL( activated() ), this, SLOT( aboutQt() ) );

  // construct the menu

  QMenu *sessionMenu = new QMenu( this );
  menuBar()->insertItem( tr("&Session"), sessionMenu );
  sessionMenu->addAction( d->actions->sessionSave );
  sessionMenu->addAction( d->actions->sessionQuit );

  QMenu *editMenu = new QMenu( this );
  menuBar()->insertItem( tr("&Edit"), editMenu );
  editMenu->addAction( d->actions->editCopy );
  editMenu->addAction( d->actions->editCopyResult );
  editMenu->addAction( d->actions->editPaste );
  editMenu->insertSeparator();
  editMenu->addAction( d->actions->insertFunction );
  editMenu->addAction( d->actions->insertVariable );
  editMenu->insertSeparator();
  editMenu->addAction( d->actions->deleteVariable );
  editMenu->insertSeparator();
  editMenu->addAction( d->actions->clearInput );
  editMenu->addAction( d->actions->clearDisplay );
  editMenu->addAction( d->actions->clearHistory );
  editMenu->addAction( d->actions->clearVariables );

  QMenu *viewMenu = new QMenu( this );
  menuBar()->insertItem( tr("&View"), viewMenu );
  viewMenu->addAction( d->actions->viewGeneral );
  viewMenu->addAction( d->actions->viewFixed );
  viewMenu->addAction( d->actions->viewExponential );
  viewMenu->insertSeparator();
  viewMenu->addAction( d->actions->digitsAuto );
  viewMenu->addAction( d->actions->digits2 );
  viewMenu->addAction( d->actions->digits3 );
  viewMenu->addAction( d->actions->digits8 );
  viewMenu->addAction( d->actions->digits15 );
  viewMenu->addAction( d->actions->digits50 );
  viewMenu->insertSeparator();
  viewMenu->addAction( d->actions->viewHexadec );
  viewMenu->addAction( d->actions->viewOctal );
  viewMenu->addAction( d->actions->viewBinary );


  QMenu *settingsMenu = new QMenu( this );
  settingsMenu->addAction( d->actions->showClearButton );
  settingsMenu->addAction( d->actions->showEvalButton );
  settingsMenu->addAction( d->actions->showKeyPad );
  QMenu* docksMenu = settingsMenu->addMenu( tr("Show &Docks") );
  docksMenu->addAction( d->historyDock->toggleViewAction() );
  docksMenu->addAction( d->functionsDock->toggleViewAction() );
  menuBar()->insertItem( tr("Se&ttings"), settingsMenu );
  settingsMenu->insertSeparator();
  settingsMenu->addAction( d->actions->configure );

  QMenu *helpMenu = new QMenu( this );
  menuBar()->insertItem( tr("&Help"), helpMenu );
  helpMenu->addAction( d->actions->helpAbout );
  helpMenu->addAction( d->actions->helpAboutQt );

  setIcon( QPixmap( ":/crunch.png" ) );

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
  }
  else
  {
    d->result->setFont( QApplication::font( d->result ) );
    d->editor->setFont( QApplication::font( d->editor ) );
  }

  d->result->setCustomAppearance( settings->customAppearance );
  d->result->setCustomTextColor( settings->customTextColor );
  d->result->setCustomBackgroundColor( settings->customBackgroundColor1,
    settings->customBackgroundColor2 );

  if( settings->format == 'g' ) d->actions->viewGeneral->setOn( true );
  if( settings->format == 'f' ) d->actions->viewFixed->setOn( true );
  if( settings->format == 'e' ) d->actions->viewExponential->setOn( true );
  if( settings->format == 'h' ) d->actions->viewHexadec->setOn( true );
  if( settings->format == 'o' ) d->actions->viewOctal->setOn( true );
  if( settings->format == 'b' ) d->actions->viewBinary->setOn( true );

  if( settings->decimalDigits < 0 ) d->actions->digitsAuto->setOn( true );
  if( settings->decimalDigits == 2 ) d->actions->digits2->setOn( true );
  if( settings->decimalDigits == 3 ) d->actions->digits3->setOn( true );
  if( settings->decimalDigits == 8 ) d->actions->digits8->setOn( true );
  if( settings->decimalDigits == 15 ) d->actions->digits15->setOn( true );
  if( settings->decimalDigits == 50 ) d->actions->digits50->setOn( true );

  if( settings->showClearInputButton )
    d->clearInputButton->show();
  else
    d->clearInputButton->hide();
  d->actions->showClearButton->setOn( settings->showClearInputButton );

  if( settings->showEvaluateButton )
    d->evalButton->show();
  else
    d->evalButton->hide();
  d->actions->showEvalButton->setOn( settings->showEvaluateButton );

  if( settings->showKeyPad )
    d->keypad->show();
  else
    d->keypad->hide();
  d->actions->showKeyPad->setOn( settings->showKeyPad );
}

void Crunch::closeEvent( QCloseEvent* e )
{
  saveSettings();
  QMainWindow::closeEvent( e );
}

void Crunch::saveSession()
{
  QString filters = tr( "Text Files (*.txt);; All Files (*.*)" );
  QString fname = QFileDialog::getSaveFileName( QString::null, filters, this, 0,
    tr("Save Session") );
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

  if( settings->saveHistory )
    settings->history = d->editor->history();

  if( settings->saveVariables )
  {
    settings->variables.clear();
    QVector<Variable> vars = d->eval->variables();
    for( int i=0; i<vars.count(); i++ )
      if( vars[i].name.lower() != "pi" )
      {
        char* str = HMath::formatFixed( vars[i].value, 100 );
        settings->variables.append( QString("%1=%2").arg( vars[i].name ).
          arg( QString( str ) ) );
        free( str );
      }
  }

  settings->save();
}

void Crunch::angleModeChanged()
{
  const QObject* s = sender();
  if( !s ) return;
  if( !s->isA( "QRadioButton" ) ) return;

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
  if( !s->isA( "QRadioButton" ) ) return;

  Settings* settings = Settings::self();

  if( d->hexButton->isChecked() && settings->format != 'h')
  {
    d->digitsGroup->setDisabled(true);
    setView('h');
  }

  if( d->decButton->isChecked() && settings->format != 'g' &&
    settings->format != 'e'  && settings->format != 'f' )
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
  QString str = Evaluator::autoFix( d->editor->text() );
  if( str.isEmpty() ) return;

  d->eval->setExpression( str );
  d->editor->appendHistory( str );
  d->historyDock->setHistory( d->editor->history() );

  HNumber result = d->eval->eval();
  if( !d->eval->error().isEmpty() )
    d->result->appendError( str, tr( "Error: %1" ).arg( d->eval->error() )  );
  else
  {
    d->result->append( str, result );
    d->editor->setAnsAvailable( true );
  }

  d->editor->setText( str );
  d->editor->selectAll();
  d->editor->stopAutoCalc();
  d->autoAns = true;
}

void Crunch::expressionSelected( const QString& e )
{
  d->editor->setText( e );
  returnPressed();
}

void Crunch::functionSelected( const QString& e )
{
  if( e.isEmpty() )
    return;
  d->editor->insert( e );
  d->editor->insert( "(" );
  QTimer::singleShot( 0, d->editor, SLOT(setFocus()) );
}

void Crunch::textChanged()
{
  if( d->autoAns )
  {
    QString expr = Evaluator::autoFix( d->editor->text() );
    if( expr.isEmpty() ) return;
    Tokens tokens = Evaluator::scan( expr );
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
       d->editor->setCursorPosition( 0, expr.length() );
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

void Crunch::clearInput()
{
  d->editor->clear();
}

void Crunch::clearVariables()
{
  d->eval->clearVariables();
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

void Crunch::viewExponential()
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

void Crunch::showClearButton()
{
  Settings* settings = Settings::self();
  settings->showClearInputButton = !settings->showClearInputButton;
  saveSettings();
  applySettings();
}

void Crunch::showEvalButton()
{
  Settings* settings = Settings::self();
  settings->showEvaluateButton = !settings->showEvaluateButton;
  saveSettings();
  applySettings();
}

void Crunch::showKeyPad()
{
  Settings* settings = Settings::self();
  settings->showKeyPad = !settings->showKeyPad;
  saveSettings();
  applySettings();
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

void Crunch::about()
{
  AboutBox* aboutBox = new AboutBox( this );
  aboutBox->setCaption( tr("About SpeedCrunch" ) );
  aboutBox->exec();
  delete aboutBox;
}

void Crunch::aboutQt()
{
  QMessageBox::aboutQt( this, tr("About Qt") );
}

void Crunch::addKeyPadText( const QString& text )
{
  int para, index;

  if( text == "<--" ) // Special case: backspace
  {
    if( !d->editor->hasSelectedText() )
    {
      d->editor->getCursorPosition( &para, &index );
      if( index == 0 )
        return;

      d->editor->setSelection( para, index-1, para, index );
      d->editor->removeSelectedText();
    }

    d->editor->removeSelectedText();
  }
  else
  {
    if( d->editor->hasSelectedText() )
      d->editor->removeSelectedText();

    bool wasAns = d->editor->text().startsWith( "ans", Qt::CaseInsensitive );

    d->editor->getCursorPosition( &para, &index );
    d->editor->insert( text );

    if( !wasAns && d->editor->text().startsWith( "ans", Qt::CaseInsensitive ) )
      d->editor->setCursorPosition( para, d->editor->text().length() );
  }

  d->editor->setFocus();
}
