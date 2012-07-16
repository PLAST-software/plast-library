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
     */
    GlobalParametersPlastn (algo::core::IParameters* parameters)
        : AbstractGlobalParameters (parameters)  { build (); }

private:

    /** Computes statistics. */
    void build (void);
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* STATISTICS_PLASTN_HPP_ */
