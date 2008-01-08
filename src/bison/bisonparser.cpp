/* parser.cpp: lexical analyzers and parsers */
/*
    Copyright (C) 2007, 2008 Wolf Lammen.
    Partly based on ideas from Ariya Hidayat

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

#include "bison/bisonparser.hxx"
#include "settings.hxx"

#include "symboltables/symbols.cpp" // for the time being do not update CMakeList
#include "symboltables/tables.cpp" // for the time being do not update CMakeList
#include "main/errors.h"

SglExprLex::Token::Token(const QString& expr, int pos, int size,
                         int type, const Symbol* symbol)
  : m_expr(expr), m_pos(pos), m_size(size), m_type(type), m_symbol(symbol)
{
}

/*-------------------------   singleton   -----------------------------*/

// the bison parser is not re-entrant, so use a singleton

SglExprLex* SglExprLex::instance = 0;

SglExprLex& SglExprLex::self()
{
  if ( !instance )
    instance = new SglExprLex;
  return *instance;
}

SglExprLex::SglExprLex()
{
  // necessary, because the editor does not call Evaluator.setExpression, when
  // ENTER is the first key pressed in a freshly launched program
  size = 0;
}

/*------------------------   public interface   -----------------------*/

void SglExprLex::setExpression(const QString& newexpr)
{
  expr = newexpr;
  size = expr.size();
}

bool SglExprLex::autoFix(const QString& newexpr)
{
  int tokencnt = 0;
  int assignseqpos = -1;
  int lastToken;

  setExpression(newexpr);
  reset();
  do
  {
    getNextScanResult();
    switch (lastScanResult.type)
    {
      case eol:
      case whitespace: continue;
      case ASSIGN: if (assignseqpos < 0) assignseqpos = start; continue;
      case UNKNOWNTOKEN: return false;
      default: assignseqpos = -1; break;
    }
    lastToken = lastScanResult.type;
    ++tokencnt;
  } while ( lastScanResult.type != eol );
  // remove trailing '='
  if ( assignseqpos >= 0 )
    expr.truncate(assignseqpos);
  // supply 'ans' as parameter, if just a function name is given
/*  if ( tokencnt == 1 && lasttoken == FUNCTION )
    expr.append(revLookup(ans));*/
  // close all unmatched left paranthesis
  while ( !closePar.isEmpty() )
    expr.append(' ' + closePar.pop());
  return true;
}

Variant SglExprLex::eval()
{
  CallBacks cb;
  NumValue pResult;
  int errorpos;
  int errortokenlg;
  Variant result;

  scan();
  cb.str2Val = str2Val;
  cb.appendStr = appendStr;
  cb.convertStr = convertStr;
  cb.addParam = addParam;
  cb.callFunction = callFunction;
  cb.assignVar = 0;
  cb.getToken = getToken;
  index = -1;
  if (parseexpr(cb, &pResult, &errorpos, &errortokenlg) == PARSE_OK)
    result = numValue2variant(pResult);
  else 
    result = SCANNER_SYNTAX_ERROR;
  strlist.clear();
  numlist.clear();
  paramlists.clear();
  tokens.clear();
  return result;
}

bool SglExprLex::run(const QStringList& script)
{
  int i = -1;
  bool result = true;
  while (result && ++i < script.size())
  {
    setExpression(script.at(i));
    result = eval().type() != TError;
  }
  return result;
}

const QList<SglExprLex::Token>& SglExprLex::scan()
{
  int type;
  reset();
  do
  {
    getNextScanResult();
    type = lastScanResult.type;
    if (type != eol)
      tokens.append(Token(expr, start, end - start, type,
                          lastScanResult.symbol));
  } while ( type != eol );
  return tokens;
}

/*------------------------   character classes   -----------------------*/

bool SglExprLex::isLetter() const
{
  QChar c = current();
  return c.isLetter() || (c == '_');
}

bool SglExprLex::isDigit() const
{
  char c = current().toAscii();
  if ( c < '0' ) return false;
  if ( c <= '1' ) return true;
  if ( radix == 2 ) return false;
  if ( c <= '7' ) return true;
  if ( radix == 8 ) return false;
  if ( c <= '9' ) return true;
  if ( radix == 10 ) return false;
  if ( c < 'A' ) return false;
  if ( c >= 'a' ) c -= 'a' - 'A';
  return  c <= 'F';
}

bool SglExprLex::isHexChar() const
{
  char c = current().toAscii();
  if ( c >= 'a' ) c -= 'a' - 'A';
  return  c >= 'A' && c <= 'F';
}

bool SglExprLex::isWhitespace() const
{
  return current() <= ' ';
}

