// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2008-2009 Helder Correia <helder.pereira.correia@gmail.com>
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

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVector>

#include <memory>

#ifdef _BISON
#include "bison/bisonparser.hxx"
#endif // _BISON

class Token
{
public:
    enum Op { InvalidOp = 0, Plus, Minus, Asterisk, Slash, Caret, LeftPar, RightPar, Semicolon,
              Percent, Exclamation, Equal, Modulo, Div };
    enum Type { stxUnknown, stxNumber, stxIdentifier, stxOperator, stxOpenPar, stxClosePar,
                stxSep };

    static const Token null;

    Token( Type type = stxUnknown, const QString & text = QString::null, int pos = -1 );
    Token( const Token & );

    HNumber asNumber() const;
    Op asOperator() const;
    QString description() const;
    bool isNumber() const { return m_type == stxNumber; }
    bool isOperator() const { return m_type >= stxOperator; }
    bool isIdentifier() const { return m_type == stxIdentifier; }
    int pos() const { return m_pos; };
    QString text() const { return m_text; }
    Type type() const { return m_type; }

    Token & operator=( const Token & );

protected:
    int m_pos;
    QString m_text;
    Type m_type;
};

class Tokens : public QVector<Token>
{
public:
    Tokens() : QVector<Token>(), m_valid( true ) {};

    bool valid() const { return m_valid; }
    void setValid( bool v ){ m_valid = v; }

protected:
    bool m_valid;
};

struct Variable
{
    QString name;
    HNumber value;
};

class Functions;

class Evaluator : public QObject
{
    Q_OBJECT

public:
    static Evaluator * instance();
    ~Evaluator();

    QString autoFix( const QString & );
    void clear();
    void clearVariables();
    QString dump() const;
    QString error() const;
    HNumber eval();
    HNumber evalNoAssign();
    HNumber evalUpdateAns();
    QString expression() const;
    HNumber get( const QString & id );
    bool has( const QString & id );
    bool isValid() const;
    char radixChar() const;
    void remove( const QString & id );
    Tokens scan( const QString & ) const;
    void set( const QString & id, HNumber value );
    void setExpression( const QString& expr );
    Tokens tokens() const;
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

