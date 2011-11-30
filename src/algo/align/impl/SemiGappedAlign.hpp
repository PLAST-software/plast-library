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

#include <algo/align/api/IAlignmentSplitter.hpp>

/********************************************************************************/
namespace algo   {
namespace align {
/** \brief Implementation of concepts about alignments with gaps. */
namespace impl   {
/********************************************************************************/

/** \brief To be completed
 *
 *
 */
class SemiGapAlign : public dp::SmartPointer
{
public:

    /** Constructor. */
    SemiGapAlign () {}

    /** Destructor. */
    virtual ~SemiGapAlign () {}

    /** */
    int compute (
        char* A,
        char* B,
        int M,
        int N,
        int* a_offset,
        int* b_offset,
        int reverse_sequence,
        int8_t** matrix,
        int gap_open,
        int gap_extend,
        int x_dropoff
    );

    /** */
    int compute2 (
        char* A,
        char* B,
        int M,
        int N,
        int* a_offset,
        int* b_offset,
        int reverse_sequence,
        int8_t** matrix,
        int gap_open,
        int gap_extend,
        int x_dropoff
    );

    /** */
    int computeblast (
        char* A,
        char* B,
        int M,
        int N,
        int* a_offset,
        int* b_offset,
        int reverse_sequence,
        int8_t** matrix,
        int gap_open,
        int gap_extend,
        int x_dropoff
    );

};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _SEMI_GAP_ALIGN_HPP_ */
