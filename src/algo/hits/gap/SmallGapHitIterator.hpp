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

/** \file SmallGapHitIterator.hpp
 *  \brief Implementation of IHitIterator interface for small gap alignments.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _SMALLGAP_HIT_ITERATOR_HPP_
#define _SMALLGAP_HIT_ITERATOR_HPP_

/********************************************************************************/

#include <seed/api/ISeedModel.hpp>

#include <algo/core/api/IScoreMatrix.hpp>
#include <algo/core/api/IAlgoParameters.hpp>

#include <algo/hits/common/AbstractPipeHitIterator.hpp>

/********************************************************************************/
namespace algo   {
namespace hits   {
/** \brief Implementation of IHitIterator interface for small gap alignments. */
namespace gapped {
/********************************************************************************/

/** \brief Implementation of IHitIterator for small gap alignments
 *
 * This implementation will take a source IHitIterator instance and computes small gap
 * score for each match.
 *
 * Matches whose score is below some threshold value are filtered out and won't be
 * iterated in linked IHitIterator (ie. further steps of the algorithm).
 *
 * This implementation follows a textbook approach for computing the score. Quicker
 * implementations may use SIMD instructions for vectorizing several scores computations.
 *
 * \see SmallGapHitIteratorSSE8
 */
class SmallGapHitIterator : public algo::hits::common::AbstractPipeHitIterator
{
public:

    /** \copydoc common::AbstractPipeHitIterator::AbstractPipeHitIterator */
    SmallGapHitIterator (
        algo::hits::IHitIterator*               sourceIterator,
        ::seed::ISeedModel*                     model,
        algo::core::IScoreMatrix*               scoreMatrix,
        algo::core::IParameters*                parameters,
        alignment::core::IAlignmentContainer*   ungapResult
    );

    /** Destructor. */
    virtual ~SmallGapHitIterator ();

    /** \copydoc common::AbstractPipeHitIterator::getName */
    const char* getName ()  { return "SmallGapHitIterator"; }

protected:

    /** \copydoc common::AbstractPipeHitIterator::clone */
    virtual AbstractPipeHitIterator* clone (algo::hits::IHitIterator* sourceIterator)
    {
        return new SmallGapHitIterator (sourceIterator, _model, _scoreMatrix, _parameters, _ungapResult);
    }

    /** \copydoc common::AbstractPipeHitIterator::iterateMethod */
    void iterateMethod (algo::hits::Hit* hit);

    /** Retrieve neighbourhoods for the provided seed occurrence.
     * \param[in]  occur     : the seed occurrence
     * \param[out] neighbour : the neighbourhood (seed+right+left) as a IWord
     */
    void extendNeighbourhood (const indexation::ISeedOccurrence* occur, database::IWord& neighbour);

    /** Compute the similarity score for two LETTER buffers.
     * \param[in] a : subject data
     * \param[in] b : query data
     */
    int16_t computeScore (const database::LETTER* a, const database::LETTER* b);
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _SMALLGAP_HIT_ITERATOR_HPP_ */
