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

#include <misc/api/Vector.hpp>

#include <algo/hits/ungap/UngapExtendHitIterator.hpp>

#include <math.h>

using namespace std;
using namespace misc;
using namespace os;
using namespace os::impl;
using namespace database;
using namespace seed;
using namespace indexation;
using namespace algo::core;
using namespace alignment::core;
using namespace statistics;

#include <stdio.h>
#define DEBUG(a)  //printf a

// Define a macro for optimized score retrieval through the vector-matrix.
#define getScore(i,j)  (_matrixAsVector [(i)+((j)<<5)])

/********************************************************************************/
namespace algo     {
namespace hits     {
namespace ungapped {
/********************************************************************************/

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
UngapExtendHitIterator::UngapExtendHitIterator (
        IHitIterator*           realIterator,
        ISeedModel*             model,
        IScoreMatrix*           scoreMatrix,
        IParameters*            parameters,
        IAlignmentContainer*    alignResult,
        IGlobalParameters*      globalStats,
        IQueryInformation*      queryInfo
)
: AbstractPipeHitIterator (realIterator, model, scoreMatrix, parameters, alignResult),
  _globalStats(0), _queryInfo(0)
{
    setGlobalStats (globalStats);
    setQueryInfo   (queryInfo);
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
UngapExtendHitIterator::~UngapExtendHitIterator ()
{
    setGlobalStats (0);
    setQueryInfo   (0);
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void UngapExtendHitIterator::iterateMethod (Hit* hit)
{
    /** Shortcuts. */
    const Vector<const ISeedOccurrence*>& occurSbjVector = hit->occur1;
    const Vector<const ISeedOccurrence*>& occurQryVector = hit->occur2;

    int16_t score      = 0;
    int16_t scoreMax   = 0;
    int16_t DROPOFF    = _parameters->ungapScoreThreshold;
    int indexLeft  = 0;
    int indexRight = 0;

    /** Statistics. */
    HIT_STATS (_inputHitsNumber += hit->indexes.size(); )

    for (list<IdxCouple>::iterator it = hit->indexes.begin();  it != hit->indexes.end();  )
    {
        /** Shortcut. */
        IdxCouple& idx = *it;

        /** Shortcuts. */
        const ISeedOccurrence* occurSbj = occurSbjVector.data [idx.first];
        const ISeedOccurrence* occurQry = occurQryVector.data [idx.second];

        const ISequence& subjectSeq = occurSbj->sequence;
        const ISequence& querySeq   = occurQry->sequence;

        /** Shortcuts. */
        LETTER* loopSbj = occurSbj->sequence.data.letters.data + occurSbj->offsetInSequence;
        LETTER* loopQry = occurQry->sequence.data.letters.data + occurQry->offsetInSequence;

        int lengthLeft  = MIN (
            occurSbj->offsetInSequence,
            occurQry->offsetInSequence
        );
        int lengthRight = MIN (
            occurSbj->sequence.getLength() - occurSbj->offsetInSequence,
            occurQry->sequence.getLength() - occurQry->offsetInSequence
        );

        indexLeft  = 0;
        indexRight = 0;
        score      = 0;
        scoreMax   = 0;

        for (int k=1; k<=lengthLeft; k++)
        {
            score += getScore (loopSbj[-k], loopQry[-k]);
            if (score > scoreMax)
            {
                scoreMax  = score;
                indexLeft = k;
            }

            if (scoreMax - score >= DROPOFF)   {  break;  }
        }

        score = scoreMax;

        for (int k=0; k<lengthRight; k++)
        {
            score += getScore (loopSbj[k], loopQry[k]);

            if (score > scoreMax)
            {
                scoreMax   = score;
                indexRight = k;
            }

            if (scoreMax - score >= DROPOFF)   {  break;  }
        }

        score = scoreMax;

        /** Now, we have computed a HSP score. */

        /** We retrieve statistical information for the current query sequence. */
        IQueryInformation::SequenceInfo& info = _queryInfo->getSeqInfo (occurQry->sequence);

        if (score >= info.cut_offs)
        {
            /** We create a new alignment. */
            Alignment align (
                &(occurQry->sequence),        &(occurSbj->sequence),
                occurQry->offsetInSequence,   occurSbj->offsetInSequence,
                indexLeft,                    indexLeft,
                indexRight,                   indexRight
            );

            if (_ungapResult->doesExist(align) == false)
            {
                /** We complete missing alignment information. */
                //align.setEvalue   ((double) info.eff_searchsp * exp((-_globalStats->lambda * (double) score) + _globalStats->logK));
				align.setEvalue   (_globalStats->scoreToEvalue((double) info.eff_searchsp, (double) score, querySeq.getLength(),subjectSeq.getLength()));

                align.setBitScore (_globalStats->rawToBitsValue((double)score));
                align.setScore    (score);
                align.setLength   (indexRight + indexLeft + 1);

                /** We get the identity value. */
                u_int32_t nbIdentities = 0;
                loopSbj = occurSbj->sequence.data.letters.data + occurSbj->offsetInSequence - indexLeft;
                loopQry = occurQry->sequence.data.letters.data + occurQry->offsetInSequence - indexLeft;

                for (u_int32_t k=0; k<align.getLength(); k++)
                {
                    if ( *(loopSbj++) == *(loopQry++) )  {  nbIdentities ++; }
                }

                align.setNbIdentities (nbIdentities);
                align.setNbMisses     (align.getLength() - nbIdentities);

                /** We add the alignment into the global alignment container. */
                _ungapResult->insert (align, 0);

                /** We just continue the iteration. */
                it++;
            }
            else
            {
                it = hit->indexes.erase (it);
            }
        }
        else
        {
            it = hit->indexes.erase (it);
        }
    }

    /** We update the statistics about iterations. */
    HIT_STATS (_outputHitsNumber += hit->indexes.size();)

    /** We are supposed to have computed scores for each hit,
     *  we can forward the information to the client.  */
    if (hit->indexes.empty() == false)      {  (_client->*_method) (hit);  }
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
