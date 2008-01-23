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


#include <main/crunch.hxx>

#include <base/constants.hxx>
#include <base/evaluator.hxx>
#include <base/functions.hxx>
#include <base/settings.hxx>
#include <gui/aboutbox.hxx>
#include <gui/autohidelabel.hxx>
#include <gui/configdlg.hxx>
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
  QAction * configure;
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
  QAction * radian;
  QAction * scrollDown;
  QAction * scrollUp;
  QAction * selectExpression;
  QAction * sessionLoad;
  QAction * sessionQuit;
  QAction * sessionSave;
  QAction * showConstants;
  QAction * showFunctions;
  QAction * showFullScreen;
  QAction * showHistory;
  QAction * showKeypad;
  QAction * showVariables;
  QAction * showMenuBar;
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
  QMenu * decimal;
  QMenu * edit;
  QMenu * format;
  QMenu * help;
  QMenu * layout;
  QMenu * session;
  QMenu * settings;
};


struct CrunchPrivate
{
  Qt::WindowFlags flags;

  Actions actions;
  Menus   menus;

  QActionGroup * angleGroup;
  QActionGroup * digitsGroup;

  bool notifyMenuBarHidden;
  bool trayNotify;

  Editor *    editor;
  Evaluator * eval;
  Keypad *    keypad;
  Result *    result;

  bool            autoAns;
  AutoHideLabel * autoCalcLabel;
  TipWidget     * tip;

  QSystemTrayIcon * trayIcon;
  QMenu           * trayIconMenu;

  ConstantsDock * constantsDock;
  FunctionsDock * functionsDock;
  HistoryDock   * historyDock;
  VariablesDock * variablesDock;

  ConfigDlg         * configDlg;
  DeleteVariableDlg * deleteVariableDlg;
  InsertFunctionDlg * insertFunctionDlg;
  InsertVariableDlg * insertVariableDlg;
};


static void setWidgetLayoutAccordingToLanguageDirection( QWidget * widget )
{
  if ( QLocale().language() == QLocale::Hebrew )
    widget->setLayoutDirection( Qt::RightToLeft );
  else
    widget->setLayoutDirection( Qt::LeftToRight );
}


// public

Crunch::Crunch() : QMainWindow()
{
  d = new CrunchPrivate;

  d->eval = new Evaluator;

  d->autoAns = false;
  d->trayIcon = 0;
  d->trayNotify = true;

  d->notifyMenuBarHidden = true;
  // outer widget and layout
  QWidget * box = new QWidget( this );
  //box->setStyleSheet( "background: #202020" );
  setCentralWidget( box );

  QVBoxLayout * boxLayout = new QVBoxLayout( box );
  boxLayout->setMargin( 0 );
  boxLayout->setSpacing( 0 );

  // display
  d->result = new Result( box );
  //d->result->setStyleSheet( "background: black" );
  boxLayout->addWidget( d->result );

  // editor
  QHBoxLayout * editorLayout = new QHBoxLayout();
  editorLayout->setMargin( 5 );
  d->editor = new Editor( d->eval, box );
  d->editor->setFocus();
  d->editor->setStyleSheet( "QTextEdit { font: bold 14px }" );
  editorLayout->addWidget( d->editor );
  boxLayout->addLayout( editorLayout );

  // we need settings to be loaded before keypad and constants dock
  Settings::self()->load();

  // keypad
  QHBoxLayout * keypadLayout = new QHBoxLayout();
  d->keypad = new Keypad( box );
  d->keypad->setFocusPolicy( Qt::NoFocus );
  d->keypad->hide();
  //d->keypad->setStyleSheet( "QPushButton { font: bold }" );
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
  connect( d->constantsDock,         SIGNAL(constantSelected( const QString& )),               SLOT(constantSelected( const QString & ))   );
  connect( d->editor,                SIGNAL(autoCalcActivated( const QString& )),              SLOT(showAutoCalc( const QString & ))       );
  connect( d->editor,                SIGNAL(autoCalcDeactivated()),                            SLOT(hideAutoCalc())                        );
  connect( d->editor,                SIGNAL(returnPressed()),                                  SLOT(returnPressed())                       );
  connect( d->editor,                SIGNAL(textChanged()),                                    SLOT(textChanged())                         );
  connect( d->functionsDock,         SIGNAL(functionSelected( const QString & )),              SLOT(functionSelected( const QString & ))   );
  connect( d->historyDock,           SIGNAL(expressionSelected( const QString & )),            SLOT(expressionSelected( const QString & )) );
  connect( d->keypad,                SIGNAL(buttonPressed( Keypad::Button )),                  SLOT(keypadButtonPressed( Keypad::Button )) );
  connect( d->result,                SIGNAL(textCopied( const QString & )),         d->editor, SLOT(paste())                               );
  connect( d->result,                SIGNAL(textCopied( const QString & )),         d->editor, SLOT(setFocus())                            );
  connect( d->variablesDock,         SIGNAL(variableSelected( const QString& )),               SLOT(variableSelected( const QString & ))   );

  // Initialize settings
  d->insertFunctionDlg = new InsertFunctionDlg( this );
  d->insertVariableDlg = 0;
  d->deleteVariableDlg = 0;

  setWindowTitle( "SpeedCrunch" );
  createUI();
  Settings::self()->load();

  if ( Settings::self()->saveSession )
    restoreLastSession();

  applySettings();
  restoreDocks();

  d->configDlg = new ConfigDlg( this );
  connect( d->configDlg, SIGNAL(settingsChanged()),                         SLOT(applySettings())         );
  connect( d->configDlg, SIGNAL(settingsChanged()),       d->constantsDock, SLOT(settingsChanged())       );
  connect( d->configDlg, SIGNAL(settingsChanged()),       d->keypad,        SLOT(settingsChanged())       );
  connect( d->configDlg, SIGNAL(settingsChanged()),       d->variablesDock, SLOT(settingsChanged())       );
  connect( this,         SIGNAL(adaptToLanguageChange()), d->configDlg,     SLOT(adaptToLanguageChange()) );

  setWidgetsLayoutAccordingToLanguageDirection();

  QTimer::singleShot( 0, this, SLOT(activate()) );
}


