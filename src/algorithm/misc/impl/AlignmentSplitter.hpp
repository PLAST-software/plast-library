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

#ifndef _ALIGNMENT_SPLITTER_HPP_
#define _ALIGNMENT_SPLITTER_HPP_

/********************************************************************************/

#include "IAlignmentSplitter.hpp"

/********************************************************************************/
namespace algo  {
/********************************************************************************/

class AlignmentSplitter : public IAlignmentSplitter
{
public:
    AlignmentSplitter (IScoreMatrix* scoreMatrix, int openGapCost, int extendGapCost);
    virtual ~AlignmentSplitter ();

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

    size_t splitAlign (Alignment& align,  int* splittab);

    void computeInfo (Alignment& align);

private:

    IScoreMatrix* _scoreMatrix;
    void setScoreMatrix (IScoreMatrix* scoreMatrix)  { SP_SETATTR (scoreMatrix); }

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
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ALIGNMENT_SPLITTER_HPP_ */
