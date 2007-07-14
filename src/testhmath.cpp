/* This file is part of the SpeedCrunch project
   Copyright (C) 2004-2006 Ariya Hidayat <ariya@kde.org>
                 2007 Helder Correia <helder.pereira.correia@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
 */


#include <iostream>
#include <string.h>

#include "hmath.h"


using namespace std;


#define CHECK(x,y)            check_value(__FILE__,__LINE__,#x,x,y)
#define CHECK_FORMAT(f,p,x,y) check_format(__FILE__,__LINE__,#x,x,f,p,y)
#define CHECK_PRECISE(x,y)    check_precise(__FILE__,__LINE__,#x,x,y)


static int hmath_total_tests  = 0;
static int hmath_failed_tests = 0;

static const HNumber PI  = HMath::pi();
static const HNumber PHI = HMath::phi();


static void check_value( const char    * file,
                               int       line,
                         const char    * msg,
                         const HNumber & n,
                         const char    * expected )
{
  hmath_total_tests++;
  char * result = HMath::formatFixed( n );
  if ( strcmp( result, expected ) )
  {
    hmath_failed_tests++;
    cerr << file << "[" << line << "]: " << msg << endl
         << "  Result  : " << result   << endl
         << "  Expected: " << expected << endl
         << endl;
  }
  free( result );
}


static void check_format( const char    * file,
                                int       line,
                          const char    * msg,
                          const HNumber & n,
                                char      format,
                                int       prec,
                          const char    * expected )
{
  hmath_total_tests++;

  char * result = HMath::format( n, format, prec );
  if ( strcmp( result, expected ) )
  {
    hmath_failed_tests++;

    cerr << file << "[" << line << "]: " << msg << endl
         << "  Result  : " << result   << endl
         << "  Expected: " << expected << endl
         << endl;
  }
  free( result );
}


static void check_precise( const char    * file,
                                 int       line,
                           const char    * msg,
                           const HNumber & n,
                           const char    * expected )
{
  hmath_total_tests++;

  char * result = HMath::formatFixed( n, 50 );
  if ( strcmp( result, expected ) )
  {
    hmath_failed_tests++;

    cerr << file << "[" << line << "]: " << msg << endl
         << "  Result  : " << result   << endl
         << "  Expected: " << expected << endl
         << endl;
  }
  free( result );
}


void test_create()
{
  CHECK( HNumber( "1.0"  ), "1"     );
  CHECK( HNumber( "2.0"  ), "2"     );
  CHECK( HNumber( "1e-3" ), "0.001" );

  // too large or too small
  CHECK( HNumber( "1e200"  ), "NaN" );
  CHECK( HNumber( "1e-200" ), "NaN" );
  CHECK_FORMAT( 'e', 2, HNumber( "1e200"  ), "NaN" );
  CHECK_FORMAT( 'e', 2, HNumber( "1e-200" ), "NaN" );
}

void test_format()
{
  // fixed decimal digits
  CHECK_FORMAT( 'f',  0, HNumber( "NaN"    ), "NaN"          );
  CHECK_FORMAT( 'f',  0, HNumber( "0"      ), "0"            );
  CHECK_FORMAT( 'f',  0, HNumber( "1.1"    ), "1"            );
  CHECK_FORMAT( 'f',  1, HNumber( "2.11"   ), "2.1"          );
  CHECK_FORMAT( 'f',  2, HNumber( "3.111"  ), "3.11"         );
  CHECK_FORMAT( 'f',  1, HNumber( "4.001"  ), "4.0"          );
  CHECK_FORMAT( 'f',  3, HNumber( "4.1111" ), "4.111"        );
  CHECK_FORMAT( 'f',  2, HNumber( "3.14"   ), "3.14"         );
  CHECK_FORMAT( 'f',  3, HNumber( "3.14"   ), "3.140"        );
  CHECK_FORMAT( 'f',  5, HNumber( "3.14"   ), "3.14000"      );
  CHECK_FORMAT( 'f',  7, HNumber( "3.14"   ), "3.1400000"    );
  CHECK_FORMAT( 'f',  7, HNumber( "-0.001" ), "-0.0010000"   );
  CHECK_FORMAT( 'f',  8, HNumber( "-0.001" ), "-0.00100000"  );
  CHECK_FORMAT( 'f',  9, HNumber( "-0.001" ), "-0.001000000" );
  CHECK_FORMAT( 'f', -1,
                HNumber( "4.000000000000000000000000000000000000000000001" ),
                "4" );

  // engineering notation
  CHECK_FORMAT( 'n', 0, HNumber( "NaN"      ), "NaN"         );
  CHECK_FORMAT( 'n', 0, HNumber( "0"        ), "0e0"         );
  CHECK_FORMAT( 'n', 0, HNumber( "3.14"     ), "3e0"         );
  CHECK_FORMAT( 'n', 1, HNumber( "3.14"     ), "3.1e0"       );
  CHECK_FORMAT( 'n', 2, HNumber( "3.14"     ), "3.14e0"      );
  CHECK_FORMAT( 'n', 3, HNumber( "3.14"     ), "3.140e0"     );
  CHECK_FORMAT( 'n', 5, HNumber( "3.14"     ), "3.14000e0"   );
  CHECK_FORMAT( 'n', 7, HNumber( "3.14"     ), "3.1400000e0" );
  CHECK_FORMAT( 'n', 3, HNumber( "-0.001"   ), "-1.000e-3"   );
  CHECK_FORMAT( 'n', 2, HNumber( "0.0001"   ), "100.00e-6"   );
  CHECK_FORMAT( 'n', 2, HNumber( "0.001"    ), "1.00e-3"     );
  CHECK_FORMAT( 'n', 2, HNumber( "0.01"     ), "10.00e-3"    );
  CHECK_FORMAT( 'n', 2, HNumber( "0.1"      ), "100.00e-3"   );
  CHECK_FORMAT( 'n', 2, HNumber( "1"        ), "1.00e0"      );
  CHECK_FORMAT( 'n', 2, HNumber( "10"       ), "10.00e0"     );
  CHECK_FORMAT( 'n', 2, HNumber( "100"      ), "100.00e0"    );
  CHECK_FORMAT( 'n', 2, HNumber( "1000"     ), "1.00e3"      );
  CHECK_FORMAT( 'n', 2, HNumber( "10000"    ), "10.00e3"     );
  CHECK_FORMAT( 'n', 2, HNumber( "100000"   ), "100.00e3"    );
  CHECK_FORMAT( 'n', 2, HNumber( "1000000"  ), "1.00e6"      );
  CHECK_FORMAT( 'n', 2, HNumber( "10000000" ), "10.00e6"     );

  // scientific notation
  CHECK_FORMAT( 'e', 0, HNumber( "NaN"      ), "NaN"         );
  CHECK_FORMAT( 'e', 0, HNumber( "0"        ), "0e0"         );
  CHECK_FORMAT( 'e', 0, HNumber( "3.14"     ), "3e0"         );
  CHECK_FORMAT( 'e', 1, HNumber( "3.14"     ), "3.1e0"       );
  CHECK_FORMAT( 'e', 2, HNumber( "3.14"     ), "3.14e0"      );
  CHECK_FORMAT( 'e', 3, HNumber( "3.14"     ), "3.140e0"     );
  CHECK_FORMAT( 'e', 5, HNumber( "3.14"     ), "3.14000e0"   );
  CHECK_FORMAT( 'e', 7, HNumber( "3.14"     ), "3.1400000e0" );
  CHECK_FORMAT( 'e', 3, HNumber( "-0.001"   ), "-1.000e-3"   );
  CHECK_FORMAT( 'e', 2, HNumber( "0.0001"   ), "1.00e-4"     );
  CHECK_FORMAT( 'e', 2, HNumber( "0.001"    ), "1.00e-3"     );
  CHECK_FORMAT( 'e', 2, HNumber( "0.01"     ), "1.00e-2"     );
  CHECK_FORMAT( 'e', 2, HNumber( "0.1"      ), "1.00e-1"     );
  CHECK_FORMAT( 'e', 2, HNumber( "1"        ), "1.00e0"      );
  CHECK_FORMAT( 'e', 2, HNumber( "10"       ), "1.00e1"      );
  CHECK_FORMAT( 'e', 2, HNumber( "100"      ), "1.00e2"      );
  CHECK_FORMAT( 'e', 2, HNumber( "1000"     ), "1.00e3"      );
  CHECK_FORMAT( 'e', 2, HNumber( "10000"    ), "1.00e4"      );
  CHECK_FORMAT( 'e', 2, HNumber( "100000"   ), "1.00e5"      );
  CHECK_FORMAT( 'e', 2, HNumber( "1000000"  ), "1.00e6"      );
  CHECK_FORMAT( 'e', 2, HNumber( "10000000" ), "1.00e7"      );

  // general format
  CHECK_FORMAT( 'g', -1, PI,  "3.14159265358979323846" );
  CHECK_FORMAT( 'g', -1, PHI, "1.6180339887498948482"  );
  CHECK_FORMAT( 'g', 3, HNumber( "0"           ), "0.000"      );
  CHECK_FORMAT( 'g', 3, HNumber( "0.000000001" ), "1.000e-9"   );
  CHECK_FORMAT( 'g', 3, HNumber( "0.00000001"  ), "1.000e-8"   );
  CHECK_FORMAT( 'g', 3, HNumber( "0.0000001"   ), "1.000e-7"   );
  CHECK_FORMAT( 'g', 3, HNumber( "0.000001"    ), "1.000e-6"   );
  CHECK_FORMAT( 'g', 3, HNumber( "0.00001"     ), "1.000e-5"   );
  CHECK_FORMAT( 'g', 3, HNumber( "0.0001"      ), "1.000e-4"   );
  CHECK_FORMAT( 'g', 3, HNumber( "0.001"       ), "0.001"      );
  CHECK_FORMAT( 'g', 3, HNumber( "0.01"        ), "0.010"      );
  CHECK_FORMAT( 'g', 3, HNumber( "0.1"         ), "0.100"      );
  CHECK_FORMAT( 'g', 3, HNumber( "10"          ), "10.000"     );
  CHECK_FORMAT( 'g', 3, HNumber( "100"         ), "100.000"    );
  CHECK_FORMAT( 'g', 3, HNumber( "1000"        ), "1000.000"   );
  CHECK_FORMAT( 'g', 3, HNumber( "10000"       ), "10000.000"  );
  CHECK_FORMAT( 'g', 3, HNumber( "100000"      ), "100000.000" );
  CHECK_FORMAT( 'g', 3, HNumber( "1000000"     ), "1.000e6"    );
  CHECK_FORMAT( 'g', 3, HNumber( "10000000"    ), "1.000e7"    );
  CHECK_FORMAT( 'g', 3, HNumber( "100000000"   ), "1.000e8"    );
  CHECK_FORMAT( 'g', 3, HNumber( "1403.1977"   ), "1403.198"   );
  CHECK_FORMAT( 'g', 3, HNumber( "2604.1980"   ), "2604.198"   );
  CHECK_FORMAT( 'g', 3, HNumber( "2.47e4"      ), "24700.000"  );
}


