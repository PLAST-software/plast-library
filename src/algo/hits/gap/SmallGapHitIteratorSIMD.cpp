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

#include <misc/api/Vector.hpp>
#include <misc/api/macros.hpp>

#include <algo/hits/gap/SmallGapHitIteratorSIMD.hpp>

#include <simd/api/vectorclass.h>

#include <designpattern/impl/SwitchTool.hpp>

using namespace std;
using namespace misc;
using namespace os;
using namespace os::impl;
using namespace database;
using namespace seed;
using namespace indexation;
using namespace algo::core;
using namespace alignment::core;
using namespace dp::impl;

#include <stdio.h>
#define DEBUG(a)   //printf a

// Define a macro for optimized score retrieval through the vector-matrix.
#define getScore(i,j)  (_matrixAsVector [(i)+((j)<<5)])
//#define getScore(i,j)  (_matrix [(i)][(j)])

/********************************************************************************/
namespace algo   {
namespace hits   {
namespace gapped {
/********************************************************************************/

#if 0
    typedef Vec16s VectorSIMD;
    const size_t VECTORSIZE = 16;
#else
    typedef Vec8s VectorSIMD;
    const size_t VECTORSIZE = 8;
#endif

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
SmallGapHitIteratorSIMD::SmallGapHitIteratorSIMD (
    IHitIterator*         realIterator,
    ISeedModel*           model,
    IScoreMatrix*         scoreMatrix,
    IParameters*          parameters,
    IAlignmentContainer*  ungapResult,
    IAlignmentContainer*  alignmentResult
)
    : AbstractPipeHitIterator (realIterator, model, scoreMatrix, parameters, ungapResult),
      _lowScoreNumber(0), _alignmentResult(0)
{
    setAlignmentResult (alignmentResult);

    DEBUG (("gap_open=%d  gap_extend=%d  scoremin=%d \n",
        _parameters->openGapCost, _parameters->extendGapCost, _parameters->smallGapThreshold
    ));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
SmallGapHitIteratorSIMD::~SmallGapHitIteratorSIMD ()
{
    setAlignmentResult (0);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
LETTER* SmallGapHitIteratorSIMD::getNeighbours1 (size_t n)
{
    if (n > _neighboursBuf1.size)  {  _neighboursBuf1.resize(n);  }

    memset (_neighboursBuf1.data,  database::CODE_X, n*sizeof(LETTER));

    return _neighboursBuf1.data;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
LETTER* SmallGapHitIteratorSIMD::getNeighbours2 (size_t n)
{
    if (n > _neighboursBuf2.size)  {  _neighboursBuf2.resize(n);  }

    memset (_neighboursBuf2.data,  database::CODE_X, n*sizeof(LETTER));

    return _neighboursBuf2.data;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
int32_t* SmallGapHitIteratorSIMD::getComputedScores (size_t n)
{
    if (n > _computedScores.size)  { _computedScores.resize(n);  }
    return _computedScores.data;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void SmallGapHitIteratorSIMD::iterateMethod  (Hit* hit)
{
    HIT_STATS_VERBOSE (_iterateMethodNbCalls++);

    /** Shortcuts. */
    const Vector<const ISeedOccurrence*>& occur1Vector = hit->occur1;
    const Vector<const ISeedOccurrence*>& occur2Vector = hit->occur2;

    /** Shortcuts. */
    size_t nbActualHits = hit->size ();
    size_t bandLength   = getNeighbourLength ();

    /** Statistics. */
    HIT_STATS (_inputHitsNumber += nbActualHits;)

    /** We need to extend the current neighbourhoods, so we create two arrays
     *  for holding these extended neighbourhoods. Note that, for a sequence,
     *  we store right and left neighbourhoods distinctly (which explain the
     *  factor 2 below for dimensioning vectors). */

    /** We choose n to be a multiple of 8. this will make the SIMD algorithm simpler. */
    size_t nb = 2*nbActualHits;
    nb = ( (size_t) ((nb-1)>>3) + 1) << 3;  /** ensure that nb % 8 == 0 */

    /** We need a vector holding the scores to be computed. */
    int32_t* computedScores = getComputedScores (nb);
    memset (computedScores, 0, nb*sizeof (int32_t));

    /** We retrieve two big buffers that will hold all neighbourhoods for subject and query. */
    size_t neighbourhoodsSize = nb*bandLength;
    LETTER* neighboursBuf1 = getNeighbours1 (neighbourhoodsSize);
    LETTER* neighboursBuf2 = getNeighbours2 (neighbourhoodsSize);

    /** We need two cursors for iterating the two allocated neighbourhoods buffers. */
    LETTER* cursor1 = neighboursBuf1;
    LETTER* cursor2 = neighboursBuf2;

    LETTER* right1 = 0;
    LETTER* left1  = 0;
    LETTER* right2 = 0;
    LETTER* left2  = 0;

    bool removable = false;

    /** We prepare some neighbourhoods data in specific memory chunk. */
    for (hit->first(); !hit->isDone(); hit->next (removable))
    {
        IdxCouple& idx = hit->currentItem();

        /** We get references on neighbourhoods buffers to be extended. */
        right1 = cursor1;   cursor1 += bandLength;
        left1  = cursor1;   cursor1 += bandLength;

        right2 = cursor2;   cursor2 += bandLength;
        left2  = cursor2;   cursor2 += bandLength;

        /** We compute right and left neighbourhoods for both sequences. */
        extendNeighbourhood (occur1Vector.data[idx.first],  right1, left1);
        extendNeighbourhood (occur2Vector.data[idx.second], right2, left2);
    }

    /** Now, 'neighbourhoods1' and 'neighbourhoods2' should hold all the wanted neighbourhoods. */
    computeScores (nb, neighboursBuf1, neighboursBuf2, computedScores);

    /** We check the scores versus the threshold. */
    size_t k=0;
    for (hit->first(); !hit->isDone(); hit->next (removable), k++)
    {
        /** We check whether we need to get rid of this occurrence or not. */
        removable = computedScores[k] < getGapThreshold();

        if (removable == false)
        {
            IdxCouple& idx = hit->currentItem();

            /** We check that the alignment is not already known. */
            removable = _alignmentResult->doesExist (
                occur1Vector.data[idx.first],
                occur2Vector.data[idx.second],
                bandLength
            );
        }
    }

    /** We update the statistics about iterations. */
    HIT_STATS (_outputHitsNumber += hit->size();)

    /** We are supposed to have computed scores for each hit,
     *  we can forward the information to the client.  */
    if (hit->empty() == false)      {  (_client->*_method) (hit);  }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void SmallGapHitIteratorSIMD::extendNeighbourhood (const ISeedOccurrence* occur, LETTER* right, LETTER* left)
{
    /** Shortcut. */
    size_t neighbourLength = getNeighbourLength ();

    /** Shortcuts. */
    database::LETTER* bufIn  = occur->sequence.data.letters.data + occur->offsetInSequence;

    /** We fill the right neighbour. */
    memcpy (right, bufIn, MIN (neighbourLength,  occur->sequence.data.letters.size - occur->offsetInSequence));

    /** We fill the left neighbour. */
    size_t imin = MIN (neighbourLength, occur->offsetInSequence);
    database::LETTER* bufOut = left;
    while (imin-- > 0)  {  *(bufOut++) = *(--bufIn);  }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void SmallGapHitIteratorSIMD::computeScores (
    size_t nb,
    const LETTER* neighbourhoods1,
    const LETTER* neighbourhoods2,
    int32_t* scores
)
{
    /** Shortcuts. */
    size_t neighbourLength = getNeighbourLength ();
    size_t neighbourWidth  = getNeighbourWidth  ();

    VectorSIMD score_gap_col;
    VectorSIMD score_gap_row;
    VectorSIMD next_score;
    VectorSIMD score;
    VectorSIMD best_score;
    VectorSIMD best_gap_arr[neighbourLength];
    VectorSIMD best_arr[neighbourLength];
    VectorSIMD temp;

    /** We load gap extension penalty to all elements of a constant. */
    VectorSIMD gap_extend (_parameters->extendGapCost);

    /** We load gap opening penalty to all elements of a constant. */
    VectorSIMD gap_open (_parameters->openGapCost);

    /** We load gap opening penalty to all elements of a constant. */
    VectorSIMD gap_open_extend (_parameters->openGapCost + _parameters->extendGapCost);

    /* Load minimal score to all elements of a constant */
    VectorSIMD score_min (-100);

    size_t first_b;
    size_t last_b;

    int16_t scoreTmp [VECTORSIZE];

    for (size_t k=0; k < nb ; k+=VECTORSIZE)
    {
        /** Shortcut. */
        size_t kNeighbourLength = k*neighbourLength;

        score           = -gap_open_extend;
        best_arr[0]     = 0;
        best_gap_arr[0] = -gap_open_extend;

        for (size_t i=1; i<neighbourWidth; i++)
        {
            best_arr[i]     = score;
            best_gap_arr[i] = score - gap_open_extend;
            score          -= gap_extend;
        }

        best_score = 0;
        first_b    = 0;
        last_b     = neighbourWidth / 2;

        for (size_t i=0; i<neighbourLength; i++)
        {
            /** Shortcut (and optimization). */
            const LETTER* pt_A = neighbourhoods1 + i + kNeighbourLength;

            score_gap_row = score_min;
            score         = score_min;

            for (size_t j=first_b; j<last_b; j++)
            {
                /** Shortcut (and optimization). */
                const LETTER* pt_B = neighbourhoods2 + j + kNeighbourLength;

                score_gap_col = best_gap_arr[j];

                const LETTER* cursor_A = pt_A;
                const LETTER* cursor_B = pt_B;

                for (size_t l=0; l<VECTORSIZE; l++)
                {
                    scoreTmp[l] = getScore (*cursor_A, *cursor_B);

                    cursor_A += neighbourLength;
                    cursor_B += neighbourLength;
                }

                next_score = best_arr[j] + temp.load_a (scoreTmp);

                score      = max (score, score_gap_col);
                score      = max (score, score_gap_row);
                best_score = max (score, best_score);

                score_gap_col -= gap_extend;
                score_gap_row -= gap_extend;

                best_gap_arr[j] = max (score - gap_open_extend, score_gap_col);
                score_gap_row   = max (score - gap_open_extend, score_gap_row);
                best_arr[j]     = score;
                score           = next_score;

            } /* end of for (size_t j=first_b; j<last_b; j++) */

            if (i >= neighbourWidth/2)   { first_b += 1; }

            if (last_b < neighbourLength)
            {
                best_arr    [last_b] = score_gap_row;
                best_gap_arr[last_b] = score_gap_row - gap_open_extend;

                last_b += 1;
            }

        }  /* end of for (i=0; i<neighbourLength; i++) */

        /** We sum right and left scores into a single global score. */
        for (size_t i=0; i<VECTORSIZE/2; i++)  {  *(scores++) = best_score.extract(2*i) + best_score.extract(2*i+1); }

    } /* end of for (k=0; k < nb; k+=VECTORSIZE) */
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
dp::IProperties* SmallGapHitIteratorSIMD::getProperties ()
{
    dp::IProperties* result = AbstractHitIterator::getProperties ();

    /** We have to aggregate values from different split instances. */
    u_int64_t lowScoreNumber = _lowScoreNumber;
    for (size_t i=0; i<_splitIterators.size(); i++)
    {
        SmallGapHitIteratorSIMD* current = dynamic_cast<SmallGapHitIteratorSIMD*> (_splitIterators[i]);
        if (current)
        {
            lowScoreNumber += current->_lowScoreNumber;
        }
    }

    result->add (1, "details");
    result->add (2, "score_ko", "%lld", lowScoreNumber);
    result->add (2, "score_ok", "%lld", getOutputHitsNumber());

    return result;
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
