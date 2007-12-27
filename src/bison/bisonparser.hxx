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
#include "math/hmath.hxx"

#include <QString>
#include <QChar>
#include <QQueue>
#include <QStack>
#include <QList>

class Symbol
{
  public:
    virtual int type();
    virtual ~Symbol();
};

typedef Symbol* PSymbol;

class SglExprLex
{
  public:
#if 0

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
#endif
    bool autoFix(const QString& newexpr);
//     QList<Token> scan();
    void setExpression(const QString& newexpr);
    static SglExprLex& self();
  private:
    static SglExprLex* instance;
    QString expr;
    QString escapetag;

    typedef enum
    {
      whitespace = ' ',
      eol        = '\0',
    };

    int index;
    int start;
    int end;
    int size;
    int escsize;
    PSymbol symbol;
//     int lastTokenType;
    QQueue<PSymbol> pendingSymbol;
/*    QStack<QString> closePar;
    QList<QByteArray>strlist;
    QList<HNumber>numlist;
    QList<QList<NumValue> >paramlists; */
    enum
    {
      stTopLevel, stNumber, stScale, stText
    } state; 
    char radix;

    QChar current() const { return expr.at(index); };
    bool atEnd() const { return index == size; };
    int symbolType() const;
    QString currentSubStr() const;
#if 0
    int lookup() const;
    int numLookup() const;
#endif
    int escLookup();
//     int greedyLookup();
    int doLookup(const QString&) const;
    int getNextTokenType();
#if 0
#endif
    void scanLetters();
    bool isLetter() const;
    bool isDigit() const;
    bool isWhitespace() const;
#if 0
    bool isSpecial() const;
    bool isAlphaNum() const
    bool matchesClosePar() const;
#endif
    bool matchesEscape() const;
    int scanNextToken();
#if 0
    int getCloseChar();
    int scanTextToken();
#endif
    int scanSysToken();
    int scanWhitespaceToken();
    int scanDigitsToken();
#if 0
    int scanTagToken();
    int scanIdentifierToken();
    int scanMidNumberToken();
#endif
    void updateState(int token);
    void reset();
#if 0
    Token createToken(int type);
    DigitSeq initStr(String s, char base);
    DigitSeq appendStr(DigitSeq seq, String s);
    NumValue convertStr(NumLiteral literal);
    static QByteArray basePrefix(char base);
#endif
  private:
    SglExprLex( const SglExprLex& );
    SglExprLex& operator=( const SglExprLex& );
    SglExprLex();
    ~SglExprLex();
};

#endif /* _BISONPARSER_H */
