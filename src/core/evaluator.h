// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2008, 2009, 2010, 2013 Helder Correia <helder.pereira.correia@gmail.com>
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

#ifndef CORE_EVALUATOR_H
#define CORE_EVALUATOR_H

#include "core/functions.h"
#include "math/hmath.h"

#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QSet>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVector>
#include <QtCore/QStack>

class Token {
public:
    enum Op { InvalidOp = 0, Plus, Minus, Asterisk, Slash, Backslash, Caret,
              Super0, Super1, Super2, Super3, Super4, Super5, Super6, Super7, Super8, Super9,
              LeftPar, RightPar, Semicolon, Percent, Exclamation, Equal, Modulo,
              LeftShift, RightShift, Ampersand, Pipe };
    enum Type { stxUnknown, stxNumber, stxIdentifier, stxOperator, stxOpenPar, stxClosePar, stxSep };

    static const Token null;

    Token(Type type = stxUnknown, const QString& text = QString::null, int pos = -1);
    Token(const Token&);

    HNumber asNumber() const;
    Op asOperator() const;
    QString description() const;
    bool isNumber() const { return m_type == stxNumber; }
    bool isOperator() const { return m_type >= stxOperator; }
    bool isIdentifier() const { return m_type == stxIdentifier; }
    int pos() const { return m_pos; }
    QString text() const { return m_text; }
    Type type() const { return m_type; }

    Token& operator=(const Token&);

protected:
    int m_pos;
    QString m_text;
    Type m_type;
};

class Tokens : public QVector<Token> {
public:
    Tokens() : QVector<Token>(), m_valid(true) { }

    bool valid() const { return m_valid; }
    void setValid(bool v) { m_valid = v; }

protected:
    bool m_valid;
};


class Evaluator : public QObject {
    Q_OBJECT

public:
    struct Variable {
        enum Type { BuiltIn, UserDefined };
        Variable() : name(""), value(HNumber(0)) { }
        Variable(const QString& n, HNumber v, Type t = UserDefined) : name(n), value(v), type(t) { }
        bool operator==(const Variable& other) const { return name == other.name; }
        QString name;
        HNumber value;
        Type type;
    };
    
    struct UserFunctionDescr {
        QString name;
        QStringList arguments;
        QString expression;

        UserFunctionDescr(QString name, QStringList arguments, QString expression)
            : name(name), arguments(arguments), expression(expression) {}
    };

    // Needed only for issue 160 workaround.
    enum AutoFixPolicy { AutoFix, NoAutoFix };

    static Evaluator* instance();
    void reset();

    QString autoFix(const QString&);
    QString dump();
    QString error() const;
    HNumber eval();
    HNumber evalNoAssign();
    HNumber evalUpdateAns();
    QString expression() const;
    bool isValid();
    Tokens scan(const QString&, AutoFixPolicy = AutoFix) const;
    void setExpression(const QString&);
    Tokens tokens() const;

    Variable getVariable(const QString&) const;
    QList<Variable> getVariables() const;
    QList<Variable> getUserDefinedVariables() const;
    QList<Variable> getUserDefinedVariablesPlusAns() const;
    void setVariable(const QString&, HNumber, Variable::Type = Variable::UserDefined);
    void unsetVariable(const QString&);
    void unsetAllUserDefinedVariables();
    bool isBuiltInVariable(const QString&) const;
    bool hasVariable(const QString&) const;
    
    //UserFunctionDescr getUserFunction(const QString&) const;
    QList<UserFunctionDescr> getUserFunctions() const;
    void setUserFunction(const UserFunctionDescr&);
    void unsetUserFunction(const QString&);
    void unsetAllUserFunctions();
    bool hasUserFunction(const QString&);

protected:
    void compile(const Tokens&);

private:
    Evaluator();
    Q_DISABLE_COPY(Evaluator)

    struct Opcode {
        enum { Nop = 0, Load, Ref, Function, Add, Sub, Neg, Mul, Div, Pow,
               Fact, Modulo, IntDiv, LSh, RSh, BAnd, BOr };

        unsigned type;
        unsigned index;

        Opcode() : type(Nop), index(0) {}
        Opcode(unsigned t) : type(t), index(0) {}
        Opcode(unsigned t, unsigned i): type(t), index(i) {}
    };
    
    struct UserFunction {
        UserFunctionDescr descr;

        QVector<HNumber> constants;
        QStringList identifiers;
        QVector<Opcode> opcodes;
        
        bool inUse;

        UserFunction(UserFunctionDescr& descr)
            : descr(descr), inUse(false) {}
        UserFunction(QString name, QStringList arguments, QString expression)
            : descr(name, arguments, expression), inUse(false) {}
    };

    bool m_dirty;
    QString m_error;
    QString m_expression;
    bool m_valid;
    QString m_assignId;
    bool m_assignFunc;
    QStringList m_assignArg;
    QVector<Opcode> m_codes;
    QVector<HNumber> m_constants;
    QStringList m_identifiers;
    QHash<QString, Variable> m_variables;
    QHash<QString, UserFunction*> m_userFunctions;

    const HNumber& checkOperatorResult(const HNumber&);
    static QString stringFromFunctionError(Function*);
    void initializeBuiltInVariables();
    HNumber exec(const QVector<Opcode>& opcodes, const QVector<HNumber>& constants,
                 const QStringList& identifiers);
    HNumber execUserFunction(UserFunction* function, QVector<HNumber>& arguments);
    UserFunction* getUserFunction(const QString&) const;
};

#endif
