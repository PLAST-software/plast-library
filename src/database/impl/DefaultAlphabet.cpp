/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.3, released November 2015                                     *
 *   Copyright (c) 2009-2015 Inria-Cnrs-Ens                                  *
 *                                                                           *
 *   PLAST is free software; you can redistribute it and/or modify it under  *
 *   the Affero GPL ver 3 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   Affero GPL ver 3 License for more details.                              *
 *****************************************************************************/

#include <database/api/IAlphabet.hpp>

#include <iostream>

using namespace std;

#define DEBUG(a)  //a

/********************************************************************************/
namespace database {
/********************************************************************************/

static LETTER subseedLetters_aminoacid[]  = { 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  21,  22,  25,  26,  27  };
static LETTER ascciiLetters_aminoacid[]   = {'A', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'V', 'W', 'Y', 'B', 'Z', 'U', 'O', 'J' };
static LETTER ncbiLetters_aminoacid[]     = { 1,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  22,   2,  23,  24,  26,  27  };

static IAlphabet alphabetsTable_aminoacid[] =
{
    {
       "subseed",

       subseedLetters_aminoacid,
       20, //sizeof(subseedLetters) / sizeof(LETTER),

       20, 23, 24
    },

    {
       "ascii",

       ascciiLetters_aminoacid,
       20, //sizeof(ascciiLetters) / sizeof(LETTER),

       'X', '*', '-'
    },

    {
       "ncbi",

       ncbiLetters_aminoacid,
       20, //sizeof(ncbiLetters) / sizeof(LETTER),

       21, 25, 0
    }
};

/********************************************************************************/

static LETTER subseedLetters_nucleotid[]  = { 0,   1,   3,   2,   4,   5 };
static LETTER ascciiLetters_nucleotid[]   = {'A', 'C', 'G', 'T', 'N', '-'};
static LETTER ncbiLetters_nucleotid[]     = { 0,   1,   2,   3,   4,   5 };

static IAlphabet alphabetsTable_nucleotid[] =
{
    {
       "subseed",

       subseedLetters_nucleotid,
       4,

       4, 4, 5
    },

    {
       "ascii",

       ascciiLetters_nucleotid,
       4,

       'N', 'N', '-'
    },

    {
       "ncbi",

       ncbiLetters_nucleotid,
       4,

       4, 4, 5
    },

    {
       "ncbi",

       ncbiLetters_nucleotid,
       4,

       4, 4, 5
    },

    {
       "ncbi",

       ncbiLetters_nucleotid,
       4,

       4, 4, 5
    }
};

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
    IAlphabet* result = 0;

    DEBUG (cout << "EncodingManager::getAlphabet:"
        << "   _kind="    << _kind
        << "   encoding=" << encoding
        << endl
    );

    if (_kind == ALPHABET_AMINO_ACID)
    {
        result = & alphabetsTable_aminoacid [encoding];
    }
    else if (_kind == ALPHABET_NUCLEOTID)
    {
        result = & alphabetsTable_nucleotid [encoding];
    }
    else
    {
        throw exception ();
    }