bool Crunch::event( QEvent * e )
{
  if ( e->type() == QEvent::WindowStateChange )
  {
    if ( windowState() & Qt::WindowMinimized )
    if ( Settings::self()->minimizeToTray )
      QTimer::singleShot( 100, this, SLOT(minimizeToTray()) );
  }

  // ensure dock windows keep their state after minimize / screen switch
  Settings * set = Settings::self();
  //if ( set->showConstants ) QTimer::singleShot( 0, d->constantsDock, SLOT(show()) );
  if ( set->showFunctions ) QTimer::singleShot( 0, d->functionsDock, SLOT(show()) );
  //if ( set->showHistory   ) QTimer::singleShot( 0, d->historyDock,   SLOT(show()) );
  //if ( set->showVariables ) QTimer::singleShot( 0, d->variablesDock, SLOT(show()) );

  return QMainWindow::event( e );
}


Crunch::~Crunch()
{
  delete d->eval;
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


void Crunch::applySettings()
{
  Settings * settings = Settings::self();
  settings->load();

  if ( settings->mainWindowSize != QSize(0, 0) )
    resize( settings->mainWindowSize );

  showInFullScreen( settings->showFullScreen );

  d->editor->setAutoCompleteEnabled( settings->autoComplete );
  d->editor->setAutoCalcEnabled( settings->autoCalc );

  if( Settings::self()->angleMode == Settings::Radian )
  {
    d->actions.radian->setChecked( true );
    d->actions.degree->setChecked( false );
  }
  else if( Settings::self()->angleMode == Settings::Degree )
  {
    d->actions.radian->setChecked( false );
    d->actions.degree->setChecked( true );
  }

  //QString l = settings->language;
  //if ( l == "" )
  //{
  //  // autodetect
  //}
  //else
  //{
  //  // load specified language
  //}

  d->historyDock->clear();
  if ( settings->saveSession && settings->history.count() > 0 )
  {
    d->editor->setHistory( settings->history );
    d->editor->setHistoryResults( settings->historyResults );
  }
  if ( settings->history.count() > 0 )
    d->historyDock->setHistory( settings->history );

  if ( settings->saveVariables )
  {
    for ( int k = 0; k < settings->variables.count(); k++ )
    {
      d->eval->setExpression( settings->variables[k] );
      d->eval->eval();
      QStringList s = settings->variables[k].split( "=" );
      d->eval->set( s[0], HNumber( s[1].toAscii().data() ) );
    }
    d->variablesDock->updateList( d->eval );
  }

  d->result->setFormat( settings->format );
  d->result->setDecimalDigits( settings->precision );
  d->editor->setFormat( settings->format );
  d->editor->setDecimalDigits( settings->precision );

  d->result->setFont( QApplication::font( d->result ) );
  d->editor->setFont( QApplication::font( d->editor ) );
  d->editor->setFixedHeight( d->editor->sizeHint().height() );

  if ( settings->format == 'g' ) d->actions.viewGeneral->setChecked    ( true );
  if ( settings->format == 'f' ) d->actions.viewFixed->setChecked      ( true );
  if ( settings->format == 'n' ) d->actions.viewEngineering->setChecked( true );
  if ( settings->format == 'e' ) d->actions.viewScientific->setChecked ( true );
  if ( settings->format == 'h' ) d->actions.viewHexadec->setChecked    ( true );
  if ( settings->format == 'o' ) d->actions.viewOctal->setChecked      ( true );
  if ( settings->format == 'b' ) d->actions.viewBinary->setChecked     ( true );

  if ( settings->precision <   0 ) d->actions.digitsAuto->setChecked( true );
  if ( settings->precision ==  2 ) d->actions.digits2->setChecked   ( true );
  if ( settings->precision ==  3 ) d->actions.digits3->setChecked   ( true );
  if ( settings->precision ==  8 ) d->actions.digits8->setChecked   ( true );
  if ( settings->precision == 15 ) d->actions.digits15->setChecked  ( true );
  if ( settings->precision == 50 ) d->actions.digits50->setChecked  ( true );

  showKeypad( settings->showKeypad );
  menuBar()->setVisible( settings->showMenuBar );

  d->actions.showConstants->setChecked ( settings->showConstants  );
  d->actions.showFullScreen->setChecked( settings->showFullScreen );
  d->actions.showFunctions->setChecked ( settings->showFunctions  );
  d->actions.showHistory->setChecked   ( settings->showHistory    );
  d->actions.showVariables->setChecked ( settings->showVariables  );

  d->constantsDock->setVisible( settings->showConstants );
  d->functionsDock->setVisible( settings->showFunctions );
  d->historyDock->setVisible( settings->showHistory );
  d->variablesDock->setVisible( settings->showVariables );

  if ( settings->minimizeToTray )
  {
    if ( ! d->trayIcon && QSystemTrayIcon::isSystemTrayAvailable() )
    {
      d->trayNotify = true;
      d->trayIcon = new QSystemTrayIcon( this );
      d->trayIcon->setToolTip( tr("SpeedCrunch") );
      d->trayIcon->setIcon( QPixmap( ":/crunch.png" ) );

      d->trayIconMenu = new QMenu( this );
      d->trayIconMenu->addAction( d->actions.editCopyResult );
      d->trayIconMenu->addSeparator();
      d->trayIconMenu->addAction( d->actions.sessionQuit    );

      d->trayIcon->setContextMenu( d->trayIconMenu );
      connect( d->trayIcon, SIGNAL(activated( QSystemTrayIcon::ActivationReason )), SLOT(trayIconActivated( QSystemTrayIcon::ActivationReason )) );
    }
  }
  else
  {
    if ( d->trayIcon )
      delete d->trayIcon;
    d->trayIcon = 0;
  }

  // treat stay-always-on-top preference
  if ( settings->stayAlwaysOnTop )
    setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint );
  else
    setWindowFlags( windowFlags() & (~ Qt::WindowStaysOnTopHint) );
  show();

  // changed settings should trigger auto calc nor auto complete
  d->editor->stopAutoCalc();
  d->editor->stopAutoComplete();
}


