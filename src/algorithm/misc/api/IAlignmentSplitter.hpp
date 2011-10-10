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

#ifndef _IALIGNMENT_SPLITTER_HPP_
#define _IALIGNMENT_SPLITTER_HPP_

/********************************************************************************/

#include "SmartPointer.hpp"

#include "IScoreMatrix.hpp"
#include "IAlignementResult.hpp"

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/** An alignment splitter is able to split a gap alignment into several ungap alignments
 * (according to some specific score matrix).
 *
 * Implementations should use dynamic programming for providing the service.
 */
class IAlignmentSplitter : public dp::SmartPointer
{
public:

    virtual size_t splitAlign (
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
    ) = 0;

    /** Returns the split ungap parts from a (gap) alignment). */
    virtual size_t splitAlign (Alignment& align,  int* splittab) = 0;

    /** Modify the provided alignment by computing some information (nb gap, identity...) */
    virtual void computeInfo (Alignment& align) = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _IALIGNMENT_SPLITTER_HPP_ */
