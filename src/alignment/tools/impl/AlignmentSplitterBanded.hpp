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

/** \file AlignmentSplitter.hpp
 *  \brief Implementation providing means for splitting a gap alignment into ungap alignments.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ALIGNMENT_SPLITTER_BANDED_HPP_
#define _ALIGNMENT_SPLITTER_BANDED_HPP_

/********************************************************************************/

#include <alignment/tools/impl/AlignmentSplitter.hpp>

/********************************************************************************/
namespace alignment {
namespace tools     {
namespace impl      {
/********************************************************************************/

/** \brief Implementation of IAlignmentResult for splitting a gap alignment into ungap alignments.
 *
 * This implementation uses dynamic programming for providing the service.
 */
class AlignmentSplitterBanded : public AlignmentSplitter
{
public:

    /** Constructor.
     * \param[in] scoreMatrix   : score matrix to be used
     * \param[in] openGapCost   : cost for opening a gap
     * \param[in] extendGapCost : cost for extending a gap
     */
    AlignmentSplitterBanded (algo::core::IScoreMatrix* scoreMatrix, int openGapCost, int extendGapCost);

    /** Destructor. */
    virtual ~AlignmentSplitterBanded ();

    /** \copydoc IAlignmentSplitter::splitAlign(Alignment&,int*) */
    size_t splitAlign (
        const database::LETTER* subjectSeq,
        const database::LETTER* querySeq,
        const misc::Range32&    sbjRange,
        const misc::Range32&    qryRange,
        SplitOutput& output
    );

private:

    int16_t* _vector_H;
    int16_t* _vector_E;
    int16_t* _vector_F;

    bool allocVectors (u_int64_t newSize);

    void dumpMatrix (int qryLen, int subLen, int delta);
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _ALIGNMENT_SPLITTER_BANDED_HPP_ */