bool SglExprLex::isSpecial() const
{
  return !isWhitespace() && !isAlphaNum();
}

bool SglExprLex::isAlphaNum() const
{
  return isLetter() || isDigit();
}

void SglExprLex::scanLetters()
{
  while ( !atEnd() && isLetter() )
    ++index;
  end = index;
}

void SglExprLex::scanSpecial()
{
  int from;

  from = index;
  while ( !atEnd() && isSpecial() )
    ++index;
  end = expr.indexOf(escapetag, from);
  if ( end < 0 || index < end )
    end = index;
}

/*------------------------   initialization/finalization   -----------------------*/

void SglExprLex::reset()
{
  end = 0;
  radix = 10;
  state = stTopLevel;
  escapetag = Settings::self()->escape;
  escsize = escapetag.size();
  pendingSymbol.clear();
  closePar.clear();
  lastScanResult.type = whitespace;
  lastScanResult.symbol = 0;
  tokens.clear();
}

/*------------------------   lexical analyzing   -----------------------*/

QString SglExprLex::checkEscape(const QString& s)
{
  if (s.size() > 1 && s.at(0) == ' ')
    return escapetag + s.mid(1);
  return s;
}

int SglExprLex::symbolType(SymType t)
{
  switch( t )
  {
    case dot         : return DOT;
    case openPar     : return OPENPAR;
    case assign      : return ASSIGN;
    case separator   : return SEP;
    case quote       : return sot;
    case functionSym : return FUNCTION;
    case scale       : return SCALE;
    case decscale    : return DECSCALE;
    case group       : return GROUPCHAR;
    default          : return UNKNOWNTOKEN;
  };
}

int SglExprLex::baseTag(const Symbol* symbol)
{
  switch(static_cast<const TagSymbol*>(symbol)->base())
  {
    case 16: return HEXTAG;
    case 8 : return OCTTAG;
    case 2 : return BINTAG;
    default: return DECTAG;
  }
}

SglExprLex::ScanResult SglExprLex::searchResult2scanResult(Tables::SearchResult sr)
{
  const ReferenceSymbol* rf;
  const TagSymbol* tag;
  ScanResult result;
  result.symbol = sr.symbol;
  result.type = 0;
  SymType symtype = result.symbol? sr.symbol->type() : unassigned;
  switch (symtype)
  {
    case referenceSym:
      rf = static_cast<const ReferenceSymbol*>(result.symbol);
      result.symbol = (*rf)[0];
      symtype = result.symbol->type();
      break;
    case tagSym:
      tag = static_cast<const TagSymbol*>(result.symbol);
      if (tag->complement())
      {
        pendingSymbol.enqueue(sr.symbol);
        result.type = CMPLTAG;
      }
      else
        result.type = baseTag(result.symbol);
    default: ;
  }
  if (result.type == 0)
    result.type = symbolType(symtype);
  return result;
}

bool SglExprLex::matchesEscape() const
{
  return escsize != 0 && expr.mid(start, escsize) == escapetag;
}

bool SglExprLex::matchesClosePar() const
{
  return !closePar.isEmpty()
         && expr.mid(start, closePar.top().size()) == closePar.top();
}

void SglExprLex::getNextScanResult()
{
  if ( !pendingSymbol.isEmpty() )
  {
    lastScanResult.symbol = pendingSymbol.dequeue();
    switch (lastScanResult.symbol->type())
    {
      case tagSym:
        lastScanResult.type = baseTag(lastScanResult.symbol);
        break;
      default: ;
    }
  }
  else
    lastScanResult = scanNextToken();
  updateState();
}

SglExprLex::ScanResult SglExprLex::scanNextToken()
{
  ScanResult result;
  result.type = UNKNOWNTOKEN;
  result.symbol = 0;
  start = end;
  index = start;
  end = size;
  if (index == end )
    result.type = eol;
  else if ( state == stText )
    result.type = scanTextToken();
  else if ( matchesClosePar() )
    result.type = getClosePar();
  else if ( matchesEscape() )
    result = scanSysToken();
  else if ( isWhitespace() )
    result.type = scanWhitespaceToken();
  else if ( isDigit() )
    result.type = scanDigitsToken();
  else if ( state == stNumber || state == stScale )
    result = scanMidNumberToken();
  else if ( isLetter() )
    result = scanIdentifierToken();
  else
    result = scanSpecialCharToken();
  return result;
}

int SglExprLex::getClosePar()
{
  end = start + closePar.pop().size();
  return lastScanResult.type == TEXT? eot : CLOSEPAR;
}

