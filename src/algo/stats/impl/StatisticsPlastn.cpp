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

#include <algo/stats/impl/StatisticsPlastn.hpp>

#include <misc/api/macros.hpp>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace misc;
using namespace statistics;
using namespace database;
using namespace algo::core;

/********************************************************************************/
namespace statistics { namespace impl {
/********************************************************************************/

///////////////////////////////////////////////////////////////////////////
// openGap  extendGap   lambda     K       H       alpha   beta  theta
///////////////////////////////////////////////////////////////////////////

/** Karlin-Altschul parameter values for substitution scores 1 and -5. */
static GlobalParameters::Info blastn_values_1_5[] = {
    { 0, 0, 1.39, 0.747, 1.38, 1.00,  0, 100 },
    { 3, 3, 1.39, 0.747, 1.38, 1.00,  0, 100 }
};

/** Karlin-Altschul parameter values for substitution scores 1 and -4. */
static GlobalParameters::Info blastn_values_1_4[] = {
    { 0, 0, 1.383, 0.738, 1.36, 1.02,  0, 100 },
    { 1, 2,  1.36,  0.67,  1.2,  1.1,  0,  98 },
    { 0, 2,  1.26,  0.43, 0.90,  1.4, -1,  91 },
    { 2, 1,  1.35,  0.61,  1.1,  1.2, -1,  98 },
    { 1, 1,  1.22,  0.35, 0.72,  1.7, -3,  88 }
};

/** Karlin-Altschul parameter values for substitution scores 2 and -7.
 * These parameters can only be applied to even scores. Any odd score must be
 * rounded down to the nearest even number before calculating the e-value.
 */
static GlobalParameters::Info blastn_values_2_7[] = {
    { 0, 0,  0.69, 0.73, 1.34, 0.515,  0, 100 },
    { 2, 4,  0.68, 0.67,  1.2,  0.55,  0,  99 },
    { 0, 4,  0.63, 0.43, 0.90,   0.7, -1,  91 },
    { 4, 2, 0.675, 0.62,  1.1,   0.6, -1,  98 },
    { 2, 2,  0.61, 0.35, 0.72,   1.7, -3,  88 }
};

/** Karlin-Altschul parameter values for substitution scores 1 and -3. */
static GlobalParameters::Info blastn_values_1_3[] = {
    { 0, 0, 1.374, 0.711, 1.31, 1.05,  0, 100 },
    { 2, 2,  1.37,  0.70,  1.2,  1.1,  0,  99 },
    { 1, 2,  1.35,  0.64,  1.1,  1.2, -1,  98 },
    { 0, 2,  1.25,  0.42, 0.83,  1.5, -2,  91 },
    { 2, 1,  1.34,  0.60,  1.1,  1.2, -1,  97 },
    { 1, 1,  1.21,  0.34, 0.71,  1.7, -2,  88 }
};

/** Karlin-Altschul parameter values for substitution scores 2 and -5.
 * These parameters can only be applied to even scores. Any odd score must be
 * rounded down to the nearest even number before calculating the e-value.
 */
static GlobalParameters::Info blastn_values_2_5[] = {
    { 0, 0, 0.675, 0.65,  1.1,  0.6, -1, 99 },
    { 2, 4,  0.67, 0.59,  1.1,  0.6, -1, 98 },
    { 0, 4,  0.62, 0.39, 0.78,  0.8, -2, 91 },
    { 4, 2,  0.67, 0.61,  1.0, 0.65, -2, 98 },
    { 2, 2,  0.56, 0.32, 0.59, 0.95, -4, 82 }
};

/** Karlin-Altschul parameter values for substitution scores 1 and -2. */
static GlobalParameters::Info blastn_values_1_2[] = {
    { 0, 0, 1.28, 0.46, 0.85, 1.5, -2, 96 },
    { 2, 2, 1.33, 0.62,  1.1, 1.2,  0, 99 },
    { 1, 2, 1.30, 0.52, 0.93, 1.4, -2, 97 },
    { 0, 2, 1.19, 0.34, 0.66, 1.8, -3, 89 },
    { 3, 1, 1.32, 0.57,  1.0, 1.3, -1, 99 },
    { 2, 1, 1.29, 0.49, 0.92, 1.4, -1, 96 },
    { 1, 1, 1.14, 0.26, 0.52, 2.2, -5, 85 }
};

/** Karlin-Altschul parameter values for substitution scores 2 and -3.
 * These parameters can only be applied to even scores. Any odd score must be
 * rounded down to the nearest even number before calculating the e-value.
 */
static GlobalParameters::Info blastn_values_2_3[] = {
    { 0, 0,  0.55, 0.21, 0.46,  1.2, -5, 87 },
    { 4, 4,  0.63, 0.42, 0.84, 0.75, -2, 99 },
    { 2, 4, 0.615, 0.37, 0.72, 0.85, -3, 97 },
    { 0, 4,  0.55, 0.21, 0.46,  1.2, -5, 87 },
    { 3, 3, 0.615, 0.37, 0.68,  0.9, -3, 97 },
    { 6, 2,  0.63, 0.42, 0.84, 0.75, -2, 99 },
    { 5, 2, 0.625, 0.41, 0.78,  0.8, -2, 99 },
    { 4, 2,  0.61, 0.35, 0.68,  0.9, -3, 96 },
    { 2, 2, 0.515, 0.14, 0.33, 1.55, -9, 81 }
};

/** Karlin-Altschul parameter values for substitution scores 3 and -4. */
static GlobalParameters::Info blastn_values_3_4[] = {
    { 6, 3, 0.389, 0.25, 0.56, 0.7, -5, 95},
    { 5, 3, 0.375, 0.21, 0.47, 0.8, -6, 92},
    { 4, 3, 0.351, 0.14, 0.35, 1.0, -9, 86},
    { 6, 2, 0.362, 0.16, 0.45, 0.8, -4, 88},
    { 5, 2, 0.330, 0.092, 0.28, 1.2, -13, 81},
    { 4, 2, 0.281, 0.046, 0.16, 1.8, -23, 69}
};

/** Karlin-Altschul parameter values for substitution scores 4 and -5. */
static GlobalParameters::Info blastn_values_4_5[] = {
    { 0, 0, 0.22, 0.061, 0.22, 1.0, -15, 74 },
    { 6, 5, 0.28,  0.21, 0.47, 0.6 , -7, 93 },
    { 5, 5, 0.27,  0.17, 0.39, 0.7,  -9, 90 },
    { 4, 5, 0.25,  0.10, 0.31, 0.8, -10, 83 },
    { 3, 5, 0.23, 0.065, 0.25, 0.9, -11, 76 }
};

/** Karlin-Altschul parameter values for substitution scores 1 and -1. */
static GlobalParameters::Info blastn_values_1_1[] = {
    { 3,  2, 1.09,  0.31, 0.55, 2.0,  -2, 99 },
    { 2,  2, 1.07,  0.27, 0.49, 2.2,  -3, 97 },
    { 1,  2, 1.02,  0.21, 0.36, 2.8,  -6, 92 },
    { 0,  2, 0.80, 0.064, 0.17, 4.8, -16, 72 },
    { 4,  1, 1.08,  0.28, 0.54, 2.0,  -2, 98 },
    { 3,  1, 1.06,  0.25, 0.46, 2.3,  -4, 96 },
    { 2,  1, 0.99,  0.17, 0.30, 3.3, -10, 90 }
};

/** Karlin-Altschul parameter values for substitution scores 3 and -2. */
static GlobalParameters::Info blastn_values_3_2[] = {
    {  5,  5, 0.208, 0.030, 0.072, 2.9, -47, 77}
};

/** Karlin-Altschul parameter values for substitution scores 5 and -4. */
static GlobalParameters::Info blastn_values_5_4[] = {
    { 10, 6, 0.163, 0.068, 0.16, 1.0, -19, 85 },
    {  8, 6, 0.146, 0.039, 0.11, 1.3, -29, 76 }
};

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void GlobalParametersPlastn::build (void)
{
    bool found = false;

    /** We don't accept default open/extend gap costs. */
    if (_parameters->openGapCost == 0  &&  _parameters->extendGapCost == 0)  {  throw MSG_STATS_MSG2;  }

    int openGap   = _parameters->openGapCost;
    int extendGap = _parameters->extendGapCost;

    DEBUG (("GlobalParametersPlastn::build:  reward=%d  penalty=%d  openCost=%d  extendCost=%d  xdrop=%d  finalXdrop=%d\n",
        _parameters->reward,      _parameters->penalty,
        _parameters->openGapCost, _parameters->extendGapCost,
        _parameters->XdroppofGap, _parameters->finalXdroppofGap
    ));

    /** We define a little macro helper for defining params according to [reward,penalty]. */
#define DEFPARAMS(r,p,table,opengapMax,extendgapMax)  \
    if (_parameters->reward==r  && _parameters->penalty==p)   \
    { \
        if (openGap == 0 && extendGap == 0)  \
        { \
            openGap   = table[0].openGap;   \
            extendGap = table[0].extendGap; \
        } \
        found = lookup (this, table, ARRAYSIZE(table), openGap, extendGap); \
    }

    DEFPARAMS (1,-5, blastn_values_1_5,  3,  3);
    DEFPARAMS (1,-4, blastn_values_1_4,  2,  2);
    DEFPARAMS (2,-7, blastn_values_2_7,  4,  4);
    DEFPARAMS (1,-3, blastn_values_1_3,  2,  2);
    DEFPARAMS (2,-5, blastn_values_2_5,  4,  4);
    DEFPARAMS (1,-2, blastn_values_1_2,  2,  2);
    DEFPARAMS (2,-3, blastn_values_2_3,  6,  4);
    DEFPARAMS (3,-4, blastn_values_3_4,  6,  3);
    DEFPARAMS (1,-1, blastn_values_1_1,  4,  2);
    DEFPARAMS (3,-2, blastn_values_3_2,  5,  5);
    DEFPARAMS (4,-5, blastn_values_4_5, 12,  8);
    DEFPARAMS (5,-4, blastn_values_5_4, 25, 10);

    _parameters->XdroppofGap      = (int)(((_parameters->XdroppofGap)      * 0.693174)/lambda);
    _parameters->finalXdroppofGap = MAX  ((int)(((_parameters->finalXdroppofGap) * 0.693174)/lambda), _parameters->XdroppofGap);

    DEBUG (("GlobalParametersPlastn::build:  reward=%d  penalty=%d  openCost=%d  extendCost=%d  lambda=%f  xdrop=%d  finalXdrop=%d\n",
        _parameters->reward, _parameters->penalty,
        _parameters->openGapCost, _parameters->extendGapCost,
        lambda,
        _parameters->XdroppofGap, _parameters->finalXdroppofGap
    ));
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
