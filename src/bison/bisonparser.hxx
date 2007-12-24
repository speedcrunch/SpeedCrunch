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

#include <QString>
#include <QChar>
#include <QQueue>
#include <QStack>
#include <QList>

class Token
{
  public:
    Token(const QString expr, int pos, int size, int type);
    int pos() const { return m_pos; };
    int size() const { return m_str.size(); };
    int type() const { return m_type; };
    QString str() const { return m_expr.mid(pos(), size()); };
  private:
    QString m_expr;
    int m_pos;
    int m.size;
    int m_type;
};

class SglExprLex
{
  public:
    bool autoFix();
    QList<Token> scan();
    void setExpression(const QString& newexpr);
  private:
    QString expr;
    QString escapetag;

    int index;
    int start;
    int end;
    int size;
    int escsize;
    QQueue<int> pendingToken;
    QStack<QString> closePar;
    QList<QByteArray>memlist;
    enum
    {
      stTopLevel, stNumber, stScale, stText
    } state;
    char radix;

    char current() const { return expr.at(index); };
    bool atEnd() const { return index == size; };
    QString currentSubStr() const;
    int lookup() const;
    int numLookup() const;
    int escLookup() const;
    int greedyLookup();
    int doLookup() const;
    int getNextTokenType();
    void scanLetters();
    bool isLetter() const;
    bool isDigit() const;
    bool isWhitespace() const;
    bool isSpecial() const;
    bool isAlphaNum() const
    bool matchesClosePar() const;
    bool matchesEscape() const;
    int scanNextToken();
    int getCloseChar();
    int scanTextToken();
    int scanSysToken();
    int scanWhitespaceToken();
    int scanDigitsToken();
    int scanTagToken();
    int scanIdentifierToken();
    int scanMidNumberToken();
    void updateState(int token);
    void reset();
    Token createToken(int type);
};
