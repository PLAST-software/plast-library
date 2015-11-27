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

/** \file UngapHitIteratorNull.hpp
 *  \brief Implementation of IHitIterator interface for ungap alignments.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _UNGAP_HIT_ITERATOR_NULL_HPP_
#define _UNGAP_HIT_ITERATOR_NULL_HPP_

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

/** \brief Null implementation of IHitIterator for ungap alignments
 *
 * Null implementation (ie. iterates nothing).
 */
class UngapHitIteratorNull : public algo::hits::common::AbstractPipeHitIterator
{
public:

    /** \copydoc common::AbstractPipeHitIterator::AbstractPipeHitIterator */
    UngapHitIteratorNull (
        algo::hits::IHitIterator*               sourceIterator,
        seed::ISeedModel*                       model,
        algo::core::IScoreMatrix*               scoreMatrix,
        algo::core::IParameters*                parameters,
        alignment::core::IAlignmentContainer*   ungapResult
    );

    /** \copydoc common::AbstractPipeHitIterator::getName */
    const char* getName ()  { return "UngapHitIteratorNull"; }

protected:

    /** \copydoc common::AbstractPipeHitIterator::iterateMethod
     * This method does nothing as null implementation.
     */
    void iterateMethod  (Hit* hit) {}

    /** \copydoc common::AbstractPipeHitIterator::clone */
    virtual AbstractPipeHitIterator* clone (IHitIterator* sourceIterator)
    {
        return new UngapHitIteratorNull (sourceIterator, _model, _scoreMatrix, _parameters, _ungapResult);
    }
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _UNGAP_HIT_ITERATOR_NULL_HPP_ */
