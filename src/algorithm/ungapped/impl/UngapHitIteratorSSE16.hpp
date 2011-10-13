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

#ifndef _UNGAP_HIT_ITERATOR_SSE_16_HPP_
#define _UNGAP_HIT_ITERATOR_SSE_16_HPP_

/********************************************************************************/

#include "UngapHitIterator.hpp"
#include "ISeedModel.hpp"
#include "IScoreMatrix.hpp"

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/** Implemented as a Design Pattern Proxy (proxy on the provided HitIterator)
 */
class UngapHitIteratorSSE16 : public UngapHitIterator
{
public:
    UngapHitIteratorSSE16 (
        indexation::IHitIterator*   realIterator,
        seed::ISeedModel*           model,
        algo::IScoreMatrix*         scoreMatrix,
        algo::IParameters*          parameters,
        IAlignmentResult*           ungapResult,
        u_int32_t                   maxHitsPerIteration
    );

    ~UngapHitIteratorSSE16 ();

    /** */
    const char* getName ()  { return "UngapHitIteratorSSE16"; }

    /** */
    dp::IProperties* getProperties ();

protected:

    /** We need a clone method. */
    virtual UngapHitIterator* clone (indexation::IHitIterator* realIterator)
    {
        return new UngapHitIteratorSSE16 (realIterator, _model, _scoreMatrix, _parameters, _ungapResult, _maxHitsPerIteration);
    }

    /** */
    void iterateMethod  (indexation::Hit* hit);

    static const unsigned char NB = 16;

    /** Statistics. */
    u_int64_t _ungapKnownNumber;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _UNGAP_HIT_ITERATOR_SSE_16_HPP_ */
