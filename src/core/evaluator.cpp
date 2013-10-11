// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2005, 2006 Johan Thelin <e8johan@gmail.com>
// Copyright (C) 2007, 2008, 2009, 2010, 2013 Helder Correia <helder.pereira.correia@gmail.com>
// Copyright (C) 2009 Wolf Lammen <ookami1@gmx.de>
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

#include "core/evaluator.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QStack>

//#define EVALUATOR_DEBUG
#ifdef EVALUATOR_DEBUG
#include <QtCore/QFile>
#include <QtCore/QTextStream>

QTextStream& operator<<(QTextStream& s, HNumber num)
{
    char* str = HMath::format(num, 'f');
    s << str;
    free(str);
    return s;
}
#endif // EVALUATOR_DEBUG

static Evaluator* s_evaluatorInstance = 0;

static void s_deleteEvaluator()
{
    delete s_evaluatorInstance;
}

const HNumber& Evaluator::checkOperatorResult(const HNumber& n)
{
    switch (n.error()) {
    case NoOperand:
        m_error = Evaluator::tr("cannot operate on a NaN");
        break;
    case Underflow:
        m_error = Evaluator::tr("underflow - tiny result is out of SpeedCrunch's number range");
        break;
    case Overflow:
        m_error = Evaluator::tr("overflow - huge result is out of SpeedCrunch's number range");
        break;
    case ZeroDivide:
        m_error = Evaluator::tr("division by zero");
        break;
    case OutOfLogicRange:
        m_error = Evaluator::tr("overflow - logic result exceeds maximum of 256 bits");
        break;
    case OutOfIntegerRange:
        m_error = Evaluator::tr("overflow - integer result exceeds maximum limit for integers");
        break;
    case TooExpensive:
        m_error = Evaluator::tr("too time consuming computation was rejected");
        break;
    default:;
    }
    return n;
}

QString Evaluator::stringFromFunctionError(Function* function)
{
    if (!function->error())
        return QString();

    QString result = QString::fromLatin1("<b>%1</b>: ");

    switch (function->error()) {
    case InvalidParamCount:
        result += Evaluator::tr("wrong number of arguments");
        break;
    case NoOperand:
        result += Evaluator::tr("does not take NaN as an argument");
        break;
    case Overflow:
    case Underflow:
    case OutOfLogicRange:
    case OutOfIntegerRange:
        result += Evaluator::tr("result out of range");
        break;
    case ZeroDivide:
    case EvalUnstable:
    case OutOfDomain:
        result += Evaluator::tr("undefined for argument domain");
        break;
    case TooExpensive:
        result += Evaluator::tr("computation too expensive");
        break;
    case BadLiteral:
    case InvalidPrecision:
    case InvalidParam:
        result += Evaluator::tr("internal error, please report a bug");
        break;
    default:
        break;
    };

    return result.arg(function->identifier());
}

class TokenStack : public QVector<Token>
{
public:
    TokenStack();

    bool isEmpty() const;
    unsigned itemCount() const;
    Token pop();
    void push(const Token& token);
    const Token& top();
    const Token& top(unsigned index);

private:
    void ensureSpace();
    int topIndex;
};

const Token Token::null;

// Helper function: return operator of given token text e.g. "*" yields Operator::Asterisk.
static Token::Op matchOperator(const QString& text)
{
    Token::Op result = Token::InvalidOp;

    if (text.length() == 1) {
        QChar p = text.at(0);
        switch(p.unicode()) {
        case '+': result = Token::Plus; break;
        case '-': result = Token::Minus; break;
        case '*': result = Token::Asterisk; break;
        case '/': result = Token::Slash; break;
        case '^': result = Token::Caret; break;
        case ';': result = Token::Semicolon; break;
        case '(': result = Token::LeftPar; break;
        case ')': result = Token::RightPar; break;
        case '%': result = Token::Percent; break;
        case '!': result = Token::Exclamation; break;
        case '=': result = Token::Equal; break;
        case '\\': result = Token::Backslash; break;
        case '&': result = Token::Ampersand; break;
        case '|': result = Token::Pipe; break;
        default: result = Token::InvalidOp;
        }
    } else if (text.length() == 2) {
        if (text == "**")
            result = Token::Caret;
        else if(text == "<<")
          result = Token::LeftShift;
        else if(text == ">>")
          result = Token::RightShift;
    }
#if 0
    else if (text.length() == 3) {
        if (text == "mod")
            result = Token::Modulo;
    }
#endif

   return result;
}

// Helper function: give operator precedence e.g. "+" is 1 while "*" is 3.
static int opPrecedence(Token::Op op)
{
    int prec;
    switch(op) {
    case Token::Exclamation: prec = 8; break;
    case Token::Percent: prec = 8; break;
    case Token::Caret: prec = 7; break;
    case Token::Asterisk: prec = 5; break;
    case Token::Slash: prec = 6; break;
    case Token::Modulo:
    case Token::Backslash: prec = 6; break;
    case Token::Plus:
    case Token::Minus: prec = 3; break;
    case Token::LeftShift:
    case Token::RightShift: prec = 2; break;
    case Token::Ampersand: prec = 1; break;
    case Token::Pipe: prec = 0; break;
    case Token::RightPar:
    case Token::Semicolon: prec = -1; break;
    case Token::LeftPar: prec = -2; break;
    default: prec = -2; break;
    }
    return prec;
}

Token::Token(Type type, const QString& text, int pos)
{
    m_type = type;
    m_text = text;
    m_pos = pos;
}

