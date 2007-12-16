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

#include "math/hmath.hxx"
#include <QString>
#include <QVector>

/*==========================================================*/

/* to minimize effects on other modules make Evaluator
   keep the old Evaluator interface. This is not
   necessary for the parsing process, and should not be
   implemented here */

/*client(s):
  deletevardlg
*/
class Variable
{
  public:
    QString name;
    HNumber value;
};

/*client(s):
  editor
*/
class Token: public TokenBase
{
  public:
    // syntactical classification
    typedef enum
    {
      stxUnknown,
      stxNumber,
      stxIdentifier,
      stxOperator,
      stxOpenPar,
      stxClosePar,
      stxSep,
    } Type;
    Type type() const { return m_type; }
    bool isIdentifier() const { return m_type == stxIdentifier; }
  protected:

    Type m_type;
};

/*client(s):
  editor
*/
class Tokens: public QVector<Token>
{
  public:
    bool valid() const { return m_valid; }
  protected:
    bool m_valid;
};

class Evaluator : public EvaluatorBase
{
  public:
/*client(s):
    editor
*/
    static Tokens scan( const QString& expr );
/*client(s):
    deletevardlg
*/
    QVector<Variable> variables() const;
/*client(s):
    deletevardlg
*/
    void remove( const QString& id );
};

#endif /* _BISONPARSER_H */
