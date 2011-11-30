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

#include <algo/hits/ungap/UngapHitIterator.hpp>

using namespace std;
using namespace misc;
using namespace os;
using namespace os::impl;
using namespace database;
using namespace seed;
using namespace indexation;
using namespace algo::core;
using namespace algo::align;

#include <stdio.h>
#define DEBUG(a)  //printf a

// Define a macro for optimized score retrieval through the vector-matrix.
#define getScore(i,j)  (_matrixAsVector [(i)+((j)<<5)])

/********************************************************************************/
namespace algo     {
namespace hits     {
namespace ungapped {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
UngapHitIterator::UngapHitIterator (
    IHitIterator*       realIterator,
    ISeedModel*         model,
    IScoreMatrix*       scoreMatrix,
    IParameters*        parameters,
    IAlignmentResult*   ungapResult

)
    : AbstractPipeHitIterator (realIterator, model, scoreMatrix, parameters, ungapResult)
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
UngapHitIterator::~UngapHitIterator ()
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
void UngapHitIterator::iterateMethod (Hit* hit)
{

    LETTER* loop1 = 0;
    LETTER* loop2 = 0;

    int8_t score    = 0;
    int8_t scoreMax = 0;

    /** Shortcuts. */
    const Vector<const ISeedOccurrence*>& occur1Vector = hit->occur1;
    const Vector<const ISeedOccurrence*>& occur2Vector = hit->occur2;

    size_t neighbourLength = _parameters->ungapNeighbourLength;

    size_t nb1 = occur1Vector.size;
    size_t nb2 = occur2Vector.size;

    for (size_t i=0; i<nb1; i++)
    {
        for (size_t j=0; j<nb2; j++)
        {
            loop1 = occur1Vector.data[i]->neighbourhood.letters.data;
            loop2 = occur2Vector.data[j]->neighbourhood.letters.data;

            score    = 0;
            scoreMax = 0;
            for (size_t k=0; k<_span+neighbourLength; k++)
            {
                score += getScore (*(loop1++), *(loop2++));
                if (score > scoreMax)  { scoreMax = score; }
            }

            score = scoreMax;
            for (size_t k=0; k<neighbourLength; k++)
            {
                score += getScore (*(loop1++), *(loop2++));
                if (score > scoreMax)  { scoreMax = score; }
            }

            /** We may call the callback if we got a decent score. */
            if (scoreMax >= _parameters->ungapScoreThreshold)
            {
                /** We increase the number of iterations. */
                _outputHitsNumber ++;

                hit->addIndexes (i,j);
            }

        }  /* end of for (j... */

    }  /* end of for (i... */

    /** We are supposed to have computed scores for each hit,
     *  we can forward the information to the client.  */
    (_client->*_method) (hit);
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
