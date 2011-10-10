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


#ifndef _ABSTRACT_SCORED_HITITERATOR_HPP_
#define _ABSTRACT_SCORED_HITITERATOR_HPP_

/********************************************************************************/

#include "AbstractHitIterator.hpp"

#include "ISeedModel.hpp"
#include "IScoreMatrix.hpp"
#include "IAlgoParameters.hpp"
#include "IAlignementResult.hpp"

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/** Iterates over hits.
 */
class AbstractScoredHitIterator : public indexation::AbstractHitIterator
{
public:

    AbstractScoredHitIterator (
        indexation::IHitIterator*   realIterator,
        seed::ISeedModel*           model,
        algo::IScoreMatrix*         scoreMatrix,
        algo::IParameters*          parameters,
        algo::IAlignmentResult*     ungapResult
    );

    virtual ~AbstractScoredHitIterator ();

    void first() {}
    dp::IteratorStatus next() { return dp::ITER_UNKNOWN; }
    bool isDone() { return true; }
    indexation::Hit* currentItem() { return 0; }

    std::vector<indexation::IHitIterator*> split (size_t nbSplit);

    void iterate (void* aClient, Method method);

    /** */
    IHitIterator* getSourceIterator ()  { return _realIterator; }

protected:

    indexation::IHitIterator* _realIterator;
    seed::ISeedModel*         _model;
    algo::IScoreMatrix*       _scoreMatrix;
    algo::IParameters*        _parameters;
    algo::IAlignmentResult*   _ungapResult;

    /** Shortcuts. */
    int8_t** _matrix;
    int8_t*  _matrixAsVector;
    size_t   _span;

    IteratorClient*  _client;
    Method           _method;

    /** Primitive of template method 'iterate'. */
    virtual void iterateMethod  (indexation::Hit* hit) = 0;

    /** Primitive of template method 'split'. */
    virtual AbstractScoredHitIterator* clone (indexation::IHitIterator* realIterator) = 0;

    /** Statistics. */
    u_int64_t _scoreOK;
    u_int64_t _scoreKO;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ABSTRACT_SCORED_HITITERATOR_HPP_ */