Token::Token(const Token& token)
{
    m_type = token.m_type;
    m_text = token.m_text;
    m_pos  = token.m_pos;
}

Token& Token::operator=(const Token& token)
{
    m_type = token.m_type;
    m_text = token.m_text;
    m_pos = token.m_pos;
    return*this;
}

HNumber Token::asNumber() const
{
    QString text = m_text;
    text.replace(",", "."); // Issue 151.
    return isNumber() ? HNumber((const char*)text.toLatin1()) : HNumber(0);
}

Token::Op Token::asOperator() const
{
    return isOperator() ? matchOperator(m_text) : InvalidOp;
}

QString Token::description() const
{
    QString desc;

    switch (m_type) {
    case stxNumber: desc = "Number"; break;
    case stxIdentifier: desc = "Identifier"; break;
    case stxOpenPar:
    case stxClosePar:
    case stxSep:
    case stxOperator: desc = "Operator"; break;
    default: desc = "Unknown"; break;
    }

    while (desc.length() < 10)
        desc.prepend(' ');
    desc.prepend("  ");
    desc.prepend(QString::number(m_pos));
    desc.append(" : ").append(m_text);

    return desc;
}

TokenStack::TokenStack() : QVector<Token>()
{
    topIndex = 0;
    ensureSpace();
}

bool TokenStack::isEmpty() const
{
    return topIndex == 0;
}

unsigned TokenStack::itemCount() const
{
    return topIndex;
}

void TokenStack::push(const Token& token)
{
    ensureSpace();
    (*this)[topIndex++] = token;
}

Token TokenStack::pop()
{
    return topIndex > 0 ? Token(at(--topIndex)) : Token();
}

const Token& TokenStack::top()
{
    return top(0);
}

const Token& TokenStack::top(unsigned index)
{
    return (topIndex > (int)index) ? at(topIndex - index - 1) : Token::null;
}

void TokenStack::ensureSpace()
{
    int length = size();
    while (topIndex >= length) {
        length += 10;
        resize(length);
    }
}

// Helper function: return true for valid identifier character.
static bool isIdentifier(QChar ch)
{
    return ch.unicode() == '_' || ch.unicode() == '$' || ch.isLetter();
}

Evaluator* Evaluator::instance()
{
    if (!s_evaluatorInstance) {
        s_evaluatorInstance = new Evaluator;
        qAddPostRoutine(s_deleteEvaluator);
    }
    return s_evaluatorInstance;
}

Evaluator::Evaluator()
{
    clear();
}

void Evaluator::setExpression(const QString& expr)
{
    m_expression = expr;
    m_dirty = true;
    m_valid = false;
    m_error = QString();
}

QString Evaluator::expression() const
{
    return m_expression;
}

// Returns the validity. Note: empty expression is always invalid.
bool Evaluator::isValid()
{
    if (m_dirty) {
        Tokens tokens = scan(m_expression);
        if (!tokens.valid())
            compile(tokens);
        else
            m_valid = false;
    }
    return m_valid;
}

void Evaluator::clear()
{
    m_expression = QString();
    m_dirty = true;
    m_valid = false;
    m_error = QString();
    m_constants.clear();
    m_codes.clear();
    m_assignId = QString();
    deleteVariables();
}

QString Evaluator::error() const
{
    return m_error;
}

// Returns list of token for the expression.
// This triggers again the lexical analysis step. It is however preferable
// (even when there's small performance penalty) because otherwise we need to
// store parsed tokens all the time which serves no good purpose.
Tokens Evaluator::tokens() const
{
    return scan(m_expression);
}

