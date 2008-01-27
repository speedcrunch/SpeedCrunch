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


#include "crunch.hxx"

#include <base/constants.hxx>
#include <base/evaluator.hxx>
#include <base/functions.hxx>
#include <base/settings.hxx>
#include <gui/aboutbox.hxx>
#include <gui/autohidelabel.hxx>
#include <gui/constantsdock.hxx>
#include <gui/deletevardlg.hxx>
#include <gui/editor.hxx>
#include <gui/functionsdock.hxx>
#include <gui/historydock.hxx>
#include <gui/insertfunctiondlg.hxx>
#include <gui/insertvardlg.hxx>
#include <gui/keypad.hxx>
#include <gui/result.hxx>
#include <gui/tipwidget.hxx>
#include <gui/variablesdock.hxx>
#include <math/hmath.hxx>

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
#include <QSystemTrayIcon>
#include <QTextStream>
#include <QTimer>
#include <QToolTip>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>

#include <stdlib.h> // rand()


struct Actions
{
  QAction * clearExpression;
  QAction * clearHistory;
  QAction * degree;
  QAction * deleteAllVariables;
  QAction * deleteVariable;
  QAction * digits15;
  QAction * digits2;
  QAction * digits3;
  QAction * digits50;
  QAction * digits8;
  QAction * digitsAuto;
  QAction * editCopy;
  QAction * editCopyResult;
  QAction * editPaste;
  QAction * helpAbout;
  QAction * helpAboutQt;
  QAction * helpGotoWebsite;
  QAction * helpTipOfTheDay;
  QAction * insertFunction;
  QAction * insertVariable;
  QAction * optionAlwaysOnTop;
  QAction * optionAutoCalc;
  QAction * optionAutoCompletion;
  QAction * optionDotAuto;
  QAction * optionDotComma;
  QAction * optionDotPoint;
  QAction * optionMinimizeToTray;
  QAction * radian;
  QAction * radixCharAuto;
  QAction * radixCharComma;
  QAction * radixCharDot;
  QAction * scrollDown;
  QAction * scrollUp;
  QAction * selectExpression;
  QAction * sessionLoad;
  QAction * sessionQuit;
  QAction * sessionSave;
  QAction * showConstants;
  QAction * showFullScreen;
  QAction * showFunctions;
  QAction * showHistory;
  QAction * showKeypad;
  QAction * showMenuBar;
  QAction * showVariables;
  QAction * viewBinary;
  QAction * viewEngineering;
  QAction * viewFixed;
  QAction * viewGeneral;
  QAction * viewHexadec;
  QAction * viewOctal;
  QAction * viewScientific;
};


struct Menus
{
  QMenu * angle;
  QMenu * behavior;
  QMenu * decimal;
  QMenu * edit;
  QMenu * format;
  QMenu * help;
  QMenu * layout;
  QMenu * radixChar;
  QMenu * session;
  QMenu * settings;
};


struct Crunch::Private
{
  Crunch * p;

  Settings            settings;
  //
  Qt::WindowFlags     flags;
  //
  Actions             actions;
  Menus               menus;
  QActionGroup *      angleGroup;
  QActionGroup *      digitsGroup;
  //
  bool                notifyMenuBarHidden;
  bool                trayNotify;
  //
  Editor *            editor;
  Evaluator *         evaluator;
  Keypad *            keypad;
  Result *            display;
  //
  bool                autoAns;
  AutoHideLabel *     autoCalcLabel;
  TipWidget *         tip;
  //
  QSystemTrayIcon *   trayIcon;
  QMenu *             trayIconMenu;
  //
  ConstantsDock *     constantsDock;
  FunctionsDock *     functionsDock;
  HistoryDock *       historyDock;
  VariablesDock *     variablesDock;
  //
  DeleteVariableDlg * deleteVariableDlg;
  InsertFunctionDlg * insertFunctionDlg;
  InsertVariableDlg * insertVariableDlg;

  void applySettings();
  void createUI();
  void saveSettings();
};


static void setWidgetLayoutAccordingToLanguageDirection( QWidget * widget )
{
  if ( QLocale().language() == QLocale::Hebrew )
    widget->setLayoutDirection( Qt::RightToLeft );
  else
    widget->setLayoutDirection( Qt::LeftToRight );
}


// public

Crunch::Crunch() : QMainWindow(), d( new Crunch::Private )
{
  d->p = this;

  d->settings.load();
  d->evaluator = new Evaluator( d->settings.radixChar, this );

  d->autoAns = false;
  d->trayIcon = 0;
  d->trayNotify = true;

  d->notifyMenuBarHidden = true;
  // outer widget and layout
  QWidget * box = new QWidget( this );
  setCentralWidget( box );

  QVBoxLayout * boxLayout = new QVBoxLayout( box );
  boxLayout->setMargin( 0 );
  boxLayout->setSpacing( 0 );

  // display
  d->display = new Result( d->settings.radixChar, d->settings.format,
                           d->settings.precision, box );
  boxLayout->addWidget( d->display );

  // editor
  QHBoxLayout * editorLayout = new QHBoxLayout();
  editorLayout->setMargin( 5 );
  d->editor = new Editor( d->evaluator, d->settings.radixChar, box );
  d->editor->setFocus();
  d->editor->setStyleSheet( "QTextEdit { font: bold 16px }" );
  editorLayout->addWidget( d->editor );
  boxLayout->addLayout( editorLayout );

  // we need settings to be loaded before keypad and constants dock
  //d->settings.load(); // load happens only once now, very first action

  // keypad
  QHBoxLayout * keypadLayout = new QHBoxLayout();
  d->keypad = new Keypad( d->settings.radixChar, box );
  d->keypad->setFocusPolicy( Qt::NoFocus );
  d->keypad->hide();
  d->keypad->setStyleSheet( "QPushButton { background: black; font: bold;"
                            "color: white; border-style: solid;"
                            "border-color: #202020; border-radius: 10px;"
                            "border-width: 2px }" );
  keypadLayout->addStretch();
  keypadLayout->addWidget( d->keypad );
  keypadLayout->addStretch();
  boxLayout->addLayout( keypadLayout );

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

  // for tip of the day and menu bar hiding message
  d->tip = new TipWidget( this );
  d->tip->hide();

  // Connect signals and slots
  QObject::connect( d->keypad,        SIGNAL( buttonPressed( Keypad::Button )),
                    this,             SLOT  ( keypadButtonPressed( Keypad::Button )) );
  QObject::connect( d->editor,        SIGNAL( autoCalcActivated( const QString & )),
                    this,             SLOT  ( showAutoCalc( const QString & ))       );
  QObject::connect( d->editor,        SIGNAL( autoCalcDeactivated()),
                    this,             SLOT  ( hideAutoCalc())                        );
  QObject::connect( d->editor,        SIGNAL( returnPressed()),
                    this,             SLOT  ( returnPressed())                       );
  QObject::connect( d->editor,        SIGNAL( textChanged()),
                    this,             SLOT  ( textChanged())                         );
  QObject::connect( d->constantsDock, SIGNAL( constantSelected( const QString & )),
                    this,             SLOT  ( constantSelected( const QString & ))   );
  QObject::connect( d->functionsDock, SIGNAL( functionSelected( const QString & )),
                    this,             SLOT  ( functionSelected( const QString & ))   );
  QObject::connect( d->historyDock,   SIGNAL( expressionSelected( const QString & )),
                    this,             SLOT  ( expressionSelected( const QString & )) );
  QObject::connect( d->variablesDock, SIGNAL( variableSelected( const QString & )),
                    this,             SLOT  ( variableSelected( const QString & ))   );
  QObject::connect( d->display,       SIGNAL( textCopied( const QString & )),
                    d->editor,        SLOT  ( paste())                               );
  QObject::connect( d->display,       SIGNAL( textCopied( const QString & )),
                    d->editor,        SLOT  ( setFocus())                            );
  QObject::connect( this,             SIGNAL( radixCharChanged( char )),
                    d->editor,        SLOT  ( setRadixChar( char ))                  );
  QObject::connect( this,             SIGNAL( radixCharChanged( char )),
                    d->display,       SLOT  ( setRadixChar( char ))                  );
  QObject::connect( this,             SIGNAL( formatChanged( char )),
                    d->display,       SLOT  ( setFormat( char ))                     );
  QObject::connect( this,             SIGNAL( precisionChanged( int )),
                    d->display,       SLOT  ( setPrecision( int ))                   );
  QObject::connect( this,             SIGNAL( radixCharChanged( char )),
                    d->evaluator,     SLOT  ( setRadixChar( char ))                  );
  QObject::connect( this,             SIGNAL( radixCharChanged( char )),
                    d->keypad,        SLOT  ( setRadixChar( char ))                  );

  // Initialize settings
  d->insertFunctionDlg = new InsertFunctionDlg( this );
  d->insertVariableDlg = 0;
  d->deleteVariableDlg = 0;

  setWindowTitle( "SpeedCrunch" );
  d->createUI();
  //d->settings.load();

  if ( d->settings.saveSession )
    restoreLastSession();

  d->applySettings();
  restoreDocks();

  setWidgetsLayoutAccordingToLanguageDirection();

  QTimer::singleShot( 0, this, SLOT(activate()) );
}


