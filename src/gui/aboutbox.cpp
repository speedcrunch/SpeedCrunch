// This file is part of the SpeedCrunch project
// Copyright (C) 2004-2006 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2005-2006 Johan Thelin <e8johan@gmail.com>
// Copyright (C) 2007-2009 Helder Correia <helder.pereira.correia@gmail.com>
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

#include "gui/aboutbox.hxx"

#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>

#define MAKE_STRING(a) DO_MAKE_STRING(a)
#define DO_MAKE_STRING(a) # a

AboutBox::AboutBox( QWidget * parent )
    : QDialog( parent )
{
    setObjectName( "AboutBox" );
    setWindowTitle( tr("About SpeedCrunch") );

    QLabel * infoLabel = new QLabel( this );
    QString website = "http://www.speedcrunch.org";
    QString info = "<b>";
    info += QString( "SpeedCrunch %1" ).arg( MAKE_STRING(SPEEDCRUNCH_VERSION) );
#ifdef SPEEDCRUNCH_PORTABLE
    info += " (Portable Edition)";
#endif
    info += "</b><br>";
    info += QString( "<a href=\"%1\">%2</a>" ).arg( website ).arg( website );
    infoLabel->setText( info );
    infoLabel->setOpenExternalLinks( true );

    QLabel * iconLabel = new QLabel( this );
    iconLabel->setPixmap( QPixmap( ":/speedcrunch.png" ) );
    iconLabel->setAlignment( Qt::AlignVCenter | Qt::AlignRight );

    QString msg = "<center>";

    const QString authors = "<p><b>%1</b><br>%2";
    msg += authors.arg( tr("Maintainer and core developer") ).arg( "Helder Correia" );
    msg += authors.arg( tr("Original author") ).arg( "Ariya Hidayat" );
    msg += authors.arg( tr("Former maintainer") ).arg( "Johan Thelin" );
    msg += authors.arg( tr("Math engine and expression parser") ).arg( "Wolf Lammen" );
    msg += authors.arg( tr("General purpose developer") ).arg( "Andreas Scherer" );
    msg += authors.arg( tr("Packager and OS X stylist") ).arg( "Alessandro Portale" );
    msg += authors.arg( tr("Original math book author") ).arg( "Petri Damst&eacute;n" );
    msg += authors.arg( tr("Based on original idea from") ).arg( "Roberto Alsina" );
    msg += authors.arg( tr("Special thanks to") ).arg( "Marco Wegner<br>"
                                                       "Michael Pyne<br>"
                                                       "Jonathan Avraham<br>"
                                                       "Jonathan Riddell<br>"
                                                       "Witold Wysota" );
    msg += authors.arg( tr("Graphics") ).arg( "Kuswanto (Zeus)<br>Oxygen Project" );

    msg += "<p>";
    msg += QString( "<b>%1</b><br>" ).arg( tr("Thanks to") );
    msg += "Alejandro Villarreal<br>"
           "Alexey Kouznetsov<br>"
           "Anders Lund<br>"
           "Aur&eacute;lien G&acirc;teau<br>"
           "Bart Martens<br>"
           "Bernhard Schiffner<br>"
           "Christian Ehrlich<br>"
           "Damir Perisa<br>"
           "Derek O'Connor<br>"
           "Eugeniu Plamadeala<br>"
           "F Chris Carrera<br>"
           "Francesco di Cugno<br>"
           "Gary Cramblitt<br>"
           "Henrique Pinto<br>"
           "Jean-Remy Falleri<br>"
           "Lars Ivar Igesund<br>"
           "Maciek Borowka<br>"
           "Matthew J Smith<br>"
           "Melchior Franz<br>"
           "Mohamed Eldesoky<br>"
           "Philippe Fremy<br>"
           "Pieter Pareit<br>"
           "Roland \"liquidat\" Wolters<br>"
           "Sarah Hobbs<br>"
           "Stephan Binner<br>"
           "Thomas Luebking<br>"
           "Thomas Nagy<br>"
           "Vibet Alexis<br>"
           "Vladimir Pouzanov<br>";
    msg += "</p>";

    msg += "<p>";
    msg += QString( "<b>%1</b><br><br>" ).arg( tr("Translations") );
    const QString i18n = "<b>%1</b><br>%2<br><br>";
    msg += i18n.arg( tr("Arabic") )
               .arg( "Ali Issa<br>Nancy Mattur<br>Kefah Issa" );
    msg += i18n.arg( tr("Argentinian Spanish") )
               .arg( "Lucas Sallovitz" );
    msg += i18n.arg( tr("Basque") )
               .arg( "I&ntilde;igo Salvador Azurmendi" );
    msg += i18n.arg( tr("Brazilian Portuguese") )
               .arg( "F&aacute;bio Margarido" );
    msg += i18n.arg( tr("Catalan") )
               .arg( "Francesc Farf&aacute;n" );
    msg += i18n.arg( tr("Czech") )
               .arg( "Blond'&aacute;k" );
    msg += i18n.arg( tr("Dutch") )
               .arg( "Bram Schoenmakers<br>Jaap Woldringh<br>Maarten Fonville" );
    msg += i18n.arg( tr("French") )
               .arg( "Cyril Foucr&eacute;<br>Gr&eacute;gory Treiber<br>Laurent Cozic" );
    msg += i18n.arg( tr("Finnish") )
               .arg( "J-P Nurmi" );
    msg += i18n.arg( tr("German") )
               .arg( "Wolf Lammen" );
    msg += i18n.arg( tr("Hebrew") )
               .arg( "Jonathan Avraham" );
    msg += i18n.arg( tr("Hungarian") )
               .arg( "T&ouml;r&ouml;k B&aacute;lint" );
    msg += i18n.arg( tr("Indonesian") )
               .arg( "Ariya Hidayat<br>Yolla Indria" );
    msg += i18n.arg( tr("Italian") )
               .arg( "Alessio Sclocco" );
    msg += i18n.arg( tr("Japanese") )
               .arg( "Kazuki Hamasaki" );
    msg += i18n.arg( tr("Norwegian") )
               .arg( "Thomas Finneid" );
    msg += i18n.arg( tr("Polish") )
               .arg( "Micha&#322; Trzebiatowski<br>Witold Wysota" );
    msg += i18n.arg( tr("Portuguese") )
               .arg( "Helder Correia" );
    msg += i18n.arg( tr("Romanian") )
               .arg( "Iulia Taulescu" );
    msg += i18n.arg( tr("Russian") )
               .arg( "Alexander Bushuev (aka winuntu)<br>Irek Khasyanov" );
    msg += i18n.arg( tr("Simplified Chinese") )
               .arg( "Liang Qi" );
    msg += i18n.arg( tr("Spanish") )
               .arg( "Francesc Farf&aacute;n" );
    msg += i18n.arg( tr("Swedish") )
               .arg( "Johan Thelin" );
    msg += i18n.arg( tr("Turkish") )
               .arg( "Serdar Soytetir" );
    msg += "</p>";

    msg += "<p>";
    const QString copy = "<b>%1 %2 %3</b><br><i>%4</i><br><br>";
    msg += copy.arg( tr("Copyright (C)") ).arg( "2004-2007" )
               .arg( "Ariya Hidayat" ).arg( "ariya@kde.org" );
    msg += copy.arg( tr("Copyright (C)") ).arg( "2005-2006" )
               .arg( "Johan Thelin" ).arg( "e8johan@gmail.com" );
    msg += copy.arg( tr("Copyright (C)") ).arg( "2007-2009" )
               .arg( "Helder Correia" ).arg( "helder.pereira.correia@gmail.com" );
    msg += copy.arg( tr("Copyright (C)") ).arg( "2007-2009" )
               .arg( "Wolf Lammen" ).arg( "ookami1@gmx.de" );
    msg += copy.arg( tr("Copyright (C)") ).arg( "2007" )
               .arg( "Petri Damst&eacute;n" ).arg( "damu@iki.fi" );
    msg += copy.arg( tr("Copyright (C)") ).arg( "2008-2009" )
               .arg( "Alessandro Portale" ).arg( "alessandro.portale@googlemail.com" );
    msg += copy.arg( tr("Copyright (C)") ).arg( "2009" )
               .arg( "Andreas Scherer" ).arg( "andreas_coder@freenet.de" );
    msg += "</p>";

    msg += "<p>";
    msg += tr( "This program is free software; you can redistribute it and/or "
               "modify it under the terms of the GNU General Public License "
               "as published by the Free Software Foundation; either version 2 "
               "of the License, or (at your option) any later version" );
    msg += "</p>";
    msg += "<p>";
    msg += tr( "This program is distributed in the hope that it will be useful, "
               "but WITHOUT ANY WARRANTY; without even the implied warranty of "
               "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
               "GNU General Public License for more details." );
    msg += "</p>";
    msg +=  "<p>&nbsp;</p>";
    msg += QString( "<p>%1</p>" )
               .arg( tr("Visit <b>http://www.speedcrunch.org</b> for more information!") );

    msg += "</center>";

    QTextEdit * textEdit = new QTextEdit( this );
    textEdit->setReadOnly( true );
    textEdit->setText( msg );

    QPushButton * closeButton = new QPushButton( this );
    closeButton->setObjectName( "CloseButton" );
    closeButton->setText( tr("Close") );
    QObject::connect( closeButton, SIGNAL(clicked()), SLOT(accept()) );

    QSpacerItem * buttonSpacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding,
                                                  QSizePolicy::Minimum );

    QVBoxLayout * mainLayout = new QVBoxLayout( this );
    QHBoxLayout * topLayout = new QHBoxLayout();
    QHBoxLayout * buttonLayout = new QHBoxLayout();

    mainLayout->addLayout( topLayout );
    mainLayout->addWidget( textEdit );
    mainLayout->addLayout( buttonLayout );
    mainLayout->setStretchFactor( textEdit, 1 );

    topLayout->addWidget( infoLabel );
    topLayout->addWidget( iconLabel );

    buttonLayout->addItem( buttonSpacer );
    buttonLayout->addWidget( closeButton );

    setWindowTitle( tr("About SpeedCrunch") );
}

