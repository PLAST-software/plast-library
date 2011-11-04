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

#ifndef _COMPOSITION_HIT_ITERATOR_HPP_
#define _COMPOSITION_HIT_ITERATOR_HPP_

/********************************************************************************/

#include "ISeedModel.hpp"
#include "IScoreMatrix.hpp"
#include "IAlgoParameters.hpp"
#include "AbstractScoredHitIterator.hpp"
#include "IStatistics.hpp"
#include "IAlignementResult.hpp"
#include "IAlignmentSplitter.hpp"

#include "UngapAlignmentResult.hpp"

#include <set>

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/** Implemented as a Design Pattern Proxy (proxy on the provided HitIterator)
 */
class CompositionHitIterator : public algo::AbstractScoredHitIterator
{
public:
    CompositionHitIterator (
        indexation::IHitIterator*       realIterator,
        seed::ISeedModel*               model,
        algo::IScoreMatrix*             scoreMatrix,
        algo::IParameters*              parameters,
        statistics::IQueryInformation*  queryInfo,
        statistics::IGlobalParameters*  globalStats,
        algo::IAlignmentResult*         ungapResult,
        algo::IAlignmentResult*         alignmentResult
    );
    virtual ~CompositionHitIterator ();

    /** */
    const char* getName ()  { return "CompositionHitIterator"; }

    /** */
    void iterate (void* aClient, Method method);

    /** */
    dp::IProperties* getProperties ();

protected:

    statistics::IQueryInformation*  _queryInfo;
    void setQueryInfo (statistics::IQueryInformation*  queryInfo)  {    SP_SETATTR(queryInfo);  }

    statistics::IGlobalParameters*  _globalStats;
    void setGlobalStats (statistics::IGlobalParameters*  globalStats)  { SP_SETATTR(globalStats); }

    algo::IAlignmentResult* _alignmentResult;
    void setAlignmentResult (algo::IAlignmentResult* alignmentResult)  { SP_SETATTR (alignmentResult); }

    IAlignmentSplitter* _splitter;
    void setAlignmentSplitter (algo::IAlignmentSplitter* splitter)  { SP_SETATTR (splitter); }

    /** We need a clone method. */
    virtual AbstractScoredHitIterator* clone (indexation::IHitIterator* realIterator)
    {
        return new CompositionHitIterator (realIterator, _model, _scoreMatrix, _parameters, _queryInfo, _globalStats, _ungapResult, _alignmentResult);
    }

    /** */
    void iterateMethod (indexation::Hit* hit);

    /** ref NCBI */
    int ALIGN_EX (
        char* A,
        char* B,
        int M,
        int N,
        int* a_offset,
        int* b_offset,
        int reverse_sequence,
        bool composition_based
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

class CompositionHitIteratorNull : public CompositionHitIterator
{
public:
    CompositionHitIteratorNull (
        indexation::IHitIterator*       realIterator,
        seed::ISeedModel*               model,
        algo::IScoreMatrix*             scoreMatrix,
        algo::IParameters*              parameters,
        statistics::IQueryInformation*  queryInfo,
        statistics::IGlobalParameters*  globalStats,
        algo::IAlignmentResult*         ungapResult,
        algo::IAlignmentResult*         alignmentResult
    ) :  CompositionHitIterator (realIterator, model, scoreMatrix, parameters, queryInfo, globalStats, ungapResult, alignmentResult) {}

    virtual ~CompositionHitIteratorNull () {}

    /** */
    const char* getName ()  { return "CompositionHitIteratorNull"; }

protected:

    /** We need a clone method. */
    virtual AbstractScoredHitIterator* clone (indexation::IHitIterator* realIterator)
    {
        return new CompositionHitIteratorNull (realIterator, _model, _scoreMatrix, _parameters, _queryInfo, _globalStats, _ungapResult, _alignmentResult);
    }

    /** */
    void iterateMethod (indexation::Hit* hit) {}
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _COMPOSITION_HIT_ITERATOR_HPP_ */