bool Crunch::event( QEvent * e )
{
  if ( e->type() == QEvent::WindowStateChange )
  {
    if ( windowState() & Qt::WindowMinimized )
    if ( d->settings.minimizeToTray )
      QTimer::singleShot( 100, this, SLOT(minimizeToTray()) );
  }

  // ensure dock windows keep their state after minimize / screen switch
  //if ( d->settings.showConstants ) QTimer::singleShot( 0, d->constantsDock, SLOT(show()) );
  if ( d->settings.showFunctions ) QTimer::singleShot( 0, d->functionsDock, SLOT(show()) );
  //if ( d->settings.showHistory   ) QTimer::singleShot( 0, d->historyDock,   SLOT(show()) );
  //if ( d->settings.showVariables ) QTimer::singleShot( 0, d->variablesDock, SLOT(show()) );

  return QMainWindow::event( e );
}


Crunch::~Crunch()
{
  //delete d->evaluator; // not needed to delele anymore, QObject child of main window
  delete d;
}


// public slots

void Crunch::about()
{
  AboutBox * aboutBox = new AboutBox( this );
  aboutBox->exec();
  delete aboutBox;
}


void Crunch::aboutQt()
{
  QMessageBox::aboutQt( this, tr("About Qt") );
}


void Crunch::addKeypadText( const QString & text )
{
  bool wasAns = d->editor->toPlainText().startsWith( "ans", Qt::CaseInsensitive );
  d->editor->insertPlainText( text );
  if ( ! wasAns && d->editor->toPlainText().startsWith( "ans", Qt::CaseInsensitive ) )
    d->editor->setCursorPosition( d->editor->text().length() );
  d->editor->setFocus();
}


void Crunch::Private::applySettings()
{
  //d->settings.load();

  if ( settings.mainWindowSize != QSize(0, 0) )
    p->resize( settings.mainWindowSize );

  p->showInFullScreen( settings.showFullScreen );

  editor->setAutoCompleteEnabled( settings.autoComplete );
  editor->setAutoCalcEnabled( settings.autoCalc );

  if ( settings.angleMode == 'r' )
    actions.radian->setChecked( true );
  else if ( settings.angleMode == 'd' )
    actions.degree->setChecked( true );

  //QString l = settings.language;
  //if ( l == "" )
  //{
  //  // autodetect
  //}
  //else
  //{
  //  // load specified language
  //}

  historyDock->clear();
  if ( settings.saveSession && settings.history.count() > 0 )
  {
    editor->setHistory( settings.history );
    editor->setHistoryResults( settings.historyResults );
  }
  if ( settings.history.count() > 0 )
    historyDock->setHistory( settings.history );

  if ( settings.saveVariables )
  {
    for ( int k = 0; k < settings.variables.count(); k++ )
    {
      evaluator->setExpression( settings.variables[k] );
      evaluator->eval();
      QStringList list = settings.variables[k].split( "=" );
      evaluator->set( list[0], HNumber( list[1].toAscii().data() ) );
    }
    variablesDock->updateList( evaluator );
  }

  //display->setFormat( settings.format );
  //display->setPrecision( settings.precision );
  editor->setFormat( settings.format );
  editor->setPrecision( settings.precision );

  //display->setFont( QApplication::font( display ) );
  //editor->setFont( QApplication::font( editor ) );
  editor->setFixedHeight( editor->sizeHint().height() );

  // format
  if      ( settings.format == 'g' ) actions.viewGeneral->setChecked    ( true );
  else if ( settings.format == 'f' ) actions.viewFixed->setChecked      ( true );
  else if ( settings.format == 'n' ) actions.viewEngineering->setChecked( true );
  else if ( settings.format == 'e' ) actions.viewScientific->setChecked ( true );
  else if ( settings.format == 'h' ) actions.viewHexadec->setChecked    ( true );
  else if ( settings.format == 'o' ) actions.viewOctal->setChecked      ( true );
  else if ( settings.format == 'b' ) actions.viewBinary->setChecked     ( true );

  // precision
  if      ( settings.precision <   0 ) actions.digitsAuto->setChecked( true );
  else if ( settings.precision ==  2 ) actions.digits2->setChecked   ( true );
  else if ( settings.precision ==  3 ) actions.digits3->setChecked   ( true );
  else if ( settings.precision ==  8 ) actions.digits8->setChecked   ( true );
  else if ( settings.precision == 15 ) actions.digits15->setChecked  ( true );
  else if ( settings.precision == 50 ) actions.digits50->setChecked  ( true );

  // radix character
  if      ( settings.radixChar == 'C' ) actions.radixCharAuto->setChecked ( true );
  else if ( settings.radixChar == '.' ) actions.radixCharDot->setChecked  ( true );
  else if ( settings.radixChar == ',' ) actions.radixCharComma->setChecked( true );

  p->showKeypad( settings.showKeypad );
  p->menuBar()->setVisible( settings.showMenuBar );

  actions.showConstants->setChecked ( settings.showConstants  );
  actions.showFullScreen->setChecked( settings.showFullScreen );
  actions.showFunctions->setChecked ( settings.showFunctions  );
  actions.showHistory->setChecked   ( settings.showHistory    );
  actions.showVariables->setChecked ( settings.showVariables  );

  constantsDock->setVisible( settings.showConstants );
  functionsDock->setVisible( settings.showFunctions );
  historyDock->setVisible  ( settings.showHistory   );
  variablesDock->setVisible( settings.showVariables );

  actions.optionAlwaysOnTop->setChecked   ( settings.stayAlwaysOnTop );
  actions.optionAutoCalc->setChecked      ( settings.autoCalc        );
  actions.optionAutoCompletion->setChecked( settings.autoComplete    );
  actions.optionMinimizeToTray->setChecked( settings.minimizeToTray  );

  if ( settings.minimizeToTray )
  {
    if ( ! trayIcon && QSystemTrayIcon::isSystemTrayAvailable() )
    {
      trayNotify = true;
      trayIcon = new QSystemTrayIcon( p );
      trayIcon->setToolTip( tr("SpeedCrunch") );
      trayIcon->setIcon( QPixmap( ":/crunch.png" ) );

      trayIconMenu = new QMenu( p );
      trayIconMenu->addAction( actions.editCopyResult );
      trayIconMenu->addSeparator();
      trayIconMenu->addAction( actions.sessionQuit    );

      trayIcon->setContextMenu( trayIconMenu );
      connect( trayIcon, SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
               p,        SLOT  ( trayIconActivated( QSystemTrayIcon::ActivationReason )) );
    }
  }
  else
  {
    if ( trayIcon )
      delete trayIcon;
    trayIcon = 0;
  }

  // treat stay-always-on-top preference
  if ( settings.stayAlwaysOnTop )
    p->setWindowFlags( p->windowFlags() | Qt::WindowStaysOnTopHint );
  else
    p->setWindowFlags( p->windowFlags() & (~ Qt::WindowStaysOnTopHint) );
  p->show();

  // changed settings should trigger auto calc nor auto complete
  editor->stopAutoCalc();
  editor->stopAutoComplete();
}


