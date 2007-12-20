/* bisonparser.h: lexical analyzers and parsers */
/*
    Copyright (C) 2007 Wolf Lammen.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License , or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING.  If not, write to:

      The Free Software Foundation, Inc.
      59 Temple Place, Suite 330
      Boston, MA 02111-1307 USA.


    You may contact the author by:
       e-mail:  ookami1 <at> gmx <dot> de
       mail:  Wolf Lammen
              Oertzweg 45
              22307 Hamburg
              Germany

*************************************************************************/

#ifndef _BISONPARSER_H
#define _BISONPARSER_H

#include "bison.h"

class SglExprLex
{
  public:
    bool autoFix();
  private:
    const int syschar = 1;
    const int sot = 2;
    const int eot = 3;
    const int ans = 4;
    const int whitespace = ' ';

    QString expr;
    QString systag;

    enum { inWhitespace
    } state;
    int pos;
    int start;
    int end;

    void setExpression(const QString& newexpr);
    int size() { return expr.size(); };
    char at(int i) { return expr.at(i); };
    int getNextToken();
    QString revLookup(int token);
    void reset();
    int getTopLevelToken();
    int getSysToken();
    int getIdentifier();
    int getNumeric();
    int getSpecialCharToken();
    void skip(QChar c);
    int lookup(const QString& identifier);
};
