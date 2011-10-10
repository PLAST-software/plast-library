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

#include "FullGapHitIterator.hpp"
#include "IStatistics.hpp"
#include "BasicAlignmentResult.hpp"
#include "UngapAlignmentResult.hpp"
#include "AlignmentSplitter.hpp"
#include "macros.hpp"

#include <math.h>

using namespace std;
using namespace os;
using namespace dp;
using namespace database;
using namespace seed;
using namespace indexation;
using namespace statistics;

#include <stdio.h>
#define DEBUG(a)  //printf a

/** Maximal subject length after which the offsets are adjusted to a subsequence. */
#define MAX_SUBJECT_OFFSET 90000

/** Approximate upper bound on a number of gaps in an HSP, needed to determine
 * the length of the subject subsequence to be retrieved for alignment with traceback. */
#define MAX_TOTAL_GAPS 3000

// Define a macro for optimized score retrieval through the vector-matrix.
#define getScore(i,j)  (_matrixAsVector [(i)+((j)<<5)])

#ifndef INT4_MIN
    /** Smallest (most negative) number represented by signed int */
    #define INT4_MIN    (-2147483647-1)
#endif

/** Lower bound for scores. Divide by two to prevent underflows. */
#define MININT INT4_MIN/2

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
FullGapHitIterator::FullGapHitIterator (
    indexation::IHitIterator*       realIterator,
    seed::ISeedModel*               model,
    algo::IScoreMatrix*             scoreMatrix,
    algo::IParameters*              parameters,
    statistics::IQueryInformation*  queryInfo,
    statistics::IGlobalParameters*  globalStats,
    algo::IAlignmentResult*         ungapResult,
    algo::IAlignmentResult*         alignmentResult
)
    : AbstractScoredHitIterator (realIterator, model, scoreMatrix, parameters, ungapResult),
      _queryInfo(0), _globalStats(0), _alignmentResult(0), _splitter(0),
      _ungapKnownNumber(0), _gapKnownNumber(0)
{
    setQueryInfo        (queryInfo);
    setGlobalStats      (globalStats);
    setAlignmentResult  (alignmentResult);

    setAlignmentSplitter (new AlignmentSplitter (scoreMatrix, parameters->openGapCost, parameters->extendGapCost) );

    DEBUG  (("xdrop=%d  finalxdrop=%d \n", _parameters->XdroppofGap, _parameters->finalXdroppofGap));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
FullGapHitIterator::~FullGapHitIterator ()
{
    setQueryInfo         (0);
    setGlobalStats       (0);
    setAlignmentResult   (0);
    setAlignmentSplitter (0);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void FullGapHitIterator::iterate (void* client, Method method)
{
    /** We call the parent method. */
    AbstractScoredHitIterator::iterate (client, method);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void FullGapHitIterator::iterateMethod  (indexation::Hit* hit)
{
    /** Shortcuts. */
    const Vector<const ISeedOccurrence*>& occur1Vector = hit->occur1;
    const Vector<const ISeedOccurrence*>& occur2Vector = hit->occur2;

    int score1=0,  score2=0;
    int leftOffsetInQuery=0, leftOffsetInSubject=0, rightOffsetInQuery=0, rightOffsetInSubject=0;
    bool shouldKeep = false;

    /** Statistics. */
    HIT_STATS (_inputHitsNumber += hit->indexes.size();)

    for (list<IdxCouple>::iterator it = hit->indexes.begin();  it != hit->indexes.end();  )
    {
        /** Shortcut. */
        IdxCouple& idx = *it;

        /** Shortcuts. */
        const ISeedOccurrence* occur1 = occur1Vector.data [idx.first];
        const ISeedOccurrence* occur2 = occur2Vector.data [idx.second];

#if 1
        /** We check that the hit we are going to process is not already known. */
        if (_ungapResult && _ungapResult->doesExist (occur1, occur2) == true)
        {
            HIT_STATS (_ungapKnownNumber ++;)

            /** We remove the current index couple. */
            it = hit->indexes.erase(it);

            continue;
        }
#endif

        /** Shortcuts. */
        const ISequence& seq1 = occur1->sequence;
        const ISequence& seq2 = occur2->sequence;

        /** Shortcuts. */
        LETTER* seqData1 = seq1.data.letters.data;
        LETTER* seqData2 = seq2.data.letters.data;

        /** By default, we don't want to keep this current hit. */
        shouldKeep = false;

        /** We compute the left part of the score. */
        score1 = SemiGappedAlign (
            seqData2,
            seqData1,
            occur2->offsetInSequence + 1,
            occur1->offsetInSequence + 1,
            & leftOffsetInQuery,
            & leftOffsetInSubject,
            1
        );

        /** We compute the right part of the score. */
        score2 = SemiGappedAlign (
            seqData2 + occur2->offsetInSequence,
            seqData1 + occur1->offsetInSequence,
            seq2.data.letters.size - occur2->offsetInSequence - 1,
            seq1.data.letters.size - occur1->offsetInSequence - 1,
            & rightOffsetInQuery,
            & rightOffsetInSubject,
            0
        );

        int score = score1 + score2;

        /** We retrieve statistical information for the current query sequence. */
        IQueryInformation::SequenceInfo& info = _queryInfo->getSeqInfoByIndex (seq2.index);

#if 0
        DEBUG (("cutoff=%d   score=%d \n", info.cut_offs, score));
        DEBUG (("   query %ld  : offsetInSeq=%ld  leftOffset=%d  rightOffset=%d\n",
            occur2->sequence.index, occur2->offsetInSequence, leftOffsetInQuery, rightOffsetInQuery
        ));
        DEBUG (("   subject %ld: offsetInSeq=%ld  leftOffset=%d  rightOffset=%d\n",
            occur1->sequence.index, occur1->offsetInSequence, leftOffsetInSubject, rightOffsetInSubject
        ));
#endif

        if (score >= info.cut_offs)
        {
            Alignment align (
                occur1,
                occur2,
                leftOffsetInQuery   - 1,
                leftOffsetInSubject - 1,
                rightOffsetInQuery,
                rightOffsetInSubject,
                score
            );

            /** We add this alignment as ungap alignments (ie the gapped alignment will be split in ungap ones). */
            if (_ungapResult)  { _ungapResult->insert (align, _splitter);  }

            shouldKeep = _alignmentResult->doesExist (align) == false;
        }

        if (shouldKeep == true)
        {
            /** We increase the number of found decent hits. */
            HIT_STATS (_outputHitsNumber ++;)

            /** We just go to the next item. */
            it++;
        }
        else
        {
            /** Statistics information. */
            HIT_STATS (_gapKnownNumber++;)

            /** We remove the current index couple. */
            it = hit->indexes.erase(it);
        }

    } /* end of for (IdxCouple* it = hit->indexes... */

    /** We are supposed to have computed scores for each hit,
     *  we can forward the information to the client.  */
    (_client->*_method) (hit);
}

/*********************************************************************
** METHOD  :
** PURPOSE : Computing full gapped alignment
** INPUT   :
**      @param A         The query sequence [in]
**      @param B         The subject sequence [in]
**      @param M         Maximal extension length in query [in]
**      @param N         Maximal extension length in subject [in]
**      @param a_offset  Resulting starting offset in query [out]
**      @param b_offset  Resulting starting offset in subject [out]
** OUTPUT  :
** RETURN  :
** REMARKS : ref: NCBI-BLAST
*********************************************************************/
int FullGapHitIterator::SemiGappedAlign (
    char* A,
    char* B,
    int M,
    int N,
    int* a_offset,
    int* b_offset,
    int reverse_sequence
)
{
    int i;                     /* sequence pointers and indices */
    int a_index;
    int b_index, b_size, first_b_index, last_b_index, b_increment;
    const char* b_ptr;

    BlastGapDP* score_array;

    int gap_open;              /* alignment penalty variables */
    int gap_extend;
    int gap_open_extend;
    int x_dropoff;

    int8_t** matrix = NULL;       /* pointers to the score matrix */
    int8_t* matrix_row = NULL;

    int score;                 /* score tracking variables */
    int score_gap_row;
    int score_gap_col;
    int next_score;
    int best_score;
    int num_extra_cells;

    int dp_mem_alloc = 1000;

    /* do initialization and sanity-checking */

    matrix = _scoreMatrix->getMatrix();

    *a_offset = 0;
    *b_offset = 0;

    gap_open        = _parameters->openGapCost;
    gap_extend      = _parameters->extendGapCost;
    gap_open_extend = gap_open + gap_extend;

    x_dropoff = _parameters->XdroppofGap;

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

    score_array = (BlastGapDP *) malloc (dp_mem_alloc * sizeof(BlastGapDP));;
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

        for (b_index = first_b_index; b_index < b_size; b_index++)
        {
#if 0
            /** We add some guard conditions in order no to read in unwanted memory.
             *  (otherwise valgrind won't be happy)  */
            if (reverse_sequence) {  if (b_ptr<=B)   { break;  } }
            else                  {  if (b_ptr>=B+N) { break;  } }
#endif
            b_ptr        += b_increment;
            score_gap_col = score_array[b_index].best_gap;
            next_score    = score_array[b_index].best + matrix_row[ (int)*b_ptr ];

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

        if (last_b_index < b_size - 1)
        {
            /* This row failed the X-dropoff test earlier than
               the last row did; just shorten the loop bounds
               before doing the next row */

            b_size = last_b_index + 1;
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
        }

        if (b_size <= N)
        {
            score_array[b_size].best     = MININT;
            score_array[b_size].best_gap = MININT;
            b_size++;
        }
    }

    free (score_array);

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
dp::IProperties* FullGapHitIterator::getProperties ()
{
    dp::IProperties* result = AbstractHitIterator::getProperties ();

    /** We have to aggregate values from different split instances. */
    u_int64_t ungapKnownNumber = _ungapKnownNumber;
    u_int64_t gapKnownNumber   = _gapKnownNumber;
    for (size_t i=0; i<_splitIterators.size(); i++)
    {
        FullGapHitIterator* current = dynamic_cast<FullGapHitIterator*> (_splitIterators[i]);
        if (current)
        {
            ungapKnownNumber += current->_ungapKnownNumber;
            gapKnownNumber   += current->_gapKnownNumber;
        }
    }

    result->add (1, "details");
    result->add (2, "known_ungap", "%ld",  ungapKnownNumber);
    result->add (2, "known_gap",   "%ld",  gapKnownNumber);
    result->add (2, "computed",    "%ld",  getInputHitsNumber() - ungapKnownNumber);

    /** We call the parent method in case we have split instances. */
    return result;

}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
