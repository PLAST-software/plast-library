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

#if 0
#ifndef _UNGAP_HIT_ITERATOR_OPTIM_HPP_
#define _UNGAP_HIT_ITERATOR_OPTIM_HPP_

/********************************************************************************/

#include "UngapHitIterator.hpp"
#include "ISeedModel.hpp"
#include "IScoreMatrix.hpp"

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/** Implemented as a Design Pattern Proxy (proxy on the provided HitIterator)
 */
class UngapHitIteratorOptim : public UngapHitIterator
{
public:
    UngapHitIteratorOptim (
        indexation::IHitIterator*   realIterator,
        seed::ISeedModel*           model,
        algo::IScoreMatrix*         scoreMatrix,
        u_int16_t                   neighbourLength,
        u_int16_t                   threshold
    );

protected:

    /** We need a clone method. */
    virtual UngapHitIterator* clone (indexation::IHitIterator* realIterator)
    {
        return new UngapHitIteratorOptim (realIterator, _model, _scoreMatrix, _neighbourLength, _threshold);
    }

    /** */
    void iterateMethod  (const indexation::Hit* hit);

    typedef u_int32_t  MatrixEntry;

    MatrixEntry _matrix2AsVector[20][20][20][20];
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _UNGAP_HIT_ITERATOR_OPTIM_HPP_ */
#endif
