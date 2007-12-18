/* speedcrunch single line expression grammar, expressed as LALR(1) grammar
   in bison style.
   See the comment at the PERCENT token, when the syntax rules do not
   exactly match the semantics of the expression, and where conflicts
   emerging from a co-existence of percent postfix operator and percent encoded
   modulo operator cannot be resolved sufficiently in a LR(1) grammar.
   Although the extensions of bison could resolve these conflicts, this was not
   strived for in this first step.
   The scanner need not have much knowledge about the syntax (mostly about the
   structure of numeric literals), and can be kept simple, reading an input string
   in a single pass, one character look-ahead, provided the encoding of tokens is
   not too messy.
*/

%{
int yylex(void)
void yyerror(const char*);
%}

/* a token that represents a string */
%token TEXT
/* token that starts a scale (exponent) in a bin, oct or hex encoded scientific
   number. Note that 'e' is not generally available, because it might be mistaken
   for a hexagesimal digit */
%token SCALECHAR
/* token that starts a scale (exponent) in decimal encoded scientific number, e.g 'E'. */
%token DECSCALECHAR
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
   input parser
*/
%token CMPLTAG
/* token representing the decimal dot. In most countries this is '.' */
%token DOT
/* token representing an open (left) parenthesis */
%token OPENPAR
/* token representing a closing (right) parenthesis */
%token CLOSEPAR
/* token representing a sequence of 1 or more decimal digits (0-9) */
%token DECSEQ
/* token representing a sequence of 1 or more binary digits (0-1) */
%token BINSEQ
/* token representing a sequence of 1 or more octal digits (0-7) */
%token OCTSEQ
/* token representing a sequence of 1 or more hexagesimal digits (0-9,A-F) */
%token HEXSEQ
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
/* the percent operator is really an obnoxity. It is used in expressions
   like 12+13%. Typical pocket calculators evaluate this to 12*(1+13/100);
   in contrast, Speedcrunch currently takes an even simpler approach and computes
   12+13/100, which is near to complete uselessness. So I am much in favour of
   changing this.
   Percent calculation is vastly used in price calculations, where
   traders add VAT to net prices (+19%), or grant discounts (-5%). In
   conjunction with operators other than + and -, pocket calculators
   usually simply divide the tagged operand by 100, assuming users know
   what they are doing.
   Now let's see how this can integrate with Speedcrunch's grammar.
   Actually, the percent operator introduces 3 different operations:
   - a binary operation '+%' equivalent to op1*(1+op2/100)
   - a binary operation '-%' equivalent to op1*(1-op2/100)
   - a unary postfix operation (divide by 100) used elsewhere,
     as in 12*13%
   This shows the difficulty: One time an operand is modified, the other
   time an operation, and the syntax looks very much the same. So it is
   hard for a parser to tell the difference based on syntactical rules only.
   You have to single out the + and - operation, which breaks the uniformity
   of operation handling, and makes the parser more complex. Even worse,
   to tell an ordinary addition (expr+expr) from a percent addition
   (expr+expr%), the parser needs a 2-element look-ahead buffer, which
   makes the underlying grammar finally LR(2).
   One remedy to this is making the % operator syntactically always a unary
   postfix operator. Then, on reduction, the operand is merely tagged as being
   a percent operand, and evaluation is postponed until operands are used in a
   binary operation, or the final result is presented. This keeps the LR(1) grammar,
   at the cost of moving part of the syntax information out of the parser to
   the actions (evaluator). And that will make the parser harder to maintain.
   The cleanest solution would be to introduce two new operations +% and -%, so the
   user tells the parser directly what he/she wants. This, on the other hand, is
   non-standard.
   So, what shall we do, make the parser overly complex and inflexible, or
   instead, violate clean programming style by scattering syntax handling code
   around, or, maybe, present a user interface not known outside of
   SpeedCrunch's world?
   We assume the second approach here.
   Just to complicate matters even more, the percent character is a shorthand for
   the modulo operator in several program languages. This ambiguity is not
   supported by this grammar, so the scanner has to resolve this (best solved by
   simply assigning a different identifier to the modulo operation) */
%token PERCENT
/* token for a variable identifier, can be read and written to. */
%token VARIABLE
/* token for a constant identifier that only can be read */
%token CONSTANT
/* any postfix operand like the factorial (!), except the %-operator */
%token POSTFIXOP
/* any prefix operator except + or - */
%token PREFIXOP
/* a token for a function identifier */
%token FUNCTION
/* the token for the binary operator (=) expressing an assignment */
%token ASSIGN

