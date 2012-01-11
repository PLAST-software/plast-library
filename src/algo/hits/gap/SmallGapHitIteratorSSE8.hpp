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

/** \file SmallGapHitIteratorSSE8.hpp
 *  \brief Implementation of IHitIterator interface for small gap alignments.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _SMALLGAP_HIT_ITERATOR_SSE8_HPP_
#define _SMALLGAP_HIT_ITERATOR_SSE8_HPP_

/********************************************************************************/

#include <misc/api/Vector.hpp>

#include <seed/api/ISeedModel.hpp>

#include <algo/core/api/IScoreMatrix.hpp>

#include <algo/hits/common/AbstractPipeHitIterator.hpp>

/********************************************************************************/
namespace algo   {
namespace hits   {
/** \brief Implementation of IHitIterator interface for small gap alignments. */
namespace gapped {
/********************************************************************************/

/** \brief Implementation of IHitIterator for small gap alignments with SIMD usage
 *
 * This implementation uses the Simple Instruction Multiple Data instructions set
 * for vectorizing several scores computations in a single call.
 *
 * The vectorization scheme allows here to compute 8 scores in the same time, so each
 * score can be computed on 128/8= 16 bits. Absolute values of maximal ungap scores
 * can't be greater than 2^16, so we don't fear any potential truncations in scores.
 */
class SmallGapHitIteratorSSE8 : public algo::hits::common::AbstractPipeHitIterator
{
public:

    /** \copydoc common::AbstractPipeHitIterator::AbstractPipeHitIterator */
    SmallGapHitIteratorSSE8 (
        algo::hits::IHitIterator*       sourceIterator,
        ::seed::ISeedModel*             model,
        algo::core::IScoreMatrix*       scoreMatrix,
        algo::core::IParameters*        parameters,
        algo::align::IAlignmentResult* ungapResult,
        algo::align::IAlignmentResult* alignmentResult
    );

    /** Destructor. */
    virtual ~SmallGapHitIteratorSSE8 ();

    /** \copydoc common::AbstractPipeHitIterator::getName */
    const char* getName ()  { return "SmallGapHitIteratorSSE8"; }

    /** \copydoc common::AbstractPipeHitIterator::getProperties */
    dp::IProperties* getProperties ();

protected:

    /** \copydoc common::AbstractPipeHitIterator::clone */
    virtual AbstractPipeHitIterator* clone (algo::hits::IHitIterator* sourceIterator)
    {
        return new SmallGapHitIteratorSSE8 (sourceIterator, _model, _scoreMatrix, _parameters, _ungapResult, _alignmentResult);
    }

    /** \copydoc common::AbstractPipeHitIterator::iterateMethod */
    void iterateMethod (algo::hits::Hit* hit);

    /** Retrieve neighbourhoods for the provided seed occurrence.
     * \param[in]  occur : the seed occurrence
     * \param[out] right : right neighbourhood
     * \param[out] left  : right neighbourhood
     */
    void extendNeighbourhood (
        const indexation::ISeedOccurrence* occur,
        database::LETTER* right,
        database::LETTER* left
    );

    /** Compute the scores.
     * \param[in] nb : number of scores
     * \param[in]  neighbourhoods1 : neighbourhoods in subject
     * \param[in]  neighbourhoods2 : neighbourhoods in query
     * \param[out] scores          : computed scores
     */
    void computeScores (
        size_t nb,
        const database::LETTER* neighbourhoods1,
        const database::LETTER* neighbourhoods2,
        int* scores
    );

    misc::Vector<int> _computedScores;
    int* getComputedScores (size_t n);

    misc::Vector<database::LETTER> _neighboursBuf1;
    database::LETTER* getNeighbours1 (size_t n);

    misc::Vector<database::LETTER> _neighboursBuf2;
    database::LETTER* getNeighbours2 (size_t n);

    /* Statistics. */
    u_int64_t _lowScoreNumber;

    /** Gap alignments. */
    algo::align::IAlignmentResult* _alignmentResult;

    /** Smart setter for _alignmentResult attribute. */
    void setAlignmentResult (algo::align::IAlignmentResult* alignmentResult)  { SP_SETATTR (alignmentResult); }
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _SMALLGAP_HIT_ITERATOR_SSE8_HPP_ */
