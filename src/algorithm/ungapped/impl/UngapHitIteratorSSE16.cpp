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

#include "UngapHitIteratorSSE16.hpp"

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
using namespace os;
using namespace database;
using namespace seed;
using namespace indexation;

#include <stdio.h>
#define DEBUG(a)  //printf a

#define MIN(a,b)  ((a) < (b) ? (a) : (b))
#define MAX(a,b)  ((a) < (b) ? (b) : (a))

/********************************************************************************/
namespace algo  {
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
    IHitIterator*       realIterator,
    ISeedModel*         model,
    IScoreMatrix*       scoreMatrix,
    IParameters*        parameters,
    IAlignmentResult*   ungapResult
)
    : UngapHitIterator (realIterator, model, scoreMatrix, parameters, ungapResult),
      _ungapKnownNumber(0)
{
    DEBUG (("UngapHitIteratorSSE16::UngapHitIteratorSSE16:  span=%ld  _neighbourLength=%d \n",
        _model->getSpan(),
        _neighbourLength
    ));
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
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void UngapHitIteratorSSE16::iterateMethod (indexation::Hit* hit)
{
    __m128i *pvb;
    __m128i pvScore;
    __m128i vscore;
    __m128i vMaxScore;
    __m128i vBias;

    size_t sizeMatrix        = _scoreMatrix->getN();
    size_t sizeNeighbour     = _span + 2*_parameters->ungapNeighbourLength;
    size_t sizeHalfNeighbour = _span + 1*_parameters->ungapNeighbourLength;

    char* databk = (char *) MemoryAllocator::singleton().calloc (sizeMatrix*sizeNeighbour + 64,  sizeof(__m128i));

    size_t aligned = ((size_t) (databk +15)) & ~(0x0f);
    pvb = (__m128i *) aligned;

    short          bias  = _scoreMatrix->getDefaultScore();
    unsigned short rbias = -bias;

    int score_arr [NB/2];  memset (score_arr,0,sizeof(score_arr));

    int dup = (rbias << 8) | (rbias & 0x00ff);
    vBias = _mm_insert_epi16    (vBias, dup, 0);
    vBias = _mm_shufflelo_epi16 (vBias, 0);
    vBias = _mm_shuffle_epi32   (vBias, 0);

    /** Shortcuts. */
    const Vector<const ISeedOccurrence*>& occur1Vector = hit->occur1;
    const Vector<const ISeedOccurrence*>& occur2Vector = hit->occur2;

    size_t nb1 = occur1Vector.size;
    size_t nb2 = occur2Vector.size;

    int actualThreshold = _parameters->ungapScoreThreshold;

    /** Statistics. */
    HIT_STATS (_inputHitsNumber += nb1 * nb2;)

    /** We loop over query occurrences. */
    for (size_t j=0; j<nb2; j+=NB)
    {
        /** We first reset to 0 all scores of the pseudo SSE score matrix. */
        memset (databk, 0, (sizeMatrix*sizeNeighbour + 64) * sizeof(__m128i));

        char* pc = (char *) pvb;

        int lmin = MIN (nb2-j, NB);

        for (size_t i=0; i<sizeMatrix; i++)
        {
            int8_t* matrixRow = _matrix[i];

            for (size_t k=0; k<sizeNeighbour; k++)
            {
                for (int l=0; l<lmin; l++)
                {
                    LETTER* occurQuery = occur2Vector.data[j+l]->neighbourhood.letters.data;
                    *pc++ = (char) (matrixRow [(int)occurQuery[k]] - bias);
                }

                /** We may have to skip some null scores. */
                pc += (NB - lmin);
            }
        }

        /** We loop over subject occurrences. */
        for (size_t i=0; i<nb1; i++)
        {
            vMaxScore = _mm_xor_si128 (vMaxScore, vMaxScore);
            vscore    = _mm_xor_si128 (vscore, vscore);

            LETTER* neighbour1 = occur1Vector.data[i]->neighbourhood.letters.data;

            for (size_t k=0; k<sizeHalfNeighbour; k++)
            {
                pvScore   = *(pvb + (neighbour1[k] * sizeNeighbour + k));
                vscore    = _mm_adds_epu8 (vscore, pvScore);
                vscore    = _mm_subs_epu8 (vscore, vBias);
                vMaxScore = _mm_max_epu8 (vMaxScore, vscore);
            }

            _mm_store_si128 (&vscore,vMaxScore);

            for (size_t k=sizeHalfNeighbour; k<sizeNeighbour; k++)
            {
                pvScore   = *(pvb + (neighbour1[k] * sizeNeighbour + k));
                vscore    = _mm_adds_epu8 (vscore, pvScore);
                vscore    = _mm_subs_epu8 (vscore, vBias);
                vMaxScore = _mm_max_epu8  (vMaxScore, vscore);
            }

            score_arr[0] =  _mm_extract_epi16 (vMaxScore,0);
            score_arr[1] =  _mm_extract_epi16 (vMaxScore,1);
            score_arr[2] =  _mm_extract_epi16 (vMaxScore,2);
            score_arr[3] =  _mm_extract_epi16 (vMaxScore,3);
            score_arr[4] =  _mm_extract_epi16 (vMaxScore,4);
            score_arr[5] =  _mm_extract_epi16 (vMaxScore,5);
            score_arr[6] =  _mm_extract_epi16 (vMaxScore,6);
            score_arr[7] =  _mm_extract_epi16 (vMaxScore,7);

            for (size_t k=0; k<NB/2; k++)
            {
                int finalScore = 0;
                bool isNotKnown = true;

                if (j+2*k+0 < nb2)
                {
                    finalScore = ( (score_arr[k] >> 0)  & 0x00ff) ;
                    if (finalScore >= actualThreshold)
                    {
                        HIT_STATS (_scoreOK ++;)

#if 1
                        isNotKnown =  (_ungapResult==0) ||
                            (_ungapResult!=0 && _ungapResult->doesExist(occur1Vector.data[i], occur2Vector.data[j+2*k+0]) == false);
#else
                        isNotKnown = true;
#endif
                        if (isNotKnown)
                        {
                            /** We increase the number of iterations. */
                            HIT_STATS (_outputHitsNumber ++;)

                            hit->addIndexes (i, j+2*k+0);
                        }
                        else
                        {
                            HIT_STATS (_ungapKnownNumber ++;)
                        }
                    }
                    else
                    {
                        HIT_STATS (_scoreKO ++;)
                    }
                }

                if (j+2*k+1 < nb2)
                {
                    finalScore = ( (score_arr[k] >> 8)  & 0x00ff) ;
                    if (finalScore >= actualThreshold)
                    {
                        HIT_STATS (_scoreOK ++;)

#if 1
                        isNotKnown = (_ungapResult==0) ||
                            (_ungapResult!=0 && _ungapResult->doesExist(occur1Vector.data[i], occur2Vector.data[j+2*k+1]) == false);
#else
                        isNotKnown = true;
#endif
                        if (isNotKnown)
                        {
                            HIT_STATS (_outputHitsNumber ++;)

                            /** We tag this hit to be used for further processing. */
                            hit->addIndexes (i, j+2*k+1);
                        }
                        else
                        {
                            HIT_STATS (_ungapKnownNumber ++;)
                        }
                    }
                    else
                    {
                        HIT_STATS (_scoreKO ++;)
                    }
                }

            }  /* end of for (size_t k=0; k<8; k++) */

#if 0
        /** We may want to go further in the algorithm with the currently decent hits. */
        if (hit->indexes.size() >= 16)
        {
            (_client->*_method) (hit);
            hit->resetIndexes();
        }
#endif

        }  /* end of for (size_t i=0; i<nb1; i++) */

#if 0
        /** We may want to go further in the algorithm with the currently decent hits. */
        if (hit->indexes.size() >= 100)
        {
            (_client->*_method) (hit);
            hit->resetIndexes();
        }
#endif

    }  /* end of for (size_t j=0; j<nb2; j+=NB) */

    /** We are supposed to have computed scores for each hit,
     *  we can forward the information to the client.  */
    (_client->*_method) (hit);

    /** Some clean up. */
    MemoryAllocator::singleton().free (databk);
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
} /* end of namespaces. */
/********************************************************************************/
