//  This file is part of the SpeedCrunch project
//  Copyright (C) 2004, 2006 Ariya Hidayat <ariya@kde.org>
//  Copyright (C) 2005, 2006 Johan Thelin <e8johan@gmail.com>
//  Copyright (C) 2007, 2009, 2013 Helder Correia <helder.pereira.correia@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; see the file COPYING.  If not, write to
//  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
//  Boston, MA 02110-1301, USA.

#include "gui/aboutbox.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QTextEdit>
#include <QVBoxLayout>

#define THANKS(name) #name"<br>"

AboutBox::AboutBox(QWidget* parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    setObjectName("AboutBox");
    setWindowTitle(tr("About SpeedCrunch"));

    QString msg = "<center>";
    msg += "<img src=\":/speedcrunch.png\"><br>";
    msg += "<b>SpeedCrunch " + QLatin1String(SPEEDCRUNCH_VERSION);
#ifdef SPEEDCRUNCH_PORTABLE
    msg += " (Portable Edition)";
#endif
    msg += "</b><br>(Qt " + QLatin1String(QT_VERSION_STR) + ")<br>";

    const QString authors = "<p><b>%2</b><br>%1";
    msg += authors.arg(tr("Maintainer, core developer, designer, translator")).arg("Helder Correia");
    msg += authors.arg(tr("Original author")).arg("Ariya Hidayat");
    msg += authors.arg(tr("Math engine")).arg("Wolf Lammen");
    msg += authors.arg(tr("Packager")).arg("Alessandro Portale");
    msg += authors.arg(tr("Former maintainer")).arg("Johan Thelin");

    msg += "<p>" + QString("<b>%1</b><br>").arg(tr("Thanks"));
    msg +=
        THANKS(Alan Davies)
        THANKS(Alejandro Villarreal)
        THANKS(Alexey Kouznetsov)
        THANKS(Anders Lund)
        THANKS(Andreas Scherer)
        THANKS(Artem Golovinsky)
        THANKS(Aur&eacute;lien G&acirc;teau)
        THANKS(Bart Martens)
        THANKS(Bernhard Schiffner)
        THANKS(Christian Ehrlich)
        THANKS(Damir Perisa)
        THANKS(Daniel Sch&auml;ufele)
        THANKS(Derek O’Connor)
        THANKS(Enrico R&oacute;s)
        THANKS(Eugeniu Plamadeala)
        THANKS(Felix Krull)
        THANKS(F Chris Carrera)
        THANKS(Francesco di Cugno)
        THANKS(Gary Cramblitt)
        THANKS(Henrik Nordstrom)
        THANKS(Henrique Pinto)
        THANKS(James Yuzwalk)
        THANKS(Jean-Remy Falleri)
        THANKS(Jonathan Avraham)
        THANKS(Jonathan Riddell)
        THANKS(Lars Ivar Igesund)
        THANKS(Maciek Borowka)
        THANKS(Marco Wegner)
        THANKS(Matthew J Smith)
        THANKS(Melchior Franz)
        THANKS(Michael Pyne)
        THANKS(Mohamed Eldesoky)
        THANKS(Oliver Gubler)
        THANKS(Petri Damst&eacute;n)
        THANKS(Philippe Fremy)
        THANKS(Pieter Pareit)
        THANKS(Roberto Alsina)
        THANKS(Roland ’liquidat’ Wolters)
        THANKS(Sarah Hobbs)
        THANKS(S&eacute;bastien Szymanski)
        THANKS(Stephan Binner)
        THANKS(Steven Honeyman)
        THANKS(Tey)
        THANKS(Thomas Luebking)
        THANKS(Thomas Nagy)
        THANKS(Vibet Alexis)
        THANKS(Vladimir Pouzanov)
        THANKS(Wictor Lund)
        THANKS(Witold Wysota)
        THANKS(1100101)
        ;

    msg += "</p><p>";
    msg += QString("<b>%1</b><br><br>").arg(tr("Translations"));
    const QString i18n = "<b>%1</b><br>%2<br><br>";
    msg += i18n.arg(tr("Arabic"))
               .arg("Ali Issa<br>Nancy Mattur<br>Kefah Issa");
    msg += i18n.arg(tr("Argentinian Spanish"))
               .arg("Lucas Sallovitz");
    msg += i18n.arg(tr("Basque"))
               .arg("I&ntilde;igo Salvador Azurmendi");
    msg += i18n.arg(tr("Brazilian Portuguese"))
               .arg("F&aacute;bio Margarido");
    msg += i18n.arg(tr("Catalan"))
               .arg("Francesc Farf&aacute;n");
    msg += i18n.arg(tr("Czech"))
               .arg("Blond'&aacute;k<br>Jakub Vasicek<br>Pavel Fric");
    msg += i18n.arg(tr("Dutch"))
               .arg("Bram Schoenmakers<br>Jaap Woldringh<br>Maarten Fonville<br>Kristof Bal");
    msg += i18n.arg(tr("Estonian"))
               .arg("Mihkel T&otilde;nnov");
    msg += i18n.arg(tr("French"))
               .arg("Cyril Foucr&eacute;<br>Gr&eacute;gory Treiber<br>Laurent Cozic");
    msg += i18n.arg(tr("Finnish"))
               .arg("J-P Nurmi<br>Lasse Liehu");
    msg += i18n.arg(tr("German"))
               .arg("Wolf Lammen");
    msg += i18n.arg(tr("Hebrew"))
               .arg("Jonathan Avraham");
    msg += i18n.arg(tr("Hungarian"))
               .arg("T&ouml;r&ouml;k B&aacute;lint");
    msg += i18n.arg(tr("Indonesian"))
               .arg("Ariya Hidayat<br>Yolla Indria");
    msg += i18n.arg(tr("Italian"))
               .arg("Alessio Sclocco");
    msg += i18n.arg(tr("Japanese"))
               .arg("Kazuki Hamasaki");
    msg += i18n.arg(tr("Korean"))
               .arg("Younguk Kim");
    msg += i18n.arg(tr("Latvian"))
               .arg("Ri&#x10D;ards Pauls");
    msg += i18n.arg(tr("Norwegian"))
               .arg("Thomas Finneid");
    msg += i18n.arg(tr("Polish"))
               .arg("Micha&#322; Trzebiatowski<br>Witold Wysota");
    msg += i18n.arg(tr("Portuguese"))
               .arg("Helder Correia");
    msg += i18n.arg(tr("Romanian"))
               .arg("Iulia Taulescu");
    msg += i18n.arg(tr("Russian"))
               .arg("Alexander Bushuev (aka winuntu)<br>Irek Khasyanov");
    msg += i18n.arg(tr("Simplified Chinese"))
               .arg("Liang Qi");
    msg += i18n.arg(tr("Spanish"))
               .arg("Francesc Farf&aacute;n");
    msg += i18n.arg(tr("Swedish"))
               .arg("Johan Thelin");
    msg += i18n.arg(tr("Turkish"))
               .arg("Serdar Soytetir");
    msg += "</p>";

    msg += "<p>";
    const QString copy = "%1 %2 <b>%3</b><br>";
    msg += copy.arg(tr("Copyright (C)")).arg("2004-2007").arg("Ariya Hidayat");
    msg += copy.arg(tr("Copyright (C)")).arg("2005-2006").arg("Johan Thelin");
    msg += copy.arg(tr("Copyright (C)")).arg("2007-2013").arg("Helder Correia");
    msg += copy.arg(tr("Copyright (C)")).arg("2007-2009").arg("Wolf Lammen");
    msg += copy.arg(tr("Copyright (C)")).arg("2007").arg("Petri Damst&eacute;n");
    msg += copy.arg(tr("Copyright (C)")).arg("2008-2009").arg("Alessandro Portale");
    msg += copy.arg(tr("Copyright (C)")).arg("2009").arg("Andreas Scherer");
    msg += "</p>";

    msg += "<p>";
    msg += tr("This program is free software; you can redistribute it and/or "
              "modify it under the terms of the GNU General Public License "
              "as published by the Free Software Foundation; either version 2 "
              "of the License, or (at your option) any later version");
    msg += "</p>";
    msg += "<p>";
    msg += tr("This program is distributed in the hope that it will be useful, "
              "but WITHOUT ANY WARRANTY; without even the implied warranty of "
              "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
              "GNU General Public License for more details.");
    msg += "</p>";

    msg += "</center>";

    QTextEdit* textEdit = new QTextEdit(this);
    textEdit->setReadOnly(true);
    textEdit->setText(msg);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(textEdit);

    setWindowTitle(tr("About SpeedCrunch"));
}