void Crunch::clearHistory()
{
  d->display->clear();
  d->editor->clearHistory();
  d->historyDock->clear();
  d->settings.history.clear();
  d->settings.historyResults.clear();
  QTimer::singleShot( 0, d->editor, SLOT( setFocus() ) );
}


void Crunch::clearExpression()
{
  d->editor->clear();
  QTimer::singleShot( 0, d->editor, SLOT( setFocus() ) );
}


void Crunch::copyResult()
{
  QClipboard * cb = QApplication::clipboard();
  HNumber num = d->evaluator->get( "ans" );
  char * strToCopy = HMath::format( num, d->settings.format, d->settings.precision );
  QString final( strToCopy );
  if ( d->display->radixChar() == ',' )
    final.replace( '.', ',' );
  cb->setText( final, QClipboard::Clipboard );
  free( strToCopy );
}


void Crunch::degree()
{
  d->settings.angleMode = 'd';
}


void Crunch::deleteAllVariables()
{
  d->evaluator->clearVariables();
  d->variablesDock->updateList( d->evaluator );
}


void Crunch::deleteVariable()
{
  if ( ! d->deleteVariableDlg )
    d->deleteVariableDlg = new DeleteVariableDlg( d->evaluator, this );
  else
    d->deleteVariableDlg->updateList();

  d->deleteVariableDlg->exec();
  d->variablesDock->updateList( d->evaluator );
}


void Crunch::digits2()
{
  setPrecision( 2 );
}


void Crunch::digits3()
{
  setPrecision( 3 );
}


void Crunch::digits8()
{
  setPrecision( 8 );
}


void Crunch::digits15()
{
  setPrecision( 15 );
}


void Crunch::digits50()
{
  setPrecision( 50 );
}


void Crunch::digitsAuto()
{
  setPrecision( -1 );
}


void Crunch::selectExpression()
{
  activateWindow();
  d->editor->selectAll();
  d->editor->setFocus();
}


void Crunch::gotoWebsite()
{
  QDesktopServices::openUrl( QUrl( QString::fromLatin1("http://www.speedcrunch.org") ) );
}


void Crunch::hideAutoCalc()
{
  d->autoCalcLabel->hideText();
}


void Crunch::insertFunction()
{
  if ( d->insertFunctionDlg->exec() == InsertFunctionDlg::Accepted )
  {
    QString fname = d->insertFunctionDlg->functionName();
    if ( ! fname.isEmpty() )
      d->editor->insert( fname );
  }
}


void Crunch::insertVariable()
{
  if ( ! d->insertVariableDlg )
    d->insertVariableDlg = new InsertVariableDlg( d->evaluator, this );
  else
    d->insertVariableDlg->updateList();

  if ( d->insertVariableDlg->exec() == InsertVariableDlg::Accepted )
  {
    QString varName = d->insertVariableDlg->variableName();
    if ( ! varName.isEmpty() )
      d->editor->insert( varName );
  }
}


void Crunch::loadSession()
{
  const char * errMsg= "File %1 is not a valid session";
  QString filters = tr( "SpeedCrunch Sessions (*.sch);;All Files (*)" );
  QString fname = QFileDialog::getOpenFileName( this, tr("Load Session"), QString::null, filters );
  if ( fname.isEmpty() )
    return;

  QFile file( fname );
  if ( ! file.open( QIODevice::ReadOnly ) )
  {
    QMessageBox::critical( this, tr("Error"), tr("Can't read from file %1").arg( fname ) );
    return;
  }

  QTextStream stream( & file );

  // version of the format
  QString version = stream.readLine();
  if ( version != "0.10" )
  {
    QMessageBox::critical( this, tr("Error"), tr(errMsg).arg( fname ) );
    return;
  }

  // number of calculations
  bool ok;
  int noCalcs = stream.readLine().toInt( &ok );
  if ( ok == false || noCalcs < 0 )
  {
    QMessageBox::critical( this, tr("Error"), tr(errMsg).arg( fname ) );
    return;
  }

  // ask for merge with current session
  const char * mergeMsg = "Merge session being loaded with current session?\n"
                          "If no, current variables and display will be cleared.";
  QMessageBox::StandardButton but = QMessageBox::question( this, tr("Question"), tr(mergeMsg),
                                                           QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                                           QMessageBox::Yes );
  if ( but == QMessageBox::Cancel )
  {
    return;
  }
  else if ( but == QMessageBox::No )
  {
    d->display->clear();
    deleteAllVariables();
    clearHistory();
    //saveSettings();
    //applySettings();
  }

  // expressions and results
  for ( int i = 0; i < noCalcs; i++ )
  {
    QString exp = stream.readLine();
    QString res = stream.readLine();
    if ( exp.isNull() || res.isNull() )
    {
      QMessageBox::critical( this, tr("Error"), tr(errMsg).arg( fname ) );
      return;
    }
    HNumber num( res.toAscii().data() );
    if ( ! num.isNan() )
      d->display->append( exp, num );
    else
      d->display->appendError( exp, res );
  }

  // variables
  int noVars = stream.readLine().toInt( &ok );
  if ( ok == false || noVars < 0 )
  {
    QMessageBox::critical( this, tr("Error"), tr(errMsg).arg( fname ) );
    return;
  }
  for ( int i = 0; i < noVars; i++ )
  {
    QString var = stream.readLine();
    QString val = stream.readLine();
    if ( var.isNull() || val.isNull() )
    {
      QMessageBox::critical( this, tr("Error"), tr(errMsg).arg( fname ) );
      return;
    }
    HNumber num( val.toAscii().data() );
    if (  num != HNumber::nan() )
      d->evaluator->set( var, num );
  }

  file.close();

  //saveSettings();
  //applySettings();
}


void Crunch::alwaysOnTopToggled( bool b )
{
  d->settings.stayAlwaysOnTop = b;
  d->actions.optionAlwaysOnTop->setChecked( b );
  //applySettings();
}


void Crunch::autoCalcToggled( bool b )
{
  d->settings.autoCalc = b;
  //applySettings();
}


void Crunch::autoCompletionToggled( bool b )
{
  d->settings.autoComplete = b;
  //applySettings();
}


void Crunch::minimizeToTrayToggled( bool b )
{
  d->settings.minimizeToTray = b;
  //applySettings();
}


void Crunch::radian()
{
  d->settings.angleMode = 'r';
}


void Crunch::saveSession()
{
  QString filters = tr( "SpeedCrunch Sessions (*.sch);;All Files (*)" );
  QString fname = QFileDialog::getSaveFileName( this, tr("Save Session"), QString::null, filters );
  if ( fname.isEmpty() )
    return;

  QFile file( fname );
  if ( ! file.open( QIODevice::WriteOnly ) )
  {
    QMessageBox::critical( this, tr("Error"), tr("Can't write to file %1").arg( fname ) );
    return;
  }

  QTextStream stream( & file );

  // version of the format
  stream << "0.10" << "\n";

  // number of calculations
  stream << d->display->count() << "\n";

  // expressions and results
  stream << d->display->asText() << "\n";

  // number of variables
  int noVars = d->evaluator->variables().count();
  stream << noVars - 2 << "\n"; // exclude pi and phi

  // variables
  for ( int i = 0; i < noVars; i++ )
  {
    Variable var = d->evaluator->variables()[i];
    if ( var.name != "pi" && var.name != "phi" )
      stream << var.name << "\n" << HMath::format( var.value ) << "\n";
  }

  file.close();
}


