/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.0, released July  2011                                        *
 *   Copyright (c) 2011                                                      *
 *                                                                           *
 *   PLAST is free software; you can redistribute it and/or modify it under  *
 *   the CECILL version 2 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   CECILL version 2 License for more details.                              *
 *****************************************************************************/

#include <database/api/IAlphabet.hpp>

/********************************************************************************/
namespace database {
/********************************************************************************/

static LETTER subseedLetters[]  = { 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  21,  22,  25,  26,  27  };
static LETTER ascciiLetters[]   = {'A', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'V', 'W', 'Y', 'B', 'Z', 'U', 'O', 'J' };
static LETTER ncbiLetters[]     = { 1,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  22,   2,  23,  24,  26,  27  };

/********************************************************************************/

static IAlphabet alphabetsTable[] =
{
    {
       "subseed",

       subseedLetters,
       20, //sizeof(subseedLetters) / sizeof(LETTER),

       20, 23, 24
    },

    {
       "ascii",

       ascciiLetters,
       20, //sizeof(ascciiLetters) / sizeof(LETTER),

       'X', '*', '-'
    },

    {
       "ncbi",

       ncbiLetters,
       20, //sizeof(ncbiLetters) / sizeof(LETTER),

       21, 25, 0
    }
};


/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IAlphabet* EncodingManager::getAlphabet (Encoding encoding)
{
    return & alphabetsTable [encoding];
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
const LETTER* EncodingManager::getEncodingConversion (Encoding from, Encoding to)
{
    static const LETTER Encoding_ascii2subseed [] = {

    CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD,
    CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD,

    CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_STAR, CODE_BAD, CODE_BAD, CODE_DASH,CODE_BAD, CODE_BAD,
    CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD,  CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD,

    CODE_BAD,   CODE_A,   CODE_B,   CODE_C,   CODE_D,   CODE_E,   CODE_F,   CODE_G,   CODE_H,   CODE_I,    CODE_J,   CODE_K,   CODE_L,   CODE_M,   CODE_N,   CODE_O,
      CODE_P,   CODE_Q,   CODE_R,   CODE_S,   CODE_T,   CODE_U,   CODE_V,   CODE_W,   CODE_X,   CODE_Y,    CODE_Z, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD,

    CODE_BAD,   CODE_A,   CODE_B,   CODE_C,   CODE_D,   CODE_E,   CODE_F,   CODE_G,   CODE_H,   CODE_I,    CODE_J,   CODE_K,   CODE_L,   CODE_M,   CODE_N,   CODE_O,
      CODE_P,   CODE_Q,   CODE_R,   CODE_S,   CODE_T,   CODE_U,   CODE_V,   CODE_W,   CODE_X,   CODE_Y,    CODE_Z, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD, CODE_BAD
    };

    static const LETTER Encoding_ascii2ncbi [] = {

     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,25, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

     0, 1, 2, 3, 4, 5, 6, 7, 8, 9,27,10,11,12,13,26,
    14,15,16,17,18,24,19,20,21,22,23, 0, 0, 0, 0, 0,

     0, 1, 2, 3, 4, 5, 6, 7, 8, 9,27,10,11,12,13,26,
    14,15,16,17,18,24,19,20,21,22,23, 0, 0, 0, 0, 0,
    };

    static const LETTER Encoding_subseed2ascii [] = {
    'A','C','D','E','F','G','H','I','K','L','M','N',
    'P','Q','R','S','T','V','W','Y','X','B','Z','*',
    '-','U','O','J'
    };

    static const LETTER Encoding_subseed2ncbi [] = {
      1,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13,
     14, 15, 16, 17, 18, 19, 20, 22, 21,  2, 23, 25,
      0, 24, 26, 27
    };

    static const LETTER Encoding_ncbi2subseed [] = {
        24,  0, 21,  1,  2,  3,  4,  5,  6,  7,  8,  9,
        10, 11, 12, 13, 14, 15, 16, 17, 18, 20, 19, 22,
        25, 23, 26, 27
    };

    /** We define the result object. */
    const LETTER* result = 0;

         if (from==SUBSEED && to==ASCII)    { result = Encoding_subseed2ascii;  }
    else if (from==SUBSEED && to==NCBI)     { result = Encoding_subseed2ncbi;   }
    else if (from==ASCII   && to==SUBSEED)  { result = Encoding_ascii2subseed;  }
    else if (from==ASCII   && to==NCBI)     { result = Encoding_ascii2ncbi;     }
    else if (from==NCBI    && to==SUBSEED)  { result = Encoding_ncbi2subseed;   }
    else                                    { result = 0;                       }

    return result;
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