int SglExprLex::scanTextToken()
{
  int finalQuote = expr.indexOf(closePar.top(), start);
  if ( finalQuote >= 0 )
  {
    end = finalQuote;
    return TEXT;
  }
  return UNKNOWNTOKEN;
}

SglExprLex::ScanResult SglExprLex::scanSysToken()
{
  Tables::SearchResult r;
  index += escsize;
  int begin = index;
  bool exactMatch = checkExact();
  r = Tables::builtinLookup(expr.mid(begin, end - begin), exactMatch);
  if (r.symbol)
  {
    // set 'end' according to the characters used in the lookup
    end = begin + r.keyused;
    if (exactMatch && index < size && isWhitespace())
      ++end;
  }
  return searchResult2scanResult(r);
}

int SglExprLex::scanWhitespaceToken()
{
  while ( !atEnd() && isWhitespace() )
    ++index;
  end = index;
  return whitespace;
}

int SglExprLex::scanDigitsToken()
{
  int result = UNKNOWNTOKEN;
  if ( state != stNumber )
  {
    radix = 10;
    result = tryScanTagToken();
  }
  if ( result == UNKNOWNTOKEN )
  {
    while ( !atEnd() && isDigit() )
      ++index;
    end = index;
    switch ( radix )
    {
      case 16: result = HEXSEQ; break;
      case 8 : result = OCTSEQ; break;
      case 2 : result = BINSEQ; break;
      default: result = DECSEQ; break;
    }
  }
  return result;
}

SglExprLex::ScanResult SglExprLex::scanSpecialCharToken()
{
  scanSpecial();
  return lookup(false);
}

int SglExprLex::tryScanTagToken()
{
  if (current() == '0')
  {
    ++index;
    while ( !atEnd() && isLetter() && !isHexChar() )
      ++index;
    end = index;
    ScanResult sr = lookup();
    if (sr.symbol && sr.symbol->type() == tagSym)
      return sr.type;
  }
  return UNKNOWNTOKEN;
}

SglExprLex::ScanResult SglExprLex::scanIdentifierToken()
{
  while ( !atEnd() && isAlphaNum() )
    ++index;
  end = index;
  return lookup();
}

SglExprLex::ScanResult SglExprLex::scanMidNumberToken()
{
  return lookup(checkExact(), '0');
}

void SglExprLex::updateState()
{
  switch (lastScanResult.type)
  {
    case HEXTAG: radix += 8; // fall through
    case OCTTAG: radix += 6; // fall through
    case BINTAG: radix -= 8; // fall through
    case DECTAG:
    case CMPLTAG:
    case DOT:
    case DECSEQ:
    case HEXSEQ:
    case OCTSEQ:
    case BINSEQ: state = stNumber; break;
    case DECSCALE:
    case SCALE: state = stScale; break;
    case GROUPCHAR: break;
    case TEXT: state = stTopLevel; break;
    case sot: state = stText; // fall through
    case OPENPAR:
      closePar.push(
          checkEscape(
              static_cast<const OpenSymbol*>(lastScanResult.symbol)->closeToken())); //fall through
    default: if ( state != stScale && state != stText ) state = stTopLevel;
  }
}

/*------------------------   table lookup   -----------------------*/

QString SglExprLex::currentSubStr() const
{
  return expr.mid(start, end - start);
}

SglExprLex::ScanResult SglExprLex::lookup(bool exact, char prefix)
{
  Tables::SearchResult r;
  QString key = currentSubStr();
  if (prefix)
    key = prefix + key;
  r = Tables::lookup(key , exact);
  if (r.symbol)
    // set 'end' according to the characters used in the lookup
    end = start + r.keyused - (prefix == 0? 0: 1);
  return searchResult2scanResult(r);
}

bool SglExprLex::checkExact()
{
  bool exactMatch = index < size && isLetter();
  if (exactMatch)
    scanLetters();
  else
    scanSpecial();
  return exactMatch;
}

/*------------------------   parser interface   -----------------------*/

static inline const QString* getQString(SafeQString* s)
{
  return reinterpret_cast<QString*>(s);
}

static inline const Variant* getVariant(SafeVariant* v)
{
  return reinterpret_cast<const Variant*>(v);
}

static inline ParamList* getParams(Params p)
{
  return reinterpret_cast<ParamList*>(p);
}

SafeQString* SglExprLex::allocString(const QString& s)
{
  strlist.append(s);
  return reinterpret_cast<SafeQString*>(&strlist.last());
}

SafeVariant* SglExprLex::allocNumber(const Variant& n)
{
  numlist.append(n);
  return reinterpret_cast<SafeVariant*>(&numlist.last());
}