void Crunch::setWidgetsLayoutAccordingToLanguageDirection()
{
  // menu bar and menus
  setWidgetLayoutAccordingToLanguageDirection( menuBar()          );
  setWidgetLayoutAccordingToLanguageDirection( d->menus.session   );
  setWidgetLayoutAccordingToLanguageDirection( d->menus.edit      );
  setWidgetLayoutAccordingToLanguageDirection( d->menus.format    );
  setWidgetLayoutAccordingToLanguageDirection( d->menus.decimal   );
  setWidgetLayoutAccordingToLanguageDirection( d->menus.settings  );
  setWidgetLayoutAccordingToLanguageDirection( d->menus.layout    );
  setWidgetLayoutAccordingToLanguageDirection( d->menus.behavior  );
  setWidgetLayoutAccordingToLanguageDirection( d->menus.radixChar );
  //setWidgetLayoutAccordingToLanguageDirection( d->menus.theme     );
  //setWidgetLayoutAccordingToLanguageDirection( d->menus.language  );
  setWidgetLayoutAccordingToLanguageDirection( d->menus.help      );
  // tip of the day
  setWidgetLayoutAccordingToLanguageDirection( d->tip );
  // docks
  setWidgetLayoutAccordingToLanguageDirection( d->constantsDock );
  setWidgetLayoutAccordingToLanguageDirection( d->functionsDock );
  // insert function dialog
  setWidgetLayoutAccordingToLanguageDirection( d->insertFunctionDlg );
  // tip of the day
  setWidgetLayoutAccordingToLanguageDirection( d->tip );

  // speedcrunch made widgets
  emit adaptToLanguageChange();
}


void Crunch::scrollDown()
{
  QScrollBar * sb = d->display->verticalScrollBar();
  int value = sb->value() + 40;
  sb->setValue( value );
}


void Crunch::scrollUp()
{
  QScrollBar * sb = d->display->verticalScrollBar();
  int value = sb->value() - 40;
  sb->setValue( value );
}


void Crunch::showMenuBar()
{
  menuBar()->setVisible( ! menuBar()->isVisible() );
  d->settings.showMenuBar = menuBar()->isVisible();

  if ( ! menuBar()->isVisible() && d->notifyMenuBarHidden )
  {
    showMenuBarTip();
    d->notifyMenuBarHidden = false;
  }

  d->display->scrollEnd();
}


void Crunch::showAutoCalc( const QString & msg )
{
  QPoint p = d->editor->mapToParent( QPoint(0, 0) );
  d->autoCalcLabel->move( p );
  d->autoCalcLabel->showText( msg );
}


void Crunch::showConstants( bool b )
{
  d->settings.showConstants = b;
  d->constantsDock->setVisible( b );
  //saveSettings();
  //applySettings();
  //d->constantsDock->raise();
}


void Crunch::showInFullScreen( bool b )
{
  b ? showFullScreen() : showNormal();
  d->settings.showFullScreen = b;
}


void Crunch::showFunctions( bool b )
{
  if ( d->settings.showFunctions == b )
    return;
  d->settings.showFunctions = b;
  d->actions.showFunctions->setChecked( b );
  d->functionsDock->setVisible( b );
}


void Crunch::showHistory( bool b )
{
  d->settings.showHistory = b;
  d->historyDock->setVisible( b );
  //saveSettings();
  //applySettings();
  //d->historyDock->raise();
}


void Crunch::showKeypad( bool b )
{
  d->settings.showKeypad = b;
  d->keypad->setVisible( b );
  d->actions.showKeypad->setChecked( b );
  d->display->scrollEnd();
}


void Crunch::showMenuBarTip()
{
  QString msg = tr("The menu bar is now hidden. "
                   "To make it visible again, press Ctrl+M.");

  QPoint p = mapFromGlobal( d->display->mapToGlobal( QPoint(0, 0) ) ) += QPoint(5, 5);
  d->tip->move( p );
  d->tip->resize( 345, d->tip->sizeHint().height() );
  d->tip->showText( msg, tr("Warning") );
}


void Crunch::showTipOfTheDay()
{
  QPoint p = mapFromGlobal( d->display->mapToGlobal( QPoint(0, 0) ) ) += QPoint(5, 5);
  d->tip->move( p );
  d->tip->resize( 345, d->tip->sizeHint().height() );

  int tipNo = rand() % 5;
  QString msg;
  switch ( tipNo )
  {
    case 0:
      msg = tr("To insert a function using keyboard, use Ctrl+F shorcut. "
               "From the dialog, you can choose the function you want to "
               "insert." );
      break;
    case 1:
      msg = tr("To insert a variable using keyboard, use Ctrl+I shorcut. "
               "From the dialog, you can choose the variable you want to "
               "insert." );
      break;
    case 2:
      msg = tr("Use variable <i>pi</i> to use pi constant." );
      break;
    case 3:
      msg = tr("Use <i>;</i> (semicolon) to separate the parameters in "
               "functions." );
      break;
    default:
      break;
  }

  d->tip->showText( msg, tr("Tip of the day") );
}


void Crunch::showVariables( bool b )
{
  d->settings.showVariables = b;
  d->variablesDock->setVisible( b );
  //saveSettings();
  //applySettings();
  //d->variablesDock->raise();
}


void Crunch::viewBinary()
{
  d->digitsGroup->setDisabled( true );
  setFormat( 'b' );
}


void Crunch::viewEngineering()
{
  d->digitsGroup->setEnabled( true );
  setFormat( 'n' );
}


void Crunch::viewFixed()
{
  d->digitsGroup->setEnabled( true );
  setFormat( 'f' );
}


void Crunch::viewGeneral()
{
  d->digitsGroup->setEnabled( true );
  setFormat( 'g' );
}


void Crunch::viewHexadec()
{
  d->digitsGroup->setDisabled( true );
  setFormat( 'h' );
}


void Crunch::viewOctal()
{
  d->digitsGroup->setDisabled( true );
  setFormat( 'o' );
}


void Crunch::viewScientific()
{
  d->digitsGroup->setEnabled( true );
  setFormat( 'e' );
}


// protected slots

void Crunch::activate()
{
  activateWindow();
  raise();
  d->editor->setFocus();
}


void Crunch::constantSelected( const QString & c )
{
  if ( c.isEmpty() )
    return;

  QString str( c );
  str.replace( '.', d->constantsDock->radixChar() );
  d->editor->insert( str );

  QTimer::singleShot( 0, d->editor, SLOT(setFocus()) );

  if ( ! isActiveWindow () )
    activateWindow();
}


void Crunch::expressionSelected( const QString & e )
{
  d->editor->setText( e );
  returnPressed();

  if ( ! isActiveWindow () )
    activateWindow();
}


void Crunch::functionSelected( const QString & e )
{
  if ( e.isEmpty() )
    return;

  d->editor->insert( e );
  d->editor->insert( "(" );

  QTimer::singleShot( 0, d->editor, SLOT(setFocus()) );

  if ( ! isActiveWindow () )
    activateWindow();
}


