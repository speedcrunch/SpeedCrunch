/* This file is part of the SpeedCrunch project
   Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>

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

#include "configdlg.h"
#include "settings.h"

#include <QApplication>
#include <QCheckBox>
#include <QColorDialog>
#include <QDialog>
#include <QFontDialog>
#include <QLabel>
#include <qlayout.h>
#include <QPainter>
#include <QPushButton>
#include <QRadioButton>
#include <QTabWidget>
#include <QTimer>
#include <QPaintEvent>
#include <QBoxLayout>
#include <QShowEvent>
#include <QVBoxLayout>

#include <q3buttongroup.h>
#include <q3grid.h>
#include <q3hbox.h>
#include <q3vbox.h>
#include <Q3Frame>

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

  QRadioButton* standardAppearanceCheck;
  QRadioButton* customAppearanceCheck;
  Q3VBox* customBox;
  QLabel* fontLabel;
  QPushButton* chooseFontButton;
  ColorButton* textColorButton;
  ColorButton* bg1ColorButton;
  ColorButton* bg2ColorButton;

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
  QWidget* appearancePage();
  QWidget* hilitePage();
};

void ConfigDlgPrivate::loadSettings()
{
  Settings* settings = Settings::self();

  saveHistoryCheck->setChecked( settings->saveHistory );
  saveVariablesCheck->setChecked( settings->saveVariables );
  autoCompleteCheck->setChecked( settings->autoComplete );
  autoCalcCheck->setChecked( settings->autoCalc );

  standardAppearanceCheck->setChecked( !settings->customAppearance );
  customAppearanceCheck->setChecked( settings->customAppearance );
  customBox->setEnabled( settings->customAppearance );

  QString str = settings->customFont.family();
  str.append( " " );
  str.append( qApp->translate("ConfigDlgPrivate", "%1pt").arg( settings->customFont.pointSizeFloat() ) );
  fontLabel->setText( str );

  textColorButton->setColor( settings->customTextColor );
  bg1ColorButton->setColor( settings->customBackgroundColor1 );
  bg2ColorButton->setColor( settings->customBackgroundColor2 );

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
  settings->customAppearance = customAppearanceCheck->isChecked();
  settings->customTextColor = textColorButton->color();
  settings->customBackgroundColor1 = bg1ColorButton->color();
  settings->customBackgroundColor2 = bg2ColorButton->color();
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

  saveHistoryCheck = new QCheckBox( qApp->translate("ConfigDlgPrivate", "Save history on exit"), page );
  saveVariablesCheck = new QCheckBox( qApp->translate("ConfigDlgPrivate", "Save variables on exit"), page );
  autoCompleteCheck = new QCheckBox( qApp->translate("ConfigDlgPrivate", "Automatic completion"), page );
  autoCalcCheck = new QCheckBox( qApp->translate("ConfigDlgPrivate", "Automatically calculate as you type"), page );

  QVBoxLayout *layout = new QVBoxLayout;

  layout->addWidget( saveHistoryCheck );
  layout->addWidget( saveVariablesCheck );
  layout->addWidget( autoCompleteCheck );
  layout->addWidget( autoCalcCheck );

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
  Q3VBox* page = new Q3VBox( centerWidget );
  page->setSpacing( 4 );
  page->setMargin( 10 );

  Q3ButtonGroup* group = new Q3ButtonGroup( page );
  group->hide();
  standardAppearanceCheck = new QRadioButton( qApp->translate("ConfigDlgPrivate", "Standard"), page );
  customAppearanceCheck = new QRadioButton( qApp->translate("ConfigDlgPrivate", "Custom"), page );
  group->insert( standardAppearanceCheck );
  group->insert( customAppearanceCheck );

  customBox = new Q3VBox( page );
  customBox->setSpacing( 10 );

  Q3HBox* fontBox = new Q3HBox( customBox );
  fontBox->setSpacing( 5 );
  new QLabel( qApp->translate("ConfigDlgPrivate", "Font:"), fontBox );
  fontLabel = new QLabel( fontBox );
  fontLabel->setMinimumWidth( 150 );
  fontLabel->setFrameStyle( Q3Frame::StyledPanel | Q3Frame::Sunken );
  fontLabel->setLineWidth( 1 );
  fontLabel->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
  chooseFontButton = new QPushButton( qApp->translate("ConfigDlgPrivate", "Choose..."), fontBox );

  Q3Grid* colorGrid = new Q3Grid( 3, customBox );
  colorGrid->setSpacing( 5 );
  (new QLabel( qApp->translate("ConfigDlgPrivate", "Text Color:"), colorGrid ))->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
  textColorButton = new ColorButton( colorGrid );
  (new QWidget( colorGrid ))->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
  (new QLabel( qApp->translate("ConfigDlgPrivate", "Background Color 1:"), colorGrid ))->setAlignment(
    Qt::AlignVCenter | Qt::AlignRight );
  bg1ColorButton = new ColorButton( colorGrid );
  (new QWidget( colorGrid ))->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
  (new QLabel( qApp->translate("ConfigDlgPrivate", "Background Color 2:"), colorGrid ))->setAlignment(
    Qt::AlignVCenter | Qt::AlignRight );
  bg2ColorButton = new ColorButton( colorGrid );
  (new QWidget( colorGrid ))->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );

  (new QWidget( page ))->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding );

  return page;
}

QWidget* ConfigDlgPrivate::hilitePage()
{
  Q3VBox* page = new Q3VBox( centerWidget );
  page->setSpacing( 4 );
  page->setMargin( 10 );

  enableHiliteCheck = new QCheckBox( qApp->translate("ConfigDlgPrivate", "Enable syntax highlight"), page );

  Q3GroupBox* colorGroup = new Q3GroupBox( 1, Qt::Vertical, page );
  colorGroup->setTitle( qApp->translate("ConfigDlgPrivate", "Highlight Colors") );
  Q3VBox* colorBox = new Q3VBox( colorGroup );
  colorBox->setSpacing( 10 );

  Q3Grid* colorGrid = new Q3Grid( 3, colorBox );
  colorGrid->setSpacing( 5 );
  (new QLabel( qApp->translate("ConfigDlgPrivate", "Number:"), colorGrid ))->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
  numberColorButton = new ColorButton( colorGrid );
  (new QWidget( colorGrid ))->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
  (new QLabel( qApp->translate("ConfigDlgPrivate", "Function:"), colorGrid ))->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
  functionColorButton = new ColorButton( colorGrid );
  (new QWidget( colorGrid ))->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
  (new QLabel( qApp->translate("ConfigDlgPrivate", "Variable:"), colorGrid ))->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
  variableColorButton = new ColorButton( colorGrid );
  (new QWidget( colorGrid ))->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
  (new QLabel( qApp->translate("ConfigDlgPrivate", "Matched Parenthesis:"), colorGrid ))->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
  matchParColorButton = new ColorButton( colorGrid );
  (new QWidget( colorGrid ))->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );

  (new QWidget( page ))->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding );

  return page;
}

ColorButton::ColorButton( QWidget* parent, const char* name ):
QPushButton( parent, name )
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
  r.addCoords( 20, 5, -20, -5 );
  QPainter painter( this );
  painter.setPen( Qt::black );
  painter.drawRect( r );
  r.addCoords( 1, 1, -1, -1 );
  painter.fillRect( r, d->color );
}

ConfigDlg::ConfigDlg( QWidget* parent, const char* name ):
QDialog( parent, name )
{
  d = new ConfigDlgPrivate;
  setCaption( tr("Configure SpeedCrunch" ) );

  QBoxLayout* mainLayout = new QVBoxLayout( this, 11, 6 );

  d->centerWidget = new QTabWidget( this );
  mainLayout->addWidget( d->centerWidget );

  Q3HBox* buttonBox = new Q3HBox( this );
  buttonBox->setSpacing( 10 );
  mainLayout->addWidget( buttonBox );
  (new QWidget( buttonBox ))->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
  d->okButton = new QPushButton( tr("OK"), buttonBox );
  d->cancelButton = new QPushButton( tr("Cancel"), buttonBox );
  d->okButton->setDefault( true );

  d->centerWidget->addTab( d->generalPage(), tr("General") );
  d->centerWidget->addTab( d->appearancePage(), tr("Appearance") );
  d->centerWidget->addTab( d->hilitePage(), tr("Syntax Highlight") );

  // signal slot
  connect( d->standardAppearanceCheck, SIGNAL( clicked() ), SLOT( customAppearance() ) );
  connect( d->customAppearanceCheck, SIGNAL( clicked() ), SLOT( customAppearance() ) );
  connect( d->chooseFontButton, SIGNAL( clicked() ), SLOT( chooseFont() ) );
  connect( d->okButton, SIGNAL( clicked() ), SLOT( accept() ) );
  connect( d->cancelButton, SIGNAL( clicked() ), SLOT( reject() ) );

  QTimer::singleShot( 0, this, SLOT( initUI() ) );
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
  d->loadSettings();
  adjustSize();
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
