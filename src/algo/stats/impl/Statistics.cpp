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
static GlobalParameters::Info blosum62_values [] =
{ // openGap  extendGap   lambda     K       H       alpha   beta
    { VMAX,    VMAX,      0.3176,  0.134,  0.4012,  0.7916,  -3.2    },
    {   11,       2,      0.2970,  0.082,  0.2700,  1.1000,  -10     },
    {   10,       2,      0.2910,  0.075,  0.2300,  1.3000,  -15     },
    {    9,       2,      0.2790,  0.058,  0.1900,  1.5000,  -19     },
    {    8,       2,      0.2640,  0.045,  0.1500,  1.8000,  -26     },
    {    7,       2,      0.2390,  0.027,  0.1000,  2.5000,  -46     },
    {    6,       2,      0.2010,  0.012,  0.0610,  3.3000,  -58     },
    {   13,       1,      0.2920,  0.071,  0.2300,  1.2000,  -11     },
    {   12,       1,      0.2830,  0.059,  0.1900,  1.5000,  -19     },
    {   11,       1,      0.2670,  0.041,  0.1400,  1.9000,  -30     },
    {   10,       1,      0.2430,  0.024,  0.1000,  2.5000,  -44     },
    {    9,       1,      0.2060,  0.010,  0.0520,  4.0000,  -87     }
};

