/* This file is part of the SpeedCrunch project
   Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
 */

#ifndef EVALUATOR
#define EVALUATOR

#include <QString>

//#define _BISON

class EvaluatorBase //refEv
{
  public:
    static QString autoFix( const QString& expr );
  protected:
    EvaluatorBase();
    ~EvaluatorBase();
  private:
    EvaluatorBase( const EvaluatorBase& );
    EvaluatorBase& operator=( const EvaluatorBase& );
};

#ifdef _BISON

#include "bison/bisonparser.hxx"

#else /* ! _BISON */

#include <math/hmath.hxx>

#include <QVector>

class Token
{
  public:
    typedef enum
    {
      Unknown,
      Number,
      Operator,
      Identifier
    } Type;

    typedef enum
    {
      InvalidOp = 0,
      Plus,           //  + (addition)
      Minus,          //  - (substraction, negation)
      Asterisk,       //  * (multiplication)
      Slash,          //  / (division)
      Caret,          //  ^ (power)
      LeftPar,        //  (
      RightPar,       //  )
      Semicolon,      // argument separator
      Percent,        // %
      Exclamation,    // ! (factorial)
      Equal,          // variable assignment
      Modulo,         // integer rest division
      Div             // integer division
    } Op;

    Token( Type type = Unknown, const QString& text = QString::null, int pos = -1 );

    Token( const Token& );
    Token& operator=( const Token& );

    Type type() const { return m_type; }
    QString text() const { return m_text; }
    int pos() const { return m_pos; };
    bool isNumber() const { return m_type == Number; }
    bool isOperator() const { return m_type == Operator; }
    bool isIdentifier() const { return m_type == Identifier; }
    HNumber asNumber() const;
    Op asOperator() const;
    QString description() const;

    static const Token null;

  protected:

    Type m_type;
    QString m_text;
    int m_pos;

};


class Tokens: public QVector<Token>
{
  public:
    Tokens(): QVector<Token>(), m_valid(true) {};
    bool valid() const { return m_valid; }
    void setValid( bool v ){ m_valid = v; }
  protected:
    bool m_valid;
};

class Variable
{
  public:
    QString name;
    HNumber value;
};

class EvaluatorPrivate;

//class Evaluator //refEv
class Evaluator : public EvaluatorBase
{
  public:

//    typedef enum { Degree, Radian } AngleMode; //refan

    Evaluator();
    ~Evaluator();
    void setExpression( const QString& expr );
    QString expression() const;
    void clear();
    bool isValid() const;
    Tokens tokens() const;
//    static Tokens scan( const QString& expr, const QString& decimalPoint );//refdp
    static Tokens scan( const QString& expr );
    QString error() const;

//    void setAngleMode( AngleMode am ); //refan
//    AngleMode angleMode() const;
    HNumber eval();
    HNumber evalUpdateAns();

//     void setDecimalPoint( const QString& d ); //refdp
//     QString decimalPoint() const;

    void set( const QString& id, HNumber value );
    HNumber get( const QString& id );
    bool has( const QString& id );
    void remove( const QString& id );
    QVector<Variable> variables() const;
    void clearVariables();

//    static QString autoFix( const QString& expr, const QString& decimalPoint );//refdp

    QString dump() const;

  protected:

    void compile( const Tokens& tokens ) const;

  private:
    EvaluatorPrivate *d;
/*    EvaluatorBase( const EvaluatorBase& ); //refEv
    EvaluatorBase& operator=( const EvaluatorBase& );*/
};

#endif /* ! _BISON */

#endif // EVALUATOR

