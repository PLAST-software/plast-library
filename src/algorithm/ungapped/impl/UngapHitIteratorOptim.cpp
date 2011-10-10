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

#if 0

#include "UngapHitIteratorOptim.hpp"

using namespace std;
using namespace os;
using namespace database;
using namespace seed;
using namespace indexation;

#include <stdio.h>
#define DEBUG(a)  printf a

#define MIN(a,b,c)  ((a) < (b) && (a) < (c) ? (a) : ((b) < (c) ? (b) : (c)  ))

// Define a macro for optimized score retrieval through the vector-matrix.
#define getScore(i,j)  (_matrixAsVector [(i)+((j)<<5)])

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
UngapHitIteratorOptim::UngapHitIteratorOptim (
    IHitIterator*   realIterator,
    ISeedModel*     model,
    IScoreMatrix*   scoreMatrix,
    u_int16_t       neighbourLength,
    u_int16_t       threshold
)
    : UngapHitIterator (realIterator, model, scoreMatrix, neighbourLength, threshold)
{
    size_t N=20;

//    _matrix2AsVector = (MatrixEntry*) os::MemoryAllocator::singleton().calloc (N*N*N*N, sizeof(MatrixEntry));
    memset (_matrix2AsVector, 0, sizeof(MatrixEntry)*N*N*N*N);

    int8_t rbias = 4;
    size_t nbPositives = 0;

    for (size_t a0=0; a0<N; a0++)
    {
        for (size_t b0=0; b0<N; b0++)
        {
            int8_t score0 = _matrix[a0][b0]; // getScore (a0,b0);

            for (size_t a1=0; a1<N; a1++)
            {
                for (size_t b1=0; b1<N; b1++)
                {
                    int8_t score1 = _matrix[a1][b1]; //getScore (a1,b1);

//                    MatrixEntry* entry = & (_matrix2AsVector[nbTotal++]);
                    MatrixEntry* entry = & (_matrix2AsVector[a0][b0][a1][b1]);

                    bool positive = (score0>0) || (score1>0);

                    *entry = ((score0+rbias)<<0) + ((score1+rbias)<<8) + ((score0+rbias+score1+rbias)<<16) + (positive ? (1<<24) : 0);
                }
            }
        }
    }

    DEBUG (("UngapHitIteratorOptim::UngapHitIteratorOptim:  neighbourLength=%d  threshold=%d  ratioPositives=%.3f\n",
        neighbourLength, threshold, (float)nbPositives / (float) (32*32*32*32)
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
void UngapHitIteratorOptim::iterateMethod (const indexation::Hit* hit)
{
    //MatrixEntry* matrix = _matrix2AsVector;

    const ISequence& s1         = hit->occur1.data[0]->sequence;
    u_int32_t offsetInSequence1 = hit->occur1.data[0]->offsetInSequence;

    const ISequence& s2         = hit->occur2.data[0]->sequence;
    u_int32_t offsetInSequence2 = hit->occur2.data[0]->offsetInSequence;

    LETTER* buf1 = s1.data.letters.data + offsetInSequence1;
    LETTER* buf2 = s2.data.letters.data + offsetInSequence2;

    LETTER* loop1 = 0;
    LETTER* loop2 = 0;

    int16_t score    = 0;
    int16_t scoreMax = 0;

    /******************************  SEED + RIGHT  ******************************/
    size_t imax = MIN (
        _span+_neighbourLength,
        s1.data.letters.size-offsetInSequence1,
        s2.data.letters.size-offsetInSequence2
    );

    score    = 0;
    scoreMax = 0;
    loop1 = buf1;  loop2 = buf2;

    /** */
    for (size_t i=0; i<imax/2; i++)
    {
//        u_int32_t offset = (loop1[0]<<15) + (loop2[0]<<10) + (loop1[1]<<5) + (loop2[1]<<0);
//        MatrixEntry entry = *(matrix + offset);
        MatrixEntry entry = _matrix2AsVector [(int)loop1[0]] [(int)loop2[0]] [(int)loop1[1]] [(int)loop2[1]];

        loop1 += 2;  loop2 += 2;


        if (entry & (1<<24))
        {
            score += ((entry>>0) & 0xFF) - 4;    if (score > scoreMax)  { scoreMax = score; }
            score += ((entry>>8) & 0xFF) - 4;    if (score > scoreMax)  { scoreMax = score; }
        }
        else
        {
            score += ((entry>>16) & 0xFF) - 8;
        }
    }

    /** We may have an extra iteration if imax is even. */
    if (imax & 1)
    {
        score += getScore (*(loop1++), *(loop2++));
        if (score > scoreMax)  { scoreMax = score; }
    }


    /******************************  LEFT  ******************************/
    imax = MIN (
        _neighbourLength,
        offsetInSequence1,
        offsetInSequence2
    );

    score = scoreMax;
    loop1 = buf1-1;  loop2 = buf2-1;

    /** */
    for (size_t i=0; i<imax/2; i++)
    {
//        u_int32_t offset = (loop1[0]<<15) + (loop2[0]<<10) + (loop1[-1]<<5) + (loop2[-1]<<0);
//        MatrixEntry entry = *(matrix + offset);
        MatrixEntry entry = _matrix2AsVector [(int)loop1[0]] [(int)loop2[0]] [(int)loop1[-1]] [(int)loop2[-1]];

        loop1 -= 2;  loop2 -= 2;


        if (entry & (1<<24))
        {
            score += ((entry>>0) & 0xFF) - 4;    if (score > scoreMax)  { scoreMax = score; }
            score += ((entry>>8) & 0xFF) - 4;    if (score > scoreMax)  { scoreMax = score; }
        }
        else
        {
            score += ((entry>>16) & 0xFF) - 8;
        }
    }

    /** We may have an extra iteration if imax is even. */
    if (imax & 1)
    {
        score += getScore (*(loop1--), *(loop2--));
        if (score > scoreMax)  { scoreMax = score; }
    }

    /** We may call the callback if we got a decent score. */
    if (scoreMax >= _threshold)   {  (_client->*_method) (hit);  }
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
#endif