Tokens Evaluator::scan(const QString& expr) const
{
    // Result.
    Tokens tokens;

    // Parsing state.
    enum { Start, Finish, Bad, InNumber, InHexa, InOctal, InBinary, InDecimal, InExpIndicator,
           InExponent, InIdentifier } state;

    // Initialize variables.
    state = Start;
    int i = 0;
    QString ex = expr;
    QString tokenText;
    int tokenStart = 0;
    Token::Type type;

    // Force a terminator.
    ex.append(QChar());

    // Main loop.
    while (state != Bad && state != Finish && i < ex.length()) {
        QChar ch = ex.at(i);

        switch (state) {
        case Start:
            tokenStart = i;

            // Skip any whitespaces.
            if (ch.isSpace())
                ++i;
            else if (ch == '?') // Comment.
                state = Finish;
            else if (ch.isDigit()) // Check for number.
                state = InNumber;
            else if (ch == '#') { // Simple hexadecimal notation.
                tokenText.append("0x");
                state = InHexa;
                ++i;
            } else if (ch == '.' || ch == ',') { // Radix character?
                tokenText.append(ex.at(i++));
                state = InDecimal;
            } else if (ch.isNull()) // Terminator character.
                state = Finish;
            else { // Look for operator match.
                int op;
                QString s;
#if 0
                // Check for three-char operator.
                s.append(ch).append(ex.at(i+1)).append(ex.at(i+2));
                op = matchOperator(s);

                // Check for two-char operator.
                if (op == Token::InvalidOp)
#endif
                {
                    s = QString(ch).append(ex.at(i+1));
                    op = matchOperator(s);
                }

                // Check for one-char operator.
                if (op == Token::InvalidOp) {
                    s = QString(ch);
                    op = matchOperator(s);
                }

                // Any matched operator?
                if (op != Token::InvalidOp) {
                    switch(op) {
                        case Token::LeftPar: type = Token::stxOpenPar; break;
                        case Token::RightPar: type = Token::stxClosePar; break;
                        case Token::Semicolon: type = Token::stxSep; break;
                        default: type = Token::stxOperator;
                    }
                    int len = s.length();
                    i += len;
                    tokens.append(Token(type, s.left(len), tokenStart));
                }
                else
                    state = Bad;
            }

            // Beginning with unknown alphanumeric?  Could be identifier, or function.
            if (state == Bad && isIdentifier(ch))
                state = InIdentifier;
            break;

        case InIdentifier:
            // Consume as long as alpha, dollar sign, underscore, or digit.
            if (isIdentifier(ch) || ch.isDigit())
                tokenText.append(ex.at(i++));
            else { // We're done with identifier.
                tokens.append(Token(Token::stxIdentifier, tokenText, tokenStart));
                tokenStart = i;
                tokenText = "";
                state = Start;
            }
            break;

        case InNumber:
            if (ch.isDigit()) // Consume as long as it's a digit.
                tokenText.append(ex.at(i++));
            else if (ch == '.' || ch == ',') { // Convert to '.'.
                tokenText.append('.');
                ++i;
                state = InDecimal;
            }
            else if (ch.toUpper() == 'E') { // Exponent?
                tokenText.append('E');
                ++i;
                state = InExpIndicator;
            } else if (ch.toUpper() == 'X' && tokenText == "0") { // Normal hexadecimal notation.
                tokenText.append('x');
                ++i;
                state = InHexa;
            } else if (ch.toUpper() == 'B' && tokenText == "0") { // Binary notation.
                tokenText.append('b');
                ++i;
                state = InBinary;
            } else if (ch.toUpper() == 'O' && tokenText == "0") { // Octal notation.
                tokenText.append('o'); ++i; state = InOctal;
            } else if (ch.toUpper() == 'D' && tokenText == "0") { // Explicit decimal notation.
                tokenText = ""; // We also need to get rid of the leading zero.
                ++i;
            } else { // We're done with integer number.
                tokens.append(Token(Token::stxNumber, tokenText, tokenStart));
                tokenText = "";
                state = Start;
            }
            break;

        case InHexa:
            if (ch.isDigit() || (ch >= 'A' && ch < 'G') || (ch >= 'a' && ch < 'g'))
                tokenText.append(ex.at(i++).toUpper());
            else { // We're done with hexadecimal number.
                tokens.append(Token(Token::stxNumber, tokenText, tokenStart));
                tokenText = "";
                state = Start;
            }
            break;

        case InBinary:
            if (ch == '0' || ch == '1')
                tokenText.append(ex.at(i++));
            else { // We're done with binary number.
                tokens.append(Token(Token::stxNumber, tokenText, tokenStart));
                tokenText = "";
                state = Start;
            }
            break;

        case InOctal:
            if (ch >= '0' && ch < '8')
                tokenText.append(ex.at(i++));
            else { // We're done with octal number.
                tokens.append(Token(Token::stxNumber, tokenText, tokenStart));
                tokenText = ""; state = Start;
            }
            break;

        case InDecimal:
            if (ch.isDigit()) // Consume as long as it's a digit.
                tokenText.append(ex.at(i++));
            else if (ch.toUpper() == 'E') { // Exponent?
                tokenText.append('E');
                ++i;
                state = InExpIndicator;
            } else { // We're done with floating-point number.
                tokens.append(Token(Token::stxNumber, tokenText, tokenStart));
                tokenText = "";
                state = Start;
            };
            break;

        case InExpIndicator:
            if (ch == '+' || ch == '-') // Possible + or - right after E.
                tokenText.append(ex.at(i++));
            else if (ch.isDigit()) // Consume as long as it's a digit.
                state = InExponent;
            else // Invalid thing here.
                state = Bad;
            break;

        case InExponent:
            if (ch.isDigit()) // Consume as long as it's a digit.
                tokenText.append(ex.at(i++));
            else { // We're done with floating-point number.
                tokens.append(Token(Token::stxNumber, tokenText, tokenStart));
                tokenText = "";
                state = Start;
            };
            break;

        case Bad:
            tokens.setValid(false);
            break;

        default:
            break;
        };
    }

    if (state == Bad)
        // Invalidating here too, because usually when we set state to Bad, the case Bad won't be run.
        tokens.setValid(false);

    return tokens;
}

