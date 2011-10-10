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

#ifndef _SMALLGAP_HIT_ITERATOR_SSE8_HPP_
#define _SMALLGAP_HIT_ITERATOR_SSE8_HPP_

/********************************************************************************/

#include "ISeedModel.hpp"
#include "IScoreMatrix.hpp"
#include "AbstractScoredHitIterator.hpp"

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/** Implemented as a Design Pattern Proxy (proxy on the provided HitIterator)
 */
class SmallGapHitIteratorSSE8 : public algo::AbstractScoredHitIterator
{
public:
    SmallGapHitIteratorSSE8 (
        indexation::IHitIterator*   realIterator,
        seed::ISeedModel*           model,
        algo::IScoreMatrix*         scoreMatrix,
        algo::IParameters*          parameters,
        algo::IAlignmentResult*     ungapResult
    );
    virtual ~SmallGapHitIteratorSSE8 ();

    /** */
    const char* getName ()  { return "SmallGapHitIteratorSSE8"; }

    /** */
    dp::IProperties* getProperties ();

protected:

    os::Vector<int> _computedScores;
    int* getComputedScores (size_t n);

    os::Vector<database::LETTER> _neighboursBuf1;
    database::LETTER* getNeighbours1 (size_t n);

    os::Vector<database::LETTER> _neighboursBuf2;
    database::LETTER* getNeighbours2 (size_t n);

    /** We need a clone method. */
    virtual AbstractScoredHitIterator* clone (indexation::IHitIterator* realIterator)
    {
        return new SmallGapHitIteratorSSE8 (realIterator, _model, _scoreMatrix, _parameters, _ungapResult);
    }

    /** */
    void iterateMethod (indexation::Hit* hit);

    /** */
    void extendNeighbourhood (
        const indexation::ISeedOccurrence* occur,
        database::LETTER* right,
        database::LETTER* left
    );

    /** */
    void computeScores (
        size_t nb,
        const database::LETTER* neighbourhoods1,
        const database::LETTER* neighbourhoods2,
        int* scores
    );

    /** Statistics. */
    u_int64_t _lowScoreNumber;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _SMALLGAP_HIT_ITERATOR_SSE8_HPP_ */
