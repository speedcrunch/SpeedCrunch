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
     SCALECHAR = 260,
     DECSCALECHAR = 261,
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
     L0 = 284,
     R1 = 285,
     L2 = 286,
     R3 = 287,
     L4 = 288,
     R5 = 289,
     L6 = 290,
     R7 = 291,
     L8 = 292,
     R9 = 293,
     PREFIX10 = 294,
     L10 = 295,
     R11 = 296,
     PREFIX12 = 297,
     L12 = 298,
     R13 = 299,
     PREFIX14 = 300,
     L14 = 301
   };
#endif
/* Tokens.  */
#define UNKNOWNTOKEN 258
#define TEXT 259
#define SCALECHAR 260
#define DECSCALECHAR 261
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
#define L0 284
#define R1 285
#define L2 286
#define R3 287
#define L4 288
#define R5 289
#define L6 290
#define R7 291
#define L8 292
#define R9 293
#define PREFIX10 294
#define L10 295
#define R11 296
#define PREFIX12 297
#define L12 298
#define R13 299
#define PREFIX14 300
#define L14 301




/* Copy the first part of user declarations.  */
#line 45 "exprparser.y"

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
#line 71 "exprparser.y"
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
#line 222 "exprparser.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 235 "exprparser.c"

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
#define YYFINAL  89
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   410

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  47
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  40
/* YYNRULES -- Number of rules.  */
#define YYNRULES  133
/* YYNRULES -- Number of states.  */
#define YYNSTATES  261

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   301

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
      45,    46
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     9,    11,    17,    21,    27,    31,
      37,    41,    47,    51,    57,    61,    67,    71,    77,    81,
      87,    92,    97,   102,   107,   112,   117,   122,   127,   132,
     137,   142,   147,   152,   157,   162,   167,   172,   177,   182,
     187,   192,   197,   202,   207,   209,   212,   214,   218,   220,
     222,   224,   227,   229,   231,   233,   238,   242,   244,   245,
     247,   251,   253,   255,   257,   259,   262,   265,   268,   271,
     274,   278,   282,   286,   290,   294,   298,   302,   306,   307,
     310,   311,   314,   315,   318,   319,   322,   323,   326,   329,
     330,   333,   338,   340,   342,   344,   346,   349,   351,   354,
     357,   360,   363,   365,   366,   368,   369,   371,   372,   374,
     375,   377,   379,   383,   385,   389,   391,   395,   397,   401,
     403,   405,   407,   409,   411,   412,   414,   416,   418,   420,
     422,   424,   426,   428
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      48,     0,    -1,    49,    -1,    24,    28,    49,    -1,    50,
      -1,    49,    29,    83,    23,    49,    -1,    49,    30,    49,
      -1,    49,    31,    83,    23,    49,    -1,    49,    32,    49,
      -1,    49,    33,    83,    23,    49,    -1,    49,    34,    49,
      -1,    49,    35,    83,    23,    49,    -1,    49,    36,    49,
      -1,    49,    37,    83,    23,    49,    -1,    49,    38,    49,
      -1,    49,    40,    83,    23,    49,    -1,    49,    41,    49,
      -1,    49,    43,    83,    23,    49,    -1,    49,    44,    49,
      -1,    49,    46,    83,    23,    49,    -1,    29,    45,    23,
      49,    -1,    31,    45,    23,    49,    -1,    33,    45,    23,
      49,    -1,    35,    45,    23,    49,    -1,    37,    45,    23,
      49,    -1,    40,    45,    23,    49,    -1,    43,    45,    23,
      49,    -1,    46,    45,    23,    49,    -1,    29,    42,    23,
      49,    -1,    31,    42,    23,    49,    -1,    33,    42,    23,
      49,    -1,    35,    42,    23,    49,    -1,    37,    42,    23,
      49,    -1,    40,    42,    23,    49,    -1,    43,    42,    23,
      49,    -1,    46,    42,    23,    49,    -1,    29,    39,    23,
      49,    -1,    31,    39,    23,    49,    -1,    33,    39,    23,
      49,    -1,    35,    39,    23,    49,    -1,    37,    39,    23,
      49,    -1,    40,    39,    23,    49,    -1,    43,    39,    23,
      49,    -1,    46,    39,    23,    49,    -1,    51,    -1,    50,
      53,    -1,    52,    -1,    13,    49,    14,    -1,    57,    -1,
      24,    -1,    25,    -1,    27,    55,    -1,    26,    -1,    21,
      -1,    52,    -1,    86,    84,    23,    54,    -1,    13,    56,
      14,    -1,    54,    -1,    -1,    49,    -1,    56,    20,    49,
      -1,     4,    -1,    58,    -1,    60,    -1,    59,    -1,     7,
      60,    -1,     8,    61,    -1,     9,    62,    -1,    10,    63,
      -1,    11,    73,    -1,    79,    64,    68,    -1,    12,    79,
      68,    -1,    80,    65,    69,    -1,    12,    80,    69,    -1,
      81,    66,    69,    -1,    12,    81,    69,    -1,    82,    67,
      69,    -1,    12,    82,    69,    -1,    -1,    12,    75,    -1,
      -1,    12,    76,    -1,    -1,    12,    77,    -1,    -1,    12,
      78,    -1,    -1,     6,    70,    -1,     5,    70,    -1,    -1,
       5,    70,    -1,    86,    83,    23,    71,    -1,    79,    -1,
      74,    -1,    79,    -1,    72,    -1,     7,    79,    -1,    73,
      -1,     8,    80,    -1,     9,    81,    -1,    10,    82,    -1,
      11,    73,    -1,    72,    -1,    -1,    79,    -1,    -1,    80,
      -1,    -1,    81,    -1,    -1,    82,    -1,    15,    -1,    79,
      85,    15,    -1,    16,    -1,    80,    85,    16,    -1,    17,
      -1,    81,    85,    17,    -1,    18,    -1,    82,    85,    18,
      -1,    22,    -1,    84,    -1,    39,    -1,    42,    -1,    45,
      -1,    -1,    19,    -1,    29,    -1,    31,    -1,    33,    -1,
      35,    -1,    37,    -1,    40,    -1,    43,    -1,    46,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   255,   255,   256,   267,   268,   269,   270,   271,   272,
     273,   274,   275,   276,   277,   278,   279,   280,   281,   282,
     283,   284,   285,   286,   287,   288,   289,   290,   291,   292,
     293,   294,   295,   296,   297,   298,   299,   300,   301,   302,
     303,   304,   305,   306,   310,   311,   315,   316,   320,   321,
     322,   323,   327,   328,   332,   333,   338,   339,   343,   344,
     345,   350,   351,   356,   357,   361,   362,   363,   364,   365,
     373,   376,   383,   386,   393,   396,   403,   406,   413,   414,
     418,   419,   423,   424,   428,   429,   434,   435,   436,   443,
     444,   448,   450,   451,   454,   455,   459,   460,   464,   465,
     466,   470,   472,   476,   477,   482,   483,   488,   489,   494,
     495,   500,   501,   506,   507,   512,   513,   518,   519,   523,
     524,   528,   529,   530,   533,   535,   539,   540,   541,   542,
     543,   544,   545,   546
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "UNKNOWNTOKEN", "TEXT", "SCALECHAR",
  "DECSCALECHAR", "DECTAG", "BINTAG", "OCTTAG", "HEXTAG", "CMPLTAG", "DOT",
  "OPENPAR", "CLOSEPAR", "DECSEQ", "BINSEQ", "OCTSEQ", "HEXSEQ",
  "GROUPCHAR", "SEP", "PERCENT", "NOPREFIX", "SIGN", "VARIABLE",
  "CONSTANT", "POSTFIXOP", "FUNCTION", "ASSIGN", "L0", "R1", "L2", "R3",
  "L4", "R5", "L6", "R7", "L8", "R9", "PREFIX10", "L10", "R11", "PREFIX12",
  "L12", "R13", "PREFIX14", "L14", "$accept", "input", "expr",
  "postfixval", "primval", "simpleval", "postfixop", "prefixval", "params",
  "paramlist", "literal", "number", "taggednumber", "decvalue", "binvalue",
  "octvalue", "hexvalue", "optdecdotfrac", "optbindotfrac",
  "optoctdotfrac", "opthexdotfrac", "optdecscale", "optbase2scale",
  "exponent", "uint", "taggedseq", "taggedbase2seq", "taggedint",
  "optdecseq", "optbinseq", "optoctseq", "opthexseq", "decseq", "binseq",
  "octseq", "hexseq", "prefixprec", "prefixop", "optgroupchar",
  "infixprec", 0
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
     295,   296,   297,   298,   299,   300,   301
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    47,    48,    48,    49,    49,    49,    49,    49,    49,
      49,    49,    49,    49,    49,    49,    49,    49,    49,    49,
      49,    49,    49,    49,    49,    49,    49,    49,    49,    49,
      49,    49,    49,    49,    49,    49,    49,    49,    49,    49,
      49,    49,    49,    49,    50,    50,    51,    51,    52,    52,
      52,    52,    53,    53,    54,    54,    55,    55,    56,    56,
      56,    57,    57,    58,    58,    59,    59,    59,    59,    59,
      60,    60,    61,    61,    62,    62,    63,    63,    64,    64,
      65,    65,    66,    66,    67,    67,    68,    68,    68,    69,
      69,    70,    70,    70,    71,    71,    72,    72,    73,    73,
      73,    74,    74,    75,    75,    76,    76,    77,    77,    78,
      78,    79,    79,    80,    80,    81,    81,    82,    82,    83,
      83,    84,    84,    84,    85,    85,    86,    86,    86,    86,
      86,    86,    86,    86
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     3,     1,     5,     3,     5,     3,     5,
       3,     5,     3,     5,     3,     5,     3,     5,     3,     5,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     1,     2,     1,     3,     1,     1,
       1,     2,     1,     1,     1,     4,     3,     1,     0,     1,
       3,     1,     1,     1,     1,     2,     2,     2,     2,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     0,     2,
       0,     2,     0,     2,     0,     2,     0,     2,     2,     0,
       2,     4,     1,     1,     1,     1,     2,     1,     2,     2,
       2,     2,     1,     0,     1,     0,     1,     0,     1,     0,
       1,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       1,     1,     1,     1,     0,     1,     1,     1,     1,     1,
       1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    61,     0,     0,     0,     0,     0,     0,     0,   111,
      49,    50,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     2,     4,    44,    46,    48,    62,    64,    63,
      78,    65,     0,   113,    66,    80,     0,   115,    67,    82,
       0,   117,    68,    84,     0,     0,     0,    69,    86,    49,
       0,     0,    58,   126,   127,   128,   129,   130,   131,   132,
     133,    54,    57,    51,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     1,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    53,    52,    45,   103,   125,
      86,     0,    89,   105,    89,     0,    89,   107,    89,     0,
      89,   109,    89,     0,    98,    99,   100,     0,     0,    71,
      47,     3,    59,     0,   121,   122,   123,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   119,     0,   120,     6,     0,     8,     0,    10,
       0,    12,     0,    14,     0,    16,     0,    18,     0,    79,
     104,    70,   112,     0,    73,    81,   106,    72,   114,    75,
      83,   108,    74,   116,    77,    85,   110,    76,   118,     0,
       0,    88,   102,    97,    93,    92,     0,    87,    56,     0,
       0,    36,    28,    20,    37,    29,    21,    38,    30,    22,
      39,    31,    23,    40,    32,    24,    41,    33,    25,    42,
      34,    26,    43,    35,    27,     0,     0,     0,     0,     0,
       0,     0,     0,    90,    96,   101,     0,    60,    55,     5,
       7,     9,    11,    13,    15,    17,    19,     0,    91,    95,
      94
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    21,    22,    23,    24,    25,   107,    62,    63,   133,
      26,    27,    28,    29,    34,    38,    42,   110,   114,   118,
     122,   129,   184,   201,   258,   202,   203,   204,   179,   185,
     190,   195,    30,    35,    39,    43,   163,   164,   111,   206
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -124
static const yytype_int16 yypact[] =
{
      24,  -124,   112,   150,    26,    64,   186,    32,   233,  -124,
      30,  -124,   273,   177,   182,   210,   214,   230,   248,   250,
     270,    92,   349,    58,  -124,  -124,  -124,  -124,  -124,  -124,
     178,  -124,    99,  -124,  -124,   193,   113,  -124,  -124,   201,
     155,  -124,  -124,    44,    99,   113,   155,  -124,    35,  -124,
     331,   233,   233,  -124,  -124,  -124,  -124,  -124,  -124,  -124,
    -124,  -124,  -124,  -124,   287,   192,   203,   205,   224,   228,
     231,   238,   240,   242,   244,   251,   255,   268,   271,   276,
     278,   280,   282,   284,   288,   291,   295,   304,   307,  -124,
     -16,   233,   -16,   233,   -16,   233,   -16,   233,   -16,   233,
     -16,   233,   -16,   233,   -16,  -124,  -124,  -124,    32,  -124,
      63,   256,     6,    99,   330,   323,    25,   113,   330,   279,
      47,   155,   330,   325,   191,   144,   220,   171,   171,  -124,
    -124,   349,   349,    98,  -124,  -124,  -124,   318,   233,   233,
     233,   233,   233,   233,   233,   233,   233,   233,   233,   233,
     233,   233,   233,   233,   233,   233,   233,   233,   233,   233,
     233,   233,  -124,   324,  -124,    73,   326,   364,   328,    37,
     329,   131,   332,    85,   334,   290,   335,   179,   347,  -124,
     169,  -124,  -124,   171,  -124,  -124,   191,  -124,  -124,  -124,
    -124,   144,  -124,  -124,  -124,  -124,   220,  -124,  -124,    32,
     186,  -124,  -124,  -124,  -124,   169,   -16,  -124,  -124,   233,
     313,   290,   179,  -124,   290,   179,  -124,   290,   179,  -124,
     290,   179,  -124,   290,   179,  -124,   290,   179,  -124,   290,
     179,  -124,   290,   179,  -124,   233,   233,   233,   233,   233,
     233,   233,   233,  -124,   169,  -124,   350,   349,  -124,    73,
     364,    37,   131,    85,   290,   179,  -124,   149,  -124,  -124,
     169
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -124,  -124,    -6,  -124,  -124,   -11,  -124,   166,  -124,  -124,
    -124,  -124,  -124,   352,  -124,  -124,  -124,  -124,  -124,  -124,
    -124,   281,    69,  -123,  -124,   137,    -2,  -124,  -124,  -124,
    -124,  -124,    -7,   -25,   -21,   -19,   -84,   339,   -26,    -9
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -125
static const yytype_int16 yytable[] =
{
      48,    61,    50,    64,    47,   207,   162,   112,   166,   115,
     168,   183,   170,   119,   172,   116,   174,   123,   176,   124,
     178,   120,  -124,   134,   125,   109,   135,   126,     1,   136,
     183,     2,     3,     4,     5,     6,     7,     8,    36,     9,
     127,   128,  -124,    37,   109,   131,   132,     9,    10,    11,
    -124,    12,   183,    13,   109,    14,   121,    15,    51,    16,
     243,    17,  -124,   109,    18,  -124,   109,    19,   127,   128,
      20,    95,    96,    97,    98,    99,    40,   100,   101,   105,
     102,   103,    41,   104,   106,   165,   115,   167,   186,   169,
     119,   171,    89,   173,   123,   175,   191,   177,   115,   119,
     123,   180,   196,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   208,   100,   101,    33,   102,   103,   209,   104,
     205,   205,   246,    99,     7,   100,   101,     9,   102,   103,
      37,   104,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   199,    44,    45,    46,
     115,  -124,    32,   109,     9,   119,    33,    97,    98,    99,
     123,   100,   101,    41,   102,   103,   205,   104,   199,    44,
      45,    46,   200,   187,  -124,   189,     9,   192,   109,   194,
     108,   197,   244,  -124,    44,    45,    46,   109,   245,    61,
      53,    64,    54,   247,    55,   113,    56,  -124,    57,  -124,
     109,    58,   109,   117,    59,   138,    65,    60,  -124,    66,
     109,    68,    67,   103,    69,   104,   139,    70,   140,   249,
     250,   251,   252,   253,   254,   255,   256,     1,  -124,   109,
       2,     3,     4,     5,     6,     7,     8,   141,     9,    71,
     260,   142,    72,    74,   143,    73,    75,    49,    11,    76,
      12,   144,    13,   145,    14,   146,    15,   147,    16,    77,
      17,   182,    78,    18,   148,    79,    19,     1,   149,    20,
       2,     3,     4,     5,     6,     7,    52,    80,     9,    83,
      81,   150,    84,    82,   151,    85,   193,    49,    11,   152,
      12,   153,    53,   154,    54,   155,    55,   156,    56,    86,
      57,   157,    87,    58,   158,    88,    59,     1,   159,    60,
       2,     3,     4,     5,     6,     7,   134,   160,     9,   135,
     161,   101,   136,   102,   103,   183,   104,    49,    11,   188,
      12,   210,    53,   198,    54,   130,    55,   235,    56,   236,
      57,   237,   238,    58,    31,   239,    59,   240,   241,    60,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     242,   100,   101,   257,   102,   103,   248,   104,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,     0,   100,
     101,   181,   102,   103,   259,   104,    93,    94,    95,    96,
      97,    98,    99,   137,   100,   101,     0,   102,   103,     0,
     104
};

static const yytype_int16 yycheck[] =
{
       7,    12,     8,    12,     6,   128,    22,    32,    92,    35,
      94,     5,    96,    39,    98,    36,   100,    43,   102,    44,
     104,    40,    16,    39,    45,    19,    42,    46,     4,    45,
       5,     7,     8,     9,    10,    11,    12,    13,    12,    15,
       5,     6,    17,    17,    19,    51,    52,    15,    24,    25,
      15,    27,     5,    29,    19,    31,    12,    33,    28,    35,
     183,    37,    18,    19,    40,    18,    19,    43,     5,     6,
      46,    34,    35,    36,    37,    38,    12,    40,    41,    21,
      43,    44,    18,    46,    26,    91,   112,    93,   113,    95,
     116,    97,     0,    99,   120,   101,   117,   103,   124,   125,
     126,   108,   121,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    14,    40,    41,    16,    43,    44,    20,    46,
     127,   128,   206,    38,    12,    40,    41,    15,    43,    44,
      17,    46,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,     7,     8,     9,    10,
     186,    17,    12,    19,    15,   191,    16,    36,    37,    38,
     196,    40,    41,    18,    43,    44,   183,    46,     7,     8,
       9,    10,    11,   114,    15,   116,    15,   118,    19,   120,
      12,   122,   199,    15,     8,     9,    10,    19,   200,   210,
      29,   210,    31,   209,    33,    12,    35,    16,    37,    16,
      19,    40,    19,    12,    43,    23,    39,    46,    17,    42,
      19,    39,    45,    44,    42,    46,    23,    45,    23,   235,
     236,   237,   238,   239,   240,   241,   242,     4,    18,    19,
       7,     8,     9,    10,    11,    12,    13,    23,    15,    39,
     257,    23,    42,    39,    23,    45,    42,    24,    25,    45,
      27,    23,    29,    23,    31,    23,    33,    23,    35,    39,
      37,    15,    42,    40,    23,    45,    43,     4,    23,    46,
       7,     8,     9,    10,    11,    12,    13,    39,    15,    39,
      42,    23,    42,    45,    23,    45,    17,    24,    25,    23,
      27,    23,    29,    23,    31,    23,    33,    23,    35,    39,
      37,    23,    42,    40,    23,    45,    43,     4,    23,    46,
       7,     8,     9,    10,    11,    12,    39,    23,    15,    42,
      23,    41,    45,    43,    44,     5,    46,    24,    25,    16,
      27,    23,    29,    18,    31,    14,    33,    23,    35,    23,
      37,    23,    23,    40,     2,    23,    43,    23,    23,    46,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      23,    40,    41,    23,    43,    44,   210,    46,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    -1,    40,
      41,   110,    43,    44,   257,    46,    32,    33,    34,    35,
      36,    37,    38,    64,    40,    41,    -1,    43,    44,    -1,
      46
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     4,     7,     8,     9,    10,    11,    12,    13,    15,
      24,    25,    27,    29,    31,    33,    35,    37,    40,    43,
      46,    48,    49,    50,    51,    52,    57,    58,    59,    60,
      79,    60,    12,    16,    61,    80,    12,    17,    62,    81,
      12,    18,    63,    82,     8,     9,    10,    73,    79,    24,
      49,    28,    13,    29,    31,    33,    35,    37,    40,    43,
      46,    52,    54,    55,    86,    39,    42,    45,    39,    42,
      45,    39,    42,    45,    39,    42,    45,    39,    42,    45,
      39,    42,    45,    39,    42,    45,    39,    42,    45,     0,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      40,    41,    43,    44,    46,    21,    26,    53,    12,    19,
      64,    85,    80,    12,    65,    85,    81,    12,    66,    85,
      82,    12,    67,    85,    80,    81,    82,     5,     6,    68,
      14,    49,    49,    56,    39,    42,    45,    84,    23,    23,
      23,    23,    23,    23,    23,    23,    23,    23,    23,    23,
      23,    23,    23,    23,    23,    23,    23,    23,    23,    23,
      23,    23,    22,    83,    84,    49,    83,    49,    83,    49,
      83,    49,    83,    49,    83,    49,    83,    49,    83,    75,
      79,    68,    15,     5,    69,    76,    80,    69,    16,    69,
      77,    81,    69,    17,    69,    78,    82,    69,    18,     7,
      11,    70,    72,    73,    74,    79,    86,    70,    14,    20,
      23,    49,    49,    49,    49,    49,    49,    49,    49,    49,
      49,    49,    49,    49,    49,    49,    49,    49,    49,    49,
      49,    49,    49,    49,    49,    23,    23,    23,    23,    23,
      23,    23,    23,    70,    79,    73,    83,    49,    54,    49,
      49,    49,    49,    49,    49,    49,    49,    23,    71,    72,
      79
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
#line 255 "exprparser.y"
    { result = (yyvsp[(1) - (1)].numvalue); ;}
    break;

  case 3:
#line 256 "exprparser.y"
    { result = assignVar((yyvsp[(1) - (3)].var), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 4:
#line 267 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(1) - (1)].numvalue); ;}
    break;

  case 5:
#line 268 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (5)].func), (yyvsp[(1) - (5)].numvalue), (yyvsp[(5) - (5)].numvalue)); ;}
    break;

  case 6:
#line 269 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (3)].func), (yyvsp[(1) - (3)].numvalue), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 7:
#line 270 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (5)].func), (yyvsp[(1) - (5)].numvalue), (yyvsp[(5) - (5)].numvalue)); ;}
    break;

  case 8:
#line 271 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (3)].func), (yyvsp[(1) - (3)].numvalue), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 9:
#line 272 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (5)].func), (yyvsp[(1) - (5)].numvalue), (yyvsp[(5) - (5)].numvalue)); ;}
    break;

  case 10:
#line 273 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (3)].func), (yyvsp[(1) - (3)].numvalue), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 11:
#line 274 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (5)].func), (yyvsp[(1) - (5)].numvalue), (yyvsp[(5) - (5)].numvalue)); ;}
    break;

  case 12:
#line 275 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (3)].func), (yyvsp[(1) - (3)].numvalue), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 13:
#line 276 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (5)].func), (yyvsp[(1) - (5)].numvalue), (yyvsp[(5) - (5)].numvalue)); ;}
    break;

  case 14:
#line 277 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (3)].func), (yyvsp[(1) - (3)].numvalue), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 15:
#line 278 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (5)].func), (yyvsp[(1) - (5)].numvalue), (yyvsp[(5) - (5)].numvalue)); ;}
    break;

  case 16:
#line 279 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (3)].func), (yyvsp[(1) - (3)].numvalue), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 17:
#line 280 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (5)].func), (yyvsp[(1) - (5)].numvalue), (yyvsp[(5) - (5)].numvalue)); ;}
    break;

  case 18:
#line 281 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (3)].func), (yyvsp[(1) - (3)].numvalue), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 19:
#line 282 "exprparser.y"
    { (yyval.numvalue) = callBinOperator((yyvsp[(2) - (5)].func), (yyvsp[(1) - (5)].numvalue), (yyvsp[(5) - (5)].numvalue)); ;}
    break;

  case 20:
#line 283 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 21:
#line 284 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 22:
#line 285 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 23:
#line 286 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 24:
#line 287 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 25:
#line 288 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 26:
#line 289 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 27:
#line 290 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 28:
#line 291 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 29:
#line 292 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 30:
#line 293 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 31:
#line 294 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 32:
#line 295 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 33:
#line 296 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 34:
#line 297 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 35:
#line 298 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 36:
#line 299 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 37:
#line 300 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 38:
#line 301 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 39:
#line 302 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 40:
#line 303 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 41:
#line 304 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 42:
#line 305 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 43:
#line 306 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 44:
#line 310 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(1) - (1)].numvalue); ;}
    break;

  case 45:
#line 311 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (2)].func), (yyvsp[(1) - (2)].numvalue)); ;}
    break;

  case 46:
#line 315 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(1) - (1)].numvalue); ;}
    break;

  case 47:
#line 316 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(2) - (3)].numvalue); ;}
    break;

  case 48:
#line 320 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(1) - (1)].numvalue); ;}
    break;

  case 49:
#line 321 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(1) - (1)].var).d; ;}
    break;

  case 50:
#line 322 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(1) - (1)].numvalue); ;}
    break;

  case 51:
#line 323 "exprparser.y"
    { (yyval.numvalue) = callFunction((yyvsp[(1) - (2)].func), (yyvsp[(2) - (2)].params)); ;}
    break;

  case 52:
#line 327 "exprparser.y"
    { (yyval.func) = (yyvsp[(1) - (1)].func); ;}
    break;

  case 53:
#line 328 "exprparser.y"
    { (yyval.func) = (yyvsp[(1) - (1)].func); ;}
    break;

  case 54:
#line 332 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(1) - (1)].numvalue); ;}
    break;

  case 55:
#line 333 "exprparser.y"
    { (yyval.numvalue) = callUnaryOperator((yyvsp[(2) - (4)].func), (yyvsp[(4) - (4)].numvalue)); ;}
    break;

  case 56:
#line 338 "exprparser.y"
    { (yyval.params) = (yyvsp[(2) - (3)].params); ;}
    break;

  case 57:
#line 339 "exprparser.y"
    { (yyval.params) = addParam(0, (yyvsp[(1) - (1)].numvalue)); ;}
    break;

  case 58:
#line 343 "exprparser.y"
    { (yyval.params) = 0; ;}
    break;

  case 59:
#line 344 "exprparser.y"
    { (yyval.params) = addParam(0, (yyvsp[(1) - (1)].numvalue)); ;}
    break;

  case 60:
#line 345 "exprparser.y"
    { (yyval.params) = addParam((yyvsp[(1) - (3)].params), (yyvsp[(3) - (3)].numvalue)); ;}
    break;

  case 61:
#line 350 "exprparser.y"
    { (yyval.numvalue) = str2Val((yyvsp[(1) - (1)].string)); ;}
    break;

  case 62:
#line 351 "exprparser.y"
    { (yyval.numvalue) = (yyvsp[(1) - (1)].numvalue); ;}
    break;

  case 63:
#line 356 "exprparser.y"
    { (yyval.numvalue) = convertStr((yyvsp[(1) - (1)].numliteral)); ;}
    break;

  case 64:
#line 357 "exprparser.y"
    { (yyval.numvalue) = convertStr((yyvsp[(1) - (1)].numliteral)); ;}
    break;

  case 65:
#line 361 "exprparser.y"
    { (yyval.numliteral) = (yyvsp[(2) - (2)].numliteral); ;}
    break;

  case 66:
#line 362 "exprparser.y"
    { (yyval.numliteral) = (yyvsp[(2) - (2)].numliteral); ;}
    break;

  case 67:
#line 363 "exprparser.y"
    { (yyval.numliteral) = (yyvsp[(2) - (2)].numliteral); ;}
    break;

  case 68:
#line 364 "exprparser.y"
    { (yyval.numliteral) = (yyvsp[(2) - (2)].numliteral); ;}
    break;

  case 69:
#line 365 "exprparser.y"
    { (yyval.numliteral).intpart = (yyvsp[(2) - (2)].seq);
                                           (yyval.numliteral).intpart.complement = 1;
                                           (yyval.numliteral).fracpart = 0;
                                           (yyval.numliteral).exp = initStr(0, 0); ;}
    break;

  case 70:
#line 373 "exprparser.y"
    { (yyval.numliteral).intpart = (yyvsp[(1) - (3)].seq);
                                           (yyval.numliteral).fracpart = (yyvsp[(2) - (3)].seq).digits;
                                           (yyval.numliteral).exp = (yyvsp[(3) - (3)].seq); ;}
    break;

  case 71:
#line 376 "exprparser.y"
    { (yyval.numliteral).intpart = initStr(0, 10);
                                           (yyval.numliteral).fracpart = (yyvsp[(2) - (3)].seq).digits;
                                           (yyval.numliteral).exp = (yyvsp[(3) - (3)].seq); ;}
    break;

  case 72:
#line 383 "exprparser.y"
    { (yyval.numliteral).intpart = (yyvsp[(1) - (3)].seq);
                                           (yyval.numliteral).fracpart = (yyvsp[(2) - (3)].seq).digits;
                                           (yyval.numliteral).exp = (yyvsp[(3) - (3)].seq); ;}
    break;

  case 73:
#line 386 "exprparser.y"
    { (yyval.numliteral).intpart = initStr(0, 2);
                                           (yyval.numliteral).fracpart = (yyvsp[(2) - (3)].seq).digits;
                                           (yyval.numliteral).exp = (yyvsp[(3) - (3)].seq); ;}
    break;

  case 74:
#line 393 "exprparser.y"
    { (yyval.numliteral).intpart = (yyvsp[(1) - (3)].seq);
                                           (yyval.numliteral).fracpart = (yyvsp[(2) - (3)].seq).digits;
                                           (yyval.numliteral).exp = (yyvsp[(3) - (3)].seq); ;}
    break;

  case 75:
#line 396 "exprparser.y"
    { (yyval.numliteral).intpart = initStr(0, 8);
                                           (yyval.numliteral).fracpart = (yyvsp[(2) - (3)].seq).digits;
                                           (yyval.numliteral).exp = (yyvsp[(3) - (3)].seq); ;}
    break;

  case 76:
#line 403 "exprparser.y"
    { (yyval.numliteral).intpart = (yyvsp[(1) - (3)].seq);
                                           (yyval.numliteral).fracpart = (yyvsp[(2) - (3)].seq).digits;
                                           (yyval.numliteral).exp = (yyvsp[(3) - (3)].seq); ;}
    break;

  case 77:
#line 406 "exprparser.y"
    { (yyval.numliteral).intpart = initStr(0, 16);
                                           (yyval.numliteral).fracpart = (yyvsp[(2) - (3)].seq).digits;
                                           (yyval.numliteral).exp = (yyvsp[(3) - (3)].seq); ;}
    break;

  case 78:
#line 413 "exprparser.y"
    { (yyval.seq) = initStr(0, 10); ;}
    break;

  case 79:
#line 414 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 80:
#line 418 "exprparser.y"
    { (yyval.seq) = initStr(0, 2); ;}
    break;

  case 81:
#line 419 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 82:
#line 423 "exprparser.y"
    { (yyval.seq) = initStr(0, 8); ;}
    break;

  case 83:
#line 424 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 84:
#line 428 "exprparser.y"
    { (yyval.seq) = initStr(0, 16); ;}
    break;

  case 85:
#line 429 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 86:
#line 434 "exprparser.y"
    { initStr(0, 0); ;}
    break;

  case 87:
#line 435 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 88:
#line 436 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 89:
#line 443 "exprparser.y"
    { initStr(0, 0); ;}
    break;

  case 90:
#line 444 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 91:
#line 448 "exprparser.y"
    { (yyval.seq) = (yyvsp[(4) - (4)].seq);
                                           (yyval.seq).sign = (yyvsp[(3) - (4)].sign); ;}
    break;

  case 92:
#line 450 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 93:
#line 451 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 94:
#line 454 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 95:
#line 455 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 96:
#line 459 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 97:
#line 460 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 98:
#line 464 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 99:
#line 465 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 100:
#line 466 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq); ;}
    break;

  case 101:
#line 470 "exprparser.y"
    { (yyval.seq) = (yyvsp[(2) - (2)].seq);
                                           (yyval.seq).complement = 1; ;}
    break;

  case 102:
#line 472 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 103:
#line 476 "exprparser.y"
    { (yyval.seq) = initStr(0, 10); ;}
    break;

  case 104:
#line 477 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 105:
#line 482 "exprparser.y"
    { (yyval.seq) = initStr(0, 2); ;}
    break;

  case 106:
#line 483 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 107:
#line 488 "exprparser.y"
    { (yyval.seq) = initStr(0, 8); ;}
    break;

  case 108:
#line 489 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 109:
#line 494 "exprparser.y"
    { (yyval.seq) = initStr(0, 16); ;}
    break;

  case 110:
#line 495 "exprparser.y"
    { (yyval.seq) = (yyvsp[(1) - (1)].seq); ;}
    break;

  case 111:
#line 500 "exprparser.y"
    { (yyval.seq) = initStr((yyvsp[(1) - (1)].string), 10); ;}
    break;

  case 112:
#line 501 "exprparser.y"
    { (yyval.seq) = appendStr((yyvsp[(1) - (3)].seq), (yyvsp[(3) - (3)].string)); ;}
    break;

  case 113:
#line 506 "exprparser.y"
    { (yyval.seq) = initStr((yyvsp[(1) - (1)].string), 2); ;}
    break;

  case 114:
#line 507 "exprparser.y"
    { (yyval.seq) = appendStr((yyvsp[(1) - (3)].seq), (yyvsp[(3) - (3)].string)); ;}
    break;

  case 115:
#line 512 "exprparser.y"
    { (yyval.seq) = initStr((yyvsp[(1) - (1)].string), 8); ;}
    break;

  case 116:
#line 513 "exprparser.y"
    { (yyval.seq) = appendStr((yyvsp[(1) - (3)].seq), (yyvsp[(3) - (3)].string)); ;}
    break;

  case 117:
#line 518 "exprparser.y"
    { (yyval.seq) = initStr((yyvsp[(1) - (1)].string), 16); ;}
    break;

  case 118:
#line 519 "exprparser.y"
    { (yyval.seq) = appendStr((yyvsp[(1) - (3)].seq), (yyvsp[(3) - (3)].string)); ;}
    break;

  case 121:
#line 528 "exprparser.y"
    { (yyval.func) = (yyvsp[(1) - (1)].func); ;}
    break;

  case 122:
#line 529 "exprparser.y"
    { (yyval.func) = (yyvsp[(1) - (1)].func); ;}
    break;

  case 123:
#line 530 "exprparser.y"
    { (yyval.func) = (yyvsp[(1) - (1)].func); ;}
    break;


/* Line 1267 of yacc.c.  */
#line 2317 "exprparser.c"
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


#line 548 "exprparser.y"


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

