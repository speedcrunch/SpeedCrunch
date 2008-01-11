/* exprparser.y: grammar for a single line input */
/*
    Copyright (C) 2007, 2008 Wolf Lammen

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

/* speedcrunch single line expression grammar, expressed as LALR(1) grammar
   in bison style.
   Note the comment at the PERCENT token, where syntax rules do not
   exactly match the semantics of the expression, and where conflicts
   emerging from a co-existence of percent postfix operator and percent encoded
   modulo operator cannot be solved sufficiently in a LR(1) grammar.
   Although the extensions of bison could resolve these conflicts, this was not
   strived for in this first step.
   The scanner need not have much knowledge about the syntax (mostly about the
   structure of numeric literals), and can be kept simple, reading an input string
   in a single pass, one character look-ahead, provided the encoding of tokens is
   not too messy.
   A special problem are overloaded operators, like - for instance, that has
   3 meanings in a usual grammar: (1) subtraction; (2) negation; (3) in a scale:
   indicating a negative integer constant. Since LALR(1) is a look-ahead algorithm,
   the lexical analyzer has to deliver a symbol in advance, without knowing
   which of the several meanings will turn out to be the correct one later on.
   This is especially troublesome, if the operator precedence is not fixed,
   but user definable, and thus, cannot be hard coded in this grammar file.
   As a solution, the lexical analyzer simply delivers all meanings in succession,
   and let the parser find the correct one.
   In order to resolve an otherwise present ambiguity, binary operator tokens must
   be followed by a token representing an overloaded unary operator. If
   no such overloaded operator exists, the lexical analyzer can deliver a special
   token NOPREFIX instead.
*/

%{
#include "bison.h"

static NumValue callBinOperator(Func op, NumValue p1, NumValue p2);
static NumValue callUnaryOperator(Func op, NumValue p);

static void yyerror(char const*);
static int yylex();

static NumValue result;

static DigitSeq initStr(String s, char base);

static FStr2Val str2Val;
static FAppendStr appendStr;
static FConvertStr convertStr;
static FAddParam addParam;
static FCallFunction callFunction;
static FAssignVar assignVar;
static FGetToken getToken;

%}

%defines
%output="exprparser.c"
%union
{
  int        sign;
  String     string;
  DigitSeq   seq;
  NumLiteral numliteral;
  NumValue   numvalue;
  Params     params;
  Var        var;
  Func       func;
}

/* a token that cannot be resolved by the scanner */
%token UNKNOWNTOKEN
/* a token that represents a string */
%token <string>TEXT
/* token that starts a scale (exponent) in a bin, oct or hex encoded scientific
   number. Note that 'e' is not generally available, because it might be mistaken
   for a hexagesimal digit */
%token SCALE
/* token that starts a scale (exponent) in decimal encoded scientific number, e.g 'E'. */
%token DECSCALE
/* token that designates decimal encoding, e.g. '0d'. It is redundant, because
   this grammar automatically assumes a decimal number when a radix tag is missing.
*/
%token DECTAG
/* token that designates binary encoding, e.g. '0b'.
   Note: This tag does not apply to the scale part of a scientific encoded number.
   The scale might be given to a different base and needs its own tag. The default
   is 'decimal encoding'. */
%token BINTAG
/* token that designates octal encoding, e.g. '0o'.
   Note: This tag does not apply to the scale part of a scientific encoded number.
   The scale might be given to a different base and needs its own tag. The default
   is 'decimal encoding'. */
%token OCTTAG
/* token that designates binary encoding, e.g. '0x'.
   Note: This tag does not apply to the scale part of a scientific encoded number.
   The scale might be given to a different base and needs its own tag. The default
   is 'decimal encoding'. */
%token HEXTAG
/* a token indicating a two's complement that has to be sign-extended by the
   input parser */
