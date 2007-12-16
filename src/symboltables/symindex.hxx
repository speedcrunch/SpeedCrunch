/* symindex.h: indices of built-in symbols
   Copyright (C) 2007 Wolf Lammen ookami1 <at> gmx <dot> de

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

/* syntactical elements */
enum {symSep = 0,              /* separates parameters = ; */
      symOpenPar,              /* open parenthesis     = ( */
      symClosePar,             /* close parenthesis    = ) */
};

enum {symDecTag = 1000,        /* prefix indicating a decimal encoding = 0d */
};

/* mathematical constants */
enum {symPi = 2000,            /* 3.1415926.. */
      symPhi,                  /* golden number = (1+sqrt(5))/2 */
};

/* astronomical constants */
enum {symAU = 3000,            /* astronomical unit AU */
};

/* simple mathematical functions */
enum {symSqrt = 4000,          /* square root */
};

/* statistics */
enum {symnPr = 5000,           /* r-size subsets in a n-size set */
      symHyperMean,            /* hypergeometric mean */
};

enum {symPlus = 6000,          /* unary or binary plus */
      symMinus,                /* unary or binary minus */
};
