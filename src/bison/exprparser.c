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
#line 84 "exprparser.y"
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
#line 232 "exprparser.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 245 "exprparser.c"

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
#define YYLAST   438

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  52
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  42
/* YYNRULES -- Number of rules.  */
#define YYNRULES  127
/* YYNRULES -- Number of states.  */
#define YYNSTATES  213

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
       0,     0,     3,     5,     9,    11,    17,    21,    27,    31,
      37,    41,    47,    51,    57,    61,    67,    71,    77,    81,
      87,    92,    97,   102,   107,   112,   117,   122,   127,   129,
     132,   134,   138,   140,   142,   144,   147,   149,   151,   153,
     158,   162,   164,   165,   167,   171,   173,   175,   177,   179,
     182,   185,   188,   191,   194,   198,   202,   206,   210,   214,
     218,   222,   226,   227,   230,   231,   234,   235,   238,   239,
     242,   243,   246,   249,   250,   253,   255,   259,   261,   262,
     265,   267,   269,   272,   274,   277,   280,   283,   286,   288,
     289,   291,   292,   294,   295,   297,   298,   300,   302,   306,
     308,   312,   314,   318,   320,   324,   326,   328,   330,   332,
     334,   336,   338,   340,   342,   344,   345,   347,   348,   350,
     352,   354,   356,   358,   360,   362,   364,   365
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      53,     0,    -1,    54,    -1,    24,    28,    54,    -1,    55,
      -1,    54,    30,    89,    93,    54,    -1,    54,    31,    54,
      -1,    54,    33,    89,    93,    54,    -1,    54,    34,    54,
      -1,    54,    36,    89,    93,    54,    -1,    54,    37,    54,
      -1,    54,    39,    89,    93,    54,    -1,    54,    40,    54,
      -1,    54,    42,    89,    93,    54,    -1,    54,    43,    54,
      -1,    54,    45,    89,    93,    54,    -1,    54,    46,    54,
      -1,    54,    48,    89,    93,    54,    -1,    54,    49,    54,
      -1,    54,    51,    89,    93,    54,    -1,    92,    50,    93,
      54,    -1,    92,    47,    93,    54,    -1,    92,    44,    93,
      54,    -1,    92,    41,    93,    54,    -1,    92,    38,    93,
      54,    -1,    92,    35,    93,    54,    -1,    92,    32,    93,
      54,    -1,    92,    29,    93,    54,    -1,    56,    -1,    55,
      58,    -1,    57,    -1,    13,    54,    14,    -1,    62,    -1,
      24,    -1,    25,    -1,    27,    60,    -1,    26,    -1,    21,
      -1,    57,    -1,    92,    90,    93,    59,    -1,    13,    61,
      14,    -1,    59,    -1,    -1,    54,    -1,    61,    20,    54,
      -1,     4,    -1,    63,    -1,    65,    -1,    64,    -1,     7,
      65,    -1,     8,    66,    -1,     9,    67,    -1,    10,    68,
      -1,    11,    79,    -1,    85,    69,    73,    -1,    12,    85,
      73,    -1,    86,    70,    74,    -1,    12,    86,    74,    -1,
      87,    71,    74,    -1,    12,    87,    74,    -1,    88,    72,
      74,    -1,    12,    88,    74,    -1,    -1,    12,    81,    -1,
      -1,    12,    82,    -1,    -1,    12,    83,    -1,    -1,    12,
      84,    -1,    -1,     6,    75,    -1,     5,    75,    -1,    -1,
       5,    75,    -1,    85,    -1,    76,    23,    77,    -1,    80,
      -1,    -1,    92,    89,    -1,    85,    -1,    78,    -1,     7,
      85,    -1,    79,    -1,     8,    86,    -1,     9,    87,    -1,
      10,    88,    -1,    11,    79,    -1,    78,    -1,    -1,    85,
      -1,    -1,    86,    -1,    -1,    87,    -1,    -1,    88,    -1,
      15,    -1,    85,    91,    15,    -1,    16,    -1,    86,    91,
      16,    -1,    17,    -1,    87,    91,    17,    -1,    18,    -1,
      88,    91,    18,    -1,    22,    -1,    90,    -1,    29,    -1,
      32,    -1,    35,    -1,    38,    -1,    41,    -1,    44,    -1,
      47,    -1,    50,    -1,    -1,    19,    -1,    -1,    30,    -1,
      33,    -1,    36,    -1,    39,    -1,    42,    -1,    45,    -1,
      48,    -1,    51,    -1,    -1,    23,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   271,   271,   272,   277,   278,   279,   280,   281,   282,
     283,   284,   285,   286,   287,   288,   289,   290,   291,   292,
     293,   294,   295,   296,   297,   298,   299,   300,   304,   305,
     309,   310,   314,   315,   316,   317,   321,   322,   326,   327,
     332,   333,   337,   338,   339,   344,   345,   350,   351,   355,
     356,   357,   358,   359,   367,   370,   377,   380,   387,   390,
     397,   400,   407,   408,   412,   413,   417,   418,   422,   423,
     428,   429,   430,   437,   438,   442,   443,   445,   449,   451,
     455,   456,   460,   461,   465,   466,   467,   471,   473,   477,
     478,   483,   484,   489,   490,   495,   496,   501,   502,   507,
     508,   513,   514,   519,   520,   524,   525,   529,   530,   531,
     532,   533,   534,   535,   536,   539,   541,   544,   546,   547,
     548,   549,   550,   551,   552,   553,   556,   557
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
  "opthexdotfrac", "optdecscale", "optbase2scale", "exponent",
  "optoverload", "uint", "taggedseq", "taggedbase2seq", "taggedint",
  "optdecseq", "optbinseq", "optoctseq", "opthexseq", "decseq", "binseq",
  "octseq", "hexseq", "prefix", "prefixop", "optgroupchar", "optinfix",
  "optsign", 0
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
      77,    77,    78,    78,    79,    79,    79,    80,    80,    81,
      81,    82,    82,    83,    83,    84,    84,    85,    85,    86,
      86,    87,    87,    88,    88,    89,    89,    90,    90,    90,
      90,    90,    90,    90,    90,    91,    91,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    93,    93
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     3,     1,     5,     3,     5,     3,     5,
       3,     5,     3,     5,     3,     5,     3,     5,     3,     5,
       4,     4,     4,     4,     4,     4,     4,     4,     1,     2,
       1,     3,     1,     1,     1,     2,     1,     1,     1,     4,
       3,     1,     0,     1,     3,     1,     1,     1,     1,     2,
       2,     2,     2,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     0,     2,     0,     2,     0,     2,     0,     2,
       0,     2,     2,     0,     2,     1,     3,     1,     0,     2,
       1,     1,     2,     1,     2,     2,     2,     2,     1,     0,
       1,     0,     1,     0,     1,     0,     1,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     0,     1,     0,     1,     1,
       1,     1,     1,     1,     1,     1,     0,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
     117,    45,     0,     0,     0,     0,     0,     0,   117,    97,
      33,    34,   117,   118,   119,   120,   121,   122,   123,   124,
     125,     0,     2,     4,    28,    30,    32,    46,    48,    47,
      62,     0,    49,     0,    99,    50,    64,     0,   101,    51,
      66,     0,   103,    52,    68,     0,     0,     0,    53,    70,
      33,     0,   117,   117,    38,    41,    35,     0,     1,     0,
     117,     0,   117,     0,   117,     0,   117,     0,   117,     0,
     117,     0,   117,     0,    37,    36,    29,    89,   116,    70,
       0,   126,   126,   126,   126,   126,   126,   126,   126,    73,
      91,    73,     0,    73,    93,    73,     0,    73,    95,    73,
       0,    84,    85,    86,   117,   117,    55,    31,     3,    43,
       0,   107,   108,   109,   110,   111,   112,   113,   114,   126,
     105,   126,   106,     6,   126,     8,   126,    10,   126,    12,
     126,    14,   126,    16,   126,    18,   126,    63,    90,    54,
      98,   127,   117,   117,   117,   117,   117,   117,   117,   117,
     117,    57,    65,    92,    56,   100,    59,    67,    94,    58,
     102,    61,    69,    96,    60,   104,     0,     0,    72,     0,
      88,    83,    77,    75,     0,    71,    40,   117,   117,   117,
     117,   117,   117,   117,   117,   117,   117,    27,    26,    25,
      24,    23,    22,    21,    20,    74,    82,    87,     0,    79,
      44,    39,     5,     7,     9,    11,    13,    15,    17,    19,
      76,    81,    80
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    21,    22,    23,    24,    25,    76,    55,    56,   110,
      26,    27,    28,    29,    35,    39,    43,    79,    91,    95,
      99,   106,   151,   168,   169,   210,   170,   171,   172,   137,
     152,   157,   162,    30,    36,    40,    44,   121,   122,    80,
      31,   142
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -97
static const yytype_int16 yypact[] =
{
     263,   -97,    96,    62,    40,     9,   226,    34,   308,   -97,
      57,   -97,   353,   -97,   -97,   -97,   -97,   -97,   -97,   -97,
     -97,   114,     2,    75,   -97,   -97,   -97,   -97,   -97,   -97,
     197,    71,   -97,   128,   -97,   -97,    67,   140,   -97,   -97,
     310,   159,   -97,   -97,   111,   128,   140,   159,   -97,   121,
     -97,   377,   308,   218,   -97,   -97,   -97,   212,   -97,    -4,
     308,    -4,   308,    -4,   308,    -4,   308,    -4,   308,    -4,
     308,    -4,   308,    -4,   -97,   -97,   -97,    34,   -97,   160,
     179,   174,   174,   174,   174,   174,   174,   174,   174,   119,
     128,   195,   187,   136,   140,   195,   193,   115,   159,   195,
     196,    83,   144,   221,   364,   364,   -97,   -97,     2,     2,
      10,   -97,   -97,   -97,   -97,   -97,   -97,   -97,   -97,   174,
     -97,   174,   -97,   258,   174,   156,   174,   297,   174,   387,
     174,   234,   174,   130,   174,   169,   174,   -97,     7,   -97,
     -97,   -97,   308,   308,   308,   308,   308,   308,   308,   308,
     364,   -97,   -97,    83,   -97,   -97,   -97,   -97,   144,   -97,
     -97,   -97,   -97,   221,   -97,   -97,    34,   226,   -97,   201,
     -97,   -97,   -97,     7,    -4,   -97,   -97,   308,    80,   308,
     308,   308,   308,   308,   308,   308,   308,   258,   156,   297,
     387,   234,   130,   169,   -97,   -97,     7,   -97,    51,   -97,
       2,   -97,   258,   156,   297,   387,   234,   130,   169,   -97,
     -97,   -97,     7
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -97,   -97,     3,   -97,   -97,   -10,   -97,    59,   -97,   -97,
     -97,   -97,   -97,   244,   -97,   -97,   -97,   -97,   -97,   -97,
     -97,   170,    63,   -96,   -97,   -97,    54,    -3,   -97,   -97,
     -97,   -97,   -97,    -7,   -28,   -17,   -34,   -57,   198,   -21,
     -11,    87
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -116
static const yytype_int16 yytable[] =
{
      49,    57,    54,    48,   124,    89,   126,    97,   128,   175,
     130,    51,   132,   103,   134,    92,   136,   101,   120,    96,
      93,    41,  -115,   100,   176,   111,    78,    42,   112,   102,
     177,   113,    59,    60,   114,    61,    62,   115,    63,    64,
     116,    65,    66,   117,    67,    68,   118,    69,    70,     9,
      71,    72,    37,    73,   195,   108,   109,    38,   166,    45,
      46,    47,   153,   123,   163,   125,     9,   127,    92,   129,
     138,   131,    96,   133,    33,   135,   100,   158,    34,    90,
      92,    96,   100,  -115,     1,    52,    78,     2,     3,     4,
       5,     6,     7,   174,   174,     9,    74,   173,   173,  -115,
      81,    75,    78,    82,    50,    11,    83,    12,     7,    84,
      13,     9,    85,    14,    58,    86,    15,   199,    87,    16,
     150,    88,    17,    98,   150,    18,   104,   105,    19,  -115,
      78,    20,    92,  -115,    78,  -115,  -115,    96,    78,   174,
      78,   150,   100,   173,    34,   187,   188,   189,   190,   191,
     192,   193,   194,  -115,   154,    78,   156,    38,   159,   196,
     161,  -115,   164,    78,   197,   104,   105,    57,    54,   143,
     144,   145,   146,   147,   148,   149,    70,    42,    71,    72,
     200,    73,   202,   203,   204,   205,   206,   207,   208,   209,
      62,   212,    63,    64,   140,    65,    66,   141,    67,    68,
     150,    69,    70,   155,    71,    72,   178,    73,   179,    77,
     160,   180,  -115,   181,   165,   182,    78,   183,    72,   184,
      73,   185,     1,   186,   198,     2,     3,     4,     5,     6,
       7,     8,   -42,     9,    45,    46,    47,   201,   -42,  -115,
      78,   111,    50,    11,   112,    12,    32,   113,    13,   139,
     114,    14,   211,   115,    15,   119,   116,    16,     0,   117,
      17,     0,   118,    18,     0,     0,    19,     1,     0,    20,
       2,     3,     4,     5,     6,     7,     8,    68,     9,    69,
      70,     0,    71,    72,     0,    73,     0,    10,    11,    60,
      12,    61,    62,    13,    63,    64,    14,    65,    66,    15,
      67,    68,    16,    69,    70,    17,    71,    72,    18,    73,
       0,    19,     1,     0,    20,     2,     3,     4,     5,     6,
       7,     8,    94,     9,     0,     0,     0,  -115,     0,    78,
       0,     0,    50,    11,    64,    12,    65,    66,    13,    67,
      68,    14,    69,    70,    15,    71,    72,    16,    73,     0,
      17,     0,     0,    18,     0,     0,    19,     1,     0,    20,
       2,     3,     4,     5,     6,     7,    53,     0,     9,     0,
       0,   166,    45,    46,    47,   167,     0,    50,    11,     9,
      12,     0,     0,    13,     0,     0,    14,   -78,     0,    15,
       0,   107,    16,     0,    13,    17,     0,    14,    18,     0,
      15,    19,     0,    16,    20,     0,    17,    59,    60,    18,
      61,    62,    19,    63,    64,    20,    65,    66,     0,    67,
      68,     0,    69,    70,     0,    71,    72,    66,    73,    67,
      68,     0,    69,    70,     0,    71,    72,     0,    73
};

static const yytype_int16 yycheck[] =
{
       7,    12,    12,     6,    61,    33,    63,    41,    65,   105,
      67,     8,    69,    47,    71,    36,    73,    45,    22,    40,
      37,    12,    15,    44,    14,    29,    19,    18,    32,    46,
      20,    35,    30,    31,    38,    33,    34,    41,    36,    37,
      44,    39,    40,    47,    42,    43,    50,    45,    46,    15,
      48,    49,    12,    51,   150,    52,    53,    17,     7,     8,
       9,    10,    90,    60,    98,    62,    15,    64,    89,    66,
      77,    68,    93,    70,    12,    72,    97,    94,    16,    12,
     101,   102,   103,    16,     4,    28,    19,     7,     8,     9,
      10,    11,    12,   104,   105,    15,    21,   104,   105,    16,
      29,    26,    19,    32,    24,    25,    35,    27,    12,    38,
      30,    15,    41,    33,     0,    44,    36,   174,    47,    39,
       5,    50,    42,    12,     5,    45,     5,     6,    48,    18,
      19,    51,   153,    18,    19,    16,    15,   158,    19,   150,
      19,     5,   163,   150,    16,   142,   143,   144,   145,   146,
     147,   148,   149,    17,    91,    19,    93,    17,    95,   166,
      97,    17,    99,    19,   167,     5,     6,   178,   178,    82,
      83,    84,    85,    86,    87,    88,    46,    18,    48,    49,
     177,    51,   179,   180,   181,   182,   183,   184,   185,   186,
      34,   198,    36,    37,    15,    39,    40,    23,    42,    43,
       5,    45,    46,    16,    48,    49,   119,    51,   121,    12,
      17,   124,    15,   126,    18,   128,    19,   130,    49,   132,
      51,   134,     4,   136,    23,     7,     8,     9,    10,    11,
      12,    13,    14,    15,     8,     9,    10,   178,    20,    18,
      19,    29,    24,    25,    32,    27,     2,    35,    30,    79,
      38,    33,   198,    41,    36,    57,    44,    39,    -1,    47,
      42,    -1,    50,    45,    -1,    -1,    48,     4,    -1,    51,
       7,     8,     9,    10,    11,    12,    13,    43,    15,    45,
      46,    -1,    48,    49,    -1,    51,    -1,    24,    25,    31,
      27,    33,    34,    30,    36,    37,    33,    39,    40,    36,
      42,    43,    39,    45,    46,    42,    48,    49,    45,    51,
      -1,    48,     4,    -1,    51,     7,     8,     9,    10,    11,
      12,    13,    12,    15,    -1,    -1,    -1,    17,    -1,    19,
      -1,    -1,    24,    25,    37,    27,    39,    40,    30,    42,
      43,    33,    45,    46,    36,    48,    49,    39,    51,    -1,
      42,    -1,    -1,    45,    -1,    -1,    48,     4,    -1,    51,
       7,     8,     9,    10,    11,    12,    13,    -1,    15,    -1,
      -1,     7,     8,     9,    10,    11,    -1,    24,    25,    15,
      27,    -1,    -1,    30,    -1,    -1,    33,    23,    -1,    36,
      -1,    14,    39,    -1,    30,    42,    -1,    33,    45,    -1,
      36,    48,    -1,    39,    51,    -1,    42,    30,    31,    45,
      33,    34,    48,    36,    37,    51,    39,    40,    -1,    42,
      43,    -1,    45,    46,    -1,    48,    49,    40,    51,    42,
      43,    -1,    45,    46,    -1,    48,    49,    -1,    51
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     4,     7,     8,     9,    10,    11,    12,    13,    15,
      24,    25,    27,    30,    33,    36,    39,    42,    45,    48,
      51,    53,    54,    55,    56,    57,    62,    63,    64,    65,
      85,    92,    65,    12,    16,    66,    86,    12,    17,    67,
      87,    12,    18,    68,    88,     8,     9,    10,    79,    85,
      24,    54,    28,    13,    57,    59,    60,    92,     0,    30,
      31,    33,    34,    36,    37,    39,    40,    42,    43,    45,
      46,    48,    49,    51,    21,    26,    58,    12,    19,    69,
      91,    29,    32,    35,    38,    41,    44,    47,    50,    86,
      12,    70,    91,    87,    12,    71,    91,    88,    12,    72,
      91,    86,    87,    88,     5,     6,    73,    14,    54,    54,
      61,    29,    32,    35,    38,    41,    44,    47,    50,    90,
      22,    89,    90,    54,    89,    54,    89,    54,    89,    54,
      89,    54,    89,    54,    89,    54,    89,    81,    85,    73,
      15,    23,    93,    93,    93,    93,    93,    93,    93,    93,
       5,    74,    82,    86,    74,    16,    74,    83,    87,    74,
      17,    74,    84,    88,    74,    18,     7,    11,    75,    76,
      78,    79,    80,    85,    92,    75,    14,    20,    93,    93,
      93,    93,    93,    93,    93,    93,    93,    54,    54,    54,
      54,    54,    54,    54,    54,    75,    85,    79,    23,    89,
      54,    59,    54,    54,    54,    54,    54,    54,    54,    54,
      77,    78,    85
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
#line 271 "exprparser.y"
    { result = (yyvsp[(1) - (1)].numvalue); ;}
    break;

  case 3:
#line 272 "exprparser.y"
    { result = assignVar((yyvsp[(1) - (3)].var), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 4:
#line 277 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(1) - (1)].numvalue); ;}
    break;

  case 5:
#line 278 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (5)].func), (yyvsp[(1) - (5)].numvalue), (yyvsp[(5) - (5)].numvalue)); ;}
    break;

  case 6:
#line 279 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (3)].func), (yyvsp[(1) - (3)].numvalue), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 7:
#line 280 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (5)].func), (yyvsp[(1) - (5)].numvalue), (yyvsp[(5) - (5)].numvalue)); ;}
    break;

  case 8:
#line 281 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (3)].func), (yyvsp[(1) - (3)].numvalue), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 9:
#line 282 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (5)].func), (yyvsp[(1) - (5)].numvalue), (yyvsp[(5) - (5)].numvalue)); ;}
    break;

  case 10:
#line 283 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (3)].func), (yyvsp[(1) - (3)].numvalue), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 11:
#line 284 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (5)].func), (yyvsp[(1) - (5)].numvalue), (yyvsp[(5) - (5)].numvalue)); ;}
    break;

  case 12:
#line 285 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (3)].func), (yyvsp[(1) - (3)].numvalue), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 13:
#line 286 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (5)].func), (yyvsp[(1) - (5)].numvalue), (yyvsp[(5) - (5)].numvalue)); ;}
    break;

  case 14:
#line 287 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (3)].func), (yyvsp[(1) - (3)].numvalue), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 15:
#line 288 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (5)].func), (yyvsp[(1) - (5)].numvalue), (yyvsp[(5) - (5)].numvalue)); ;}
    break;

  case 16:
#line 289 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (3)].func), (yyvsp[(1) - (3)].numvalue), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 17:
#line 290 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (5)].func), (yyvsp[(1) - (5)].numvalue), (yyvsp[(5) - (5)].numvalue)); ;}
    break;

  case 18:
#line 291 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (3)].func), (yyvsp[(1) - (3)].numvalue), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 19:
#line 292 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (5)].func), (yyvsp[(1) - (5)].numvalue), (yyvsp[(5) - (5)].numvalue)); ;}
    break;

  case 20:
#line 293 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 21:
#line 294 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 22:
#line 295 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 23:
#line 296 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 24:
#line 297 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 25:
#line 298 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 26:
#line 299 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 27:
#line 300 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 28:
#line 304 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(1) - (1)].numvalue); ;}
    break;

  case 29:
#line 305 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (2)].func), (yyvsp[(1) - (2)].numvalue)); ;}
    break;

  case 30:
#line 309 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(1) - (1)].numvalue); ;}
    break;

  case 31:
#line 310 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(2) - (3)].numvalue); ;}
    break;

  case 32:
#line 314 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(1) - (1)].numvalue); ;}
    break;

  case 33:
#line 315 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(1) - (1)].var).d; ;}
    break;

  case 34:
#line 316 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(1) - (1)].numvalue); ;}
    break;

  case 35:
#line 317 "exprparser.y"
    { (yyval.numvalue) = callFunction((yyvsp[(1) - (2)].func), (yyvsp[(2) - (2)].params)); ;}
    break;

  case 36:
#line 321 "exprparser.y"
    { (yyval.func) = (yyvsp[(1) - (1)].func); ;}
    break;

  case 37:
#line 322 "exprparser.y"
    { (yyval.func) = (yyvsp[(1) - (1)].func); ;}
    break;

  case 38:
#line 326 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(1) - (1)].numvalue); ;}
    break;

  case 39:
#line 327 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 40:
#line 332 "exprparser.y"
    { (yyval.params) = (yyvsp[(2) - (3)].params); ;}
    break;

  case 41:
#line 333 "exprparser.y"
    { (yyval.params) = addParam(0, (yyvsp[(1) - (1)].numvalue)); ;}
    break;

  case 42:
#line 337 "exprparser.y"
    { (yyval.params) = 0; ;}
    break;

  case 43:
#line 338 "exprparser.y"
    { (yyval.params) = addParam(0, (yyvsp[(1) - (1)].numvalue)); ;}
    break;

  case 44:
#line 339 "exprparser.y"
    { (yyval.params) = addParam((yyvsp[(1) - (3)].params), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 45:
#line 344 "exprparser.y"
    { (yyval.numvalue) = str2Val((yyvsp[(1) - (1)].string)); ;}
    break;

  case 46:
#line 345 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(1) - (1)].numvalue); ;}
    break;

  case 47:
#line 350 "exprparser.y"
    { (yyval.numvalue) = convertStr((yyvsp[(1) - (1)].numliteral)); ;}
    break;

  case 48:
#line 351 "exprparser.y"
    { (yyval.numvalue) = convertStr((yyvsp[(1) - (1)].numliteral)); ;}
    break;

  case 49:
#line 355 "exprparser.y"
    { (yyval.numliteral) = (yyvsp[(2) - (2)].numliteral); ;}
    break;

  case 50:
#line 356 "exprparser.y"
    { (yyval.numliteral) = (yyvsp[(2) - (2)].numliteral); ;}
    break;

  case 51:
#line 357 "exprparser.y"
    { (yyval.numliteral) = (yyvsp[(2) - (2)].numliteral); ;}
    break;

  case 52:
#line 358 "exprparser.y"
    { (yyval.numliteral) = (yyvsp[(2) - (2)].numliteral); ;}
    break;

  case 53:
#line 359 "exprparser.y"
    { (yyval.numliteral).intpart = (yyvsp[(2) - (2)].seq);
                                           (yyval.numliteral).intpart.complement = 1;
                                           (yyval.numliteral).fracpart = 0;
                                           (yyval.numliteral).exp = initStr(0, 10); ;}
    break;

  case 54:
#line 367 "exprparser.y"
    { (yyval.numliteral).intpart = (yyvsp[(1) - (3)].seq);
                                           (yyval.numliteral).fracpart = (yyvsp[(2) - (3)].seq).digits;
                                           (yyval.numliteral).exp = (yyvsp[(3) - (3)].seq); ;}
    break;

  case 55:
#line 370 "exprparser.y"
    { (yyval.numliteral).intpart = initStr(0, 10);
                                           (yyval.numliteral).fracpart = (yyvsp[(2) - (3)].seq).digits;
                                           (yyval.numliteral).exp = (yyvsp[(3) - (3)].seq); ;}
    break;

  case 56:
#line 377 "exprparser.y"
    { (yyval.numliteral).intpart = (yyvsp[(1) - (3)].seq);
                                           (yyval.numliteral).fracpart = (yyvsp[(2) - (3)].seq).digits;
                                           (yyval.numliteral).exp = (yyvsp[(3) - (3)].seq); ;}
    break;

  case 57:
#line 380 "exprparser.y"
    { (yyval.numliteral).intpart = initStr(0, 2);
                                           (yyval.numliteral).fracpart = (yyvsp[(2) - (3)].seq).digits;
                                           (yyval.numliteral).exp = (yyvsp[(3) - (3)].seq); ;}
    break;

  case 58:
#line 387 "exprparser.y"
    { (yyval.numliteral).intpart = (yyvsp[(1) - (3)].seq);
                                           (yyval.numliteral).fracpart = (yyvsp[(2) - (3)].seq).digits;
                                           (yyval.numliteral).exp = (yyvsp[(3) - (3)].seq); ;}
    break;

  case 59:
#line 390 "exprparser.y"
    { (yyval.numliteral).intpart = initStr(0, 8);
                                           (yyval.numliteral).fracpart = (yyvsp[(2) - (3)].seq).digits;
                                           (yyval.numliteral).exp = (yyvsp[(3) - (3)].seq); ;}
    break;

  case 60:
#line 397 "exprparser.y"
    { (yyval.numliteral).intpart = (yyvsp[(1) - (3)].seq);
                                           (yyval.numliteral).fracpart = (yyvsp[(2) - (3)].seq).digits;
                                           (yyval.numliteral).exp = (yyvsp[(3) - (3)].seq); ;}
    break;

  case 61:
#line 400 "exprparser.y"
    { (yyval.numliteral).intpart = initStr(0, 16);
                                           (yyval.numliteral).fracpart = (yyvsp[(2) - (3)].seq).digits;
                                           (yyval.numliteral).exp = (yyvsp[(3) - (3)].seq); ;}
    break;

  case 62:
#line 407 "exprparser.y"
    { (yyval.seq) = initStr(0, 10); ;}
    break;

  case 63:
#line 408 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 64:
#line 412 "exprparser.y"
    { (yyval.seq) = initStr(0, 2); ;}
    break;

  case 65:
#line 413 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 66:
#line 417 "exprparser.y"
    { (yyval.seq) = initStr(0, 8); ;}
    break;

  case 67:
#line 418 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 68:
#line 422 "exprparser.y"
    { (yyval.seq) = initStr(0, 16); ;}
    break;

  case 69:
#line 423 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 70:
#line 428 "exprparser.y"
    { (yyval.seq) = initStr(0, 10); ;}
    break;

  case 71:
#line 429 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 72:
#line 430 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 73:
#line 437 "exprparser.y"
    { (yyval.seq) = initStr(0, 10); ;}
    break;

  case 74:
#line 438 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 75:
#line 442 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 76:
#line 443 "exprparser.y"
    { (yyval.seq) = (yyvsp[(3) - (3)].seq);
                                           (yyval.seq).sign = (yyvsp[(2) - (3)].sign); ;}
    break;

  case 77:
#line 445 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 80:
#line 455 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 81:
#line 456 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 82:
#line 460 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 83:
#line 461 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 84:
#line 465 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 85:
#line 466 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 86:
#line 467 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 87:
#line 471 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq);
                                           (yyval.seq).complement = 1; ;}
    break;

  case 88:
#line 473 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 89:
#line 477 "exprparser.y"
    { (yyval.seq) = initStr(0, 10); ;}
    break;

  case 90:
#line 478 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 91:
#line 483 "exprparser.y"
    { (yyval.seq) = initStr(0, 2); ;}
    break;

  case 92:
#line 484 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 93:
#line 489 "exprparser.y"
    { (yyval.seq) = initStr(0, 8); ;}
    break;

  case 94:
#line 490 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 95:
#line 495 "exprparser.y"
    { (yyval.seq) = initStr(0, 16); ;}
    break;

  case 96:
#line 496 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 97:
#line 501 "exprparser.y"
    { (yyval.seq) = initStr((yyvsp[(1) - (1)].string), 10); ;}
    break;

  case 98:
#line 502 "exprparser.y"
    { (yyval.seq) = appendStr((yyvsp[(1) - (3)].seq), (yyvsp[(3) - (3)].string)); ;}
    break;

  case 99:
#line 507 "exprparser.y"
    { (yyval.seq) = initStr((yyvsp[(1) - (1)].string), 2); ;}
    break;

  case 100:
#line 508 "exprparser.y"
    { (yyval.seq) = appendStr((yyvsp[(1) - (3)].seq), (yyvsp[(3) - (3)].string)); ;}
    break;

  case 101:
#line 513 "exprparser.y"
    { (yyval.seq) = initStr((yyvsp[(1) - (1)].string), 8); ;}
    break;

  case 102:
#line 514 "exprparser.y"
    { (yyval.seq) = appendStr((yyvsp[(1) - (3)].seq), (yyvsp[(3) - (3)].string)); ;}
    break;

  case 103:
#line 519 "exprparser.y"
    { (yyval.seq) = initStr((yyvsp[(1) - (1)].string), 16); ;}
    break;

  case 104:
#line 520 "exprparser.y"
    { (yyval.seq) = appendStr((yyvsp[(1) - (3)].seq), (yyvsp[(3) - (3)].string)); ;}
    break;

  case 107:
#line 529 "exprparser.y"
    { (yyval.func) = (yyvsp[(1) - (1)].func); ;}
    break;

  case 108:
#line 530 "exprparser.y"
    { (yyval.func) = (yyvsp[(1) - (1)].func); ;}
    break;

  case 109:
#line 531 "exprparser.y"
    { (yyval.func) = (yyvsp[(1) - (1)].func); ;}
    break;

  case 110:
#line 532 "exprparser.y"
    { (yyval.func) = (yyvsp[(1) - (1)].func); ;}
    break;

  case 111:
#line 533 "exprparser.y"
    { (yyval.func) = (yyvsp[(1) - (1)].func); ;}
    break;

  case 112:
#line 534 "exprparser.y"
    { (yyval.func) = (yyvsp[(1) - (1)].func); ;}
    break;

  case 113:
#line 535 "exprparser.y"
    { (yyval.func) = (yyvsp[(1) - (1)].func); ;}
    break;

  case 114:
#line 536 "exprparser.y"
    { (yyval.func) = (yyvsp[(1) - (1)].func); ;}
    break;

  case 126:
#line 556 "exprparser.y"
    { (yyval.sign) = 0   ;}
    break;

  case 127:
#line 557 "exprparser.y"
    { (yyval.sign) = (yyvsp[(1) - (1)].sign); ;}
    break;


/* Line 1267 of yacc.c.  */
#line 2265 "exprparser.c"
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


#line 559 "exprparser.y"


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

