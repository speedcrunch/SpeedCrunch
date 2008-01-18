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
  bool inText = false;

  setExpression(newexpr);
  reset();
  do
  {
    getNextScanResult();
    switch (lastScanResult.type)
    {
      case eol:
      case sot:
      case eot: inText = !inText;
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
  if (inText)
    expr.append(closePar.pop());
  while ( !closePar.isEmpty() )
    expr.append(addDelim(closePar.pop()));
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
  cb.assignVar = assignVar;
  cb.getToken = getToken;
  index = -1;
  if (parseexpr(cb, &pResult, &errorpos, &errortokenlg) == PARSE_OK)
    result = numValue2variant(pResult);
  else 
    result = SyntaxError;
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
    result = Error(eval()) == Success;
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
  pending.clear();
  closePar.clear();
  lastScanResult.type = whitespace;
  lastScanResult.symbol = 0;
  tokens.clear();
}

/*------------------------   lexical analyzing   -----------------------*/

int SglExprLex::findSymbolType(SymType* types, SymType match)
{
  int result = -1;
  for (; ++result < maxOverloadSymbols && *(types + result) != match;);
  if (result == maxOverloadSymbols)
    result = -1;
  return result;
}

QString SglExprLex::checkEscape(const QString& s)
{
  if (s.size() > 1 && s.at(0) == ' ')
    return escapetag + s.mid(1);
  return s;
}

static int tokenOrder(int token)
{
  switch(token)
  {
    case L0       :
    case R1       :
    case L2       :
    case R3       :
    case L4       :
    case R5       :
    case L6       :
    case R7       :
    case L8       :
    case R9       :
    case L10      :
    case R11      :
    case L12      :
    case R13      :
    case L14      : return 2;
    case PREFIX0  :
    case PREFIX2  :
    case PREFIX4  :
    case PREFIX6  :
    case PREFIX8  :
    case PREFIX10 :
    case PREFIX12 :
    case PREFIX14 : return 1;
    default       : return 0;
  }
}

SglExprLex::ScanResult SglExprLex::searchResult2scanResult(Tables::SearchResult sr)
{
  ScanResult tokens[maxOverloadSymbols];
  ScanResult tmp;
  SymType stype;

  tokens[0].type = UNKNOWNTOKEN;
  tokens[0].symbol = 0;
  if (sr.count > maxOverloadSymbols)
    sr.count = 0;
  for (int i = -1; ++i < sr.count; ++sr.pt)
  {
    tmp.symbol = sr.pt.value();
    stype = tmp.symbol? tmp.symbol->type() : unassigned;
    switch(stype)
    {
      case tagSym:
        tmp.type = baseTag(tmp.symbol);
        break;
      case operatorSym:
        tmp.type = opToken(tmp.symbol);
        break;
      default:
        tmp.type = symbolType(stype);
    }
    // sort overloaded symbols (insertion sort)
    int j = i;
    for (; --j >= 0 && tokenOrder(tokens[j].type) < tokenOrder(tmp.type);)
      tokens[j+1] = tokens[j];
    tokens[j+1] = tmp;
  }
  for (int i = sr.count; --i > 0;)
    pending.enqueue(tokens[i]);
  if (sr.count == 1)
  {
    switch (stype)
    {
      case operatorSym:
        if (!tmp.symbol->asOp()->isUnary())
        {
          // append a NOPREFIX token for not overloaded binary operators
          tmp.type = NOPREFIX;
          tmp.symbol = 0;
          pending.enqueue(tmp);
        }
        break;
      case tagSym:
        if (tmp.symbol->asTag()->complement())
        {
          // split a complement tag into a base token and a complement token
          pending.enqueue(tmp);
          tokens[0].type = CMPLTAG;
        }
        break;
      default: ;
    }
  }
  return tokens[0];
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
  do
  {
    if ( !pending.isEmpty() )
      lastScanResult = pending.dequeue();
    else
      lastScanResult = scanNextToken();
    updateState();
  } while (lastScanResult.type == leaveState);
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
  if (r.pt.value())
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
    if (!atEnd() && isLetter())
    {
      ++index;
      while ( !atEnd() && isLetter() && !isHexChar() )
        ++index;
      end = index;
      ScanResult sr = lookup();
      if (sr.symbol && sr.symbol->type() == tagSym)
        return sr.type;
    }
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
  ScanResult result = lookup(checkExact(), state == stNumber? '0' : '1');
  if (result.type == UNKNOWNTOKEN)
  {
    result.type = leaveState;
    end = start;
  }
  return result;
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
    case DECSEQ:
    case HEXSEQ:
    case OCTSEQ:
    case BINSEQ: if (state != stScale) state = stNumber; break;
    case DOT: state = stNumber; break;
    case DECSCALE:
    case SCALE: state = stScale; break;
    case SIGN:
    case GROUPCHAR: break;
    case leaveState:
    case TEXT: state = stTopLevel; break;
    case sot: state = stText; // fall through
    case OPENPAR:
      closePar.push(
          checkEscape(lastScanResult.symbol->asOpen()->closeToken())); //fall through
    default: if ( state != stScale && state != stText ) state = stTopLevel;
  }
}

