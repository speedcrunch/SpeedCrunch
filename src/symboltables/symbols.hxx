/* symbols.hxx: symbols of the parser/lexical analyzer */
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

#ifndef _SYMBOLS_H
#define _SYMBOLS_H

#include <QString>

class Symbol
{
  public:
    virtual int type();
    virtual ~Symbol();
};

typedef Symbol* PSymbol;

class SyntaxSymbol: public Symbol
{
  public:
    int type();
    SyntaxSymbol(int aType);
  private:
    int m_type;
};

class OpenSymbol: public SyntaxSymbol
{
  public:
    OpenSymbol(int aType, const QString& end);
    ~OpenSymbol();
    const QString& closeToken() const { return m_end; };
  private:
    QString m_end;
    SyntaxSymbol* closeSymbol;
};

class FunctionSymbol: public SyntaxSymbol
{
};

#endif /* _SYMBOLS_H */