void Crunch::clearHistory()
{
  d->result->clear();
  d->editor->clearHistory();
  d->historyDock->clear();
  Settings * set = Settings::self();
  set->history.clear();
  set->historyResults.clear();
  QTimer::singleShot( 0, d->editor, SLOT(setFocus()) );
}


void Crunch::clearExpression()
{
  d->editor->clear();
  QTimer::singleShot(0, d->editor, SLOT(setFocus()) );
}


void Crunch::configure()
{
  saveSettings();
  d->configDlg->exec();
}


void Crunch::copyResult()
{
  QClipboard * cb = QApplication::clipboard();
  HNumber num = d->eval->get( "ans" );
  Settings * set = Settings::self();
  char * strToCopy = HMath::format( num, set->format, set->precision );
  QString final( strToCopy );
  if ( Settings::self()->dot() == ',' )
    final.replace( '.', ',' );
  cb->setText( final, QClipboard::Clipboard );
  free( strToCopy );
}


void Crunch::degree()
{
  Settings::self()->angleMode = Settings::Degree;
}


void Crunch::deleteAllVariables()
{
  d->eval->clearVariables();
  d->variablesDock->updateList( d->eval );
}


void Crunch::deleteVariable()
{
  if ( ! d->deleteVariableDlg )
    d->deleteVariableDlg = new DeleteVariableDlg( d->eval, this );
  else
    d->deleteVariableDlg->updateList();

  d->deleteVariableDlg->exec();
  d->variablesDock->updateList( d->eval );
}


void Crunch::digits2()
{
  setDigits( 2 );
}


void Crunch::digits3()
{
  setDigits( 3 );
}


void Crunch::digits8()
{
  setDigits( 8 );
}


void Crunch::digits15()
{
  setDigits(15);
}


void Crunch::digits50()
{
  setDigits( 50 );
}


void Crunch::digitsAuto()
{
  setDigits( -1 );
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
    d->insertVariableDlg = new InsertVariableDlg( d->eval, this );
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
    d->result->clear();
    deleteAllVariables();
    clearHistory();
    saveSettings();
    applySettings();
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
      d->result->append( exp, num );
    else
      d->result->appendError( exp, res );
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
      d->eval->set( var, num );
  }

  file.close();

  saveSettings();
  applySettings();
}


void Crunch::radian()
{
  Settings::self()->angleMode = Settings::Radian;
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
  stream << d->result->count() << "\n";

  // expressions and results
  stream << d->result->asText() << "\n";

  // number of variables
  int noVars = d->eval->variables().count();
  stream << noVars - 2 << "\n"; // exclude pi and phi

  // variables
  for ( int i = 0; i < noVars; i++ )
  {
    Variable var = d->eval->variables()[i];
    if ( var.name != "pi" && var.name != "phi" )
      stream << var.name << "\n" << HMath::format( var.value ) << "\n";
  }

  file.close();
}


void Crunch::setWidgetsLayoutAccordingToLanguageDirection()
{
  // menu bar and menus
  setWidgetLayoutAccordingToLanguageDirection( menuBar()        );
  setWidgetLayoutAccordingToLanguageDirection( d->menus.session   );
  setWidgetLayoutAccordingToLanguageDirection( d->menus.edit      );
  setWidgetLayoutAccordingToLanguageDirection( d->menus.format      );
  setWidgetLayoutAccordingToLanguageDirection( d->menus.settings  );
  setWidgetLayoutAccordingToLanguageDirection( d->menus.help      );
  // tip of the day
  setWidgetLayoutAccordingToLanguageDirection( d->tip           );
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
  QScrollBar * sb = d->result->verticalScrollBar();
  int value = sb->value() + 40;
  sb->setValue( value );
}


void Crunch::scrollUp()
{
  QScrollBar * sb = d->result->verticalScrollBar();
  int value = sb->value() - 40;
  sb->setValue( value );
}


void Crunch::showMenuBar()
{
  menuBar()->setVisible( ! menuBar()->isVisible() );
  Settings::self()->showMenuBar = menuBar()->isVisible();

  if ( ! menuBar()->isVisible() && d->notifyMenuBarHidden )
  {
    showMenuBarTip();
    d->notifyMenuBarHidden = false;
  }

  d->result->scrollEnd();
}


