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


#ifndef _IALGO_PARAMETERS_HPP_
#define _IALGO_PARAMETERS_HPP_

/********************************************************************************/

#include "SmartPointer.hpp"
#include "types.hpp"

#include <string>
#include <vector>

#include <stdio.h>

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/** */
typedef std::pair<u_int64_t,u_int64_t>  Range;

/** Define several piece of information used for parameterizing the algorithm.
 */
class IParameters : public dp::SmartPointer
{
public:

    types::AlgoKind_e         algoKind;

    types::SeedModelKind_e    seedModelKind;
    size_t                    seedSpan;
    std::vector<std::string>  subseedStrings;

    types::ScoreMatrixKind_e  matrixKind;

    std::string   subjectUri;
    Range         subjectRange;

    std::string   queryUri;
    Range         queryRange;

    bool    filterQuery;

    int     ungapNeighbourLength;
    int     ungapScoreThreshold;

    int     smallGapBandLength;
    int     smallGapBandWidth;
    int     smallGapThreshold;
    int     openGapCost;
    int     extendGapCost;

    float   evalue;

    int XdroppofGap;
    int finalXdroppofGap;

    std::string outputfile;

    std::vector<types::ReadingFrame_e> strands;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _IALGO_PARAMETERS_HPP_ */
