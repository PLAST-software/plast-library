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

#include <misc/api/Vector.hpp>
#include <misc/api/macros.hpp>

#include <algo/hits/ungap/UngapHitIteratorSIMD.hpp>

using namespace std;
using namespace misc;
using namespace os;
using namespace os::impl;
using namespace database;
using namespace seed;
using namespace indexation;
using namespace algo::core;
using namespace alignment::core;
using namespace dp::impl;

#include <stdio.h>
#define DEBUG(a)  //printf a

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
UngapHitIteratorSIMD::UngapHitIteratorSIMD (
    IHitIterator*        realIterator,
    ISeedModel*          model,
    IScoreMatrix*        scoreMatrix,
    IParameters*         parameters,
    IAlignmentContainer* ungapResult,
    u_int32_t            maxHitsPerIteration,
    bool&                isRunning
)
    : AbstractPipeHitIterator (realIterator, model, scoreMatrix, parameters, ungapResult),
      _ungapKnownNumber(0), _databk(0), _totalTSC(0), _otherTSC(0), _isRunning (isRunning)
{
    DEBUG (("UngapHitIteratorSIMD::UngapHitIteratorSIMD:  span=%ld  _neighbourLength=%d \n",
        _model->getSpan(),
        _neighbourLength
    ));

    /** We overide the default value. */
    if (maxHitsPerIteration > 0)  { _maxHitsPerIteration = maxHitsPerIteration; }

    size_t sizeMatrix        = _scoreMatrix->getN();
    size_t sizeNeighbour     = _span + 2*_parameters->ungapNeighbourLength;

    /** We allocate some memory space for inner processing. */
    _databkSize = (sizeMatrix*sizeNeighbour + 64) * 8*sizeof(VectorSIMD);
    _databk = (char *) DefaultFactory::memory().malloc (_databkSize);

    size_t aligned = ((size_t) (_databk + 0x0f)) & ~(0x0f);
    _pvb = (VectorSIMD*) aligned;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
UngapHitIteratorSIMD::~UngapHitIteratorSIMD ()
{
    /** Some clean up. */
    DefaultFactory::memory().free (_databk);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void UngapHitIteratorSIMD::iterateMethod (Hit* hit)
{
    HIT_STATS_VERBOSE (_iterateMethodNbCalls++);

    VectorSIMD pvScore;
    VectorSIMD score;
    VectorSIMD maxScore;

    u_int8_t sizeMatrix        = _scoreMatrix->getN();
    u_int8_t sizeNeighbour     = _span + 2*_parameters->ungapNeighbourLength;
    u_int8_t sizeHalfNeighbour = _span + 1*_parameters->ungapNeighbourLength;

    u_int32_t currentNbHits = hit->size();

    short   bias  = _scoreMatrix->getDefaultScore();
    VectorSIMD vBias (-bias);

    /** Shortcuts. */
    const Vector<const ISeedOccurrence*>& occur1Vector = hit->occur1;
    const Vector<const ISeedOccurrence*>& occur2Vector = hit->occur2;
    const ISeedOccurrence* occur1ForIndexI = 0;

    size_t nb1 = occur1Vector.size;
    size_t nb2 = occur2Vector.size;

    VectorSIMD threshold  (_parameters->ungapScoreThreshold - 1);
    VectorSIMD threshold2 (_parameters->ungapScoreThreshold);

    /** Statistics. */
    HIT_STATS (_inputHitsNumber += nb1 * nb2;)

    /** We retrieve the whole neighbourhoods buffer. This is important to get the full buffer
     * for locality concerns (ie. for optimizing CPU cache usage). */
    const LETTER* neighboursOccur2 = hit->neighbourhoodsOccur2;

    /** We loop over query occurrences. */
    for (size_t j=0; _isRunning && j<nb2; j+=VECTORSIZE)
    {
        /** We first reset to 0 all scores of the pseudo SSE score matrix. */
        memset (_databk, 0, _databkSize);

        char* loop = (char *) _pvb;

        u_int8_t lmin  = MIN (nb2 - j, VECTORSIZE);
        u_int8_t delta = (VECTORSIZE - lmin);

        for (u_int8_t m=0; m<sizeMatrix; m++)
        {
            int8_t* matrixRow = _matrix[m];

            for (u_int8_t k=0; k<sizeNeighbour; k++)
            {
                for (u_int8_t l=0; l<lmin; l++)
                {
                    *loop++ = (char) (matrixRow [(int) neighboursOccur2[(j+l)*sizeNeighbour+k]] - bias);
                }

                /** We may have to skip some null scores (in case lmin > NB). */
                loop += delta;
            }
        }

        /** We loop over subject occurrences. */
        for (size_t i=0; i<nb1; i++)
        {
            /** We use a shortcut (avoids several memory accesses). */
            occur1ForIndexI = occur1Vector.data[i];

            LETTER* neighbour1 = occur1ForIndexI->neighbourhood.letters.data;

            maxScore = 0;
            score    = 0;

            for (u_int8_t k=0; k<sizeHalfNeighbour; k++)
            {
                pvScore.load_a (_pvb + (neighbour1[k] * sizeNeighbour + k));
                score    = add_saturated (score, pvScore);
                score    = sub_saturated (score, vBias);
                maxScore = max           (maxScore, score);
            }

            score = maxScore;

            for (u_int8_t k=sizeHalfNeighbour; k<sizeNeighbour; k++)
            {
                pvScore.load_a (_pvb + (neighbour1[k] * sizeNeighbour + k));
                score    = add_saturated (score,    pvScore);
                score    = sub_saturated (score,    vBias);
                maxScore = max           (maxScore, score);
            }

            /** Note the trick here: we first re-calibrate the computed max score
             *  in order to get rid of possible saturations. */
            maxScore = min (maxScore, threshold2);

            /** We compare the computed score to the threshold. */
            VectorSIMDs cmp = maxScore > threshold;

            /** Possible optimization: just continue if we know that every threshold tests failed. */
            if (horizontal_or(cmp) == 0)  { continue; }

            /** We retrieve the threshold comparison as a vector. */
            u_int8_t cmpVec [VECTORSIZE];   cmp.store_a (cmpVec);

            for (u_int8_t k=0; k<VECTORSIZE; k++)
            {
                /** If the value is 0, it means that the two comparisons failed the threshold test. */
                if (cmpVec[k] != 0)
                {
                    size_t idx = j+k;

                    if (_ungapResult->doesExist(occur1ForIndexI, occur2Vector.data[idx], 0) == false)
                    {
                        /** We tag this hit to be used for further processing. */
                        currentNbHits = hit->addIndexes (i, idx);

                        /** We may want to go further in the algorithm with the currently found hits.
                         *  The important consequence is that we can limit the number of hits processed by
                         *  further iterators. Without such a threshold, these iterators may have to deal with
                         *  thousands of hits, which can be prohibitive in terms of memory usage.
                         */
                        /** Note the "== 4" here; since the next step (likely small gaps) uses a SIMD scheme
                         *  that vectorizes 8 scores computation at a time, we try to send to it blocks of 4 hits
                         *  in order to compute 4 left and 4 right useful scores (and no fake score used to fill up
                         *  to 8 the SIMD 128 bits variables).  */
                        if (currentNbHits == 4)
                        {
                            HIT_STATS (_outputHitsNumber += currentNbHits;)
                            (_client->*_method) (hit);
                            currentNbHits = hit->resetIndexes();
                        }
                    }
                }
            }

        }  /* end of for (size_t i=0; i<nb1; i++) */

    }  /* end of for (size_t j=0; j<nb2; j+=NB) */

    /** We are supposed to have computed scores for each hit, we can forward the information to the client.  */
    if (currentNbHits > 0)
    {
        HIT_STATS (_outputHitsNumber += currentNbHits;)
        (_client->*_method) (hit);
        currentNbHits = hit->resetIndexes();
    }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
dp::IProperties* UngapHitIteratorSIMD::getProperties ()
{
    dp::IProperties* result = AbstractHitIterator::getProperties ();

    /** We have to aggregate values from different split instances. */
    u_int64_t scoreKO           = _scoreKO;
    u_int64_t scoreOK           = _scoreOK;
    u_int64_t ungapKnownNumber  = _ungapKnownNumber;
    for (size_t i=0; i<_splitIterators.size(); i++)
    {
        UngapHitIteratorSIMD* current = dynamic_cast<UngapHitIteratorSIMD*> (_splitIterators[i]);
        if (current)
        {
            scoreKO          += current->_scoreKO;
            scoreOK          += current->_scoreOK;
            ungapKnownNumber += current->_ungapKnownNumber;
        }
    }

    result->add (1, "details");
    result->add (2, "score_ko",    "%ld",  scoreKO);
    result->add (2, "score_ok",    "%ld",  scoreOK);
    result->add (2, "known_ungap", "%ld",  ungapKnownNumber);

    /** We call the parent method in case we have split instances. */
    return result;
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