%token CMPLTAG
/* token representing the decimal dot. In most countries this is '.' */
%token DOT
/* token representing an open (left) parenthesis */
%token OPENPAR
/* token representing a closing (right) parenthesis */
%token CLOSEPAR
/* token representing a sequence of 1 or more digits (0-9) */
%token <string>DECSEQ
/* token representing a sequence of 1 or more binary digits (0-1) */
%token <string>BINSEQ
/* token representing a sequence of 1 or more octal digits (0-7) */
%token <string>OCTSEQ
/* token representing a sequence of 1 or more hexagesimal digits (0-9,A-F) */
%token <string>HEXSEQ
/* token for a grouping char that makes large numbers more readable.
   In most countries this is a comma (,).
   This token has no semantics; the parser just checks its correct
   placement. It is an aid to the user; neither the scanner nor the
   parser needs this. So, the scanner can safely skip this token if it
   can do the (simple) syntax check itself */
%token GROUPCHAR
/* token separating list elements, as in parameter lists in function calls.
   This token has no semantics, but it is syntactically necessary to keep
   expressions unambiguous */
%token SEP
/* the percent operator is really problematic. It is used in expressions
   like 12+13%. Typical pocket calculators evaluate this to 12*(1+13/100);
   in contrast, you will find Speedcrunch computing 12+13/100, which is
   almost useless. So I am much in favour of changing this.
   Percent arithmetic is vastly used in price calculations, where
   traders add VAT to net prices (+19%), or grant discounts (-5%). In
   conjunction with operators other than + and -, pocket calculators
   usually simply divide the tagged operand by 100, assuming users know
   what they are doing.
   Now let's see how this can integrate into Speedcrunch's grammar.
   Actually, the percent operator introduces 3 different operations:
   - a binary operation '+%' equivalent to op1*(1+op2/100)
   - a binary operation '-%' equivalent to op1*(1-op2/100)
   - a unary postfix operation (divide by 100) used elsewhere,
     as in 12*13%
   This shows the difficulty: One time an operand is modified, next time an
   operation, and the syntax looks very much the same. That's why it is
   hard for a parser to tell the difference based on syntactical rules only.
   You have to treat the + and - operation specially, which breaks the uniformity
   of operation handling, and makes the parser more complex. Even worse,
   to distinguish between an ordinary addition (expr+expr) and a percent
   addition (expr+expr%), the parser needs a 2-element look-ahead buffer, which
   makes the underlying grammar finally LR(2).
   One remedy to this is making the % operator syntactically always a unary
   postfix operator. Then, on reduction, the operand is merely tagged as being
   a percent operand, and evaluation is postponed until operands are used in a
   binary operation, or the final result is presented. This keeps the LR(1) grammar,
   at the cost of moving part of the syntax information out of the parser to
   the actions (evaluator). And that will make the parser harder to maintain.
   The cleanest solution would be to introduce two new operations +% and -%, so the
   user tells the parser directly what he/she wants. This, on the other hand, is
   non-standard behaviour.
   So, what shall we do, make the parser overly complex and inflexible, or
   instead, violate clean programming style by scattering syntax handling code
   around, or, maybe, present a user interface not known outside of
   SpeedCrunch's world?
   We assume the second approach here.
   Just to complicate matters even more, the percent character is a shorthand for
   the modulo operator in several program languages. This ambiguity is not
   supported by this grammar, so the scanner has to resolve this (best solved by
   simply assigning a different identifier to the modulo operation)
   [ it seems to be *very difficult* to overload postfix operators in LR grammars,
     as one might see in the following example:
     Is '13%-2' equivalent to '13/100 - 2' or to '13 mod -2'??.
     This ambiguity even cannot be resolved by using parenthesis!! ] */
%token <func>PERCENT
/* when the scanner detects an operator token, it delivers two tokens in
   succession: One describing the precedence of the operator when seen as an
   infix operator; the next describing the precedence when the operator is a
   prefix operator.
   If an operator cannot be used as a prefix operator, this token is returned in
   the second position */
