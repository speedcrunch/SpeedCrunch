/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     UNKNOWNTOKEN = 258,
     TEXT = 259,
     SCALE = 260,
     DECSCALE = 261,
     DECTAG = 262,
     BINTAG = 263,
     OCTTAG = 264,
     HEXTAG = 265,
     CMPLTAG = 266,
     DOT = 267,
     OPENPAR = 268,
     CLOSEPAR = 269,
     DECSEQ = 270,
     BINSEQ = 271,
     OCTSEQ = 272,
     HEXSEQ = 273,
     GROUPCHAR = 274,
     SEP = 275,
     PERCENT = 276,
     NOPREFIX = 277,
     SIGN = 278,
     VARIABLE = 279,
     CONSTANT = 280,
     POSTFIXOP = 281,
     FUNCTION = 282,
     ASSIGN = 283,
     PREFIX0 = 284,
     L0 = 285,
     R1 = 286,
     PREFIX2 = 287,
     L2 = 288,
     R3 = 289,
     PREFIX4 = 290,
     L4 = 291,
     R5 = 292,
     PREFIX6 = 293,
     L6 = 294,
     R7 = 295,
     PREFIX8 = 296,
     L8 = 297,
     R9 = 298,
     PREFIX10 = 299,
     L10 = 300,
     R11 = 301,
     PREFIX12 = 302,
     L12 = 303,
     R13 = 304,
     PREFIX14 = 305,
     L14 = 306
   };
#endif
/* Tokens.  */
#define UNKNOWNTOKEN 258
#define TEXT 259
#define SCALE 260
#define DECSCALE 261
#define DECTAG 262
#define BINTAG 263
#define OCTTAG 264
#define HEXTAG 265
#define CMPLTAG 266
#define DOT 267
#define OPENPAR 268
#define CLOSEPAR 269
#define DECSEQ 270
#define BINSEQ 271
#define OCTSEQ 272
#define HEXSEQ 273
#define GROUPCHAR 274
#define SEP 275
#define PERCENT 276
#define NOPREFIX 277
#define SIGN 278
#define VARIABLE 279
#define CONSTANT 280
#define POSTFIXOP 281
#define FUNCTION 282
#define ASSIGN 283
#define PREFIX0 284
#define L0 285
#define R1 286
#define PREFIX2 287
#define L2 288
#define R3 289
#define PREFIX4 290
#define L4 291
#define R5 292
#define PREFIX6 293
#define L6 294
#define R7 295
#define PREFIX8 296
#define L8 297
#define R9 298
#define PREFIX10 299
#define L10 300
#define R11 301
#define PREFIX12 302
#define L12 303
#define R13 304
#define PREFIX14 305
#define L14 306




