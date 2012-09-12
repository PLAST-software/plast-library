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

#include <alignment/tools/impl/SemiGappedAlignTraceback.hpp>

#include <iostream>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace database;
using namespace dp;
using namespace os;
using namespace os::impl;
using namespace algo::core;

/********************************************************************************/
namespace alignment {
namespace tools     {
namespace impl      {
/********************************************************************************/


/********************************************************************************/

/** Operation types within the edit script*/
typedef enum EGapAlignOpType {
   eGapAlignDel     = 0, /** Deletion: a gap in query */
   eGapAlignDel2    = 1, /** Frame shift deletion of two nucleotides */
   eGapAlignDel1    = 2, /** Frame shift deletion of one nucleotide */
   eGapAlignSub     = 3, /** Substitution */
   eGapAlignIns1    = 4, /** Frame shift insertion of one nucleotide */
   eGapAlignIns2    = 5, /** Frame shift insertion of two nucleotides */
   eGapAlignIns     = 6, /** Insertion: a gap in subject */
   eGapAlignDecline = 7, /** Non-aligned region */
   eGapAlignInvalid = 8  /** Invalid operation */
} EGapAlignOpType;

/********************************************************************************/

struct EditBlock
{
    EditBlock (EGapAlignOpType type, int32_t nb) : op_type(type), num_ops(nb) {}

    EGapAlignOpType op_type;
    int32_t         num_ops;
};

/********************************************************************************/

/** Values for the editing script operations in traceback */
enum {
    SCRIPT_SUB           = 0x0,     /**< Substitution */
    SCRIPT_GAP_IN_A      = 0x1,     /**< Deletion */
    SCRIPT_GAP_IN_B      = 0x2,     /**< Insertion */
    SCRIPT_OP_MASK       = 0x3,     /**< Mask for edit script operations */

