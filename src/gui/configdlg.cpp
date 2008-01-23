/* This file is part of the SpeedCrunch project
   Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2007-2008 Helder Correia <helder.pereira.correia@gmail.com>

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

#include <base/settings.hxx>
#include <gui/configdlg.hxx>

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

#include <qwwcolorbutton.h>


struct ConfigDlgPrivate
{
  QCheckBox   * autoCalcCheck;
  QCheckBox   * autoCompleteCheck;
  QCheckBox   * minimizeToTrayCheck;
  QCheckBox   * saveSessionCheck;
  QCheckBox   * stayAlwaysOnTopCheck;
  QComboBox   * decimalList;
  //QComboBox *   languageList;
  QPushButton * cancelButton;
  QPushButton * okButton;
  QTabWidget  * centerWidget;
  QWidget     * generalPage();
  QWidget     * generalPg;

  void loadSettings();
  void saveSettings();
};


void ConfigDlgPrivate::loadSettings()
{
  Settings * settings = Settings::self();

  saveSessionCheck->setChecked( settings->saveSession );
  autoCompleteCheck->setChecked( settings->autoComplete );
  autoCalcCheck->setChecked( settings->autoCalc );
  minimizeToTrayCheck->setChecked( settings->minimizeToTray );
  stayAlwaysOnTopCheck->setChecked( settings->stayAlwaysOnTop );

  decimalList->setCurrentIndex( 0 );
  if      ( settings->autoDetectDot ) decimalList->setCurrentIndex(0);
  else if ( settings->dot() == '.'  ) decimalList->setCurrentIndex(1);
  else if ( settings->dot() == ','  ) decimalList->setCurrentIndex(2);
}

void ConfigDlgPrivate::saveSettings()
{
  Settings * s = Settings::self();
  s->saveSession     = saveSessionCheck->isChecked();
  s->autoComplete    = autoCompleteCheck->isChecked();
  s->autoCalc        = autoCalcCheck->isChecked();
  s->minimizeToTray  = minimizeToTrayCheck->isChecked();
  s->stayAlwaysOnTop = stayAlwaysOnTopCheck->isChecked();

  s->setDot( decimalList->currentIndex() == 1 ?
               "." : decimalList->currentIndex() == 2 ?
                       "," : QString());
  s->save();
}

QWidget* ConfigDlgPrivate::generalPage()
{
  QWidget * page = new QWidget( centerWidget );

  autoCalcCheck        = new QCheckBox( qApp->translate("ConfigDlgPrivate", "Automatically calculate as you &type"), page );
  autoCompleteCheck    = new QCheckBox( qApp->translate("ConfigDlgPrivate", "Automatic &completion"),                page );
  minimizeToTrayCheck  = new QCheckBox( qApp->translate("ConfigDlgPrivate", "Minimi&ze to system tray"),             page );
  saveSessionCheck     = new QCheckBox( qApp->translate("ConfigDlgPrivate", "Restore last &session on start-up"),    page );
  stayAlwaysOnTopCheck = new QCheckBox( qApp->translate("ConfigDlgPrivate", "Stay always-on-&top"),                  page );

  QWidget * box = new QWidget( page );
  QVBoxLayout* boxLayout = new QVBoxLayout;
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

  //QLabel* languageLabel = new QLabel( box );
  //languageLabel->setText( qApp->translate("ConfigDlgPrivate", "Language:") );

  //languageList = new QComboBox( box );
  //languageList->setEditable( false );
  //languageList->addItem( qApp->translate("ConfigDlgPrivate", "Auto detect") );
  //languageList->addItem( qApp->translate("ConfigDlgPrivate", "English") );
  //languageList->addItem( qApp->translate("ConfigDlgPrivate", "Portuguese") );

  //boxLayout->addWidget( languageLabel );
  //boxLayout->addWidget( languageList );
  //boxLayout->addItem( new QSpacerItem( 10, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum ) );

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addWidget( saveSessionCheck     );
  layout->addWidget( autoCompleteCheck    );
  layout->addWidget( autoCalcCheck        );
  layout->addWidget( minimizeToTrayCheck  );
  layout->addWidget( stayAlwaysOnTopCheck );
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


ConfigDlg::ConfigDlg( QWidget * parent ) : QDialog( parent )
{
  d = new ConfigDlgPrivate;
  setWindowTitle( tr("Configure SpeedCrunch" ) );

  d->centerWidget = new QTabWidget( this );
  d->generalPg = d->generalPage();
  d->centerWidget->addTab( d->generalPg, tr("&General") );

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
  setUpdatesEnabled( false );
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