/** Supported values (gap-existence, extension, etc.) for BLOSUM50. */
static GlobalParameters::Info  blosum50_values[] =
{ // openGap  extendGap   lambda     K       H       alpha   beta
    { VMAX,    VMAX,      0.2318,  0.112,  0.3362,  0.6895,  -4.0    },
    {   13,       3,      0.212,   0.063,  0.1900,  1.1000,  -16     },
    {   12,       3,      0.206,   0.055,  0.1700,  1.2000,  -18     },
    {   11,       3,      0.197,   0.042,  0.1400,  1.4000,  -25     },
    {   10,       3,      0.186,   0.031,  0.1100,  1.7000,  -34     },
    {    9,       3,      0.172,   0.022,  0.0820,  2.1000,  -48     },
    {   16,       2,      0.215,   0.066,  0.2000,  1.0500,  -15     },
    {   15,       2,      0.210,   0.058,  0.1700,  1.2000,  -20     },
    {   14,       2,      0.202,   0.045,  0.1400,  1.4000,  -27     },
    {   13,       2,      0.193,   0.035,  0.1200,  1.6000,  -32     },
    {   12,       2,      0.181,   0.025,  0.0950,  1.9000,  -41     },
    {   19,       1,      0.212,   0.057,  0.1800,  1.2000,  -21     },
    {   18,       1,      0.207,   0.050,  0.1500,  1.4000,  -28     },
    {   17,       1,      0.198,   0.037,  0.1200,  1.6000,  -33     },
    {   16,       1,      0.186,   0.025,  0.1000,  1.9000,  -42     },
    {   15,       1,      0.171,   0.015,  0.0630,  2.7000,  -76     }
};

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool AbstractGlobalParameters::lookup (AbstractGlobalParameters* globalParams, void* table, size_t size, int openGap, int extendGap)
{
    bool found = false;

    for (size_t i=0; !found &&  i<size; i++)
    {
        /** Shortcuts. */
        Info&  current = ((Info*)table)[i];

        IParameters* params = globalParams->_parameters;

        if (current.openGap==openGap  &&  current.extendGap==extendGap)
        {
            globalParams->lambda  = current.lambda;
            globalParams->K       = current.K;
            globalParams->H       = current.H;
            globalParams->alpha   = current.alpha;
            globalParams->beta    = current.beta;
            globalParams->evalue  = params->evalue;
            globalParams->logK    = log(globalParams->K);

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
double AbstractGlobalParameters::scoreToEvalue(double effSearchSp, double score, size_t qryLength, size_t sbjLength)
{
	return effSearchSp * exp((-lambda * (double) score) + logK);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool AbstractGlobalParameters::evalueToCutoff(int&cutoff, double effSearchSp, double evalue, size_t qryLength, size_t sbjLength)
{
    int      es;
    double   esave;
    int      s_changed = 0;

    if (lambda == -1. || K == -1. || H == -1.)
        return 1;

    /*
   Calculate a cutoff score, S, from the Expected
   (or desired) number of reported HSPs, E.
     */
    es = 1;
    esave = evalue;

    if (evalue > 0.)
    {
        es = (int) (ceil( log((double)(K * effSearchSp / evalue)) / lambda ));
    }

    /*
   Pick the larger cutoff score between the user's choice
   and that calculated from the value of E.
     */
    if (es > cutoff) {
        s_changed = 1;
        cutoff = es;
    }

    /*
      Re-calculate E from the cutoff score, if E going in was too high
     */
    if (esave <= 0. || !s_changed)
    {
    	evalue = effSearchSp * exp((double)(-lambda * cutoff) + logK);
    }

    VERBOSE (("QueryInformation::evalueToCutoff:  S=%d  E=%f  es=%d  searchsp=%lld  s_changed=%d  esave=%f\n", cutoff, evalue, es, effSearchSp, s_changed, esave));

    return 0;
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
                lookup (this, blosum62_values, ARRAYSIZE(blosum62_values), _parameters->openGapCost, _parameters->extendGapCost);
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
                lookup (this, blosum50_values, ARRAYSIZE(blosum50_values), _parameters->openGapCost, _parameters->extendGapCost);
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
      _subjectNbSequences(subjectNbSequences),
      _isBuilt (false), _synchro(0)
{
    /** We remind the global parameters. */
    setGlobalParams (globalParams);

    /** We remind the query database. */
    setQueryDb (queryDb);

    /** We create a synchronizer for the building process. */
    _synchro = os::impl::DefaultFactory::singleton().getThreadFactory().newSynchronizer();

    /** We may have to build the information if not already done. */
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

    if (_synchro)  { delete _synchro; }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IQueryInformation::SequenceInfo& QueryInformation::getSeqInfo (const database::ISequence& seq)
{
    map <ISequenceDatabase*, Container>::iterator lookup = _seqInfoMap.find (seq.database);
    if (lookup != _seqInfoMap.end())
    {
    	return (lookup->second) [seq.index];
    }
    else
    {
		return _seqInfoMap [seq.database] [seq.index];
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
void QueryInformation::build (void)
{
    /** We need to protect this method since it can be accessed on the same instance by
     *  different threads. */
    os::LocalSynchronizer synch (_synchro);

    /** We just check that we have not already built the information. */
    if (_isBuilt == true)  { return; }

    int         cutoffs             = 0;
    int         length_adjustment   = 0;
    long long   eff_searchsp        = 0;
    double      evalue              = _globalParams->evalue;

    DEBUG (("QueryInformation::build:  K=%f logK=%f alpha=%f lambda=%f beta=%f evalue=%f\n",
        _globalParams->K, _globalParams->logK, _globalParams->alpha,
        _globalParams->lambda, _globalParams->beta, _globalParams->evalue
    ));

    DEBUG (("QueryInformation::build:  nbSeq=%ld\n", _queryDb->getSequencesNumber()));

    /** We need to iterate each sequence of the database. */
    ISequenceIterator* itSeq = _queryDb->createSequenceIterator();
    LOCAL (itSeq);

    size_t i=0;
    for (itSeq->first(); ! itSeq->isDone(); itSeq->next(), i++)
    {
        const ISequence* seq = itSeq->currentItem();

        /** Shortcut. */
        size_t seqLength = seq->data.letters.size;

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
        //_globalParams->evalueToCutoff(cutoffs, eff_searchsp, evalue, seqLength, _subjectDbSize);

        /** We fill the current sequence info. */
        IQueryInformation::SequenceInfo info;

        info.sequence_length = seqLength;
        info.length_adjust   = length_adjustment;
        info.cut_offs        = cutoffs;
        info.eff_searchsp    = eff_searchsp;

        /** We add it to the container.  Note that we use 'seq->database' (instead of '_queryDb' directly)
         *  because '_queryDb' may be a database composed of several databases, and the 'database' attribute
         *  may reference a child database belonging to '_queryDb'. */
        _seqInfoMap[seq->database].push_back (info);

        VERBOSE (("[%d]  searchsp=%ld  seqLength=%ld  lenAdjust=%ld  => cuttof=%d  evalue=%f  scoremin=%d\n",
            i, eff_searchsp, info.sequence_length, length_adjustment, cutoffs, evalue, _parameters->smallGapThreshold
        ));

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

    /** We memorize (for next call) that we have computed the needed information. */
    _isBuilt = true;
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

    VERBOSE (("QueryInformation::computeCutoffs:  S=%d  E=%f  es=%d  searchsp=%lld  s_changed=%d  esave=%f\n", S, E, es, searchsp, s_changed, esave));

    return 0;
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
