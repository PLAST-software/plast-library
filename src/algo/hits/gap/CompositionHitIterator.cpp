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

#include <os/impl/DefaultOsFactory.hpp>

#include <misc/api/macros.hpp>
#include <misc/api/Vector.hpp>

#include <algo/stats/api/IStatistics.hpp>

#include <algo/hits/gap/CompositionHitIterator.hpp>

#include <algo/align/impl/BasicAlignmentResult.hpp>
#include <algo/align/impl/UngapAlignmentResult.hpp>
#include <algo/align/impl/AlignmentSplitter.hpp>
#include <algo/align/impl/SemiGappedAlign.hpp>

#include <math.h>

using namespace std;
using namespace misc;
using namespace os;
using namespace os::impl;
using namespace database;
using namespace seed;
using namespace indexation;
using namespace statistics;
using namespace algo::core;
using namespace algo::align;
using namespace algo::align::impl;

#include <stdio.h>
#define DEBUG(a)  //printf a

#ifndef INT4_MIN
    /** Smallest (most negative) number represented by signed int */
    #define INT4_MIN    (-2147483647-1)
#endif

/** Lower bound for scores. Divide by two to prevent underflows. */
#define MININT INT4_MIN/2

/********************************************************************************/
namespace algo   {
namespace hits   {
namespace gapped {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
CompositionHitIterator::CompositionHitIterator (
    IHitIterator*       realIterator,
    ISeedModel*         model,
    IScoreMatrix*       scoreMatrix,
    IParameters*        parameters,
    IAlignmentResult*   ungapResult,
    IQueryInformation*  queryInfo,
    IGlobalParameters*  globalStats,
    IAlignmentResult*   alignmentResult
)
    : FullGapHitIterator (realIterator, model, scoreMatrix, parameters, ungapResult, queryInfo, globalStats, alignmentResult)
{
    setDynPro (new SemiGapAlign (_scoreMatrix, _parameters->openGapCost, _parameters->extendGapCost, _parameters->finalXdroppofGap));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
CompositionHitIterator::~CompositionHitIterator ()
{
    setQueryInfo         (0);
    setGlobalStats       (0);
    setAlignmentResult   (0);
    setAlignmentSplitter (0);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void CompositionHitIterator::iterateMethod  (Hit* hit)
{
    HIT_STATS (_iterateMethodNbCalls++);

    double ln2 = 0.69314718055994530941;

    /** Shortcuts. */
    const Vector<const ISeedOccurrence*>& occur1Vector = hit->occur1;
    const Vector<const ISeedOccurrence*>& occur2Vector = hit->occur2;

    int scoreLeft=0,  scoreRight=0;
    int leftOffsetInQuery=0, leftOffsetInSubject=0, rightOffsetInQuery=0, rightOffsetInSubject=0;
    bool shouldKeep = false;

    /** Statistics. */
    HIT_STATS (_inputHitsNumber += hit->indexes.size();)

    for (list<IdxCouple>::iterator it = hit->indexes.begin();  it != hit->indexes.end();  )
    {
        /** Shortcut. */
        IdxCouple& idx = *it;

        /** Shortcuts. */
        const ISeedOccurrence* occurSubject = occur1Vector.data [idx.first];
        const ISeedOccurrence* occurQuery   = occur2Vector.data [idx.second];

        /** Shortcuts. */
        const ISequence& subjectSeq = occurSubject->sequence;
        const ISequence& querySeq   = occurQuery->sequence;

        /** Shortcuts. */
        LETTER* subjectData = subjectSeq.data.letters.data;
        LETTER* queryData   = querySeq.data.letters.data;

        /** By default, we don't want to keep this current hit. */
        shouldKeep = false;

        /** We compute the left part of the score. Note that the left extension includes the starting point,
         * the right extension does not. */
        scoreLeft = _dynpro->compute (
            queryData,
            subjectData,
            occurQuery->offsetInSequence   + 1,
            occurSubject->offsetInSequence + 1,
            & leftOffsetInQuery,
            & leftOffsetInSubject,
            1
        );

        /** We compute the right part of the score. */
        scoreRight = _dynpro->compute (
            queryData   + occurQuery->offsetInSequence,
            subjectData + occurSubject->offsetInSequence,
            querySeq.data.letters.size   - occurQuery->offsetInSequence   - 1,
            subjectSeq.data.letters.size - occurSubject->offsetInSequence - 1,
            & rightOffsetInQuery,
            & rightOffsetInSubject,
            0
        );

        int score = scoreLeft + scoreRight;

        /** We retrieve statistical information for the current query sequence. */
        IQueryInformation::SequenceInfo& info = _queryInfo->getSeqInfo (querySeq);

        if (score >= info.cut_offs)
        {
            /** We create a new alignment. */
            Alignment align (
                occurSubject,
                occurQuery,
                leftOffsetInQuery   - 1,
                leftOffsetInSubject - 1,
                rightOffsetInQuery,
                rightOffsetInSubject,
                score
            );

            /** We check that the alignment is not already known. */
            if (_alignmentResult->doesExist (align) == false)
            {
                /** We increase the number of found decent hits. */
                HIT_STATS (_outputHitsNumber ++;)

                /** We complete missing alignment information. */
                align._evalue   = (double) info.eff_searchsp * exp((-_globalStats->lambda * (double) score) + _globalStats->logK);
                align._bitscore = (_globalStats->lambda * (double)score - _globalStats->logK) / ln2;

                /** This will compute identity, nb gaps, ... */
                _splitter->computeInfo (align);

                /** We add the alignment into the global alignment container. */
                _alignmentResult->insert (align, 0);

                /** We just go to the next item. */
                it++;
            }
            else
            {
                /** Statistics information. */
                HIT_STATS (_gapKnownNumber++;)

                /** We remove the current index couple. */
                it = hit->indexes.erase(it);
            }

        } /* end of if (score >= info.cut_offs) */

        else
        {
            /** We remove the current index couple. */
            it = hit->indexes.erase(it);
        }

    } /* end of for (IdxCouple* it = hit->indexes... */

    /** We are supposed to have computed scores for each hit,
     *  we can forward the information to the client.  */
    (_client->*_method) (hit);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
dp::IProperties* CompositionHitIterator::getProperties ()
{
    dp::IProperties* result = AbstractHitIterator::getProperties ();

    /** We have to aggregate values from different split instances. */
    u_int64_t ungapKnownNumber = _ungapKnownNumber;
    u_int64_t gapKnownNumber   = _gapKnownNumber;
    for (size_t i=0; i<_splitIterators.size(); i++)
    {
        CompositionHitIterator* current = dynamic_cast<CompositionHitIterator*> (_splitIterators[i]);
        if (current)
        {
            ungapKnownNumber += current->_ungapKnownNumber;
            gapKnownNumber   += current->_gapKnownNumber;
        }
    }

    result->add (1, "details");
    result->add (2, "known_gap",   "%ld",  gapKnownNumber);

    /** We call the parent method in case we have split instances. */
    return result;

}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
