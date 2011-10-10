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

#include "SmallGapHitIterator.hpp"
#include "macros.hpp"

using namespace std;
using namespace os;
using namespace database;
using namespace seed;
using namespace indexation;

#include <stdio.h>
#define DEBUG(a)  //printf a

// Define a macro for optimized score retrieval through the vector-matrix.
#define getScore(i,j)  (_matrixAsVector [(i)+((j)<<5)])

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
SmallGapHitIterator::SmallGapHitIterator (
    indexation::IHitIterator* realIterator,
    seed::ISeedModel*         model,
    algo::IScoreMatrix*       scoreMatrix,
    algo::IParameters*        parameters,
    algo::IAlignmentResult*   ungapResult
)
    : AbstractScoredHitIterator (realIterator, model, scoreMatrix, parameters, ungapResult)
{
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
SmallGapHitIterator::~SmallGapHitIterator ()
{
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void SmallGapHitIterator::iterateMethod  (indexation::Hit* hit)
{
    DEBUG (("SmallGapHitIterator::iterateMethod \n"));

    /** Shortcuts. */
    const Vector<const ISeedOccurrence*>& occur1Vector = hit->occur1;
    const Vector<const ISeedOccurrence*>& occur2Vector = hit->occur2;

    /** Shortcuts. */
    size_t nb1 = occur1Vector.size;
    size_t nb2 = occur2Vector.size;

    /** We need to extend the current neighbourhoods, so we create two arrays
     *  for holding these extended neighbourhoods. */
    std::vector<IWord> neighbourhood1 (nb1);
    std::vector<IWord> neighbourhood2 (nb2);

    std::vector<bool> isExtended1 (nb1);   for (size_t k=0; k<nb1; k++)  { isExtended1[k] = false; }
    std::vector<bool> isExtended2 (nb2);   for (size_t k=0; k<nb2; k++)  { isExtended2[k] = false; }

    for (list<IdxCouple>::iterator it = hit->indexes.begin();  it != hit->indexes.end();  )
    {
        /** Shortcut. */
        IdxCouple& idx = *it;

        IWord& neighbour1 = neighbourhood1[idx.first];
        IWord& neighbour2 = neighbourhood2[idx.second];

        if (isExtended1[idx.first] == false)
        {
            extendNeighbourhood (occur1Vector.data[idx.first], neighbour1);
            isExtended1[idx.first] = true;
        }

        if (isExtended2[idx.second] == false)
        {
            extendNeighbourhood (occur2Vector.data[idx.second], neighbour2);
            isExtended2[idx.second] = true;
        }

        int16_t scoreRight = computeScore (
                neighbour1.letters.data,
                neighbour2.letters.data
        );

        int16_t scoreLeft  = computeScore (
                neighbour1.letters.data  + _parameters->smallGapBandLength,
                neighbour2.letters.data  + _parameters->smallGapBandLength
        );

        if (scoreRight + scoreLeft >= _parameters->smallGapThreshold)
        {
            /** We increase the number of iterations. */
            _outputHitsNumber ++;

            /** We just continue the iteration. */
            it++;
        }
        else
        {
            /** We remove the current index couple. */
            it = hit->indexes.erase (it);
        }
    }

    /** We are supposed to have computed scores for each hit,
     *  we can forward the information to the client.  */
    (_client->*_method) (hit);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
int16_t SmallGapHitIterator::computeScore (const database::LETTER* a, const database::LETTER* b)
{
    int score_gap_col;
    int score_gap_row;
    int next_score;
    int best_gap_arr [_parameters->smallGapBandLength];
    int best_arr     [_parameters->smallGapBandLength];

    const int score_min = -100;

    int gap_open_extend = _parameters->extendGapCost + _parameters->openGapCost;

    int score       = -gap_open_extend;
    best_arr    [0] = 0;
    best_gap_arr[0] = -gap_open_extend;

    for (int i=1; i<_parameters->smallGapBandWidth; i++)
    {
        best_arr[i]     = score;
        best_gap_arr[i] = score - _parameters->openGapCost;
        score          -= _parameters->extendGapCost;
    }

    int    best_score = 0;
    size_t first_b    = 0;
    size_t last_b     = _parameters->smallGapBandWidth / 2;

    for (int i=0; i<_parameters->smallGapBandLength; i++)
    {
        score_gap_row = score_min;
        score         = score_min;

        for (size_t j=first_b; j<last_b; j++)
        {
            score_gap_col = best_gap_arr[j];

            //next_score = best_arr[j] + getScore (a[i], b[j]);
            next_score = best_arr[j] + _matrix [(int)a[i]] [(int)b[j]];

            if (score < score_gap_col)  score = score_gap_col;
            if (score < score_gap_row)  score = score_gap_row;

            if (best_score < score)   {  best_score = score; }

            score_gap_col -= _parameters->extendGapCost;
            score_gap_row -= _parameters->extendGapCost;

            best_gap_arr[j] = MAX (score - gap_open_extend, score_gap_col);
            score_gap_row   = MAX (score - gap_open_extend, score_gap_row);
            best_arr[j]     = score;
            score           = next_score;
        }

        if (i >= _parameters->smallGapBandWidth/2)      first_b += 1;

        if (last_b < _parameters->smallGapBandLength)
        {
            best_arr[last_b]     = score_gap_row;
            best_gap_arr[last_b] = score_gap_row - gap_open_extend;
            last_b += 1;
        }
    }

    /** We return the result. */
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
void SmallGapHitIterator::extendNeighbourhood (const indexation::ISeedOccurrence* occur, database::IWord& neighbour)
{
    /** Shortcut. */
    size_t neighbourLength = _parameters->smallGapBandLength;

    /** We resize the neighbourhood. */
    neighbour.letters.resize (2*neighbourLength);

    DEBUG (("SmallGapHitIterator::extendNeighbourhood: size after 'resize' is %d\n",
        neighbour.letters.size
    ));

    /** Shortcuts. */
    database::LETTER* bufIn  = occur->sequence.data.letters.data + occur->offsetInSequence;
    database::LETTER* bufOut = neighbour.letters.data;

    /** We fill the word with default letter. */
    memset (bufOut, database::CODE_X, neighbour.letters.size);

    /** We fill the right neighbour. */
    memcpy (bufOut, bufIn, MIN (neighbourLength,  occur->sequence.data.letters.size - occur->offsetInSequence));

    /** We fill the left neighbour. */
    bufOut += neighbourLength;
    size_t imin = MIN (neighbourLength, occur->offsetInSequence);

    while (imin-- > 0)
    {
        *(bufOut++) = *(--bufIn);
    }
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