void Crunch::keypadButtonPressed( Keypad::Button b )
{
  switch ( b )
  {
    case Keypad::Key0:         d->editor->insert( "0" );      break;
    case Keypad::Key1:         d->editor->insert( "1" );      break;
    case Keypad::Key2:         d->editor->insert( "2" );      break;
    case Keypad::Key3:         d->editor->insert( "3" );      break;
    case Keypad::Key4:         d->editor->insert( "4" );      break;
    case Keypad::Key5:         d->editor->insert( "5" );      break;
    case Keypad::Key6:         d->editor->insert( "6" );      break;
    case Keypad::Key7:         d->editor->insert( "7" );      break;
    case Keypad::Key8:         d->editor->insert( "8" );      break;
    case Keypad::Key9:         d->editor->insert( "9" );      break;
    case Keypad::KeyPlus:      d->editor->insert( "+" );      break;
    case Keypad::KeyMinus:     d->editor->insert( "-" );      break;
    case Keypad::KeyTimes:     d->editor->insert( "*" );      break;
    case Keypad::KeyDivide:    d->editor->insert( "/" );      break;
    case Keypad::KeyEE:        d->editor->insert( "e" );      break;
    case Keypad::KeyLeftPar:   d->editor->insert( "(" );      break;
    case Keypad::KeyRightPar:  d->editor->insert( ")" );      break;
    case Keypad::KeyRaise:     d->editor->insert( "^" );      break;
    case Keypad::KeyPercent:   d->editor->insert( "%" );      break;
    case Keypad::KeyFactorial: d->editor->insert( "!" );      break;
    case Keypad::KeyX:         d->editor->insert( "x" );      break;
    case Keypad::KeyXEquals:   d->editor->insert( "x=" );     break;
    case Keypad::KeyPi:        d->editor->insert( "pi" );     break;
    case Keypad::KeyAns:       d->editor->insert( "ans" );    break;
    case Keypad::KeyLn:        d->editor->insert( "ln(" );    break;
    case Keypad::KeyExp:       d->editor->insert( "exp(" );   break;
    case Keypad::KeySin:       d->editor->insert( "sin(" );   break;
    case Keypad::KeyCos:       d->editor->insert( "cos(" );   break;
    case Keypad::KeyTan:       d->editor->insert( "tan(" );   break;
    case Keypad::KeyAcos:      d->editor->insert( "acos(" );  break;
    case Keypad::KeyAtan:      d->editor->insert( "atan(" );  break;
    case Keypad::KeyAsin:      d->editor->insert( "asin(" );  break;
    case Keypad::KeySqrt:      d->editor->insert( "sqrt(" );  break;

    case Keypad::KeyEquals:    d->editor->evaluate(); break;
    case Keypad::KeyClear:     clearExpression(); break;
    case Keypad::KeyRadixChar: d->editor->insert( d->keypad->radixChar() ); break;
    default:;
  }

  QTimer::singleShot( 0, d->editor, SLOT(setFocus()) );
}


void Crunch::minimizeToTray()
{
  if ( d->trayIcon )
  {
    hide();
    d->trayIcon->show();
    if ( d->trayNotify )
      QTimer::singleShot( 500, this, SLOT(showTrayMessage()) );
    d->trayNotify = false;
  }
}


void Crunch::restoreDocks()
{
  restoreState( d->settings.mainWindowState );

  if ( d->settings.showHistory )
  if ( d->settings.historyDockFloating )
  if ( ! d->historyDock->isFloating() )
  {
    d->historyDock->hide();
    d->historyDock->setFloating( true );
    d->historyDock->move( d->settings.historyDockLeft, d->settings.historyDockTop );
    d->historyDock->resize( d->settings.historyDockWidth, d->settings.historyDockHeight );
    QTimer::singleShot(0, d->historyDock, SLOT(show()));
  }

  //if ( d->settings.showFunctions )
  //if ( d->settings.functionsDockFloating )
  //if ( ! d->functionsDock->isFloating() )
  //{
  //  d->functionsDock->hide();
  //  d->functionsDock->setFloating( true );
  //  d->functionsDock->move( d->settings.functionsDockLeft, d->settings.functionsDockTop );
  //  d->functionsDock->resize( d->settings.functionsDockWidth, d->settings.functionsDockHeight );
  //  QTimer::singleShot( 0, d->functionsDock, SLOT(show()) );
  //}
  if ( d->settings.showFunctions && d->settings.functionsDockFloating && ! d->functionsDock->isFloating() )
  {
    d->functionsDock->hide();
    d->functionsDock->setFloating( true );
    d->functionsDock->move( d->settings.functionsDockLeft, d->settings.functionsDockTop );
    d->functionsDock->resize( d->settings.functionsDockWidth, d->settings.functionsDockHeight );
    d->functionsDock->setFloating( false );
    QTimer::singleShot( 0, d->functionsDock, SLOT(show()) );
  }

  if ( d->settings.showVariables )
  if ( d->settings.variablesDockFloating )
  if ( ! d->variablesDock->isFloating() )
  {
    d->variablesDock->hide();
    d->variablesDock->setFloating( true );
    d->variablesDock->move( d->settings.variablesDockLeft, d->settings.variablesDockTop );
    d->variablesDock->resize( d->settings.variablesDockWidth, d->settings.variablesDockHeight );
    QTimer::singleShot( 0, d->variablesDock, SLOT(show()) );
  }

  if ( d->settings.showConstants )
  if ( d->settings.constantsDockFloating )
  if ( !d->constantsDock->isFloating() )
  {
    d->constantsDock->hide();
    d->constantsDock->setFloating( true );
    d->constantsDock->move( d->settings.constantsDockLeft, d->settings.constantsDockTop );
    d->constantsDock->resize( d->settings.constantsDockWidth, d->settings.constantsDockHeight );
    QTimer::singleShot( 0, d->constantsDock, SLOT(show()) );
  }
}


void Crunch::restoreLastSession()
{
  if ( d->settings.historyResults.count() != d->settings.history.count() )
  {
    clearHistory();
    return;
  }

  for ( int i = 0 ; i < d->settings.history.count(); i++ )
  {
    const char * resultStr = d->settings.historyResults[i].toAscii().data();
    HNumber result( resultStr );
    if ( ! result.isNan() )
      d->display->append( d->settings.history[i], result );
    else
      d->display->appendError( d->settings.history[i], resultStr );
  }
}


void Crunch::returnPressed()
{
  QString str = d->evaluator->autoFix( d->editor->text() );
  if ( str.isEmpty() )
    return;

  d->evaluator->setExpression( str );
  d->historyDock->setHistory( d->editor->history() );

  HNumber result = d->evaluator->evalUpdateAns();
  if ( ! d->evaluator->error().isEmpty() )
  {
    d->display->appendError( str, d->evaluator->error() );
    d->editor->appendHistory( str, d->evaluator->error() );
  }
  else
  {
    d->display->append( str, result );
    d->editor->appendHistory( str, HMath::format( result ) );
    d->editor->setAnsAvailable( true );
    d->variablesDock->updateList( d->evaluator );
  }

  d->editor->setText( str );
  d->editor->selectAll();
  d->editor->stopAutoCalc();
  d->editor->stopAutoComplete();
  d->autoAns = true;

  QTimer::singleShot( 0, d->editor, SLOT(setFocus()) );

  if ( ! isActiveWindow() )
    activateWindow();
}


void Crunch::showTrayMessage()
{
  const char * msg = "SpeedCrunch is minimized.  Left click the icon to restore it or right click for options.";
  if ( d->trayIcon )
    d->trayIcon->showMessage( QString(), tr(msg), QSystemTrayIcon::NoIcon, 4000 );
}


void Crunch::textChanged()
{
  if ( d->autoAns )
  {
    QString expr = d->evaluator->autoFix( d->editor->text() );
    if ( expr.isEmpty() )
	return;
    Tokens tokens = d->evaluator->scan( expr );
    if ( tokens.count() == 1 )
    if (    tokens[0].asOperator() == Token::Plus
         || tokens[0].asOperator() == Token::Minus
         || tokens[0].asOperator() == Token::Asterisk
         || tokens[0].asOperator() == Token::Slash
         || tokens[0].asOperator() == Token::Caret )
     {
       d->autoAns = false;
       expr.prepend( "ans" );
       d->editor->setText( expr );
       d->editor->setCursorPosition( expr.length() );
     }
  }
}


