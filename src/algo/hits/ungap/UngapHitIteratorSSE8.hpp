/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.3, released November 2015                                     *
 *   Copyright (c) 2009-2015 Inria-Cnrs-Ens                                  *
 *                                                                           *
 *   PLAST is free software; you can redistribute it and/or modify it under  *
 *   the Affero GPL ver 3 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   Affero GPL ver 3 License for more details.                              *
 *****************************************************************************/

/** \file UngapHitIteratorSSE8.hpp
 *  \brief SIMD implementation of IHitIterator interface for ungap alignments.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _UNGAP_HIT_ITERATOR_SSE_HPP_
#define _UNGAP_HIT_ITERATOR_SSE_HPP_

/********************************************************************************/

#include <seed/api/ISeedModel.hpp>

#include <algo/core/api/IScoreMatrix.hpp>

#include <algo/hits/common/AbstractPipeHitIterator.hpp>

/********************************************************************************/
namespace algo     {
namespace hits     {
/** \brief Implementation of IHitIterator interface for ungap alignments. */
namespace ungapped {
/********************************************************************************/

/** \brief IHitIterator for ungap alignments with SIMD usage
 *
 * This implementation uses the Simple Instruction Multiple Data instructions set
 * for vectorizing several scores computations in a single call.
 *
 * The vectorization scheme allows here to compute 8 scores in the same time, so each
 * score can be computed on 128/8= 16 bits. Absolute values of maximal ungap scores
 * can't be greater than 2^16, so we don't fear any potential truncations in scores.
 *
 * \see UngapHitIteratorSSE16
 */
class UngapHitIteratorSSE8 : public algo::hits::common::AbstractPipeHitIterator
{
public:

    /** \copydoc common::AbstractPipeHitIterator::AbstractPipeHitIterator */
    UngapHitIteratorSSE8 (
        algo::hits::IHitIterator*               sourceIterator,
        seed::ISeedModel*                       model,
        algo::core::IScoreMatrix*               scoreMatrix,
        algo::core::IParameters*                parameters,
        alignment::core::IAlignmentContainer*   ungapResult
    );

    /** Destructor. */
    ~UngapHitIteratorSSE8 ();

    /** \copydoc common::AbstractPipeHitIterator::getName */
    const char* getName ()  { return "UngapHitIteratorSSE8"; }

protected:

    /** \copydoc common::AbstractPipeHitIterator::clone */
    virtual AbstractPipeHitIterator* clone (IHitIterator* sourceIterator)
    {
        return new UngapHitIteratorSSE8 (sourceIterator, _model, _scoreMatrix, _parameters, _ungapResult);
    }

    /** \copydoc common::AbstractPipeHitIterator::iterateMethod
     * This method does the actual filtering by computing the hit scores.
     */
    void iterateMethod  (Hit* hit);

    static const unsigned char NB = 8;
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _UNGAP_HIT_ITERATOR_SSE_HPP_ */