void Crunch::showAutoCalc( const QString & msg )
{
  QPoint p = d->editor->mapToParent( QPoint(0, 0) );
  d->autoCalcLabel->move( p );
  d->autoCalcLabel->showText( msg );
}


void Crunch::showConstants( bool b )
{
  Settings * set = Settings::self();
  set->showConstants = b;
  if ( b )
    d->constantsDock->show();
  else
    d->constantsDock->hide();
  //saveSettings();
  //applySettings();
  //d->constantsDock->raise();
}


void Crunch::showInFullScreen( bool b )
{
  b ? showFullScreen() : showNormal();
  Settings::self()->showFullScreen = b;
}


void Crunch::showFunctions( bool b )
{
  Settings * set = Settings::self();
  if ( set->showFunctions == b )
    return;
  set->showFunctions = b;
  d->actions.showFunctions->setChecked( b );
  d->functionsDock->setVisible( b );
}


void Crunch::showHistory( bool b )
{
  Settings* set = Settings::self();
  set->showHistory = b;
  if ( b )
    d->historyDock->show();
  else
    d->historyDock->hide();
  //saveSettings();
  //applySettings();
  //d->historyDock->raise();
}


void Crunch::showKeypad( bool b )
{
  Settings * settings = Settings::self();
  settings->showKeypad = b;
  d->keypad->setVisible( b );
  d->actions.showKeypad->setChecked( b );
  d->result->scrollEnd();
}


void Crunch::showMenuBarTip()
{
  QString msg = tr("The menu bar is now hidden. "
                   "To make it visible again, press Ctrl+M.");

  QPoint p = mapFromGlobal( d->result->mapToGlobal( QPoint(0, 0) ) ) += QPoint(5, 5);
  d->tip->move( p );
  d->tip->resize( 345, d->tip->sizeHint().height() );
  d->tip->showText( msg, tr("Warning") );
}


void Crunch::showTipOfTheDay()
{
  QPoint p = mapFromGlobal( d->result->mapToGlobal( QPoint(0, 0) ) ) += QPoint(5, 5);
  d->tip->move( p );
  d->tip->resize( 345, d->tip->sizeHint().height() );

  int tipNo = rand() % 5;
  QString msg;
  switch ( tipNo )
  {
    case 0:
      msg = tr("You can customize the syntax highlight colors. "
               "Use menu <i>Settings, Configure</i>, and then from the "
               "configuration dialog, choose tab <i>Syntax Highlight</i>." );
      break;
    case 1:
      msg = tr("To insert a function using keyboard, use Ctrl+F shorcut. "
               "From the dialog, you can choose the function you want to "
               "insert." );
      break;
    case 2:
      msg = tr("To insert a variable using keyboard, use Ctrl+I shorcut. "
               "From the dialog, you can choose the variable you want to "
               "insert." );
      break;
    case 3:
      msg = tr("Use variable <i>pi</i> to use pi constant." );
      break;
    case 4:
      msg = tr("Use <i>;</i> (semicolon) to separate the parameters in "
               "functions." );
      break;
    default:
      break;
  }

  d->tip->showText( msg, tr("Tip of the day") );
}


void Crunch::showVariables( bool b)
{
  Settings * settings = Settings::self();
  settings->showVariables = b;
  if ( b )
    d->variablesDock->show();
  else
    d->variablesDock->hide();
  //saveSettings();
  //applySettings();
  //d->variablesDock->raise();
}


void Crunch::viewBinary()
{
  d->digitsGroup->setDisabled( true );
  setView( 'b' );
}


void Crunch::viewEngineering()
{
  d->digitsGroup->setEnabled( true );
  setView( 'n' );
}


void Crunch::viewFixed()
{
  d->digitsGroup->setEnabled( true );
  setView( 'f' );
}


void Crunch::viewGeneral()
{
  d->digitsGroup->setEnabled( true );
  setView( 'g' );
}


void Crunch::viewHexadec()
{
  d->digitsGroup->setDisabled( true );
  setView( 'h' );
}


void Crunch::viewOctal()
{
  d->digitsGroup->setDisabled( true );
  setView( 'o' );
}


