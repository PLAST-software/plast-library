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

#ifndef _UNGAP_HIT_ITERATOR_SSE_HPP_
#define _UNGAP_HIT_ITERATOR_SSE_HPP_

/********************************************************************************/

#include "UngapHitIterator.hpp"
#include "ISeedModel.hpp"
#include "IScoreMatrix.hpp"

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/** Implemented as a Design Pattern Proxy (proxy on the provided HitIterator)
 */
class UngapHitIteratorSSE8 : public UngapHitIterator
{
public:
    UngapHitIteratorSSE8 (
        indexation::IHitIterator*   realIterator,
        seed::ISeedModel*           model,
        algo::IScoreMatrix*         scoreMatrix,
        algo::IParameters*          parameters,
        algo::IAlignmentResult*     ungapResult
    );

    ~UngapHitIteratorSSE8 ();

    /** */
    const char* getName ()  { return "UngapHitIteratorSSE8"; }

protected:

    /** We need a clone method. */
    virtual UngapHitIterator* clone (indexation::IHitIterator* realIterator)
    {
        return new UngapHitIteratorSSE8 (realIterator, _model, _scoreMatrix, _parameters, _ungapResult);
    }

    /** */
    void iterateMethod  (indexation::Hit* hit);

    static const unsigned char NB = 8;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _UNGAP_HIT_ITERATOR_SSE_HPP_ */
