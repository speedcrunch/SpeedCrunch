/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

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
/* Line 1529 of yacc.c.  */
#line 162 "exprparser.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

