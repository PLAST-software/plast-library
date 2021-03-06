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

/** \file UngapHitIterator.hpp
 *  \brief Implementation of IHitIterator interface for ungap alignments.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _UNGAP_HIT_ITERATOR_HPP_
#define _UNGAP_HIT_ITERATOR_HPP_

/********************************************************************************/

#include <seed/api/ISeedModel.hpp>

#include <algo/core/api/IScoreMatrix.hpp>
#include <algo/core/api/IAlgoParameters.hpp>

#include <algo/hits/common/AbstractPipeHitIterator.hpp>

/********************************************************************************/
namespace algo     {
namespace hits     {
/** \brief Implementation of IHitIterator interface for ungap alignments. */
namespace ungapped {
/********************************************************************************/

/** \brief IHitIterator for ungap alignments
 *
 * This implementation will take a source IHitIterator instance and computes ungap
 * score for each match.
 *
 * Matches whose score is below some threshold value are filtered out and won't be
 * iterated in linked IHitIterator (ie. further steps of the algorithm).
 *
 * This implementation follows a textbook approach for computing the score. Quicker
 * implementations may use SIMD instructions for vectorizing several scores computations.
 *
 * \see UngapHitIteratorSSE8
 * \see UngapHitIteratorSSE16
 */
class UngapHitIterator : public algo::hits::common::AbstractPipeHitIterator
{
public:

    /** \copydoc common::AbstractPipeHitIterator::AbstractPipeHitIterator */
    UngapHitIterator (
        algo::hits::IHitIterator*               sourceIterator,
        seed::ISeedModel*                       model,
        algo::core::IScoreMatrix*               scoreMatrix,
        algo::core::IParameters*                parameters,
        alignment::core::IAlignmentContainer*   ungapResult
    );

    /** Destructor. */
    virtual ~UngapHitIterator ();

    /** \copydoc common::AbstractPipeHitIterator::getName */
    const char* getName ()  { return "UngapHitIterator"; }

protected:

    /** \copydoc common::AbstractPipeHitIterator::clone */
    virtual AbstractPipeHitIterator* clone (IHitIterator* sourceIterator)
    {
        return new UngapHitIterator (sourceIterator, _model, _scoreMatrix, _parameters, _ungapResult);
    }

    /** \copydoc common::AbstractPipeHitIterator::iterateMethod
     * This method does the actual filtering by computing the hit scores.
     */
    void iterateMethod  (Hit* hit);
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _UNGAP_HIT_ITERATOR_HPP_ */
