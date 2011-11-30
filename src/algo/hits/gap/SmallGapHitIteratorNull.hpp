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

/** \file SmallGapHitIteratorNull.hpp
 *  \brief Implementation of IHitIterator interface for small gap alignments.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _SMALLGAP_HIT_ITERATOR_NULL_HPP_
#define _SMALLGAP_HIT_ITERATOR_NULL_HPP_

/********************************************************************************/

#include <seed/api/ISeedModel.hpp>

#include <algo/core/api/IScoreMatrix.hpp>

#include <algo/hits/common/AbstractPipeHitIterator.hpp>

/********************************************************************************/
namespace algo   {
namespace hits   {
/** \brief Implementation of IHitIterator interface for small gap alignments. */
namespace gapped {
/********************************************************************************/

/** \brief Null implementation of IHitIterator for small gap alignments
 *
 * This is a null implementation that iters no item at all.
 */
class SmallGapHitIteratorNull : public algo::hits::common::AbstractPipeHitIterator
{
public:

    /** \copydoc common::AbstractPipeHitIterator::AbstractPipeHitIterator */
    SmallGapHitIteratorNull (
        algo::hits::IHitIterator*       sourceIterator,
        seed::ISeedModel*               model,
        algo::core::IScoreMatrix*       scoreMatrix,
        algo::core::IParameters*        parameters,
        algo::align::IAlignmentResult* ungapResult
    );

    /** Destructor. */
    virtual ~SmallGapHitIteratorNull ();

    /** \copydoc common::AbstractPipeHitIterator::getName */
    const char* getName ()  { return "SmallGapHitIteratorNull"; }

protected:

    /** \copydoc common::AbstractPipeHitIterator::clone */
    virtual AbstractPipeHitIterator* clone (algo::hits::IHitIterator* sourceIterator)
    {
        return new SmallGapHitIteratorNull (sourceIterator, _model, _scoreMatrix, _parameters, _ungapResult);
    }

    /** \copydoc common::AbstractPipeHitIterator::iterateMethod
     * Nothing done here since this is the null implementation.
     */
    void iterateMethod (algo::hits::Hit* hit) {}
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _SMALLGAP_HIT_ITERATOR_NULL_HPP_ */
