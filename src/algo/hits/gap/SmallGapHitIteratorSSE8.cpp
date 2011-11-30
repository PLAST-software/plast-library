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

#include <misc/api/Vector.hpp>
#include <misc/api/macros.hpp>

#include <algo/hits/gap/SmallGapHitIteratorSSE8.hpp>

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
using namespace algo::align;

#include <stdio.h>
#define DEBUG(a)  //printf a

#define SIZEV 8

// Define a macro for optimized score retrieval through the vector-matrix.
#define getScore(i,j)  (_matrixAsVector [(i)+((j)<<5)])

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
SmallGapHitIteratorSSE8::SmallGapHitIteratorSSE8 (
    IHitIterator*         realIterator,
    ISeedModel*           model,
    IScoreMatrix*         scoreMatrix,
    IParameters*          parameters,
    IAlignmentResult*     ungapResult
)
    : AbstractPipeHitIterator (realIterator, model, scoreMatrix, parameters, ungapResult),
      _lowScoreNumber(0)
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
SmallGapHitIteratorSSE8::~SmallGapHitIteratorSSE8 ()
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
LETTER* SmallGapHitIteratorSSE8::getNeighbours1 (size_t n)
{
    if (n > _neighboursBuf1.size)  { _neighboursBuf1.resize(n);  }
    return _neighboursBuf1.data;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
LETTER* SmallGapHitIteratorSSE8::getNeighbours2 (size_t n)
{
    if (n > _neighboursBuf2.size)  { _neighboursBuf2.resize(n);  }
    return _neighboursBuf2.data;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
int* SmallGapHitIteratorSSE8::getComputedScores (size_t n)
{
    if (n > _computedScores.size)  { _computedScores.resize(n);  }
    return _computedScores.data;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void SmallGapHitIteratorSSE8::iterateMethod  (Hit* hit)
{
    DEBUG (("SmallGapHitIteratorSSE8::iterateMethod \n"));

    /** We may have nothing to do. */
    if (hit->indexes.empty())  { return; }

    /** Shortcuts. */
    const Vector<const ISeedOccurrence*>& occur1Vector = hit->occur1;
    const Vector<const ISeedOccurrence*>& occur2Vector = hit->occur2;

    /** Shortcuts. */
    size_t nbActualHits = hit->indexes.size ();
    size_t bandLength   = _parameters->smallGapBandLength;

    /** Statistics. */
    HIT_STATS (_inputHitsNumber += nbActualHits;)

    /** We need to extend the current neighbourhoods, so we create two arrays
     *  for holding these extended neighbourhoods. Note that, for a sequence,
     *  we store right and left neighbourhoods distinctly (which explain the
     *  factor 2 below for dimensioning vectors). */

    /** We need a vector holding the scores to be computed. */
    int* computedScores = getComputedScores (2*nbActualHits);

    /** We retrieve two big buffers that will hold all neighbourhoods for subject and query. */
    size_t neighbourhoodsSize = 2*nbActualHits*_parameters->smallGapBandLength;
    LETTER* neighboursBuf1 = getNeighbours1 (neighbourhoodsSize);
    LETTER* neighboursBuf2 = getNeighbours2 (neighbourhoodsSize);

    /** We need two cursors for iterating the two allocated neighbourhoods buffers. */
    LETTER* cursor1 = neighboursBuf1;
    LETTER* cursor2 = neighboursBuf2;

    LETTER* right1 = 0;
    LETTER* left1  = 0;
    LETTER* right2 = 0;
    LETTER* left2  = 0;

    for (list<IdxCouple>::iterator it = hit->indexes.begin();  it != hit->indexes.end();  it++)
    {
        /** We get references on neighbourhoods buffers to be extended. */
        right1 = cursor1;   cursor1 += bandLength;
        left1  = cursor1;   cursor1 += bandLength;

        right2 = cursor2;   cursor2 += bandLength;
        left2  = cursor2;   cursor2 += bandLength;

        /** We compute right and left neighbourhoods for both sequences. */
        extendNeighbourhood (occur1Vector.data[it->first],  right1, left1);
        extendNeighbourhood (occur2Vector.data[it->second], right2, left2);

#if 0
        printf ("occur1='%s'\n", occur1Vector.data[it->first]->sequence.toString().c_str() );
        printf ("occur2='%s'\n", occur2Vector.data[it->second]->sequence.toString().c_str() );
        const LETTER* convert = EncodingManager::singleton().getEncodingConversion(SUBSEED, ASCII);
        printf ("-> r1: offset=%ld  ", occur1Vector.data[it->first]->offsetInSequence);  for (size_t k=0; k<_parameters->smallGapBandLength; k++)  { printf ("%c", convert[right1[k]]); }  printf("\n");
        printf ("-> l1: offset=%ld  ", occur1Vector.data[it->first]->offsetInSequence);  for (size_t k=0; k<_parameters->smallGapBandLength; k++)  { printf ("%c", convert[left1[k]]); }  printf("\n");
        printf ("-> r2: offset=%ld  ", occur2Vector.data[it->second]->offsetInSequence);  for (size_t k=0; k<_parameters->smallGapBandLength; k++)  { printf ("%c", convert[right2[k]]); }  printf("\n");
        printf ("-> l2: offset=%ld  ", occur2Vector.data[it->second]->offsetInSequence);  for (size_t k=0; k<_parameters->smallGapBandLength; k++)  { printf ("%c", convert[left2[k]]); }  printf("\n");
        printf ("\n");
#endif
    }

    /** Now, 'neighbourhoods1' and 'neighbourhoods2' should hold all the wanted neighbourhoods. */
    computeScores (2*nbActualHits, neighboursBuf1, neighboursBuf2, computedScores);

    /** We check the scores versus the threshold. */
    size_t k=0;
    for (list<IdxCouple>::iterator it = hit->indexes.begin();  it != hit->indexes.end();  k++)
    {
        int score = computedScores[2*k+0] + computedScores[2*k+1];

        if (score >= _parameters->smallGapThreshold)
        {
            /** We increase the number of iterations. */
            _outputHitsNumber ++;

            /** We just continue the iteration. */
            it++;
        }
        else
        {
            _lowScoreNumber ++;

            /** We remove the current index couple. */
            it = hit->indexes.erase (it);
        }
    }

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
void SmallGapHitIteratorSSE8::extendNeighbourhood (const ISeedOccurrence* occur, LETTER* right, LETTER* left)
{
    /** Shortcut. */
    size_t neighbourLength = _parameters->smallGapBandLength;

    /** Shortcuts. */
    database::LETTER* bufIn  = occur->sequence.data.letters.data + occur->offsetInSequence;

    /** We fill the word with default letter. */
    memset (right, database::CODE_X, neighbourLength);
    memset (left,  database::CODE_X, neighbourLength);

    /** We fill the right neighbour. */
    memcpy (right, bufIn, MIN (neighbourLength,  occur->sequence.data.letters.size - occur->offsetInSequence));

    /** We fill the left neighbour. */
    size_t imin = MIN (neighbourLength, occur->offsetInSequence);
    database::LETTER* bufOut = left;
    while (imin-- > 0)  {  *(bufOut++) = *(--bufIn);  }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void SmallGapHitIteratorSSE8::computeScores (
    size_t nb,
    const LETTER* neighbourhoods1,
    const LETTER* neighbourhoods2,
    int* scores
)
{
    /** Shortcuts. */
    size_t neighbourLength = _parameters->smallGapBandLength;
    size_t neighbourWidth  = _parameters->smallGapBandWidth;

    __m128i vscore_gap_col;
    __m128i vscore_gap_row;
    __m128i vnext_score;
    __m128i vscore;
    __m128i vbest_score;
    __m128i vbest_gap_arr[neighbourLength];
    __m128i vbest_arr[neighbourLength];
    __m128i vgap_extend;
    __m128i vgap_open;
    __m128i vgap_open_extend;
    __m128i vscore_min;
    __m128i vtemp;
    __m128i vtemp1;

    /** We load gap extension penalty to all elements of a constant. */
    vgap_extend = _mm_set1_epi16 (-_parameters->extendGapCost);

    /** We load gap opening penalty to all elements of a constant. */
    vgap_open =  _mm_set1_epi16 (-_parameters->openGapCost);

    /** We load gap opening penalty to all elements of a constant. */
    vgap_open_extend =  _mm_set1_epi16 (-(_parameters->openGapCost + _parameters->extendGapCost));

    /* Load minimal score to all elements of a constant */
    vscore_min = _mm_set1_epi16 (-100);

    size_t k=0;
    size_t i=0;

    size_t first_b;
    size_t last_b;

    short temp_score [SIZEV];

    size_t hitIndex = 0;

    for (k=0; k /*+ SIZEV*/ < nb ; k+=SIZEV)
    {
        vbest_arr[0] = _mm_set1_epi16(0);
        //vbest_arr[0] = 0;

        _mm_store_si128(&vscore, vgap_open_extend);
        //vscore = -vgap_opend_extend;

        _mm_store_si128 (vbest_gap_arr, vgap_open_extend);
        //vbest_gap_arr[0] = -vgap_opend_extend;

        for (i=1; i<neighbourWidth; i++)
        {
            _mm_store_si128 (vbest_arr+i, vscore);
            //vbest_arr[i] = vscore;

            vtemp = _mm_adds_epi16 (vscore, vgap_open_extend);
            _mm_store_si128 (vbest_gap_arr+i, vtemp);
            //vbest_gap_arr[i] = vscore - vgap_open_extend;

            vscore = _mm_adds_epi16(vscore, vgap_extend);
            //vscore -= vgap_extend;
        }

        vbest_score = _mm_set1_epi16(0);
        //vbest_score = 0;

        first_b = 0;
        last_b  = neighbourWidth / 2;

        /** Shortcuts. */
        const LETTER* pt_A = 0;
        const LETTER* pt_B = 0;

        for (i=0; i<neighbourLength; i++)
        {
            _mm_store_si128 (&vscore_gap_row, vscore_min);
            _mm_store_si128 (&vscore, vscore_min);

            for (size_t j=first_b; j<last_b; j++)
            {
                _mm_store_si128 (&vscore_gap_col, *(vbest_gap_arr+j));

                //vscore_gap_col = vbest_gap_arr[j];

                //----------vnext_score = vbest_arr[j] + h_matrix[h_A[l + i]][h_B[l+j]];
                // vtemp = h_matrix[h_A[l + i]][h_B[l+j]]

                for (size_t l=0; l<SIZEV; l++)
                {
                    if (k+l<nb)
                    {
                        pt_A =  neighbourhoods1 + (k+l) * neighbourLength;
                        pt_B =  neighbourhoods2 + (k+l) * neighbourLength;

                        temp_score[l] = getScore (pt_A[i], pt_B[j]) ;
                    }
                    else
                    {
                        temp_score[l] = 0;
                    }
                }

                vtemp = _mm_set_epi16 (
                    temp_score[7],temp_score[6],temp_score[5],temp_score[4],
                    temp_score[3],temp_score[2],temp_score[1],temp_score[0]
                );

                //vtemp = _mm_set1_epi16(0);
                vtemp = _mm_adds_epi16(vtemp,*(vbest_arr+j));
                _mm_store_si128 (&vnext_score,vtemp);

                //--------    end vnext_score = vbest_arr[j] +  h_matrix ----//

                vscore = _mm_max_epi16 (vscore, vscore_gap_col);
                //if(vscore<vscore_gap_col) vscore = vscore_gap_col;

                vscore = _mm_max_epi16 (vscore, vscore_gap_row);
                //if(vscore<vscore_gap_row) vscore = vscore_gap_row;

                vbest_score = _mm_max_epi16 (vbest_score, vscore);
                //if(vbest_score<vscore) {
                //  vbest_score = vscore;
                //pos_extent = j;
                //}

                vscore_gap_col = _mm_adds_epi16 (vscore_gap_col, vgap_extend);
                //vscore_gap_col -= vgap_extend;

                vscore_gap_row = _mm_adds_epi16 (vscore_gap_row, vgap_extend);
                //vscore_gap_row -= vgap_extend;

                vtemp  = _mm_adds_epi16 (vscore, vgap_open_extend);
                vtemp1 = _mm_max_epi16  (vtemp,  vscore_gap_col);
                _mm_store_si128 (vbest_gap_arr+j,vtemp1);
                //vbest_gap_arr[j] = Max2(vscore - vgap_open_extend, vscore_gap_col);

                vscore_gap_row = _mm_max_epi16 (vtemp, vscore_gap_row);
                //vscore_gap_row = Max2(vscore - vgap_open_extend, vscore_gap_row);

                _mm_store_si128 (vbest_arr+j, vscore);
                //vbest_arr[j] = vscore;

                _mm_store_si128 (&vscore, vnext_score);
                //vscore = vnext_score;
            }

            if (i > 7)      first_b += 1;

            if (last_b < neighbourLength)
            {
                _mm_store_si128 (vbest_arr+last_b, vscore_gap_row);
                //vbest_arr[last_b] = vscore_gap_row;

                vtemp = _mm_adds_epi16 (vscore_gap_row, vgap_open_extend);
                _mm_store_si128 (vbest_gap_arr+last_b,vtemp);
                //vbest_gap_arr[last_b] = vscore_gap_row - vgap_open_extend;

                last_b += 1;
            }
        }

        if (hitIndex < nb)  scores[hitIndex++] =  _mm_extract_epi16 (vbest_score,0);
        if (hitIndex < nb)  scores[hitIndex++] =  _mm_extract_epi16 (vbest_score,1);
        if (hitIndex < nb)  scores[hitIndex++] =  _mm_extract_epi16 (vbest_score,2);
        if (hitIndex < nb)  scores[hitIndex++] =  _mm_extract_epi16 (vbest_score,3);
        if (hitIndex < nb)  scores[hitIndex++] =  _mm_extract_epi16 (vbest_score,4);
        if (hitIndex < nb)  scores[hitIndex++] =  _mm_extract_epi16 (vbest_score,5);
        if (hitIndex < nb)  scores[hitIndex++] =  _mm_extract_epi16 (vbest_score,6);
        if (hitIndex < nb)  scores[hitIndex++] =  _mm_extract_epi16 (vbest_score,7);

    } /* end of for (k=0; k < nb - SIZEV; k+=SIZEV) */
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
dp::IProperties* SmallGapHitIteratorSSE8::getProperties ()
{
    dp::IProperties* result = AbstractHitIterator::getProperties ();

    /** We have to aggregate values from different split instances. */
    u_int64_t lowScoreNumber = _lowScoreNumber;
    for (size_t i=0; i<_splitIterators.size(); i++)
    {
        SmallGapHitIteratorSSE8* current = dynamic_cast<SmallGapHitIteratorSSE8*> (_splitIterators[i]);
        if (current)
        {
            lowScoreNumber += current->_lowScoreNumber;
        }
    }

    result->add (1, "details");
    result->add (2, "score_ko", "%lld", lowScoreNumber);
    result->add (2, "score_ok", "%lld", getOutputHitsNumber());

    return result;
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