void Evaluator::compile(const Tokens& tokens)
{
#ifdef EVALUATOR_DEBUG
    QFile debugFile("eval.log");
    debugFile.open(QIODevice::WriteOnly);
    QTextStream dbg(&debugFile);
#endif

    // Initialize variables.
    m_dirty = false;
    m_valid = false;
    m_codes.clear();
    m_constants.clear();
    m_identifiers.clear();
    m_error = QString();

    // Sanity check.
    if (tokens.count() == 0)
        return;

    TokenStack syntaxStack;
    QStack<int> argStack;
    unsigned argCount = 1;

    for (int i = 0; i <= tokens.count(); ++i) {
        // Helper token: InvalidOp is end-of-expression.
        Token token = (i < tokens.count()) ? tokens.at(i) : Token(Token::stxOperator);
        Token::Type tokenType = token.type();
        if (tokenType >= Token::stxOperator)
            tokenType = Token::stxOperator;

#ifdef EVALUATOR_DEBUG
        dbg << "\n";
        dbg << "Token: " << token.description() << "\n";
#endif

        // Unknown token is invalid.
        if (tokenType == Token::stxUnknown)
            break;

        // For constants, push immediately to stack. Generate code to load from a constant.
        if (tokenType == Token::stxNumber) {
            syntaxStack.push(token);
            m_constants.append(token.asNumber());
            m_codes.append(Opcode(Opcode::Load, m_constants.count() - 1));
#ifdef EVALUATOR_DEBUG
            dbg << "  Push " << token.asNumber() << " to constant pools" << "\n";
#endif
        }

        // For identifier, push immediately to stack. Generate code to load from reference.
        if (tokenType == Token::stxIdentifier) {
            syntaxStack.push(token);
            m_identifiers.append(token.text());
            m_codes.append(Opcode(Opcode::Ref, m_identifiers.count() - 1));
#ifdef EVALUATOR_DEBUG
            dbg << "  Push " << token.text() << " to identifier pools" << "\n";
#endif
        }

        // Special case for percentage.
        if (tokenType == Token::stxOperator && token.asOperator() == Token::Percent
             && syntaxStack.itemCount() >= 1 && !syntaxStack.top().isOperator())
        {
            m_constants.append(HNumber("0.01"));
            m_codes.append(Opcode(Opcode::Load, m_constants.count() - 1));
            m_codes.append(Opcode(Opcode::Mul));
#ifdef EVALUATOR_DEBUG
            dbg << "  Handling percentage" << "\n";
#endif
        }

        // For any other operator, try to apply all parsing rules.
        if (tokenType == Token::stxOperator && token.asOperator() != Token::Percent) {
#ifdef EVALUATOR_DEBUG
            dbg << "  Checking rules..." << "\n";
#endif
            // Repeat until no more rule applies.
            bool argHandled = false;
            while (true) {
                bool ruleFound = false;

                // Rule for function last argument: id (arg) -> arg.
                if (!ruleFound && syntaxStack.itemCount() >= 4) {
                    Token par2 = syntaxStack.top();
                    Token arg = syntaxStack.top(1);
                    Token par1 = syntaxStack.top(2);
                    Token id = syntaxStack.top(3);
                    if (par2.asOperator() == Token::RightPar && !arg.isOperator()
                         && par1.asOperator() == Token::LeftPar && id.isIdentifier())
                    {
                        ruleFound = true;
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.push(arg);
                        m_codes.append(Opcode(Opcode::Function, argCount));
#ifdef EVALUATOR_DEBUG
                        dbg << "    Rule for function last argument " << argCount << " \n";
#endif
                        argCount = argStack.empty() ? 0 : argStack.pop();
                    }
                }

                // Are we entering a function? If token is operator, and stack already has: id (arg
                if (!ruleFound && !argHandled && tokenType == Token::stxOperator
                     && syntaxStack.itemCount() >= 3)
                {
                    Token arg = syntaxStack.top();
                    Token par = syntaxStack.top(1);
                    Token id = syntaxStack.top(2);
                    if (!arg.isOperator() && par.asOperator() == Token::LeftPar
                         && id.isIdentifier())
                    {
                        ruleFound = true;
                        argStack.push(argCount);
                        argCount = 1;
#ifdef EVALUATOR_DEBUG
                        dbg << "  Entering function " << argCount << " \n";
#endif
                        break;
                    }
                }

                // Rule for postfix operators.
                if (!ruleFound && syntaxStack.itemCount() >= 2) {
                    Token postfix = syntaxStack.top();
                    Token y = syntaxStack.top(1);
                    if (postfix.isOperator() && !y.isOperator())
                        switch (postfix.asOperator()) {
                            case Token::Exclamation:
                                ruleFound = true;
                                syntaxStack.pop();
                                syntaxStack.pop();
                                syntaxStack.push(y);
                                m_codes.append(Opcode(Opcode::Fact));
                                break;
                            default:;
#ifdef EVALUATOR_DEBUG
                                dbg << "    Rule for postfix operator" << "\n";
#endif
                        }
                }

                // Rule for parenthesis: (Y) -> Y.
                if (!ruleFound && syntaxStack.itemCount() >= 3) {
                    Token right = syntaxStack.top();
                    Token y = syntaxStack.top(1);
                    Token left = syntaxStack.top(2);
                    if (right.isOperator() && !y.isOperator() && left.isOperator()
                         && right.asOperator() == Token::RightPar
                         && left.asOperator() == Token::LeftPar)
                    {
                        ruleFound = true;
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.push(y);
#ifdef EVALUATOR_DEBUG
                        dbg << "    Rule for (Y) -> Y" << "\n";
#endif
                    }
                }

                // Rule for simplified syntax for function e.g. "sin pi" or "cos 1.2"
                // i.e no need for parentheses like "sin(pi)" or "cos(1.2)".
                if (!ruleFound && syntaxStack.itemCount() >= 2) {
                    Token arg = syntaxStack.top();
                    Token id = syntaxStack.top(1);
                    if (!arg.isOperator() && id.isIdentifier()
                         && FunctionRepo::instance()->find(id.text()))
                    {
                        ruleFound = true;
                        m_codes.append(Opcode(Opcode::Function, 1));
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.push(arg);
#ifdef EVALUATOR_DEBUG
                        dbg << "    Rule for simplified function syntax" << "\n";
#endif
                    }
                }

                // R.ule for unary operator in simplified function syntax. This handles case like "sin -90".
                if (!ruleFound && syntaxStack.itemCount() >= 3) {
                    Token x = syntaxStack.top();
                    Token op = syntaxStack.top(1);
                    Token id = syntaxStack.top(2);
                    if (!x.isOperator() && op.isOperator() && id.isIdentifier()
                         && FunctionRepo::instance()->find(id.text())
                         && (op.asOperator() == Token::Plus || op.asOperator() == Token::Minus))
                    {
                        ruleFound = true;
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.push(x);
                        if (op.asOperator() == Token::Minus)
                            m_codes.append(Opcode(Opcode::Neg));
#ifdef EVALUATOR_DEBUG
                        dbg << "    Rule for unary operator in simplified function syntax" << "\n";
#endif
                    }
                }

#if 0
                // Rule for unary postfix operator in simplified function syntax. This handles case like "sin 90!"
                if (!ruleFound && syntaxStack.itemCount() >= 3) {
                    Token op = syntaxStack.top();
                    Token x = syntaxStack.top(1);
                    Token id = syntaxStack.top(2);
                    if (id.isIdentifier() && m_functions->find(id.text())) {
                        if (!x.isOperator() && op.isOperator() &&
                             op.asOperator() == Token::Exclamation)
                        {
                            ruleFound = true;
                            syntaxStack.pop();
                            syntaxStack.pop();
                            syntaxStack.push(x);
                            m_codes.append(Opcode(Opcode::Fact));
#ifdef EVALUATOR_DEBUG
                            dbg << "    Rule for unary operator in simplified function syntax"
                                << "\n";
#endif
                        }
                    }
                }
#endif

                // Rule for function arguments, if token is , or ): id (arg1 ; arg2 -> id (arg.
                if (!ruleFound && syntaxStack.itemCount() >= 5
                     && (token.asOperator() == Token::RightPar
                         || token.asOperator() == Token::Semicolon))
                {
                    Token arg2 = syntaxStack.top();
                    Token sep = syntaxStack.top(1);
                    Token arg1 = syntaxStack.top(2);
                    Token par = syntaxStack.top(3);
                    Token id = syntaxStack.top(4);
                    if (!arg2.isOperator() && sep.asOperator() == Token::Semicolon
                         && !arg1.isOperator()
                         && par.asOperator() == Token::LeftPar && id.isIdentifier())
                    {
                        ruleFound = true;
                        argHandled = true;
                        syntaxStack.pop();
                        syntaxStack.pop();
                        ++argCount;
#ifdef EVALUATOR_DEBUG
                        dbg << "    Rule for function argument " << argCount << " \n";
#endif
                    }
                }

                // Rule for function call with parentheses, but without argument e.g. "2*PI()".
                if (!ruleFound && syntaxStack.itemCount() >= 3) {
                    Token par2 = syntaxStack.top();
                    Token par1 = syntaxStack.top(1);
                    Token id = syntaxStack.top(2);
                    if (par2.asOperator() == Token::RightPar
                         && par1.asOperator() == Token::LeftPar && id.isIdentifier())
                    {
                        ruleFound = true;
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.push(Token(Token::stxNumber));
                        m_codes.append(Opcode(Opcode::Function, 0));
#ifdef EVALUATOR_DEBUG
                        dbg << "    Rule for function call with parentheses, but without argument"
                            << "\n";
#endif
                    }
                }

                // Rule for binary operator:  A (op) B -> A.
                // Conditions: precedence of op >= precedence of token.
                // Action: push (op) to result e.g. "A * B" becomes "A" if token is operator "+".
                // Exception: for caret (power operator), if op is another caret
                // then the rule doesn't apply, e.g. "2^3^2" is evaluated as "2^(3^2)".
                if (!ruleFound && syntaxStack.itemCount() >= 3) {
                    Token b = syntaxStack.top();
                    Token op = syntaxStack.top(1);
                    Token a = syntaxStack.top(2);
                    if (!a.isOperator() && !b.isOperator() && op.isOperator()
                         && token.asOperator() != Token::LeftPar
                         && token.asOperator() != Token::Caret
                         && opPrecedence(op.asOperator()) >= opPrecedence(token.asOperator()))
                    {
                        ruleFound = true;
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.push(b);
                        switch (op.asOperator()) {
                            // Simple binary operations.
                            case Token::Plus:      m_codes.append(Opcode::Add); break;
                            case Token::Minus:     m_codes.append(Opcode::Sub); break;
                            case Token::Asterisk:  m_codes.append(Opcode::Mul); break;
                            case Token::Slash:     m_codes.append(Opcode::Div); break;
                            case Token::Caret:     m_codes.append(Opcode::Pow); break;
                            case Token::Modulo:    m_codes.append(Opcode::Modulo); break;
                            case Token::Backslash: m_codes.append(Opcode::IntDiv); break;
                            case Token::LeftShift: m_codes.append(Opcode::LSh); break;
                            case Token::RightShift: m_codes.append(Opcode::RSh); break;
                            case Token::Ampersand: m_codes.append(Opcode::BAnd); break;
                            case Token::Pipe:      m_codes.append(Opcode::BOr); break;
                            default: break;
                        };
#ifdef EVALUATOR_DEBUG
                        dbg << "    Rule for binary operator" << "\n";
#endif
                    }
                }

                // Rule for unary operator:  (op1) (op2) X -> (op1) X.
                // Conditions: op2 is unary, token is not '('.
                // Action: push (op2) to result e.g. "* - 2" becomes "*".
                if (!ruleFound && token.asOperator() != Token::LeftPar
                        && syntaxStack.itemCount() >= 3)
                {
                    Token x = syntaxStack.top();
                    Token op2 = syntaxStack.top(1);
                    Token op1 = syntaxStack.top(2);
                    if (!x.isOperator() && op1.isOperator() && op2.isOperator()
                         && (op2.asOperator() == Token::Plus || op2.asOperator() == Token::Minus))
                    {
                        ruleFound = true;
                        if (op2.asOperator() == Token::Minus)
                            m_codes.append(Opcode(Opcode::Neg));
                    }
#if 0
                    else { // Maybe postfix.
                        x = op2;
                        op2 = syntaxStack.top();
                        if (!x.isOperator() && op1.isOperator() && op2.isOperator()
                             && op2.asOperator() == Token::Exclamation)
                        {
                            ruleFound = true;
                            m_codes.append(Opcode(Opcode::Fact));
                        }
                    }
#endif
                    if (ruleFound) {
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.push(x);
#ifdef EVALUATOR_DEBUG
                        dbg << "    Rule for unary operator" << "\n";
#endif
                    }
                }

                // auxiliary rule for unary operator:  (op) X -> X
                // conditions: op is unary, op is first in syntax stack,
                // token is not '(' or '^' or '!' or any power operator
                // action: push (op) to result
                if (!ruleFound && token.asOperator() != Token::LeftPar
                     && token.asOperator() != Token::Exclamation
                     && syntaxStack.itemCount() == 2)
                {
                    Token x = syntaxStack.top();
                    Token op = syntaxStack.top(1);
                    if (!x.isOperator() && op.isOperator()
                         && (op.asOperator() == Token::Plus || op.asOperator() == Token::Minus))
                    {
                        ruleFound = true;
                        if (op.asOperator() == Token::Minus)
                            m_codes.append(Opcode(Opcode::Neg));
                    } else {
                        x = op;
                        op = syntaxStack.top();
                        if (!x.isOperator() && op.isOperator())
                        {
                            // If we don't really find the rule, restore ruleFound as if nothing happened, see below.
                            bool ruleFoundOldValue = ruleFound;
                            ruleFound = true;
                            if (op.asOperator() == Token::Exclamation)
                                m_codes.append(Opcode(Opcode::Fact));
                            else ruleFound = ruleFoundOldValue;
                        }
                    }
                    if (ruleFound) {
                        syntaxStack.pop();
                        syntaxStack.pop();
                        syntaxStack.push(x);
#ifdef EVALUATOR_DEBUG
                        dbg << "    Rule for unary operator (auxiliary)" << "\n";
#endif
                    }
                }

                if (!ruleFound)
                    break;
            }

            // Can't apply rules anymore, push the token.
            if (token.asOperator() != Token::Percent)
                syntaxStack.push(token);
        }
    }

    // syntaxStack must left only one operand and end-of-expression (i.e. InvalidOp).
    m_valid = false;
    if (syntaxStack.itemCount() == 2 && syntaxStack.top().isOperator()
         && syntaxStack.top().asOperator() == Token::InvalidOp
         && !syntaxStack.top(1).isOperator())
    {
        m_valid = true;
    }

#ifdef EVALUATOR_DEBUG
    dbg << "Dump: " << dump() << "\n";
    debugFile.close();
#endif

    // Bad parsing ? clean-up everything.
    if (!m_valid) {
        m_constants.clear();
        m_codes.clear();
        m_identifiers.clear();
    }
}

