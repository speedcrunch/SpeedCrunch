/* parser.cpp: lexical analyzers and parsers */
/*
    Copyright (C) 2007 Wolf Lammen.
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

#include "symboltables/symbols.cpp"
#include "symboltables/tables.cpp" // for the time being do not update CMakeList

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
  // enter is the first key pressed in a freshly launched program
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
  int lasttype = 0;
  int type;

  setExpression(newexpr);
  reset();
//  inText = false;
  do
  {
    type = getNextTokenType();
    switch (type)
    {
      case eol:
      case whitespace: continue;
      case ASSIGN: if (assignseqpos < 0) assignseqpos = start; continue;
      case UNKNOWNTOKEN: return false;
      default: assignseqpos = -1; break;
    }
    lasttype = type;
    ++tokencnt;
  } while ( type != eol );
  // remove trailing '='
  if ( assignseqpos >= 0 )
    expr.truncate(assignseqpos);
  // supply 'ans' as parameter, if just a function name is given
/*  if ( tokencnt == 1 && lasttoken == FUNCTION )
    expr.append(revLookup(ans));
  // close all unmatched left paranthesis
  if ( state == stText )
    expr.append(closePar.pop());*/
  while ( !closePar.isEmpty() )
    expr.append(' ' + closePar.pop());
  return true;
}
#if 0

QList<Token> SglExprLex::scan()
{
  QList<Token> tokenList;
  int type;

  reset();
  do
  {
    type = getNextTokenType();
    tokenList.append(createToken(type));
  } while ( type != eol );
  return tokenList;
}

Token SglExprLex::createToken(int type)
{
  // FIXME supply a symbol
  return Token(expr, start, end - start, type);
}
#endif
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
  if ( radix == 10 || radix == 0 ) return false;
  if ( c < 'A' ) return false;
  if ( c >= 'a' ) c -= 'a' - 'A';
  return  c <= 'F';
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
  index = 0;
  radix = 10;
  state = stTopLevel;
  escapetag = Settings::self()->escape;
  escsize = escapetag.size();
  pendingSymbol.clear();
  closePar.clear();
/*  lastTokenType = whitespace;
  strlist.clear();
  numlist.clear();
  paramlists.clear();*/
}

/*------------------------   lexical analyzing   -----------------------*/

QString SglExprLex::checkEscape(const QString& s)
{
  if (s.size() > 1 && s.at(0) == ' ')
    return escapetag + s.mid(1);
  return s;
}