%token NOPREFIX
/* a sign token used in the scale of a literal constant */
%token <sign>SIGN
/* token for a variable identifier, can be read and written to. */
%token <var>VARIABLE
/* token for a constant identifier that only can be read */
%token <numvalue>CONSTANT
/* any postfix operand like the factorial (!), except the %-operator */
%token <func>POSTFIXOP
/* a token for a function identifier */
%token <func>FUNCTION
/* the token for the binary operator (=) expressing an assignment */
%token ASSIGN

/* list of binary operators ordered by precedence and left-to-right or
   right-to-left associativeness. 0 has lowest precedence, 14 highest.
   The indirect syntax declaration here has the advantage, that users can setup
   their own precedence order or associativeness.
   So the scanner reads out the current precedence from a table or so, and returns
   the token L6, if it finds out, it has encountered a level 6, left-to-right
   operator.
*/
%left <func>L0 PREFIX0
%right <func>R1
%left <func>L2 PREFIX2
%right <func>R3
%left <func>L4 PREFIX4
%right <func>R5
%left <func>L6 PREFIX6
%right <func>R7
%left <func>L8 PREFIX8
%right <func>R9
%left <func>L10 PREFIX10
%right <func>R11
%left <func>L12 PREFIX12
%right <func>R13
%left <func>L14 PREFIX14

%type <seq> decseq binseq octseq hexseq
      optdecdotfrac optbindotfrac optoctdotfrac opthexdotfrac
      optdecscale optbase2scale exponent
      taggedbase2seq taggedseq optdecseq optbinseq optoctseq opthexseq
      uint taggedint
%type <numliteral> decvalue binvalue octvalue hexvalue taggednumber
%type <numvalue> expr postfixval primval simpleval literal prefixval number
%type <params> params paramlist
%type <func> prefixop postfixop

%start input

%%
/* the start symbol, a line containing an expression or an assignment
   to a variable */
input:
      expr                               { result = $1; }
    | VARIABLE ASSIGN expr               { result = assignVar($1, $3); }
    ;
/* on top level, an expression is either a binary operation of two
   operands, or a primary expression with possible prefixes (and postfixes). */
expr:
      postfixval                            { $$ = $1; }
    | expr L0 prefix expr %prec L0          { $$ = callBinOperator($2, $1, $4); }
    | expr R1 expr                          { $$ = callBinOperator($2, $1, $3); }
    | expr L2 prefix expr %prec L2          { $$ = callBinOperator($2, $1, $4); }
    | expr R3 expr                          { $$ = callBinOperator($2, $1, $3); }
    | expr L4 prefix expr %prec L4          { $$ = callBinOperator($2, $1, $4); }
    | expr R5 expr                          { $$ = callBinOperator($2, $1, $3); }
    | expr L6 prefix expr %prec L6          { $$ = callBinOperator($2, $1, $4); }
    | expr R7 expr                          { $$ = callBinOperator($2, $1, $3); }
    | expr L8 prefix expr %prec L8          { $$ = callBinOperator($2, $1, $4); }
    | expr R9 expr                          { $$ = callBinOperator($2, $1, $3); }
    | expr L10 prefix expr %prec L10        { $$ = callBinOperator($2, $1, $4); }
    | expr R11 expr                         { $$ = callBinOperator($2, $1, $3); }
    | expr L12 prefix expr %prec L12        { $$ = callBinOperator($2, $1, $4); }
    | expr R13 expr                         { $$ = callBinOperator($2, $1, $3); }
    | expr L14 prefix expr %prec L14        { $$ = callBinOperator($2, $1, $4); }
    | optinfix PREFIX14 expr %prec PREFIX14 { $$ = callUnaryOperator($2, $3); }
    | optinfix PREFIX12 expr %prec PREFIX12 { $$ = callUnaryOperator($2, $3); }
    | optinfix PREFIX10 expr %prec PREFIX10 { $$ = callUnaryOperator($2, $3); }
    | optinfix PREFIX8 expr %prec PREFIX8   { $$ = callUnaryOperator($2, $3); }
    | optinfix PREFIX6 expr %prec PREFIX6   { $$ = callUnaryOperator($2, $3); }
    | optinfix PREFIX4 expr %prec PREFIX4   { $$ = callUnaryOperator($2, $3); }
    | optinfix PREFIX2 expr %prec PREFIX2   { $$ = callUnaryOperator($2, $3); }
    | optinfix PREFIX0 expr %prec PREFIX0   { $$ = callUnaryOperator($2, $3); }
    ;
