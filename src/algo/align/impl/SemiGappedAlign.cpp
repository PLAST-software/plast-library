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

#include <algo/align/impl/SemiGappedAlign.hpp>

#include <iostream>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace database;
using namespace dp;
using namespace os;
using namespace os::impl;
using namespace algo::core;

//#define VERBOSE

/********************************************************************************/
namespace algo   {
namespace align {
namespace impl   {
/********************************************************************************/

/** We define an alias for the variable managing scores. */
typedef int32_t ScoreInt;

struct BlastGapDP {
    ScoreInt best;       /* score of best path that ends in a match at this position */
    ScoreInt best_gap;   /* score of best path that ends in a gap   at this position */
};

/** Lower bound for scores. */
#define MININT - ( 1 << (8*sizeof(ScoreInt) - 2) )

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
SemiGapAlign::SemiGapAlign (
    algo::core::IScoreMatrix* scoreMatrix,
    int openGapCost,
    int extendGapCost,
    int Xdropoff
)
    : _scoreMatrix(0), _openGapCost(openGapCost), _extendGapCost(extendGapCost),
      _openExtendGapCost(openGapCost + extendGapCost), _Xdropoff(Xdropoff)
{
    setScoreMatrix (scoreMatrix);

    /** We may have to recalibrate the xdrop criterion. */
    _Xdropoff = MAX (_Xdropoff, _openExtendGapCost);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
SemiGapAlign::~SemiGapAlign ()
{
    setScoreMatrix (0);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
int SemiGapAlign::compute (
    char* A,
    char* B,
    int M,
    int N,
    int* a_offset,
    int* b_offset,
    int reverse_sequence
)
{
    /** Just a check. */
//    if (N <= 0 || M <= 0)  {   return 0;  }

    /** Define as local variable (ie. in stack). */
    int8_t** matrix             = _scoreMatrix->getMatrix();
    int      gap_extend         = _extendGapCost;
    int      gap_open_extend    = _openExtendGapCost;
    int      x_dropoff          = _Xdropoff;

    u_int16_t i;                     /* sequence pointers and indices */
    u_int16_t a_index;

    u_int16_t b_index;
    u_int16_t b_size;
    u_int16_t first_b_index;
    u_int16_t last_b_index;

    int8_t      b_increment = 0;
    const char* b_ptr       = 0;
    BlastGapDP* score_array = 0;
    int8_t*     matrix_row  = 0;

    ScoreInt score;                 /* score tracking variables */
    ScoreInt score_gap_row;
    ScoreInt score_gap_col;
    ScoreInt next_score;
    ScoreInt best_score;

    u_int16_t num_extra_cells;
    u_int16_t dp_mem_alloc = 1000;

    /* do initialization and sanity-checking */
    *a_offset = 0;
    *b_offset = 0;

    /* Allocate and fill in the auxiliary bookeeping structures.
       Since A and B could be very large, maintain a window
       of auxiliary structures only large enough to contain to current
       set of DP computations. The initial window size is determined
       by the number of cells needed to fail the x-dropoff test */

    if (gap_extend > 0)     {  num_extra_cells = x_dropoff / gap_extend + 3;  }
    else                    {  num_extra_cells = N + 3;                       }

    if (num_extra_cells > dp_mem_alloc)
    {
        dp_mem_alloc = MAX(num_extra_cells + 100, 2 * dp_mem_alloc);
    }

    score_array = (BlastGapDP *) DefaultFactory::memory().malloc (dp_mem_alloc * sizeof(BlastGapDP));;
    score       = -gap_open_extend;
    score_array[0].best     = 0;
    score_array[0].best_gap = -gap_open_extend;

    for (i = 1; i <= N; i++)
    {
        if (score < -x_dropoff)  {   break;  }

        score_array[i].best      = score;
        score_array[i].best_gap  = score - gap_open_extend;
        score                   -= gap_extend;
    }

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
        /* pick out the row of the score matrix appropriate for A[a_index] */
        if (reverse_sequence)   {  matrix_row = matrix[(int) A[ M - a_index ] ];  }
        else                    {  matrix_row = matrix[(int) A[ a_index ] ];      }

        if (reverse_sequence)   {  b_ptr = &B[N - first_b_index];   }
        else                    {  b_ptr = &B[first_b_index];       }

//if (reverse_sequence)   {  printf ("    >>> M=%d  N=%d  A[%2d]=%2d  ", M,N, a_index, A[ M - a_index ]);  }
//else                    {  printf ("    >>> M=%d  N=%d  A[%2d]=%2d  ", M,N, a_index, A[ a_index ]);  }

        /* initialize running-score variables */
        score         = MININT;
        score_gap_row = MININT;
        last_b_index  = first_b_index;

        /**********************************************************************/
        /**                             LOOP  B                               */
        /**********************************************************************/
//printf ("   >>>>>>>>> B (in [%2d..%2d]) : ", first_b_index, b_size-1);
        for (b_index = first_b_index; b_index < b_size; b_index++)
        {
            /** Shortcut (avoid several memory access). */
            BlastGapDP& sc = score_array[b_index];

            b_ptr        += b_increment;
            score_gap_col = sc.best_gap;
            next_score    = sc.best + matrix_row [(int) *b_ptr];

//printf ("%2d ", *b_ptr);

            /** We update the score as being Max (score, score_gap_col, score_gap_row) */
            if (score < score_gap_col)  {  score = score_gap_col;  }
            if (score < score_gap_row)  {  score = score_gap_row;  }

            if (best_score - score > x_dropoff)
            {
                /* the current best score failed the X-dropoff criterion. Note that this does not stop
                 * the inner loop, only forces future iterations to skip this column of B.
                   Also, if the very first letter of B that was tested failed the X dropoff criterion,
                   make sure future inner loops start one letter to the right */
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
                score_gap_row   = MAX (score - gap_open_extend, score_gap_row);
                sc.best_gap     = MAX (score - gap_open_extend, score_gap_col);
                sc.best         = score;
            }

            score = next_score;

        /**********************************************************************/
        } /* end of for (b_index =... */
        /**********************************************************************/
//printf("\n");

        /* Finish aligning if the best scores for all positions of B will fail the X-dropoff test,
         * i.e. the inner loop bounds have converged to each other */
        if (first_b_index == b_size)  {   break;  }

        /* enlarge the window for score data if necessary */

        if (last_b_index + num_extra_cells + 3 >= dp_mem_alloc)
        {
            dp_mem_alloc = MAX(last_b_index + num_extra_cells + 100, 2 * dp_mem_alloc);
            score_array = (BlastGapDP *) DefaultFactory::memory().realloc(score_array, dp_mem_alloc * sizeof(BlastGapDP));
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
               The next inner loop will have larger bounds */
            while (score_gap_row >= (best_score - x_dropoff) && b_size <= N)
            {
                score_array[b_size].best     = score_gap_row;
                score_array[b_size].best_gap = score_gap_row - gap_open_extend;
                score_gap_row               -= gap_extend;
                b_size++;
            }
        }

        if (b_size <= N)
        {
            score_array[b_size].best     = MININT;
            score_array[b_size].best_gap = MININT;
            b_size++;
        }

    /**********************************************************************/
    } /* end of for (a_index =... */
    /**********************************************************************/

    DefaultFactory::memory().free (score_array);

    return best_score;
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