void Crunch::viewScientific()
{
  d->digitsGroup->setEnabled( true );
  setView( 'e' );
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
  str.replace( '.', Settings::self()->dot() );
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
    case Keypad::KeyEquals:    d->editor->evaluate();         break;
    case Keypad::KeyClear:     clearExpression();             break;
    case Keypad::KeyDot:       d->editor->insert( Settings::self()->dot() ); break;
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
  Settings* settings = Settings::self();

  restoreState( settings->mainWindowState );

  if ( settings->showHistory )
  if ( settings->historyDockFloating )
  if ( ! d->historyDock->isFloating() )
  {
    d->historyDock->hide();
    d->historyDock->setFloating( true );
    d->historyDock->move( settings->historyDockLeft, settings->historyDockTop );
    d->historyDock->resize( settings->historyDockWidth, settings->historyDockHeight );
    QTimer::singleShot(0, d->historyDock, SLOT(show()));
  }

  //if ( settings->showFunctions )
  //if ( settings->functionsDockFloating )
  //if ( ! d->functionsDock->isFloating() )
  //{
  //  d->functionsDock->hide();
  //  d->functionsDock->setFloating( true );
  //  d->functionsDock->move( settings->functionsDockLeft, settings->functionsDockTop );
  //  d->functionsDock->resize( settings->functionsDockWidth, settings->functionsDockHeight );
  //  QTimer::singleShot( 0, d->functionsDock, SLOT(show()) );
  //}
  if ( settings->showFunctions && settings->functionsDockFloating && ! d->functionsDock->isFloating() )
  {
    d->functionsDock->hide();
    d->functionsDock->setFloating( true );
    d->functionsDock->move( settings->functionsDockLeft, settings->functionsDockTop );
    d->functionsDock->resize( settings->functionsDockWidth, settings->functionsDockHeight );
    d->functionsDock->setFloating( false );
    QTimer::singleShot( 0, d->functionsDock, SLOT(show()) );
  }

  if ( settings->showVariables )
  if ( settings->variablesDockFloating )
  if ( ! d->variablesDock->isFloating() )
  {
    d->variablesDock->hide();
    d->variablesDock->setFloating( true );
    d->variablesDock->move( settings->variablesDockLeft, settings->variablesDockTop );
    d->variablesDock->resize( settings->variablesDockWidth, settings->variablesDockHeight );
    QTimer::singleShot( 0, d->variablesDock, SLOT(show()) );
  }

  if ( settings->showConstants )
  if ( settings->constantsDockFloating )
  if ( !d->constantsDock->isFloating() )
  {
    d->constantsDock->hide();
    d->constantsDock->setFloating( true );
    d->constantsDock->move( settings->constantsDockLeft, settings->constantsDockTop );
    d->constantsDock->resize( settings->constantsDockWidth, settings->constantsDockHeight );
    QTimer::singleShot( 0, d->constantsDock, SLOT(show()) );
  }
}


void Crunch::restoreLastSession()
{
  Settings * set = Settings::self();
  if ( set->historyResults.count() != set->history.count() )
  {
    clearHistory();
    return;
  }

  for ( int i = 0 ; i < set->history.count(); i++ )
  {
    const char * resultStr = set->historyResults[i].toAscii().data();
    HNumber result( resultStr );
    if ( ! result.isNan() )
      d->result->append( set->history[i], result );
    else
      d->result->appendError( set->history[i], resultStr );
  }
}


