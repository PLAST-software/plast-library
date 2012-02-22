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

/** \file IAlignmentSplitter.hpp
 *  \brief Interface providing means for splitting a gap alignment into ungap alignments.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _IALIGNMENT_SPLITTER_HPP_
#define _IALIGNMENT_SPLITTER_HPP_

/********************************************************************************/

#include <designpattern/api/SmartPointer.hpp>
#include <algo/core/api/IScoreMatrix.hpp>
#include <alignment/core/api/Alignment.hpp>

/********************************************************************************/
namespace alignment {
namespace tools     {
/********************************************************************************/

/** \brief Interface providing means for splitting a gap alignment into ungap alignments.
 *
 * An alignment splitter is able to split a gap alignment into several ungap alignments
 * (according to some specific score matrix).
 *
 * Implementations should use dynamic programming for providing the service.
 *
 * Note that auxiliaries information (such as identity number, number of misses, ...) may be computed.
 */
class IAlignmentSplitter : public dp::SmartPointer
{
public:

    /** Result information of a dynamic prog. */
    struct SplitOutput
    {
        u_int32_t*          splittab;
        u_int32_t           identity;
        u_int32_t           positive;
        u_int32_t           nbGapQry;
        u_int32_t           nbGapSbj;
        u_int32_t           nbMis;
        u_int32_t           alignSize;
        database::LETTER*   subjectAlign;
        database::LETTER*   queryAlign;

        SplitOutput (u_int32_t* tab=0, database::LETTER* sbjAlign=0, database::LETTER* qryAlign=0)
            : splittab(tab), subjectAlign(sbjAlign), queryAlign(qryAlign) {}
    };

    /** Split a gap alignment into ungap alignments.
     * \param[in] subjectSeq        : buffer holding the subject sequence
     * \param[in] querySeq          : buffer holding the query sequence
     * \param[in] subjectStartInSeq : start offset in the subject
     * \param[in] subjectEndInSeq   : stop offset in the subject
     * \param[in] queryStartInSeq   : start offset in the query
     * \param[in] queryEndInSeq     : stop offset in the query
     * \param[out] splittab         : concatenated offsets of the ungap split
     * \param[out] identity         : number of identical letters between both sequences
     * \param[out] nbGap            : number of gaps
     * \param[out] nbMis            : number of misses
     * \param[out] alignSize        : alignment size
     * \param[in] subjectAlign
     * \param[in] queryAlign
     */
    virtual size_t splitAlign (
        const database::LETTER* subjectSeq,
        const database::LETTER* querySeq,
        const misc::Range32&    sbjRange,
        const misc::Range32&    qryRange,
        SplitOutput& output
    ) = 0;

    /** Returns the split ungap parts from a (gap) alignment.
     * \param[in]  align    : the alignment to be split
     * \param[out] splittab : concatenated offsets of the ungap split
     * \return the number of splits
     */
    virtual size_t splitAlign (core::Alignment& align,  SplitOutput& output) = 0;
};

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/

#endif /* _IALIGNMENT_SPLITTER_HPP_ */
