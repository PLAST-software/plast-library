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

#ifndef _SMALLGAP_HIT_ITERATOR_HPP_
#define _SMALLGAP_HIT_ITERATOR_HPP_

/********************************************************************************/

#include "ISeedModel.hpp"
#include "IScoreMatrix.hpp"
#include "IAlgoParameters.hpp"
#include "AbstractScoredHitIterator.hpp"

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/** Implemented as a Design Pattern Proxy (proxy on the provided HitIterator)
 */
class SmallGapHitIterator : public algo::AbstractScoredHitIterator
{
public:
    SmallGapHitIterator (
        indexation::IHitIterator*   realIterator,
        seed::ISeedModel*           model,
        algo::IScoreMatrix*         scoreMatrix,
        algo::IParameters*          parameters,
        algo::IAlignmentResult*     ungapResult
    );
    virtual ~SmallGapHitIterator ();

    /** */
    const char* getName ()  { return "SmallGapHitIterator"; }

protected:

    /** We need a clone method. */
    virtual AbstractScoredHitIterator* clone (indexation::IHitIterator* realIterator)
    {
        return new SmallGapHitIterator (realIterator, _model, _scoreMatrix, _parameters, _ungapResult);
    }

    /** */
    void iterateMethod (indexation::Hit* hit);

    /** */
    void extendNeighbourhood (const indexation::ISeedOccurrence* occur, database::IWord& neighbour);

    int16_t computeScore (const database::LETTER* a, const database::LETTER* b);
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _SMALLGAP_HIT_ITERATOR_HPP_ */