void Crunch::trayIconActivated( QSystemTrayIcon::ActivationReason r )
{
  if ( r == QSystemTrayIcon::Context )
  {
    d->trayIconMenu->show();
  }
  else
  {
    showNormal();
    activateWindow();
    d->editor->setFocus();
    QTimer::singleShot( 0, d->trayIcon, SLOT(hide()) );
  }

  // work around docks does not reappear (under KDE/Linux)
//#ifdef Q_OS_UNIX
//  if ( d->historyDock->isFloating() )
//  {
//    d->historyDock->hide();
//    QTimer::singleShot( 0, d->historyDock, SLOT(show()) );
//  }
//  if ( d->functionsDock->isFloating() )
//  {
//    d->functionsDock->hide();
//    QTimer::singleShot( 0, d->functionsDock, SLOT(show()) );
//  }
//  if ( d->variablesDock->isFloating() )
//  {
//    d->variablesDock->hide();
//    QTimer::singleShot( 0, d->variablesDock, SLOT(show()) );
//  }
//  if ( d->constantsDock->isFloating() )
//  {
//    d->constantsDock->hide();
//    QTimer::singleShot( 0, d->constantsDock, SLOT(show()) );
//  }
//#endif
}


void Crunch::variableSelected( const QString & v )
{
  if ( v.isEmpty() )
    return;
  d->editor->insert( v );

  QTimer::singleShot( 0, d->editor, SLOT(setFocus()) );

  if ( ! isActiveWindow () )
    activateWindow();
}


void Crunch::radixCharAutoActivated()
{
  char c = 'C';
  d->settings.radixChar = c;
  emit radixCharChanged( c );
}


void Crunch::radixCharDotActivated()
{
  char c = '.';
  d->settings.radixChar = c;
  emit radixCharChanged( c );
}


void Crunch::radixCharCommaActivated()
{
  char c = ',';
  d->settings.radixChar = c;
  emit radixCharChanged( c );
}


// protected

void Crunch::closeEvent( QCloseEvent * e )
{
  if ( d->trayIcon )
    d->trayIcon->hide();
  d->saveSettings();
  emit quitApplication();
  QMainWindow::closeEvent( e );
}


// private

