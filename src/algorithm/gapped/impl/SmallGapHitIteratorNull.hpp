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

#ifndef _SMALLGAP_HIT_ITERATOR_NULL_HPP_
#define _SMALLGAP_HIT_ITERATOR_NULL_HPP_

/********************************************************************************/

#include "ISeedModel.hpp"
#include "IScoreMatrix.hpp"
#include "AbstractScoredHitIterator.hpp"

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/**
 */
class SmallGapHitIteratorNull : public algo::AbstractScoredHitIterator
{
public:
    SmallGapHitIteratorNull (
        indexation::IHitIterator*   realIterator,
        seed::ISeedModel*           model,
        algo::IScoreMatrix*         scoreMatrix,
        algo::IParameters*          parameters,
        algo::IAlignmentResult*     ungapResult
    );
    virtual ~SmallGapHitIteratorNull ();

    /** */
    const char* getName ()  { return "SmallGapHitIteratorNull"; }

protected:

    /** We need a clone method. */
    virtual AbstractScoredHitIterator* clone (indexation::IHitIterator* realIterator)
    {
        return new SmallGapHitIteratorNull (realIterator, _model, _scoreMatrix, _parameters, _ungapResult);
    }

    /** */
    void iterateMethod (indexation::Hit* hit) {}
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _SMALLGAP_HIT_ITERATOR_NULL_HPP_ */