/* a value with postfixoperators */
postfixval:
      primval                            { $$ = $1; }
    | postfixval postfixop               { $$ = callUnaryOperator($2, $1); }
    ;
/* A single value, possibly being a group like '(expression)'. */
primval:
      simpleval                          { $$ = $1; }
    | OPENPAR expr CLOSEPAR              { $$ = $2; }
    ;
/* a single value, not being a group */
simpleval:
      literal                            { $$ = $1; }
    | VARIABLE                           { $$ = $1.d; }
    | CONSTANT                           { $$ = $1; }
    | FUNCTION params                    { $$ = callFunction($1, $2); }
    ;
/* a single postfix operator */
postfixop:
      POSTFIXOP                          { $$ = $1; }
    | PERCENT                            { $$ = $1; }
    ;
/* a simpleval with optional prefixes */
prefixval:
      simpleval                          { $$ = $1; }
    | optinfix prefixop prefixval        { $$ = callUnaryOperator($2, $3); }
    ;
/* a possibly empty parameter list enclosed in parenthesis, or a
   single parameter without parenthesis */
params:
      OPENPAR paramlist CLOSEPAR         { $$ = $2; }
    | prefixval                          { $$ = addParam(0, $1); }
    ;
/* parameter list separated by ';', but without the enclosing parenthesis */
paramlist:
      /* empty */                        { $$ = 0; }
    | expr                               { $$ = addParam(0, $1); }
    | paramlist SEP expr                 { $$ = addParam($1, $3); }
    ;
/* a literal (= constant expression), either being a string or a numeric
   constant */
literal:
      TEXT                               { $$ = str2Val($1); }
    | number                             { $$ = $1; }
    ;
/* a numeric constant, either being a usual decimal encoded value, or
   a constant having a radix tag like 0x */
number:
      decvalue                           { $$ = convertStr($1); }
    | taggednumber                       { $$ = convertStr($1); }
    ;
/* a numeric constant with a radix tag */
taggednumber:
      DECTAG decvalue                    { $$ = $2; }
    | BINTAG binvalue                    { $$ = $2; }
    | OCTTAG octvalue                    { $$ = $2; }
    | HEXTAG hexvalue                    { $$ = $2; }
    | CMPLTAG taggedbase2seq             { $$.intpart = $2;
                                           $$.intpart.complement = 1;
                                           $$.fracpart = 0;
                                           $$.exp = initStr(0, 10); }
    ;
/* a decimal encoded value, with optional fraction and/or scale. If a scale
   is present, its meaning is *10^scale */
decvalue:
      decseq optdecdotfrac optdecscale   { $$.intpart = $1;
                                           $$.fracpart = $2.digits;
                                           $$.exp = $3; }
    | DOT decseq optdecscale             { $$.intpart = initStr(0, 10);
                                           $$.fracpart = $2.digits;
                                           $$.exp = $3; }
    ;
/* a binary encoded number, with optional fraction and/or scale, but without
   radix tag. If a scale is present, its semantics is *2^scale. */
binvalue:
      binseq optbindotfrac optbase2scale { $$.intpart = $1;
                                           $$.fracpart = $2.digits;
                                           $$.exp = $3; }
    | DOT binseq optbase2scale           { $$.intpart = initStr(0, 2);
                                           $$.fracpart = $2.digits;
                                           $$.exp = $3; }
    ;
