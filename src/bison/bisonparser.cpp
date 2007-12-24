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

/*------------------------   public interface   -----------------------*/

void SglExprLex::setExpression(const QString& newexpr)
{
  expr = newexpr;
  size = expr.size();

  // replace all funny characters
  for (int i = -1; ++i < size ;)
    if (current() < ' ')
      expr[i] = ' ';

  reset();
}

bool SglExprLex::autoFix();
{
  int tokencnt = 0;
  int assignseqpos = -1;
  int lasttype = 0;
  int type;
  QString close;

  reset();
  inText = false;
  do
  {
    type = getNextTokenType();
    switch (type)
    {
      case eol:
      case whitespace: continue;
      case ASSIGN: if (assignpos < 0) assignpos = curpos; continue;
      case TOKENUNKNOWN: return false;
      default: assignpos = -1; break;
    }
    lasttype = type;
    ++tokencount;
  } while ( type != eol );
  // remove trailing '='
  if ( assignpos >= 0 )
    expr.truncate(assignpos);
  // supply 'ans' as parameter, if just a function name is given
  if ( tokencnt == 1 && lasttoken == FUNCTION )
    expr.append(revLookup(ans));
  // close all unmatched left paranthesis
  if ( state == stText )
    expr.append(closePar.pop());
  while ( !closePar.isEmpty() )
    expr.append( ' ' + closePar.pop());
  return true;
}

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

const Token& SglExprLex::createToken(int type)
{
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
  return !isWhitespace() && !isAlphaNum;
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

/*------------------------   initialization/finalization   -----------------------*/

void SglExprLex::reset()
{
  index = 0;
  radix = 10;
  state = stTopLevel;
  pendingToken.clear();
  memlist.clear();
}

/*------------------------   lexical analyzing   -----------------------*/

bool SglExprLex::matchesEscape() const
{
  return escsize != 0 && expr.mid(start, escsize) == escapetag;
}

bool SglExprLex::matchesClosePar() const
{
  return !closePar.isEmpty() 
         && expr.mid(start, closePar.top.size()) == closePar.top();
}

int SglExprLex::getNextTokenType()
{
  if ( !pendingToken.isEmpty() )
    result = pendingToken.dequeue();
  else
    result = scanNextToken();
  updateState(result);
  return result;
}

int SglExprLex::scanNextToken()
{
  int result;

  start = index;
  end = size;
  if (index == end )
    result = eol;
  else if ( state == stText )
    result = scanTextToken();
  else if ( matchesClosePar() )
    result = getClosePar()
  else if ( matchesEscape() )
    result = scanSysToken();
  else if ( isWhitespace() )
    result = scanWhitespaceToken()
  else if ( isDigit() )
    result = scanDigitsToken()
  else if ( state == stNumber || state == stScale )
    result = scanMidNumberToken()
  else if ( isLetter() )
    result = scanIdentifierToken()
  else
    result = greedyLookup();
  return result;
}

int SglExprLex::getCloseChar()
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
  return TOKENUNKNOWN;
}

int SglExprLex::scanSysToken()
{
  int result;
  index += escapetag.size();
  scanLetters();
  result = escLookup();
  if ( !atEnd() && isWhitespace() )
    ++index;
  return result;
}

int SglExprLex::scanWhitespaceToken()
{
  while ( !atEnd() && isWhitespace() )
    ++index;
  end = index;
  return ' ';
}

int SglExprLex::scanDigitsToken()
{
  if ( state != stNumber )
  {
    result = scanTagToken();
    if ( result != TOKENUNKNOWN )
      return result;
  }
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
    default: return TOKENUNKNOWN;
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

void SglExprLex::updateState(int token)
{
  if ( state != stNumber ) radix = 10;
  switch (token)
  {
    case sot: state = stText; break;
    case eot: state = stTopLevel; break;
    case HEXTAG: radix += 8; // fall through
    case OCTTAG: redix += 6; // fall through
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
    default: if ( state != stScale ) state = stTopLevel;
  }
}

/*------------------------   table lookup   -----------------------*/

QString SglExprLex::currentSubStr() const
{
  return expr.mid(start, end - start);
}

int SglExprLex::lookup() const
{
  return doLookup(currentSubStr());
}

int SglExprLex::numLookup() const
{
  return doLookup('0' + currentSubStr());
}

int SglExprLex::escLookup() const
{
  return doLookup(' ' + currentSubStr());
}

int SglExprLex::greedyLookup()
{
  //set end
}

int SglExprLex::doLookup() const
{
}

/*------------------------   parser interface   -----------------------*/

