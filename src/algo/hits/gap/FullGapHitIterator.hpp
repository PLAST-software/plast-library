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

/** \file FullGapHitIterator.hpp
 *  \brief Implementation of IHitIterator interface for full gap alignments.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _FULL_HIT_ITERATOR_HPP_
#define _FULL_HIT_ITERATOR_HPP_

/********************************************************************************/

#include <seed/api/ISeedModel.hpp>

#include <algo/core/api/IScoreMatrix.hpp>
#include <algo/core/api/IAlgoParameters.hpp>

#include <algo/stats/api/IStatistics.hpp>

#include <algo/hits/common/AbstractPipeHitIterator.hpp>

#include <algo/align/api/IAlignmentResult.hpp>
#include <algo/align/api/IAlignmentSplitter.hpp>

#include <algo/align/impl/UngapAlignmentResult.hpp>

#include <set>

/********************************************************************************/
namespace algo   {
namespace hits   {
/** \brief Implementation of IHitIterator interface for small gap alignments. */
namespace gapped {
/********************************************************************************/

/** \brief Implementation of IHitIterator for full gap alignments
 *
 * Full implementation of the dynamic programming algorithm for computing the gap
 * alignments.
 *
 * This implementation relies on some code of the NCBI BLAST (see SemiGappedAlign()).
 */
class FullGapHitIterator : public algo::hits::common::AbstractPipeHitIterator
{
public:

    /** \copydoc common::AbstractPipeHitIterator::AbstractPipeHitIterator
     * \param[in] queryInfo        : information about query sequences (including cutoffs)
     * \param[in] globalStats      : global statistics
     * \param[out] alignmentResult : list of gap alignments to be filled during algorithm execution
     */
    FullGapHitIterator (
        algo::hits::IHitIterator*       sourceIterator,
        seed::ISeedModel*               model,
        algo::core::IScoreMatrix*       scoreMatrix,
        algo::core::IParameters*        parameters,
        algo::align::IAlignmentResult* ungapResult,
        statistics::IQueryInformation*  queryInfo,
        statistics::IGlobalParameters*  globalStats,
        algo::align::IAlignmentResult* alignmentResult
    );

    /** Destructor. */
    virtual ~FullGapHitIterator ();

    /** \copydoc common::AbstractPipeHitIterator::getName */
    const char* getName ()  { return "FullGapHitIterator"; }

    /** \copydoc common::AbstractPipeHitIterator::getProperties */
    dp::IProperties* getProperties ();

protected:

    /** \copydoc common::AbstractPipeHitIterator::clone */
    virtual AbstractPipeHitIterator* clone (algo::hits::IHitIterator* sourceIterator)
    {
        return new FullGapHitIterator (sourceIterator, _model, _scoreMatrix, _parameters, _ungapResult, _queryInfo, _globalStats, _alignmentResult);
    }

    /** \copydoc common::AbstractPipeHitIterator::iterateMethod */
    void iterateMethod (algo::hits::Hit* hit);

    /** Information about query sequences. */
    statistics::IQueryInformation*  _queryInfo;

    /** Smart setter for _queryInfo attribute. */
    void setQueryInfo (statistics::IQueryInformation*  queryInfo)  {    SP_SETATTR(queryInfo);  }

    /** Global statistics. */
    statistics::IGlobalParameters*  _globalStats;

    /** Smart setter for _globalStats attribute. */
    void setGlobalStats (statistics::IGlobalParameters*  globalStats)  { SP_SETATTR(globalStats); }

    /** Gap alignments. */
    algo::align::IAlignmentResult* _alignmentResult;

    /** Smart setter for _alignmentResult attribute. */
    void setAlignmentResult (algo::align::IAlignmentResult* alignmentResult)  { SP_SETATTR (alignmentResult); }

    /** Tool that knows how to split an gap alignment into several ungap alignments. */
    algo::align::IAlignmentSplitter* _splitter;

    /** Smart setter for _splitter attribute. */
    void setAlignmentSplitter (algo::align::IAlignmentSplitter* splitter)  { SP_SETATTR (splitter); }

    /** Dynamic programming for finding a gap alignment betweed two sequences. Ref NCBI.
     */
    int SemiGappedAlign (
        char* A,
        char* B,
        int M,
        int N,
        int* a_offset,
        int* b_offset,
        int reverse_sequence,
        int8_t** matrix,
        int gap_open,
        int gap_extend,
        int x_dropoff
    );

    struct BlastGapDP {
        int best;
        int best_gap;
    };

    u_int64_t _ungapKnownNumber;
    u_int64_t _gapKnownNumber;

    bool _checkMemory;
};

/********************************************************************************/

/** \brief Null implementation of IHitIterator for full gap alignments
 */
class FullGapHitIteratorNull : public algo::hits::common::AbstractPipeHitIterator
{
public:

    /** \copydoc common::AbstractPipeHitIterator::AbstractPipeHitIterator
     * \param[in] queryInfo        : information about query sequences (including cutoffs)
     * \param[in] globalStats      : global statistics
     * \param[out] alignmentResult : list of gap alignments to be filled during algorithm execution
     */
    FullGapHitIteratorNull (
        algo::hits::IHitIterator*       sourceIterator,
        seed::ISeedModel*               model,
        algo::core::IScoreMatrix*       scoreMatrix,
        algo::core::IParameters*        parameters,
        algo::align::IAlignmentResult* ungapResult,
        statistics::IQueryInformation*  queryInfo,
        statistics::IGlobalParameters*  globalStats,
        algo::align::IAlignmentResult* alignmentResult
    ) :  common::AbstractPipeHitIterator (sourceIterator, model, scoreMatrix, parameters, ungapResult) {}

    /** Destructor. */
    virtual ~FullGapHitIteratorNull () {}

    /** \copydoc common::AbstractPipeHitIterator::getName */
    const char* getName ()  { return "FullGapHitIteratorNull"; }

protected:

    /** \copydoc common::AbstractPipeHitIterator::clone */
    virtual AbstractPipeHitIterator* clone (algo::hits::IHitIterator* sourceIterator)
    {
        return new FullGapHitIteratorNull (sourceIterator, _model, _scoreMatrix, _parameters, _ungapResult, 0, 0, 0);
    }

    /** \copydoc common::AbstractPipeHitIterator::iterateMethod */
    void iterateMethod (algo::hits::Hit* hit) {}
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _FULL_HIT_ITERATOR_HPP_ */
