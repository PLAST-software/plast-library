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

/** \file UngapHitIteratorSSE16.hpp
 *  \brief SIMD implementation of IHitIterator interface for ungap alignments.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _UNGAP_HIT_ITERATOR_SSE_16_HPP_
#define _UNGAP_HIT_ITERATOR_SSE_16_HPP_

/********************************************************************************/

#include <seed/api/ISeedModel.hpp>

#include <algo/core/api/IScoreMatrix.hpp>

#include <algo/hits/ungap/UngapHitIterator.hpp>

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
 * The vectorization scheme allows here to compute 16 scores in the same time, so each
 * score can be computed on 128/16= 8 bits. Absolute values of maximal ungap scores
 * can be greater than 2^8, so we may have some potential truncations in scores.
 *
 * \see UngapHitIteratorSSE8
 */
class UngapHitIteratorSSE16 : public algo::hits::common::AbstractPipeHitIterator
{
public:

    /** \copydoc common::AbstractPipeHitIterator::AbstractPipeHitIterator
     * \param[in] maxHitsPerIteration : maximum hits to be processed per seed iteration.
     */
    UngapHitIteratorSSE16 (
        algo::hits::IHitIterator*       sourceIterator,
        seed::ISeedModel*               model,
        algo::core::IScoreMatrix*       scoreMatrix,
        algo::core::IParameters*        parameters,
        algo::align::IAlignmentResult* ungapResult,
        u_int32_t                       maxHitsPerIteration
    );

    /** Destructor. */
    ~UngapHitIteratorSSE16 ();

    /** \copydoc common::AbstractPipeHitIterator::getName */
    const char* getName ()  { return "UngapHitIteratorSSE16"; }

    /** \copydoc common::AbstractPipeHitIterator::getProperties */
    dp::IProperties* getProperties ();

protected:

    /** \copydoc common::AbstractPipeHitIterator::clone */
    virtual common::AbstractPipeHitIterator* clone (IHitIterator* sourceIterator)
    {
        return new UngapHitIteratorSSE16 (sourceIterator, _model, _scoreMatrix, _parameters, _ungapResult, _maxHitsPerIteration);
    }

    /** \copydoc common::AbstractPipeHitIterator::iterateMethod */
    void iterateMethod  (Hit* hit);

    static const unsigned char NB = 16;

    /* Statistics. */
    u_int64_t _ungapKnownNumber;

    /** Inner buffer. */
    char* _databk;
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _UNGAP_HIT_ITERATOR_SSE_16_HPP_ */
