// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2008 Helder Correia <helder.pereira.correia@gmail.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#ifndef CORE_EVALUATOR_HXX
#define CORE_EVALUATOR_HXX

// #define _BISON

#include "math/hmath.hxx"

#include <QObject>
#include <QString>
#include <QVector>

#include <memory>

#ifdef _BISON
# include "bison/bisonparser.hxx"
#endif

class Token
{
  public:
    enum Op   { InvalidOp = 0, Plus, Minus, Asterisk, Slash, Caret, LeftPar,
                RightPar, Semicolon, Percent, Exclamation, Equal, Modulo, Div };
    enum Type { stxUnknown, stxNumber, stxIdentifier, stxOperator, stxOpenPar,
                stxClosePar, stxSep, };

    static const Token null;

    Token( Type type = stxUnknown, const QString & text = QString::null,
           int pos = -1 );
    Token( const Token & );
    Token & operator=( const Token & );
    Type    type() const { return m_type; }
    QString text() const { return m_text; }
    int     pos() const { return m_pos; };
    bool    isNumber() const { return m_type == stxNumber; }
    bool    isOperator() const { return m_type >= stxOperator; }
    bool    isIdentifier() const { return m_type == stxIdentifier; }
    HNumber asNumber() const;
    Op      asOperator() const;
    QString description() const;

  protected:
    Type m_type;
    QString m_text;
    int m_pos;
};

class Tokens : public QVector<Token>
{
  public:
    Tokens() : QVector<Token>(), m_valid(true) {};
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

class Functions;

class Evaluator : public QObject
{
  Q_OBJECT

  public:
    Evaluator( Functions *, QObject * parent = 0 );
    ~Evaluator();

    bool    has( const QString & id );
    bool    isValid() const;
    HNumber eval();
    HNumber evalNoAssign();
    HNumber evalUpdateAns();
    HNumber get( const QString & id );
    char    radixChar() const;
    QString autoFix( const QString & );
    QString dump() const;
    QString error() const;
    QString expression() const;
    Tokens  scan( const QString & ) const;
    Tokens  tokens() const;
    void    clear();
    void    clearVariables();
    void    remove( const QString & id );
    void    set( const QString & id, HNumber value );
    void    setExpression( const QString& expr );

    QVector<Variable> variables() const;

  protected:
    void compile( const Tokens & ) const;

  private:
    struct Private;
    const std::auto_ptr<Private> d;
    Evaluator();
    Evaluator( const Evaluator & );
    Evaluator & operator=( const Evaluator & );
};

#endif