int SglExprLex::symbolType() const
{
  if ( symbol )
    switch( symbol->type() )
    {
      case '.': return DOT;
      case '(': return OPENPAR;
      case '=': return ASSIGN;
      case ';': return SEP;
      case '"': return sot;
      default : return CLOSEPAR;
    };
  return UNKNOWNTOKEN;
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

int SglExprLex::getNextTokenType()
{
  int result;

  if ( !pendingSymbol.isEmpty() )
  {
    symbol = pendingSymbol.dequeue();
    result = symbol->type();
  }
  else
    result = scanNextToken();
  updateState(result);
  return result;
}

int SglExprLex::scanNextToken()
{
  int result = UNKNOWNTOKEN;

  symbol = 0;
  start = index;
  end = size;
  if (index == end )
    result = eol;
  else if ( state == stText )
    result = scanTextToken();
  else if ( matchesClosePar() )
    result = getClosePar();
  else if ( matchesEscape() )
    result = scanSysToken();
  else if ( isWhitespace() )
    result = scanWhitespaceToken();
  else if ( isDigit() )
    result = scanDigitsToken();
/*  else if ( state == stNumber || state == stScale )
    result = scanMidNumberToken();
  else if ( isLetter() )
    result = scanIdentifierToken();*/
/*  else
    result = greedyLookup();*/
  return result;
}

int SglExprLex::getClosePar()
{
  closePar.pop();
  return state == stText? eot : CLOSEPAR;
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

int SglExprLex::scanSysToken()
{
  index += escsize;
  int begin = index;
  bool exactMatch = index < size && isLetter();
  if ( exactMatch )
    scanLetters();
  else
    scanSpecial();
  symbol = Tables::builtinLookup(expr.mid(begin, end - begin), exactMatch);
  return symbolType();
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
/*  if ( state != stNumber )
  {
    result = scanTagToken();
    if ( result != UNKNOWNTOKEN )
      return result;
  }*/
  while ( !atEnd() && isDigit() )
    ++index;
  end = index;
  switch ( radix )
  {
    case 16: return HEXSEQ;
    case 8 : return OCTSEQ;
    case 2 : return BINSEQ;
    default: return DECSEQ;
  }
}
#if 0
int SglExprLex::scanTagToken()
{
  int tag;
  bool compltag = false;

  switch ( scanInNumber() )
  {
    case 'h': compltag = true; //fall through
    case 'H': tag = HEXTAG; break;
    case 'o': compltag = true; //fall through
    case 'O': tag = OCTTAG; break;
    case 'b': compltag = true; //fall through
    case 'B': tag = BINTAG; break;
    case 'D': tag = DECTAG; break;
    default: return UNKNOWNTOKEN;
  }
  if (!compltag)
    return tag;
  pendingToken.enqueue(tag);
  return CMPLTAG;
}

int SglExprLex::scanIdentifierToken()
{
  while ( !atEnd() && isAlphaNum() )
    ++index;
  end = index;
  return lookup();
}

int SglExprLex::scanMidNumberToken()
{
  if ( isLetter() )
  {
    scanLetters();
    return numLookup();
  }
  while ( !atEnd() && isSpecial() )
    ++index;
  end = index;
  return numLookup();
}
#endif
void SglExprLex::updateState(int token)
{
  if ( state != stNumber ) radix = 10;
  switch (token)
  {
    case sot: state = stText; break;
/*    case HEXTAG: radix += 8; // fall through
    case OCTTAG: redix += 6; // fall through
    case BINTAG: radix -= 8; // fall through
    case DECTAG:
    case CMPLTAG: */
    case DOT:
    case DECSEQ:
    case HEXSEQ:
    case OCTSEQ:
    case BINSEQ: state = stNumber; break;
/*    case DECSCALE:
    case SCALE: state = stScale; break;
    case GROUPCHAR: break;*/
    case TEXT: break;
    case OPENPAR:
      closePar.push(
          checkEscape(
              static_cast<OpenSymbol*>(symbol)->closeToken())); //fall through
    default: if ( state != stScale ) state = stTopLevel;
  }
}

/*------------------------   table lookup   -----------------------*/

QString SglExprLex::currentSubStr() const
{
  return expr.mid(start, end - start);
}
#if 0
int SglExprLex::lookup() const
{
  return doLookup(currentSubStr());
}

int SglExprLex::numLookup() const
{
  return doLookup('0' + currentSubStr());
}
int SglExprLex::greedyLookup()
{
  //set end
}

int SglExprLex::doLookup(const QString& token) const
{
  return UNKNOWNTOKEN;
}

/*------------------------   parser interface   -----------------------*/

QByteArray SglExprLex::basePrefix(base)
{
  // FIXME this actually belongs to hmath
  QByteArray result;

  switch (base)
  {
    case 2 : result = "0b"; break;
    case 8 : result = "0o"; break;
    case 16: result = "0x"; break;
    default:;
  }
}

DigitSeq SglExprLex::initStr(String s, char base)
{
  DigitSeq result;

  result.base = base;
  result.digits = s;
  result.complement = false;
  result.sign = 0;
  return result;
}

DigitSeq SglExprLex::appendStr(DigitSeq seq, String s)
{
  strlist.prepend(seq.digits);
  strlist.first().append(s);
  seq.digits = (const char*)strlist.first();
  return seq;
}

NumValue SglExprLex::convertStr(NumLiteral literal)
{
  // FIXME most of this should really be part of hmath
  char base = literal.intpart.base;
  char expbase = literal.exp.base;
  NumValue result;
  QByteArray str = basePrefix(base);
  int bias = 0;

  if ( literal.intpart.complement 
       && literal.intpart.digits[0] - '0' >= base >> 1 )
    bias = qstrlen(literal.intpart.digits);
  if ( qstrlen(literal.intpart.digits) )
    str.append(literal.intpart.digits);
  if ( qstrlen(literal.intpart.fracpart) )
  {
    str.append('.');
    str.append(literal.intpart.fracpart);
  }
  HNumber value(str);
  if ( bias )
    value -= raise(HNumber(base), bias);
  if ( qstrlen(literal.exp.digits) )
  {
    str = basePrefix(expbase);
    bias = 0;
    if ( literal.exp.complement
         && literal.exp.digits[0] - '0' >= base >> 1 )
      bias = qstrlen(literal.exp.digits);
    HNumber exp = HNumber(literal.exp.digits);
    if ( bias )
      exp -= raise(HNumber(expbase), bias);
    if ( literal.exp.sign < 0 )
      exp = - exp;
    value *= raise(HNumber(base), exp);
  }
  numList.prepend(value);
  result.val = &numList.first();
  result.percent = false;
  return result;
}

Params SglExprLex::addParam(Params list, NumValue val)
{
  if ( !list )
  {
    QList<NumValue> newparams;
    paramlists.prepend(newparams);
    list = &paramlists.first();
  }
  static_cast< QList<NumValue> >(list).append(val);
  return list;
}
#endif