/*------------------------   table lookup   -----------------------*/

int SglExprLex::symbolType(SymType t)
{
  switch( t )
  {
    case dot         : return DOT;
    case openSym     : return OPENPAR;
    case assign      : return ASSIGN;
    case separator   : return SEP;
    case quote       : return sot;
    case functionSym : return FUNCTION;
    case scale       : return SCALE;
    case decscale    : return DECSCALE;
    case signPlus    :
    case signMinus   : return SIGN;
    case group       : return GROUPCHAR;
    case varSym      : return VARIABLE;
    case ansSym      :
    case constSym    : return CONSTANT;
    default          : return UNKNOWNTOKEN;
  };
}

int SglExprLex::baseTag(const Symbol* symbol)
{
  switch(symbol->asTag()->base())
  {
    case 16: return HEXTAG;
    case 8 : return OCTTAG;
    case 2 : return BINTAG;
    default: return DECTAG;
  }
}

int SglExprLex::opToken(const Symbol* symbol)
{
  const OperatorSymbol* os = static_cast<const OperatorSymbol*>(symbol);
  if (os->isUnary())
    switch (os->precedence())
    {
      case 0 : return PREFIX0;
      case 2 : return PREFIX2;
      case 4 : return PREFIX4;
      case 6 : return PREFIX6;
      case 8 : return PREFIX8;
      case 10: return PREFIX10;
      case 12: return PREFIX12;
      case 14: return PREFIX14;
      case 1 :
      case 3 :
      case 5 :
      case 7 :
      case 9 :
      case 11:
      case 13: return POSTFIXOP;
      default: return UNKNOWNTOKEN;
    }
  switch (os->precedence())
  {
    case 0 : return L0;
    case 1 : return R1;
    case 2 : return L2;
    case 3 : return R3;
    case 4 : return L4;
    case 5 : return R5;
    case 6 : return L6;
    case 7 : return R7;
    case 8 : return L8;
    case 9 : return R9;
    case 10: return L10;
    case 11: return R11;
    case 12: return L12;
    case 13: return R13;
    case 14: return L14;
    default: return UNKNOWNTOKEN;
  }
}

QString SglExprLex::addDelim(const QString& closekey)
{
  if (closekey.size() == 1 && !Tables::keysContainChar(closekey.at(0)))
    return closekey;
  return ' ' + closekey;
}

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
  if (r.count != 0)
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

String SglExprLex::allocString(const QString& s)
{
  strlist.append(s);
  return strlist.size() - 1;
}

VariantIdx SglExprLex::allocNumber(const Variant& n)
{
  numlist.append(n);
  return numlist.size() - 1;
}

