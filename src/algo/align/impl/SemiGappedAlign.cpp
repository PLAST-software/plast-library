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

struct BlastGapDP {
    int best;       /* score of best path that ends in a match at this position */
    int best_gap;   /* score of best path that ends in a gap   at this position */
};

#ifndef INT4_MIN
    /** Smallest (most negative) number represented by signed int */
    #define INT4_MIN    (-2147483647-1)
#endif

/** Lower bound for scores. Divide by two to prevent underflows. */
#define MININT INT4_MIN/2


/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
//#define TT

int SemiGapAlign::compute2 (
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
)
{
    int i;                     /* sequence pointers and indices */
    int a_index;
    int b_index, b_size, first_b_index, last_b_index, b_increment;
    const char* b_ptr;

    BlastGapDP* score_array;

    int gap_open_extend;

    int8_t* matrix_row = NULL;

    int score;                 /* score tracking variables */
    int score_gap_row;
    int score_gap_col;
    int next_score;
    int best_score;
    int num_extra_cells;

    int dp_mem_alloc = 1000;

    /* do initialization and sanity-checking */

    *a_offset = 0;
    *b_offset = 0;

    gap_open_extend = gap_open + gap_extend;

    if (x_dropoff < gap_open_extend)
        x_dropoff = gap_open_extend;

    if(N <= 0 || M <= 0)
        return 0;

    /* Allocate and fill in the auxiliary bookeeping structures.
       Since A and B could be very large, maintain a window
       of auxiliary structures only large enough to contain to current
       set of DP computations. The initial window size is determined
       by the number of cells needed to fail the x-dropoff test */

    if (gap_extend > 0)     num_extra_cells = x_dropoff / gap_extend + 3;
    else                    num_extra_cells = N + 3;

    if (num_extra_cells > dp_mem_alloc)
    {
        dp_mem_alloc = MAX(num_extra_cells + 100, 2 * dp_mem_alloc);
    }

    score_array = (BlastGapDP *) DefaultFactory::memory().malloc (dp_mem_alloc * sizeof(BlastGapDP));;

    score_array[0].best     = 0;
    score_array[0].best_gap = -gap_open_extend;
    score                   = -gap_open_extend;

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

    if (reverse_sequence)   b_increment = -1;
    else                    b_increment =  1;

    /**********************************************************************/
    /**                             LOOP  A                               */
    /**********************************************************************/
    for (a_index = 1; a_index <= M; a_index++)
    {
        /* pick out the row of the score matrix appropriate for A[a_index] */

        if(reverse_sequence)    matrix_row = matrix[(int) A[ M - a_index + 1] ];
        else                    matrix_row = matrix[(int) A[ a_index - 1] ];

        if(reverse_sequence)    b_ptr = &B[N - first_b_index];
        else                    b_ptr = &B[first_b_index];

#ifdef TT
printf ("------------------------- a=%d  b_ptr=%d   first_b_index=%d  b_size=%d  ------------------------------\n",
    (reverse_sequence ? A[M-a_index+1] : A[a_index-1]), *b_ptr, first_b_index, b_size
);
#endif

#ifdef TT
    for (i=0; i<=N; i++)
    {
        printf ("[%d]  best=%d  best_gap=%d\n", i, score_array[i].best, score_array[i].best_gap);
    }
#endif

        /* initialize running-score variables */
        score         = MININT;
        score_gap_row = MININT;
        last_b_index  = first_b_index;

        /**********************************************************************/
        /**                             LOOP  B                               */
        /**********************************************************************/
        int idx = 0;
        for (b_index = first_b_index; b_index < b_size; b_index++, b_ptr += b_increment)
        {
            int currentB =  *b_ptr;

            score_gap_col = score_array[b_index].best_gap;
            next_score    = score_array[b_index].best + matrix_row[currentB];
#ifdef TT
printf ("%d[%d,%d,%d][%d,%d]  ---  ",
    b_index,
    currentB, score_gap_col, next_score,
    score, best_score
);
#endif

            if (score < score_gap_col)      score = score_gap_col;
            if (score < score_gap_row)      score = score_gap_row;

            if (best_score - score > x_dropoff)
            {
                /* the current best score failed the X-dropoff
                   criterion. Note that this does not stop the
                   inner loop, only forces future iterations to
                   skip this column of B.

                   Also, if the very first letter of B that was
                   tested failed the X dropoff criterion, make
                   sure future inner loops start one letter to
                   the right */

                if (b_index == first_b_index)
                    first_b_index++;
                else
                    score_array[b_index].best = MININT;
            }
            else
            {
                last_b_index = b_index;

                if (score > best_score)
                {
                    best_score = score;
                    *a_offset = a_index;
                    *b_offset = b_index;
                }

                /* If starting a gap at this position will improve
                   the best row, or column, score, update them to
                   reflect that. */
                score_gap_row -= gap_extend;
                score_gap_col -= gap_extend;

                score_array[b_index].best_gap = MAX(score - gap_open_extend, score_gap_col);
                score_gap_row                 = MAX(score - gap_open_extend, score_gap_row);
                score_array[b_index].best     = score;
            }

            score = next_score;

        /**********************************************************************/
        } /* end of for (b_index =... */
        /**********************************************************************/

#ifdef TT
printf ("\n");
printf ("=> END OF LOOP B: b_size=%d  score=%d  best_score=%d  \n", b_size, score, best_score);
#endif
        /* Finish aligning if the best scores for all positions
           of B will fail the X-dropoff test, i.e. the inner loop
           bounds have converged to each other */

        if (first_b_index == b_size)
            break;

        /* enlarge the window for score data if necessary */

        if (last_b_index + num_extra_cells + 3 >= dp_mem_alloc)
        {

            dp_mem_alloc = MAX(last_b_index + num_extra_cells + 100, 2 * dp_mem_alloc);
            score_array = (BlastGapDP *) DefaultFactory::memory().realloc (score_array, dp_mem_alloc *sizeof(BlastGapDP));
        }

        if (last_b_index < b_size - 1)
        {
            /* This row failed the X-dropoff test earlier than
               the last row did; just shorten the loop bounds
               before doing the next row */

            b_size = last_b_index + 1;
#ifdef TT
printf ("1. b_size=%d\n", b_size);
#endif
        }
        else
        {
            /* The inner loop finished without failing the X-dropoff
               test; initialize extra bookkeeping structures until
               the X dropoff test fails or we run out of letters in B.
               The next inner loop will have larger bounds */

            while (score_gap_row >= (best_score - x_dropoff) && b_size <= N)
            {
#ifdef TT
printf ("ENLARGE WINDOW: b_size=%d\n", b_size);
#endif
                score_array[b_size].best     = score_gap_row;
                score_array[b_size].best_gap = score_gap_row - gap_open_extend;
                score_gap_row               -= gap_extend;
                b_size++;
            }
#ifdef TT
printf ("2. b_size=%d\n", b_size);
#endif
        }

        if (b_size <= N)
        {
            score_array[b_size].best     = MININT;
            score_array[b_size].best_gap = MININT;
            b_size++;
#ifdef TT
printf ("3. b_size=%d\n", b_size);
#endif
        }

    /**********************************************************************/
    } /* end of for (a_index =... */
    /**********************************************************************/

    DefaultFactory::memory().free (score_array);

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
int SemiGapAlign::compute (
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
)
{
    return computeblast (A-1,B-1,M,N,a_offset,b_offset,reverse_sequence, matrix, gap_open,gap_extend,x_dropoff);

    int i;                     /* sequence pointers and indices */
    int a_index;
    int b_index, b_size, first_b_index, last_b_index, b_increment;
    const char* b_ptr;

    BlastGapDP* score_array;

    int gap_open_extend;

    int8_t* matrix_row = NULL;

    int score;                 /* score tracking variables */
    int score_gap_row;
    int score_gap_col;
    int next_score;
    int best_score;
    int num_extra_cells;

    int dp_mem_alloc = 1000;

    /* do initialization and sanity-checking */

    *a_offset = 0;
    *b_offset = 0;

    gap_open_extend = gap_open + gap_extend;

    if (x_dropoff < gap_open_extend)
        x_dropoff = gap_open_extend;

    if(N <= 0 || M <= 0)
        return 0;

    /* Allocate and fill in the auxiliary bookeeping structures.
       Since A and B could be very large, maintain a window
       of auxiliary structures only large enough to contain to current
       set of DP computations. The initial window size is determined
       by the number of cells needed to fail the x-dropoff test */

    if (gap_extend > 0)
        num_extra_cells = x_dropoff / gap_extend + 3;
    else
        num_extra_cells = N + 3;

    if (num_extra_cells > dp_mem_alloc)
    {
        dp_mem_alloc = MAX(num_extra_cells + 100, 2 * dp_mem_alloc);
    }

    score_array = (BlastGapDP *) DefaultFactory::memory().malloc (dp_mem_alloc * sizeof(BlastGapDP));;
    score = -gap_open_extend;
    score_array[0].best = 0;
    score_array[0].best_gap = -gap_open_extend;

    for (i = 1; i <= N; i++)
    {
        if (score < -x_dropoff)  {   break;  }

        score_array[i].best      = score;
        score_array[i].best_gap  = score - gap_open_extend;
        score                   -= gap_extend;
    }

    /* The inner loop below examines letters of B from
       index 'first_b_index' to 'b_size' */
    b_size        = i;
    best_score    = 0;
    first_b_index = 0;

    if (reverse_sequence)   {  b_increment = -1;  }
    else                    {  b_increment =  1;  }

    for (a_index = 1; a_index <= M; a_index++)
    {
        /* pick out the row of the score matrix appropriate for A[a_index] */
        if (reverse_sequence)   {  matrix_row = matrix[(int) A[ M - a_index ] ];  }
        else                    {  matrix_row = matrix[(int) A[ a_index ] ];      }

        if (reverse_sequence)   {  b_ptr = &B[N - first_b_index];   }
        else                    {  b_ptr = &B[first_b_index];       }

        /* initialize running-score variables */
        score         = MININT;
        score_gap_row = MININT;
        last_b_index  = first_b_index;

#ifdef TT
printf ("0) reverse=%d  first_b_index=%d   b_size=%d  val=%d \n ",
    reverse_sequence, first_b_index, b_size,
    (reverse_sequence ?  A[ M - a_index ]  : A[ a_index ]  )
);
//const char* tmp = reverse_sequence ? &B[N - first_b_index] : &B[first_b_index];
//for (size_t k=first_b_index; k<b_size; k++)  { printf ("%2d ", *tmp);  tmp+=b_increment; }
//printf ("\n           ==> ");
#endif

        for (b_index = first_b_index; b_index < b_size; b_index++)
        {
#ifdef WITH_DYNPRO_CORRECT
            if (_checkMemory == true)
            {
                /** We add some guard conditions in order no to read in unwanted memory.
                 *  (otherwise valgrind won't be happy)  */
                if (reverse_sequence) {  if (b_ptr<=B)   { break;  } }
                else                  {  if (b_ptr>=B+N) { break;  } }
            }
#endif
            b_ptr        += b_increment;
            score_gap_col = score_array[b_index].best_gap;
            next_score    = score_array[b_index].best + matrix_row[ (int)*b_ptr ];

#ifdef TT
printf ("%d[%d] ", *b_ptr,  matrix_row[ (int)*b_ptr ]);
#endif
            if (score < score_gap_col)
                score = score_gap_col;

            if (score < score_gap_row)
                score = score_gap_row;

            if (best_score - score > x_dropoff)
            {
                /* the current best score failed the X-dropoff criterion. Note that this does not stop
                 * the inner loop, only forces future iterations to skip this column of B.

                   Also, if the very first letter of B that was tested failed the X dropoff criterion,
                   make sure future inner loops start one letter to the right */

                if (b_index == first_b_index)
                    first_b_index++;
                else
                    score_array[b_index].best = MININT;
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
                score_gap_row                -= gap_extend;
                score_gap_col                -= gap_extend;
                score_array[b_index].best_gap = MAX (score - gap_open_extend, score_gap_col);
                score_gap_row                 = MAX (score - gap_open_extend, score_gap_row);
                score_array[b_index].best     = score;
            }

            score = next_score;

        } /* end of for (b_index... */

#ifdef TT
    printf ("\n");
#endif
        /* Finish aligning if the best scores for all positions
           of B will fail the X-dropoff test, i.e. the inner loop
           bounds have converged to each other */

        if (first_b_index == b_size)
            break;

        /* enlarge the window for score data if necessary */

        if (last_b_index + num_extra_cells + 3 >= dp_mem_alloc)
        {
            dp_mem_alloc = MAX(last_b_index + num_extra_cells + 100, 2 * dp_mem_alloc);
            score_array = (BlastGapDP *) DefaultFactory::memory().realloc(score_array, dp_mem_alloc * sizeof(BlastGapDP));
        }

        if (last_b_index < b_size - 1)
        {
            /* This row failed the X-dropoff test earlier than
               the last row did; just shorten the loop bounds
               before doing the next row */

            b_size = last_b_index + 1;
//if (b_size>=N)  { printf ("ko2  bsize=%d  N=%d\n", b_size, N); }
        }
        else
        {
            /* The inner loop finished without failing the X-dropoff
               test; initialize extra bookkeeping structures until
               the X dropoff test fails or we run out of letters in B.
               The next inner loop will have larger bounds */

            while (score_gap_row >= (best_score - x_dropoff) && b_size <= N)
            {
                score_array[b_size].best     = score_gap_row;
                score_array[b_size].best_gap = score_gap_row - gap_open_extend;
                score_gap_row               -= gap_extend;
                b_size++;
            }
//if (b_size>=N)  { printf ("ko3  bsize=%d  N=%d   prev=%d\n", b_size, N, k); }
        }

        if (b_size <= N)
        {
            score_array[b_size].best     = MININT;
            score_array[b_size].best_gap = MININT;
            b_size++;
//if (b_size>=N)  { printf ("ko4  bsize=%d  N=%d\n", b_size, N); }
        }


    } /* end of for (a_index... */

    DefaultFactory::memory().free (score_array);

#ifdef TT
    printf ("=========> %d %d  (score=%d)\n", *a_offset, *b_offset, best_score);
#endif
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
int SemiGapAlign::computeblast (
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
)
{
    int i;                     /* sequence pointers and indices */
    int a_index;
    int b_index, b_size, first_b_index, last_b_index, b_increment;
    const char* b_ptr;

    BlastGapDP* score_array = 0;

    int dp_mem_alloc = 1000;

    int gap_open_extend;

    int8_t* matrix_row = NULL;

    int score;                 /* score tracking variables */
    int score_gap_row;
    int score_gap_col;
    int next_score;
    int best_score;
    int num_extra_cells;

    /* do initialization and sanity-checking */

    *a_offset = 0;
    *b_offset = 0;
    gap_open_extend = gap_open + gap_extend;

    if (x_dropoff < gap_open_extend)
        x_dropoff = gap_open_extend;

    if(N <= 0 || M <= 0)
        return 0;

    /* Allocate and fill in the auxiliary bookeeping structures.
       Since A and B could be very large, maintain a window
       of auxiliary structures only large enough to contain to current
       set of DP computations. The initial window size is determined
       by the number of cells needed to fail the x-dropoff test */

    if (gap_extend > 0)
        num_extra_cells = x_dropoff / gap_extend + 3;
    else
        num_extra_cells = N + 3;


    if (num_extra_cells > dp_mem_alloc)
    {
        dp_mem_alloc = MAX(num_extra_cells + 100, 2 * dp_mem_alloc);
    }

    score_array = (BlastGapDP *) malloc (dp_mem_alloc * sizeof(BlastGapDP));

    score = -gap_open_extend;
    score_array[0].best = 0;
    score_array[0].best_gap = -gap_open_extend;

    for (i = 1; i <= N; i++) {
        if (score < -x_dropoff)
            break;

        score_array[i].best = score;
        score_array[i].best_gap = score - gap_open_extend;
        score -= gap_extend;
    }

    /* The inner loop below examines letters of B from
       index 'first_b_index' to 'b_size' */

    b_size = i;
    best_score = 0;
    first_b_index = 0;
    if (reverse_sequence)
        b_increment = -1;
    else
        b_increment = 1;

    for (a_index = 1; a_index <= M; a_index++) {
        /* pick out the row of the score matrix
           appropriate for A[a_index] */

        if(reverse_sequence)
            matrix_row = matrix[ A[ M - a_index ] ];
        else
            matrix_row = matrix[ A[ a_index ] ];

        if(reverse_sequence)
            b_ptr = &B[N - first_b_index];
        else
            b_ptr = &B[first_b_index];

        /* initialize running-score variables */
        score = MININT;
        score_gap_row = MININT;
        last_b_index = first_b_index;

        for (b_index = first_b_index; b_index < b_size; b_index++) {

            b_ptr += b_increment;
            score_gap_col = score_array[b_index].best_gap;
            next_score = score_array[b_index].best + matrix_row[ *b_ptr ];

            if (score < score_gap_col)
                score = score_gap_col;

            if (score < score_gap_row)
                score = score_gap_row;

            if (best_score - score > x_dropoff) {

                /* the current best score failed the X-dropoff
                   criterion. Note that this does not stop the
                   inner loop, only forces future iterations to
                   skip this column of B.

                   Also, if the very first letter of B that was
                   tested failed the X dropoff criterion, make
                   sure future inner loops start one letter to
                   the right */

                if (b_index == first_b_index)
                    first_b_index++;
                else
                    score_array[b_index].best = MININT;
            }
            else {
                last_b_index = b_index;
                if (score > best_score) {
                    best_score = score;
                    *a_offset = a_index;
                    *b_offset = b_index;
                    //printf ("a=%d b=%d score=%d\n", *a_offset, *b_offset, best_score);
                }

                /* If starting a gap at this position will improve
                   the best row, or column, score, update them to
                   reflect that. */

                score_gap_row -= gap_extend;
                score_gap_col -= gap_extend;
                score_array[b_index].best_gap = MAX(score - gap_open_extend,
                                                    score_gap_col);
                score_gap_row = MAX(score - gap_open_extend, score_gap_row);
                score_array[b_index].best = score;
            }

            score = next_score;
        }

        /* Finish aligning if the best scores for all positions
           of B will fail the X-dropoff test, i.e. the inner loop
           bounds have converged to each other */

        if (first_b_index == b_size)
            break;

        /* enlarge the window for score data if necessary */

        if (last_b_index + num_extra_cells + 3 >= dp_mem_alloc)
        {
            dp_mem_alloc = MAX(last_b_index + num_extra_cells + 100, 2 * dp_mem_alloc);
            score_array = (BlastGapDP *)realloc(score_array, dp_mem_alloc * sizeof(BlastGapDP));
        }

        if (last_b_index < b_size - 1) {
            /* This row failed the X-dropoff test earlier than
               the last row did; just shorten the loop bounds
               before doing the next row */

            b_size = last_b_index + 1;
        }
        else {
            /* The inner loop finished without failing the X-dropoff
               test; initialize extra bookkeeping structures until
               the X dropoff test fails or we run out of letters in B.
               The next inner loop will have larger bounds */

            while (score_gap_row >= (best_score - x_dropoff) && b_size <= N) {
                score_array[b_size].best = score_gap_row;
                score_array[b_size].best_gap = score_gap_row - gap_open_extend;
                score_gap_row -= gap_extend;
                b_size++;
            }
        }

        if (b_size <= N) {
            score_array[b_size].best = MININT;
            score_array[b_size].best_gap = MININT;
            b_size++;
        }
    }

    free (score_array);

    return best_score;

}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
