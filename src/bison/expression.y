/* speedcrunch single line expression grammar */

%{
int yylex(void)
void yyerror(const char*);
%}

/* a token that represents a string */
%token TEXT
/* token that starts a scale (exponent) in a bin, oct or hex encoded scientific
   number. Note that 'e' is not generally available, because it might be taken for a
   hexagesimal digit */
%token SCALECHAR
/* token that starts a scale (exponent) in decimal encoded scientific number, e.g 'E'. */
%token DECSCALECHAR
/* token that designates decimal encoding, e.g. 'd'. You can do without that, because
   this grammar automatically assumes a decimal number when a radix tag is missing.
   This char is always preceded by a (different) token '0' */
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

%token CMPLTAG DOT
       OPENPAR CLOSEPAR DECDIGIT BINDIGIT OCTDIGIT HEXDIGIT SIGN NUMWS SEP PERCENT
       VARIABLE CONSTANT POSTFIXOP PREFIXOP FUNCTION ASSIGN

%left L0OP
%right R1OP
%left L2OP
%right R3OP
%left L4OP
%right R5OP
%left L6OP
%right R7OP
%left L8OP
%right R9OP
%left L10OP
%right R11OP
%left L12OP
%right R13OP
%left L14OP

%%
input:
    | expr
    | VARIABLE ASSIGN expr
    ;
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
    ;
optnumws: /* empty */
    | NUMWS
    ;
primexpr: optprefixop postfixexpr
    ;
optprefixop: /* empty */
    | prefixopseq
    ;
prefixopseq: PREFIXOP
    | prefixopseq PREFIXOP
    ;
simpleexpr: literal
    | VARIABLE
    | CONSTANT
    | functioncall
    | OPENPAR expr CLOSEPAR
    ;
functioncall: FUNCTION OPENPAR paramlist CLOSEPAR
    ;
paramlist: /* empty */
    | expr
    | paramlist SEP expr
    ;
func1: func1list prefixopseq postfixexpr
    ;
func1list: FUNCTION
    | func1list FUNCTION
    ;
postfixexpr: simpleexpr optpostfixop
    | func1
    ;
optpostfixop: /* empty */
    | postfixseq
    ;
postfixseq: POSTFIXOP
    | postfixseq POSTFIXOP
    ;
literal: TEXT
    | number
    ;
number: decvalue
    | taggednumber
    ;
decvalue: decseq optdecdotfrac optdecscale
    | DOT decseq optdecscale
    ;
decseq: DECDIGIT
    | decseq optnumws DECDIGIT
    ;
optdecscale: /* empty */
    | decscale
    ;
decscale: DECSCALECHAR exponent
    ;
optdecdotfrac: /* empty */
    | decdotfrac
    ;
decdotfrac: DOT optdecseq
    ;
optdecseq: /* empty */
    | decseq
    ;
taggednumber: DECTAG decvalue
    | BINTAG binvalue
    | OCTTAG octvalue
    | HEXTAG hexvalue
    | cmplint
    ;
binvalue: binseq optbindotfrac optbase2scale
    | DOT binseq optbase2scale
    ;
octvalue: octseq optoctdotfrac optbase2scale
    | DOT octseq optbase2scale
    ;
hexvalue: hexseq opthexdotfrac optbase2scale
    | DOT hexseq optbase2scale
    ;
optbindotfrac: /*empty */
    | bindotfrac
    ;
optoctdotfrac: /*empty */
    | octdotfrac
    ;
opthexdotfrac: /*empty */
    | hexdotfrac
    ;
bindotfrac: DOT optbinseq
    ;
octdotfrac: DOT optoctseq
    ;
hexdotfrac: DOT opthexseq
    ;
optbinseq: /* empty */
    | binseq
    ;
optoctseq: /* empty */
    | octseq
    ;
opthexseq: /* empty */
    | hexseq
    ;
binseq: BINDIGIT
    | binseq optnumws BINDIGIT
    ;
octseq: OCTDIGIT
    | octseq optnumws OCTDIGIT
    ;
hexseq: HEXDIGIT
    | hexseq optnumws HEXDIGIT
    ;
optbase2scale: /* empty */
    | base2scale
    ;
base2scale: SCALECHAR exponent
    ;
exponent: SIGN uint
    | taggedint
    ;
uint: decseq
    | taggedseq
    ;
taggedseq: DECTAG decseq
    | taggedbase2seq
    ;
taggedbase2seq: BINTAG binseq
    | OCTTAG octseq
    | HEXTAG hexseq
    ;
cmplint: CMPLTAG taggedbase2seq
    ;
taggedint: cmplint
    | taggedseq
%%
