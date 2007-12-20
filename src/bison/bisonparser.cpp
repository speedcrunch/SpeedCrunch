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

void QString::setExpression(const QString& newexpr)
{
  expr = newexpr;

  // replace all funny characters
  int sz = size();
  for (int i = -1; ++i < sz ;)
    if (at(i) < ' ')
      expr[i] = ' ';

  // replace all system tags
  if (!systag.isEmpty())
  {
    QString replacement;
    replacement.fill(syschar, systag.size());
    expr.replace(systag, replacement);
  }
  reset();
}

bool SglExprLex::autoFix();
{
  int parlevel = 0;
  int quotecnt = 0;
  int tokencnt = 0;
  int sz;
  int curpos;
  int assignseqpos = -1;
  int lasttype = 0;
  QChar c;

  reset();
  int type = whitespace;
  while (type != 0)
  {
    curpos = pos;
    type = getNextToken();
    switch (type)
    {
      case whitespace: continue;
      case OPENPAR: ++parlevel; continue;
      case CLOSEPAR: --parlevel; continue;
      case sot:
      case eot: quotecnt ^= 1; continue;
      case ASSIGN: if (assignpos < 0) assignpos = curpos; continue;
      case TOKENUNKNOWN: return false;
      default: assignpos = -1; break;
    }
    lasttype = type;
    ++tokencount;
  }
  // remove trailing '='
  if (assignpos >= 0)
    expr.truncate(assignpos);
  // add missing 'end of quotation' mark
  if ( quotecnt != 0 )
    expr.append(revLookup(eot));
  // supply 'ans' as parameter, if just a function name is given
  if (tokencnt == 1 && lasttoken == FUNCTION)
    expr.append(revLookup(ans));
  // close all unmatched left paranthesis
  for (int i = 0; ++i < parlevel ;)
    expr.append(revLookup(CLOSEPAR));
  return true;
}

void SglExprLex::reset()
{
  pos = 0;
  state = inWhitespace;
}

int SglExprLex::getNextToken()
{
  start = pos;
  if (pos >= expr.size())
    return 0;
  switch( state )
  {
    case inWhitespace: return getTopLevelToken();
  }
}

void SglExprLex::skip(QChar c)
{
  while (at(pos) == c)
    ++pos;
}

int SglExprLex::getTopLevelToken()
{
  QChar c = at(pos);
  if (c == systag)
    return getSysToken();
  if ((c >= 'a' && c <= 'z')
      || (c >= 'A' && c <= 'Z')
      || (c == '_'))
    return getIdentifier();
  if (c >= '0' && c <= '9')
    return getNumeric();
  return getSpecialCharToken();
}

int SglExprLex::getSysToken()
{
  int tagsz, p1;

  tagsz = systag.size()
  p1 = pos;
  skip(syschar);
  end = pos;
  if ( pos - p1 != tagsz )
    // two systags concatenated
    return TOKENUNKNOWN;
  end = expr.indexOf(systag, pos);
  if ( end < 0 )
    // no end tag
    return TOKENUNKNOWN;
  pos = end + tagsz;
  end = pos;
  // lookup with a single syschar ahead
  return lookup(expr.mid(p1 + tagsz - 1, end - p1 - tagsz));
}
