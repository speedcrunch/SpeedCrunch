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
#include <QEvent>
#include <QFile>
#include <QLibraryInfo>
#include <QLocale>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>


struct BookDock::Private
{
  BookDock *     p;
  QTextBrowser * sheet;
  QString        path;
  QString        index;
  QString        file;
  QString        language;
  QPushButton *  backButton;
  QPushButton *  forwardButton;
  QPushButton *  indexButton;
  QWidget *      buttonLayoutWidget;
  QHBoxLayout *  buttonLayout;

  void handleLayoutDirection();
};


void BookDock::Private::handleLayoutDirection()
{
  if ( p->layoutDirection() == Qt::RightToLeft )
  {
    buttonLayout->removeWidget( forwardButton );
    buttonLayout->insertWidget( 0, forwardButton );
  }
  else
  {
    buttonLayout->removeWidget( backButton );
    buttonLayout->insertWidget( 0, backButton );
  }
}


BookDock::BookDock( const QString & directory, const QString & file,
                    const QString & language, QWidget * parent )
  : QDockWidget( parent ), d( new BookDock::Private )
{
  d->p = this;
  d->path  = directory;
  d->file  = file;
  d->index = file;

  QWidget* widget = new QWidget( this );
  QVBoxLayout* bookLayout = new QVBoxLayout;

  d->sheet = new QTextBrowser( this );
  d->sheet->setLineWrapMode( QTextEdit::NoWrap );
  //d->sheet->setOpenLinks( false ); // remove to stick with Qt 4.2
  d->sheet->setSearchPaths( QStringList() << d->path );

  connect( d->sheet, SIGNAL( anchorClicked( const QUrl & ) ),
           this, SLOT( anchorClicked( const QUrl & ) ) );

  d->buttonLayoutWidget = new QWidget;
  d->buttonLayout = new QHBoxLayout( d->buttonLayoutWidget );
  d->buttonLayout->setSpacing( 0 );
  d->buttonLayout->setMargin( 0 );

  d->backButton = new QPushButton( "", this );
  d->backButton->setIcon( QPixmap( ":/book_back.png" ) );
  d->backButton->setFlat( true );

  connect( d->backButton, SIGNAL( clicked() ), d->sheet, SLOT( backward() ) );
  connect( d->sheet, SIGNAL( backwardAvailable( bool ) ),
           d->backButton, SLOT( setEnabled( bool ) ) );

  d->buttonLayout->addWidget( d->backButton );

  d->forwardButton = new QPushButton( "", this );
  d->forwardButton->setIcon( QPixmap( ":/book_forward.png" ) );
  d->forwardButton->setFlat( true );

  connect( d->forwardButton, SIGNAL( clicked() ),
           d->sheet, SLOT( forward() ) );
  connect( d->sheet, SIGNAL( forwardAvailable( bool ) ),
           d->forwardButton, SLOT( setEnabled( bool ) ) );

  d->buttonLayout->addWidget( d->forwardButton );

  d->indexButton = new QPushButton( "", this );
  d->indexButton->setIcon( QPixmap( ":/book_home.png" ) );
  d->indexButton->setFlat( true );

  connect( d->indexButton, SIGNAL( clicked() ), this, SLOT( home() ) );

  d->buttonLayout->addWidget( d->indexButton );
  d->buttonLayout->addStretch();

  bookLayout->addWidget( d->buttonLayoutWidget );
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

    QString expr = link.toString().mid( 6 );

#if QT_VERSION < 0x040400
    // Qt 4.3 and earlier do not properly decode the URL
    // and thus %5E does not become ^
    // We do it manually here
    // FIXME: should we check this at run-time?
    expr.replace( QLatin1String("%5E"), QLatin1String("^"), Qt::CaseInsensitive );
#endif

    emit expressionSelected( expr );
  }
  else
  {
    d->sheet->setSource( link );
    d->file = QFileInfo( link.path() ).fileName();
  }

  // necessary for QTextBrowser to always draw correctly (why?)
  // causes a bug on Window or maybe just about the version number
  //d->sheet->adjustSize();
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

  // fallback to English
  if ( locale == "C" )
    locale = "en";

  d->language = locale;
  QString path = d->path + d->language + "/" + d->file;
  QString src = path;

  QDir dir( path );
  if ( ! dir.isReadable() )
  {
    QString localeShort = locale.left( 2 ).toLower();
    src = d->path + localeShort + "/" + d->file;
  }

  d->sheet->setSource( QUrl::fromLocalFile( src ) );
  d->handleLayoutDirection();
}


void BookDock::setTitle( const QString & title )
{
  setWindowTitle( title );
}


// protected

void BookDock::changeEvent( QEvent * e )
{
  if ( e->type() == QEvent::LayoutDirectionChange )
    d->handleLayoutDirection();
  else
    QDockWidget::changeEvent( e );
}
