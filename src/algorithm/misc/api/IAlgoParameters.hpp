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

#include "IAlgoTypes.hpp"
#include "SmartPointer.hpp"

#include <string>
#include <vector>

#include <stdio.h>

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/** */
typedef std::pair<u_int64_t,u_int64_t>  Range;

/** */
class IParameters : public dp::SmartPointer
{
public:

    AlgoKind_e                algoKind;

    SeedModelKind_e           seedModelKind;
    size_t                    seedSpan;
    std::vector<std::string>  subseedStrings;

    ScoreMatrixKind_e   matrixKind;

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
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _IALGO_PARAMETERS_HPP_ */