void test_op()
{
  // addition
  CHECK( HNumber( 0 ) + HNumber( 0    ), "0"    );
  CHECK( HNumber( 1 ) + HNumber( 0    ), "1"    );
  CHECK( HNumber( 1 ) + HNumber( 1    ), "2"    );
  CHECK( HNumber( 1 ) + HNumber( 2    ), "3"    );
  CHECK( HNumber( 1 ) + HNumber( 10   ), "11"   );
  CHECK( HNumber( 1 ) + HNumber( 100  ), "101"  );
  CHECK( HNumber( 1 ) + HNumber( 1000 ), "1001" );

  // subtraction
  CHECK( HNumber( 0 ) - HNumber( 0 ), "0"  );
  CHECK( HNumber( 1 ) - HNumber( 0 ), "1"  );
  CHECK( HNumber( 1 ) - HNumber( 2 ), "-1" );

  // division
  CHECK( HNumber( 1 ) / HNumber( 2 ),     "0.5" );
  CHECK( HNumber( 1 ) / HMath::sin( PI ), "NaN" );

  CHECK_PRECISE( HNumber( 1 ) / HNumber( 3 ),
                 "0.33333333333333333333333333333333333333333333333333" );
  CHECK_PRECISE( HNumber( 2 ) / HNumber( 3 ),
                 "0.66666666666666666666666666666666666666666666666667" );
  CHECK_PRECISE( HNumber( 1 ) / HNumber( 7 ),
                 "0.14285714285714285714285714285714285714285714285714" );
  CHECK_PRECISE( HNumber( 2 ) / HNumber( 7 ),
                 "0.28571428571428571428571428571428571428571428571429" );
  CHECK_PRECISE( HNumber( 3 ) / HNumber( 7 ),
                 "0.42857142857142857142857142857142857142857142857143" );
  CHECK_PRECISE( HNumber( 4 ) / HNumber( 7 ),
                 "0.57142857142857142857142857142857142857142857142857" );
  CHECK_PRECISE( HNumber( 1 ) / HNumber( 9 ),
                 "0.11111111111111111111111111111111111111111111111111" );

  // multiplication
  CHECK( HNumber( 0     ) * HNumber( 0     ), "0"    );
  CHECK( HNumber( "1.0" ) * HNumber( "0.0" ), "0"    );
  CHECK( HNumber( 1     ) * HNumber( 1     ), "1"    );
  CHECK( HNumber( 3     ) * HNumber( -4    ), "-12"  );
  CHECK( HNumber( -2    ) * HNumber( 5     ), "-10"  );
  CHECK( HNumber( 6     ) * HNumber( 7     ), "42"   );
  CHECK( HNumber( "1.5" ) * HNumber( "1.5" ), "2.25" );
}


