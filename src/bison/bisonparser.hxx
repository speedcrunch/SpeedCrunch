/* bisonparser.h: lexical analyzers and parsers */
/*
    Copyright (C) 2007, 2008 Wolf Lammen.

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

#include "math/hmath.hxx"
#include "bison.h"
#include "symboltables/tables.hxx"

#include <QString>
#include <QStringList>
#include <QChar>
#include <QQueue>
#include <QStack>
#include <QList>

class SglExprLex
{
  public:

    class Token
    {
      public:
        Token(const QString& expr, int pos, int size, int type, const Symbol*);
        int pos() const { return m_pos; };
        int size() const { return m_size; };
        int type() const { return m_type; };
        QString str() const { return m_expr.mid(pos(), size()); };
        const Symbol* symbol() const { return m_symbol; }
      private:
        QString m_expr;
        int m_pos;
        int m_size;
        int m_type;
        const Symbol* m_symbol;
    };
    bool autoFix(const QString& newexpr);
    Variant eval();
    bool run(const QStringList& script);
    const QList<Token>& scan();
    void setExpression(const QString& newexpr);
    static SglExprLex& self();
  private:
    typedef struct
    {
      int type;
      const Symbol* symbol;
    } ScanResult;
    static SglExprLex* instance;
    QString expr;
    QString escapetag;

    enum
    {
      whitespace = ' ',
      eol        = '\0',
      sot        = '`',
      eot        = '\'',
    };
    enum { maxOverloadSymbols = 4 };

    int index;
    int start;
    int end;
    int size;
    int escsize;
    ScanResult lastScanResult;
    QQueue<ScanResult> pending;
    QStack<QString> closePar;
    QList<Token> tokens;
    QList<QString>strlist;
    QList<Variant>numlist;
    QList<ParamList>paramlists;

    enum
    {
      stTopLevel, stNumber, stScale, stText
    } state; 
    char radix;

    QChar current() const { return expr.at(index); };
    bool atEnd() const { return index == size; };
    static int symbolType(SymType t);
    static int baseTag(const Symbol* symbol);
    QString currentSubStr() const;
    bool checkExact();
    ScanResult lookup(bool exact = true, char prefix = 0);
    static const Symbol* follow(const Symbol* symbol);
    static int findSymbolType(SymType* types, SymType match);
    static QString addDelim(const QString&);

    void getNextScanResult();
    QString checkEscape(const QString& s);
    void scanLetters();
    void scanSpecial();
    bool isLetter() const;
    bool isDigit() const;
    bool isHexChar() const;
    bool isWhitespace() const;
    bool isSpecial() const;
    bool isAlphaNum() const;
    ScanResult searchResult2scanResult(Tables::SearchResult);
    bool matchesClosePar() const;
    bool matchesEscape() const;
    ScanResult scanNextToken();
    int getClosePar();
    int scanTextToken();
    ScanResult scanSysToken();
    int scanWhitespaceToken();
    int scanDigitsToken();
    ScanResult scanSpecialCharToken();
    int tryScanTagToken();
    ScanResult scanIdentifierToken();
    ScanResult scanMidNumberToken();
    void updateState();
    void reset();

    SafeVariant* allocNumber(const Variant& n);
    SafeQString* allocString(const QString&);
    NumValue variant2numValue(const Variant&);
    static Variant numValue2variant(NumValue);
    static int getToken(YYSTYPE* val, int* pos, int* lg);
    int mGetToken(YYSTYPE* val, int* pos, int* lg);
    static Params addParam(Params list, NumValue val);
    Params mAddParam(Params list, NumValue val);
    static NumValue callFunction(Func f, Params params);
    NumValue mCallFunction(Func f, Params params);
    static NumValue str2Val(SafeQString*);
    NumValue mStr2Val(SafeQString*);
    static DigitSeq appendStr(DigitSeq ds, SafeQString* seq);
    DigitSeq mAppendStr(DigitSeq ds, SafeQString* seq);
    static NumValue convertStr(NumLiteral literal);
    NumValue mConvertStr(NumLiteral literal);

    static const char* basePrefix(char base);
    SglExprLex( const SglExprLex& );
    SglExprLex& operator=( const SglExprLex& );
    SglExprLex();
    ~SglExprLex();
};

#endif /* _BISONPARSER_H */