NumValue SglExprLex::variant2numValue(const Variant& v)
{
  NumValue result;
  result.percent = 0;
  result.val = allocNumber(v);
  return result;
}

Variant SglExprLex::numValue2variant(NumValue n)
{
  Variant result;
  const Variant* v = getVariant(n.val);
  if (n.percent)
    result = *(const HNumber*)v / HNumber(100);
  else
    result = *v;
  return result;
}

int SglExprLex::getToken(YYSTYPE* val, int* pos, int* lg)
{
  return instance->mGetToken(val, pos, lg);
}

int SglExprLex::mGetToken(YYSTYPE* val, int* pos, int* lg)
{
  while (++index < tokens.size())
  {
    bool skip;
    switch (tokens.at(index).type())
    {
      case eot:
      case sot:
      case whitespace: skip = true; break;
      default: skip = false; break;
    }
    if (!skip)
      break;
  }
  if (index >= tokens.size())
    return 0;
  const Token& token = tokens.at(index);
  *pos = token.pos();
  *lg = token.size();
  switch (token.type())
  {
    case FUNCTION: val->func = token.symbol(); break;
    case DECSEQ  :
    case HEXSEQ  :
    case OCTSEQ  :
    case BINSEQ  :
    case TEXT    : val->string = allocString(token.str()); break;
    default      : ;
  }
  return token.type();
}

Params SglExprLex::addParam(Params list, NumValue val)
{
  return instance->mAddParam(list, val);
}

Params SglExprLex::mAddParam(Params list, NumValue val)
{
  if (!list)
  {
    ParamList newList;
    paramlists.append(newList);
    list = reinterpret_cast<Params>(&paramlists.last());
  }
  getParams(list)->append(numValue2variant(val));
  return list;
}

NumValue SglExprLex::callFunction(Func f, Params params)
{
  return instance->mCallFunction(f, params);
}

NumValue SglExprLex::mCallFunction(Func f, Params params)
{
  return variant2numValue(
           dynamic_cast<const FunctionSymbolIntf*>(f)->eval(
             *getParams(params)));
}

NumValue SglExprLex::str2Val(SafeQString* s)
{
  return instance->mStr2Val(s);
}

NumValue SglExprLex::mStr2Val(SafeQString* s)
{
  NumValue result;
  Variant v = *getQString(s);
  result.percent = 0;
  result.val = allocNumber(v);
  return result;
}

DigitSeq SglExprLex::appendStr(DigitSeq ds, SafeQString* seq)
{
  return instance->mAppendStr(ds, seq);
}

DigitSeq SglExprLex::mAppendStr(DigitSeq ds, SafeQString* seq)
{
  DigitSeq result = ds;
  ds.digits = allocString(*getQString(ds.digits) + *getQString(seq));
  return result;
}

NumValue SglExprLex::convertStr(NumLiteral literal)
{
  return instance->mConvertStr(literal);
}

NumValue SglExprLex::mConvertStr(NumLiteral literal)
{
  // FIXME most of this should really be part of hmath
  NumValue result;
  char base = literal.intpart.base;
  char expbase = literal.exp.base;
  QByteArray str;
  if (literal.intpart.digits)
    str = getQString(literal.intpart.digits)->toLatin1();

  int bias = 0;
  if ( literal.intpart.complement
       && str[0] - '0' >= base >> 1 )
    bias = str.size();
  if (base != 10)
    str = basePrefix(base) + str;
  if (literal.fracpart)
  {
    str.append('.');
    str.append(getQString(literal.fracpart)->toLatin1());
  }
  HNumber value(str.data());
  if (bias)
    value -= HMath::raise(HNumber(base), bias);
  if (literal.exp.digits)
  {
    str = basePrefix(expbase) + getQString(literal.exp.digits)->toLatin1();
//    bias = 0;
/*    if ( literal.exp.complement
         && literal.exp.digits[0] - '0' >= base >> 1 )
      bias = qstrlen(literal.exp.digits);*/
    HNumber exp(str.data());
/*    if ( bias )
      exp -= raise(HNumber(expbase), bias);
    if ( literal.exp.sign < 0 )
      exp = - exp;*/
    value *= HMath::raise(HNumber(base), exp);
  }
  result.val = allocNumber(value);
  result.percent = false;
  return result;
}

const char* SglExprLex::basePrefix(char base)
{
  // FIXME this actually belongs to hmath

  const char* result;
  switch (base)
  {
    case 2 : result = "0b"; break;
    case 8 : result = "0o"; break;
    case 16: result = "0x"; break;
    default: result = "0d"; break;
  }
  return result;
}