    SCRIPT_EXTEND_GAP_A  = (1<<2), /**< continue a gap in A */
    SCRIPT_EXTEND_GAP_B  = (1<<3)  /**< continue a gap in B */
};

/********************************************************************************/

/** Lower bound for scores. */
#define MININT - ( 1 << (8*sizeof(ScoreInt) - 2) )

/** Minimal size of a chunk for state array allocation. */
#define CHUNKSIZE   2097152

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
SemiGapAlignTraceback::SemiGapAlignTraceback (
    algo::core::IScoreMatrix* scoreMatrix,
    int openGapCost,
    int extendGapCost,
    int Xdropoff
) : SemiGapAlign (scoreMatrix, openGapCost, extendGapCost, Xdropoff),
    _memory (DefaultFactory::memory()),
    _dp_mem_alloc (1000), _edit_script_num_rows(100),
    _score_array(0), _edit_script(0), _edit_start_offset(0),
    _state_struct (0)
{
    _score_array = (BlastGapDP *) _memory.malloc (_dp_mem_alloc * sizeof(BlastGapDP));

    _edit_script          = (u_int8_t**) _memory.malloc(sizeof(u_int8_t*) * _edit_script_num_rows);
    _edit_start_offset    = (int32_t*)   _memory.malloc(sizeof(int32_t)   * _edit_script_num_rows);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
SemiGapAlignTraceback::~SemiGapAlignTraceback ()
{
    _memory.free (_score_array);
    _memory.free (_edit_script);
    _memory.free (_edit_start_offset);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
int SemiGapAlignTraceback::compute (
	const char* A,
	const char* B,
	u_int32_t   M,
	u_int32_t   N,
	u_int32_t*  a_offset,
	u_int32_t*  b_offset,
    bool        reverse_sequence
)
{
    DEBUG (("----------------------------------------------------------------------------------------------------\n"));
    DEBUG (("SemiGapAlignTraceback::compute:  |Q|=%3d |S|=%3d \n", M,N));

    /** Define as local variable (ie. in stack). */
    int8_t** matrix             = _scoreMatrix->getMatrix();
    int      gap_extend         = _extendGapCost;
    int      gap_open_extend    = _openExtendGapCost;
    int      x_dropoff          = _Xdropoff;

    u_int32_t i;                     /* sequence pointers and indices */
    u_int32_t a_index;

    u_int32_t b_index;
    u_int32_t b_size;
    u_int32_t first_b_index;
    u_int32_t last_b_index;

    int8_t      b_increment = 0;
    const char* b_ptr       = 0;
    int8_t*     matrix_row  = 0;

    ScoreInt score         = 0;                 /* score tracking variables */
    ScoreInt score_gap_row = 0;
    ScoreInt score_gap_col = 0;
    ScoreInt next_score    = 0;
    ScoreInt best_score    = score;

    GapStateArrayStruct* state_struct = 0;
    u_int8_t*  edit_script_row        = 0;
    int32_t    orig_b_index           = 0;
    u_int8_t   script                 = 0;
    u_int32_t  num_extra_cells;

    /* do initialization and sanity-checking */
    *a_offset = 0;
    *b_offset = 0;

    if (x_dropoff < gap_open_extend)  {  x_dropoff = gap_open_extend;  }

    if (N <= 0 || M <= 0) {  return 0;  }

    /* Initialize traceback information. edit_script[] is a 2-D array which is filled in
     * row by row as the dynamic programming takes place
     */
    gapPurgeState (_state_struct);

    /* Conceptually, traceback requires a byte array of size MxN. To save on memory, each row of the array
     * is allocated separately. edit_script[i] points to the storage reserved for row i, and edit_start_offset[i]
     * gives the offset into the B sequence corresponding to element 0 of edit_script[i]. Also make the number of
     * edit script rows grow dynamically  */

    /* Allocate and fill in the auxiliary bookeeping structures. Since A and B could be very large, maintain a window
     * of auxiliary structures only large enough to contain to currentset of DP computations. The initial window
     * size is determined by the number of cells needed to fail the x-dropoff test */

    if (gap_extend > 0)     {  num_extra_cells = x_dropoff / gap_extend + 3;  }
    else                    {  num_extra_cells = N + 3;                       }

    if (num_extra_cells > _dp_mem_alloc)
    {
        _dp_mem_alloc = MAX(num_extra_cells + 100, 2 * _dp_mem_alloc);
        _score_array = (BlastGapDP *) _memory.realloc (_score_array, _dp_mem_alloc * sizeof(BlastGapDP));
    }

    state_struct = gapGetState (&_state_struct, num_extra_cells);

    _edit_script[0]       = state_struct->state_array;
    _edit_start_offset[0] = 0;
    edit_script_row       = state_struct->state_array;

    score       = -gap_open_extend;
    _score_array[0].best     = 0;
    _score_array[0].best_gap = -gap_open_extend;

    for (i = 1; i <= N; i++)
    {
        if (score < -x_dropoff)  {   break;  }

        _score_array[i].best      = score;
        _score_array[i].best_gap  = score - gap_open_extend;
        score                   -= gap_extend;
        edit_script_row[i]       = SCRIPT_GAP_IN_A;
    }

    state_struct->used = i + 1;

    /* The inner loop below examines letters of B from index 'first_b_index' to 'b_size' */
    b_size        = i;
    best_score    = 0;
    first_b_index = 0;

    if (reverse_sequence)   {  b_increment = -1;  }
    else                    {  b_increment =  1;  }

    /**********************************************************************/
    /**                             LOOP  A                               */
    /**********************************************************************/
    for (a_index = 1; a_index <= M; a_index++)
    {
        /* Set up the next row of the edit script; this involves allocating memory for the row, then pointing to it.
         * It is not necessary to allocate space for offsets less than first_b_index (since the inner loop will never
         * look at them); It is unknown at this point how far to the right the current traceback row will extend;
         * all that's known for sure is that the previous row fails the X-dropoff test after b_size cells, and that
         * the current row can go at most num_extra_cells beyond that before failing the test
         */
        if (gap_extend > 0) { state_struct = gapGetState (&_state_struct, b_size - first_b_index + num_extra_cells); }
        else                { state_struct = gapGetState (&_state_struct, N + 3 - first_b_index);                    }

        if (a_index == _edit_script_num_rows)
        {
            _edit_script_num_rows = _edit_script_num_rows * 2;
            _edit_script       = (u_int8_t**) _memory.realloc (_edit_script,       _edit_script_num_rows *sizeof(u_int8_t*));
            _edit_start_offset = (int32_t*)   _memory.realloc (_edit_start_offset, _edit_script_num_rows *sizeof(int32_t)  );
        }

        _edit_script[a_index]       = state_struct->state_array + state_struct->used + 1;
        _edit_start_offset[a_index] = first_b_index;

        /* the inner loop assumes the current traceback row begins at offset zero of B */
        edit_script_row = _edit_script[a_index] - first_b_index;
        orig_b_index    = first_b_index;

        /* pick out the row of the score matrix appropriate for A[a_index] */
        if (reverse_sequence)   {  matrix_row = matrix[(int) A[ M - a_index ] ];  }
        else                    {  matrix_row = matrix[(int) A[ a_index ] ];      }

        if (reverse_sequence)   {  b_ptr = &B[N - first_b_index];   }
        else                    {  b_ptr = &B[first_b_index];       }

        /* initialize running-score variables */
        score         = MININT;
        score_gap_row = MININT;
        last_b_index  = first_b_index;

        /**********************************************************************/
        /**                             LOOP  B                               */
        /**********************************************************************/
        for (b_index = first_b_index; b_index < b_size; b_index++)
        {
            /** Shortcut (avoid several memory access). */
            BlastGapDP& sc = _score_array[b_index];

            b_ptr        += b_increment;
            score_gap_col = sc.best_gap;
            next_score    = sc.best + matrix_row [(int) *b_ptr];

            /* script, script_row and script_col contain the actions specified by the dynamic programming.
             * when the inner loop has finished, 'script' contains all of the actions to perform, and is
             * written to edit_script[a_index][b_index]. */
            script = SCRIPT_SUB;

            if (score < score_gap_col)
            {
                score  = score_gap_col;
                script = SCRIPT_GAP_IN_B;
            }
            if (score < score_gap_row)
            {
                score  = score_gap_row;
                script = SCRIPT_GAP_IN_A;
            }

            if (best_score - score > x_dropoff)
            {
                /* the current best score failed the X-dropoff criterion. Note that this does not stop
                 * the inner loop, only forces future iterations to skip this column of B. Also, if the
                 * very first letter of B that was tested failed the X dropoff criterion, make sure future
                 * inner loops start one letter to the right */
                if (b_index == first_b_index)  {  first_b_index++;      }
                else                           {  sc.best = MININT;     }
            }

            else
            {
                last_b_index = b_index;

                if (score > best_score)
                {
                    best_score = score;
                    *a_offset  = a_index;
                    *b_offset  = b_index;
                }

                /* If starting a gap at this position will improve the best row, or column, score,
                 * update them to reflect that. */
                score_gap_row  -= gap_extend;
                score_gap_col  -= gap_extend;

                if (score_gap_col < score - gap_open_extend)
                {
                    sc.best_gap = score - gap_open_extend;
                }
                else
                {
                    sc.best_gap = score_gap_col;
                    script |= SCRIPT_EXTEND_GAP_B;
                }

                if (score_gap_row < score - gap_open_extend)
                {
                    score_gap_row = score - gap_open_extend;
                }
                else
                {
                    script |= SCRIPT_EXTEND_GAP_A;
                }

                sc.best = score;
            }

            score                    = next_score;
            edit_script_row[b_index] = script;

        /**********************************************************************/
        } /* end of for (b_index =... */
        /**********************************************************************/

        /* Finish aligning if the best scores for all positions of B will fail the X-dropoff test,
         * i.e. the inner loop bounds have converged to each other */
        if (first_b_index == b_size)  {   break;  }

        /* enlarge the window for score data if necessary */

        if (last_b_index + num_extra_cells + 3 >= _dp_mem_alloc)
        {
            _dp_mem_alloc = MAX(last_b_index + num_extra_cells + 100, 2 * _dp_mem_alloc);
            _score_array = (BlastGapDP *) _memory.realloc(_score_array, _dp_mem_alloc * sizeof(BlastGapDP));
        }

        if (last_b_index < b_size - 1)
        {
            /* This row failed the X-dropoff test earlier than the last row did; just shorten
             * the loop bounds before doing the next row */
            b_size = last_b_index + 1;
        }
        else
        {
            /* The inner loop finished without failing the X-dropoff test; initialize extra
             * bookkeeping structures until the X dropoff test fails or we run out of letters in B.
             * The next inner loop will have larger bounds */
            while (score_gap_row >= (best_score - x_dropoff) && b_size <= N)
            {
                _score_array[b_size].best     = score_gap_row;
                _score_array[b_size].best_gap = score_gap_row - gap_open_extend;
                score_gap_row               -= gap_extend;
                edit_script_row[b_size]      = SCRIPT_GAP_IN_A;
                b_size++;
            }
        }

        /* update the memory allocator to reflect the exact number of traceback cells this row needed */
        state_struct->used += MAX(b_index, b_size) - orig_b_index + 1;

        if (b_size <= N)
        {
            _score_array[b_size].best     = MININT;
            _score_array[b_size].best_gap = MININT;
            b_size++;
        }

    /**********************************************************************/
    } /* end of for (a_index =... */
    /**********************************************************************/

    /* Pick the optimal path through the now complete edit_script[][].
     * This is equivalent to flattening the 2-D array into a 1-D list of actions. */

    a_index = *a_offset;
    b_index = *b_offset;

#if 0
    script = SCRIPT_SUB;

    list<EditBlock> blocksList;

    while (a_index > 0 || b_index > 0)
    {
        /* Retrieve the next action to perform. Rows of the traceback array do not necessarily start
           at offset zero of B, so a correction is needed to point to the correct position */

        u_int8_t next_script = _edit_script[a_index][b_index - _edit_start_offset[a_index]];

        switch(script)
        {
        case SCRIPT_GAP_IN_A:
            script = next_script & SCRIPT_OP_MASK;
            if (next_script & SCRIPT_EXTEND_GAP_A)  { script = SCRIPT_GAP_IN_A; }
            break;

        case SCRIPT_GAP_IN_B:
            script = next_script & SCRIPT_OP_MASK;
            if (next_script & SCRIPT_EXTEND_GAP_B)  { script = SCRIPT_GAP_IN_B; }
            break;

        default:
            script = next_script & SCRIPT_OP_MASK;
            break;
        }

             if (script == SCRIPT_GAP_IN_A) { b_index--; }
        else if (script == SCRIPT_GAP_IN_B) { a_index--; }
        else
        {
            a_index--;
            b_index--;
        }

        if (blocksList.empty())
        {
            blocksList.push_back (EditBlock((EGapAlignOpType)script, 1));
        }
        else
        {
            EditBlock& back = blocksList.back ();
            if (back.op_type == (EGapAlignOpType)script)
            {
                back.num_ops ++;
            }
            else
            {
                blocksList.push_back (EditBlock((EGapAlignOpType)script, 1));
            }
        }

#if 0
        GapPrelimEditBlockAdd (edit_block, (EGapAlignOpType)script, 1);
#endif
    }


 printf ("SemiGapAlignTraceback::compute: nbBlocks=%ld  (reverse=%d) \n", blocksList.size (), reverse_sequence);

 for (list<EditBlock>::iterator it = blocksList.begin(); it != blocksList.end(); it++)
 {
     printf ("    block: type=%d  nb=%d \n", it->op_type, it->num_ops);
 }

#endif

    DEBUG (("SemiGapAlignTraceback::compute:  |Q|=%3d |S|=%3d  => a_index=%d  b_index=%d  score=%d  best=%d  aOffset=%d  bOffset=%d  b_size=%d  first_b_index=%d\n",
        M,N, a_index, b_index, score, best_score, *a_offset, *b_offset, b_size, first_b_index
    ));

    return best_score;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
SemiGapAlignTraceback::GapStateArrayStruct* SemiGapAlignTraceback::gapGetState (GapStateArrayStruct** head, u_int32_t length)
{
    GapStateArrayStruct* retval = 0;
    GapStateArrayStruct* var    = 0;
    GapStateArrayStruct* last   = 0;

    int32_t chunksize = MAX(CHUNKSIZE, length + length/3);

    length += length/3;  /* Add on about 30% so the end will get reused. */
    retval = NULL;

    if (*head == NULL)
    {
       retval = (GapStateArrayStruct*)   _memory.malloc(sizeof(GapStateArrayStruct));
       retval->state_array = (u_int8_t*) _memory.malloc(chunksize*sizeof(u_int8_t));
       retval->length = chunksize;
       retval->used = 0;
       retval->next = NULL;
       *head = retval;
    }
    else
    {
       var = *head;
       last = *head;
       while (var)
       {
          if (length < (var->length - var->used))
          {
             retval = var;
             break;
          }
          else if (var->used == 0)
          {
             /* If it's empty and too small, replace. */
              _memory.free (var->state_array);
             var->state_array = (u_int8_t*) _memory.malloc (chunksize*sizeof(u_int8_t));
             var->length = chunksize;
             retval = var;
             break;
          }
          last = var;
          var = var->next;
       }

       if (var == NULL)
       {
          retval = (GapStateArrayStruct*) _memory.malloc(sizeof(GapStateArrayStruct));
          retval->state_array = (u_int8_t*) _memory.malloc(chunksize*sizeof(u_int8_t));
          retval->length = chunksize;
          retval->used = 0;
          retval->next = NULL;
          last->next = retval;
       }
    }

    return retval;
}

#if 0
/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void SemiGapAlignTraceback::GapPrelimEditBlockAdd (GapPrelimEditBlock *edit_block, EGapAlignOpType op_type, Int4 num_ops)
{
    if (num_ops == 0)
        return;

    if (edit_block->last_op == op_type)
        edit_block->edit_ops[edit_block->num_ops-1].num += num_ops;
    else
        s_GapPrelimEditBlockAddNew(edit_block, op_type, num_ops);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
GapPrelimEditBlock* SemiGapAlignTraceback::GapPrelimEditBlockNew(void)
{
    GapPrelimEditBlock *edit_block = malloc(sizeof(GapPrelimEditBlock));
    if (edit_block != NULL) {
        edit_block->edit_ops = NULL;
        edit_block->num_ops_allocated = 0;
        edit_block->num_ops = 0;
        edit_block->last_op = eGapAlignInvalid;
        s_GapPrelimEditBlockRealloc(edit_block, 100);
    }
    return edit_block;
}
#endif

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