/* list of binary operators ordered by precedence and left-to-right or
   right-to-left associativeness. 0 has lowest precedence, 14 highest.
   Since the operators + and - are unary as well, they receive special
   treatment in this grammar. The scanner need not know what kind of
   operator an encountered + or - is; it always delivers the token for a
   binary operation. Of course, an addition will reported by the scanner as
   exactly one of the PLUSMINUSn, according to the precedence of an addition.
   The indirect syntax declaration here has the advantage, that users can setup
   their own precedence order or associativeness.
   So the scanner reads out the current precedence from a table or so, and returns
   the token L6OP, if it finds out, it has encountered a level 6, left-to-right
   operator.
*/
%left L0OP PLUSMINUS0
%right R1OP
%left L2OP PLUSMINUS2
%right R3OP
%left L4OP PLUSMINUS4
%right R5OP
%left L6OP PLUSMINUS6
%right R7OP
%left L8OP PLUSMINUS8
%right R9OP
%left L10OP PLUSMINUS10
%right R11OP
%left L12OP PLUSMINUS12
%right R13OP
%left L14OP PLUSMINUS14

%%
/* the start symbol, a line containing an expression or an assignment
   to a variable */
input: /* empty */
    | expr
    | VARIABLE ASSIGN expr
    ;
/* on top level, an expression is either a binary operation of two
   operands, or a primary expression with possible prefixes and postfixes.
   Do not try to be too clever here! Defining a rule 'binaryop', that
   takes a list of all binary operations, and simplifying this rule
   to:
   expr: primexpr | expr binaryop expr;
   will break LALR(1)!
*/
expr: primexpr
    | expr L0OP expr
    | expr R1OP expr
    | expr L2OP expr
    | expr R3OP expr
    | expr L4OP expr
    | expr R5OP expr
    | expr L6OP expr
    | expr R7OP expr
    | expr L8OP expr
    | expr R9OP expr
    | expr L10OP expr
    | expr R11OP expr
    | expr L12OP expr
    | expr R13OP expr
    | expr L14OP expr
    | expr PLUSMINUS0 expr
    | expr PLUSMINUS2 expr
    | expr PLUSMINUS4 expr
    | expr PLUSMINUS6 expr
    | expr PLUSMINUS8 expr
    | expr PLUSMINUS10 expr
    | expr PLUSMINUS12 expr
    | expr PLUSMINUS14 expr
    ;
/* a single value, together with optional prefix and postfix
   operators */
primexpr: prefix postfixval
    | postfixval
    ;
/* a sequence of 1 or more prefix operators */
prefix: prefixop
    | prefix prefixop
    ;
/* a single prefix operand */
prefixop: PREFIXOP
    | sign
    ;
/* a single value with optional postfix operators */
postfixval: primval optpostfix
    ;
/* a single value, not being a group */
simpleval: literal
    | VARIABLE
    | CONSTANT
    | FUNCTION param
    ;
/* A single value, possibly being a group like '(expression)'. */
primval: simpleval
    | OPENPAR expr CLOSEPAR
    ;
/* a possibly empty sequence of postfix operators */
optpostfix: /* empty */
    | postfix
    ;
/* a sequence of 1 or more postfix operators */
postfix: postfixop
    | postfix postfixop
    ;
/* a single postfix operator */
postfixop: POSTFIXOP
    | PERCENT
    ;
/* a possibly empty parameter list enclosed in parenthesis, or a
   single parameter without parenthesis */
param: OPENPAR paramlist CLOSEPAR
    | simpleval
    ;
/* parameter list separated by ';', but without the enclosing parenthesis */
paramlist: /* empty */
    | expr
    | paramlist SEP expr
    ;
/* a literal (= constant expression), either being a string or a numeric
   constant */
literal: TEXT
    | number
    ;
/* a numeric constant, either being a usual decimal encoded value, or
   a constant having a radix tag like 0x */
number: decvalue
    | taggednumber
    ;
/* a decimal encoded value, with optional fraction and/or scale. If a scale
   is present, its meaning is *10^scale */
decvalue: decseq optdecdotfrac optdecscale
    | DOT decseq optdecscale
    ;
