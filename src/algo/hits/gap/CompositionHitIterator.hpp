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

/** \file CompositionHitIterator.hpp
 *  \brief Implementation of IHitIterator interface for full gap alignments.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _COMPOSITION_HIT_ITERATOR_HPP_
#define _COMPOSITION_HIT_ITERATOR_HPP_

/********************************************************************************/

#include <seed/api/ISeedModel.hpp>

#include <algo/core/api/IScoreMatrix.hpp>
#include <algo/core/api/IAlgoParameters.hpp>

#include <algo/stats/api/IStatistics.hpp>

#include <algo/hits/common/AbstractPipeHitIterator.hpp>
#include <algo/hits/gap/FullGapHitIterator.hpp>

#include <alignment/core/api/IAlignmentContainer.hpp>
#include <alignment/tools/api/IAlignmentSplitter.hpp>

#include <alignment/core/impl/UngapAlignmentContainer.hpp>

#include <set>

/********************************************************************************/
namespace algo   {
namespace hits   {
/** \brief Implementation of IHitIterator interface for small gap alignments. */
namespace gapped {
/********************************************************************************/

/** \brief Implementation of IHitIterator for full gap alignments
 *
 * Last step in the PLAST algorithm. This is here where the resulting Alignment
 * instances are created.
 *
 * This implementation relies on some code of the NCBI BLAST (see ALIGN_EX()).
 */
class CompositionHitIterator : public algo::hits::gapped::FullGapHitIterator
{
public:

    /** \copydoc common::FullGapHitIterator::FullGapHitIterator
     * \param[in] queryInfo        : information about query sequences (including cutoffs)
     * \param[in] globalStats      : global statistics
     * \param[out] alignmentResult : list of gap alignments to be filled during algorithm execution
     */
        CompositionHitIterator (
        algo::hits::IHitIterator*               sourceIterator,
        seed::ISeedModel*                       model,
        algo::core::IScoreMatrix*               scoreMatrix,
        algo::core::IParameters*                parameters,
        alignment::core::IAlignmentContainer*   ungapResult,
        statistics::IQueryInformation*          queryInfo,
        statistics::IGlobalParameters*          globalStats,
        alignment::core::IAlignmentContainer*   alignmentResult
    );

    /** Destructor. */
    virtual ~CompositionHitIterator ();

    /** \copydoc common::AbstractPipeHitIterator::getName */
    const char* getName ()  { return "CompositionHitIterator"; }

    /** \copydoc common::AbstractPipeHitIterator::getProperties */
    dp::IProperties* getProperties ();

protected:

    /** \copydoc common::AbstractPipeHitIterator::clone */
    virtual CompositionHitIterator* clone (algo::hits::IHitIterator* sourceIterator)
    {
        return new CompositionHitIterator (sourceIterator, _model, _scoreMatrix, _parameters, _ungapResult, _queryInfo, _globalStats, _alignmentResult);
    }

    /** \copydoc common::AbstractPipeHitIterator::iterateMethod */
    void iterateMethod (algo::hits::Hit* hit);
};

/********************************************************************************/

/** \brief Null implementation of IHitIterator for full gap alignments
 */
class CompositionHitIteratorNull : public algo::hits::common::AbstractPipeHitIterator
{
public:

    /** \copydoc common::AbstractPipeHitIterator::AbstractPipeHitIterator
     * \param[in] queryInfo        : information about query sequences (including cutoffs)
     * \param[in] globalStats      : global statistics
     * \param[out] alignmentResult : list of gap alignments to be filled during algorithm execution
     */
    CompositionHitIteratorNull (
        algo::hits::IHitIterator*               sourceIterator,
        seed::ISeedModel*                       model,
        algo::core::IScoreMatrix*               scoreMatrix,
        algo::core::IParameters*                parameters,
        alignment::core::IAlignmentContainer*   ungapResult,
        statistics::IQueryInformation*          queryInfo,
        statistics::IGlobalParameters*          globalStats,
        alignment::core::IAlignmentContainer*   alignmentResult
    ) :  common::AbstractPipeHitIterator (sourceIterator, model, scoreMatrix, parameters, ungapResult) {}

    /** Destructor. */
    virtual ~CompositionHitIteratorNull () {}

    /** \copydoc common::AbstractPipeHitIterator::getName */
    const char* getName ()  { return "CompositionHitIteratorNull"; }

protected:

    /** \copydoc common::AbstractPipeHitIterator::clone */
    virtual AbstractPipeHitIterator* clone (algo::hits::IHitIterator* sourceIterator)
    {
        return new CompositionHitIteratorNull (sourceIterator, _model, _scoreMatrix, _parameters, _ungapResult, 0, 0, 0);
    }

    /** \copydoc common::AbstractPipeHitIterator::iterateMethod */
    void iterateMethod (algo::hits::Hit* hit) {}
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _COMPOSITION_HIT_ITERATOR_HPP_ */
