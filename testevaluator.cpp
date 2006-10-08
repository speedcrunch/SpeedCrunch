/* This file is part of the SpeedCrunch project
   Copyright (C) 2004-2006 Ariya Hidayat <ariya@kde.org>

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


#include <iostream>
#include <string.h>

#include <QtCore>

#include "hmath.h"
#include "evaluator.h"
#include "functions.h"

static int eval_total_tests = 0;
static int eval_failed_tests = 0;

#define CHECK_EVAL(x,y)  checkEval(__FILE__,__LINE__,#x,x,y)
#define CHECK_EVAL_PRECISE(x,y)  checkEvalPrecise(__FILE__,__LINE__,#x,x,y)

static void checkEval( const char *file, int line, const char* msg, 
const QString& expr, const char* expected )
{
  eval_total_tests++;

  Evaluator e;
  e.setAngleMode(Evaluator::Radian);
  e.setExpression(QString::fromLatin1(expr));
  HNumber rn = e.eval();
  
  if( !e.error().isEmpty() )
  {
    eval_failed_tests++;
    std::cerr << file << "["<< line <<"]: " << msg;
    std::cerr << "  Error: "  << qPrintable(e.error());
    std::cerr << std::endl;
  }
  else
  {
    char* result = HMath::formatFixed( rn );
    if( strcmp( result, expected ) )
    {
      eval_failed_tests++;
      std::cerr << file << "["<< line <<"]: " << msg;
      std::cerr << "  Result: " << result;
      std::cerr << ", ";
      std::cerr << "Expected: " << expected;
      std::cerr << std::endl;
    }
    free( result );
  }  
}

static void checkEvalPrecise( const char *file, int line, const char* msg, 
const QString& expr, const char* expected )
{
  eval_total_tests++;

  Evaluator e;
  e.setExpression(QString::fromLatin1(expr));
  HNumber rn = e.eval();
  
  // we compare up to 50 decimals, not exact number because it's often difficult
  // to represent the result as rational numbers, e.g. PI
  char* result = HMath::formatFixed( rn, 50 );
  if( strcmp( result, expected ) )
  {
    eval_failed_tests++;
    std::cerr << file << "["<< line <<"]: " << msg;
    std::cerr << "  Result: " << result;
    std::cerr << ", ";
    std::cerr << "Expected: " << expected;
    std::cerr << std::endl;
  }
  free( result );
}

void test_constants()
{
  CHECK_EVAL( "1", "1" );
}

void test_unary()
{
  CHECK_EVAL( "-0", "0" );
  CHECK_EVAL( "-1", "-1" );
  CHECK_EVAL( "--1", "1" );
  CHECK_EVAL( "---1", "-1" );
  CHECK_EVAL( "----1", "1" );
  CHECK_EVAL( "-----1", "-1" );
  CHECK_EVAL( "------1", "1" );
  
  CHECK_EVAL( "-ABS(1)", "-1");
  CHECK_EVAL( "-ABS(-2)", "-2");
  CHECK_EVAL( "--ABS(-3)", "3");
  CHECK_EVAL( "---ABS(-4)", "-4");
  
  // see http://en.wikipedia.org/wiki/Empty_product    
  CHECK_EVAL( "0^0", "1" ); 
  
  CHECK_EVAL( "1^0", "1" );
  CHECK_EVAL( "1^1", "1" );
  CHECK_EVAL( "1^2", "1" );
  CHECK_EVAL( "1^3", "1" );
  
  CHECK_EVAL( "-1^0", "1" );
  CHECK_EVAL( "-1^1", "-1" );
  CHECK_EVAL( "-1^2", "1" );
  CHECK_EVAL( "-1^3", "-1" );
  
  CHECK_EVAL( "2^0", "1" );
  CHECK_EVAL( "2^1", "2" );
  CHECK_EVAL( "2^2", "4" );
  CHECK_EVAL( "2^3", "8" );
  CHECK_EVAL( "2^4", "16" );
  CHECK_EVAL( "2^5", "32" );
  CHECK_EVAL( "2^6", "64" );
  CHECK_EVAL( "2^7", "128" );
  CHECK_EVAL( "2^8", "256" );
  CHECK_EVAL( "2^9", "512" );
  CHECK_EVAL( "2^10", "1024" );
  
  CHECK_EVAL( "-2^0", "1" );
  CHECK_EVAL( "-2^1", "-2" );
  CHECK_EVAL( "-2^2", "4" );
  CHECK_EVAL( "-2^3", "-8" );
  CHECK_EVAL( "-2^4", "16" );
  CHECK_EVAL( "-2^5", "-32" );
  CHECK_EVAL( "-2^6", "64" );
  CHECK_EVAL( "-2^7", "-128" );
  CHECK_EVAL( "-2^8", "256" );
  CHECK_EVAL( "-2^9", "-512" );
  CHECK_EVAL( "-2^10", "1024" );
  
  CHECK_EVAL( "(5-7)^2", "4" );
}

void test_binary()
{
  CHECK_EVAL( "0+0", "0" );
  CHECK_EVAL( "1+0", "1" );
  CHECK_EVAL( "0+1", "1" );
  CHECK_EVAL( "1+1", "2" );
}

void test_function_basic()
{
  CHECK_EVAL( "ABS(0)", "0" );
  CHECK_EVAL( "ABS(1)", "1" );
  CHECK_EVAL( "ABS(-1)", "1" );
  CHECK_EVAL( "ABS(--1)", "1" );
  CHECK_EVAL( "ABS(---1)", "1" );
  CHECK_EVAL( "ABS((1))", "1" );
  CHECK_EVAL( "ABS((-1))", "1" );
  
  CHECK_EVAL( "ABS(1/2)", "0.5" );
  CHECK_EVAL( "ABS(-1/2)", "0.5" );
  CHECK_EVAL( "ABS(-1/-2)", "0.5" );
  CHECK_EVAL( "ABS(1/-2)", "0.5" );
  
  CHECK_EVAL( "INT(0)", "0" );
  CHECK_EVAL( "INT(1)", "1" );
  CHECK_EVAL( "INT(-1)", "-1" );
  CHECK_EVAL( "INT(0.5)", "0" );
  CHECK_EVAL( "INT(-0.75)", "0" );
  CHECK_EVAL( "INT(-0.9999*1)", "0" );
  CHECK_EVAL( "INT(0.9999*1)", "0" );
  CHECK_EVAL( "INT(2.1)", "2" );
  CHECK_EVAL( "INT(-3.4)", "-3" );
  
  CHECK_EVAL_PRECISE( "exp((1)/2) + exp((1)/2)", "3.29744254140025629369730157562832714330755220142030" );  
}

void test_function_trig()
{
  CHECK_EVAL_PRECISE( "pi", "3.14159265358979323846264338327950288419716939937511" );
  
  CHECK_EVAL( "sin(0)", "0" );
  CHECK_EVAL( "cos(0)", "1" );
  CHECK_EVAL( "tan(0)", "0" );
  
  CHECK_EVAL( "sin(pi)", "0" );
  CHECK_EVAL( "cos(pi)", "-1" );
  CHECK_EVAL( "tan(pi)", "0" );

  CHECK_EVAL( "sin(-pi)", "0" );
  CHECK_EVAL( "cos(-pi)", "-1" );
  CHECK_EVAL( "tan(-pi)", "0" );

  CHECK_EVAL( "sin(--pi)", "0" );
  CHECK_EVAL( "cos(--pi)", "-1" );
  CHECK_EVAL( "tan(--pi)", "0" );

  CHECK_EVAL( "sin(pi/2)", "1" );
  CHECK_EVAL( "cos(pi/2)", "0" );
  // Note: tan(pi/2) gives invalid result

  CHECK_EVAL( "sin(-pi/2)", "-1" );
  CHECK_EVAL( "cos(-pi/2)", "0" );

  CHECK_EVAL( "sin(-pi/2) + sin(pi/2)", "0" );
  CHECK_EVAL( "sin(-pi/2) - sin(pi/2)", "-2" );
  CHECK_EVAL( "cos(-pi/2) + cos(pi/2)", "0" );
  CHECK_EVAL( "cos(-pi/2) - cos(pi/2)", "0" );
  
  CHECK_EVAL( "asin(sin(1))", "1");
  CHECK_EVAL( "acos(cos(1))", "1");
}

void test_function_stat()
{
  CHECK_EVAL( "MIN(0)", "0" );
  CHECK_EVAL( "MIN(0, 1)", "0" );
  CHECK_EVAL( "MIN(0, 2)", "0" );
  CHECK_EVAL( "MIN(-1, 0)", "-1" );
  CHECK_EVAL( "MIN(-1, 1)", "-1" );
  CHECK_EVAL( "MIN(-0.01, 0)", "-0.01" );
  CHECK_EVAL( "MIN(0, 1, 2)", "0" );
  CHECK_EVAL( "MIN(-1, 0, 1, 2)", "-1" );
  CHECK_EVAL( "MIN(-2, -1, 0, 1, 2)", "-2" );

  CHECK_EVAL( "MAX(0)", "0" );
  CHECK_EVAL( "MAX(0, 1)", "1" );
  CHECK_EVAL( "MAX(0, 2)", "2" );
  CHECK_EVAL( "MAX(-1, 0)", "0" );
  CHECK_EVAL( "MAX(-1, 1)", "1" );
  CHECK_EVAL( "MAX(0.01, 0)", "0.01" );
  CHECK_EVAL( "MAX(0, 1, 2)", "2" );
  CHECK_EVAL( "MAX(-1, 0, 1, 2)", "2" );
  CHECK_EVAL( "MAX(-2, -1, 0, 1, 2)", "2" );
}

int main(int argc, char** argv)
{
  eval_total_tests = 0;
  eval_failed_tests = 0;
  
  test_constants();
  test_unary();
  test_binary();
  
  test_function_basic();
  test_function_trig();
  test_function_stat();

  std::cerr << eval_total_tests << " total, ";
  std::cerr << eval_failed_tests << " failed\n";

  return eval_failed_tests;
}
