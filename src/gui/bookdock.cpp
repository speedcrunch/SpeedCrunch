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
#include <QDir>
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
  QString        index;
  QString        file;
  QString        language;
  QPushButton *  backButton;
  QPushButton *  forwardButton;
  QPushButton *  indexButton;
};


BookDock::BookDock( const QString & directory, const QString & file,
                    const QString & language, QWidget * parent )
  : QDockWidget( parent ), d( new BookDock::Private )
{
  d->path  = directory;
  d->file  = file;
  d->index = file;

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
  d->backButton = new QPushButton( "", this );
  d->backButton->setIcon( QPixmap( ":/book_back.png" ) );
  d->backButton->setFlat( true );

  connect( d->backButton, SIGNAL( clicked() ), d->sheet, SLOT( backward() ) );
  connect( d->sheet, SIGNAL( backwardAvailable( bool ) ),
           d->backButton, SLOT( setEnabled( bool ) ) );

  buttonLayout->addWidget( d->backButton );

  d->forwardButton = new QPushButton( "", this );
  d->forwardButton->setIcon( QPixmap( ":/book_forward.png" ) );
  d->forwardButton->setFlat( true );

  connect( d->forwardButton, SIGNAL( clicked() ),
           d->sheet, SLOT( forward() ) );
  connect( d->sheet, SIGNAL( forwardAvailable( bool ) ),
           d->forwardButton, SLOT( setEnabled( bool ) ) );

  buttonLayout->addWidget( d->forwardButton );

  d->indexButton = new QPushButton( "", this );
  d->indexButton->setIcon( QPixmap( ":/book_home.png" ) );
  d->indexButton->setFlat( true );

  connect( d->indexButton, SIGNAL( clicked() ), this, SLOT( home() ) );

  buttonLayout->addWidget( d->indexButton );

  buttonLayout->addStretch();
  bookLayout->addLayout( buttonLayout );
  bookLayout->addWidget( d->sheet );

  widget->setLayout( bookLayout );
  setWidget( widget );

  setMinimumWidth( 150 );
  setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  setWindowIcon( QIcon() ); // no icon

  setLanguage( language );
}


BookDock::~BookDock()
{
  delete d;
}


// public slots

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
    d->file = QFileInfo( link.path() ).fileName();
  }

  // necessary for QTextBrowser to always draw correctly (why?)
  d->sheet->adjustSize();
}


void BookDock::home()
{
  d->sheet->setSource( d->index );
  d->file = d->index;
  setLanguage( d->language );
}


void BookDock::setLanguage( const QString & languageCode )
{
  d->language = languageCode;

  // buttons
  d->backButton   ->setText( tr( "Back"    ) );
  d->forwardButton->setText( tr( "Forward" ) );
  d->indexButton  ->setText( tr( "Index"   ) );

  // page
  QString locale = (languageCode == "C") ? QLocale().name()
                                     : languageCode;
  d->language = locale;
  QString path = d->path + d->language + "/" + d->file;
  QDir dir( path );
  if ( dir.isReadable() )
  {
    d->sheet->setSource( path );
  }
  else
  {
    QString localeShort = locale.left( 2 ).toLower();
    d->sheet->setSource( d->path + localeShort + "/" + d->file );
  }
}


void BookDock::setTitle( const QString & title )
{
  setWindowTitle( title );
}