void Crunch::Private::createUI()
{
  // createActions
  actions.clearExpression       = new QAction( tr("Clear E&xpression"),        p );
  actions.clearHistory          = new QAction( tr("Clear &History"),           p );
  actions.degree                = new QAction( tr("&Degree"),                  p );
  actions.deleteAllVariables    = new QAction( tr("Delete All V&ariables"),    p );
  actions.deleteVariable        = new QAction( tr("D&elete Variable..."),      p );
  actions.digits15              = new QAction( tr("&15 Decimal Digits"),       p );
  actions.digits2               = new QAction( tr("&2 Decimal Digits"),        p );
  actions.digits3               = new QAction( tr("&3 Decimal Digits"),        p );
  actions.digits50              = new QAction( tr("&50 Decimal Digits"),       p );
  actions.digits8               = new QAction( tr("&8 Decimal Digits"),        p );
  actions.digitsAuto            = new QAction( tr("&Automatic Precision"),     p );
  actions.editCopy              = new QAction( tr("&Copy"),                    p );
  actions.editCopyResult        = new QAction( tr("Copy Last &Result"),        p );
  actions.editPaste             = new QAction( tr("&Paste"),                   p );
  actions.helpAbout             = new QAction( tr("&About"),                   p );
  actions.helpAboutQt           = new QAction( tr("About &Qt"),                p );
  actions.helpGotoWebsite       = new QAction( tr("SpeedCrunch &Web Site..."), p );
  actions.helpTipOfTheDay       = new QAction( tr("&Tip of the Day"),          p );
  actions.insertFunction        = new QAction( tr("Insert &Function..."),      p );
  actions.insertVariable        = new QAction( tr("Insert &Variable..."),      p );
  actions.optionAutoCalc        = new QAction( tr("&Partial Results"),         p );
  actions.optionAutoCompletion  = new QAction( tr("Automatic &Completion"),    p );
  actions.optionAlwaysOnTop     = new QAction( tr("Stay Always On &Top"),      p );
  actions.optionMinimizeToTray  = new QAction( tr("&Minimize To System Tray"), p );
  actions.radixCharAuto         = new QAction( tr("&Automatic"),               p );
  actions.radixCharDot          = new QAction( tr("&Dot"),                     p );
  actions.radixCharComma        = new QAction( tr("&Comma"),                   p );
  actions.radian                = new QAction( tr("&Radian"),                  p );
  actions.scrollDown            = new QAction( tr("Scroll Display Down"),      p );
  actions.scrollUp              = new QAction( tr("Scroll Display Up"),        p );
  actions.selectExpression      = new QAction( tr("&Select Expression"),       p );
  actions.sessionLoad           = new QAction( tr("&Load..."),                 p );
  actions.sessionQuit           = new QAction( tr("&Quit"),                    p );
  actions.sessionSave           = new QAction( tr("&Save..."),                 p );
  actions.showConstants         = new QAction( tr("Show &Constants"),          p );
  actions.showFullScreen        = new QAction( tr("Full &Screen Mode"),        p );
  actions.showFunctions         = new QAction( tr("Show &Functions"),          p );
  actions.showHistory           = new QAction( tr("Show &History"),            p );
  actions.showKeypad            = new QAction( tr("Show &Keypad"),             p );
  actions.showMenuBar           = new QAction( tr("Hide &Menu Bar"),           p );
  actions.showVariables         = new QAction( tr("Show &Variables"),          p );
  actions.viewBinary            = new QAction( tr("&Binary"),                  p );
  actions.viewEngineering       = new QAction( tr("&Engineering"),             p );
  actions.viewFixed             = new QAction( tr("&Fixed Decimal"),           p );
  actions.viewGeneral           = new QAction( tr("&General"),                 p );
  actions.viewHexadec           = new QAction( tr("&Hexadecimal"),             p );
  actions.viewOctal             = new QAction( tr("&Octal"),                   p );
  actions.viewScientific        = new QAction( tr("&Scientific"),              p );

  // define shortcuts
  actions.clearExpression->setShortcut (            Qt::Key_Escape   );
  actions.clearHistory->setShortcut    ( Qt::CTRL + Qt::Key_Y        );
  actions.degree->setShortcut          (            Qt::Key_F10      );
  actions.deleteVariable->setShortcut  ( Qt::CTRL + Qt::Key_D        );
  actions.editCopyResult->setShortcut  ( Qt::CTRL + Qt::Key_R        );
  actions.editCopy->setShortcut        ( Qt::CTRL + Qt::Key_C        );
  actions.editPaste->setShortcut       ( Qt::CTRL + Qt::Key_V        );
  actions.helpTipOfTheDay->setShortcut ( Qt::CTRL + Qt::Key_T        );
  actions.insertFunction->setShortcut  ( Qt::CTRL + Qt::Key_F        );
  actions.insertVariable->setShortcut  ( Qt::CTRL + Qt::Key_I        );
  actions.radian->setShortcut          (            Qt::Key_F9       );
  actions.scrollDown->setShortcut      (            Qt::Key_PageDown );
  actions.scrollUp->setShortcut        (            Qt::Key_PageUp   );
  actions.selectExpression->setShortcut( Qt::CTRL + Qt::Key_A        );
  actions.sessionLoad->setShortcut     ( Qt::CTRL + Qt::Key_L        );
  actions.sessionQuit->setShortcut     ( Qt::CTRL + Qt::Key_Q        );
  actions.sessionSave->setShortcut     ( Qt::CTRL + Qt::Key_S        );
  actions.showFullScreen->setShortcut  (            Qt::Key_F11      );
  actions.showKeypad->setShortcut      ( Qt::CTRL + Qt::Key_K        );
  actions.showMenuBar->setShortcut     ( Qt::CTRL + Qt::Key_M        );
  actions.viewBinary->setShortcut      (            Qt::Key_F5       );
  actions.viewGeneral->setShortcut     (            Qt::Key_F7       );
  actions.viewHexadec->setShortcut     (            Qt::Key_F8       );
  actions.viewOctal->setShortcut       (            Qt::Key_F6       );

  // define checkable
  actions.degree->setCheckable              ( true );
  actions.digits15->setCheckable            ( true );
  actions.digits2->setCheckable             ( true );
  actions.digits3->setCheckable             ( true );
  actions.digits50->setCheckable            ( true );
  actions.digits8->setCheckable             ( true );
  actions.digitsAuto->setCheckable          ( true );
  actions.optionAlwaysOnTop->setCheckable   ( true );
  actions.optionAutoCalc->setCheckable      ( true );
  actions.optionAutoCompletion->setCheckable( true );
  actions.optionMinimizeToTray->setCheckable( true );
  actions.radian->setCheckable              ( true );
  actions.radixCharAuto->setCheckable       ( true );
  actions.radixCharComma->setCheckable      ( true );
  actions.radixCharDot->setCheckable        ( true );
  actions.showConstants->setCheckable       ( true );
  actions.showFullScreen->setCheckable      ( true );
  actions.showFunctions->setCheckable       ( true );
  actions.showHistory->setCheckable         ( true );
  actions.showKeypad->setCheckable          ( true );
  actions.showVariables->setCheckable       ( true );
  actions.viewBinary->setCheckable          ( true );
  actions.viewEngineering->setCheckable     ( true );
  actions.viewFixed->setCheckable           ( true );
  actions.viewGeneral->setCheckable         ( true );
  actions.viewHexadec->setCheckable         ( true );
  actions.viewOctal->setCheckable           ( true );
  actions.viewScientific->setCheckable      ( true );

  // signals and slots
  QObject::connect( actions.clearHistory,         SIGNAL(activated()),     p,      SLOT(clearHistory())                );
  QObject::connect( actions.clearExpression,      SIGNAL(activated()),     p,      SLOT(clearExpression())             );
  QObject::connect( actions.degree,               SIGNAL(activated()),     p,      SLOT(degree())                      );
  QObject::connect( actions.deleteAllVariables,   SIGNAL(activated()),     p,      SLOT(deleteAllVariables())          );
  QObject::connect( actions.deleteVariable,       SIGNAL(activated()),     p,      SLOT(deleteVariable())              );
  QObject::connect( actions.digits15,             SIGNAL(activated()),     p,      SLOT(digits15())                    );
  QObject::connect( actions.digits2,              SIGNAL(activated()),     p,      SLOT(digits2())                     );
  QObject::connect( actions.digits3,              SIGNAL(activated()),     p,      SLOT(digits3())                     );
  QObject::connect( actions.digits50,             SIGNAL(activated()),     p,      SLOT(digits50())                    );
  QObject::connect( actions.digits8,              SIGNAL(activated()),     p,      SLOT(digits8())                     );
  QObject::connect( actions.digitsAuto,           SIGNAL(activated()),     p,      SLOT(digitsAuto())                  );
  QObject::connect( actions.editCopyResult,       SIGNAL(activated()),     p,      SLOT(copyResult())                  );
  QObject::connect( actions.editCopy,             SIGNAL(activated()),     editor, SLOT(copy())                        );
  QObject::connect( actions.editPaste,            SIGNAL(activated()),     editor, SLOT(paste())                       );
  QObject::connect( actions.selectExpression,     SIGNAL(activated()),     p,      SLOT(selectExpression())            );
  QObject::connect( actions.helpAboutQt,          SIGNAL(activated()),     p,      SLOT(aboutQt())                     );
  QObject::connect( actions.helpAbout,            SIGNAL(activated()),     p,      SLOT(about())                       );
  QObject::connect( actions.helpGotoWebsite,      SIGNAL(activated()),     p,      SLOT(gotoWebsite())                 );
  QObject::connect( actions.helpTipOfTheDay,      SIGNAL(activated()),     p,      SLOT(showTipOfTheDay())             );
  QObject::connect( actions.insertFunction,       SIGNAL(activated()),     p,      SLOT(insertFunction())              );
  QObject::connect( actions.insertVariable,       SIGNAL(activated()),     p,      SLOT(insertVariable())              );
  QObject::connect( actions.radian,               SIGNAL(activated()),     p,      SLOT(radian())                      );
  QObject::connect( actions.scrollDown,           SIGNAL(activated()),     p,      SLOT(scrollDown())                  );
  QObject::connect( actions.scrollUp,             SIGNAL(activated()),     p,      SLOT(scrollUp())                    );
  QObject::connect( actions.sessionLoad,          SIGNAL(activated()),     p,      SLOT(loadSession())                 );
  QObject::connect( actions.sessionQuit,          SIGNAL(activated()),     p,      SLOT(close())                       );
  QObject::connect( actions.sessionSave,          SIGNAL(activated()),     p,      SLOT(saveSession())                 );
  QObject::connect( actions.showConstants,        SIGNAL(toggled( bool )), p,      SLOT(showConstants( bool ))         );
  QObject::connect( actions.showFullScreen,       SIGNAL(toggled( bool )), p,      SLOT(showInFullScreen( bool ))      );
  QObject::connect( actions.showFunctions,        SIGNAL(toggled( bool )), p,      SLOT(showFunctions( bool ))         );
  QObject::connect( actions.showHistory,          SIGNAL(toggled( bool )), p,      SLOT(showHistory( bool ))           );
  QObject::connect( actions.showKeypad,           SIGNAL(toggled( bool )), p,      SLOT(showKeypad( bool ))            );
  QObject::connect( actions.showMenuBar,          SIGNAL(activated()),     p,      SLOT(showMenuBar())                 );
  QObject::connect( actions.showVariables,        SIGNAL(toggled( bool )), p,      SLOT(showVariables( bool ))         );
  QObject::connect( actions.viewBinary,           SIGNAL(activated()),     p,      SLOT(viewBinary())                  );
  QObject::connect( actions.viewEngineering,      SIGNAL(activated()),     p,      SLOT(viewEngineering())             );
  QObject::connect( actions.viewFixed,            SIGNAL(activated()),     p,      SLOT(viewFixed())                   );
  QObject::connect( actions.viewGeneral,          SIGNAL(activated()),     p,      SLOT(viewGeneral())                 );
  QObject::connect( actions.viewHexadec,          SIGNAL(activated()),     p,      SLOT(viewHexadec())                 );
  QObject::connect( actions.viewOctal,            SIGNAL(activated()),     p,      SLOT(viewOctal())                   );
  QObject::connect( actions.viewScientific,       SIGNAL(activated()),     p,      SLOT(viewScientific())              );
  QObject::connect( actions.optionAutoCalc,       SIGNAL(toggled( bool )), p,      SLOT(autoCalcToggled( bool ))       );
  QObject::connect( actions.optionAutoCompletion, SIGNAL(toggled( bool )), p,      SLOT(autoCompletionToggled( bool )) );
  QObject::connect( actions.optionAlwaysOnTop,    SIGNAL(toggled( bool )), p,      SLOT(alwaysOnTopToggled( bool ))    );
  QObject::connect( actions.optionMinimizeToTray, SIGNAL(toggled( bool )), p,      SLOT(minimizeToTrayToggled( bool )) );
  QObject::connect( actions.radixCharAuto,        SIGNAL(activated()),     p,      SLOT(radixCharAutoActivated())      );
  QObject::connect( actions.radixCharDot,         SIGNAL(activated()),     p,      SLOT(radixCharDotActivated())       );
  QObject::connect( actions.radixCharComma,       SIGNAL(activated()),     p,      SLOT(radixCharCommaActivated())     );

  // synchronize dock actions
  QObject::connect( constantsDock->toggleViewAction(), SIGNAL(toggled( bool )), actions.showConstants, SLOT(setChecked( bool )) );
  QObject::connect( functionsDock->toggleViewAction(), SIGNAL(toggled( bool )), actions.showFunctions, SLOT(setChecked( bool )) );
  QObject::connect( historyDock->toggleViewAction(),   SIGNAL(toggled( bool )), actions.showHistory,   SLOT(setChecked( bool )) );
  QObject::connect( variablesDock->toggleViewAction(), SIGNAL(toggled( bool )), actions.showVariables, SLOT(setChecked( bool )) );

  // construct the menu
  menus.session = new QMenu( tr("&Session"), p );
  p->menuBar()->addMenu( menus.session );
  menus.session->addAction( actions.sessionLoad );
  menus.session->addAction( actions.sessionSave );
  menus.session->addSeparator();
  menus.session->addAction( actions.sessionQuit );

  menus.edit = new QMenu( tr("&Edit"), p );
  p->menuBar()->addMenu( menus.edit );
  menus.edit->addAction( actions.editCopy         );
  menus.edit->addAction( actions.editCopyResult   );
  menus.edit->addAction( actions.editPaste        );
  menus.edit->addAction( actions.selectExpression );
  menus.edit->addSeparator();
  menus.edit->addAction( actions.insertFunction );
  menus.edit->addAction( actions.insertVariable );
  menus.edit->addSeparator();
  menus.edit->addAction( actions.deleteVariable     );
  menus.edit->addAction( actions.deleteAllVariables );
  menus.edit->addSeparator();
  menus.edit->addAction( actions.clearExpression );
  menus.edit->addAction( actions.clearHistory    );

  // format menu
  QActionGroup * formatGroup = new QActionGroup( p );
  formatGroup->addAction( actions.viewBinary );
  formatGroup->addAction( actions.viewGeneral );
  formatGroup->addAction( actions.viewFixed );
  formatGroup->addAction( actions.viewEngineering );
  formatGroup->addAction( actions.viewScientific );
  formatGroup->addAction( actions.viewOctal );
  formatGroup->addAction( actions.viewHexadec );

  digitsGroup = new QActionGroup( p );
  digitsGroup->addAction( actions.digitsAuto );
  digitsGroup->addAction( actions.digits2 );
  digitsGroup->addAction( actions.digits3 );
  digitsGroup->addAction( actions.digits8 );
  digitsGroup->addAction( actions.digits15 );
  digitsGroup->addAction( actions.digits50 );

  menus.format = new QMenu( tr("&Format"), p );
  p->menuBar()->addMenu( menus.format );
  menus.format->addAction( actions.viewBinary );
  menus.format->addAction( actions.viewOctal );

  // decimal submenu
  menus.decimal = menus.format->addMenu( tr("Decimal") );
  menus.decimal->addAction( actions.viewGeneral );
  menus.decimal->addAction( actions.viewFixed );
  menus.decimal->addAction( actions.viewEngineering );
  menus.decimal->addAction( actions.viewScientific );
  menus.decimal->addSeparator();
  menus.decimal->addAction( actions.digitsAuto );
  menus.decimal->addAction( actions.digits2 );
  menus.decimal->addAction( actions.digits3 );
  menus.decimal->addAction( actions.digits8 );
  menus.decimal->addAction( actions.digits15 );
  menus.decimal->addAction( actions.digits50 );

  menus.format->addAction( actions.viewHexadec );

  // angle menu
  angleGroup = new QActionGroup( p );
  angleGroup->addAction( actions.radian );
  angleGroup->addAction( actions.degree );

  menus.angle = new QMenu( tr("&Angle"), p );
  p->menuBar()->addMenu( menus.angle );
  menus.angle->addAction( actions.radian );
  menus.angle->addAction( actions.degree );

  // settings menu
  menus.settings = new QMenu( tr("Se&ttings"), p );
  p->menuBar()->addMenu( menus.settings );

  // layout submenu
  menus.layout = menus.settings->addMenu( tr("&Layout") );
  menus.layout->addAction( actions.showKeypad );
  menus.layout->addSeparator();
  menus.layout->addAction( actions.showHistory   );
  menus.layout->addAction( actions.showFunctions );
  menus.layout->addAction( actions.showVariables );
  menus.layout->addAction( actions.showConstants );
  menus.layout->addSeparator();
  menus.layout->addAction( actions.showMenuBar );
  menus.layout->addSeparator();
  menus.layout->addAction( actions.showFullScreen );

  // behavior submenu
  menus.behavior = menus.settings->addMenu( tr("&Behavior") );
  menus.behavior->addAction( actions.optionAutoCalc       );
  menus.behavior->addAction( actions.optionAutoCompletion );
  menus.behavior->addAction( actions.optionAlwaysOnTop    );
  menus.behavior->addAction( actions.optionMinimizeToTray );

  // radix character submenu
  QActionGroup * radixCharGroup = new QActionGroup( p );
  radixCharGroup->addAction( actions.radixCharAuto  );
  radixCharGroup->addAction( actions.radixCharDot   );
  radixCharGroup->addAction( actions.radixCharComma );

  menus.radixChar = menus.settings->addMenu( tr("Radix &Character") );
  menus.radixChar->addAction( actions.radixCharAuto  );
  menus.radixChar->addAction( actions.radixCharDot   );
  menus.radixChar->addAction( actions.radixCharComma );

  menus.help = new QMenu( tr("&Help"), p );
  p->menuBar()->addMenu( menus.help );
  menus.help->addAction( actions.helpTipOfTheDay );
  menus.help->addAction( actions.helpGotoWebsite );
  menus.help->addSeparator();
  menus.help->addAction( actions.helpAbout );
  menus.help->addAction( actions.helpAboutQt );

  p->addActions( p->menuBar()->actions() );
  p->addAction( actions.scrollDown );
  p->addAction( actions.scrollUp );

  p->setWindowIcon( QPixmap( ":/crunch.png" ) );
}


