// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
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


#include "3rdparty/util/binreloc.h"
#include "gui/mainwindow.hxx"

#include <QApplication>
#include <QLocale>
#include <QTranslator>


QTranslator * createTranslator()
{
  QTranslator * translator = new QTranslator();
  bool          foundTranslator = false;
  QString       localeFile = QLocale().name();
  QString       localeDir;

#ifdef Q_OS_WIN32
  if ( ! foundTranslator )
    if ( translator->load( localeFile ) )
      foundTranslator = true;
#endif // Q_OS_WIN32

  BrInitError error;

  if ( br_init( &error ) == 0 && error != BR_INIT_ERROR_DISABLED )
  {
      printf( "Warning: BinReloc failed to initialize (error code %d)\n", error );
      printf( "Will fallback to hardcoded default path.\n" );
  }

  localeDir = QString( br_find_data_dir( 0 ) ) + "/speedcrunch/locale";
  if ( ! foundTranslator )
  {
    if ( translator->load( localeFile, localeDir ) )
      foundTranslator = true;
  }

  if ( foundTranslator )
    return translator;
  else
    return 0;
}


int main( int argc, char * * argv )
{
  QApplication app( argc, argv );

  QTranslator * tr = createTranslator();
  if ( tr )
    app.installTranslator( tr );

  MainWindow mw;
  mw.show();

  app.connect( & app, SIGNAL( lastWindowClosed() ),
               & app, SLOT( quit() ) );

  app.connect( & mw, SIGNAL( quitApplication() ),
               & app, SLOT( quit() ) );

  return app.exec();
}