/* Copy the first part of user declarations.  */
#line 58 "exprparser.y"

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
static FCreateVar createVar;
static FGetToken getToken;



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 85 "exprparser.y"
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
/* Line 193 of yacc.c.  */
#line 233 "exprparser.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 246 "exprparser.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  58
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   432

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  52
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  40
/* YYNRULES -- Number of rules.  */
#define YYNRULES  123
/* YYNRULES -- Number of states.  */
#define YYNSTATES  192

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   306

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     9,    11,    16,    20,    25,    29,
      34,    38,    43,    47,    52,    56,    61,    65,    70,    74,
      79,    83,    87,    91,    95,    99,   103,   107,   111,   113,
     116,   118,   122,   124,   126,   128,   131,   133,   135,   137,
     141,   145,   147,   148,   150,   154,   156,   158,   160,   162,
     165,   168,   171,   174,   177,   181,   185,   189,   193,   197,
     201,   205,   209,   210,   213,   214,   217,   218,   221,   222,
     225,   226,   229,   232,   233,   236,   238,   241,   243,   245,
     247,   250,   252,   255,   258,   261,   264,   266,   267,   269,
     270,   272,   273,   275,   276,   278,   280,   284,   286,   290,
     292,   296,   298,   302,   304,   306,   308,   310,   312,   314,
     316,   318,   320,   322,   323,   325,   326,   328,   330,   332,
     334,   336,   338,   340
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      53,     0,    -1,    54,    -1,    24,    28,    54,    -1,    55,
      -1,    54,    30,    88,    54,    -1,    54,    31,    54,    -1,
      54,    33,    88,    54,    -1,    54,    34,    54,    -1,    54,
      36,    88,    54,    -1,    54,    37,    54,    -1,    54,    39,
      88,    54,    -1,    54,    40,    54,    -1,    54,    42,    88,
      54,    -1,    54,    43,    54,    -1,    54,    45,    88,    54,
      -1,    54,    46,    54,    -1,    54,    48,    88,    54,    -1,
      54,    49,    54,    -1,    54,    51,    88,    54,    -1,    91,
      50,    54,    -1,    91,    47,    54,    -1,    91,    44,    54,
      -1,    91,    41,    54,    -1,    91,    38,    54,    -1,    91,
      35,    54,    -1,    91,    32,    54,    -1,    91,    29,    54,
      -1,    56,    -1,    55,    58,    -1,    57,    -1,    13,    54,
      14,    -1,    62,    -1,    24,    -1,    25,    -1,    27,    60,
      -1,    26,    -1,    21,    -1,    57,    -1,    91,    89,    59,
      -1,    13,    61,    14,    -1,    59,    -1,    -1,    54,    -1,
      61,    20,    54,    -1,     4,    -1,    63,    -1,    65,    -1,
      64,    -1,     7,    65,    -1,     8,    66,    -1,     9,    67,
      -1,    10,    68,    -1,    11,    78,    -1,    84,    69,    73,
      -1,    12,    84,    73,    -1,    85,    70,    74,    -1,    12,
      85,    74,    -1,    86,    71,    74,    -1,    12,    86,    74,
      -1,    87,    72,    74,    -1,    12,    87,    74,    -1,    -1,
      12,    80,    -1,    -1,    12,    81,    -1,    -1,    12,    82,
      -1,    -1,    12,    83,    -1,    -1,     6,    75,    -1,     5,
      75,    -1,    -1,     5,    75,    -1,    84,    -1,    23,    76,
      -1,    79,    -1,    84,    -1,    77,    -1,     7,    84,    -1,
      78,    -1,     8,    85,    -1,     9,    86,    -1,    10,    87,
      -1,    11,    78,    -1,    77,    -1,    -1,    84,    -1,    -1,
      85,    -1,    -1,    86,    -1,    -1,    87,    -1,    15,    -1,
      84,    90,    15,    -1,    16,    -1,    85,    90,    16,    -1,
      17,    -1,    86,    90,    17,    -1,    18,    -1,    87,    90,
      18,    -1,    22,    -1,    89,    -1,    29,    -1,    32,    -1,
      35,    -1,    38,    -1,    41,    -1,    44,    -1,    47,    -1,
      50,    -1,    -1,    19,    -1,    -1,    30,    -1,    33,    -1,
      36,    -1,    39,    -1,    42,    -1,    45,    -1,    48,    -1,
      51,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   259,   259,   260,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,   275,   276,   277,   278,   279,   280,
     281,   282,   283,   284,   285,   286,   287,   288,   292,   293,
     297,   298,   302,   303,   304,   305,   309,   310,   314,   315,
     320,   321,   325,   326,   327,   332,   333,   338,   339,   343,
     344,   345,   346,   347,   355,   358,   365,   368,   375,   378,
     385,   388,   395,   396,   400,   401,   405,   406,   410,   411,
     416,   417,   418,   425,   426,   430,   431,   433,   437,   438,
     442,   443,   447,   448,   449,   453,   455,   459,   460,   465,
     466,   471,   472,   477,   478,   483,   484,   489,   490,   495,
     496,   501,   502,   506,   507,   511,   512,   513,   514,   515,
     516,   517,   518,   521,   523,   526,   528,   529,   530,   531,
     532,   533,   534,   535
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "UNKNOWNTOKEN", "TEXT", "SCALE",
  "DECSCALE", "DECTAG", "BINTAG", "OCTTAG", "HEXTAG", "CMPLTAG", "DOT",
  "OPENPAR", "CLOSEPAR", "DECSEQ", "BINSEQ", "OCTSEQ", "HEXSEQ",
  "GROUPCHAR", "SEP", "PERCENT", "NOPREFIX", "SIGN", "VARIABLE",
  "CONSTANT", "POSTFIXOP", "FUNCTION", "ASSIGN", "PREFIX0", "L0", "R1",
  "PREFIX2", "L2", "R3", "PREFIX4", "L4", "R5", "PREFIX6", "L6", "R7",
  "PREFIX8", "L8", "R9", "PREFIX10", "L10", "R11", "PREFIX12", "L12",
  "R13", "PREFIX14", "L14", "$accept", "input", "expr", "postfixval",
  "primval", "simpleval", "postfixop", "prefixval", "params", "paramlist",
  "literal", "number", "taggednumber", "decvalue", "binvalue", "octvalue",
  "hexvalue", "optdecdotfrac", "optbindotfrac", "optoctdotfrac",
  "opthexdotfrac", "optdecscale", "optbase2scale", "exponent", "uint",
  "taggedseq", "taggedbase2seq", "taggedint", "optdecseq", "optbinseq",
  "optoctseq", "opthexseq", "decseq", "binseq", "octseq", "hexseq",
  "prefix", "prefixop", "optgroupchar", "optinfix", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    52,    53,    53,    54,    54,    54,    54,    54,    54,
      54,    54,    54,    54,    54,    54,    54,    54,    54,    54,
      54,    54,    54,    54,    54,    54,    54,    54,    55,    55,
      56,    56,    57,    57,    57,    57,    58,    58,    59,    59,
      60,    60,    61,    61,    61,    62,    62,    63,    63,    64,
      64,    64,    64,    64,    65,    65,    66,    66,    67,    67,
      68,    68,    69,    69,    70,    70,    71,    71,    72,    72,
      73,    73,    73,    74,    74,    75,    75,    75,    76,    76,
      77,    77,    78,    78,    78,    79,    79,    80,    80,    81,
      81,    82,    82,    83,    83,    84,    84,    85,    85,    86,
      86,    87,    87,    88,    88,    89,    89,    89,    89,    89,
      89,    89,    89,    90,    90,    91,    91,    91,    91,    91,
      91,    91,    91,    91
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     3,     1,     4,     3,     4,     3,     4,
       3,     4,     3,     4,     3,     4,     3,     4,     3,     4,
       3,     3,     3,     3,     3,     3,     3,     3,     1,     2,
       1,     3,     1,     1,     1,     2,     1,     1,     1,     3,
       3,     1,     0,     1,     3,     1,     1,     1,     1,     2,
       2,     2,     2,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     0,     2,     0,     2,     0,     2,     0,     2,
       0,     2,     2,     0,     2,     1,     2,     1,     1,     1,
       2,     1,     2,     2,     2,     2,     1,     0,     1,     0,
       1,     0,     1,     0,     1,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     1,     0,     1,     1,     1,     1,
       1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
     115,    45,     0,     0,     0,     0,     0,     0,   115,    95,
      33,    34,   115,   116,   117,   118,   119,   120,   121,   122,
     123,     0,     2,     4,    28,    30,    32,    46,    48,    47,
      62,     0,    49,     0,    97,    50,    64,     0,    99,    51,
      66,     0,   101,    52,    68,     0,     0,     0,    53,    70,
      33,     0,   115,   115,    38,    41,    35,     0,     1,     0,
     115,     0,   115,     0,   115,     0,   115,     0,   115,     0,
     115,     0,   115,     0,    37,    36,    29,    87,   114,    70,
       0,   115,   115,   115,   115,   115,   115,   115,   115,    73,
      89,    73,     0,    73,    91,    73,     0,    73,    93,    73,
       0,    82,    83,    84,     0,     0,    55,    31,     3,    43,
       0,   105,   106,   107,   108,   109,   110,   111,   112,   115,
     103,   115,   104,     6,   115,     8,   115,    10,   115,    12,
     115,    14,   115,    16,   115,    18,   115,    63,    88,    54,
      96,    27,    26,    25,    24,    23,    22,    21,    20,     0,
      57,    65,    90,    56,    98,    59,    67,    92,    58,   100,
      61,    69,    94,    60,   102,     0,     0,     0,    72,    86,
      81,    77,    75,    71,    40,   115,    39,     5,     7,     9,
      11,    13,    15,    17,    19,    74,    80,    85,    76,    79,
      78,    44
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    21,    22,    23,    24,    25,    76,    55,    56,   110,
      26,    27,    28,    29,    35,    39,    43,    79,    91,    95,
      99,   106,   150,   168,   188,   169,   170,   171,   137,   151,
     156,   161,    30,    36,    40,    44,   121,   122,    80,    31
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -101
static const yytype_int16 yypact[] =
{
     172,  -101,    55,    72,    80,    54,   181,    53,   217,  -101,
      41,  -101,   262,  -101,  -101,  -101,  -101,  -101,  -101,  -101,
    -101,   104,   333,   123,  -101,  -101,  -101,  -101,  -101,  -101,
      31,   202,  -101,    99,  -101,  -101,    11,   116,  -101,  -101,
     100,   105,  -101,  -101,    23,    99,   116,   105,  -101,    13,
    -101,   311,   217,   127,  -101,  -101,  -101,   247,  -101,   166,
     217,   166,   217,   166,   217,   166,   217,   166,   217,   166,
     217,   166,   217,   166,  -101,  -101,  -101,    53,  -101,    16,
     143,   217,   217,   217,   217,   217,   217,   217,   217,    15,
      99,   159,   149,    19,   116,   159,   154,    21,   105,   159,
     155,   137,   129,    29,     2,     2,  -101,  -101,   333,   333,
      87,  -101,  -101,  -101,  -101,  -101,  -101,  -101,  -101,   307,
    -101,   217,  -101,   352,   217,   368,   217,   381,   217,    60,
     217,   212,   217,    81,   217,   119,   217,  -101,   106,  -101,
    -101,   352,   368,   381,    60,   212,    81,   119,  -101,     2,
    -101,  -101,   137,  -101,  -101,  -101,  -101,   129,  -101,  -101,
    -101,  -101,    29,  -101,  -101,    53,   181,   320,  -101,  -101,
    -101,  -101,   106,  -101,  -101,   217,  -101,   352,   368,   381,
      60,   212,    81,   119,  -101,  -101,   106,  -101,  -101,  -101,
     106,   333
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -101,  -101,    -8,  -101,  -101,    -9,  -101,    58,  -101,  -101,
    -101,  -101,  -101,   184,  -101,  -101,  -101,  -101,  -101,  -101,
    -101,    95,   -40,  -100,  -101,    25,    -4,  -101,  -101,  -101,
    -101,  -101,    -6,   -29,   -31,   -33,    20,   136,    -7,    -5
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -114
static const yytype_int16 yytable[] =
{
      51,    49,    48,    54,    89,   173,    93,    57,    97,   165,
      45,    46,    47,   166,   103,   102,   101,     9,   104,   105,
     149,   104,   105,    90,   149,   167,   149,  -113,  -113,    92,
      78,  -113,    78,    96,    78,    98,  -113,   100,    78,  -113,
      78,  -113,    78,    77,   108,   109,  -113,  -113,    78,   185,
      78,   153,   123,   155,   125,   158,   127,   160,   129,   163,
     131,   152,   133,   157,   135,   162,    41,     7,     9,    52,
       9,   138,    42,   141,   142,   143,   144,   145,   146,   147,
     148,   124,    92,   126,    33,   128,    96,   130,    34,   132,
     100,   134,    37,   136,    92,    96,   100,    38,   172,   172,
      66,   174,    67,    68,    58,    69,    70,   175,    71,    72,
      54,    73,    94,   177,    57,    34,   178,  -113,   179,    78,
     180,  -113,   181,    42,   182,    78,   183,    70,   184,    71,
      72,     1,    73,    38,     2,     3,     4,     5,     6,     7,
       8,   -42,     9,   172,    74,    92,  -113,   -42,    78,    75,
      96,    50,    11,  -113,    12,   100,    78,    13,   140,   186,
      14,   190,   187,    15,   149,   154,    16,   191,    72,    17,
      73,   159,    18,   164,   139,    19,     1,   176,    20,     2,
       3,     4,     5,     6,     7,     8,    32,     9,   120,    45,
      46,    47,   189,   119,     0,   111,    10,    11,   112,    12,
       0,   113,    13,     0,   114,    14,     0,   115,    15,     0,
     116,    16,     0,   117,    17,     0,   118,    18,     0,     0,
      19,     1,     0,    20,     2,     3,     4,     5,     6,     7,
       8,    81,     9,     0,    82,     0,     0,    83,     0,     0,
      84,    50,    11,    85,    12,     0,    86,    13,     0,    87,
      14,     0,    88,    15,     0,    68,    16,    69,    70,    17,
      71,    72,    18,    73,     0,    19,     1,     0,    20,     2,
       3,     4,     5,     6,     7,    53,   111,     9,     0,   112,
       0,     0,   113,     0,     0,   114,    50,    11,   115,    12,
       0,   116,    13,     0,   117,    14,     0,   118,    15,     0,
       0,    16,     0,     0,    17,     0,     0,    18,     0,     0,
      19,     1,     0,    20,     2,     3,     4,     5,     6,     7,
       0,     0,     9,     0,     0,   107,     0,   165,    45,    46,
      47,    50,    11,     0,    12,     9,     0,    13,     0,     0,
      14,    59,    60,    15,    61,    62,    16,    63,    64,    17,
      65,    66,    18,    67,    68,    19,    69,    70,    20,    71,
      72,     0,    73,    59,    60,     0,    61,    62,     0,    63,
      64,     0,    65,    66,     0,    67,    68,     0,    69,    70,
       0,    71,    72,    60,    73,    61,    62,     0,    63,    64,
       0,    65,    66,     0,    67,    68,     0,    69,    70,     0,
      71,    72,    62,    73,    63,    64,     0,    65,    66,     0,
      67,    68,     0,    69,    70,     0,    71,    72,    64,    73,
      65,    66,     0,    67,    68,     0,    69,    70,     0,    71,
      72,     0,    73
};

static const yytype_int16 yycheck[] =
{
       8,     7,     6,    12,    33,   105,    37,    12,    41,     7,
       8,     9,    10,    11,    47,    46,    45,    15,     5,     6,
       5,     5,     6,    12,     5,    23,     5,    16,    15,    36,
      19,    16,    19,    40,    19,    12,    17,    44,    19,    18,
      19,    18,    19,    12,    52,    53,    15,    18,    19,   149,
      19,    91,    60,    93,    62,    95,    64,    97,    66,    99,
      68,    90,    70,    94,    72,    98,    12,    12,    15,    28,
      15,    77,    18,    81,    82,    83,    84,    85,    86,    87,
      88,    61,    89,    63,    12,    65,    93,    67,    16,    69,
      97,    71,    12,    73,   101,   102,   103,    17,   104,   105,
      40,    14,    42,    43,     0,    45,    46,    20,    48,    49,
     119,    51,    12,   121,   119,    16,   124,    17,   126,    19,
     128,    15,   130,    18,   132,    19,   134,    46,   136,    48,
      49,     4,    51,    17,     7,     8,     9,    10,    11,    12,
      13,    14,    15,   149,    21,   152,    17,    20,    19,    26,
     157,    24,    25,    16,    27,   162,    19,    30,    15,   165,
      33,   167,   166,    36,     5,    16,    39,   175,    49,    42,
      51,    17,    45,    18,    79,    48,     4,   119,    51,     7,
       8,     9,    10,    11,    12,    13,     2,    15,    22,     8,
       9,    10,   167,    57,    -1,    29,    24,    25,    32,    27,
      -1,    35,    30,    -1,    38,    33,    -1,    41,    36,    -1,
      44,    39,    -1,    47,    42,    -1,    50,    45,    -1,    -1,
      48,     4,    -1,    51,     7,     8,     9,    10,    11,    12,
      13,    29,    15,    -1,    32,    -1,    -1,    35,    -1,    -1,
      38,    24,    25,    41,    27,    -1,    44,    30,    -1,    47,
      33,    -1,    50,    36,    -1,    43,    39,    45,    46,    42,
      48,    49,    45,    51,    -1,    48,     4,    -1,    51,     7,
       8,     9,    10,    11,    12,    13,    29,    15,    -1,    32,
      -1,    -1,    35,    -1,    -1,    38,    24,    25,    41,    27,
      -1,    44,    30,    -1,    47,    33,    -1,    50,    36,    -1,
      -1,    39,    -1,    -1,    42,    -1,    -1,    45,    -1,    -1,
      48,     4,    -1,    51,     7,     8,     9,    10,    11,    12,
      -1,    -1,    15,    -1,    -1,    14,    -1,     7,     8,     9,
      10,    24,    25,    -1,    27,    15,    -1,    30,    -1,    -1,
      33,    30,    31,    36,    33,    34,    39,    36,    37,    42,
      39,    40,    45,    42,    43,    48,    45,    46,    51,    48,
      49,    -1,    51,    30,    31,    -1,    33,    34,    -1,    36,
      37,    -1,    39,    40,    -1,    42,    43,    -1,    45,    46,
      -1,    48,    49,    31,    51,    33,    34,    -1,    36,    37,
      -1,    39,    40,    -1,    42,    43,    -1,    45,    46,    -1,
      48,    49,    34,    51,    36,    37,    -1,    39,    40,    -1,
      42,    43,    -1,    45,    46,    -1,    48,    49,    37,    51,
      39,    40,    -1,    42,    43,    -1,    45,    46,    -1,    48,
      49,    -1,    51
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     4,     7,     8,     9,    10,    11,    12,    13,    15,
      24,    25,    27,    30,    33,    36,    39,    42,    45,    48,
      51,    53,    54,    55,    56,    57,    62,    63,    64,    65,
      84,    91,    65,    12,    16,    66,    85,    12,    17,    67,
      86,    12,    18,    68,    87,     8,     9,    10,    78,    84,
      24,    54,    28,    13,    57,    59,    60,    91,     0,    30,
      31,    33,    34,    36,    37,    39,    40,    42,    43,    45,
      46,    48,    49,    51,    21,    26,    58,    12,    19,    69,
      90,    29,    32,    35,    38,    41,    44,    47,    50,    85,
      12,    70,    90,    86,    12,    71,    90,    87,    12,    72,
      90,    85,    86,    87,     5,     6,    73,    14,    54,    54,
      61,    29,    32,    35,    38,    41,    44,    47,    50,    89,
      22,    88,    89,    54,    88,    54,    88,    54,    88,    54,
      88,    54,    88,    54,    88,    54,    88,    80,    84,    73,
      15,    54,    54,    54,    54,    54,    54,    54,    54,     5,
      74,    81,    85,    74,    16,    74,    82,    86,    74,    17,
      74,    83,    87,    74,    18,     7,    11,    23,    75,    77,
      78,    79,    84,    75,    14,    20,    59,    54,    54,    54,
      54,    54,    54,    54,    54,    75,    84,    78,    76,    77,
      84,    54
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 259 "exprparser.y"
    { result = (yyvsp[(1) - (1)].numvalue); ;}
    break;

  case 3:
#line 260 "exprparser.y"
    { result = assignVar((yyvsp[(1) - (3)].var), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 4:
#line 265 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(1) - (1)].numvalue); ;}
    break;

  case 5:
#line 266 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (4)].func), (yyvsp[(1) - (4)].numvalue), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 6:
#line 267 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (3)].func), (yyvsp[(1) - (3)].numvalue), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 7:
#line 268 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (4)].func), (yyvsp[(1) - (4)].numvalue), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 8:
#line 269 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (3)].func), (yyvsp[(1) - (3)].numvalue), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 9:
#line 270 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (4)].func), (yyvsp[(1) - (4)].numvalue), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 10:
#line 271 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (3)].func), (yyvsp[(1) - (3)].numvalue), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 11:
#line 272 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (4)].func), (yyvsp[(1) - (4)].numvalue), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 12:
#line 273 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (3)].func), (yyvsp[(1) - (3)].numvalue), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 13:
#line 274 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (4)].func), (yyvsp[(1) - (4)].numvalue), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 14:
#line 275 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (3)].func), (yyvsp[(1) - (3)].numvalue), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 15:
#line 276 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (4)].func), (yyvsp[(1) - (4)].numvalue), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 16:
#line 277 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (3)].func), (yyvsp[(1) - (3)].numvalue), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 17:
#line 278 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (4)].func), (yyvsp[(1) - (4)].numvalue), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 18:
#line 279 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (3)].func), (yyvsp[(1) - (3)].numvalue), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 19:
#line 280 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (4)].func), (yyvsp[(1) - (4)].numvalue), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 20:
#line 281 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (3)].func), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 21:
#line 282 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (3)].func), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 22:
#line 283 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (3)].func), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 23:
#line 284 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (3)].func), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 24:
#line 285 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (3)].func), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 25:
#line 286 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (3)].func), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 26:
#line 287 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (3)].func), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 27:
#line 288 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (3)].func), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 28:
#line 292 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(1) - (1)].numvalue); ;}
    break;

  case 29:
#line 293 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (2)].func), (yyvsp[(1) - (2)].numvalue)); ;}
    break;

  case 30:
#line 297 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(1) - (1)].numvalue); ;}
    break;

  case 31:
#line 298 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(2) - (3)].numvalue); ;}
    break;

  case 32:
#line 302 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(1) - (1)].numvalue); ;}
    break;

  case 33:
#line 303 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(1) - (1)].var).d; ;}
    break;

  case 34:
#line 304 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(1) - (1)].numvalue); ;}
    break;

  case 35:
#line 305 "exprparser.y"
    { (yyval.numvalue) = callFunction((yyvsp[(1) - (2)].func), (yyvsp[(2) - (2)].params)); ;}
    break;

  case 36:
#line 309 "exprparser.y"
    { (yyval.func) = (yyvsp[(1) - (1)].func); ;}
    break;

  case 37:
#line 310 "exprparser.y"
    { (yyval.func) = (yyvsp[(1) - (1)].func); ;}
    break;

  case 38:
#line 314 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(1) - (1)].numvalue); ;}
    break;

  case 39:
#line 315 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (3)].func), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 40:
#line 320 "exprparser.y"
    { (yyval.params) = (yyvsp[(2) - (3)].params); ;}
    break;

  case 41:
#line 321 "exprparser.y"
    { (yyval.params) = addParam(0, (yyvsp[(1) - (1)].numvalue)); ;}
    break;

  case 42:
#line 325 "exprparser.y"
    { (yyval.params) = 0; ;}
    break;

  case 43:
#line 326 "exprparser.y"
    { (yyval.params) = addParam(0, (yyvsp[(1) - (1)].numvalue)); ;}
    break;

  case 44:
#line 327 "exprparser.y"
    { (yyval.params) = addParam((yyvsp[(1) - (3)].params), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 45:
#line 332 "exprparser.y"
    { (yyval.numvalue) = str2Val((yyvsp[(1) - (1)].string)); ;}
    break;

  case 46:
#line 333 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(1) - (1)].numvalue); ;}
    break;

  case 47:
#line 338 "exprparser.y"
    { (yyval.numvalue) = convertStr((yyvsp[(1) - (1)].numliteral)); ;}
    break;

  case 48:
#line 339 "exprparser.y"
    { (yyval.numvalue) = convertStr((yyvsp[(1) - (1)].numliteral)); ;}
    break;

  case 49:
#line 343 "exprparser.y"
    { (yyval.numliteral) = (yyvsp[(2) - (2)].numliteral); ;}
    break;

  case 50:
#line 344 "exprparser.y"
    { (yyval.numliteral) = (yyvsp[(2) - (2)].numliteral); ;}
    break;

  case 51:
#line 345 "exprparser.y"
    { (yyval.numliteral) = (yyvsp[(2) - (2)].numliteral); ;}
    break;

  case 52:
#line 346 "exprparser.y"
    { (yyval.numliteral) = (yyvsp[(2) - (2)].numliteral); ;}
    break;

  case 53:
#line 347 "exprparser.y"
    { (yyval.numliteral).intpart = (yyvsp[(2) - (2)].seq);
                                           (yyval.numliteral).intpart.complement = 1;
                                           (yyval.numliteral).fracpart = 0;
                                           (yyval.numliteral).exp = initStr(0, 10); ;}
    break;

  case 54:
#line 355 "exprparser.y"
    { (yyval.numliteral).intpart = (yyvsp[(1) - (3)].seq);
                                           (yyval.numliteral).fracpart = (yyvsp[(2) - (3)].seq).digits;
                                           (yyval.numliteral).exp = (yyvsp[(3) - (3)].seq); ;}
    break;

  case 55:
#line 358 "exprparser.y"
    { (yyval.numliteral).intpart = initStr(0, 10);
                                           (yyval.numliteral).fracpart = (yyvsp[(2) - (3)].seq).digits;
                                           (yyval.numliteral).exp = (yyvsp[(3) - (3)].seq); ;}
    break;

  case 56:
#line 365 "exprparser.y"
    { (yyval.numliteral).intpart = (yyvsp[(1) - (3)].seq);
                                           (yyval.numliteral).fracpart = (yyvsp[(2) - (3)].seq).digits;
                                           (yyval.numliteral).exp = (yyvsp[(3) - (3)].seq); ;}
    break;

  case 57:
#line 368 "exprparser.y"
    { (yyval.numliteral).intpart = initStr(0, 2);
                                           (yyval.numliteral).fracpart = (yyvsp[(2) - (3)].seq).digits;
                                           (yyval.numliteral).exp = (yyvsp[(3) - (3)].seq); ;}
    break;

  case 58:
#line 375 "exprparser.y"
    { (yyval.numliteral).intpart = (yyvsp[(1) - (3)].seq);
                                           (yyval.numliteral).fracpart = (yyvsp[(2) - (3)].seq).digits;
                                           (yyval.numliteral).exp = (yyvsp[(3) - (3)].seq); ;}
    break;

  case 59:
#line 378 "exprparser.y"
    { (yyval.numliteral).intpart = initStr(0, 8);
                                           (yyval.numliteral).fracpart = (yyvsp[(2) - (3)].seq).digits;
                                           (yyval.numliteral).exp = (yyvsp[(3) - (3)].seq); ;}
    break;

  case 60:
#line 385 "exprparser.y"
    { (yyval.numliteral).intpart = (yyvsp[(1) - (3)].seq);
                                           (yyval.numliteral).fracpart = (yyvsp[(2) - (3)].seq).digits;
                                           (yyval.numliteral).exp = (yyvsp[(3) - (3)].seq); ;}
    break;

  case 61:
#line 388 "exprparser.y"
    { (yyval.numliteral).intpart = initStr(0, 16);
                                           (yyval.numliteral).fracpart = (yyvsp[(2) - (3)].seq).digits;
                                           (yyval.numliteral).exp = (yyvsp[(3) - (3)].seq); ;}
    break;

  case 62:
#line 395 "exprparser.y"
    { (yyval.seq) = initStr(0, 10); ;}
    break;

  case 63:
#line 396 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 64:
#line 400 "exprparser.y"
    { (yyval.seq) = initStr(0, 2); ;}
    break;

  case 65:
#line 401 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 66:
#line 405 "exprparser.y"
    { (yyval.seq) = initStr(0, 8); ;}
    break;

  case 67:
#line 406 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 68:
#line 410 "exprparser.y"
    { (yyval.seq) = initStr(0, 16); ;}
    break;

  case 69:
#line 411 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 70:
#line 416 "exprparser.y"
    { (yyval.seq) = initStr(0, 10); ;}
    break;

  case 71:
#line 417 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 72:
#line 418 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 73:
#line 425 "exprparser.y"
    { (yyval.seq) = initStr(0, 10); ;}
    break;

  case 74:
#line 426 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 75:
#line 430 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 76:
#line 431 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq);
                                           (yyval.seq).sign = (yyvsp[(1) - (2)].sign); ;}
    break;

  case 77:
#line 433 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 78:
#line 437 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 79:
#line 438 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 80:
#line 442 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 81:
#line 443 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 82:
#line 447 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 83:
#line 448 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 84:
#line 449 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 85:
#line 453 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq);
                                           (yyval.seq).complement = 1; ;}
    break;

  case 86:
#line 455 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 87:
#line 459 "exprparser.y"
    { (yyval.seq) = initStr(0, 10); ;}
    break;

  case 88:
#line 460 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 89:
#line 465 "exprparser.y"
    { (yyval.seq) = initStr(0, 2); ;}
    break;

  case 90:
#line 466 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 91:
#line 471 "exprparser.y"
    { (yyval.seq) = initStr(0, 8); ;}
    break;

  case 92:
#line 472 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 93:
#line 477 "exprparser.y"
    { (yyval.seq) = initStr(0, 16); ;}
    break;

  case 94:
#line 478 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 95:
#line 483 "exprparser.y"
    { (yyval.seq) = initStr((yyvsp[(1) - (1)].string), 10); ;}
    break;

  case 96:
#line 484 "exprparser.y"
    { (yyval.seq) = appendStr((yyvsp[(1) - (3)].seq), (yyvsp[(3) - (3)].string)); ;}
    break;

  case 97:
#line 489 "exprparser.y"
    { (yyval.seq) = initStr((yyvsp[(1) - (1)].string), 2); ;}
    break;

  case 98:
#line 490 "exprparser.y"
    { (yyval.seq) = appendStr((yyvsp[(1) - (3)].seq), (yyvsp[(3) - (3)].string)); ;}
    break;

  case 99:
#line 495 "exprparser.y"
    { (yyval.seq) = initStr((yyvsp[(1) - (1)].string), 8); ;}
    break;

  case 100:
#line 496 "exprparser.y"
    { (yyval.seq) = appendStr((yyvsp[(1) - (3)].seq), (yyvsp[(3) - (3)].string)); ;}
    break;

  case 101:
#line 501 "exprparser.y"
    { (yyval.seq) = initStr((yyvsp[(1) - (1)].string), 16); ;}
    break;

  case 102:
#line 502 "exprparser.y"
    { (yyval.seq) = appendStr((yyvsp[(1) - (3)].seq), (yyvsp[(3) - (3)].string)); ;}
    break;

  case 105:
#line 511 "exprparser.y"
    { (yyval.func) = (yyvsp[(1) - (1)].func); ;}
    break;

  case 106:
#line 512 "exprparser.y"
    { (yyval.func) = (yyvsp[(1) - (1)].func); ;}
    break;

  case 107:
#line 513 "exprparser.y"
    { (yyval.func) = (yyvsp[(1) - (1)].func); ;}
    break;

  case 108:
#line 514 "exprparser.y"
    { (yyval.func) = (yyvsp[(1) - (1)].func); ;}
    break;

  case 109:
#line 515 "exprparser.y"
    { (yyval.func) = (yyvsp[(1) - (1)].func); ;}
    break;

  case 110:
#line 516 "exprparser.y"
    { (yyval.func) = (yyvsp[(1) - (1)].func); ;}
    break;

  case 111:
#line 517 "exprparser.y"
    { (yyval.func) = (yyvsp[(1) - (1)].func); ;}
    break;

  case 112:
#line 518 "exprparser.y"
    { (yyval.func) = (yyvsp[(1) - (1)].func); ;}
    break;


/* Line 1267 of yacc.c.  */
#line 2245 "exprparser.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 537 "exprparser.y"


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
  createVar = callbacks.createVar;
  getToken = callbacks.getToken;

  *pos = -1;
  *lg = 0;
  /* FIXME track error locations */

  errcode = yyparse();
  *r = result;
  return errcode;
}