void Crunch::Private::saveSettings()
{
  // main window
  settings.mainWindowState = p->saveState();
  settings.mainWindowSize  = p->size();

  // history
  settings.history        = editor->history();
  settings.historyResults = editor->historyResults();

  // variables
  if ( settings.saveVariables )
  {
    settings.variables.clear();
    QVector<Variable> vars = evaluator->variables();
    for ( int i = 0; i < vars.count(); i++ )
    {
      QString name = vars[i].name;
      char * value = HMath::formatScientific( vars[i].value, DECPRECISION );
      settings.variables.append( QString( "%1=%2" ).arg( name ).arg( QString( value ) ) );
      free( value );
    }
  }

  // history dock
  settings.historyDockFloating   = historyDock->isFloating();
  settings.historyDockLeft       = historyDock->x();
  settings.historyDockTop        = historyDock->y();
  settings.historyDockWidth      = historyDock->width();
  settings.historyDockHeight     = historyDock->height();
  // functions dock
  settings.functionsDockFloating = functionsDock->isFloating();
  settings.functionsDockLeft     = functionsDock->x();
  settings.functionsDockTop      = functionsDock->y();
  settings.functionsDockWidth    = functionsDock->width();
  settings.functionsDockHeight   = functionsDock->height();
  // variables dock
  settings.variablesDockFloating = variablesDock->isFloating();
  settings.variablesDockLeft     = variablesDock->x();
  settings.variablesDockTop      = variablesDock->y();
  settings.variablesDockWidth    = variablesDock->width();
  settings.variablesDockHeight   = variablesDock->height();
  // constants dock
  settings.constantsDockFloating = constantsDock->isFloating();
  settings.constantsDockLeft     = constantsDock->x();
  settings.constantsDockTop      = constantsDock->y();
  settings.constantsDockWidth    = constantsDock->width();
  settings.constantsDockHeight   = constantsDock->height();

  settings.save();
}


void Crunch::setPrecision( int p )
{
  d->settings.precision = p;
  emit precisionChanged( p );
  //saveSettings();
  //applySettings();
}


void Crunch::setFormat( char c )
{
  d->settings.format = c;
  emit formatChanged( c );
  //saveSettings();
  //applySettings();
}


void Crunch::setRadixChar( char c )
{
  d->settings.radixChar = c;
  emit radixCharChanged( c );
}
