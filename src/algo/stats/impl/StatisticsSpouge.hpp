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

/** \file StatisticsSpouge.hpp
 *  \brief Implementation of statistics for protein based on Spouge statistics management
 *  \date 03/10/2014
 *  \author sbrillet
 */

#ifndef STATISTICS_SPOUGE_HPP_
#define STATISTICS_SPOUGE_HPP_

/********************************************************************************/

#include <os/api/IThread.hpp>

#include <database/api/ISequenceDatabase.hpp>

#include <algo/core/api/IAlgoParameters.hpp>

#include <algo/stats/api/IStatistics.hpp>

#include <vector>
#include <map>
#include <stdio.h>

/********************************************************************************/
namespace statistics  {
/** \brief Implementation of statistics concepts */
namespace impl {
/********************************************************************************/

/********************************************************************************/

/** \brief Implementation of IGlobalParametersSpouge interface
 *
 *  This class uses a IParameters instance (coming from command line options for instance)
 *  for customizing the generation of the statistical parameters comes form spouge statistics methods
 */
class GlobalParametersSpouge : public AbstractGlobalParameters
{
public:

    /** Constructor.
     *  \param[in] parameters : parameters used for configuring the global statistics.
     *  \param[in] subjectDbLength : Subject database length
     */
	GlobalParametersSpouge (algo::core::IParameters* parameters, size_t subjectDbLength)
        : AbstractGlobalParameters (parameters,subjectDbLength)  {  build(); }

    /** Structure holding statistical information. */
    struct Info
    {
        double openGap;
        double extendGap;
        double lambda;
        double K;
        double H;
        double alpha;
        double beta;
        double C;
        double alpha_v;
        double sigma;
    };

protected:
    bool lookup (AbstractGlobalParameters* globalParams, void* table, size_t size, int openGap, int extendGap);

    /** \copydoc IGlobalParameters::scoreToEvalue */
    double scoreToEvalue(double effSearchSp, double score, size_t qryLength, size_t sbjLength);

    /** \copydoc IGlobalParameters::evalueToCutoff */
    bool evalueToCutoff(int&cutoff, double effSearchSp, double evalue, size_t qryLength, size_t sbjLength);

    /** \copydoc IGlobalParameters::useCutoff */
    bool useCutoff() {return false;};

    /** Computes statistics. */
    void build (void);
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* STATISTICS_SPOUGE_HPP_ */
