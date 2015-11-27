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

/** \file StatisticsPlastn.hpp
 *  \brief Implementation of statistics management
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef STATISTICS_PLASTN_HPP_
#define STATISTICS_PLASTN_HPP_

/********************************************************************************/

#include <algo/stats/impl/Statistics.hpp>

/********************************************************************************/
namespace statistics  {
/** \brief Implementation of statistics concepts */
namespace impl {
/********************************************************************************/

/** \brief Implementation of IGlobalParameters interface
 *
 *  This class uses a IParameters instance (coming from command line options for instance)
 *  for customizing the generation of the statistical parameters
 *  (openGapCost, extendGapCost, etc...).
 */
class GlobalParametersPlastn : public AbstractGlobalParameters
{
public:

    /** Constructor.
     *  \param[in] parameters : parameters used for configuring the global statistics.
     *  \param[in] subjectDbLength : Subject database length
     */
    GlobalParametersPlastn (algo::core::IParameters* parameters, size_t subjectDbLength)
        : AbstractGlobalParameters (parameters, subjectDbLength)  { build (); }

private:
	#define BLAST_KARLIN_LAMBDA_ACCURACY_DEFAULT    (1.e-5) /**< LAMBDA_ACCURACY_DEFAULT == accuracy to which Lambda should be calc'd */
	#define BLAST_KARLIN_LAMBDA_ITER_DEFAULT        17 /**< LAMBDA_ITER_DEFAULT == no. of iterations in LambdaBis = ln(accuracy)/ln(2)*/
	#define BLAST_KARLIN_LAMBDA0_DEFAULT    		0.5 /**< Initial guess for the value of Lambda in BlastKarlinLambdaNR */
	#define BLAST_KARLIN_K_SUMLIMIT_DEFAULT 		0.0001 /**< K_SUMLIMIT_DEFAULT == sumlimit used in BlastKarlinLHtoK() */
	#define BLAST_KARLIN_K_ITER_MAX 				100 /**< upper limit on iterations for BlastKarlinLHtoK */
	#define BLAST_MAX_RANGE_REWARD_PENALTY			15

    /** Holds score frequencies used in calculation
    of Karlin-Altschul parameters for an ungapped search.
    */
    typedef struct Blast_ScoreFreq {
        int32_t      score_min; /**< lowest allowed scores */
        int32_t      score_max; /**< highest allowed scores */
        int32_t      obs_min;   /**< lowest observed (actual) scores */
        int32_t      obs_max;   /**< highest observed (actual) scores */
        double       score_avg; /**< average score, must be negative for local alignment. */
        double       sprob0[BLAST_MAX_RANGE_REWARD_PENALTY];     /**< arrays for frequency of given score, shifted down by score_min. */
        double*       sprob;     /**< arrays for frequency of given score, shifted down by score_min. */
    } Blast_ScoreFreq;

    /** Initialize the ScoreFreq structure */
    void init_sfp(GlobalParametersPlastn::Blast_ScoreFreq* sfp);

    /** Blast calculation of K,H,logK,lambda for the ungap threshold and Xdrop threshold */
    int16_t karlinBlkUngappedCalc(double &K, double &lambda, double &H, double &logK, Blast_ScoreFreq* sfp);

    /** Calculate H, the relative entropy of the p's and q's */
    double karlinLtoH(Blast_ScoreFreq* sfp, double lambda);

    /** Calculate K */
    double karlinLHtoK(Blast_ScoreFreq* sfp, double lambda, double H);

    /** Blast calculation LambdaNr */
    double karlinLambdaNR(Blast_ScoreFreq* sfp, double initialLambdaGuess);

    /** Blast great common divisor */
    int32_t greatCommonDivisor(int32_t a, int32_t b);

    /** Exponentional with base e
     *  @param x input operand
     *  @return exp(x) - 1
     */
   double expm1(double	x);

    /** Find positive solution to sum_{i=low}^{high} exp(i lambda) * probs[i] = 1. */
    double nlmKarlinLambdaNR(double* probs, int32_t d, int32_t low, int32_t high,
    		double lambda0, double tolx, int32_t itmax, int32_t maxNewton, int32_t * itn );

    /** Computes statistics. */
    void build (void);
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* STATISTICS_PLASTN_HPP_ */