void Crunch::returnPressed()
{
  QString str = Evaluator::autoFix( d->editor->text() );
  if ( str.isEmpty() )
    return;

  d->eval->setExpression( str );
  d->historyDock->setHistory( d->editor->history() );

  HNumber result = d->eval->evalUpdateAns();
  if ( ! d->eval->error().isEmpty() )
  {
    d->result->appendError( str, d->eval->error() );
    d->editor->appendHistory( str, d->eval->error() );
  }
  else
  {
    d->result->append( str, result );
    d->editor->appendHistory( str, HMath::format( result ) );
    d->editor->setAnsAvailable( true );
    d->variablesDock->updateList( d->eval );
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
  const char * msg = "SpeedCrunch is minimized.\nLeft click on the icon to restore it or right click for options menu.";
  if ( d->trayIcon )
    d->trayIcon->showMessage( QString(), tr(msg), QSystemTrayIcon::NoIcon, 4000 );
}


void Crunch::textChanged()
{
  if ( d->autoAns )
  {
    QString expr = Evaluator::autoFix( d->editor->text() );
    if ( expr.isEmpty() )
	return;
    Tokens tokens = Evaluator::scan( expr );
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


// protected

void Crunch::closeEvent( QCloseEvent * e )
{
  if ( d->trayIcon )
    d->trayIcon->hide();
  saveSettings();
  emit quitApplication();
  QMainWindow::closeEvent( e );
}


// private

void Crunch::createUI()
{
  // createActions
  d->actions.clearExpression    = new QAction( tr("Clear E&xpression"),        this );
  d->actions.clearHistory       = new QAction( tr("Clear &History"),           this );
  d->actions.configure          = new QAction( tr("Confi&gure..."),            this );
  d->actions.degree             = new QAction( tr("&Degree"),                  this );
  d->actions.deleteAllVariables = new QAction( tr("Delete All V&ariables"),    this );
  d->actions.deleteVariable     = new QAction( tr("D&elete Variable..."),      this );
  d->actions.digits15           = new QAction( tr("&15 Decimal Digits"),       this );
  d->actions.digits2            = new QAction( tr("&2 Decimal Digits"),        this );
  d->actions.digits3            = new QAction( tr("&3 Decimal Digits"),        this );
  d->actions.digits50           = new QAction( tr("&50 Decimal Digits"),       this );
  d->actions.digits8            = new QAction( tr("&8 Decimal Digits"),        this );
  d->actions.digitsAuto         = new QAction( tr("&Automatic Precision"),     this );
  d->actions.editCopy           = new QAction( tr("&Copy"),                    this );
  d->actions.editCopyResult     = new QAction( tr("Copy Last &Result"),        this );
  d->actions.editPaste          = new QAction( tr("&Paste"),                   this );
  d->actions.helpAbout          = new QAction( tr("&About"),                   this );
  d->actions.helpAboutQt        = new QAction( tr("About &Qt"),                this );
  d->actions.helpGotoWebsite    = new QAction( tr("SpeedCrunch &Web Site..."), this );
  d->actions.helpTipOfTheDay    = new QAction( tr("&Tip of the Day"),          this );
  d->actions.insertFunction     = new QAction( tr("Insert &Function..."),      this );
  d->actions.insertVariable     = new QAction( tr("Insert &Variable..."),      this );
  d->actions.radian             = new QAction( tr("&Radian"),                  this );
  d->actions.scrollDown         = new QAction( tr("Scroll Display Down"),      this );
  d->actions.scrollUp           = new QAction( tr("Scroll Display Up"),        this );
  d->actions.selectExpression   = new QAction( tr("&Select Expression"),       this );
  d->actions.sessionLoad        = new QAction( tr("&Load..."),                 this );
  d->actions.sessionQuit        = new QAction( tr("&Quit"),                    this );
  d->actions.sessionSave        = new QAction( tr("&Save..."),                 this );
  d->actions.showConstants      = new QAction( tr("Show &Constants"),          this );
  d->actions.showFullScreen     = new QAction( tr("Full &Screen Mode"),        this );
  d->actions.showFunctions      = new QAction( tr("Show &Functions"),          this );
  d->actions.showHistory        = new QAction( tr("Show &History"),            this );
  d->actions.showKeypad         = new QAction( tr("Show &Keypad"),             this );
  d->actions.showMenuBar        = new QAction( tr("Hide &Menu Bar"),           this );
  d->actions.showVariables      = new QAction( tr("Show &Variables"),          this );
  d->actions.viewBinary         = new QAction( tr("&Binary"),                  this );
  d->actions.viewEngineering    = new QAction( tr("&Engineering"),             this );
  d->actions.viewFixed          = new QAction( tr("&Fixed Decimal"),           this );
  d->actions.viewGeneral        = new QAction( tr("&General"),                 this );
  d->actions.viewHexadec        = new QAction( tr("&Hexadecimal"),             this );
  d->actions.viewOctal          = new QAction( tr("&Octal"),                   this );
  d->actions.viewScientific     = new QAction( tr("&Scientific"),              this );

  // define shortcuts
  d->actions.clearExpression->setShortcut (            Qt::Key_Escape   );
  d->actions.clearHistory->setShortcut    ( Qt::CTRL + Qt::Key_Y        );
  d->actions.degree->setShortcut          (            Qt::Key_F10      );
  d->actions.deleteVariable->setShortcut  ( Qt::CTRL + Qt::Key_D        );
  d->actions.editCopyResult->setShortcut  ( Qt::CTRL + Qt::Key_R        );
  d->actions.editCopy->setShortcut        ( Qt::CTRL + Qt::Key_C        );
  d->actions.editPaste->setShortcut       ( Qt::CTRL + Qt::Key_V        );
  d->actions.helpTipOfTheDay->setShortcut ( Qt::CTRL + Qt::Key_T        );
  d->actions.insertFunction->setShortcut  ( Qt::CTRL + Qt::Key_F        );
  d->actions.insertVariable->setShortcut  ( Qt::CTRL + Qt::Key_I        );
  d->actions.radian->setShortcut          (            Qt::Key_F9       );
  d->actions.scrollDown->setShortcut      (            Qt::Key_PageDown );
  d->actions.scrollUp->setShortcut        (            Qt::Key_PageUp   );
  d->actions.selectExpression->setShortcut( Qt::CTRL + Qt::Key_A        );
  d->actions.sessionLoad->setShortcut     ( Qt::CTRL + Qt::Key_L        );
  d->actions.sessionQuit->setShortcut     ( Qt::CTRL + Qt::Key_Q        );
  d->actions.sessionSave->setShortcut     ( Qt::CTRL + Qt::Key_S        );
  d->actions.showFullScreen->setShortcut  (            Qt::Key_F11      );
  d->actions.showKeypad->setShortcut      ( Qt::CTRL + Qt::Key_K        );
  d->actions.showMenuBar->setShortcut     ( Qt::CTRL + Qt::Key_M        );
  d->actions.viewBinary->setShortcut      (            Qt::Key_F5       );
  d->actions.viewGeneral->setShortcut     (            Qt::Key_F7       );
  d->actions.viewHexadec->setShortcut     (            Qt::Key_F8       );
  d->actions.viewOctal->setShortcut       (            Qt::Key_F6       );

  // define checkable
  d->actions.degree->setCheckable         ( true );
  d->actions.digits15->setCheckable       ( true );
  d->actions.digits2->setCheckable        ( true );
  d->actions.digits3->setCheckable        ( true );
  d->actions.digits50->setCheckable       ( true );
  d->actions.digits8->setCheckable        ( true );
  d->actions.digitsAuto->setCheckable     ( true );
  d->actions.radian->setCheckable         ( true );
  d->actions.showConstants->setCheckable  ( true );
  d->actions.showFullScreen->setCheckable ( true );
  d->actions.showFunctions->setCheckable  ( true );
  d->actions.showHistory->setCheckable    ( true );
  d->actions.showKeypad->setCheckable     ( true );
  d->actions.showVariables->setCheckable  ( true );
  d->actions.viewBinary->setCheckable     ( true );
  d->actions.viewEngineering->setCheckable( true );
  d->actions.viewFixed->setCheckable      ( true );
  d->actions.viewGeneral->setCheckable    ( true );
  d->actions.viewHexadec->setCheckable    ( true );
  d->actions.viewOctal->setCheckable      ( true );
  d->actions.viewScientific->setCheckable ( true );

  // signals and slots
  connect( d->actions.clearHistory,         SIGNAL(activated()),     this,      SLOT(clearHistory())           );
  connect( d->actions.clearExpression,      SIGNAL(activated()),     this,      SLOT(clearExpression())        );
  connect( d->actions.configure,            SIGNAL(activated()),     this,      SLOT(configure())              );
  connect( d->actions.degree,               SIGNAL(activated()),     this,      SLOT(degree())                 );
  connect( d->actions.deleteAllVariables,   SIGNAL(activated()),     this,      SLOT(deleteAllVariables())     );
  connect( d->actions.deleteVariable,       SIGNAL(activated()),     this,      SLOT(deleteVariable())         );
  connect( d->actions.digits15,             SIGNAL(activated()),     this,      SLOT(digits15())               );
  connect( d->actions.digits2,              SIGNAL(activated()),     this,      SLOT(digits2())                );
  connect( d->actions.digits3,              SIGNAL(activated()),     this,      SLOT(digits3())                );
  connect( d->actions.digits50,             SIGNAL(activated()),     this,      SLOT(digits50())               );
  connect( d->actions.digits8,              SIGNAL(activated()),     this,      SLOT(digits8())                );
  connect( d->actions.digitsAuto,           SIGNAL(activated()),     this,      SLOT(digitsAuto())             );
  connect( d->actions.editCopyResult,       SIGNAL(activated()),     this,      SLOT(copyResult())             );
  connect( d->actions.editCopy,             SIGNAL(activated()),     d->editor, SLOT(copy())                   );
  connect( d->actions.editPaste,            SIGNAL(activated()),     d->editor, SLOT(paste())                  );
  connect( d->actions.selectExpression,     SIGNAL(activated()),     this,      SLOT(selectExpression())       );
  connect( d->actions.helpAboutQt,          SIGNAL(activated()),     this,      SLOT(aboutQt())                );
  connect( d->actions.helpAbout,            SIGNAL(activated()),     this,      SLOT(about())                  );
  connect( d->actions.helpGotoWebsite,      SIGNAL(activated()),     this,      SLOT(gotoWebsite())            );
  connect( d->actions.helpTipOfTheDay,      SIGNAL(activated()),     this,      SLOT(showTipOfTheDay())        );
  connect( d->actions.insertFunction,       SIGNAL(activated()),     this,      SLOT(insertFunction())         );
  connect( d->actions.insertVariable,       SIGNAL(activated()),     this,      SLOT(insertVariable())         );
  connect( d->actions.radian,               SIGNAL(activated()),     this,      SLOT(radian())                 );
  connect( d->actions.scrollDown,           SIGNAL(activated()),     this,      SLOT(scrollDown())             );
  connect( d->actions.scrollUp,             SIGNAL(activated()),     this,      SLOT(scrollUp())               );
  connect( d->actions.sessionLoad,          SIGNAL(activated()),     this,      SLOT(loadSession())            );
  connect( d->actions.sessionQuit,          SIGNAL(activated()),     this,      SLOT(close())                  );
  connect( d->actions.sessionSave,          SIGNAL(activated()),     this,      SLOT(saveSession())            );
  connect( d->actions.showConstants,        SIGNAL(toggled( bool )), this,      SLOT(showConstants( bool ))    );
  connect( d->actions.showFullScreen,       SIGNAL(toggled( bool )), this,      SLOT(showInFullScreen( bool )) );
  connect( d->actions.showFunctions,        SIGNAL(toggled( bool )), this,      SLOT(showFunctions( bool ))    );
  connect( d->actions.showHistory,          SIGNAL(toggled( bool )), this,      SLOT(showHistory( bool ))      );
  connect( d->actions.showKeypad,           SIGNAL(toggled( bool )), this,      SLOT(showKeypad( bool ))       );
  connect( d->actions.showMenuBar,          SIGNAL(activated()),     this,      SLOT(showMenuBar())            );
  connect( d->actions.showVariables,        SIGNAL(toggled( bool )), this,      SLOT(showVariables( bool ))    );
  connect( d->actions.viewBinary,           SIGNAL(activated()),     this,      SLOT(viewBinary())             );
  connect( d->actions.viewEngineering,      SIGNAL(activated()),     this,      SLOT(viewEngineering())        );
  connect( d->actions.viewFixed,            SIGNAL(activated()),     this,      SLOT(viewFixed())              );
  connect( d->actions.viewGeneral,          SIGNAL(activated()),     this,      SLOT(viewGeneral())            );
  connect( d->actions.viewHexadec,          SIGNAL(activated()),     this,      SLOT(viewHexadec())            );
  connect( d->actions.viewOctal,            SIGNAL(activated()),     this,      SLOT(viewOctal())              );
  connect( d->actions.viewScientific,       SIGNAL(activated()),     this,      SLOT(viewScientific())         );

  // synchronize dock actions
  connect( d->constantsDock->toggleViewAction(), SIGNAL(toggled( bool )), d->actions.showConstants, SLOT(setChecked( bool )) );
  connect( d->functionsDock->toggleViewAction(), SIGNAL(toggled( bool )), d->actions.showFunctions, SLOT(setChecked( bool )) );
  connect( d->historyDock->toggleViewAction(),   SIGNAL(toggled( bool )), d->actions.showHistory,   SLOT(setChecked( bool )) );
  connect( d->variablesDock->toggleViewAction(), SIGNAL(toggled( bool )), d->actions.showVariables, SLOT(setChecked( bool )) );

  // construct the menu
  d->menus.session = new QMenu( tr("&Session"), this );
  menuBar()->addMenu( d->menus.session );
  d->menus.session->addAction( d->actions.sessionLoad );
  d->menus.session->addAction( d->actions.sessionSave );
  d->menus.session->addSeparator();
  d->menus.session->addAction( d->actions.sessionQuit );

  d->menus.edit = new QMenu( tr("&Edit"), this );
  menuBar()->addMenu( d->menus.edit );
  d->menus.edit->addAction( d->actions.editCopy           );
  d->menus.edit->addAction( d->actions.editCopyResult     );
  d->menus.edit->addAction( d->actions.editPaste          );
  d->menus.edit->addAction( d->actions.selectExpression   );
  d->menus.edit->addSeparator();
  d->menus.edit->addAction( d->actions.insertFunction     );
  d->menus.edit->addAction( d->actions.insertVariable     );
  d->menus.edit->addSeparator();
  d->menus.edit->addAction( d->actions.deleteVariable     );
  d->menus.edit->addAction( d->actions.deleteAllVariables );
  d->menus.edit->addSeparator();
  d->menus.edit->addAction( d->actions.clearExpression    );
  d->menus.edit->addAction( d->actions.clearHistory       );

  QActionGroup * formatGroup = new QActionGroup( this );
  formatGroup->addAction( d->actions.viewBinary );
  // decimal submenu
    formatGroup->addAction( d->actions.viewGeneral );
    formatGroup->addAction( d->actions.viewFixed );
    formatGroup->addAction( d->actions.viewEngineering );
    formatGroup->addAction( d->actions.viewScientific );
  formatGroup->addAction( d->actions.viewOctal );
  formatGroup->addAction( d->actions.viewHexadec );

  d->digitsGroup = new QActionGroup( this );
  d->digitsGroup->addAction( d->actions.digitsAuto );
  d->digitsGroup->addAction( d->actions.digits2 );
  d->digitsGroup->addAction( d->actions.digits3 );
  d->digitsGroup->addAction( d->actions.digits8 );
  d->digitsGroup->addAction( d->actions.digits15 );
  d->digitsGroup->addAction( d->actions.digits50 );

  d->angleGroup = new QActionGroup( this );
  d->angleGroup->addAction( d->actions.radian );
  d->angleGroup->addAction( d->actions.degree );

  d->menus.format = new QMenu( tr("&Format"), this );
  menuBar()->addMenu( d->menus.format );
  d->menus.format->addAction( d->actions.viewBinary );
  d->menus.format->addAction( d->actions.viewOctal );
  d->menus.decimal = d->menus.format->addMenu( tr("Decimal") );
    d->menus.decimal->addAction( d->actions.viewGeneral );
    d->menus.decimal->addAction( d->actions.viewFixed );
    d->menus.decimal->addAction( d->actions.viewEngineering );
    d->menus.decimal->addAction( d->actions.viewScientific );
    d->menus.decimal->addSeparator();
    d->menus.decimal->addAction( d->actions.digitsAuto );
    d->menus.decimal->addAction( d->actions.digits2 );
    d->menus.decimal->addAction( d->actions.digits3 );
    d->menus.decimal->addAction( d->actions.digits8 );
    d->menus.decimal->addAction( d->actions.digits15 );
    d->menus.decimal->addAction( d->actions.digits50 );
  d->menus.format->addAction( d->actions.viewHexadec );

  d->menus.angle = new QMenu( tr("&Angle"), this );
  menuBar()->addMenu( d->menus.angle );
  d->menus.angle->addAction( d->actions.radian );
  d->menus.angle->addAction( d->actions.degree );

  d->menus.settings = new QMenu( tr("Se&ttings"), this );
  menuBar()->addMenu( d->menus.settings );
  d->menus.layout = d->menus.settings->addMenu( tr("Layout") );
    d->menus.layout->addAction( d->actions.showKeypad     );
    d->menus.layout->addSeparator();
    d->menus.layout->addAction( d->actions.showHistory    );
    d->menus.layout->addAction( d->actions.showFunctions  );
    d->menus.layout->addAction( d->actions.showVariables  );
    d->menus.layout->addAction( d->actions.showConstants  );
    d->menus.layout->addSeparator();
    d->menus.layout->addAction( d->actions.showMenuBar    );
    d->menus.layout->addSeparator();
    d->menus.layout->addAction( d->actions.showFullScreen );
  d->menus.settings->addAction( d->actions.configure      );

  d->menus.help = new QMenu( tr("&Help"), this );
  menuBar()->addMenu( d->menus.help );
  d->menus.help->addAction( d->actions.helpTipOfTheDay );
  d->menus.help->addAction( d->actions.helpGotoWebsite );
  d->menus.help->addSeparator();
  d->menus.help->addAction( d->actions.helpAbout );
  d->menus.help->addAction( d->actions.helpAboutQt );

  this->addActions( menuBar()->actions() );
  this->addAction( d->actions.scrollDown );
  this->addAction( d->actions.scrollUp );

  setWindowIcon( QPixmap( ":/crunch.png" ) );
}


void Crunch::saveSettings()
{
  Settings * settings = Settings::self();

  settings->mainWindowSize = size();

  settings->history = d->editor->history();
  settings->historyResults = d->editor->historyResults();

  if( settings->saveVariables )
  {
    settings->variables.clear();
    QVector<Variable> vars = d->eval->variables();
    for ( int i = 0; i < vars.count(); i++ )
    {
      QString name = vars[i].name;
      char * value = HMath::formatScientific( vars[i].value, DECPRECISION );
      settings->variables.append( QString("%1=%2").arg( name ).arg( QString( value ) ) );
      free( value );
    }
  }

  // docks
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


void Crunch::setDigits( int i )
{
  Settings::self()->precision = i;
  saveSettings();
  applySettings();
}


void Crunch::setView( char c )
{
  Settings::self()->format = c;
  saveSettings();
  applySettings();
}
