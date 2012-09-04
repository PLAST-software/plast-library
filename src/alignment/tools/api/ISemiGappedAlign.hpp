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

/** \file ISemiGapAlign.hpp
 *  \brief Interface providing means for splitting a gap alignment into ungap alignments.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ISEMI_GAP_ALIGN_HPP_
#define _ISEMI_GAP_ALIGN_HPP_

/********************************************************************************/

#include <designpattern/api/SmartPointer.hpp>

/********************************************************************************/
namespace alignment {
namespace tools     {
/********************************************************************************/

/** \brief To be completed
 */
class ISemiGapAlign : public dp::SmartPointer
{
public:

    /** */
    virtual ~ISemiGapAlign ()  {}

    /** */
    virtual int compute (
        const char* A,
        const char* B,
        int M,
        int N,
        int* a_offset,
        int* b_offset,
        int reverse_sequence
    ) = 0;
};

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/

#endif /* _ISEMI_GAP_ALIGN_HPP_ */
