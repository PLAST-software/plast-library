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

#ifndef _ALIGNMENT_SPLITTER_HPP_
#define _ALIGNMENT_SPLITTER_HPP_

/********************************************************************************/

#include <algo/align/api/IAlignmentSplitter.hpp>

/********************************************************************************/
namespace algo   {
namespace align {
/** \brief Implementation of concepts about alignments with gaps. */
namespace impl   {
/********************************************************************************/

/** \brief Implementation of IAlignmentResult for splitting a gap alignment into ungap alignments.
 *
 * This implementation uses dynamic programming for providing the service.
 */
class AlignmentSplitter : public IAlignmentSplitter
{
public:

    /** Constructor.
     * \param[in] scoreMatrix   : score matrix to be used
     * \param[in] openGapCost   : cost for opening a gap
     * \param[in] extendGapCost : cost for extending a gap
     */
    AlignmentSplitter (algo::core::IScoreMatrix* scoreMatrix, int openGapCost, int extendGapCost);

    /** Destructor. */
    virtual ~AlignmentSplitter ();

    /** \copydoc IAlignmentSplitter::splitAlign */
    size_t splitAlign (
        const database::LETTER* subjectSeq,
        const database::LETTER* querySeq,
        u_int32_t subjectStartInSeq,
        u_int32_t subjectEndInSeq,
        u_int32_t queryStartInSeq,
        u_int32_t queryEndInSeq,
        int* splittab,
        size_t& identity,
        size_t& nbGap,
        size_t& nbMis,
        size_t& alignSize,
        database::LETTER* subjectAlign = 0,
        database::LETTER* queryAlign   = 0
    );

    /** \copydoc IAlignmentSplitter::splitAlign(Alignment&,int*) */
    size_t splitAlign (Alignment& align,  int* splittab);

    /** \copydoc IAlignmentSplitter::computeInfo */
    void computeInfo (Alignment& align);

private:

    algo::core::IScoreMatrix* _scoreMatrix;
    void setScoreMatrix (algo::core::IScoreMatrix* scoreMatrix)  { SP_SETATTR (scoreMatrix); }

    int _openGapCost;
    int _extendGapCost;

    short** _matrix_H;
    short** _matrix_E;
    short** _matrix_F;

    int _DefaultAlignSize;
    int _MaxAlignSize;

    short** newMatrix  (int nrows, int ncols);
    void    freeMatrix (short*** mat);
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _ALIGNMENT_SPLITTER_HPP_ */