HNumber Evaluator::evalNoAssign()
{
    QStack<HNumber> stack;
    QStack<QString> refs;
    int index;
    HNumber val1, val2;
    QVector<HNumber> args;
    QString fname;
    Function* function;

    if (m_dirty) {
        Tokens tokens = scan(m_expression);

        // Invalid expression?
        if (!tokens.valid()) {
            m_error = tr("invalid expression");
            return HNumber(0);
        }

        // Variable assignment?
        m_assignId = QString();
        if (tokens.count() > 2 && tokens.at(0).isIdentifier()
             && tokens.at(1).asOperator() == Token::Equal)
        {
            m_assignId = tokens.at(0).text();
            tokens.erase(tokens.begin());
            tokens.erase(tokens.begin());
        }

        compile(tokens);
    }

    for (int pc = 0; pc < m_codes.count(); ++pc) {
        const Opcode& opcode = m_codes.at(pc);
        index = opcode.index;
        switch (opcode.type) {
            // No operation.
            case Opcode::Nop:
                break;

            // Load a constant, push to stack.
            case Opcode::Load:
                val1 = m_constants.at(index);
                stack.push(val1);
                break;

            // Unary operation.
            case Opcode::Neg:
                if (stack.count() < 1) {
                    m_error = tr("invalid expression");
                    return HMath::nan();
                }
                val1 = stack.pop();
                val1 = checkOperatorResult(-val1);
                stack.push(val1);
                break;

            // Binary operation: take two values from stack, do the operation, push the result to stack.
            case Opcode::Add:
                if (stack.count() < 2) {
                    m_error = tr("invalid expression");
                    return HMath::nan();
                }
                val1 = stack.pop();
                val2 = stack.pop();
                val2 = checkOperatorResult(val2 + val1);
                stack.push(val2);
                break;

            case Opcode::Sub:
                if (stack.count() < 2) {
                    m_error = tr("invalid expression");
                    return HMath::nan();
                }
                val1 = stack.pop();
                val2 = stack.pop();
                val2 = checkOperatorResult(val2 - val1);
                stack.push(val2);
                break;

            case Opcode::Mul:
                if (stack.count() < 2) {
                    m_error = tr("invalid expression");
                    return HMath::nan();
                }
                val1 = stack.pop();
                val2 = stack.pop();
                val2 = checkOperatorResult(val2 * val1);
                stack.push(val2);
                break;

            case Opcode::Div:
                if (stack.count() < 2) {
                    m_error = tr("invalid expression");
                    return HMath::nan();
                }
                val1 = stack.pop();
                val2 = stack.pop();
                val2 = checkOperatorResult(val2 / val1);
                stack.push(val2);
                break;

            case Opcode::Pow:
                if (stack.count() < 2) {
                    m_error = tr("invalid expression");
                    return HMath::nan();
                }
                val1 = stack.pop();
                val2 = stack.pop();
                val2 = checkOperatorResult(HMath::raise(val2, val1));
                stack.push(val2);
                break;

            case Opcode::Fact:
                if (stack.count() < 1) {
                    m_error = tr("invalid expression");
                    return HMath::nan();
                }
                val1 = stack.pop();
                val1 = checkOperatorResult(HMath::factorial(val1));
                stack.push(val1);
                break;

            case Opcode::Modulo:
                if (stack.count() < 2) {
                    m_error = tr("invalid expression");
                    return HMath::nan();
                }
                val1 = stack.pop();
                val2 = stack.pop();
                val2 = checkOperatorResult(val2 % val1);
                stack.push(val2);
                break;

            case Opcode::IntDiv:
                if (stack.count() < 2) {
                    m_error = tr("invalid expression");
                    return HMath::nan();
                }
                val1 = stack.pop();
                val2 = stack.pop();
                val2 = checkOperatorResult(val2 / val1);
                stack.push(HMath::integer(val2));
                break;

            case Opcode::LSh:
                if (stack.count() < 2) {
                    m_error = tr("invalid expression");
                    return HMath::nan();
                }
                val1 = stack.pop();
                val2 = stack.pop();
                val2 = val2 << val1;
                stack.push(val2);
                break;

            case Opcode::RSh:
                if (stack.count() < 2) {
                    m_error = tr("invalid expression");
                    return HMath::nan();
                }
                val1 = stack.pop();
                val2 = stack.pop();
                val2 = val2 >> val1;
                stack.push(val2);
                break;

            case Opcode::BAnd:
                if (stack.count() < 2) {
                    m_error = tr("invalid expression");
                    return HMath::nan();
                }
                val1 = stack.pop();
                val2 = stack.pop();
                val2 &= val1;
                stack.push(val2);
                break;

            case Opcode::BOr:
                if (stack.count() < 2) {
                    m_error = tr("invalid expression");
                    return HMath::nan();
                }
                val1 = stack.pop();
                val2 = stack.pop();
                val2 |= val1;
                stack.push(val2);
                break;

            // Reference.
            case Opcode::Ref:
                fname = m_identifiers.at(index);
                if (has(fname)) // Variable.
                    stack.push(get(fname));
                else { // Function.
                    function = FunctionRepo::instance()->find(fname);
                    if (function)
                        refs.push(fname);
                    else {
                        m_error = fname + ": " + tr("unknown function or variable");
                        return HMath::nan();
                    }
                }
                break;

            // Calling function.
            case Opcode::Function:
                // Must do this first to avoid crash when using vars like functions.
                if (refs.isEmpty())
                    break;

                fname = refs.pop();
                function = FunctionRepo::instance()->find(fname);
                if (!function) {
                    m_error = fname + ": " + tr("unknown function or variable");
                    return HMath::nan();
                }

                if (stack.count() < index) {
                    m_error = tr("invalid expression");
                    return HMath::nan();
                }

                args.clear();
                for(; index; --index)
                    args.insert(args.begin(), stack.pop());

                if (!args.count()) {
                    m_error = QString::fromLatin1("%1(%2)").arg(fname).arg(function->usage());
                    return HMath::nan();
                }

                stack.push(function->exec(args));
                if (function->error()) {
                    m_error = stringFromFunctionError(function);
                    return HMath::nan();
                }
                break;

            default:
                break;
        }
    }

    // More than one value in stack? Unsuccesfull execution...
    if (stack.count() != 1) {
        m_error = tr("invalid expression");
        return HMath::nan();
    }

    HNumber result = stack.pop();

    return result;
}

