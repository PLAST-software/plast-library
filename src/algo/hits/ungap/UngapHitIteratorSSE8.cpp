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

#include <algo/hits/ungap/UngapHitIteratorSSE8.hpp>

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
UngapHitIteratorSSE8::UngapHitIteratorSSE8 (
    IHitIterator*        realIterator,
    ISeedModel*          model,
    IScoreMatrix*        scoreMatrix,
    IParameters*         parameters,
    IAlignmentContainer* ungapResult
)
    : AbstractPipeHitIterator (realIterator, model, scoreMatrix, parameters, ungapResult)
{
    DEBUG (("UngapHitIteratorSSE::UngapHitIteratorSSE:  span=%ld  _neighbourLength=%d \n",
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
UngapHitIteratorSSE8::~UngapHitIteratorSSE8 ()
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
void UngapHitIteratorSSE8::iterateMethod (Hit* hit)
{
    __m128i *pvb;
    __m128i pvScore;
    __m128i vscore;
    __m128i vMaxScore;

    /** Some shortcuts. */
    size_t sizeMatrix        = _scoreMatrix->getN();
    size_t sizeNeighbour     = _span + 2*_parameters->ungapNeighbourLength;
    size_t sizeHalfNeighbour = _span + 1*_parameters->ungapNeighbourLength;

    /** We allocate the pseudo score matrix between alphabet and database 2. */
    char* databk = (char *) DefaultFactory::memory().calloc (sizeMatrix*sizeNeighbour + 64,  sizeof(__m128i));
    size_t aligned = ((size_t) (databk +15)) & ~(0x0f);
    pvb = (__m128i *) aligned;

    short score_arr [NB];  memset (score_arr,0,sizeof(score_arr));

    /** Shortcuts. */
    const Vector<const ISeedOccurrence*>& occur1Vector = hit->occur1;
    const Vector<const ISeedOccurrence*>& occur2Vector = hit->occur2;

    size_t nb1 = occur1Vector.size;
    size_t nb2 = occur2Vector.size;

    /** We loop over all occurrence in database 2.
     *  Note that we increase each time by 8 due to the SSE vectorization scheme.
     */
    for (size_t j=0; j<nb2; j+=NB)
    {
        int16_t* pc = (int16_t*) pvb;

        /** We build the vector of __m128i objects that provides the scores for each letter of the
         *  neighbourhood of 8 occurrences for each possible letter of the alphabet.
         *  Note that nb2 may be not a multiple of 8; if not, we fill the vector with a null score
         *  because these are fake data and so must not change the maximum score.
         */
        for (size_t n=0; n<sizeMatrix; n++)
        {
            int8_t* matrixRow = _matrix[n];

            for (size_t k=0; k<sizeNeighbour; k++)
            {
                for (size_t l=0; l<NB; l++)
                {
                    /** We test whether nb2 is a multiple of 8; we do that by checking that the nth occurrence
                     *  (that we are going to use for the vectorization scheme) belongs to the database 2.  */
                    if (j+l < nb2)
                    {
                        /** We retrieve the current neighbourhood. */
                        LETTER* neighbour2 = occur2Vector.data[j+l]->neighbourhood.letters.data;

                        /** We set a 2 bytes score for the current 'k' alphabet letter and
                         * the 'k'th letter of the neighbourhood. */
                        *pc++ = (int16_t) (matrixRow [(int)neighbour2[k]]);
                    }
                    else
                    {
                        /** We fill with null score. */
                        *pc++ = 0;
                    }
                }
            }
        }

        /** Now, we have build a pseudo score matrix between 8 neighbourhoods and all possible alphabet letters.  */

        /** We loop over all occurrences in database 1. */
        for (size_t i=0; i<nb1; i++)
        {
            /** We retrieve the neighbourhood of the ith occurrence in database 1. */
            LETTER* neighbour1 = occur1Vector.data[i]->neighbourhood.letters.data;

            /** We initialize score computation variables. */
            vMaxScore = _mm_setzero_si128 ();
            vscore    = _mm_setzero_si128 ();

            /** We compute the score for seed and right neighbourhood. */
            for (size_t k=0; k<sizeHalfNeighbour; k++)
            {
                pvScore   = *(pvb + (neighbour1[k] * sizeNeighbour + k));
                vscore    = _mm_add_epi16 (vscore, pvScore);
                vMaxScore = _mm_max_epi16 (vMaxScore, vscore);
            }

            _mm_store_si128 (&vscore,vMaxScore);

            /** We compute the score for left neighbourhood. */
            for (size_t k=sizeHalfNeighbour; k<sizeNeighbour; k++)
            {
                pvScore   = *(pvb + (neighbour1[k] * sizeNeighbour + k));
                vscore    = _mm_add_epi16 (vscore, pvScore);
                vMaxScore = _mm_max_epi16  (vMaxScore, vscore);
            }

            /** We extract 8 scores from the 128 bits. */
            score_arr[0] =  _mm_extract_epi16 (vMaxScore,0);
            score_arr[1] =  _mm_extract_epi16 (vMaxScore,1);
            score_arr[2] =  _mm_extract_epi16 (vMaxScore,2);
            score_arr[3] =  _mm_extract_epi16 (vMaxScore,3);
            score_arr[4] =  _mm_extract_epi16 (vMaxScore,4);
            score_arr[5] =  _mm_extract_epi16 (vMaxScore,5);
            score_arr[6] =  _mm_extract_epi16 (vMaxScore,6);
            score_arr[7] =  _mm_extract_epi16 (vMaxScore,7);

            /** We compare each computed score with the given score threshold. */
            for (size_t k=0; k<8; k++)
            {
                if (score_arr[k] >= _parameters->ungapScoreThreshold)
                {
                    /** We increase the number of iterations. */
                    _outputHitsNumber++;

                    if (j+k < nb2)
                    {
                        hit->addIndexes (i, j+k);
                    }
                }

            }  /* end of for (size_t k=0; k<8; k++) */

        }  /* end of for (size_t i=0; i<nb1; i++) */

    }  /* end of for (size_t j=0; j<nb2; j+=NB) */

    /** We are supposed to have computed scores for each hit,
     *  we can forward the information to the client.  */
    (_client->*_method) (hit);

    /** Some clean up. */
    DefaultFactory::memory().free (databk);
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
