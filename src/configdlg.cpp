/* This file is part of the SpeedCrunch project
   Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2007 Helder Correia <helder.pereira.correia@gmail.com>

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

#include "configdlg.h"
#include "settings.h"

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDialog>
#include <QFontDialog>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLocale>
#include <QPainter>
#include <QPushButton>
#include <QRadioButton>
#include <QTabWidget>
#include <QTimer>
#include <QVBoxLayout>


class ColorButtonPrivate
{
public:
  QColor color;
};


class ConfigDlgPrivate
{
public:
  QTabWidget* centerWidget;

  QCheckBox* saveHistoryCheck;
  QCheckBox* saveVariablesCheck;
  QCheckBox* autoCompleteCheck;
  QCheckBox* autoCalcCheck;
  QCheckBox* minimizeToTrayCheck;

  QComboBox* decimalList;

  QRadioButton* standardAppearanceCheck;
  QRadioButton* customAppearanceCheck;
  QWidget* customBox;
  QLabel* fontLabel;
  QPushButton* chooseFontButton;
  ColorButton* textColorButton;
  ColorButton* bg1ColorButton;
  ColorButton* bg2ColorButton;
  ColorButton* errorColorButton;

  QCheckBox* enableHiliteCheck;
  ColorButton* numberColorButton;
  ColorButton* functionColorButton;
  ColorButton* variableColorButton;
  ColorButton* operatorColorButton;
  ColorButton* matchParColorButton;

  QPushButton* okButton;
  QPushButton* cancelButton;
  void loadSettings();
  void saveSettings();
  QWidget* generalPage();
  QWidget* generalPg;
  QWidget* appearancePage();
  QWidget* appearancePg;
  QWidget* hilitePage();
  QWidget* hilitePg;
};

void ConfigDlgPrivate::loadSettings()
{
  Settings* settings = Settings::self();

  saveHistoryCheck->setChecked( settings->saveHistory );
  saveVariablesCheck->setChecked( settings->saveVariables );
  autoCompleteCheck->setChecked( settings->autoComplete );
  autoCalcCheck->setChecked( settings->autoCalc );
  minimizeToTrayCheck->setChecked( settings->minimizeToTray );

  decimalList->setCurrentIndex(0);
  if( settings->decimalPoint == "." )
    decimalList->setCurrentIndex(1);
  if( settings->decimalPoint == "," )
    decimalList->setCurrentIndex(2);

  standardAppearanceCheck->setChecked( !settings->customAppearance );
  customAppearanceCheck->setChecked( settings->customAppearance );
  customBox->setEnabled( settings->customAppearance );

  QString str = settings->customFont.family();
  str.append( " " );
  str.append( QString("%1pt").arg( settings->customFont.pointSizeF() ) );
  fontLabel->setText( str );

  textColorButton->setColor( settings->customTextColor );
  bg1ColorButton->setColor( settings->customBackgroundColor1 );
  bg2ColorButton->setColor( settings->customBackgroundColor2 );
  errorColorButton->setColor( settings->customErrorColor );

  enableHiliteCheck->setChecked( settings->enableSyntaxHighlight );
  numberColorButton->setColor( settings->highlightNumberColor );
  functionColorButton->setColor( settings->highlightFunctionColor );
  variableColorButton->setColor( settings->highlightVariableColor );
  matchParColorButton->setColor( settings->matchedParenthesisColor );
}

void ConfigDlgPrivate::saveSettings()
{
  Settings* settings = Settings::self();
  settings->saveHistory = saveHistoryCheck->isChecked();
  settings->saveVariables = saveVariablesCheck->isChecked();
  settings->autoComplete = autoCompleteCheck->isChecked();
  settings->autoCalc = autoCalcCheck->isChecked();
  settings->minimizeToTray = minimizeToTrayCheck->isChecked();
  settings->decimalPoint = decimalList->currentIndex()==1 ? QString('.') :
    decimalList->currentIndex()==2 ? QString(',') : QString();
  settings->customAppearance = customAppearanceCheck->isChecked();
  settings->customTextColor = textColorButton->color();
  settings->customBackgroundColor1 = bg1ColorButton->color();
  settings->customBackgroundColor2 = bg2ColorButton->color();
  settings->customErrorColor = errorColorButton->color();
  settings->enableSyntaxHighlight = enableHiliteCheck->isChecked();
  settings->highlightNumberColor = numberColorButton->color();
  settings->highlightFunctionColor = functionColorButton->color();
  settings->highlightVariableColor = variableColorButton->color();
  settings->matchedParenthesisColor = matchParColorButton->color();
  settings->save();
}

QWidget* ConfigDlgPrivate::generalPage()
{
  QWidget *page = new QWidget( centerWidget );

  saveHistoryCheck = new QCheckBox( qApp->translate("ConfigDlgPrivate", "Save &history on exit"), page );
  saveVariablesCheck = new QCheckBox( qApp->translate("ConfigDlgPrivate", "Save &variables on exit"), page );
  autoCompleteCheck = new QCheckBox( qApp->translate("ConfigDlgPrivate", "Automatic &completion"), page );
  autoCalcCheck = new QCheckBox( qApp->translate("ConfigDlgPrivate", "Automatically calculate as you &type"), page );
  minimizeToTrayCheck = new QCheckBox( qApp->translate("ConfigDlgPrivate", "Minimi&ze to system tray"), page );

  QWidget* box = new QWidget( page );
  QHBoxLayout* boxLayout = new QHBoxLayout;
  box->setLayout( boxLayout );
  boxLayout->setMargin( 0 );

  QLabel* decimalLabel = new QLabel( box );
  decimalLabel->setText( qApp->translate("ConfigDlgPrivate", "Decimal point:") );
  //decimalLabel->setAlignment( Qt::AlignVCenter | Qt::AlignRight );

  decimalList = new QComboBox( box );
  decimalList->setEditable( false );
  decimalList->addItem( qApp->translate("ConfigDlgPrivate", "Auto detect") );
  decimalList->addItem( qApp->translate("ConfigDlgPrivate", "Use dot (.)") );
  decimalList->addItem( qApp->translate("ConfigDlgPrivate", "Use comma (,)" ) );

  boxLayout->addWidget( decimalLabel );
  boxLayout->addWidget( decimalList );
  boxLayout->addItem( new QSpacerItem( 10, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum ) );

  QVBoxLayout *layout = new QVBoxLayout;

  layout->addWidget( saveHistoryCheck );
  layout->addWidget( saveVariablesCheck );
  layout->addWidget( autoCompleteCheck );
  layout->addWidget( autoCalcCheck );
  layout->addWidget( minimizeToTrayCheck );
  layout->addItem( new QSpacerItem( 0, 20, QSizePolicy::Minimum, QSizePolicy::Minimum ) );
  layout->addWidget( box );

  QWidget *temp = new QWidget( page );
  temp->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding );
  layout->addWidget( temp );

  layout->setSpacing( 4 );
  layout->setMargin( 10 );

  page->setLayout( layout );

  return page;
}

QWidget* ConfigDlgPrivate::appearancePage()
{
  QWidget* page = new QWidget( centerWidget );
  QVBoxLayout* layout = new QVBoxLayout;
  page->setLayout( layout );

  QButtonGroup* group = new QButtonGroup( page );
  standardAppearanceCheck = new QRadioButton( qApp->translate("ConfigDlgPrivate", "S&tandard"), page );
  customAppearanceCheck = new QRadioButton( qApp->translate("ConfigDlgPrivate", "&Custom"), page );
  group->addButton( standardAppearanceCheck );
  group->addButton( customAppearanceCheck );

  customBox = new QWidget( page );
  QVBoxLayout* customLayout = new QVBoxLayout;
  customBox->setLayout( customLayout );
  customLayout->setMargin( 0 );

  QWidget* fontBox = new QWidget( customBox );
  QHBoxLayout* fontLayout = new QHBoxLayout;
  fontBox->setLayout( fontLayout );
  fontLayout->setMargin( 0 );

  QLabel* label = new QLabel( qApp->translate("ConfigDlgPrivate", "Font:"), fontBox );
  fontLabel = new QLabel( fontBox );
  fontLabel->setMinimumWidth( 150 );
  fontLabel->setFrameShape( QFrame::StyledPanel );
  fontLabel->setFrameShadow( QFrame::Sunken );
  fontLabel->setLineWidth( 1 );
  fontLabel->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
  chooseFontButton = new QPushButton( qApp->translate("ConfigDlgPrivate", "C&hoose..."), fontBox );

  fontLayout->addWidget( label );
  fontLayout->addWidget( fontLabel );
  fontLayout->addWidget( chooseFontButton );

  QWidget* colorGroup = new QWidget( page );

  QGridLayout* colorLayout = new QGridLayout;
  colorGroup->setLayout( colorLayout );

  QLabel* label1 = new QLabel( colorGroup );
  QLabel* label2 = new QLabel( colorGroup );
  QLabel* label3 = new QLabel( colorGroup );
  QLabel* label4 = new QLabel( colorGroup );

  label1->setText( qApp->translate("ConfigDlgPrivate", "Text Color:" ) );
  label2->setText( qApp->translate("ConfigDlgPrivate", "Background Color 1:" ) );
  label3->setText( qApp->translate("ConfigDlgPrivate", "Background Color 2:" ) );
  label4->setText( qApp->translate("ConfigDlgPrivate", "Error Color:" ) );

  label1->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
  label2->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
  label3->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
  label4->setAlignment( Qt::AlignVCenter | Qt::AlignRight );

  textColorButton = new ColorButton( colorGroup );
  bg1ColorButton = new ColorButton( colorGroup );
  bg2ColorButton = new ColorButton( colorGroup );
  errorColorButton = new ColorButton( colorGroup );

  colorLayout->addWidget( label1, 0, 0 );
  colorLayout->addWidget( textColorButton, 0, 1 );
  colorLayout->addWidget( label2, 1, 0 );
  colorLayout->addWidget( bg1ColorButton, 1, 1 );
  colorLayout->addWidget( label3, 2, 0 );
  colorLayout->addWidget( bg2ColorButton, 2, 1 );
  colorLayout->addWidget( label4, 3, 0 );
  colorLayout->addWidget( errorColorButton, 3, 1 );
  colorLayout->addItem( new QSpacerItem( 20, 0, QSizePolicy::MinimumExpanding,
    QSizePolicy::Minimum ), 0, 2 );

  customLayout->addWidget( fontBox );
  customLayout->addWidget( colorGroup );

  layout->addWidget( standardAppearanceCheck );
  layout->addWidget( customAppearanceCheck );
  layout->addWidget( customBox );
  layout->addItem( new QSpacerItem( 0, 20, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding ) );

  return page;
}

QWidget* ConfigDlgPrivate::hilitePage()
{
  QWidget* page = new QWidget( centerWidget );
  QVBoxLayout* layout = new QVBoxLayout;
  page->setLayout( layout );

  enableHiliteCheck = new QCheckBox( qApp->translate("ConfigDlgPrivate", "&Enable syntax highlight"), page );

  QGroupBox* colorGroup = new QGroupBox( page );
  colorGroup->setTitle( qApp->translate("ConfigDlgPrivate", "Highlight Colors") );

  QGridLayout* colorLayout = new QGridLayout;
  colorGroup->setLayout( colorLayout );

  QLabel* label1 = new QLabel( colorGroup );
  QLabel* label2 = new QLabel( colorGroup );
  QLabel* label3 = new QLabel( colorGroup );
  QLabel* label4 = new QLabel( colorGroup );

  label1->setText( qApp->translate("ConfigDlgPrivate", "Number:") );
  label2->setText( qApp->translate("ConfigDlgPrivate", "Function:") );
  label3->setText( qApp->translate("ConfigDlgPrivate", "Variable:") );
  label4->setText( qApp->translate("ConfigDlgPrivate", "Matched Parenthesis:") );

  label1->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
  label2->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
  label3->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
  label4->setAlignment( Qt::AlignVCenter | Qt::AlignRight );

  numberColorButton = new ColorButton( colorGroup );
  functionColorButton = new ColorButton( colorGroup );
  variableColorButton = new ColorButton( colorGroup );
  matchParColorButton = new ColorButton( colorGroup );

  colorLayout->addWidget( label1, 0, 0 );
  colorLayout->addWidget( numberColorButton, 0, 1 );
  colorLayout->addWidget( label2, 1, 0 );
  colorLayout->addWidget( functionColorButton, 1, 1 );
  colorLayout->addWidget( label3, 2, 0 );
  colorLayout->addWidget( variableColorButton, 2, 1 );
  colorLayout->addWidget( label4, 3, 0 );
  colorLayout->addWidget( matchParColorButton, 3, 1 );
  colorLayout->addItem( new QSpacerItem( 20, 0, QSizePolicy::MinimumExpanding,
    QSizePolicy::Minimum ), 0, 2 );

  layout->addWidget( enableHiliteCheck );
  layout->addWidget( colorGroup );
  layout->addItem( new QSpacerItem( 0, 20, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding ) );

  return page;
}

ColorButton::ColorButton( QWidget* parent ):
QPushButton( parent )
{
  d = new ColorButtonPrivate;
  connect( this, SIGNAL( clicked() ), SLOT( showColorPicker() ) );
}

ColorButton::~ColorButton()
{
  delete d;
}

void ColorButton::setColor( QColor c )
{
  d->color = c;
  update();
}

QColor ColorButton::color() const
{
  return d->color;
}

QSize ColorButton::sizeHint() const
{
  ensurePolished();

  int w = 16;
  int h = 16;
  QStyleOptionButton opt;

  opt.initFrom( this );
  QSize sh = style()->sizeFromContents( QStyle::CT_PushButton, &opt,
                                        QSize( w, h ), this);
  sh = sh.expandedTo( QApplication::globalStrut() );

  return sh;
}

void ColorButton::showColorPicker()
{
  QColor newColor = QColorDialog::getColor( d->color, this );
  if( newColor.isValid() )
    setColor( newColor );
}

void ColorButton::paintEvent( QPaintEvent* e )
{
  QPushButton::paintEvent( e );
  if( !isEnabled() ) return;

  QRect r = rect();
  r.adjust( 20, 5, -20, -5 );
  QPainter painter( this );
  painter.setPen( Qt::black );
  painter.drawRect( r );
  r.adjust( 1, 1, -1, -1 );
  painter.fillRect( r, d->color );
}

ConfigDlg::ConfigDlg( QWidget* parent ):
QDialog( parent )
{
  d = new ConfigDlgPrivate;
  setWindowTitle( tr("Configure SpeedCrunch" ) );

  d->centerWidget = new QTabWidget( this );
  d->generalPg = d->generalPage();
  d->centerWidget->addTab( d->generalPg, tr("&General") );
  d->appearancePg = d->appearancePage();
  d->centerWidget->addTab( d->appearancePg, tr("&Appearance") );
  d->hilitePg = d->hilitePage();
  d->centerWidget->addTab( d->hilitePg, tr("&Syntax Highlight") );

  QWidget* buttonBox = new QWidget( this );
  QHBoxLayout* buttonLayout = new QHBoxLayout;
  buttonLayout->setMargin( 0 );
  buttonBox->setLayout( buttonLayout );

  d->okButton = new QPushButton( tr("OK"), buttonBox );
  d->cancelButton = new QPushButton( tr("Cancel"), buttonBox );
  d->okButton->setDefault( true );

  buttonLayout->addItem( new QSpacerItem( 50, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum ) );
  buttonLayout->addWidget( d->okButton );
  buttonLayout->addWidget( d->cancelButton );

  QBoxLayout* mainLayout = new QVBoxLayout;
  mainLayout->addWidget( d->centerWidget );
  mainLayout->addWidget( buttonBox );
  setLayout( mainLayout );

  // signal slot
  connect( d->standardAppearanceCheck, SIGNAL( clicked() ), SLOT( customAppearance() ) );
  connect( d->customAppearanceCheck, SIGNAL( clicked() ), SLOT( customAppearance() ) );
  connect( d->chooseFontButton, SIGNAL( clicked() ), SLOT( chooseFont() ) );
  connect( d->okButton, SIGNAL( clicked() ), SLOT( accept() ) );
  connect( d->cancelButton, SIGNAL( clicked() ), SLOT( reject() ) );

  initUI();
}


void ConfigDlg::adaptToLanguageChange()
{
  Qt::LayoutDirection ld;
  if ( QLocale().language() == QLocale::Hebrew )
    ld = Qt::RightToLeft;
  else
    ld = Qt::LeftToRight;

  d->centerWidget->setLayoutDirection( ld );
}


void ConfigDlg::showEvent( QShowEvent* )
{
}


ConfigDlg::~ConfigDlg()
{
  delete d;
}

void ConfigDlg::initUI()
{
  setUpdatesEnabled( true );
  d->loadSettings();
  setUpdatesEnabled( true );
  adjustSize();
  emit settingsChanged();
}


void ConfigDlg::accept()
{
  d->saveSettings();
  emit settingsChanged();
  QDialog::accept();
}

void ConfigDlg::chooseFont()
{
  bool ok;
  QFont font = QFontDialog::getFont( &ok, Settings::self()->customFont, this );
  if( ok ) Settings::self()->customFont = font;
  d->loadSettings();
}

void ConfigDlg::customAppearance()
{
  Settings::self()->customAppearance = d->customAppearanceCheck->isChecked();
  d->loadSettings();
}