HNumber Evaluator::eval()
{
    HNumber result = evalNoAssign(); // This sets m_assignId.

    // Cannot overwrite built-in variables and functions.
    if (m_assignId == QLatin1String("pi")
         || m_assignId == QLatin1String("phi")
         || m_assignId == QLatin1String("e")
         || m_assignId == QLatin1String("ans")
         || FunctionRepo::instance()->find(m_assignId))
    {
        m_error = tr("%1 is a reserved name, please choose another").arg(m_assignId);
        return HMath::nan();
    }

    // Handle variable assignment, e.g. "x=2*4".
    if (!m_assignId.isEmpty())
        set(m_assignId, result);

    return result;
}

HNumber Evaluator::evalUpdateAns()
{
    HNumber result = eval();

    // "ans" is default variable to hold calculation result.
    if (m_error.isEmpty())
        set(QString("ans"), result);

    return result;
}

void Evaluator::set(const QString& id, HNumber value)
{
    if (!id.isEmpty()) {
        m_variables[id].name  = id;
        m_variables[id].value = value;
    }
}

HNumber Evaluator::get(const QString& id)
{
    if (id.isEmpty())
        return HNumber(0);

    if (!m_variables.contains(id))
        set(id, HNumber(0));
    return m_variables.value(id).value;
}

