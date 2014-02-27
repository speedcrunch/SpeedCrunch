// This file is part of the SpeedCrunch project
// Copyright (C) 2014 Helder Correia <helder.pereira.correia@gmail.com>
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

#include "core/manual.h"

#define STYLE \
    "<style>" \
        "body {" \
            "background-color: #ffffff;" \
            "color: #000000;" \
        "}" \
        \
        "h2 {" \
            "font-family: \"Times New Roman\", Verdana, Arial, Helvetica, sans-serif;" \
        "}" \
        \
        ".caption {" \
            "font-style: italic;" \
            "font-size: 10pt;" \
        \
        "}" \
        \
        "a:link {" \
            "text-decoration: none;" \
        "}" \
        \
        ".page-link a:link {" \
            "color: #555;" \
            "font-family: Verdana, Arial, Helvetica, sans-serif;" \
        "}" \
    "</style>"

#define BEGIN QString("<html><head>"STYLE"</head><body>")+
#define SECTION(s) "<h3>"+(s)+"</h3>"
#define BR "<br />"
#define INDEX_LINK "<span class=\"page-link\"><a href=\"index\">"+Manual::tr("Index")+"</a></span>"
#define LINK(link,text) "<span class=\"page-link\"><a href=\""#link"\">"+text+"</a></span>"
#define TITLE(s) "<h1>"+(s)+"</h1><div>"
#define TABLE "<table><tr>"
#define ROW "</tr><tr>"
#define _TABLE "</div></tr></table>"
#define END "</body></html>"

static QString makeIndexPage()
{
    return
        BEGIN
        TITLE(Manual::tr("Index"))
        LINK(shortcuts, Manual::tr("Keyboard shortcuts")) BR
        LINK(functions, Manual::tr("Built-in functions")) BR
        LINK(constants, Manual::tr("Named constants")) BR
        LINK(variables, Manual::tr("User-defined variables")) BR
        END;
}

static QString makeShortcutsPage()
{
    return
        BEGIN
        TITLE(Manual::tr("Keyboard shortcuts"))
        END;
}

static QString makeFunctionsPage()
{
    return
        BEGIN
        TITLE(Manual::tr("Built-in functions"))
        END;
}

static QString makeConstantsPage()
{
    return
        BEGIN
        TITLE(Manual::tr("Named constants"))
        END;
}

static QString makeVariablesPage()
{
    return
        BEGIN
        TITLE(Manual::tr("User-defined variables"))
        END;
}

void Manual::createPages()
{
    addPage("index", &makeIndexPage);
    addPage("shortcuts", &makeShortcutsPage);
    addPage("functions", &makeFunctionsPage);
    addPage("constants", &makeConstantsPage);
    addPage("variables", &makeVariablesPage);
}