/* an octal encoded number, with optional fraction and/or scale, but without
   radix tag. If a scale is present, its semantics is *8^scale. */
octvalue:
      octseq optoctdotfrac optbase2scale { $$.intpart = $1;
                                           $$.fracpart = $2.digits;
                                           $$.exp = $3; }
    | DOT octseq optbase2scale           { $$.intpart = initStr(0, 8);
                                           $$.fracpart = $2.digits;
                                           $$.exp = $3; }
    ;
/* a hexagesimal encoded number, with optional fraction and/or scale, but without
   radix tag. If a scale is present, its semantics is *16^scale. */
hexvalue:
      hexseq opthexdotfrac optbase2scale { $$.intpart = $1;
                                           $$.fracpart = $2.digits;
                                           $$.exp = $3; }
    | DOT hexseq optbase2scale           { $$.intpart = initStr(0, 16);
                                           $$.fracpart = $2.digits;
                                           $$.exp = $3; }
    ;
/* a possibly missing fractional part of a significand. The fractional part
   may reduce to a dot only */
optdecdotfrac:
      /* empty */                        { $$ = initStr(0, 10); }
    | DOT optdecseq                      { $$ = $2; }
    ;
/* a possibly missing fractional part of a binary encoded constant */
optbindotfrac:
      /* empty */                        { $$ = initStr(0, 2); }
    | DOT optbinseq                      { $$ = $2; }
    ;
/* a possibly missing fractional part of an octal encoded constant */
optoctdotfrac:
      /* empty */                        { $$ = initStr(0, 8); }
    | DOT optoctseq                      { $$ = $2; }
    ;
/* a possibly missing fractional part of a hexagesimal encoded constant */
opthexdotfrac:
      /* empty */                        { $$ = initStr(0, 16); }
    | DOT opthexseq                      { $$ = $2; }
    ;
/* a possibly missing scale, together with a scale indicator (E10, or E-2)
   used with decimal encoded values */
optdecscale:
      /* empty */                        { $$ = initStr(0, 10); }
    | DECSCALE exponent                  { $$ = $2; }
    | SCALE exponent                     { $$ = $2; }
    ;
/* a possibly missing scale part of a constant given to a non-decimal base,
   together with an introducing scale character, e.g. p+33, p0xAA, p-0o72.
   If the scale carries no radix tag, a decimal encoding is assumed, regardless
   of the radix the significand is encoded to. */
optbase2scale:
      /* empty */                        { $$ = initStr(0, 10); }
    | SCALE exponent                     { $$ = $2; }
    ;
/* the signed integer representing a scale */
exponent:
      decseq                             { $$ = $1; }
    | SIGN uint                          { $$ = $2;
                                           $$.sign = $1; }
    | taggedint                          { $$ = $1; }
    ;
/* an unsigned integer, given to any base */
uint:
      decseq                             { $$ = $1; }
    | taggedseq                          { $$ = $1; }
    ;
/* an unsigned integer, together with a radix tag */
taggedseq:
      DECTAG decseq                      { $$ = $2; }
    | taggedbase2seq                     { $$ = $1; }
    ;
/* a non-decimal encoded unsigned integer */
taggedbase2seq:
      BINTAG binseq                      { $$ = $2; }
    | OCTTAG octseq                      { $$ = $2; }
    | HEXTAG hexseq                      { $$ = $2; }
    ;
/* an integer with a tags, but without a sign character */
taggedint:
      CMPLTAG taggedbase2seq             { $$ = $2;
                                           $$.complement = 1; }
    | taggedseq                          { $$ = $1; }
    ;
/* a possibly missing sequence of digits, together with grouping characters */
optdecseq:
      /* empty */                        { $$ = initStr(0, 10); }
    | decseq                             { $$ = $1; }
    ;