bool Evaluator::has(const QString& id) const
{
    return id.isEmpty() ? false : m_variables.contains(id);
}

void Evaluator::deleteVariable(const QString& id)
{
    m_variables.remove(id);
}

QVector<Variable> Evaluator::variables() const
{
    QVector<Variable> result;

    for (QMap<QString,Variable>::ConstIterator it = m_variables.begin(); it != m_variables.end(); ++it) {
        Variable var = { it.value().name, it.value().value };
        result.append(var);
    }

    return result;
}

void Evaluator::deleteVariables()
{
  HNumber ansBackup = get(QString("ans"));

  m_variables.clear();

  set(QString("ans"), ansBackup);
  set(QString("pi"), HMath::pi());
  set(QString("phi"), HMath::phi());
  set(QString("e"), HMath::e());
}

static void replaceSuperscriptPowersWithCaretEquivalent(QString& expr)
{
    expr.replace(QString::fromUtf8("⁰"), QLatin1String("^0"));
    expr.replace(QString::fromUtf8("¹"), QLatin1String("^1"));
    expr.replace(QString::fromUtf8("²"), QLatin1String("^2"));
    expr.replace(QString::fromUtf8("³"), QLatin1String("^3"));
    expr.replace(QString::fromUtf8("⁴"), QLatin1String("^4"));
    expr.replace(QString::fromUtf8("⁵"), QLatin1String("^5"));
    expr.replace(QString::fromUtf8("⁶"), QLatin1String("^6"));
    expr.replace(QString::fromUtf8("⁷"), QLatin1String("^7"));
    expr.replace(QString::fromUtf8("⁸"), QLatin1String("^8"));
    expr.replace(QString::fromUtf8("⁹"), QLatin1String("^9"));
}

