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
#include "crunch.h"
#include "config.h"

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

#ifdef INSTALL_PREFIX 
 	QDir qmpath( INSTALL_PREFIX );
 	qmpath.cd( "share/crunch" ); 

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
#endif // INSTALL_PREFIX

	if( !foundTranslator )
	{
    qmfile = QString("./crunch_") + locale + ".qm";
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
    qmfile = QString("./crunch_") + localeShort + ".qm";
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