/* a possibly empty sequence of binary digits together with grouping
   characters */
optbinseq:
      /* empty */                        { $$ = initStr(0, 2); }
    | binseq                             { $$ = $1; }
    ;
/* a possibly empty sequence of octal digits together with grouping
   characters */
optoctseq:
      /* empty */                        { $$ = initStr(0, 8); }
    | octseq                             { $$ = $1; }
    ;
/* a possibly empty sequence of hexagesimal digits together with grouping
   characters */
opthexseq:
      /* empty */                        { $$ = initStr(0, 16); }
    | hexseq                             { $$ = $1; }
    ;
/* a sequence of decimal digits, possibly interspersed with digit
   grouping characters */
decseq:
      DECSEQ                             { $$ = initStr($1, 10); }
    | decseq optgroupchar DECSEQ         { $$ = appendStr($1, $3); }
    ;
/* a sequence of 1 or more binary digits, possibly with interspersing
   grouping characters */
binseq:
      BINSEQ                             { $$ = initStr($1, 2); }
    | binseq optgroupchar BINSEQ         { $$ = appendStr($1, $3); }
    ;
/* a sequence of 1 or more octal digits, possibly with interspersing
   grouping characters */
octseq:
      OCTSEQ                             { $$ = initStr($1, 8); }
    | octseq optgroupchar OCTSEQ         { $$ = appendStr($1, $3); }
    ;
/* a sequence of 1 or more hexagesimal digits, possibly with interspersing
   grouping characters */
hexseq:
      HEXSEQ                             { $$ = initStr($1, 16); }
    | hexseq optgroupchar HEXSEQ         { $$ = appendStr($1, $3); }
    ;
/* the precedence of an operator when used as a unary prefix operator */
prefix:
      NOPREFIX
    | prefixop
    ;
/* prefix operators */
prefixop:
      PREFIX0                            { $$ = $1; }
    | PREFIX2                            { $$ = $1; }
    | PREFIX4                            { $$ = $1; }
    | PREFIX6                            { $$ = $1; }
    | PREFIX8                            { $$ = $1; }
    | PREFIX10                           { $$ = $1; }
    | PREFIX12                           { $$ = $1; }
    | PREFIX14                           { $$ = $1; }
    ;
/* a possibly missing grouping char */
optgroupchar:
      /* empty */
    | GROUPCHAR
    ;
/* overloaded symbol, not used */
optinfix:
      /* empty */
    | L0
    | L2
    | L4
    | L6
    | L8
    | L10
    | L12
    | L14
    ;
%%

/* internal stuff */

/* convenience functions */

static NumValue callBinOperator(Func op, NumValue p1, NumValue p2)
{
  return callFunction(op, addParam(addParam(0, p1), p2));
}

static NumValue callUnaryOperator(Func op, NumValue p)
{
  return callFunction(op, addParam(0, p));
}

static int yylex()
{
  int pos, lg;
  /* FIXME track error locations */
  return getToken(&yylval, &pos, &lg);
}

static void yyerror(char const *msg)
{
  /* FIXME provide error diagnostics */
}

DigitSeq initStr(String s, char base)
{
  DigitSeq result;

  result.base = base;
  result.digits = s;
  result.complement = 0;
  result.sign = 0;
  return result;
}

/* exports */

int parseexpr(CallBacks callbacks, NumValue* r, int* pos, int* lg)
{
  int errcode;

  str2Val = callbacks.str2Val;
  appendStr = callbacks.appendStr;
  convertStr = callbacks.convertStr;
  addParam = callbacks.addParam;
  callFunction = callbacks.callFunction;
  assignVar = callbacks.assignVar;
  getToken = callbacks.getToken;

  *pos = -1;
  *lg = 0;
  /* FIXME track error locations */

  errcode = yyparse();
  *r = result;
  return errcode;
}
