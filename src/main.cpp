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
 
#include <qapplication.h>
#include <qtextcodec.h>
#include <qtranslator.h>
#include <qfileinfo.h>
#include <qdir.h>

#include "binreloc.h"
#include "crunch.h"

QTranslator *createTranslator()
{
  QString locale = QString::fromLatin1( QTextCodec::locale() );
  QString localeShort = locale.left( 2 ).lower();
	
	bool foundTranslator = false;
	
	QString qmfile;
	QFileInfo fi;
	QTranslator* translator;
	
#ifdef Q_OS_WIN32
	if( !foundTranslator )
  {
  	qmfile = QString("crunch_") + locale + ".qm";
  	fi = QFileInfo( qmfile );
  	if( fi.exists() )
  	{
    	translator = new QTranslator( 0 );
    	translator->load( qmfile );

			foundTranslator = true;
  	}
	}

	if( !foundTranslator )
  {
    qmfile = QString("crunch_") + localeShort + ".qm";
    fi = QFileInfo( qmfile );
    if( fi.exists() )
    {
      translator = new QTranslator( 0 );
      translator->load( qmfile );

			foundTranslator = true;
    }
  }
#endif // Q_OS_WIN32

    BrInitError error;

    if (br_init (&error) == 0 && error != BR_INIT_ERROR_DISABLED) {
        printf ("Warning: BinReloc failed to initialize (error code %d)\n", error);
        printf ("Will fallback to hardcoded default path.\n");
    } 	
    
    // Search with the following order:
    // (1) install prefix + share/crunch, e.g. "/usr/local/share/crunch"
    // (2) install prefix + share, e.g. "/usr/local/share"
    // (3) current directory
    
    
    // item (1)
    QString shareDir = QString(br_find_data_dir(0)).append("/crunch");
 	QDir qmpath( shareDir );
 	qmpath.cd("crunch");

	if( !foundTranslator )
  {
  	qmfile = qmpath.absPath() + "/crunch_" + locale + ".qm";
  	fi = QFileInfo( qmfile );
  	if( fi.exists() )
  	{
    	translator = new QTranslator( 0 );
    	translator->load( qmfile );

			foundTranslator = true;
  	}
	}
	
	if( !foundTranslator )
  {
    qmfile = qmpath.absPath() + "/crunch_" + localeShort + ".qm";
    fi = QFileInfo( qmfile );
    if( fi.exists() )
    {
      translator = new QTranslator( 0 );
      translator->load( qmfile );
      
			foundTranslator = true;
    }
	}

	// item (2), fallback is item (3)
 	qmpath = QDir( br_find_data_dir(".") );

	if( !foundTranslator )
  {
  	qmfile = qmpath.absPath() + "/crunch_" + locale + ".qm";
  	fi = QFileInfo( qmfile );
  	if( fi.exists() )
  	{
    	translator = new QTranslator( 0 );
    	translator->load( qmfile );

			foundTranslator = true;
  	}
	}
	
	if( !foundTranslator )
  {
    qmfile = qmpath.absPath() + "/crunch_" + localeShort + ".qm";
    fi = QFileInfo( qmfile );
    if( fi.exists() )
    {
      translator = new QTranslator( 0 );
      translator->load( qmfile );
      
			foundTranslator = true;
    }
	}

	if( foundTranslator )
		return translator;
	else
	  return new QTranslator( 0 );
}

int main( int argc, char ** argv )
{
  QApplication a( argc, argv );
  a.installTranslator( createTranslator() );
  
  Crunch* v = new Crunch();
  v->show();
  
  a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
  return a.exec();
}
