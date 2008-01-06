/* bison.h: C interface to bison parser */
/*
    Copyright (C) 2007 Wolf Lammen.

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

#ifndef _BISON_H
#define _BISON_H

#ifdef __cplusplus

class SafeVariant;
class SafeQString;
class SafeParamList;
class Symbol;

extern "C" {

#else

# define SafeVariant void
# define SafeParamList void
# define Symbol void
# define SafeQString void

#endif /* __cplusplus */

/* return values of parse */
#define PARSE_OK          0
#define PARSE_SYNTAXERROR 1
#define PARSE_OUTOFMEM    2

typedef SafeQString* String;

typedef struct DigitSeq
{
  String digits;
  char  base;
  char  complement;
  signed char sign;
} DigitSeq;

typedef struct NumLiteral
{
  DigitSeq intpart;
  String fracpart;
  DigitSeq exp;
} NumLiteral;

typedef struct NumValue
{
  SafeVariant* val;
  char percent;
} NumValue;

typedef SafeParamList* Params;

typedef const Symbol* Func;

typedef struct Var
{
  void* write;
  NumValue d;
} Var;

#ifndef YYSTYPE_IS_DECLARED
# include "exprparser.h"
#endif

/* call backs */

typedef NumValue (*FStr2Val)(String s);
typedef DigitSeq (*FAppendStr)(DigitSeq seq, String s);
typedef NumValue (*FConvertStr)(NumLiteral text);
typedef Params   (*FAddParam)(Params list, NumValue val);
typedef NumValue (*FCallFunction)(Func f, Params params);
typedef NumValue (*FAssignVar)(Var variable, NumValue val);
/* return values and YYSTYPE are defined in exprparser.h */
typedef int      (*FGetToken)(YYSTYPE* val, int* pos, int* lg);

typedef struct CallBacks
{
  FAppendStr    appendStr;
  FConvertStr   convertStr;
  FAddParam     addParam;
  FCallFunction callFunction;
  FAssignVar    assignVar;
  FGetToken     getToken;
  FStr2Val      str2Val;
} CallBacks;

/* parser. return values are defined above as PARSE_... */
int parseexpr(CallBacks callbacks, NumValue* result, int* pos, int* lg);

#ifdef __cplusplus
}
#endif

#endif /* _BISON_H */
