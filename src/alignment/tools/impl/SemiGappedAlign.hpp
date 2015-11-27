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

/** \file SemiGapAlign.hpp
 *  \brief Implementation providing means for splitting a gap alignment into ungap alignments.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _SEMI_GAP_ALIGN_HPP_
#define _SEMI_GAP_ALIGN_HPP_

/********************************************************************************/

#include <alignment/tools/api/ISemiGappedAlign.hpp>
#include <alignment/tools/api/IAlignmentSplitter.hpp>
#include <algo/core/api/IScoreMatrix.hpp>

/********************************************************************************/
namespace alignment {
namespace tools     {
namespace impl      {
/********************************************************************************/

/** \brief To be completed
 *
 *
 */
class SemiGapAlign : public ISemiGapAlign
{
public:

    /** Constructor. */
    SemiGapAlign (
        algo::core::IScoreMatrix* scoreMatrix,
        int openGapCost,
        int extendGapCost,
        int Xdropoff
    );

    /** Destructor. */
    virtual ~SemiGapAlign ();

    /** */
    int compute (
        const char* A,
        const char* B,
        u_int32_t M,
        u_int32_t N,
        u_int32_t* a_offset,
        u_int32_t* b_offset,
        bool reverse_sequence
    );

protected:

    algo::core::IScoreMatrix* _scoreMatrix;
    void setScoreMatrix (algo::core::IScoreMatrix* scoreMatrix)  { SP_SETATTR(scoreMatrix); }

    int _openGapCost;
    int _extendGapCost;
    int _openExtendGapCost;
    int _Xdropoff;
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _SEMI_GAP_ALIGN_HPP_ */