void test_functions()
{
  // pi
  CHECK( PI, "3.14159265358979323846" );
  CHECK_PRECISE( PI, "3.14159265358979323846264338327950288419716939937511" );

  // phi
  CHECK( PHI, "1.6180339887498948482" );
  CHECK_PRECISE( PHI, "1.61803398874989484820458683436563811772030917980576" );

  // abs
  CHECK( HMath::abs( "NaN"            ), "NaN"           );
  CHECK( HMath::abs( "0"              ), "0"             );
  CHECK( HMath::abs( "1"              ), "1"             );
  CHECK( HMath::abs( "100"            ), "100"           );
  CHECK( HMath::abs( "-100"           ), "100"           );
  CHECK( HMath::abs( "-3.14159"       ), "3.14159"       );
  CHECK( HMath::abs( "-0.00000014159" ), "0.00000014159" );

  // floor
  CHECK( HMath::floor( "NaN"        ), "NaN" );
  CHECK( HMath::floor( "0"          ), "0"   );
  CHECK( HMath::floor( "5"          ), "5"   );
  CHECK( HMath::floor( "-5"         ), "-5"  );
  CHECK( HMath::floor( "3.14"       ), "3"   );
  CHECK( HMath::floor( "1.77"       ), "1"   );
  CHECK( HMath::floor( "-2.6041980" ), "-3"  );
  CHECK( HMath::floor( "2.6041980"  ), "2"   );
  CHECK( HMath::floor( "0.000001"   ), "0"   );
  CHECK( HMath::floor( "-0.000001"  ), "-1"  );

  // ceil
  CHECK( HMath::ceil( "NaN"        ), "NaN" );
  CHECK( HMath::ceil( "0"          ), "0"   );
  CHECK( HMath::ceil( "5"          ), "5"   );
  CHECK( HMath::ceil( "-5"         ), "-5"  );
  CHECK( HMath::ceil( "3.14"       ), "4"   );
  CHECK( HMath::ceil( "1.77"       ), "2"   );
  CHECK( HMath::ceil( "-2.6041980" ), "-2"  );
  CHECK( HMath::ceil( "2.6041980"  ), "3"   );
  CHECK( HMath::ceil( "0.000001"   ), "1"   );
  CHECK( HMath::ceil( "-0.000001"  ), "0"   );

  // gcd
  CHECK( HMath::gcd( "NaN",  "NaN" ), "NaN" );
  CHECK( HMath::gcd( "NaN",  "5"   ), "NaN" );
  CHECK( HMath::gcd( "5",    "NaN" ), "NaN" );
  CHECK( HMath::gcd( "0",    "0"   ), "0"   );
  CHECK( HMath::gcd( "0",    "5"   ), "5"   );
  CHECK( HMath::gcd( "5",    "0"   ), "5"   );
  CHECK( HMath::gcd( "0",    "-5"  ), "5"   );
  CHECK( HMath::gcd( "-5",   "0"   ), "5"   );
  CHECK( HMath::gcd( "9",    "-27" ), "9"   );
  CHECK( HMath::gcd( "99",   "103" ), "1"   );
  CHECK( HMath::gcd( "-102", "306" ), "102" );

  // round/1
  CHECK( HMath::round( "NaN"   ), "NaN" );
  CHECK( HMath::round( "3.14"  ), "3"   );
  CHECK( HMath::round( "-1.77" ), "-2"  );
  // round/2
  CHECK( HMath::round( "NaN",        3  ), "NaN"     );
  CHECK( HMath::round( "0.005",      -3 ), "0"       );
  CHECK( HMath::round( "0.005",      2  ), "0.01"    );
  CHECK( HMath::round( "3.14159",    4  ), "3.1416"  );
  CHECK( HMath::round( "3.14159",    3  ), "3.142"   );
  CHECK( HMath::round( "3.14159",    2  ), "3.14"    );
  CHECK( HMath::round( "3.14159",    1  ), "3.1"     );
  CHECK( HMath::round( "3.14159",    0  ), "3"       );
  CHECK( HMath::round( "-2.6041980", 4  ), "-2.6042" );
  CHECK( HMath::round( "-2.6041980", 3  ), "-2.604"  );
  CHECK( HMath::round( "-2.6041980", 2  ), "-2.6"    );
  CHECK( HMath::round( "-2.6041980", 1  ), "-2.6"    );
  CHECK( HMath::round( "-2.6041980", 0  ), "-3"      );

  // trunc/1
  CHECK( HMath::trunc( "NaN"  ), "NaN" );
  CHECK( HMath::trunc( "0"    ), "0"   );
  CHECK( HMath::trunc( "3.14" ), "3"   );
  CHECK( HMath::trunc( "1.77" ), "1"   );
  // trunc/2
  CHECK( HMath::trunc( "NaN",        3   ), "NaN"       );
  CHECK( HMath::trunc( "0.00013",    -3  ), "0"         );
  CHECK( HMath::trunc( "0.00013",    4   ), "0.0001"    );
  CHECK( HMath::trunc( "3.14159",    4   ), "3.1415"    );
  CHECK( HMath::trunc( "3.14159",    3   ), "3.141"     );
  CHECK( HMath::trunc( "3.14159",    2   ), "3.14"      );
  CHECK( HMath::trunc( "3.14159",    1   ), "3.1"       );
  CHECK( HMath::trunc( "3.14159",    0   ), "3"         );
  CHECK( HMath::trunc( "-2.6041980", 4   ), "-2.6041"   );
  CHECK( HMath::trunc( "-2.6041980", 3   ), "-2.604"    );
  CHECK( HMath::trunc( "-2.6041980", 2   ), "-2.6"      );
  CHECK( HMath::trunc( "-2.6041980", 1   ), "-2.6"      );
  CHECK( HMath::trunc( "-2.6041980", 0   ), "-2"        );
  CHECK( HMath::trunc( "-2.6041980", 999 ), "-2.604198" );
  CHECK( HMath::trunc( "-2.6041980", -2  ), "-2"        );

  // integer
  CHECK( HMath::integer( "NaN"      ), "NaN" );
  CHECK( HMath::integer( "0"        ), "0"   );
  CHECK( HMath::integer( "0.25"     ), "0"   );
  CHECK( HMath::integer( "0.85"     ), "0"   );
  CHECK( HMath::integer( "14.0377"  ), "14"  );
  CHECK( HMath::integer( "-0.25"    ), "0"   );
  CHECK( HMath::integer( "-0.85"    ), "0"   );
  CHECK( HMath::integer( "-14.0377" ), "-14" );

  // frac
  CHECK( HMath::frac( "NaN"      ), "NaN"      );
  CHECK( HMath::frac( "0"        ), "0"        );
  CHECK( HMath::frac( "3.14159"  ), "0.14159"  );
  CHECK( HMath::frac( "0.14159"  ), "0.14159"  );
  CHECK( HMath::frac( "-3.14159" ), "-0.14159" );
  CHECK( HMath::frac( "-0.14159" ), "-0.14159" );

  // sqrt
  CHECK( HMath::sqrt( "NaN"  ), "NaN" );
  CHECK( HMath::sqrt( -1     ), "NaN" );
  CHECK( HMath::sqrt( 0      ), "0"   );
  CHECK( HMath::sqrt( 1      ), "1"   );
  CHECK( HMath::sqrt( 4      ), "2"   );
  CHECK( HMath::sqrt( 9      ), "3"   );
  CHECK( HMath::sqrt( 16     ), "4"   );
  CHECK( HMath::sqrt( "0.04" ), "0.2" );
  CHECK( HMath::sqrt( "0.09" ), "0.3" );
  CHECK( HMath::sqrt( "0.16" ), "0.4" );
  CHECK_PRECISE( HMath::sqrt( 2  ),
                 "1.41421356237309504880168872420969807856967187537695" );
  CHECK_PRECISE( HMath::sqrt( 3  ),
                 "1.73205080756887729352744634150587236694280525381038" );
  CHECK_PRECISE( HMath::sqrt( 5  ),
                 "2.23606797749978969640917366873127623544061835961153" );
  CHECK_PRECISE( HMath::sqrt( 7  ),
                 "2.64575131106459059050161575363926042571025918308245" );
  CHECK_PRECISE( HMath::sqrt( 8  ),
                 "2.82842712474619009760337744841939615713934375075390" );
  CHECK_PRECISE( HMath::sqrt( 10 ),
                 "3.16227766016837933199889354443271853371955513932522" );
  CHECK_PRECISE( HMath::sqrt( 11 ),
                 "3.31662479035539984911493273667068668392708854558935" );
  CHECK_PRECISE( HMath::sqrt( 12 ),
                 "3.46410161513775458705489268301174473388561050762076" );
  CHECK_PRECISE( HMath::sqrt( 13 ),
                 "3.60555127546398929311922126747049594625129657384525" );
  CHECK_PRECISE( HMath::sqrt( 14 ),
                 "3.74165738677394138558374873231654930175601980777873" );
  CHECK_PRECISE( HMath::sqrt( 15 ),
                 "3.87298334620741688517926539978239961083292170529159" );
  CHECK_PRECISE( HMath::sqrt( 17 ),
                 "4.12310562561766054982140985597407702514719922537362" );
  CHECK_PRECISE( HMath::sqrt( 18 ),
                 "4.24264068711928514640506617262909423570901562613084" );
  CHECK_PRECISE( HMath::sqrt( 19 ),
                 "4.35889894354067355223698198385961565913700392523244" );
  CHECK_PRECISE( HMath::sqrt( 20 ),
                 "4.47213595499957939281834733746255247088123671922305" );

  // cbrt
  CHECK( HMath::cbrt( "NaN"    ), "NaN"  );
  CHECK( HMath::cbrt( 0        ), "0"    );
  CHECK( HMath::cbrt( 1        ), "1"    );
  CHECK( HMath::cbrt( -1       ), "-1"   );
  CHECK( HMath::cbrt( 8        ), "2"    );
  CHECK( HMath::cbrt( -8       ), "-2"   );
  CHECK( HMath::cbrt( 27       ), "3"    );
  CHECK( HMath::cbrt( -27      ), "-3"   );
  CHECK( HMath::cbrt( 64       ), "4"    );
  CHECK( HMath::cbrt( -64      ), "-4"   );
  CHECK( HMath::cbrt( "0.008"  ), "0.2"  );
  CHECK( HMath::cbrt( "-0.008" ), "-0.2" );
  CHECK( HMath::cbrt( "0.027"  ), "0.3"  );
  CHECK( HMath::cbrt( "-0.027" ), "-0.3" );
  CHECK( HMath::cbrt( "0.064"  ), "0.4"  );
  CHECK( HMath::cbrt( "-0.064" ), "-0.4" );
  CHECK_PRECISE( HMath::cbrt( 2   ),
                 "1.25992104989487316476721060727822835057025146470151"  );
  CHECK_PRECISE( HMath::cbrt( 3   ),
                 "1.44224957030740838232163831078010958839186925349935"  );
  CHECK_PRECISE( HMath::cbrt( -20 ),
                 "-2.71441761659490657151808946967948920480510776948910" );

  // sign
  CHECK( HMath::sign( "NaN" ), "NaN" );
  CHECK( HMath::sign( 0     ), "0"   );
  CHECK( HMath::sign( 1     ), "1"   );
  CHECK( HMath::sign( -1    ), "-1"  );
  CHECK( HMath::sign( 2     ), "1"   );
  CHECK( HMath::sign( -2    ), "-1"  );

  // factorial/1
  CHECK( HMath::factorial( "NaN"  ), "NaN"   );
  CHECK( HMath::factorial( -1     ), "NaN"   );
  CHECK( HMath::factorial( 0      ), "1"     );
  CHECK( HMath::factorial( 1      ), "1"     );
  CHECK( HMath::factorial( 2      ), "2"     );
  CHECK( HMath::factorial( 3      ), "6"     );
  CHECK( HMath::factorial( 4      ), "24"    );
  CHECK( HMath::factorial( 5      ), "120"   );
  CHECK( HMath::factorial( 6      ), "720"   );
  CHECK( HMath::factorial( 7      ), "5040"  );
  CHECK( HMath::factorial( 8      ), "40320" );
  CHECK( HMath::factorial( "5.23" ), "NaN"   );
  CHECK( HMath::factorial( "-5"   ), "NaN"   );
  // factorial/2
  CHECK( HMath::factorial( "NaN", "NaN" ), "NaN" );
  CHECK( HMath::factorial( "NaN", 7     ), "NaN" );
  CHECK( HMath::factorial( 8,     "NaN" ), "NaN" );
  CHECK( HMath::factorial( 8,     0     ), "NaN" );
  CHECK( HMath::factorial( 8,     6     ), "336" );
  CHECK( HMath::factorial( 8,     7     ), "56"  );
  CHECK( HMath::factorial( 8,     8     ), "8"   );

  // nCr
  CHECK( HMath::nCr( "NaN", "NaN" ), "NaN"  );
  CHECK( HMath::nCr( "NaN", 5     ), "NaN"  );
  CHECK( HMath::nCr( 5,     "NaN" ), "NaN"  );
  CHECK( HMath::nCr( 21,    -1    ), "NaN"  );
  CHECK( HMath::nCr( -21,   2     ), "NaN"  );
  CHECK( HMath::nCr( 21,    0     ), "1"    );
  CHECK( HMath::nCr( 21,    1     ), "21"   );
  CHECK( HMath::nCr( 21,    2     ), "210"  );
  CHECK( HMath::nCr( 21,    3     ), "1330" );
  CHECK( HMath::nCr( 21,    19    ), "210"  );
  CHECK( HMath::nCr( 21,    20    ), "21"   );
  CHECK( HMath::nCr( 21,    21    ), "1"    );
  CHECK( HMath::nCr( 21,    22    ), "NaN"  );
  CHECK( HMath::nCr( -21,   2     ), "NaN"  );
  CHECK( HMath::nCr( 0,     0     ), "1"    );

  // nPr
  CHECK( HMath::nPr( "NaN", "NaN" ), "NaN"       );
  CHECK( HMath::nPr( "NaN", 5     ), "NaN"       );
  CHECK( HMath::nPr( 5,     "NaN" ), "NaN"       );
  CHECK( HMath::nPr( 21,    22    ), "NaN"       );
  CHECK( HMath::nPr( -21,   2     ), "NaN"       );
  CHECK( HMath::nPr( 21,    -1    ), "NaN"       );
  CHECK( HMath::nPr( 0,     0     ), "1"         );
  CHECK( HMath::nPr( 21,    0     ), "1"         );
  CHECK( HMath::nPr( 21,    1     ), "21"        );
  CHECK( HMath::nPr( 21,    2     ), "420"       );
  CHECK( HMath::nPr( 21,    3     ), "7980"      );
  CHECK( HMath::nPr( 21,    4     ), "143640"    );
  CHECK( HMath::nPr( 21,    5     ), "2441880"   );
  CHECK( HMath::nPr( 21,    6     ), "39070080"  );
  CHECK( HMath::nPr( 21,    7     ), "586051200" );

  // raise
  CHECK( HMath::raise( "NaN", "NaN" ), "NaN"   );
  CHECK( HMath::raise( "NaN", "0"   ), "NaN"   );
  CHECK( HMath::raise( "-1",  "NaN" ), "NaN"   );
  CHECK( HMath::raise( 10,    -3    ), "0.001" );
  CHECK( HMath::raise( 10,    -2    ), "0.01"  );
  CHECK( HMath::raise( 10,    -1    ), "0.1"   );
  CHECK( HMath::raise( 10,    0     ), "1"     );
  CHECK( HMath::raise( 10,    1     ), "10"    );
  CHECK( HMath::raise( 10,    2     ), "100"   );
  CHECK( HMath::raise( 10,    3     ), "1000"  );
  CHECK( HMath::raise( 10,    4     ), "10000" );
  CHECK( HMath::raise( "2",   "2"   ), "4"     );
  CHECK( HMath::raise( "3",   "3"   ), "27"    );
  CHECK( HMath::raise( "4",   "4"   ), "256"   );
  CHECK_PRECISE( HMath::raise( "2", "0.1" ),
                 "1.07177346253629316421300632502334202290638460497756" );
  CHECK_PRECISE( HMath::raise( "2", "0.2" ),
                 "1.14869835499703500679862694677792758944385088909780" );
  CHECK_PRECISE( HMath::raise( "2", "0.3" ),
                 "1.23114441334491628449939306916774310987613776110082" );

  // exp
  CHECK( HMath::exp( "NaN" ), "NaN" );
  CHECK( HMath::exp( "0"   ), "1"   );
  CHECK_PRECISE( HMath::exp( "0.1" ),
                 "1.10517091807564762481170782649024666822454719473752" );
  CHECK_PRECISE( HMath::exp( "0.2" ),
                 "1.22140275816016983392107199463967417030758094152050" );
  CHECK_PRECISE( HMath::exp( "0.3" ),
                 "1.34985880757600310398374431332800733037829969735937" );
  CHECK_PRECISE( HMath::exp( "0.4" ),
                 "1.49182469764127031782485295283722228064328277393743" );
  CHECK_PRECISE( HMath::exp( "0.5" ),
                 "1.64872127070012814684865078781416357165377610071015" );
  CHECK_PRECISE( HMath::exp( "0.6" ),
                 "1.82211880039050897487536766816286451338223880854644" );
  CHECK_PRECISE( HMath::exp( "0.7" ),
                 "2.01375270747047652162454938858306527001754239414587" );
  CHECK_PRECISE( HMath::exp( "0.8" ),
                 "2.22554092849246760457953753139507675705363413504848" );
  CHECK_PRECISE( HMath::exp( "0.9" ),
                 "2.45960311115694966380012656360247069542177230644008" );
  CHECK_PRECISE( HMath::exp( "1.0" ),
                 "2.71828182845904523536028747135266249775724709369996" );
  CHECK_PRECISE( HMath::exp( "100" ),
                 "26881171418161354484126255515800135873611118."
                 "77374192241519160861528028703490956491415887109722"   );

  // binom *
  CHECK( HMath::binomialPmf( "NaN", "NaN", "NaN" ), "NaN"        );
  CHECK( HMath::binomialPmf( "NaN", "NaN", "0.5" ), "NaN"        );
  CHECK( HMath::binomialPmf( "NaN", "10",  "NaN" ), "NaN"        );
  CHECK( HMath::binomialPmf( "NaN", "10",  "0.5" ), "NaN"        );
  CHECK( HMath::binomialPmf( "5",   "NaN", "NaN" ), "NaN"        );
  CHECK( HMath::binomialPmf( "5",   "NaN", "0.5" ), "NaN"        );
  CHECK( HMath::binomialPmf( "5",   "10",  "NaN" ), "NaN"        );
  CHECK( HMath::binomialPmf( "5",   "10",  "0.5" ), "0.24609375" );
  CHECK( HMath::binomialPmf( "-5",  "10",  "0.5" ), "NaN"        );
  CHECK_PRECISE( HMath::binomialPmf( "5", "10", "0.5" ),
                 "0.24609375000000000000000000000000000000000000000000" );

  CHECK( HMath::binomialCdf( "NaN", "NaN", "NaN" ), "NaN"         );
  CHECK( HMath::binomialCdf( "NaN", "NaN", "0.5" ), "NaN"         );
  CHECK( HMath::binomialCdf( "NaN", "10",  "NaN" ), "NaN"         );
  CHECK( HMath::binomialCdf( "NaN", "10",  "0.5" ), "NaN"         );
  CHECK( HMath::binomialCdf( "5",   "NaN", "NaN" ), "NaN"         );
  CHECK( HMath::binomialCdf( "5",   "NaN", "0.5" ), "NaN"         );
  CHECK( HMath::binomialCdf( "5",   "10",  "NaN" ), "NaN"         );
  CHECK( HMath::binomialCdf( "5",   "10",  "0.5" ), "0.623046875" );
  CHECK( HMath::binomialCdf( "-5",  "10",  "0.5" ), "NaN"         );
  CHECK_PRECISE( HMath::binomialCdf( "5", "10", "0.5" ),
                 "0.62304687500000000000000000000000000000000000000000" );

  CHECK( HMath::binomialMean( "NaN", "NaN" ), "NaN" );
  CHECK( HMath::binomialMean( "NaN", "0.5" ), "NaN" );
  CHECK( HMath::binomialMean( "10",  "NaN" ), "NaN" );
  CHECK( HMath::binomialMean( "10",  "0.5" ), "5"   );
  CHECK( HMath::binomialMean( "-10", "0.5" ), "NaN" );
  CHECK_PRECISE( HMath::binomialMean( "10", "0.5" ),
                 "5.00000000000000000000000000000000000000000000000000" );

  CHECK( HMath::binomialVariance( "NaN", "NaN" ), "NaN" );
  CHECK( HMath::binomialVariance( "NaN", "0.5" ), "NaN" );
  CHECK( HMath::binomialVariance( "10",  "NaN" ), "NaN" );
  CHECK( HMath::binomialVariance( "10",  "0.5" ), "2.5" );
  CHECK( HMath::binomialVariance( "-10", "0.5" ), "NaN" );
  CHECK_PRECISE( HMath::binomialVariance( "10", "0.5" ),
                 "2.50000000000000000000000000000000000000000000000000" );

  // hyper *
  CHECK( HMath::hypergeometricPmf( "NaN", "NaN", "NaN", "NaN" ), "NaN" );
  CHECK( HMath::hypergeometricPmf( "NaN", "NaN", "NaN", "5"   ), "NaN" );
  CHECK( HMath::hypergeometricPmf( "NaN", "NaN", "10",  "NaN" ), "NaN" );
  CHECK( HMath::hypergeometricPmf( "NaN", "NaN", "10",  "5"   ), "NaN" );
  CHECK( HMath::hypergeometricPmf( "1",   "15",  "NaN", "NaN" ), "NaN" );
  CHECK( HMath::hypergeometricPmf( "1",   "15",  "NaN", "5"   ), "NaN" );
  CHECK( HMath::hypergeometricPmf( "1",   "15",  "10",  "NaN" ), "NaN" );
  CHECK( HMath::hypergeometricPmf( "-1",  "15",  "10",  "5"   ), "NaN" );
  CHECK( HMath::hypergeometricPmf( "1",   "15",  "10",  "5"   ),
         "0.01665001665001665002" );
  CHECK_PRECISE( HMath::hypergeometricPmf( "1", "15", "10", "5" ),
                 "0.01665001665001665001665001665001665001665001665002" );

  CHECK( HMath::hypergeometricCdf( "NaN", "NaN", "NaN", "NaN" ), "NaN" );
  CHECK( HMath::hypergeometricCdf( "NaN", "NaN", "NaN", "5"   ), "NaN" );
  CHECK( HMath::hypergeometricCdf( "NaN", "NaN", "10",  "NaN" ), "NaN" );
  CHECK( HMath::hypergeometricCdf( "NaN", "NaN", "10",  "5"   ), "NaN" );
  CHECK( HMath::hypergeometricCdf( "1",   "15",  "NaN", "NaN" ), "NaN" );
  CHECK( HMath::hypergeometricCdf( "1",   "15",  "NaN", "5"   ), "NaN" );
  CHECK( HMath::hypergeometricCdf( "1",   "15",  "10",  "NaN" ), "NaN" );
  CHECK( HMath::hypergeometricCdf( "-1",  "15",  "10",  "5"   ), "NaN" );
  CHECK( HMath::hypergeometricCdf( "1",   "15",  "10",  "5"   ),
         "0.01698301698301698302" );
  CHECK_PRECISE( HMath::hypergeometricCdf( "1", "15", "10", "5" ),
                 "0.01698301698301698301698301698301698301698301698302" );

  CHECK( HMath::hypergeometricMean( "NaN", "NaN", "NaN" ), "NaN" );
  CHECK( HMath::hypergeometricMean( "NaN", "NaN", "5"   ), "NaN" );
  CHECK( HMath::hypergeometricMean( "15",  "10",  "NaN" ), "NaN" );
  CHECK( HMath::hypergeometricMean( "15",  "10",  "5"   ),
         "3.33333333333333333333" );
  CHECK_PRECISE( HMath::hypergeometricMean( "15", "10", "5" ),
                 "3.33333333333333333333333333333333333333333333333333" );

  CHECK( HMath::hypergeometricVariance( "NaN", "NaN", "NaN" ), "NaN" );
  CHECK( HMath::hypergeometricVariance( "NaN", "NaN", "5"   ), "NaN" );
  CHECK( HMath::hypergeometricVariance( "15",  "10",  "NaN" ), "NaN" );
  CHECK( HMath::hypergeometricVariance( "15",  "10",  "5"   ),
         "0.79365079365079365079" );
  CHECK_PRECISE( HMath::hypergeometricVariance( "15", "10", "5" ),
                 "0.79365079365079365079365079365079365079365079365079" );

  // poi *
  CHECK( HMath::poissonPmf( "NaN", "NaN" ), "NaN" );
  CHECK( HMath::poissonPmf( "NaN", "5"   ), "NaN" );
  CHECK( HMath::poissonPmf( "2",   "NaN" ), "NaN" );
  CHECK( HMath::poissonPmf( "-2",  "5"   ), "NaN" );
  CHECK( HMath::poissonPmf( "2",   "5"   ), "0.08422433748856833871" );
  CHECK_PRECISE( HMath::poissonPmf( "2", "5" ),
                 "0.08422433748856833870795060528935530311061981284194" );

  CHECK( HMath::poissonCdf( "NaN", "NaN" ), "NaN" );
  CHECK( HMath::poissonCdf( "NaN", "5"   ), "NaN" );
  CHECK( HMath::poissonCdf( "2",   "NaN" ), "NaN" );
  CHECK( HMath::poissonCdf( "-2",  "5"   ), "NaN" );
  CHECK( HMath::poissonCdf( "2",   "5"   ), "0.12465201948308114129" );
  CHECK_PRECISE( HMath::poissonCdf( "2", "5" ),
                 "0.12465201948308114128776689582824584860371732300607" );

  CHECK( HMath::poissonMean( "NaN" ), "NaN" );
  CHECK( HMath::poissonMean( "5"   ), "5"   );
  CHECK( HMath::poissonMean( "-5"  ), "NaN" );
  CHECK_PRECISE( HMath::poissonMean( "5" ),
                 "5.00000000000000000000000000000000000000000000000000" );

  CHECK( HMath::poissonVariance( "NaN" ), "NaN" );
  CHECK( HMath::poissonVariance( "5"   ), "5"   );
  CHECK( HMath::poissonVariance( "-5"  ), "NaN" );
  CHECK_PRECISE( HMath::poissonVariance( "5" ),
                 "5.00000000000000000000000000000000000000000000000000" );

  // ln
  CHECK( HMath::ln( "NaN"  ), "NaN" );
  CHECK( HMath::ln( "-0.1" ), "NaN" );
  CHECK( HMath::ln( "0"    ), "NaN" );
  CHECK( HMath::ln( "0.00000000001"), "-25.3284360229345025242" );
  CHECK_PRECISE( HMath::ln( "0.1" ),
                 "-2.30258509299404568401799145468436420760110148862877" );
  CHECK_PRECISE( HMath::ln( "0.2" ),
                 "-1.60943791243410037460075933322618763952560135426852" );
  CHECK_PRECISE( HMath::ln( "0.3" ),
                 "-1.20397280432593599262274621776183850295361093080602" );
  CHECK_PRECISE( HMath::ln( "0.4" ),
                 "-0.91629073187415506518352721176801107145010121990826" );
  CHECK_PRECISE( HMath::ln( "0.5" ),
                 "-0.69314718055994530941723212145817656807550013436026" );
  CHECK_PRECISE( HMath::ln( "0.6" ),
                 "-0.51082562376599068320551409630366193487811079644577" );
  CHECK_PRECISE( HMath::ln( "0.7" ),
                 "-0.35667494393873237891263871124118447796401675904691" );
  CHECK_PRECISE( HMath::ln( "0.8" ),
                 "-0.22314355131420975576629509030983450337460108554801" );
  CHECK_PRECISE( HMath::ln( "0.9" ),
                 "-0.10536051565782630122750098083931279830612037298327" );
  CHECK_PRECISE( HMath::ln( "1.0" ),
                 "0.00000000000000000000000000000000000000000000000000"  );
  CHECK_PRECISE( HMath::ln( "1.1" ),
                 "0.09531017980432486004395212328076509222060536530864"  );
  CHECK_PRECISE( HMath::ln( "1.2" ),
                 "0.18232155679395462621171802515451463319738933791449"  );
  CHECK_PRECISE( HMath::ln( "1.3" ),
                 "0.26236426446749105203549598688095439720416645613143"  );
  CHECK_PRECISE( HMath::ln( "1.4" ),
                 "0.33647223662121293050459341021699209011148337531334"  );
  CHECK_PRECISE( HMath::ln( "1.5" ),
                 "0.40546510810816438197801311546434913657199042346249"  );
  CHECK_PRECISE( HMath::ln( "1.6" ),
                 "0.47000362924573555365093703114834206470089904881225"  );
  CHECK_PRECISE( HMath::ln( "1.7" ),
                 "0.53062825106217039623154316318876232798710152395697"  );
  CHECK_PRECISE( HMath::ln( "1.8" ),
                 "0.58778666490211900818973114061886376976937976137698"  );
  CHECK_PRECISE( HMath::ln( "1.9" ),
                 "0.64185388617239477599103597720348932963627777267036"  );
  CHECK_PRECISE( HMath::ln( "2.0" ),
                 "0.69314718055994530941723212145817656807550013436026"  );
  CHECK_PRECISE( HMath::ln( "3.0" ),
                 "1.09861228866810969139524523692252570464749055782275"  );
  CHECK_PRECISE( HMath::ln( "4.0" ),
                 "1.38629436111989061883446424291635313615100026872051"  );
  CHECK_PRECISE( HMath::ln( "100" ),
                 "4.60517018598809136803598290936872841520220297725755"  );

  // log
  CHECK( HMath::log( "NaN"   ), "NaN" );
  CHECK( HMath::log( "-1"    ), "NaN" );
  CHECK( HMath::log( "0"     ), "NaN" );
  CHECK( HMath::log( "1e-5"  ), "-5"  );
  CHECK( HMath::log( "1e-4"  ), "-4"  );
  CHECK( HMath::log( "1e-3"  ), "-3"  );
  CHECK( HMath::log( "10"    ), "1"   );
  CHECK( HMath::log( "100"   ), "2"   );
  CHECK( HMath::log( "1000"  ), "3"   );
  CHECK( HMath::log( "10000" ), "4"   );
  CHECK( HMath::log( "1e5"   ), "5"   );
  CHECK( HMath::log( "1e6"   ), "6"   );
  CHECK( HMath::log( "1e7"   ), "7"   );
  CHECK( HMath::log( "1e8"   ), "8"   );
  CHECK( HMath::log( "1e9"   ), "9"   );
  CHECK( HMath::log( "1e10"  ), "10"  );
  CHECK( HMath::log( "0.00000000001" ), "-11" );

  // lg
  CHECK( HMath::lg( "NaN" ), "NaN" );
  CHECK( HMath::lg( "-1"  ), "NaN" );
  CHECK( HMath::lg( "0"   ), "NaN" );
  CHECK( HMath::lg( "1"   ), "0"   );
  CHECK( HMath::lg( "2"   ), "1"   );
  CHECK( HMath::lg( "4"   ), "2"   );
  CHECK( HMath::lg( "8"   ), "3"   );
  CHECK( HMath::lg( "16"  ), "4"   );
  CHECK( HMath::lg( "32"  ), "5"   );
  CHECK( HMath::lg( "32"  ), "5"   );
  CHECK( HMath::lg( "0.00000000001" ), "-36.54120904376098582657" );

  // sin
  CHECK( HMath::sin( "NaN"   ), "NaN" );
  CHECK( HMath::sin( "0"     ), "0"   );
  CHECK( HMath::sin( PI      ), "0"   );
  CHECK( HMath::sin( PI*2    ), "0"   );
  CHECK( HMath::sin( PI*(-1) ), "0"   );
  CHECK( HMath::sin( PI*(-3) ), "0"   );
  CHECK( HMath::sin( PI*6/3  ), "0"   );
  CHECK( HMath::sin( PI*9/3  ), "0"   );
  CHECK( HMath::sin( PI/2    ), "1"   );
  CHECK( HMath::sin( PI/4    ), "0.7071067811865475244"   );
  CHECK( HMath::sin( PI/3    ), "0.86602540378443864676"  );
  CHECK( HMath::sin( PI*2/3  ), "0.86602540378443864676"  );
  CHECK( HMath::sin( PI*4/3  ), "-0.86602540378443864676" );
  CHECK( HMath::sin( PI*5/3  ), "-0.86602540378443864676" );
  CHECK( HMath::sin( PI*7/3  ), "0.86602540378443864676"  );
  CHECK_PRECISE( HMath::sin( "0.0" ),
                 "0.00000000000000000000000000000000000000000000000000"  );
  CHECK_PRECISE( HMath::sin( "0.1" ),
                 "0.09983341664682815230681419841062202698991538801798"  );
  CHECK_PRECISE( HMath::sin( "0.2" ),
                 "0.19866933079506121545941262711838975037020672954021"  );
  CHECK_PRECISE( HMath::sin( "0.3" ),
                 "0.29552020666133957510532074568502737367783211174262"  );
  CHECK_PRECISE( HMath::sin( "0.4" ),
                 "0.38941834230865049166631175679570526459306018344396"  );
  CHECK_PRECISE( HMath::sin( "0.5" ),
                 "0.47942553860420300027328793521557138808180336794060"  );
  CHECK_PRECISE( HMath::sin( "0.6" ),
                 "0.56464247339503535720094544565865790710988808499415"  );
  CHECK_PRECISE( HMath::sin( "0.7" ),
                 "0.64421768723769105367261435139872018306581384457369"  );
  CHECK_PRECISE( HMath::sin( "0.8" ),
                 "0.71735609089952276162717461058138536619278523779142"  );
  CHECK_PRECISE( HMath::sin( "0.9" ),
                 "0.78332690962748338846138231571354862314014792572031"  );
  CHECK_PRECISE( HMath::sin( "1.0" ),
                 "0.84147098480789650665250232163029899962256306079837"  );
  CHECK_PRECISE( HMath::sin( "2.0" ),
                 "0.90929742682568169539601986591174484270225497144789"  );
  CHECK_PRECISE( HMath::sin( "3.0" ),
                 "0.14112000805986722210074480280811027984693326425227"  );
  CHECK_PRECISE( HMath::sin( "4.0" ),
                 "-0.75680249530792825137263909451182909413591288733647" );
  CHECK_PRECISE( HMath::sin( "5.0" ),
                 "-0.95892427466313846889315440615599397335246154396460" );
  CHECK_PRECISE( HMath::sin( "1e22" ),
                 "-0.85220084976718880177270589375302936826176215041004" );

  // cos
  CHECK( HMath::cos( "NaN"       ), "NaN"  );
  CHECK( HMath::cos( PI/2        ), "0"    );
  CHECK( HMath::cos( PI*3/2      ), "0"    );
  CHECK( HMath::cos( PI/2*(-1)*9 ), "0"    );
  CHECK( HMath::cos( 0           ), "1"    );
  CHECK( HMath::cos( PI*6/3      ), "1"    );
  CHECK( HMath::cos( PI*9/3      ), "-1"   );
  CHECK( HMath::cos( PI/1        ), "-1"   );
  CHECK( HMath::cos( PI/3        ), "0.5"  );
  CHECK( HMath::cos( PI*2/3      ), "-0.5" );
  CHECK( HMath::cos( PI*4/3      ), "-0.5" );
  CHECK( HMath::cos( PI*5/3      ), "0.5"  );
  CHECK( HMath::cos( PI*7/3      ), "0.5"  );
  CHECK( HMath::cos( PI/4        ), "0.7071067811865475244" );
  CHECK_PRECISE( HMath::cos( "0.0" ),
                 "1.00000000000000000000000000000000000000000000000000" );
  CHECK_PRECISE( HMath::cos( "0.1" ),
                 "0.99500416527802576609556198780387029483857622541508" );
  CHECK_PRECISE( HMath::cos( "0.2" ),
                 "0.98006657784124163112419651674816887739352436080657" );
  CHECK_PRECISE( HMath::cos( "0.3" ),
                 "0.95533648912560601964231022756804989824421408263204" );
  CHECK_PRECISE( HMath::cos( "0.4" ),
                 "0.92106099400288508279852673205180161402585956931985" );
  CHECK_PRECISE( HMath::cos( "0.5" ),
                 "0.87758256189037271611628158260382965199164519710974" );
  CHECK_PRECISE( HMath::cos( "0.6" ),
                 "0.82533561490967829724095249895537603887809103918847" );
  CHECK_PRECISE( HMath::cos( "0.7" ),
                 "0.76484218728448842625585999019186490926821055037370" );
  CHECK_PRECISE( HMath::cos( "0.8" ),
                 "0.69670670934716542092074998164232492610178601370806" );
  CHECK_PRECISE( HMath::cos( "0.9" ),
                 "0.62160996827066445648471615140713350872176136659124" );
  CHECK_PRECISE( HMath::cos( "1.0" ),
                 "0.54030230586813971740093660744297660373231042061792" );
  CHECK_PRECISE( HMath::cos( "2.0" ),
                 "-0.41614683654714238699756822950076218976600077107554" );
  CHECK_PRECISE( HMath::cos( "3.0" ),
                 "-0.98999249660044545727157279473126130239367909661559" );
  CHECK_PRECISE( HMath::cos( "4.0" ),
                 "-0.65364362086361191463916818309775038142413359664622" );
  CHECK_PRECISE( HMath::cos( "1e22" ),
                 "0.52321478539513894549759447338470949214091997243939"  );

  // tan
  CHECK( HMath::tan( "NaN"     ), "NaN" );
  CHECK( HMath::tan( PI/2      ), "NaN" );
  CHECK( HMath::tan( PI/2*3    ), "NaN" );
  CHECK( HMath::tan( PI/2*5    ), "NaN" );
  CHECK( HMath::tan( PI/2*(-3) ), "NaN" );
  CHECK( HMath::tan( PI/2*(-5) ), "NaN" );
  CHECK( HMath::tan( PI/1      ), "0"   );
  CHECK( HMath::tan( PI*(-3)   ), "0"   );
  CHECK( HMath::tan( PI*11     ), "0"   );
  CHECK( HMath::tan( PI/4      ), "1"   );
  CHECK( HMath::tan( PI/3      ), "1.73205080756887729353" );
  CHECK_PRECISE( HMath::tan( "0.0" ),
                 "0.00000000000000000000000000000000000000000000000000"  );
  CHECK_PRECISE( HMath::tan( "0.1" ),
                 "0.10033467208545054505808004578111153681900480457644"  );
  CHECK_PRECISE( HMath::tan( "0.2" ),
                 "0.20271003550867248332135827164753448262687566965163"  );
  CHECK_PRECISE( HMath::tan( "0.3" ),
                 "0.30933624960962323303530367969829466725781590680046"  );
  CHECK_PRECISE( HMath::tan( "0.4" ),
                 "0.42279321873816176198163542716529033394198977271569"  );
  CHECK_PRECISE( HMath::tan( "0.5" ),
                 "0.54630248984379051325517946578028538329755172017979"  );
  CHECK_PRECISE( HMath::tan( "0.6" ),
                 "0.68413680834169231707092541746333574524265408075678"  );
  CHECK_PRECISE( HMath::tan( "0.7" ),
                 "0.84228838046307944812813500221293771718722125080420"  );
  CHECK_PRECISE( HMath::tan( "0.8" ),
                 "1.02963855705036401274636117282036528416821960677231"  );
  CHECK_PRECISE( HMath::tan( "0.9" ),
                 "1.26015821755033913713457548539574847783362583439629"  );
  CHECK_PRECISE( HMath::tan( "1.0" ),
                 "1.55740772465490223050697480745836017308725077238152"  );
  CHECK_PRECISE( HMath::tan( "2.0" ),
                 "-2.18503986326151899164330610231368254343201774622766" );
  CHECK_PRECISE( HMath::tan( "3.0" ),
                 "-0.14254654307427780529563541053391349322609228490180" );
  CHECK_PRECISE( HMath::tan( "4.0" ),
                 "1.15782128234957758313734241826732392311976276736714"  );

  // cot
  CHECK( HMath::cot( "NaN"     ), "NaN" );
  CHECK( HMath::cot( 0         ), "NaN" );
  CHECK( HMath::cot( PI        ), "NaN" );
  CHECK( HMath::cot( PI*2      ), "NaN" );
  CHECK( HMath::cot( PI*3      ), "NaN" );
  CHECK( HMath::cot( PI*(-1)   ), "NaN" );
  CHECK( HMath::cot( PI*(-2)   ), "NaN" );
  CHECK( HMath::cot( PI*(-3)   ), "NaN" );
  CHECK( HMath::cot( PI/2      ), "0"   );
  CHECK( HMath::cot( PI/2*3    ), "0"   );
  CHECK( HMath::cot( PI/2*5    ), "0"   );
  CHECK( HMath::cot( PI/(-2)   ), "0"   );
  CHECK( HMath::cot( PI/(-2)*3 ), "0"   );
  CHECK( HMath::cot( PI/(-2)*5 ), "0"   );
  CHECK_PRECISE( HMath::cot( "0.1" ),
                 "9.96664442325923785979411268927059390763024832915145"  );
  CHECK_PRECISE( HMath::cot( "0.2" ),
                 "4.93315487558689365736801632174474118540562176228750"  );
  CHECK_PRECISE( HMath::cot( "0.3" ),
                 "3.23272814376582751371392053451257979612306377207793"  );
  CHECK_PRECISE( HMath::cot( "0.4" ),
                 "2.36522242003911058702332902504860335138937304631794"  );
  CHECK_PRECISE( HMath::cot( "0.5" ),
                 "1.83048772171245191926801943896881662375810794801613"  );
  CHECK_PRECISE( HMath::cot( "0.6" ),
                 "1.46169594707810214033930842740714256443262393263873"  );
  CHECK_PRECISE( HMath::cot( "0.7" ),
                 "1.18724183212667935367236269369115744209994867903861"  );
  CHECK_PRECISE( HMath::cot( "0.8" ),
                 "0.97121460065047441252084679894165650872369163680112"  );
  CHECK_PRECISE( HMath::cot( "0.9" ),
                 "0.79355114784231712550449309982343541572162687887550"  );
  CHECK_PRECISE( HMath::cot( "1.0" ),
                 "0.64209261593433070300641998659426562023027811391817"  );
  CHECK_PRECISE( HMath::cot( "2.0" ),
                 "-0.45765755436028576375027741043204727642848632923167" );
  CHECK_PRECISE( HMath::cot( "3.0" ),
                 "-7.01525255143453346942855137952647657829310335209635" );
  CHECK_PRECISE( HMath::cot( "4.0" ),
                 "0.86369115445061661394651434594081763350176570849799"  );

  // sec
  CHECK( HMath::sec( "NaN"     ), "NaN" );
  CHECK( HMath::sec( 0         ), "1"   );
  CHECK( HMath::sec( PI        ), "-1"  );
  CHECK( HMath::sec( PI*2      ), "1"   );
  CHECK( HMath::sec( PI*3      ), "-1"  );
  CHECK( HMath::sec( PI*(-1)   ), "-1"  );
  CHECK( HMath::sec( PI*(-2)   ), "1"   );
  CHECK( HMath::sec( PI*(-3)   ), "-1"  );
  CHECK( HMath::sec( PI/2      ), "NaN" );
  CHECK( HMath::sec( PI/2*3    ), "NaN" );
  CHECK( HMath::sec( PI/2*5    ), "NaN" );
  CHECK( HMath::sec( PI/(-2)   ), "NaN" );
  CHECK( HMath::sec( PI/(-2)*3 ), "NaN" );
  CHECK( HMath::sec( PI/(-2)*5 ), "NaN" );
  CHECK_PRECISE( HMath::sec( "0.1" ),
                 "1.00502091840045542846511410131163581939665289709269"  );
  CHECK_PRECISE( HMath::sec( "0.2" ),
                 "1.02033884494119268979243277684919018946197757380999"  );
  CHECK_PRECISE( HMath::sec( "0.3" ),
                 "1.04675160153808560093279084558703692841456893499328"  );
  CHECK_PRECISE( HMath::sec( "0.4" ),
                 "1.08570442838323870311392995515876310121797526730283"  );
  CHECK_PRECISE( HMath::sec( "0.5" ),
                 "1.13949392732454912231332776820494992842372524604900"  );
  CHECK_PRECISE( HMath::sec( "0.6" ),
                 "1.21162831451231670461455512137241473057527998567989"  );
  CHECK_PRECISE( HMath::sec( "0.7" ),
                 "1.30745925973359386987467283530531145422836345495873"  );
  CHECK_PRECISE( HMath::sec( "0.8" ),
                 "1.43532419967223980049695240816284402319218304405383"  );
  CHECK_PRECISE( HMath::sec( "0.9" ),
                 "1.60872581046604951302401286230639310432053967212693"  );
  CHECK_PRECISE( HMath::sec( "1.0" ),
                 "1.85081571768092561791175324139865019347039665509401"  );
  CHECK_PRECISE( HMath::sec( "2.0" ),
                 "-2.40299796172238098975460040142006622624512109315453" );
  CHECK_PRECISE( HMath::sec( "3.0" ),
                 "-1.01010866590799375130303648146319295518501902819060" );
  CHECK_PRECISE( HMath::sec( "4.0" ),
                 "-1.52988565646639757462951092293735504703880002436905" );

  // csc
  CHECK( HMath::csc( "NaN"     ), "NaN" );
  CHECK( HMath::csc( 0         ), "NaN" );
  CHECK( HMath::csc( PI        ), "NaN" );
  CHECK( HMath::csc( PI*2      ), "NaN" );
  CHECK( HMath::csc( PI*3      ), "NaN" );
  CHECK( HMath::csc( PI*(-1)   ), "NaN" );
  CHECK( HMath::csc( PI*(-2)   ), "NaN" );
  CHECK( HMath::csc( PI*(-3)   ), "NaN" );
  CHECK( HMath::csc( PI/2      ), "1"   );
  CHECK( HMath::csc( PI/2*3    ), "-1"  );
  CHECK( HMath::csc( PI/2*5    ), "1"   );
  CHECK( HMath::csc( PI/(-2)   ), "-1"  );
  CHECK( HMath::csc( PI/(-2)*3 ), "1"   );
  CHECK( HMath::csc( PI/(-2)*5 ), "-1"  );
  CHECK_PRECISE( HMath::csc( "0.1" ),
                 "10.01668613163477664870635254208068373689462246452006" );
  CHECK_PRECISE( HMath::csc( "0.2" ),
                 "5.03348954767234420242609636752585272222462656686394"  );
  CHECK_PRECISE( HMath::csc( "0.3" ),
                 "3.38386336182412258496047655147742034395467222595556"  );
  CHECK_PRECISE( HMath::csc( "0.4" ),
                 "2.56793245554778307034468729669613783401624871596957"  );
  CHECK_PRECISE( HMath::csc( "0.5" ),
                 "2.08582964293348818577250167545929030196230958681696"  );
  CHECK_PRECISE( HMath::csc( "0.6" ),
                 "1.77103219668772537337461210710543723169043983943920"  );
  CHECK_PRECISE( HMath::csc( "0.7" ),
                 "1.55227032695710391198997364095492783063640855362864"  );
  CHECK_PRECISE( HMath::csc( "0.8" ),
                 "1.39400781938863617450248222610694684266568140951681"  );
  CHECK_PRECISE( HMath::csc( "0.9" ),
                 "1.27660621345889549601564565223201364314354454128779"  );
  CHECK_PRECISE( HMath::csc( "1.0" ),
                 "1.18839510577812121626159945237455100352782983409796"  );
  CHECK_PRECISE( HMath::csc( "2.0" ),
                 "1.09975017029461646675669739702631289665876444314985"  );
  CHECK_PRECISE( HMath::csc( "3.0" ),
                 "7.08616739573718591821753227246127986736644022513951"  );
  CHECK_PRECISE( HMath::csc( "4.0" ),
                 "-1.32134870881090237769679175637286490993025203772967" );

  // atan
  CHECK( HMath::atan( "NaN" ), "NaN" );
  CHECK( HMath::atan( "0.10033467208545054505808004578111153681900480457644" ),
         "0.1" );
  CHECK( HMath::atan( "0.20271003550867248332135827164753448262687566965163" ),
         "0.2" );
  CHECK( HMath::atan( "0.30933624960962323303530367969829466725781590680046" ),
         "0.3" );
  CHECK( HMath::atan( "0.42279321873816176198163542716529033394198977271569" ),
         "0.4" );
  CHECK( HMath::atan( "0.54630248984379051325517946578028538329755172017979" ),
         "0.5" );
  CHECK( HMath::atan( "0.68413680834169231707092541746333574524265408075678" ),
         "0.6" );
  CHECK( HMath::atan( "0.84228838046307944812813500221293771718722125080420" ),
         "0.7" );
  CHECK( HMath::atan( "1.02963855705036401274636117282036528416821960677231" ),
         "0.8" );
  CHECK( HMath::atan( "1.26015821755033913713457548539574847783362583439629" ),
         "0.9" );
  CHECK( HMath::atan( "1.55740772465490223050697480745836017308725077238152" ),
         "1"   );
  CHECK_PRECISE( HMath::atan( "0.0"  ),
                 "0.00000000000000000000000000000000000000000000000000"  );
  CHECK_PRECISE( HMath::atan( "0.1"  ),
                 "0.09966865249116202737844611987802059024327832250431"  );
  CHECK_PRECISE( HMath::atan( "0.2"  ),
                 "0.19739555984988075837004976519479029344758510378785"  );
  CHECK_PRECISE( HMath::atan( "0.3"  ),
                 "0.29145679447786709199560462143289119350316759901207"  );
  CHECK_PRECISE( HMath::atan( "0.4"  ),
                 "0.38050637711236488630358791681043310449740571365810"  );
  CHECK_PRECISE( HMath::atan( "0.5"  ),
                 "0.46364760900080611621425623146121440202853705428612"  );
  CHECK_PRECISE( HMath::atan( "0.6"  ),
                 "0.54041950027058415544357836460859991013514825146259"  );
  CHECK_PRECISE( HMath::atan( "1.0"  ),
                 "0.78539816339744830961566084581987572104929234984378"  );
  CHECK_PRECISE( HMath::atan( "-0.1" ),
                 "-0.09966865249116202737844611987802059024327832250431" );
  CHECK_PRECISE( HMath::atan( "-0.2" ),
                 "-0.19739555984988075837004976519479029344758510378785" );
  CHECK_PRECISE( HMath::atan( "-0.3" ),
                 "-0.29145679447786709199560462143289119350316759901207" );
  CHECK_PRECISE( HMath::atan( "-0.4" ),
                 "-0.38050637711236488630358791681043310449740571365810" );
  CHECK_PRECISE( HMath::atan( "-0.5" ),
                 "-0.46364760900080611621425623146121440202853705428612" );
  CHECK_PRECISE( HMath::atan( "-0.6" ),
                 "-0.54041950027058415544357836460859991013514825146259" );
  CHECK_PRECISE( HMath::atan( "-1.0" ),
                 "-0.78539816339744830961566084581987572104929234984378" );

  // asin
  CHECK( HMath::asin( "NaN" ), "NaN"                     );
  CHECK( HMath::asin( "-1"  ), "-1.57079632679489661923" );
  CHECK( HMath::asin( "0"   ), "0"                       );
  CHECK( HMath::asin( "1"   ), "1.57079632679489661923"  );
  CHECK( HMath::asin( "0.09983341664682815230681419841062202698991538801798" ),
         "0.1" );
  CHECK( HMath::asin( "0.19866933079506121545941262711838975037020672954021" ),
         "0.2" );
  CHECK( HMath::asin( "0.29552020666133957510532074568502737367783211174262" ),
         "0.3" );
  CHECK( HMath::asin( "0.38941834230865049166631175679570526459306018344396" ),
         "0.4" );
  CHECK( HMath::asin( "0.47942553860420300027328793521557138808180336794060" ),
         "0.5" );
  CHECK( HMath::asin( "0.56464247339503535720094544565865790710988808499415" ),
         "0.6" );
  CHECK( HMath::asin( "0.64421768723769105367261435139872018306581384457369" ),
         "0.7" );
  CHECK( HMath::asin( "0.71735609089952276162717461058138536619278523779142" ),
         "0.8" );
  CHECK_PRECISE( HMath::asin( "0.0" ),
                 "0.00000000000000000000000000000000000000000000000000" );
  CHECK_PRECISE( HMath::asin( "0.1" ),
                 "0.10016742116155979634552317945269331856867597222963" );
  CHECK_PRECISE( HMath::asin( "0.2" ),
                 "0.20135792079033079145512555221762341024003808140223" );
  CHECK_PRECISE( HMath::asin( "0.3" ),
                 "0.30469265401539750797200296122752916695456003170678" );
  CHECK_PRECISE( HMath::asin( "0.4" ),
                 "0.41151684606748801938473789761733560485570113512703" );

  // acos
  CHECK( HMath::acos( "NaN" ), "NaN"                    );
  CHECK( HMath::acos( "-1"  ), "3.14159265358979323846" );
  CHECK( HMath::acos( "0"   ), "1.57079632679489661923" );
  CHECK( HMath::acos( "1"   ), "0"                      );
  CHECK_PRECISE( HMath::acos( "0.1" ),
                 "1.47062890563333682288579851218705812352990872745792" );
  CHECK_PRECISE( HMath::acos( "0.2" ),
                 "1.36943840600456582777619613942212803185854661828532" );
  CHECK_PRECISE( HMath::acos( "0.3" ),
                 "1.26610367277949911125931873041222227514402466798078" );
  CHECK_PRECISE( HMath::acos( "0.4" ),
                 "1.15927948072740859984658379402241583724288356456053" );

  // sinh
  CHECK( HMath::sinh( "NaN" ), "NaN" );
  CHECK_PRECISE( HMath::sinh( "0.1" ),
                 "0.10016675001984402582372938352190502351492091687856" );
  CHECK_PRECISE( HMath::sinh( "0.2" ),
                 "0.20133600254109398762556824301031737297449484262574" );
  CHECK_PRECISE( HMath::sinh( "0.3" ),
                 "0.30452029344714261895843526700509522909802423268018" );
  CHECK_PRECISE( HMath::sinh( "0.4" ),
                 "0.41075232580281550854021001384469810435315092436331" );
  CHECK_PRECISE( HMath::sinh( "0.5" ),
                 "0.52109530549374736162242562641149155910592898261148" );
  CHECK_PRECISE( HMath::sinh( "0.6" ),
                 "0.63665358214824127112345437546514831902496342592790" );
  CHECK_PRECISE( HMath::sinh( "0.7" ),
                 "0.75858370183953350345987464759276815415493761421703" );
  CHECK_PRECISE( HMath::sinh( "0.8" ),
                 "0.88810598218762300657471757318975698055970959688815" );
  CHECK_PRECISE( HMath::sinh( "0.9" ),
                 "1.02651672570817527595833616197842235379403446513485" );
  CHECK_PRECISE( HMath::sinh( "1.0" ),
                 "1.17520119364380145688238185059560081515571798133410" );

  // cosh
  CHECK( HMath::cosh( "NaN" ), "NaN" );
  CHECK_PRECISE( HMath::cosh( "0.1" ),
                 "1.00500416805580359898797844296834164470962627785896" );
  CHECK_PRECISE( HMath::cosh( "0.2" ),
                 "1.02006675561907584629550375162935679733308609889476" );
  CHECK_PRECISE( HMath::cosh( "0.3" ),
                 "1.04533851412886048502530904632291210128027546467919" );
  CHECK_PRECISE( HMath::cosh( "0.4" ),
                 "1.08107237183845480928464293899252417629013184957412" );
  CHECK_PRECISE( HMath::cosh( "0.5" ),
                 "1.12762596520638078522622516140267201254784711809867" );
  CHECK_PRECISE( HMath::cosh( "0.6" ),
                 "1.18546521824226770375191329269771619435727538261853" );
  CHECK_PRECISE( HMath::cosh( "0.7" ),
                 "1.25516900563094301816467474099029711586260477992884" );
  CHECK_PRECISE( HMath::cosh( "0.8" ),
                 "1.33743494630484459800481995820531977649392453816033" );
  CHECK_PRECISE( HMath::cosh( "0.9" ),
                 "1.43308638544877438784179040162404834162773784130523" );
  CHECK_PRECISE( HMath::cosh( "1.0" ),
                 "1.54308063481524377847790562075706168260152911236586" );
}


int main( int argc, char * * argv )
{
  hmath_total_tests  = 0;
  hmath_failed_tests = 0;

  test_create();
  test_format();
  test_op();
  test_functions();

  std::cerr << hmath_total_tests  << " total, "
            << hmath_failed_tests << " failed"
            << endl;

  HMath::finalize();
  return hmath_failed_tests;
}
