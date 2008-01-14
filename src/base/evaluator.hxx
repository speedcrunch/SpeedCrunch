/* This file is part of the SpeedCrunch project
   Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
                 2008 Helder Correia <helder.pereira.correia@gmail.com>

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

// #define _BISON

#include <QString>
#include <math/hmath.hxx>

#include <QVector>
#ifdef _BISON
# include "bison/bisonparser.hxx"
#endif

class Token
{
  public:
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


    Token( Type type = stxUnknown, const QString& text = QString::null, int pos = -1 );

    Token( const Token& );
    Token& operator=( const Token& );

    Type type() const { return m_type; }
    QString text() const { return m_text; }
    int pos() const { return m_pos; };
    bool isNumber() const { return m_type == stxNumber; }
    bool isOperator() const { return m_type >= stxOperator; }
    bool isIdentifier() const { return m_type == stxIdentifier; }
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

class Evaluator
{
  public:

    Evaluator();
    ~Evaluator();
    void setExpression( const QString& expr );
    QString expression() const;
    void clear();
    bool isValid() const;
    Tokens tokens() const;
    static Tokens scan( const QString& expr );
    QString error() const;

    HNumber evalNoAssign();
    HNumber eval();
    HNumber evalUpdateAns();

    void set( const QString& id, HNumber value );
    HNumber get( const QString& id );
    bool has( const QString& id );
    void remove( const QString& id );
    QVector<Variable> variables() const;
    void clearVariables();

    static QString autoFix( const QString& expr );

    QString dump() const;

  protected:

    void compile( const Tokens& tokens ) const;

  private:

    EvaluatorPrivate *d;
    Evaluator( const Evaluator& );
    Evaluator& operator=( const Evaluator& );
};

#endif // EVALUATOR