    return result;
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
    switch (_kind)
    {
        case ALPHABET_NUCLEOTID:    return getEncodingConversion_nucleotid (from, to);
        case ALPHABET_AMINO_ACID:   return getEncodingConversion_aminoacid (from, to);

        default:
            throw exception();
            return 0;
    }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
const LETTER* EncodingManager::getEncodingConversion_aminoacid (Encoding from, Encoding to)
{
    static const LETTER bad = CODE_X;

    static const LETTER Encoding_ascii2subseed [] = {

      bad,      bad,      bad,      bad,      bad,      bad,      bad,      bad,      bad,      bad,      bad,      bad,      bad,      bad,       bad,      bad,
      bad,      bad,      bad,      bad,      bad,      bad,      bad,      bad,      bad,      bad,      bad,      bad,      bad,      bad,       bad,      bad,

      bad,      bad,      bad,      bad,      bad,      bad,      bad,      bad,      bad,      bad,       CODE_STAR, bad,     bad,      CODE_DASH,bad,      bad,
      bad,      bad,      bad,      bad,      bad,      bad,      bad,      bad,      bad,      bad,       bad,      bad,      bad,      bad,      bad,      bad,

      bad,      CODE_A,   CODE_B,   CODE_C,   CODE_D,   CODE_E,   CODE_F,   CODE_G,   CODE_H,   CODE_I,    CODE_J,   CODE_K,   CODE_L,   CODE_M,   CODE_N,   CODE_O,
      CODE_P,   CODE_Q,   CODE_R,   CODE_S,   CODE_T,   CODE_U,   CODE_V,   CODE_W,   CODE_X,   CODE_Y,    CODE_Z,   bad,      bad,      bad,      bad,      bad,

      bad,      CODE_A,   CODE_B,   CODE_C,   CODE_D,   CODE_E,   CODE_F,   CODE_G,   CODE_H,   CODE_I,    CODE_J,   CODE_K,   CODE_L,   CODE_M,   CODE_N,   CODE_O,
      CODE_P,   CODE_Q,   CODE_R,   CODE_S,   CODE_T,   CODE_U,   CODE_V,   CODE_W,   CODE_X,   CODE_Y,    CODE_Z,   bad,      bad,      bad,      bad,      bad
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

    static const LETTER Encoding_ncbi2subseed_DNA [] = { 0, 1, 5, 16, 11, 24 };
    static const LETTER Encoding_ncbiAmb2subseed_DNA [] = { 24, 0, 1, 11, 5, 11, 11, 11, 16, 11, 11, 11, 11, 11, 11, 11 };

    /** We define the result object. */
    const LETTER* result = 0;

         if (from==SUBSEED && to==ASCII)    { result = Encoding_subseed2ascii;  }
    else if (from==SUBSEED && to==NCBI)     { result = Encoding_subseed2ncbi;   }
    else if (from==ASCII   && to==SUBSEED)  { result = Encoding_ascii2subseed;  }
    else if (from==ASCII   && to==NCBI)     { result = Encoding_ascii2ncbi;     }
    else if (from==NCBI    && to==SUBSEED)  { result = Encoding_ncbi2subseed;   }
    else if (from==NCBI_DNA_NO_AMB 	&& to==SUBSEED)  			{ result = Encoding_ncbi2subseed_DNA;   }
    else if (from==NCBI_DNA_WITH_AMB&& to==SUBSEED)  			{ result = Encoding_ncbiAmb2subseed_DNA;}
    else                                    {
    	result = 0;
    }

    return result;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
const LETTER* EncodingManager::getEncodingConversion_nucleotid (Encoding from, Encoding to)
{
    static const LETTER bad = 4;

    static const LETTER Encoding_ascii2subseed [] = {

    bad, bad, bad, bad, bad, bad, bad, bad, bad, bad, bad, bad, bad, bad, bad, bad,
    bad, bad, bad, bad, bad, bad, bad, bad, bad, bad, bad, bad, bad, bad, bad, bad,

    bad, bad, bad, bad, bad, bad, bad, bad, bad, bad, bad, bad, bad, bad, bad, bad,
    bad, bad, bad, bad, bad, bad, bad, bad, bad, bad, bad, bad, bad, bad, bad, bad,

    bad,   0, bad,   1, bad, bad, bad,   3, bad, bad, bad, bad, bad, bad,   4, bad,
    bad, bad, bad, bad,   2,   2, bad, bad, bad, bad, bad, bad, bad, bad, bad, bad,

    bad,   0, bad,   1, bad, bad, bad,   3, bad, bad, bad, bad, bad, bad,   4, bad,
    bad, bad, bad, bad,   2,   2, bad, bad, bad, bad, bad, bad, bad, bad, bad, bad
};

    static const LETTER Encoding_ascii2ncbi [] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

        0, 0, 1, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 4, 0, 0,
        0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

        0, 0, 1, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 4, 0, 0,
        0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    static const LETTER Encoding_subseed2ascii [] = {'A','C','T','G','N','-'};

    static const LETTER Encoding_subseed2ncbi [] = { 0, 1, 3, 2, 4, 5 };
    static const LETTER Encoding_ncbi2subseed [] = { 0, 1, 3, 2, 4, 5 };
    static const LETTER Encoding_ncbiAmb2subseed [] = { 5, 0, 1, 4, 3, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4 };

    /** We define the result object. */
    const LETTER* result = 0;

         if (from==SUBSEED 			&& to==ASCII)    			{ result = Encoding_subseed2ascii;  }
    else if (from==SUBSEED 			&& to==NCBI)     			{ result = Encoding_subseed2ncbi;   }
    else if (from==ASCII   			&& to==SUBSEED)  			{ result = Encoding_ascii2subseed;  }
    else if (from==ASCII   			&& to==NCBI)     			{ result = Encoding_ascii2ncbi;     }
    else if (from==NCBI    			&& to==SUBSEED)  			{ result = Encoding_ncbi2subseed;   }
    else if (from==NCBI_DNA_NO_AMB 	&& to==SUBSEED)  			{ result = Encoding_ncbi2subseed;   }
    else if (from==NCBI_DNA_WITH_AMB&& to==SUBSEED)  			{ result = Encoding_ncbiAmb2subseed;}
    else                                    					{ result = 0;                       }

    return result;
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

