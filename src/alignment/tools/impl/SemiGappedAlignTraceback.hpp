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

#ifndef _SEMI_GAP_ALIGN_TRACEBACK_HPP_
#define _SEMI_GAP_ALIGN_TRACEBACK_HPP_

/********************************************************************************/

#include <alignment/tools/impl/SemiGappedAlign.hpp>
#include <alignment/tools/api/IAlignmentSplitter.hpp>

/********************************************************************************/
namespace alignment {
namespace tools     {
namespace impl      {
/********************************************************************************/

/** \brief To be completed
 *
 *
 */
class SemiGapAlignTraceback : public SemiGapAlign
{
public:

    /** Constructor. */
    SemiGapAlignTraceback (
        algo::core::IScoreMatrix* scoreMatrix,
        int openGapCost,
        int extendGapCost,
        int Xdropoff
    );

    /** Destructor. */
    virtual ~SemiGapAlignTraceback ();

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

private:

    /** We define an alias for the variable managing scores. */
    typedef int32_t ScoreInt;

    struct BlastGapDP {
        ScoreInt best;       /* score of best path that ends in a match at this position */
        ScoreInt best_gap;   /* score of best path that ends in a gap   at this position */
    };

    /** Shortcut. */
    os::IMemoryAllocator& _memory;

    /** */
    u_int32_t   _dp_mem_alloc;
    u_int32_t   _edit_script_num_rows;

    BlastGapDP* _score_array;
    u_int8_t**  _edit_script;
    int32_t*    _edit_start_offset;

    /********************************************************************************/

    /** Structure to keep memory for state structure. */
    struct GapStateArrayStruct
    {
        int32_t              length;        /** length of the state_array.  */
        int32_t              used;          /** how much of length is used. */
        u_int8_t*            state_array;   /** array to be used.           */
        GapStateArrayStruct* next;          /** Next link in the list.      */
    };

    /** */
    bool gapPurgeState (GapStateArrayStruct* state_struct)
    {
       while (state_struct)
       {
          state_struct->used = 0;
          state_struct       = state_struct->next;
       }
       return true;
    }

    /** */
    GapStateArrayStruct* gapGetState (GapStateArrayStruct** head, u_int32_t length);

    /** */
    GapStateArrayStruct* _state_struct;
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _SEMI_GAP_ALIGN_TRACEBACK_HPP_ */
