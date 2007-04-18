/* This file is part of the SpeedCrunch project
   Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
                 2005-2006 Johan Thelin <e8johan@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "evaluator.h"
#include "functions.h"

#include <float.h>
#include <limits.h>
#include <math.h>

#include <QApplication>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QStack>

// #define EVALUATOR_DEBUG

#ifdef EVALUATOR_DEBUG
#include <QFile>
#include <QTextStream>

QTextStream& operator<<( QTextStream& s, HNumber num )
{
  char* str = HMath::formatFixed( num );
  s << str;
  delete[] str;
  return s;
}

#endif

class Opcode
{
public:

  enum { Nop = 0, Load, Ref, Function, Add, Sub, Neg, Mul, Div, Pow, Fact, Modulo, IntDiv };

  unsigned type;
  unsigned index;

  Opcode(): type(Nop), index(0) {};
  Opcode( unsigned t ): type(t), index(0) {};
  Opcode( unsigned t, unsigned i ): type(t), index(i) {};
};

class EvaluatorPrivate
{
public:
  Evaluator *evaluator;
  bool dirty;
  bool valid;
  QString expression;

  QString error;
  QMap<QString,Variable> variables;
  Evaluator::AngleMode angleMode;

  QString assignId;
  QVector<Opcode> codes;
  QStringList identifiers;
  QVector<HNumber> constants;
};

class TokenStack : public QVector<Token>
{
public:
  TokenStack();
  bool isEmpty() const;
  unsigned itemCount() const;
  void push( const Token& token );
  Token pop();
  const Token& top();
  const Token& top( unsigned index );
private:
  void ensureSpace();
  int topIndex;
};

// for null token
const Token Token::null;

// helper function: return operator of given token text
// e.g. "*" yields Operator::Asterisk, and so on
static Token::Op matchOperator( const QString& text )
{
  Token::Op result = Token::InvalidOp;

  if( text.length() == 1 )
  {
    QChar p = text[0];
    switch( p.unicode() )
    {
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
        default : result = Token::InvalidOp; break;
    }
  }

  else if( text.length() == 2 )
  {
    if( text == "**" ) result = Token::Caret;
  }
#if 0
   else if( text.length() == 3 )
   {
      if (text == "mod") result = Token::Modulo;
      else if (text == "div") result = Token::Div;
   }
#endif

  return result;
}

// helper function: give operator precedence
// e.g. "+" is 1 while "*" is 3
static int opPrecedence( Token::Op op )
{
  int prec = -1;
  switch( op )
  {
    case Token::Exclamation  : prec = 8; break;
    case Token::Percent      : prec = 8; break;
    case Token::Caret        : prec = 7; break;
    case Token::Asterisk     : prec = 5; break;
    case Token::Slash        : prec = 6; break;
    case Token::Modulo       : prec = 6; break;
    case Token::Div          : prec = 6; break;
    case Token::Plus         : prec = 3; break;
    case Token::Minus        : prec = 3; break;
    case Token::RightPar     : prec = 0; break;
    case Token::LeftPar      : prec = -1; break;
    case Token::Semicolon    : prec = 0; break;
    default: prec = -1; break;
  }
  return prec;
}

// creates a token
Token::Token( Type type, const QString& text, int pos )
{
  m_type = type;
  m_text = text;
  m_pos = pos;
}

// copy constructor
Token::Token( const Token& token )
{
  m_type = token.m_type;
  m_text = token.m_text;
  m_pos = token.m_pos;
}

// assignment operator
Token& Token::operator=( const Token& token )
{
  m_type = token.m_type;
  m_text = token.m_text;
  m_pos = token.m_pos;
  return *this;
}

HNumber Token::asNumber() const
{
  if( isNumber() )
    return HNumber( m_text.latin1() );
  else
    return HNumber( 0 );
}

Token::Op Token::asOperator() const
{
  if( isOperator() ) return matchOperator( m_text );
  else return InvalidOp;
}

QString Token::description() const
{
  QString desc;

  switch (m_type )
  {
    case  Number:     desc = "Number"; break;
    case  Identifier: desc = "Identifier"; break;
    case  Operator:   desc = "Operator"; break;
    default:          desc = "Unknown"; break;
  }

  while( desc.length() < 10 ) desc.prepend( ' ' );
  desc.prepend( "  " );
  desc.prepend( QString::number( m_pos ) );
  desc.append( " : " ).append( m_text );

  return desc;
}


TokenStack::TokenStack(): QVector<Token>()
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

void TokenStack::push( const Token& token )
{
  ensureSpace();
  (*this)[ topIndex++ ] = token;
}

Token TokenStack::pop()
{
  return (topIndex > 0 ) ? Token( at( --topIndex ) ) : Token();
}

const Token& TokenStack::top()
{
  return top( 0 );
}

const Token& TokenStack::top( unsigned index )
{
  if( topIndex > (int)index )
    return at( topIndex-index-1 );
  return Token::null;
}

void TokenStack::ensureSpace()
{
  while( topIndex >= size() )
    resize( size() + 10 );
}


// helper function: return true for valid identifier character
static bool isIdentifier( QChar ch )
{
  return ( ch.unicode() == '_' ) || (ch.unicode() == '$' ) || ( ch.isLetter() );
}


// Constructor

Evaluator::Evaluator()
{
  d = new EvaluatorPrivate;
  clear();
}

// Destructor

Evaluator::~Evaluator()
{
  delete d;
}

// Sets a new expression
// note that both the real lex and parse processes will happen later on
// when needed (i.e. "lazy parse"), for example during evaluation.

void Evaluator::setExpression( const QString& expr )
{
  d->expression = expr;
  d->dirty = true;
  d->valid = false;
  d->error = QString::null;
}

// Returns the expression

QString Evaluator::expression() const
{
  return d->expression;
}

// Returns the validity
// note: empty expression is always invalid.

bool Evaluator::isValid() const
{
  if( d->dirty )
  {
    Tokens tokens = scan( d->expression );
    if( !tokens.valid() )
      compile( tokens );
    else
      d->valid = false;
  }
  return d->valid;
}

// Clears everything, also mark it as invalid.

void Evaluator::clear()
{
  d->expression = QString::null;
  d->dirty = true;
  d->valid = false;

  d->error = QString::null;
  d->angleMode = Degree;

  d->constants.clear();
  d->codes.clear();
  d->assignId = QString::null;

  clearVariables();
}

// Returns error message

QString Evaluator::error() const
{
  return d->error;
}

// Returns list of token for the expression.
// this triggers again the lexical analysis step. it is however preferable
// (even when there's small performance penalty) because otherwise we need to
// store parsed tokens all the time which serves no good purpose.

Tokens Evaluator::tokens() const
{
  return scan( d->expression );
}

Tokens Evaluator::scan( const QString& expr )
{
  // to hold the result
  Tokens tokens;

  // parsing state
  enum { Start, Finish, Bad, InNumber, InHexa, InOctal, InBinary, InDecimal, InExpIndicator,
    InExponent, InIdentifier } state;

  // TODO use locale settings if specified
  QString thousand = "";
  QString decimal = ".";

  // initialize variables
  state = Start;
  int i = 0;
  QString ex = expr;
  QString tokenText;
  int tokenStart = 0;

  // force a terminator
  ex.append( QChar() );

  // main loop
  while( (state != Bad) && (state != Finish) && (i < ex.length()) )
  {
    QChar ch = ex.at(i);

    switch( state )
    {

    case Start:

       tokenStart = i;

       // skip any whitespaces
       if( ch.isSpace() ) i++;

       // check for number
       else if( ch.isDigit() )
       {
         state = InNumber;
       }
       else if ( ch == '#' ) // simple hexadec notation
       {
          tokenText.append( "0x" );
          state = InHexa;
          i++;
       }

       // decimal dot ?
       else if ( QString(ch) == decimal )
       {
         tokenText.append( ex.at(i++) );
         state = InDecimal;
       }

       // terminator character
       else if ( ch == QChar::null )
          state = Finish;

       // look for operator match
       else
       {
         int op;
         QString s;
          
#if 0
         // check for three-chars operator
         s.append( ch ).append( ex.at(i+1) ).append( ex.at(i+2) );
         op = matchOperator( s );

         // check for two-chars operator
         if( op == Token::InvalidOp )
#endif
         {
          
            s = QString( ch ).append( ex.at(i+1) );
            op = matchOperator( s );
         }

         // check for one-char operator
         if( op == Token::InvalidOp )
         {
           s = QString( ch );
           op = matchOperator( s );
         }

         // any matched operator ?
         if( op != Token::InvalidOp )
         {
           int len = s.length();
           i += len;
           tokens.append( Token( Token::Operator, s.left( len ), tokenStart ) );
         }
         else state = Bad;
        }
       
       // beginning with unknown alphanumeric ?
       // could be identifier, or function...
       if( state == Bad && isIdentifier( ch ) )
       {
         state = InIdentifier;
       }
       break;

    case InIdentifier:

       // consume as long as alpha, dollar sign, underscore, or digit
       if( isIdentifier( ch )  || ch.isDigit() ) tokenText.append( ex.at(i++) );

       // we're done with identifier
       else
       {
             tokens.append( Token( Token::Identifier, tokenText, tokenStart ) );
             tokenStart = i;
             tokenText = "";
             state = Start;
       }
       break;


    case InNumber:

       // consume as long as it's digit
       if( ch.isDigit() ) tokenText.append( ex.at(i++) );

       // skip thousand separator
       else if( !thousand.isEmpty() && ( ch ==thousand[0] ) ) i++;

       // convert decimal separator to '.'
       else if( !decimal.isEmpty() && ( ch == decimal[0] ) )
       {
         tokenText.append( '.' );
         i++;
         state = InDecimal;
       }

       // exponent ?
       else if( ch.upper() == 'E' )
       {
         tokenText.append( 'E' );
         i++;
         state = InExpIndicator;
       }
       else if (ch.upper() == 'X' && tokenText == "0") // normal hexadec notation
       {
         tokenText.append( 'x' ); i++; state = InHexa;
       }
       else if (ch.upper() == 'B' && tokenText == "0") // binary notation
       {
         tokenText.append( 'b' ); i++; state = InBinary;
       }
       else if (ch.upper() == 'O' && tokenText == "0") // octal notation
       {
         tokenText.append( 'o' ); i++; state = InOctal;
       }
       else if (ch.upper() == 'D' && tokenText == "0") // explicit decimal notation
       {
         // we also need to get rid of the leading zero
         tokenText = ""; i++;
       }

       // we're done with integer number
       else
       {
         tokens.append( Token( Token::Number, tokenText, tokenStart ) );
         tokenText = ""; state = Start;
       }
       break;
       
    case InHexa:
       if (ch.isDigit() || (ch >= 'A' && ch < 'G') || (ch >= 'a' && ch < 'g'))
          tokenText.append( ex.at(i++).toUpper() );
       else // we're done with hexa number
       {
         tokens.append( Token( Token::Number, tokenText, tokenStart ) );
         tokenText = ""; state = Start;
       }
       break;
    case InBinary:
       if (ch == '0' || ch == '1') // very strict rule ;)
          tokenText.append( ex.at(i++) );
       else // we're done with binary number
       {
         tokens.append( Token( Token::Number, tokenText, tokenStart ) );
         tokenText = "";
         state = Start;
       }
       break;
    case InOctal:
       if (ch >= '0' && ch < '8') // octal has only 8 digits, 8 & 9 are invalid
          tokenText.append( ex.at(i++) );
       else // we're done with octal number
       {
         tokens.append( Token( Token::Number, tokenText, tokenStart ) );
         tokenText = ""; state = Start;
       }
       break;
    
    case InDecimal:

       // consume as long as it's digit
       if( ch.isDigit() ) tokenText.append( ex.at(i++) );

       // exponent ?
       else if( ch.upper() == 'E' )
       {
         tokenText.append( 'E' );
         i++;
         state = InExpIndicator;
       }

       // we're done with floating-point number
       else
       {
         tokens.append( Token( Token::Number, tokenText, tokenStart ) );
         tokenText = "";
         state = Start;
       };
       break;

    case InExpIndicator:

       // possible + or - right after E, e.g 1.23E+12 or 4.67E-8
       if( ( ch == '+' ) || ( ch == '-' ) ) tokenText.append( ex.at(i++) );

       // consume as long as it's digit
       else if( ch.isDigit() ) state = InExponent;

       // invalid thing here
       else state = Bad;

       break;

    case InExponent:

       // consume as long as it's digit
       if( ch.isDigit() ) tokenText.append( ex.at(i++) );

       // we're done with floating-point number
       else
       {
         tokens.append( Token( Token::Number, tokenText, tokenStart ) );
         tokenText = "";
         state = Start;
       };
       break;

    case Bad: // bad bad bad
      tokens.setValid( false );
      break;

    default:
       break;
    };
  };

  return tokens;
}


void Evaluator::compile( const Tokens& tokens ) const
{
#ifdef EVALUATOR_DEBUG
    QFile debugFile( "eval.log" );
    debugFile.open( QIODevice::WriteOnly );
    QTextStream dbg( &debugFile );
#endif

  // initialize variables
  d->dirty = false;
  d->valid = false;
  d->codes.clear();
  d->constants.clear();
  d->identifiers.clear();
  d->error = QString::null;

  // sanity check
  if( tokens.count() == 0 ) return;

  TokenStack syntaxStack;
  QStack<int> argStack;
  unsigned argCount = 1;

  for( int i = 0; i <= tokens.count(); i++ )
  {
    // helper token: InvalidOp is end-of-expression
    Token token =  ( i < tokens.count() ) ? tokens[i] : Token( Token::Operator );
    Token::Type tokenType = token.type();

#ifdef EVALUATOR_DEBUG
#endif

#ifdef EVALUATOR_DEBUG
    dbg << "\n";
    dbg << "Token: " << token.description() << "\n";
#endif

    // unknown token is invalid
    if( tokenType == Token::Unknown ) break;

    // for constants, push immediately to stack
    // generate code to load from a constant
    if ( tokenType == Token::Number )
    {
      syntaxStack.push( token );
      d->constants.append( token.asNumber() );
      d->codes.append( Opcode( Opcode::Load, d->constants.count()-1 ) );
#ifdef EVALUATOR_DEBUG
    dbg << "  Push " << token.asNumber() << " to constant pools" << "\n";
#endif
    }

    // for identifier, push immediately to stack
    // generate code to load from reference
    if( tokenType == Token::Identifier )
    {
      syntaxStack.push( token );
      d->identifiers.append( token.text() );
      d->codes.append( Opcode( Opcode::Ref, d->identifiers.count()-1 ) );
#ifdef EVALUATOR_DEBUG
    dbg << "  Push " << token.text() << " to identifier pools" << "\n";
#endif
    }

    // special case for percentage
    if( tokenType == Token::Operator )
    if( token.asOperator() == Token::Percent )
    if( syntaxStack.itemCount() >= 1 )
    if( !syntaxStack.top().isOperator() )
    {
      d->constants.append( HNumber("0.01") );
      d->codes.append( Opcode( Opcode::Load, d->constants.count()-1 ) );
      d->codes.append( Opcode( Opcode::Mul ) );
#ifdef EVALUATOR_DEBUG
          dbg << "  Handling percentage" << "\n";
#endif
    }

    // for any other operator, try to apply all parsing rules
    if( tokenType == Token::Operator )
    if( token.asOperator() != Token::Percent )
    {
#ifdef EVALUATOR_DEBUG
          dbg << "  Checking rules..." << "\n";
#endif
      // repeat until no more rule applies
    bool argHandled = false;
      for( ; ; )
      {
            bool ruleFound = false;
    
        // rule for function last argument:
        //  id ( arg ) -> arg
        if( !ruleFound )
        if( syntaxStack.itemCount() >= 4 )
        {
          Token par2 = syntaxStack.top();
          Token arg = syntaxStack.top( 1 );
          Token par1 = syntaxStack.top( 2 );
          Token id = syntaxStack.top( 3 );
          if( par2.asOperator() == Token::RightPar )
          if( !arg.isOperator() )
          if( par1.asOperator() == Token::LeftPar )
          if( id.isIdentifier() )
          {
            ruleFound = true;
            syntaxStack.pop();
            syntaxStack.pop();
            syntaxStack.pop();
            syntaxStack.pop();
            syntaxStack.push( arg );
            d->codes.append( Opcode( Opcode::Function, argCount ) );
#ifdef EVALUATOR_DEBUG
          dbg << "    Rule for function last argument " << argCount << " \n";
#endif
            argCount = argStack.empty() ? 0 : argStack.pop();
          }
        }

        // are we entering a function ?
        // if token is operator, and stack already has: id ( arg
        if( !ruleFound )
        if( !argHandled )
        if( tokenType == Token::Operator )
        if( syntaxStack.itemCount() >= 3 )
        {
            Token arg = syntaxStack.top();
            Token par = syntaxStack.top( 1 );
            Token id = syntaxStack.top( 2 );
            if( !arg.isOperator() )
            if( par.asOperator() == Token::LeftPar )
            if( id.isIdentifier() )
            {
                ruleFound = true;
                argStack.push( argCount );
                argCount = 1;
#ifdef EVALUATOR_DEBUG
                dbg << "  Entering function " << argCount << " \n";
#endif
                break;
            }
        }
    

        // rule for simplified syntax for function e.g. "sin pi" or "cos 1.2"
        // i.e no need for parentheses like "sin(pi)" or "cos(1.2)"
        if( !ruleFound )
        if( syntaxStack.itemCount() >= 2 )
        {
          Token arg = syntaxStack.top();
          Token id = syntaxStack.top( 1 );
          if( !arg.isOperator() )
          if( id.isIdentifier() )
          if( FunctionRepository::self()->function( id.text() ) )
          {
            ruleFound = true;
            d->codes.append( Opcode( Opcode::Function, 1 ) );
            syntaxStack.pop();
            syntaxStack.pop();
            syntaxStack.push( arg );
#ifdef EVALUATOR_DEBUG
          dbg << "    Rule for simplified function syntax" << "\n";
#endif
          }
        }

         // rule for unary operator in simplified function syntax
         // this handles case like "sin -90"
         if( !ruleFound )
         if( syntaxStack.itemCount() >= 3 )
         {
            Token x = syntaxStack.top();
            Token op = syntaxStack.top( 1 );
            Token id = syntaxStack.top( 2 );
            if( !x.isOperator() )
            if( op.isOperator() )
            if( id.isIdentifier() )
            if( FunctionRepository::self()->function( id.text() ) )
            if( ( op.asOperator() == Token::Plus ) ||
               ( op.asOperator() == Token::Minus ) )
            {
              ruleFound = true;
              syntaxStack.pop();
              syntaxStack.pop();
              syntaxStack.push( x );
              if( op.asOperator() == Token::Minus )
                d->codes.append( Opcode( Opcode::Neg ) );
#ifdef EVALUATOR_DEBUG
          dbg << "    Rule for unary operator in simplified function syntax" << "\n";
#endif
            }
          }
         
         // rule for unary postfix operator in simplified function syntax
         // this handles case like "sin 90!"
         if( !ruleFound )
         if( syntaxStack.itemCount() >= 3 )
         {
            Token op = syntaxStack.top();
            Token x = syntaxStack.top( 1 );
            Token id = syntaxStack.top( 2 );
            if( id.isIdentifier() && FunctionRepository::self()->function( id.text() ) )
            {
               if( !x.isOperator() && op.isOperator() && 
                  op.asOperator() == Token::Exclamation )
               {
                  ruleFound = true;
                  syntaxStack.pop();
                  syntaxStack.pop();
                  syntaxStack.push( x );
                  d->codes.append( Opcode( Opcode::Fact ) );
#ifdef EVALUATOR_DEBUG
                  dbg << "    Rule for unary operator in simplified function syntax" << "\n";
#endif
               }
            }
         }


        // rule for function arguments, if token is , or )
        // id ( arg1 ; arg2 -> id ( arg
        if( !ruleFound )
        if( syntaxStack.itemCount() >= 5 )
        if( ( token.asOperator() == Token::RightPar ) ||
        ( token.asOperator() == Token::Semicolon ) )
        {
          Token arg2 = syntaxStack.top();
          Token sep = syntaxStack.top( 1 );
          Token arg1 = syntaxStack.top( 2 );
          Token par = syntaxStack.top( 3 );
          Token id = syntaxStack.top( 4 );
          if( !arg2.isOperator() )
          if( sep.asOperator() == Token::Semicolon )
          if( !arg1.isOperator() )
          if( par.asOperator() == Token::LeftPar )
          if( id.isIdentifier() )
          {
            ruleFound = true;
            argHandled = true;
            syntaxStack.pop();
            syntaxStack.pop();
            argCount++;
#ifdef EVALUATOR_DEBUG
          dbg << "    Rule for function argument " << argCount << " \n";
#endif
          }
        }

        // rule for function call with parentheses, but without argument
        // e.g. "2*PI()"
        if( !ruleFound )
        if( syntaxStack.itemCount() >= 3 )
        {
          Token par2 = syntaxStack.top();
          Token par1 = syntaxStack.top( 1 );
          Token id = syntaxStack.top( 2 );
          if( par2.asOperator() == Token::RightPar )
          if( par1.asOperator() == Token::LeftPar )
          if( id.isIdentifier() )
          {
            ruleFound = true;
            syntaxStack.pop();
            syntaxStack.pop();
            syntaxStack.pop();
            syntaxStack.push( Token( Token::Number ) );
            d->codes.append( Opcode( Opcode::Function, 0 ) );
#ifdef EVALUATOR_DEBUG
          dbg << "    Rule for function call with parentheses, but without argument" << "\n";
#endif
          }
        }

        // rule for parenthesis:  ( Y ) -> Y
        if( !ruleFound )
        if( syntaxStack.itemCount() >= 3 )
        {
          Token right = syntaxStack.top();
          Token y = syntaxStack.top( 1 );
          Token left = syntaxStack.top( 2 );
          if( right.isOperator() )
          if( !y.isOperator() )
          if( left.isOperator() )
          if( right.asOperator() == Token::RightPar )
          if( left.asOperator() == Token::LeftPar )
          {
            ruleFound = true;
            syntaxStack.pop();
            syntaxStack.pop();
            syntaxStack.pop();
            syntaxStack.push( y );
#ifdef EVALUATOR_DEBUG
          dbg << "    Rule for (Y) -> Y" << "\n";
#endif
          }
        }

        // rule for binary operator:  A (op) B -> A
        // conditions: precedence of op >= precedence of token
        // action: push (op) to result
        // e.g. "A * B" becomes "A" if token is operator "+"
        // exception: for caret (power operator), if op is another caret
        // then the rule doesn't apply, e.g. "2^3^2" is evaluated as "2^(3^2)"
        if( !ruleFound )
        if( syntaxStack.itemCount() >= 3 )
        {
          Token b = syntaxStack.top();
          Token op = syntaxStack.top( 1 );
          Token a = syntaxStack.top( 2 );
          if( !a.isOperator() )
          if( !b.isOperator() )
          if( op.isOperator() )
          if( token.asOperator() != Token::LeftPar )
          if( token.asOperator() != Token::Caret )
          if( opPrecedence( op.asOperator() ) >= opPrecedence( token.asOperator() ) )
          {
            ruleFound = true;
            syntaxStack.pop();
            syntaxStack.pop();
            syntaxStack.pop();
            syntaxStack.push( b );
            switch( op.asOperator() )
            {
              // simple binary operations
              case Token::Plus:         d->codes.append( Opcode::Add ); break;
              case Token::Minus:        d->codes.append( Opcode::Sub ); break;
              case Token::Asterisk:     d->codes.append( Opcode::Mul ); break;
              case Token::Slash:        d->codes.append( Opcode::Div ); break;
              case Token::Caret:        d->codes.append( Opcode::Pow ); break;
              case Token::Modulo:       d->codes.append( Opcode::Modulo ); break;
              case Token::Div:          d->codes.append( Opcode::IntDiv ); break;
              default: break;
            };
#ifdef EVALUATOR_DEBUG
          dbg << "    Rule for binary operator" << "\n";
#endif
          }
         }

         // rule for unary operator:  (op1) (op2) X -> (op1) X
         // conditions: op2 is unary, token is not '('
         // action: push (op2) to result
         // e.g.  "* - 2" becomes "*"
         if( !ruleFound )
         if( token.asOperator() != Token::LeftPar )
         if( syntaxStack.itemCount() >= 3 )
         {
            Token x = syntaxStack.top();
            Token op2 = syntaxStack.top( 1 );
            Token op1 = syntaxStack.top( 2 );
            if( !x.isOperator() && op1.isOperator() && op2.isOperator() &&
               ( op2.asOperator() == Token::Plus || op2.asOperator() == Token::Minus ) )
            {
               ruleFound = true;
               if( op2.asOperator() == Token::Minus )
                  d->codes.append( Opcode( Opcode::Neg ) );
            }
            else // maybe postfix
            {
               x = op2; op2 = syntaxStack.top();
               if( !x.isOperator() && op1.isOperator() && op2.isOperator() &&
               op2.asOperator() == Token::Exclamation )
               {
                  ruleFound = true;
                  d->codes.append( Opcode( Opcode::Fact ) );
               }
            }
            if (ruleFound)
            {
               syntaxStack.pop();
               syntaxStack.pop();
               syntaxStack.push( x );
#ifdef EVALUATOR_DEBUG
          dbg << "    Rule for unary operator" << "\n";
#endif
            }
         }

         // auxilary rule for unary operator:  (op) X -> X
         // conditions: op is unary, op is first in syntax stack, token is not '('
         // action: push (op) to result
         if( !ruleFound )
         if( token.asOperator() != Token::LeftPar )
         if( syntaxStack.itemCount() == 2 )
         {
            Token x = syntaxStack.top();
            Token op = syntaxStack.top( 1 );
            if( !x.isOperator() && op.isOperator() &&
               ( op.asOperator() == Token::Plus || op.asOperator() == Token::Minus ) )
            {
              ruleFound = true;
              if( op.asOperator() == Token::Minus )
                d->codes.append( Opcode( Opcode::Neg ) );
            }
            else
            {
               x = op; op = syntaxStack.top();
               if( !x.isOperator() && op.isOperator() &&
                  ( op.asOperator() == Token::Exclamation ) )
               {
                  ruleFound = true;
                  d->codes.append( Opcode( Opcode::Fact ) );
               }
            }
            if (ruleFound)
            {
              syntaxStack.pop();
              syntaxStack.pop();
              syntaxStack.push( x );
#ifdef EVALUATOR_DEBUG
          dbg << "    Rule for unary operator (auxilary)" << "\n";
#endif
            }
          }

        if( !ruleFound ) break;
      }

      // can't apply rules anymore, push the token
      if( token.asOperator() != Token::Percent )
        syntaxStack.push( token );
    }
  }

  // syntaxStack must left only one operand and end-of-expression (i.e. InvalidOp)
  d->valid = false;
  if( syntaxStack.itemCount() == 2 )
  if( syntaxStack.top().isOperator() )
  if( syntaxStack.top().asOperator() == Token::InvalidOp )
  if( !syntaxStack.top(1).isOperator() )
    d->valid = true;

#ifdef EVALUATOR_DEBUG
    dbg << "Dump: " << dump() << "\n";
#endif

  // bad parsing ? clean-up everything
  if( !d->valid )
  {
    d->constants.clear();
    d->codes.clear();
    d->identifiers.clear();
  }

#ifdef EVALUATOR_DEBUG
  debugFile.close();
#endif
}

Evaluator::AngleMode Evaluator::angleMode() const
{
  return d->angleMode;
}

void Evaluator::setAngleMode( AngleMode am )
{
  d->angleMode = am;
}

HNumber Evaluator::eval()
{
  QStack<HNumber> stack;
  QStack<QString> refs;
  int index;
  HNumber val1, val2;
  QVector<HNumber> args;
  QString fname;
  Function* function;

  if( d->dirty )
  {
    Tokens tokens = scan( d->expression );

    // invalid expression ?
    if( !tokens.valid() )
    {
      d->error = qApp->translate( "Error", "Invalid expression" );
      return HNumber( 0 );
    }

    // variable assignment?
    d->assignId = QString::null;
    if( tokens.count() > 2 )
    if( tokens[0].isIdentifier() )
    if( tokens[1].asOperator() == Token::Equal )
    {
      d->assignId = tokens[0].text();
      tokens.erase( tokens.begin() );
      tokens.erase( tokens.begin() );
    }

    compile( tokens );
  }

  // can not overwrite PI
  if( d->assignId.lower() == QString("pi") )
  {
     d->error = qApp->translate( "Error", "Can not overwrite PI" );
     return HNumber( 0 );
  }

  // variable can't have the same name as function
  if( FunctionRepository::self()->function( d->assignId ) )
  {
     d->error = qApp->translate( "Error",
       "'%1' is a function" ).arg( d->assignId );
     return HNumber( 0 );
  }

  // magic: always set here to avoid being overwritten by user
  set( QString("PI"), HMath::pi() );

  for( int pc = 0; pc < d->codes.count(); pc++ )
  {
    Opcode& opcode = d->codes[pc];
    index = opcode.index;
    switch( opcode.type )
    {
      // no operation
      case Opcode::Nop:
        break;

      // load a constant, push to stack
      case Opcode::Load:
        val1 = d->constants[index];
        stack.push( val1 );
        break;

      // unary operation
      case Opcode::Neg:
        if( stack.count() < 1 )
        {
          d->error = qApp->translate( "Error", "Invalid expression" );
          return HNumber( 0 );
        }
        val1 = stack.pop();
        val1 = HMath::negate( val1 );
        stack.push( val1 );
        break;

      // binary operation: take two values from stack, do the operation,
      // push the result to stack
      case Opcode::Add:
        if( stack.count() < 2 )
        {
          d->error = qApp->translate( "Error", "Invalid expression" );
          return HNumber( 0 );
        }
        val1 = stack.pop();
        val2 = stack.pop();
        val2 += val1;
        stack.push( val2 );
        break;

      case Opcode::Sub:
        if( stack.count() < 2 )
        {
          d->error = qApp->translate( "Error", "Invalid expression" );
          return HNumber( 0 );
        }
        val1 = stack.pop();
        val2 = stack.pop();
        val2 -= val1;
        stack.push( val2 );
        break;

      case Opcode::Mul:
        if( stack.count() < 2 )
        {
          d->error = qApp->translate( "Error", "Invalid expression" );
          return HNumber( 0 );
        }
        val1 = stack.pop();
        val2 = stack.pop();
        val2 *= val1;
        stack.push( val2 );
        break;

      case Opcode::Div:
        if( stack.count() < 2 )
        {
          d->error = qApp->translate( "Error", "Invalid expression" );
          return HNumber( 0 );
        }
        val1 = stack.pop();
        val2 = stack.pop();
        if( val1.isZero() )
        {
          d->error = qApp->translate( "Error", "Divide by zero" );
          return HNumber( 0 );
        }
        val2 /= val1;
        stack.push( val2 );
        break;

      case Opcode::Pow:
        if( stack.count() < 2 )
        {
          d->error = qApp->translate( "Error", "Invalid expression" );
          return HNumber( 0 );
        }
        val1 = stack.pop();
        val2 = stack.pop();
        val2 = HMath::raise( val2, val1 );
        stack.push( val2 );
        break;
       
      case Opcode::Fact:
        if( stack.count() < 1 )
        {
          d->error = qApp->translate( "Error", "Invalid expression" );
          return HNumber( 0 );
        }
        val1 = stack.pop();
        val1 = HMath::factorial( val1 );
        stack.push( val1 );
        break;
       
      case Opcode::Modulo:
        if( stack.count() < 2 )
        {
          d->error = qApp->translate( "Error", "Invalid expression" );
          return HNumber( 0 );
        }
        val1 = stack.pop();
        val2 = stack.pop();
        if( val1.isZero() )
        {
          d->error = qApp->translate( "Error", "Divide by zero" );
          return HNumber( 0 );
        }
        val2 = val2 % val1;
        stack.push( val2 );
        break;
      
      case Opcode::IntDiv:
        if( stack.count() < 2 )
        {
          d->error = qApp->translate( "Error", "Invalid expression" );
          return HNumber( 0 );
        }
        val1 = stack.pop();
        val2 = stack.pop();
        if( val1.isZero() )
        {
          d->error = qApp->translate( "Error", "Divide by zero" );
          return HNumber( 0 );
        }
        val2 /= val1;
        stack.push( HMath::integer(val2) );
        break;
       
      // reference
      case Opcode::Ref:
        fname = d->identifiers[index];
        if( has( fname ) )
          stack.push( get( fname ) );
        else
        {
          function = FunctionRepository::self()->function( fname );
          if( function )
            refs.push( fname );
          else
          {
            d->error = qApp->translate( "Error",
              "Undefined variable '%1'" ).arg( fname );
            return HNumber( 0 );
          }
        }
        break;

      // calling function
      case Opcode::Function:
        if( stack.count() < index )
        {
          d->error = qApp->translate( "Error", "Invalid expression" );
          return HNumber( 0 );
        }

        args.clear();
        for( ; index; index-- )
          args.insert( args.begin(), stack.pop() );

        fname = refs.pop();
        function = FunctionRepository::self()->function( fname );
        if( !function )
        {
          d->error = QString( qApp->translate( "Error",
            "Unknown function or identifier: %1" ) ).arg( fname );
          return HNumber( 0 );
        }

        stack.push( function->exec( this, args ) );
        if( !function->error().isEmpty() )
        {
          d->error = function->error();
          return HNumber( 0 );
        }

        break;

      default:
        break;
    }
  }

  // more than one value in stack ? unsuccesful execution...
  if( stack.count() != 1 )
  {
    d->error = qApp->translate( "Error", "Invalid expression" );
    return HNumber( 0 );
  }

  HNumber result = stack.pop();

  // handle variable assignment, e.g. "x=2*4"
  if( !d->assignId.isEmpty() )
    set( d->assignId, result );

  // "ans" is default variable to hold calculation result
  set( QString("ans"), result );

  return result;
}

void Evaluator::set( const QString& id, HNumber value )
{
  if( !id.isEmpty() )
  {
    d->variables[ id.upper() ].name = id;
    d->variables[ id.upper() ].value = value;
  }
}

HNumber Evaluator::get( const QString& id )
{
  if( id.isEmpty() ) return HNumber( 0 );

  if( !d->variables.contains( id.upper() ) )
    set( id, HNumber( 0 ) );
  return d->variables[ id.upper() ].value;
}

bool Evaluator::has( const QString& id )
{
  return id.isEmpty() ? false : d->variables.contains( id.upper() );
}

void Evaluator::remove( const QString& id )
{
  d->variables.remove( id.upper() );
}

QVector<Variable> Evaluator::variables() const
{
  QVector<Variable> result;

  QMap<QString,Variable>::Iterator it;
  for ( it = d->variables.begin(); it != d->variables.end(); ++it )
  {
    Variable var;
    var.name = it.data().name;
    var.value = it.data().value;
    result.append( var );
  }

  return result;
}

void Evaluator::clearVariables()
{
  d->variables.clear();
  set( QString("PI"), HMath::pi() );
  set( QString("ans"), 0 );
}

QString Evaluator::autoFix( const QString& expr )
{
  int par = 0;
  QString result;

  // strip off all funny characters
  for( int c = 0; c < expr.length(); c++ )
    if( expr[c] >= QChar(32) )
      result.append( expr[c] );

  // automagically close all parenthesis
  Tokens tokens = Evaluator::scan( result );
  for( int i=0; i<tokens.count(); i++ )
    if( tokens[i].asOperator() == Token::LeftPar ) par++;
    else if( tokens[i].asOperator() == Token::RightPar ) par--;
  for(; par > 0; par-- )
    result.append( ')' );

  // special treatment for simple function
  // e.g. "cos" is regarded as "cos(ans)"
  if( !result.isEmpty() )
  {
    Tokens tokens = Evaluator::scan( result );
    if( tokens.count() == 1 )
    {
      if( tokens[0].isIdentifier() )
      {
        Function* f = FunctionRepository::self()->function( tokens[0].text() );
        if( f ) result.append( "(ans)" );
      }
    }
  }

  return result;
}

// Debugging aid

QString Evaluator::dump() const
{
  QString result;
  int c;

  if( d->dirty )
  {
    Tokens tokens = scan( d->expression );
    compile( tokens );
  }

  result = QString("Expression: [%1]\n").arg( d->expression );

  result.append("  Constants:\n");
  for( c = 0; c < d->constants.count(); c++ )
  {
    QString vtext;
    HNumber val = d->constants[c];
    char* ss = HMath::formatFixed( val );
    result += QString("    #%1 = %2\n").arg(c).arg( ss );
    free( ss );
  }

  result.append("\n");
  result.append("  Identifiers:\n");
  for( c = 0; c < d->identifiers.count(); c++ )
  {
    QString vtext;
    result += QString("    #%1 = %2\n").arg(c).arg( d->identifiers[c] );
  }

  result.append("\n");
  result.append("  Code:\n");
  for( int i = 0; i < d->codes.count(); i++ )
  {
    QString ctext;
    switch( d->codes[i].type )
    {
      case Opcode::Load:     ctext = QString("Load #%1").arg( d->codes[i].index ); break;
      case Opcode::Ref:      ctext = QString("Ref #%1").arg( d->codes[i].index ); break;
      case Opcode::Function: ctext = QString("Function (%1)").arg( d->codes[i].index ); break;
      case Opcode::Add:      ctext = "Add"; break;
      case Opcode::Sub:      ctext = "Sub"; break;
      case Opcode::Mul:      ctext = "Mul"; break;
      case Opcode::Div:      ctext = "Div"; break;
      case Opcode::Neg:      ctext = "Neg"; break;
      case Opcode::Pow:      ctext = "Pow"; break;
      case Opcode::Fact:     ctext = "Fact"; break;
      default: ctext = "Unknown"; break;
    }
    result.append( "   " ).append( ctext ).append("\n");
  }

  return result;
}
