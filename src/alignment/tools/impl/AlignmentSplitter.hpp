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

/** \file AlignmentSplitter.hpp
 *  \brief Implementation providing means for splitting a gap alignment into ungap alignments.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ALIGNMENT_SPLITTER_HPP_
#define _ALIGNMENT_SPLITTER_HPP_

/********************************************************************************/

#include <alignment/tools/api/IAlignmentSplitter.hpp>

/********************************************************************************/
namespace alignment {
namespace tools     {
namespace impl      {
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

    /** \copydoc IAlignmentSplitter::splitAlign(Alignment&,int*) */
    size_t splitAlign (
        const database::LETTER* subjectSeq,
        const database::LETTER* querySeq,
        const misc::Range32&    sbjRange,
        const misc::Range32&    qryRange,
        SplitOutput& output
    );

    /** \copydoc IAlignmentSplitter::splitAlign(Alignment&,int*) */
    size_t splitAlign (core::Alignment& align,  SplitOutput& output);

protected:

    algo::core::IScoreMatrix* _scoreMatrix;
    void setScoreMatrix (algo::core::IScoreMatrix* scoreMatrix)  { SP_SETATTR (scoreMatrix); }

    int _openGapCost;
    int _extendGapCost;

    int _band;

    int16_t** _matrix_H;
    int16_t** _matrix_E;
    int16_t** _matrix_F;

    u_int64_t _DefaultAlignSize;
    u_int64_t _MaxAlignSize;

    int16_t** newMatrix  (int nrows, int ncols);
    void      freeMatrix (int16_t*** mat);

    /** */
    virtual void dump (
		const SplitOutput& output,
	    const misc::Range32&    qryRange,
	    const misc::Range32&    sbjRange,
	    char* qryLocal,
	    char* subLocal
	);

    /** */
    virtual void dumpMatrix (int qryLen, int subLen, int delta);
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _ALIGNMENT_SPLITTER_HPP_ */
