// This file is part of the SpeedCrunch project
// Copyright (C) 2007 Petri Damstén <damu@iki.fi>
// Copyright (C) 2008 Helder Correia <helder.pereira.correia@gmail.com>
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


#include <gui/bookdock.hxx>

#include <QComboBox>
#include <QFile>
#include <QLibraryInfo>
#include <QLocale>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>


struct BookDock::Private
{
  QTextBrowser * sheet;
  QString        path;
  QString        lang;
  QString        file;
};


BookDock::BookDock( const QString & directory, const QString & file,
                    const QString & title, const QString & language,
                    QWidget * parent )
  : QDockWidget( title, parent ),
    d( new BookDock::Private )
{
  d->path = directory;
  d->file = file;
  QString locale = (language == "C") ? QLocale().name()
                                     : language;
  QString localeShort = locale.left( 2 ).toLower();
  d->lang = locale;

  QWidget* widget = new QWidget( this );
  QVBoxLayout* bookLayout = new QVBoxLayout;

  d->sheet = new QTextBrowser( this );
  d->sheet->setLineWrapMode( QTextEdit::NoWrap );
  //d->sheet->setOpenLinks(false); // remove to stick with Qt 4.2
  d->sheet->setSearchPaths( QStringList() << d->path );

  connect( d->sheet, SIGNAL( anchorClicked( const QUrl & ) ),
           this, SLOT( anchorClicked( const QUrl & ) ) );

  QHBoxLayout * buttonLayout = new QHBoxLayout;
  buttonLayout->setSpacing( 0 );
  buttonLayout->setMargin( 0 );
  QPushButton * button = new QPushButton( tr("Back"), this );
  button->setIcon( QPixmap( ":/book_back.png" ) );
  button->setFlat( true );

  connect( button, SIGNAL( clicked() ), d->sheet, SLOT( backward() ) );
  connect( d->sheet, SIGNAL( backwardAvailable( bool ) ),
           button, SLOT( setEnabled( bool ) ) );

  buttonLayout->addWidget( button );

  button = new QPushButton( tr("Forward"), this );
  button->setIcon( QPixmap( ":/book_forward.png" ) );
  button->setFlat( true );

  connect( button, SIGNAL( clicked() ), d->sheet, SLOT( forward() ) );
  connect( d->sheet, SIGNAL( forwardAvailable( bool ) ),
           button, SLOT( setEnabled( bool ) ) );

  buttonLayout->addWidget( button );

  button = new QPushButton( tr("Index"), this );
  button->setIcon( QPixmap( ":/book_home.png" ) );
  button->setFlat( true );
  connect( button, SIGNAL( clicked() ), d->sheet, SLOT( home() ) );
  buttonLayout->addWidget( button );

  buttonLayout->addStretch();
  bookLayout->addLayout( buttonLayout );
  bookLayout->addWidget( d->sheet );

  widget->setLayout( bookLayout );
  setWidget( widget );

  setMinimumWidth( 150 );
  setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  setWindowIcon( QIcon() ); // no icon

  d->sheet->setSource( d->path + d->lang + "/" + d->file );
}


BookDock::~BookDock()
{
  delete d;
}


void BookDock::anchorClicked ( const QUrl & link )
{
  if ( link.toString().startsWith( "file:#" ) )
  {
    // avoid appended history garbage after clicking on a formula (unknown)
    d->sheet->backward();
    d->sheet->forward();

    emit expressionSelected( link.toString().mid( 6 ) );
  }
  else
  {
    d->sheet->setSource( link );
  }

  // necessary for QTextBrowser to always draw correctly (why?)
  d->sheet->adjustSize();
}