QString Evaluator::autoFix(const QString& expr)
{
    int par = 0;
    QString result;

    // Strip off all funny characters.
    for(int c = 0; c < expr.length(); ++c)
        if (expr.at(c) >= QChar(32))
            result.append(expr.at(c));

    // No extra whitespaces at the beginning and at the end.
    result = result.trimmed();

    // Strip trailing equal sign (=).
    while(result.endsWith("="))
        result = result.left(result.length() - 1);

    replaceSuperscriptPowersWithCaretEquivalent(result);

    // Automagically close all parenthesis.
    Tokens tokens = Evaluator::scan(result);
    if (tokens.count()) {
        for(int i = 0; i < tokens.count(); ++i)
            if (tokens.at(i).asOperator() == Token::LeftPar)
                ++par;
            else if (tokens.at(i).asOperator() == Token::RightPar)
                --par;

        if (par < 0)
            par = 0;

        // If the scanner stops in the middle, do not bother to apply fix.
        const Token& lastToken = tokens.at(tokens.count() - 1);
        if (lastToken.pos() + lastToken.text().length() >= result.length())
            while (par--)
                result.append(')');
    }

    // Special treatment for simple function e.g. "cos" is regarded as "cos(ans)".
    if (!result.isEmpty()) {
        Tokens tokens = Evaluator::scan(result);

        if (tokens.count() == 1 && tokens.at(0).isIdentifier()
             && FunctionRepo::instance()->find(tokens.at(0).text()))
        {
            result.append("(ans)");
        }
    }

    return result;
}

QString Evaluator::dump()
{
    QString result;
    int c;

    if (m_dirty) {
        Tokens tokens = scan(m_expression);
        compile(tokens);
    }

    result = QString("Expression: [%1]\n").arg(m_expression);

    result.append("  Constants:\n");
    for (c = 0; c < m_constants.count(); ++c) {
        QString vtext;
        HNumber val = m_constants.at(c);
        char* ss = HMath::format(val, 'f');
        result += QString("    #%1 = %2\n").arg(c).arg(ss);
        free(ss);
    }

    result.append("\n");
    result.append("  Identifiers:\n");
    for(c = 0; c < m_identifiers.count(); ++c) {
        QString vtext;
        result += QString("    #%1 = %2\n").arg(c).arg(m_identifiers.at(c));
    }

    result.append("\n");
    result.append("  Code:\n");
    for(int i = 0; i < m_codes.count(); ++i) {
        QString ctext;
        switch (m_codes.at(i).type) {
            case Opcode::Load: ctext = QString("Load #%1").arg(m_codes.at(i).index); break;
            case Opcode::Ref: ctext = QString("Ref #%1").arg(m_codes.at(i).index); break;
            case Opcode::Function: ctext = QString("Function (%1)").arg(m_codes.at(i).index);
                                   break;
            case Opcode::Add: ctext = "Add"; break;
            case Opcode::Sub: ctext = "Sub"; break;
            case Opcode::Mul: ctext = "Mul"; break;
            case Opcode::Div: ctext = "Div"; break;
            case Opcode::Neg: ctext = "Neg"; break;
            case Opcode::Pow: ctext = "Pow"; break;
            case Opcode::Fact: ctext = "Fact"; break;
            case Opcode::LSh: ctext = "LSh"; break;
            case Opcode::RSh: ctext = "RSh"; break;
            case Opcode::BAnd: ctext = "BAnd"; break;
            case Opcode::BOr: ctext = "BOr"; break;
            default: ctext = "Unknown"; break;
        }
        result.append("   ").append(ctext).append("\n");
    }

    return result;
}
