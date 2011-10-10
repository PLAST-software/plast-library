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

#include "Statistics.hpp"

#include <math.h>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace statistics;
using namespace database;
using namespace algo;

/********************************************************************************/
namespace statistics {
/********************************************************************************/

#define INT2_MAX    32768

/** Number of statistical parameters in each row of the precomputed tables. */
#define BLAST_NUM_STAT_VALUES 8

/** Holds values (gap-opening, extension, etc.) for a matrix. */
typedef double array_of_8[BLAST_NUM_STAT_VALUES];

#define BLOSUM62_VALUES_MAX 12 /**< Number of different combinations supported for BLOSUM62. */

static array_of_8 blosum62_values [BLOSUM62_VALUES_MAX] = {
    {(double) INT2_MAX, (double) INT2_MAX, (double) INT2_MAX, 0.3176, 0.134, 0.4012, 0.7916, -3.2},
    {11, 2, (double) INT2_MAX, 0.297, 0.082, 0.27, 1.1, -10},
    {10, 2, (double) INT2_MAX, 0.291, 0.075, 0.23, 1.3, -15},
    {9, 2, (double) INT2_MAX, 0.279, 0.058, 0.19, 1.5, -19},
    {8, 2, (double) INT2_MAX, 0.264, 0.045, 0.15, 1.8, -26},
    {7, 2, (double) INT2_MAX, 0.239, 0.027, 0.10, 2.5, -46},
    {6, 2, (double) INT2_MAX, 0.201, 0.012, 0.061, 3.3, -58},
    {13, 1, (double) INT2_MAX, 0.292, 0.071, 0.23, 1.2, -11},
    {12, 1, (double) INT2_MAX, 0.283, 0.059, 0.19, 1.5, -19},
    {11, 1, (double) INT2_MAX, 0.267, 0.041, 0.14, 1.9, -30},
    {10, 1, (double) INT2_MAX, 0.243, 0.024, 0.10, 2.5, -44},
    {9, 1, (double) INT2_MAX, 0.206, 0.010, 0.052, 4.0, -87},
}; /**< Supported values (gap-existence, extension, etc.) for BLOSUM62. */

#define BLOSUM50_VALUES_MAX 16 /**< Number of different combinations supported for BLOSUM50. */

static array_of_8 blosum50_values[BLOSUM50_VALUES_MAX] = {
    {(double) INT2_MAX, (double) INT2_MAX, (double) INT2_MAX, 0.2318, 0.112, 0.3362, 0.6895, -4.0},
    {13, 3, (double) INT2_MAX, 0.212, 0.063, 0.19, 1.1, -16},
    {12, 3, (double) INT2_MAX, 0.206, 0.055, 0.17, 1.2, -18},
    {11, 3, (double) INT2_MAX, 0.197, 0.042, 0.14, 1.4, -25},
    {10, 3, (double) INT2_MAX, 0.186, 0.031, 0.11, 1.7, -34},
    {9, 3, (double) INT2_MAX, 0.172, 0.022, 0.082, 2.1, -48},
    {16, 2, (double) INT2_MAX, 0.215, 0.066, 0.20, 1.05, -15},
    {15, 2, (double) INT2_MAX, 0.210, 0.058, 0.17, 1.2, -20},
    {14, 2, (double) INT2_MAX, 0.202, 0.045, 0.14, 1.4, -27},
    {13, 2, (double) INT2_MAX, 0.193, 0.035, 0.12, 1.6, -32},
    {12, 2, (double) INT2_MAX, 0.181, 0.025, 0.095, 1.9, -41},
    {19, 1, (double) INT2_MAX, 0.212, 0.057, 0.18, 1.2, -21},
    {18, 1, (double) INT2_MAX, 0.207, 0.050, 0.15, 1.4, -28},
    {17, 1, (double) INT2_MAX, 0.198, 0.037, 0.12, 1.6, -33},
    {16, 1, (double) INT2_MAX, 0.186, 0.025, 0.10, 1.9, -42},
    {15, 1, (double) INT2_MAX, 0.171, 0.015, 0.063, 2.7, -76},
};  /**< Supported values (gap-existence, extension, etc.) for BLOSUM50. */

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
GlobalParameters::GlobalParameters (algo::IParameters* parameters)
    : _parameters (parameters)
{
    build ();

    DEBUG (("GlobalParameters::GlobalParameters::  K=%f logK=%f alpha=%f lambda=%f beta=%f  evalue=%f\n",
        K, logK, alpha, lambda, beta, evalue
    ));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void GlobalParameters::setParameters (algo::IParameters* parameters)
{
    if (_parameters != 0)  { _parameters->forget(); }
    _parameters = parameters;
    if (_parameters != 0)  { _parameters->use();    }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void GlobalParameters::build (void)
{
    switch (_parameters->matrixKind)
    {
        case ENUM_BLOSUM62:
        {
            if (_parameters->openGapCost   == 0)   {  _parameters->openGapCost   = 11;  }
            if (_parameters->extendGapCost == 0)   {  _parameters->extendGapCost = 1;   }

            for (size_t i=0; i<BLOSUM62_VALUES_MAX; i++)
            {
                /** Shortcut. */
                array_of_8&  current = blosum62_values[i];

                if (current[0]==_parameters->openGapCost  &&  current[1]==_parameters->extendGapCost)
                {
                    lambda  = current[3];
                    K       = current[4];
                    H       = current[5];
                    alpha   = current[6];
                    beta    = current[7];
                    evalue  = _parameters->evalue;
                    logK    = log(K);
                }
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

            for (size_t i=0; i<BLOSUM50_VALUES_MAX; i++)
            {
                /** Shortcut. */
                array_of_8&  current = blosum50_values[i];

                if (current[0]==_parameters->openGapCost  &&  current[1]==_parameters->extendGapCost)
                {
                    lambda  = current[3];
                    K       = current[4];
                    H       = current[5];
                    alpha   = current[6];
                    beta    = current[7];
                    evalue  = _parameters->evalue;
                    logK    = log(K);
                }
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
QueryInformation::QueryInformation (
    IGlobalParameters*  globalParams,
    IParameters*        parameters,
    ISequenceDatabase*  queryDb,
    u_int64_t           subjectDbSize,
    u_int32_t           subjectNbSequences
)
    : _globalParams(0),
      _parameters (parameters),
      _queryDb (0),
      _subjectDbSize(subjectDbSize),
      _subjectNbSequences(subjectNbSequences)
{
    /** We remind the global parameters. */
    setGlobalParams (globalParams);

    /** We remind the query database. */
    setQueryDb (queryDb);

    /** We compute the parameters. */
    build ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
QueryInformation::~QueryInformation ()
{
    setGlobalParams (0);
    setQueryDb (0);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void QueryInformation::build (void)
{
    ISequence   sequence;
    int         cutoffs             = 0;
    int         length_adjustment   = 0;
    long long   eff_searchsp        = 0;
    double      evalue              = _globalParams->evalue;

    /** Shortcut. */
    size_t nbSeq = _queryDb->getSequencesNumber();

    /** We resize the vector that will hold statistical data for each sequence. */
    _seqInfo.resize (nbSeq);

    DEBUG (("QueryInformation::build:  nbSeq=%ld  _subjectDbSize=%lld  _subjectNbSequences=%ld \n",
        nbSeq, _subjectDbSize, _subjectNbSequences
    ));

    DEBUG (("QueryInformation::build:  K=%f logK=%f alpha=%f lambda=%f beta=%f evalue=%f\n",
        _globalParams->K, _globalParams->logK, _globalParams->alpha,
        _globalParams->lambda, _globalParams->beta, _globalParams->evalue
    ));

    /** We loop over each sequence. */
    for (size_t i=0; i<nbSeq; i++)
    {
        bool found = _queryDb->getSequenceByIndex (i, sequence);

        if (!found)  { continue; }

        /** Shortcut. */
        size_t seqLength = sequence.data.letters.size;

        /** We compute length adjustment for the current sequence. */
        computeLengthAdjustment (
            _globalParams->K,
            _globalParams->logK,
            _globalParams->alpha / _globalParams->lambda,
            _globalParams->beta,
            seqLength,
            _subjectDbSize,
            _subjectNbSequences,
            length_adjustment
        );

        /** We compute the search space size. */
        eff_searchsp = (long long)
            (seqLength - length_adjustment) *
            (_subjectDbSize - length_adjustment * _subjectNbSequences);

        /** We compute the cutoffs. */
        cutoffs = 0;
        evalue  = _globalParams->evalue;
        computeCutoffs (cutoffs, evalue, eff_searchsp, 0);

        /** We fill the current sequence info. */
        IQueryInformation::SequenceInfo& info = _seqInfo[i];

        info.sequence_length = seqLength;
        info.length_adjust   = length_adjustment;
        info.cut_offs        = cutoffs;
        info.eff_searchsp    = eff_searchsp;

        /** We update the minimum score. */
        if (cutoffs < _parameters->smallGapThreshold)     {  _parameters->smallGapThreshold = cutoffs;  }
    }

    /** We may have to rescale minimum score. */
    if (_parameters->matrixKind == ENUM_BLOSUM62)
    {
        if  (_parameters->smallGapThreshold < 40)   {  _parameters->smallGapThreshold = 40;  }
    }
    else if (_parameters->matrixKind == ENUM_BLOSUM50)
    {
        if  (_parameters->smallGapThreshold < 46)   {  _parameters->smallGapThreshold = 46;  }
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
bool QueryInformation::computeLengthAdjustment (
    double K,
    double logK,
    double alpha_d_lambda,
    double beta,
    int query_length,
    long long db_length,
    int db_num_seqs,
    int& length_adjustment
)
{
    int i;                     /* iteration index */
    const int kMaxIterations = 20;     /* maximum allowed iterations */
    double m = (double) query_length;
    double n = (double) db_length;
    double N = (double) db_num_seqs;
    double ell;                 /* A float value of the length adjustment */
    double ss;                  /* effective size of the search space */
    double ell_min = 0, ell_max;   /* At each iteration i,
     * ell_min <= ell <= ell_max. */
    bool converged    = false;       /* True if the iteration converged */
    double ell_next = 0;        /* Value the variable ell takes at iteration
     * i + 1 */
    /* Choose ell_max to be the largest nonnegative value that satisfies
     *
     *    K * (m - ell) * (n - N * ell) > MAX(m,n)
     *
     * Use quadratic formula: 2 c /( - b + sqrt( b*b - 4 * a * c )) */
    /* scope of a, mb, and c, the coefficients in the quadratic formula
     * (the variable mb is -b) */
    //printf("length_adjustment %d\n",*length_adjustment);
    double a  = N;
    double mb = m * N + n;
    double c  = n * m - n/K;

    if(c < 0) {
        length_adjustment = 0;
        return 1;
    } else {
        ell_max = 2 * c / (mb + sqrt(mb * mb - 4 * a * c));
    }

    /* end scope of a, mb and c */

    for(i = 1; i <= kMaxIterations; i++) {      /* for all iteration indices */
        double ell_bar;         /* proposed next value of ell */
        ell      = ell_next;
        ss       = (m - ell) * (n - N * ell);
        ell_bar  = alpha_d_lambda * (logK + log(ss)) + beta;
        if(ell_bar >= ell) { /* ell is no bigger than the true fixed point */
            ell_min = ell;
            if(ell_bar - ell_min <= 1.0) {
                converged = true;
                break;
            }
            if(ell_min == ell_max) { /* There are no more points to check */
                break;
            }
        } else { /* else ell is greater than the true fixed point */
            ell_max = ell;
        }
        if(ell_min <= ell_bar && ell_bar <= ell_max) {
            /* ell_bar is in range. Accept it */
            ell_next = ell_bar;
        } else { /* else ell_bar is not in range. Reject it */
            ell_next = (i == 1) ? ell_max : (ell_min + ell_max) / 2;
        }
    } /* end for all iteration indices */
    if(converged==true) { /* the iteration converged */
        /* If ell_fixed is the (unknown) true fixed point, then we
         * wish to set (*length_adjustment) to floor(ell_fixed).  We
         * assume that floor(ell_min) = floor(ell_fixed) */
        length_adjustment = (int) ell_min;
        /* But verify that ceil(ell_min) != floor(ell_fixed) */
        ell = ceil(ell_min);
        if( ell <= ell_max ) {
            ss = (m - ell) * (n - N * ell);
            if(alpha_d_lambda * (logK + log(ss)) + beta >= ell) {
                /* ceil(ell_min) == floor(ell_fixed) */
                length_adjustment = (int) ell;
            }
        }
    } else { /* else the iteration did not converge. */
        /* Use the best value seen so far */
        length_adjustment = (int) ell_min;
    }

//    printf ("BLAST_ComputeLengthAdjustment:  length_adjustment=%ld \n",
//            length_adjustment
//        );

    return converged;
}

/*********************************************************************
** METHOD  :
** PURPOSE : Calculate the cutoff score, S, and the highest expected score.
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS : ref NCBI BlastCutoffs
*********************************************************************/
int QueryInformation::computeCutoffs (
    int&        S,           /* cutoff score */
    double&     E,           /* expected no. of HSPs scoring at or above S */
    long long   searchsp,    /* size of search space. */
    double      gap_decay_rate
)
{
    int      es;
    double   esave;
    int      s_changed = 0;

    if (_globalParams->lambda == -1. || _globalParams->K == -1. || _globalParams->H == -1.)
        return 1;

    /*
   Calculate a cutoff score, S, from the Expected
   (or desired) number of reported HSPs, E.
     */
    es = 1;
    esave = E;

    if (E > 0.)
    {
        es = (int) (ceil( log((double)(_globalParams->K * searchsp / E)) / _globalParams->lambda ));
    }

    /*
   Pick the larger cutoff score between the user's choice
   and that calculated from the value of E.
     */
    if (es > S) {
        s_changed = 1;
        S = es;
    }

    /*
      Re-calculate E from the cutoff score, if E going in was too high
     */
    if (esave <= 0. || !s_changed)
    {
        E = searchsp * exp((double)(-_globalParams->lambda * S) + _globalParams->logK);
    }

    DEBUG (("QueryInformation::computeCutoffs:  S=%d  E=%f  es=%d  searchsp=%lld  s_changed=%d  esave=%f\n", S, E, es, searchsp, s_changed, esave));

    return 0;
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