HNumber SglExprLex::cvtNumber(const DigitSeq& descriptor, String frac)
{
  QByteArray str;
  HNumber bias(0);
  if (descriptor.digits)
    str = strlist.at(descriptor.digits).toLatin1();
  if (descriptor.complement && str[0] - '0' >= descriptor.base >> 1)
    bias = HMath::raise(HNumber(descriptor.base), str.size());
  if (descriptor.base != 10)
    str.prepend(basePrefix(descriptor.base));
  if (frac >= 0)
  {
    str.append('.');
    str.append(strlist.at(frac).toLatin1());
  }
  if (descriptor.sign >= 0)
    return HNumber(str.data()) - bias;
  return bias - HNumber(str.data());
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
  const Variant* v = &numlist.at(n.val);
/*  if (n.percent)
    result = *(const HNumber*)v / HNumber(100);
  else*/
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
    case L0        :
    case R1        :
    case L2        :
    case R3        :
    case L4        :
    case R5        :
    case L6        :
    case R7        :
    case L8        :
    case R9        :
    case L10       :
    case R11       :
    case L12       :
    case R13       :
    case L14       :
    case PREFIX0   :
    case PREFIX2   :
    case PREFIX4   :
    case PREFIX6   :
    case PREFIX8   :
    case PREFIX10  :
    case PREFIX12  :
    case PREFIX14  :
    case POSTFIXOP :
    case FUNCTION  : val->func = index; break;
    case DECSEQ    :
    case HEXSEQ    :
    case OCTSEQ    :
    case BINSEQ    :
    case TEXT      : val->string = allocString(token.str()); break;
    case SIGN      : val->sign = token.symbol()->type() == 'M'? -1 : 1; break;
    case CONSTANT  :
      val->numvalue = variant2numValue(token.symbol()->asConst()->value()); break;
    case VARIABLE  :
      val->var.v = index;
      val->var.d = variant2numValue(token.symbol()->asConst()->value());
      break;
    default        : ;
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
    list = paramlists.size() - 1;
  }
  paramlists[list].append(numValue2variant(val));
  return list;
}

NumValue SglExprLex::callFunction(Func f, Params params)
{
  return instance->mCallFunction(f, params);
}

NumValue SglExprLex::mCallFunction(Func f, Params params)
{
  return variant2numValue(tokens.at(f).symbol()
                          ->asFunc()->eval(paramlists.at(params)));
}

NumValue SglExprLex::str2Val(String s)
{
  return instance->mStr2Val(s);
}

NumValue SglExprLex::mStr2Val(String s)
{
  NumValue result;
  Variant v = strlist.at(s);
  result.percent = 0;
  result.val = allocNumber(v);
  return result;
}

DigitSeq SglExprLex::appendStr(DigitSeq ds, String seq)
{
  return instance->mAppendStr(ds, seq);
}

DigitSeq SglExprLex::mAppendStr(DigitSeq ds, String seq)
{
  DigitSeq result = ds;
  ds.digits = allocString(strlist.at(ds.digits) + strlist.at(seq));
  return result;
}

NumValue SglExprLex::convertStr(NumLiteral literal)
{
  return instance->mConvertStr(literal);
}

NumValue SglExprLex::mConvertStr(NumLiteral literal)
{
  NumValue result;
/*  HNumber value = cvtNumber(literal.intpart, literal.fracpart);
  if (literal.exp.digits)
    value *= HMath::raise(HNumber(literal.intpart.base), cvtNumber(literal.exp));
  result.val = allocNumber(value);
  result.percent = false;*/
  return result;
}

NumValue SglExprLex::assignVar(Var variable, NumValue val)
{
  return instance->mAssignVar(variable, val);
}

NumValue SglExprLex::mAssignVar(Var variable, NumValue val)
{
  *(tokens.at(variable.v).symbol()->asVar()->leftVal()) = numValue2variant(val);
  variable.d = val;
  return val;
}

Var SglExprLex::createVar(String s)
{
  Var result;
/*  result.v = Tables::createVarSymbol(*getQString(s))->writePtr();
  result.d.val = 0;
  result.d.percent = false;*/
  return result;
}
