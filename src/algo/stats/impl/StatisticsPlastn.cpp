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

#include <math.h>
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
    { 1, 1,  1.22,  0.35, 0.72,  1.7, -3,  88 },
    { 5, 2,  1.383, 0.738, 1.362, 0, 0, 0 }   // ADDED
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
    { 1, 1,  1.21,  0.34, 0.71,  1.7, -2,  88 },
    { 5, 2,  1.374, 0.710, 1.307, 0, 0, 0}  // ADDED

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
    { 1, 1, 1.14, 0.26, 0.52, 2.2, -5, 85 },
    { 5, 2, 1.332, 0.620, 1.124, 0, 0, 0}  // ADDED

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
    { 3, 5, 0.23,  0.065, 0.25, 0.9, -11, 76 },
    {12, 8, 0.301, 0.306, 0.753, 0,   0,  0  }  // ADDED
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
    if (!found) {  throw GlobalParametersFailure (MSG_STATS_MSG2); }

    double K1 = 0;
    double lambda1 = 1;
    double H1 = 0;
    double logK1 = 0;
    Blast_ScoreFreq sfp;

    init_sfp(&sfp);
    karlinBlkUngappedCalc(K1,lambda1,H1,logK1,&sfp);

/*    _parameters->ungapScoreThreshold = (int)bitsToRawValue((double)_parameters->ungapScoreThreshold);
    _parameters->XdroppofUnGap    = (int)(((_parameters->XdroppofUnGap)    * M_LN2)/lambda);
    _parameters->XdroppofGap      = (int)(((_parameters->XdroppofGap)      * M_LN2)/lambda);
    _parameters->finalXdroppofGap = MAX  ((int)(((_parameters->finalXdroppofGap) * M_LN2)/lambda), _parameters->XdroppofGap);*/

    _parameters->ungapScoreThreshold = (((_parameters->ungapScoreThreshold * M_LN2)+ logK1) / lambda1);

    _parameters->XdroppofUnGap    = (int)(((_parameters->XdroppofUnGap)    * M_LN2)/lambda1);
    _parameters->XdroppofGap      = (int)(((_parameters->XdroppofGap)      * M_LN2)/lambda1);
    _parameters->finalXdroppofGap = MAX  ((int)(((_parameters->finalXdroppofGap) * M_LN2)/lambda1), _parameters->XdroppofGap);

    DEBUG (("GlobalParametersPlastn::build:  reward=%d  penalty=%d  openCost=%d  extendCost=%d  lambda=%f  xdrop=%d  finalXdrop=%d\n",
        _parameters->reward, _parameters->penalty,
        _parameters->openGapCost, _parameters->extendGapCost,
        lambda,
        _parameters->XdroppofGap, _parameters->finalXdroppofGap
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
void GlobalParametersPlastn::init_sfp(GlobalParametersPlastn::Blast_ScoreFreq* sfp)
{
    sfp->score_min = sfp->obs_min = _parameters->penalty;
    sfp->score_max = sfp->obs_max = _parameters->reward;
    for (int ni=0;ni<BLAST_MAX_RANGE_REWARD_PENALTY;ni++)
    {
    	sfp->sprob0[ni] = 0;
    }
    sfp->sprob = sfp->sprob0;
    sfp->sprob -= sfp->score_min;        /* center around 0 */
    sfp->score_avg = sfp->score_min*0.75+sfp->score_max*0.25;

    //calculate the average score, in the identity matrix, we have 75% of penalty and 25% of reward
    // because of the diagonal, so we calculate the score_avg like this
    sfp->sprob[sfp->score_min] = 0.75;
    sfp->sprob[sfp->score_max] = 0.25;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
int16_t GlobalParametersPlastn::karlinBlkUngappedCalc(double &K, double &lambda, double &H, double &logK, Blast_ScoreFreq* sfp)
{


	if (sfp == NULL)
		return 1;

	/* Calculate the parameter Lambda */

	lambda = karlinLambdaNR(sfp, BLAST_KARLIN_LAMBDA0_DEFAULT);
	if (lambda < 0.)
		goto ErrExit;


	/* Calculate H */
	H = karlinLtoH(sfp, lambda);
	if (H < 0.)
		goto ErrExit;


	/* Calculate K and log(K) */

	K = karlinLHtoK(sfp, lambda, H);
	if (K < 0.)
		goto ErrExit;
	logK = log(K);

	/* Normal return */
	return 0;

ErrExit:
	lambda = H = K = -1.;
	logK = HUGE_VAL;
	return 1;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
double GlobalParametersPlastn::karlinLambdaNR(Blast_ScoreFreq* sfp, double initialLambdaGuess)
{
	int32_t  low;        /* Lowest score (must be negative)  */
	int32_t  high;       /* Highest score (must be positive) */
	int32_t  itn;
	int32_t  i, d;
	double*  sprob;
	double   returnValue;

	low = sfp->obs_min;
	high = sfp->obs_max;
	if (sfp->score_avg >= 0.) {   /* Expected score must be negative */
		return -1.0;
	}
	//   if (BlastScoreChk(low, high) != 0) return -1.;

	sprob = sfp->sprob;
	/* Find greatest common divisor of all scores */
	for (i = 1, d = -low; i <= high-low && d > 1; ++i) {
		if (sprob[i+low] != 0.0) {
			d = greatCommonDivisor(d, i);
		}
	}
	returnValue =
			nlmKarlinLambdaNR( sprob, d, low, high,
					initialLambdaGuess,
					BLAST_KARLIN_LAMBDA_ACCURACY_DEFAULT,
					20, 20 + BLAST_KARLIN_LAMBDA_ITER_DEFAULT, &itn );


	return returnValue;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
double GlobalParametersPlastn::karlinLtoH(Blast_ScoreFreq* sfp, double lambda)
{
	int32_t  score;
	double   H, etonlam, sum, scale;

	double *probs = sfp->sprob;
	int32_t low   = sfp->obs_min,  high  = sfp->obs_max;

	if (lambda < 0.) {
		return -1.;
	}
	//if (BlastScoreChk(low, high) != 0) return -1.;

	etonlam = exp( - lambda );
	sum = low * probs[low];
	for( score = low + 1; score <= high; score++ ) {
		sum = score * probs[score] + etonlam * sum;
	}

	scale = pow( etonlam, high );
	if( scale > 0.0 ) {
		H = lambda * sum/scale;
	} else { /* Underflow of exp( -lambda * high ) */
		H = lambda * exp( lambda * high + log(sum) );
	}
	return H;
}


/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
double GlobalParametersPlastn::karlinLHtoK(Blast_ScoreFreq* sfp, double lambda, double H)
{
	/*The next array stores the probabilities of getting each possible
      score in an alignment of fixed length; the array is shifted
      during part of the computation, so that
      entry 0 is for score 0.  */
//	double         alignmentScoreProbabilities[1000];
	double         *alignmentScoreProbabilities=NULL;
	int32_t            low;    /* Lowest score (must be negative) */
	int32_t            high;   /* Highest score (must be positive) */
	int32_t            range;  /* range of scores, computed as high - low*/
	double          K;      /* local copy of K  to return*/
	int             i;   /*loop index*/
	int             iterCounter; /*counter on iterations*/
	int32_t            divisor; /*candidate divisor of all scores with
                               non-zero probabilities*/
	/*highest and lowest possible alignment scores for current length*/
	int32_t            lowAlignmentScore, highAlignmentScore;
	int32_t            first, last; /*loop indices for dynamic program*/
	register double innerSum;
	double          oldsum, oldsum2;  /* values of innerSum on previous
                                         iterations*/
	double          outerSum;        /* holds sum over j of (innerSum
                                        for iteration j/j)*/

	double          score_avg; /*average score*/
	/*first term to use in the closed form for the case where
      high == 1 or low == -1, but not both*/
	double          firstTermClosedForm;  /*usually store H/lambda*/
	int             iterlimit; /*upper limit on iterations*/
	double          sumlimit; /*lower limit on contributions
                                to sum over scores*/

	/*array of score probabilities reindexed so that low is at index 0*/
	double         *probArrayStartLow;

	/*pointers used in dynamic program*/
	double         *ptrP, *ptr1, *ptr2, *ptr1e;
	double          expMinusLambda; /*e^^(-Lambda) */

	if (lambda <= 0. || H <= 0.) {
		/* Theory dictates that H and lambda must be positive, so
		 * return -1 to indicate an error */
		return -1.;
	}

	/*Karlin-Altschul theory works only if the expected score
      is negative*/
	if (sfp->score_avg >= 0.0) {
		return -1.;
	}

	low   = sfp->obs_min;
	high  = sfp->obs_max;
	range = high - low;

	probArrayStartLow = &sfp->sprob[low];
	/* Look for the greatest common divisor ("delta" in Appendix of PNAS 87 of
       Karlin&Altschul (1990) */
	for (i = 1, divisor = -low; i <= range && divisor > 1; ++i) {
		if (probArrayStartLow[i] != 0.0)
			divisor = greatCommonDivisor(divisor, i);
	}

	high   /= divisor;
	low    /= divisor;
	lambda *= divisor;

	range = high - low;

	firstTermClosedForm = H/lambda;
	expMinusLambda      = exp((double) -lambda);

	if (low == -1 && high == 1) {
		K = (sfp->sprob[low*divisor] - sfp->sprob[high*divisor]) *
				(sfp->sprob[low*divisor] - sfp->sprob[high*divisor]) / sfp->sprob[low*divisor];
		return(K);
	}

	if (low == -1 || high == 1) {
		if (high != 1) {
			score_avg = sfp->score_avg / divisor;
			firstTermClosedForm
			= (score_avg * score_avg) / firstTermClosedForm;
		}
		return firstTermClosedForm * (1.0 - expMinusLambda);
	}

	sumlimit  = BLAST_KARLIN_K_SUMLIMIT_DEFAULT;
	iterlimit = BLAST_KARLIN_K_ITER_MAX;

	alignmentScoreProbabilities =
			(double *)calloc((iterlimit*range + 1), sizeof(*alignmentScoreProbabilities));
	if (alignmentScoreProbabilities == NULL)
		return -1.;

	outerSum = 0.;
	lowAlignmentScore = highAlignmentScore = 0;
	alignmentScoreProbabilities[0] = innerSum = oldsum = oldsum2 = 1.;

	for (iterCounter = 0;
			((iterCounter < iterlimit) && (innerSum > sumlimit));
			outerSum += innerSum /= ++iterCounter) {
		first = last = range;
		lowAlignmentScore  += low;
		highAlignmentScore += high;
		/*dynamic program to compute P(i,j)*/
				for (ptrP = alignmentScoreProbabilities +
						(highAlignmentScore-lowAlignmentScore);
						ptrP >= alignmentScoreProbabilities;
						*ptrP-- =innerSum) {
					ptr1  = ptrP - first;
					ptr1e = ptrP - last;
					ptr2  = probArrayStartLow + first;
					for (innerSum = 0.; ptr1 >= ptr1e; ) {
						innerSum += *ptr1  *  *ptr2;
						ptr1--;
						ptr2++;
					}
					if (first)
						--first;
					if (ptrP - alignmentScoreProbabilities <= range)
						--last;
				}
				/* Horner's rule */
				innerSum = *++ptrP;
				for( i = lowAlignmentScore + 1; i < 0; i++ ) {
					innerSum = *++ptrP + innerSum * expMinusLambda;
				}
				innerSum *= expMinusLambda;

				for (; i <= highAlignmentScore; ++i)
					innerSum += *++ptrP;
				oldsum2 = oldsum;
				oldsum  = innerSum;
	}

	K = -exp((double)-2.0*outerSum) /
			(firstTermClosedForm*expm1(-(double)lambda));

	if (alignmentScoreProbabilities != NULL)
		free(alignmentScoreProbabilities);

	return K;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
int32_t GlobalParametersPlastn::greatCommonDivisor(int32_t a, int32_t b)
{
	int32_t   c;

	b = ABS(b);
	if (b > a)
		c=a, a=b, b=c;

	while (b != 0) {
		c = a%b;
		a = b;
		b = c;
	}
	return a;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
double GlobalParametersPlastn::expm1(double	x)
{
  double	absx = ABS(x);

  if (absx > .33)
    return exp(x) - 1.;

  if (absx < 1.e-16)
    return x;

  return x * (1. + x *
             (1./2. + x *
             (1./6. + x *
             (1./24. + x *
             (1./120. + x *
             (1./720. + x *
             (1./5040. + x *
             (1./40320. + x *
             (1./362880. + x *
             (1./3628800. + x *
             (1./39916800. + x *
             (1./479001600. +
              x/6227020800.))))))))))));
}


/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
double GlobalParametersPlastn::nlmKarlinLambdaNR(double* probs, int32_t d, int32_t low, int32_t high,
		double lambda0, double tolx, int32_t itmax, int32_t maxNewton, int32_t * itn )
{
	int32_t k;
	double x0, x, a = 0, b = 1;
	double f = 4;  /* Larger than any possible value of the poly in [0,1] */
	int32_t isNewton = 0; /* we haven't yet taken a Newton step. */

	if (d<=0)
		throw GlobalParametersFailure (MSG_STATS_MSG2);

	x0 = exp( -lambda0 );
	x = ( 0 < x0 && x0 < 1 ) ? x0 : .5;
	for( k = 0; k < itmax; k++ ) { /* all iteration indices k */

		int32_t i;
		double g, fold = f;
		int32_t wasNewton = isNewton; /* If true, then the previous step was a */
		/* Newton step */
		isNewton  = 0;            /* Assume that this step is not */

		/* Horner's rule for evaluating a polynomial and its derivative */
		g = 0;
		f = probs[low];
		for( i = low + d; i < 0; i += d ) {
			g = x * g + f;
			f = f * x + probs[i];
		}
		g = x * g + f;
		f = f * x + probs[0] - 1;
		for( i = d; i <= high; i += d ) {
			g = x * g + f;
			f = f * x + probs[i];
		}
		/* End Horner's rule */

		if( f > 0 ) {
			a = x; /* move the left endpoint */
		} else if( f < 0 ) {
			b = x; /* move the right endpoint */
		} else { /* f == 0 */
			break; /* x is an exact solution */
		}
		if( b - a < 2 * a * ( 1 - b ) * tolx ) {
			/* The midpoint of the interval converged */
			x = (a + b) / 2; break;
		}

		if( k >= maxNewton ||
				/* If convergence of Newton's method appears to be failing; or */
				( wasNewton && fabs( f ) > .9 * fabs(fold) ) ||
				/* if the previous iteration was a Newton step but didn't decrease
				 * f sufficiently; or */
				g >= 0
				/* if a Newton step will move us away from the desired solution */
		) { /* then */
			/* bisect */
			x = (a + b)/2;
		} else {
			/* try a Newton step */
			double p = - f/g;
			double y = x + p;
			if( y <= a || y >= b ) { /* The proposed iterate is not in (a,b) */
				x = (a + b)/2;
			} else { /* The proposed iterate is in (a,b). Accept it. */
				isNewton = 1;
				x = y;
				if( fabs( p ) < tolx * x * (1-x) ) break; /* Converged */
			} /* else the proposed iterate is in (a,b) */
		} /* else try a Newton step. */
	} /* end for all iteration indices k */
	*itn = k;
	return -log(x)/d;
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
