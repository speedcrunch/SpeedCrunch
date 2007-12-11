/* parser.h: lexical analyzers and parsers */
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

#include "math/hmath.hxx"
#include "symboltables/vars.h"
#include <QString>
#include <QVector>

class Token
{
  public:
    // syntactical classification
    typedef enum
    {
      stxUnknown,
      stxNumber,
      stxIdentifier,
      stxOperator,
      stxOpenPar,  //refty
      stxClosePar,
      stxSep,
    } Type;


    QString text() const { return m_text; }
    Type type() const { return m_type; }
    int pos() const { return m_pos; };
    bool isIdentifier() const { return m_type == stxIdentifier; }

  private:
    QString m_text;
    Type m_type;
    int m_pos;

};

class Tokens: public QVector<Token>
{
  public:
    bool valid() const { return m_valid; }
  private:
    bool m_valid;
};

class Evaluator : public EvaluatorBase
{
  public:
    QString error() const;

    static Tokens scan( const QString& expr );
    static QString autoFix( const QString& expr );

    void setExpression( const QString& expr );
    HNumber eval();
    HNumber evalUpdateAns();

};

#endif /* _BISONPARSER_H */
