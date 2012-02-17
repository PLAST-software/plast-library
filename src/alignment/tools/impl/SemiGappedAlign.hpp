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

/** \file SemiGapAlign.hpp
 *  \brief Implementation providing means for splitting a gap alignment into ungap alignments.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _SEMI_GAP_ALIGN_HPP_
#define _SEMI_GAP_ALIGN_HPP_

/********************************************************************************/

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
class SemiGapAlign : public dp::SmartPointer
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
        char* A,
        char* B,
        int M,
        int N,
        int* a_offset,
        int* b_offset,
        int reverse_sequence
    );

private:

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