/* a sequence of decimal digits, possibly interspersed with digit
   grouping characters */
decseq: DECSEQ
    | decseq optgroupchar DECSEQ
    ;
/* a possibly missing grouping char */
optgroupchar: /* empty */
    | GROUPCHAR
    ;
/* a possibly missing scale, together with a scale indicator (E10, or E-2)
   used with decimal encoded values */
optdecscale: /* empty */
    | DECSCALECHAR exponent
    ;
/* a possibly missing fractional part of a significand. The fractional part
   may reduce to a dot only */
optdecdotfrac: /* empty */
    | DOT optdecseq
    ;
/* a possibly missing sequence of digits, together with grouping characters */
optdecseq: /* empty */
    | decseq
    ;
/* a numeric constant having a radix tag */
taggednumber: DECTAG decvalue
    | BINTAG binvalue
    | OCTTAG octvalue
    | HEXTAG hexvalue
    | cmplint
    ;
/* a binary encoded number, with optional fraction and/or scale, but without
   radix tag. If a scale is present, its semantics is *2^scale. */
binvalue: binseq optbindotfrac optbase2scale
    | DOT binseq optbase2scale
    ;
/* an octal encoded number, with optional fraction and/or scale, but without
   radix tag. If a scale is present, its semantics is *8^scale. */
octvalue: octseq optoctdotfrac optbase2scale
    | DOT octseq optbase2scale
    ;
/* a hexagesimal encoded number, with optional fraction and/or scale, but without
   radix tag. If a scale is present, its semantics is *16^scale. */
hexvalue: hexseq opthexdotfrac optbase2scale
    | DOT hexseq optbase2scale
    ;
/* a possibly missing fractional part of a binary encoded constant */
optbindotfrac: /* empty */
    | DOT optbinseq
    ;
/* a possibly missing fractional part of an octal encoded constant */
optoctdotfrac: /* empty */
    | DOT optoctseq
    ;
/* a possibly missing fractional part of a hexagesimal encoded constant */
opthexdotfrac: /* empty */
    | DOT opthexseq
    ;
/* a possibly empty sequence of binary digits together with grouping
   characters */
optbinseq: /* empty */
    | binseq
    ;
/* a possibly empty sequence of octal digits together with grouping
   characters */
optoctseq: /* empty */
    | octseq
    ;
/* a possibly empty sequence of hexagesimal digits together with grouping
   characters */
opthexseq: /* empty */
    | hexseq
    ;
/* a sequence of 1 or more binary digits, possibly with interspersing
   grouping characters */
binseq: BINSEQ
    | binseq optgroupchar BINSEQ
    ;
/* a sequence of 1 or more octal digits, possibly with interspersing
   grouping characters */
octseq: OCTSEQ
    | octseq optgroupchar OCTSEQ
    ;
/* a sequence of 1 or more hexagesimal digits, possibly with interspersing
   grouping characters */
hexseq: HEXSEQ
    | hexseq optgroupchar HEXSEQ
    ;
/* a possibly missing scale part of a constant given to a non-decimal base,
   together with an introducing scale character, e.g. p+33, p0xAA, p-0o72.
   If the scale carries no radix tag, a decimal encoding is assumed, regardless
   of the radix the significand is encoded to. */
optbase2scale: /* empty */
    | SCALECHAR exponent
    ;
/* the signed integer representing a scale */
exponent: sign uint
    | decseq
    | taggedint
    ;
/* a sign, + or - */
sign: PLUSMINUS0
    | PLUSMINUS2
    | PLUSMINUS4
    | PLUSMINUS6
    | PLUSMINUS8
    | PLUSMINUS10
    | PLUSMINUS12
    | PLUSMINUS14
    ;
/* an unsigned integer, given to any base */
uint: decseq
    | taggedseq
    ;
/* an unsigned integer, together with a radix tag */
taggedseq: DECTAG decseq
    | taggedbase2seq
    ;
/* a non-decimal encoded unsigned integer */
taggedbase2seq: BINTAG binseq
    | OCTTAG octseq
    | HEXTAG hexseq
    ;
/* a signed integer, given in two's complement notation */
cmplint: CMPLTAG taggedbase2seq
    ;
/* an integer with a radix tag, but without a sign character */
taggedint: cmplint
    | taggedseq
    ;

%%
