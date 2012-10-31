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

#include <algo/hits/ungap/UngapHitIteratorSSE16.hpp>

#if __SSE3__
    #include <pmmintrin.h>
#else
#if __SSE2__
    #include <emmintrin.h>
#else
    #warning error undefined __SSE3__ or __SSE2__
#endif
#endif

using namespace std;
using namespace misc;
using namespace os;
using namespace os::impl;
using namespace database;
using namespace seed;
using namespace indexation;
using namespace algo::core;
using namespace alignment::core;

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
UngapHitIteratorSSE16::UngapHitIteratorSSE16 (
    IHitIterator*        realIterator,
    ISeedModel*          model,
    IScoreMatrix*        scoreMatrix,
    IParameters*         parameters,
    IAlignmentContainer* ungapResult,
    u_int32_t            maxHitsPerIteration,
    bool&                isRunning
)
    : AbstractPipeHitIterator (realIterator, model, scoreMatrix, parameters, ungapResult),
      _ungapKnownNumber(0), _databk(0), _isRunning (isRunning)
{
    DEBUG (("UngapHitIteratorSSE16::UngapHitIteratorSSE16:  span=%ld  _neighbourLength=%d \n",
        _model->getSpan(),
        _neighbourLength
    ));

    /** We overide the default value. */
    if (maxHitsPerIteration > 0)  { _maxHitsPerIteration = maxHitsPerIteration; }

    size_t sizeMatrix        = _scoreMatrix->getN();
    size_t sizeNeighbour     = _span + 2*_parameters->ungapNeighbourLength;

    /** We allocate some memory space for inner processing. */
    _databk = (char *) DefaultFactory::memory().calloc (sizeMatrix*sizeNeighbour + 64,  sizeof(__m128i));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
UngapHitIteratorSSE16::~UngapHitIteratorSSE16 ()
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
void UngapHitIteratorSSE16::iterateMethod (Hit* hit)
{
    HIT_STATS_VERBOSE (_iterateMethodNbCalls++);

    __m128i *pvb;
    __m128i pvScore;
    __m128i vscore;
    __m128i vMaxScore;

    u_int8_t sizeMatrix        = _scoreMatrix->getN();
    u_int8_t sizeNeighbour     = _span + 2*_parameters->ungapNeighbourLength;
    u_int8_t sizeHalfNeighbour = _span + 1*_parameters->ungapNeighbourLength;

    u_int32_t currentNbHits = hit->size();

    char* databk = _databk;

    size_t aligned = ((size_t) (databk + 0x0f)) & ~(0x0f);
    pvb = (__m128i *) aligned;

    short   bias  = _scoreMatrix->getDefaultScore();
    __m128i vBias = _mm_set1_epi8 (-bias);

    /** Shortcuts. */
    const Vector<const ISeedOccurrence*>& occur1Vector = hit->occur1;
    const Vector<const ISeedOccurrence*>& occur2Vector = hit->occur2;
    const ISeedOccurrence* occur1ForIndexI = 0;

    size_t nb1 = occur1Vector.size;
    size_t nb2 = occur2Vector.size;

    __m128i vThreshold  = _mm_set1_epi8 (_parameters->ungapScoreThreshold - 1);
    __m128i vThreshold2 = _mm_set1_epi8 (_parameters->ungapScoreThreshold);

    /** Statistics. */
    HIT_STATS (_inputHitsNumber += nb1 * nb2;)

    /** We retrieve the whole neighbourhoods buffer. This is important to get the full buffer
     * for locality concerns (ie. for optimizing CPU cache usage). */
    const LETTER* neighboursOccur2 = hit->neighbourhoodsOccur2;

    /** We loop over query occurrences. */
    for (size_t j=0; _isRunning && j<nb2; j+=NB)
    {
        /** We first reset to 0 all scores of the pseudo SSE score matrix. */
        memset (databk, 0, (sizeMatrix*sizeNeighbour + 64) * sizeof(__m128i));

        char* pc = (char *) pvb;

        u_int8_t lmin  = MIN (nb2 - j, NB);
        u_int8_t delta = (NB - lmin);

        for (u_int8_t m=0; m<sizeMatrix; m++)
        {
            int8_t* matrixRow = _matrix[m];

            for (u_int8_t k=0; k<sizeNeighbour; k++)
            {
                for (u_int8_t l=0; l<lmin; l++)
                {
                    *pc++ = (char) (matrixRow [(int) neighboursOccur2[(j+l)*sizeNeighbour+k]] - bias);
                }

                /** We may have to skip some null scores (in case lmin > NB). */
                pc += delta;
            }
        }

        /** We loop over subject occurrences. */
        for (size_t i=0; i<nb1; i++)
        {
            /** We use a shortcut (avoids several memory accesses). */
            occur1ForIndexI = occur1Vector.data[i];

            LETTER* neighbour1 = occur1ForIndexI->neighbourhood.letters.data;

            vMaxScore = _mm_set1_epi8 (0);
            vscore    = _mm_set1_epi8 (0);

            for (u_int8_t k=0; k<sizeHalfNeighbour; k++)
            {
                pvScore   = *(pvb + (neighbour1[k] * sizeNeighbour + k));
                vscore    = _mm_adds_epu8 (vscore,    pvScore);
                vscore    = _mm_subs_epu8 (vscore,    vBias);
                vMaxScore = _mm_max_epu8  (vMaxScore, vscore);
            }

            _mm_store_si128 (&vscore,vMaxScore);

            for (u_int8_t k=sizeHalfNeighbour; k<sizeNeighbour; k++)
            {
                pvScore   = *(pvb + (neighbour1[k] * sizeNeighbour + k));
                vscore    = _mm_adds_epu8 (vscore,    pvScore);
                vscore    = _mm_subs_epu8 (vscore,    vBias);
                vMaxScore = _mm_max_epu8  (vMaxScore, vscore);
            }

            /** Note the trick here: we first re-calibrate the computed max score
             *  in order to get rid of possible saturations. */
            vMaxScore = _mm_min_epu8 (vMaxScore, vThreshold2);

            /** We compare the max score to the wanted threshold.
             *  We get the test result as a mask of 16 bits.
             *  For each bit, 1 means that the score passed the threshold test. */
            u_int16_t maskTestThreshold = _mm_movemask_epi8 (_mm_cmpgt_epi8 (vMaxScore, vThreshold));

            /** Possible optimization: just continue if we know that every threshold tests failed. */
            if (maskTestThreshold == 0)  {  continue; }

            for (u_int8_t k=0; k<NB; k++)
            {
                /** If the value is 0, it means that the two comparisons failed the threshold test. */
                if ( (maskTestThreshold >> k) & 0x1)
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
dp::IProperties* UngapHitIteratorSSE16::getProperties ()
{
    dp::IProperties* result = AbstractHitIterator::getProperties ();

    /** We have to aggregate values from different split instances. */
    u_int64_t scoreKO           = _scoreKO;
    u_int64_t scoreOK           = _scoreOK;
    u_int64_t ungapKnownNumber  = _ungapKnownNumber;
    for (size_t i=0; i<_splitIterators.size(); i++)
    {
        UngapHitIteratorSSE16* current = dynamic_cast<UngapHitIteratorSSE16*> (_splitIterators[i]);
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
