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

#include <misc/api/types.hpp>
#include <misc/api/macros.hpp>
#include <misc/api/PlastStrings.hpp>

#include <os/impl/DefaultOsFactory.hpp>

#include <algo/stats/impl/Statistics.hpp>
#include <algo/stats/impl/StatisticsSpouge.hpp>

#include <math.h>

#include <stdio.h>
#define DEBUG(a)  //printf a
#define VERBOSE(a)  //printf a

using namespace std;
using namespace misc;
using namespace statistics;
using namespace database;
using namespace algo::core;

/********************************************************************************/
namespace statistics { namespace impl {
/********************************************************************************/

#define VMAX    (double)32768

/** Supported values (gap-existence, extension, etc.) for BLOSUM62. */
static GlobalParametersSpouge::Info blosum62_values [] =
{ // openGap  extendGap   lambda     K       H       alpha   beta    C   alpha_v  sigma
	{ VMAX,    VMAX,      0.3176,  0.134,  0.4012,  0.7916,   -3.2,  0.623757,    4.964660,    4.964660},
	{   11,       2,      0.2970,  0.082,  0.2700,  1.1000,  -10,    0.641766,   12.673800,   12.757600},
	{   10,       2,      0.2910,  0.075,  0.2300,  1.3000,  -15,    0.649362,   16.474000,   16.602600},
	{    9,       2,      0.2790,  0.058,  0.1900,  1.5000,  -19,    0.659245,   22.751900,   22.950000},
	{    8,       2,      0.2640,  0.045,  0.1500,  1.8000,  -26,    0.672692,   35.483800,   35.821300},
	{    7,       2,      0.2390,  0.027,  0.1000,  2.5000,  -46,    0.702056,   61.238300,   61.886000},
	{    6,       2,      0.2010,  0.012,  0.0610,  3.3000,  -58,    0.740802,  140.417000,  141.882000},
	{   13,       1,      0.2920,  0.071,  0.2300,  1.2000,  -11,    0.647715,   19.506300,   19.893100},
	{   12,       1,      0.2830,  0.059,  0.1900,  1.5000,  -19,    0.656391,   27.856200,   28.469900},
	{   11,       1,      0.2670,  0.041,  0.1400,  1.9000,  -30,    0.669720,   42.602800,   43.636200},
	{   10,       1,      0.2430,  0.024,  0.1000,  2.5000,  -44,    0.693267,   83.178700,   85.065600},
	{    9,       1,      0.2060,  0.010,  0.0520,  4.0000,  -87,    0.731887,  210.333000,  214.842000}
};

/** Supported values (gap-existence, extension, etc.) for BLOSUM50. */
static GlobalParametersSpouge::Info  blosum50_values[] =
{ // openGap  extendGap   lambda     K       H       alpha   beta    C   alpha_v  sigma
	{ VMAX,    VMAX,      0.2318,  0.112,  0.3362,  0.6895,   -4.0, 0.609639,    5.388310,    5.388310},
	{   13,       3,      0.2120,  0.063,  0.1900,  1.1000,  -16,   0.639287,   18.113800,   18.202800},
	{   12,       3,      0.2060,  0.055,  0.1700,  1.2000,  -18,   0.644715,   22.654600,   22.777700},
	{   11,       3,      0.1970,  0.042,  0.1400,  1.4000,  -25,   0.656327,   29.861100,   30.045700},
	{   10,       3,      0.1860,  0.031,  0.1100,  1.7000,  -34,   0.671150,   42.393800,   42.674000},
	{    9,       3,      0.1720,  0.022,  0.0820,  2.1000,  -48,   0.694326,   66.069600,   66.516400},
	{   16,       2,      0.2150,  0.066,  0.2000,  1.0500,  -15,   0.633899,   17.951800,   18.092100},
	{   15,       2,      0.2100,  0.058,  0.1700,  1.2000,  -20,   0.641985,   21.940100,   22.141800},
	{   14,       2,      0.2020,  0.045,  0.1400,  1.4000,  -27,   0.650682,   28.681200,   28.961900},
	{   13,       2,      0.1930,  0.035,  0.1200,  1.6000,  -32,   0.660984,   42.059500,   42.471600},
	{   12,       2,      0.1810,  0.025,  0.0950,  1.9000,  -41,   0.678090,   63.747600,   64.397300},
	{   19,       1,      0.2120,  0.057,  0.1800,  1.2000,  -21,   0.635714,   26.311200,   26.923300},
	{   18,       1,      0.2070,  0.050,  0.1500,  1.4000,  -28,   0.643523,   34.903700,   35.734800},
	{   17,       1,      0.1980,  0.037,  0.1200,  1.6000,  -33,   0.654504,   48.895800,   50.148600},
	{   16,       1,      0.1860,  0.025,  0.1000,  1.9000,  -42,   0.667750,   76.469100,   78.443000},
	{   15,       1,      0.1710,  0.015,  0.0630,  2.7000,  -76,   0.694575,  140.053000,  144.160000}
};

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool GlobalParametersSpouge::lookup (AbstractGlobalParameters* globalParams, void* table, size_t size, int openGap, int extendGap)
{
    bool found = false;

    for (size_t i=0; !found &&  i<size; i++)
    {
        /** Shortcuts. */
        Info&  current = ((Info*)table)[i];

        IParameters* params = _parameters;

        if (current.openGap==openGap  &&  current.extendGap==extendGap)
        {
            globalParams->lambda  = current.lambda;
            globalParams->K       = current.K;
            globalParams->H       = current.H;
            globalParams->alpha   = current.alpha;
            globalParams->beta    = current.beta;
            globalParams->evalue  = params->evalue;
            globalParams->logK    = log(globalParams->K);
            globalParams->C       = current.C;
            globalParams->Alpha_v = current.alpha_v;
            globalParams->Sigma   = current.sigma;

            globalParams->a_un     = ((Info*)table)[0].alpha;
            globalParams->Alpha_un = ((Info*)table)[0].alpha_v;

            globalParams->G  = current.openGap + current.extendGap;

            globalParams->b        = 2.0 * globalParams->G * (globalParams->a_un     - globalParams->alpha);
            globalParams->Beta     = 2.0 * globalParams->G * (globalParams->Alpha_un - globalParams->Alpha_v);
            globalParams->Tau      = 2.0 * globalParams->G * (globalParams->Alpha_un - globalParams->Sigma);

            found = true;
        }
    }

    if (!found) {  throw GlobalParametersFailure (MSG_STATS_MSG2); }

    return found;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void GlobalParametersSpouge::build (void)
{
    bool found = false;

    switch (_parameters->matrixKind)
    {
        case ENUM_BLOSUM62:
        {
            if (_parameters->openGapCost   == 0)   {  _parameters->openGapCost   = 11;  }
            if (_parameters->extendGapCost == 0)   {  _parameters->extendGapCost = 1;   }

            if (_parameters->algoKind == ENUM_TPLASTX)
            {
                K      = 0.133956;
                alpha  = 0.7916;
                beta   = -3.2;
                lambda = 0.317606;
                evalue  = _parameters->evalue;
                logK    = log(K);
            }
            else
            {
                found = lookup (this, blosum62_values, ARRAYSIZE(blosum62_values), _parameters->openGapCost, _parameters->extendGapCost);
            }

            if (_parameters->smallGapThreshold   == 0)    { _parameters->smallGapThreshold   = 54; }
            if (_parameters->ungapScoreThreshold == 0)    { _parameters->ungapScoreThreshold = 38; }

            if (_parameters->XdroppofGap == 0)      { _parameters->XdroppofGap = 15;  }
            _parameters->XdroppofGap  = (int)(((_parameters->XdroppofGap) * 0.693174)/lambda);

            if (_parameters->finalXdroppofGap == 0) { _parameters->finalXdroppofGap = 25; }
            _parameters->finalXdroppofGap = (int)(((_parameters->finalXdroppofGap) * 0.693174)/lambda);

            break;
        }

        case ENUM_BLOSUM50:
        {
            if (_parameters->openGapCost   == 0)   {  _parameters->openGapCost   = 13;  }
            if (_parameters->extendGapCost == 0)   {  _parameters->extendGapCost = 2;   }

            if (_parameters->algoKind == ENUM_TPLASTX)
            {
                K       = 0.112;
                alpha   = 0.6895;
                beta    = -4;
                lambda  = 0.317606;
                evalue  = _parameters->evalue;
                logK    = log(K);
            }
            else
            {
                found = lookup (this, blosum50_values, ARRAYSIZE(blosum50_values), _parameters->openGapCost, _parameters->extendGapCost);
            }

            if (_parameters->smallGapThreshold   == 0)    { _parameters->smallGapThreshold   = 60; }
            if (_parameters->ungapScoreThreshold == 0)    { _parameters->ungapScoreThreshold = 44; }

            if (_parameters->XdroppofGap == 0)      { _parameters->XdroppofGap = 15;  }
            _parameters->XdroppofGap  = (int)(((_parameters->XdroppofGap) * 0.693174)/lambda);

            if (_parameters->finalXdroppofGap == 0) { _parameters->finalXdroppofGap = 25; }
            _parameters->finalXdroppofGap = (int)(((_parameters->finalXdroppofGap) * 0.693174)/lambda);

            break;
        }

        default:
        	throw MSG_STATS_MSG1;
            break;
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
double GlobalParametersSpouge::scoreToEvalue(double effSearchSp, double score, size_t qryLength, size_t sbjLength)
{
    // the score and lambda may have been rescaled.  We will compute the scaling factor
    // and use it to scale a, alpha, and Sigma similarly.
    double scale_factor = 1.0;

    // the pair-wise e-value must be scaled back to db-wise e-value
    double db_scale_factor = (db_length) ?  (double)db_length/(double)sbjLength : 1.0;

    double lambda_    = lambda;                    // kbp->Lambda;
    double k_         = K;                         // kbp->K;
    double ai_hat_    = alpha * scale_factor;      // gbp->a     * scale_factor;
    double bi_hat_    = b;                      // gbp->b;
    double alphai_hat_= Alpha_v * scale_factor ; //gbp->Alpha * scale_factor;
    double betai_hat_ = Beta;
    double sigma_hat_ = Sigma * scale_factor;      //gbp->Sigma * scale_factor;
    double tau_hat_   = Tau;


    /* here we consider symmetric matrix only */
    double aj_hat_    = ai_hat_;
    double bj_hat_    = bi_hat_;
    double alphaj_hat_= alphai_hat_;
    double betaj_hat_ = betai_hat_;

    /* this is 1/sqrt(2.0*PI) */
    static double const_val = 0.39894228040143267793994605993438;

    double m_li_y, vi_y, sqrt_vi_y, m_F, P_m_F;
    double n_lj_y, vj_y, sqrt_vj_y, n_F, P_n_F;
    double c_y, p1, p2, area;

    m_li_y = qryLength - (ai_hat_*score + bi_hat_);
    vi_y = MAX(2.0*alphai_hat_/lambda_, alphai_hat_*score+betai_hat_);
    sqrt_vi_y = sqrt(vi_y);
    m_F = m_li_y/sqrt_vi_y;
    P_m_F = 0.5 + 0.5 * erf(m_F);
    p1 = m_li_y * P_m_F + sqrt_vi_y * const_val * exp(-0.5*m_F*m_F);

    n_lj_y = sbjLength - (aj_hat_*score + bj_hat_);
    vj_y = MAX(2.0*alphaj_hat_/lambda_, alphaj_hat_*score+betaj_hat_);
    sqrt_vj_y = sqrt(vj_y);
    n_F = n_lj_y/sqrt_vj_y;
    P_n_F = 0.5 + 0.5 * erf(n_F);
    p2 = n_lj_y * P_n_F + sqrt_vj_y * const_val * exp(-0.5*n_F*n_F);

    c_y = MAX(2.0*sigma_hat_/lambda_, sigma_hat_*score+tau_hat_);
    area = p1 * p2 + c_y * P_m_F * P_n_F;

    double res = area * k_ * exp(-lambda_ * score) * db_scale_factor;
    return res;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool GlobalParametersSpouge::evalueToCutoff(int&cutoff, double effSearchSp, double evalue, size_t qryLength, size_t sbjLength)
{
    int a, b, c;
    double e;
    double db_scale_factor = 1.0;

    b = MAX((int)(log(db_scale_factor/evalue) / lambda), 2);

    e = scoreToEvalue(effSearchSp, b, qryLength, sbjLength);

    if (e > evalue) {
        while (e> evalue) {
            a = b;
            b *= 2;
            e = scoreToEvalue(effSearchSp, b, qryLength, sbjLength);
        }
    } else {
        a = 0;
    }
    while(b-a > 1) {
        c = (a+b)/2;
        e = scoreToEvalue(effSearchSp, c, qryLength, sbjLength);
        if (e> evalue) {
            a = c;
        } else {
            b = c;
        }
    }
    cutoff = a;
	return true;
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
