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

#include <designpattern/impl/RangeIterator.hpp>

#include <alignment/tools/impl/AlignmentSplitterBanded.hpp>

#include <iostream>

#include <stdio.h>
#define DEBUG(a)  //printf a
#define INFO(a)   printf a

using namespace std;
using namespace database;
using namespace dp;
using namespace os;
using namespace os::impl;
using namespace algo::core;
using namespace alignment::core;

#define ROW(vector,i,delta)     ((vector) + (2*(i)+1)*(delta))
#define NEXT(vector,delta)      ((vector) + 2*(delta))
#define PREVIOUS(vector,delta)  ((vector) - 2*(delta))
#define CELL(vector,i,j,delta)  (ROW (vector,i,delta) [j])

#define MINFTY  -9999

/********************************************************************************/
namespace alignment {
namespace tools     {
namespace impl      {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AlignmentSplitterBanded::AlignmentSplitterBanded (IScoreMatrix* scoreMatrix, int openGapCost, int extendGapCost)
 : AlignmentSplitter (scoreMatrix, openGapCost, extendGapCost),
   _vector_H (0), _vector_E(0), _vector_F(0)
{
    /** We set a 0 default value for the 3 vectors size. It will force to allocate them with decent size at the
     * first need. */
    _DefaultAlignSize = 0;

    /** We set a maximum size for the 3 vectors allocations. Note that the AlignmentSplitter class uses N*N matrix
     * allocations, so its _MaxAlignSize can't be so big. Here, we use vectors (and not matrixes) since
     * the underlying matrixes are sparse (banded in other words). */
    _MaxAlignSize = 50*1024*1024;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AlignmentSplitterBanded::~AlignmentSplitterBanded ()
{
    DEBUG (("AlignmentSplitterBanded::~AlignmentSplitterBanded : _DefaultAlignSize=%d\n", _DefaultAlignSize));

    DefaultFactory::memory().free (_vector_H);
    DefaultFactory::memory().free (_vector_E);
    DefaultFactory::memory().free (_vector_F);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool AlignmentSplitterBanded::allocVectors (int newSize)
{
    bool result = false;

    DEBUG (("AlignmentSplitterBanded::allocVectors : this=%p _DefaultAlignSize=%d  _MaxAlignSize=%d  newSize=%d \n",
        this, _DefaultAlignSize, _MaxAlignSize, newSize
    ));

    if (newSize < _MaxAlignSize)
    {
        _DefaultAlignSize = newSize;

        _vector_H = (int16_t*) DefaultFactory::memory().realloc (_vector_H, _DefaultAlignSize*sizeof (int16_t));
        _vector_E = (int16_t*) DefaultFactory::memory().realloc (_vector_E, _DefaultAlignSize*sizeof (int16_t));
        _vector_F = (int16_t*) DefaultFactory::memory().realloc (_vector_F, _DefaultAlignSize*sizeof (int16_t));

        int16_t val = MINFTY;
        for (int i=0; i<_DefaultAlignSize; i++)  { _vector_H[i] = val; }
        for (int i=0; i<_DefaultAlignSize; i++)  { _vector_E[i] = val; }
        for (int i=0; i<_DefaultAlignSize; i++)  { _vector_F[i] = val; }

        result = true;
    }
    else
    {
        DEBUG (("AlignmentSplitterBanded::allocVectors:  newSize=%d  _MaxAlignSize=%d\n", newSize, _MaxAlignSize));
    }

    return result;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
size_t AlignmentSplitterBanded::splitAlign (
    const database::LETTER* sbjSeq,
    const database::LETTER* qrySeq,
    const misc::Range32&    sbjRange,
    const misc::Range32&    qryRange,
    SplitOutput& output
)
{
    /** Extract from "Pairwise sequence alignment for very long sequences on GPUs", Junjie Li, Sanjay Ranka and Sartaj Sahni
     *
     * α be the gap opening penalty, and β the gap extension penalty => the penalty for a gap of length k is α+kβ
     *
     *  -> H[i][j] is the score of the best local alignment for (q1 ...qi ) and (s1 ...sj ) that ends at positions i and j,
     *     respectively, of the two sequences.
     *
     *  -> E[i,j] is the score of the best local alignment for (q1 ...qi ) and (s1 ...sj ) under the constraint that qi is
     *     aligned to a gap.
     *
     *  -> F[i,j] is the score of the best local alignment for (q1 ...qi ) and (s1 ...sj ) under the constraint that sj is
     *     aligned to a gap.
     *
     *     The initial conditions are:
     *          H[0,0] = H[i,0] = H[0,j] = 0;
     *          E[0,0] = −∞; E[i,0] = −α−iβ; E[0,j] = −∞;
     *          F[0,0] = −∞; F[i,0] = −∞;    F[0,j] = −α − jβ;      1 ≤ i ≤ m, 1 ≤ j ≤ n.
     */

    int s, alignLength, nbHspOpen, i, j, isHspOpened;

    /** Note: we have to add 1 to the band width because we need to have fence values (like -1000)
     *  to limit the band in the dynamic programming. */
    int delta = ABS (qryRange.getLength() - sbjRange.getLength()) + _band + 1;

    /** Shortcuts. */
    int8_t** MATRIX = _scoreMatrix->getMatrix();

    int qryLen = qryRange.getLength();
    int subLen = sbjRange.getLength();

    const char* qryStr = qrySeq + qryRange.begin;
    const char* subStr = sbjSeq + sbjRange.begin;

    int        splitVectorSize = output.splittab.size;
    u_int32_t* splittab        = output.splittab.data;

#if 0
    const LETTER* convert = EncodingManager::singleton().getEncodingConversion(SUBSEED, ASCII);
    printf ("QRY: "); for (int ii=0; ii<qryLen; ii++)  { printf ("%c", convert[(int)qryStr[ii]]);  }  printf("\n\n");
    printf ("SBJ: "); for (int ii=0; ii<subLen; ii++)  { printf ("%c", convert[(int)subStr[ii]]);  }  printf("\n\n");
#endif

    int opengap   = _openGapCost;
    int extendgap = _extendGapCost;

    /** We compute the required size of the containers used for processing the dynamic programming. */
    int requiredLen = (qryLen+1) * (2*delta+1);

    /** We check whether the current allocation size is big enough. */
    if (requiredLen > _DefaultAlignSize)
    {
        /** We may ask too much memory. In such a case, we just return from the method with a 0 size value. */
        if (allocVectors (requiredLen) == false)  { return 0; }
    }

    int16_t* H0 = ROW (_vector_H, 0, delta);
    int16_t* E0 = ROW (_vector_E, 0, delta);
    int16_t* F0 = ROW (_vector_F, 0, delta);

    int16_t* H1 = NEXT (H0, delta);
    int16_t* E1 = NEXT (E0, delta);
    int16_t* F1 = NEXT (F0, delta);

    for (i=0; i<=delta; i++)
    {
        H0[i]=0; F0[i]=0;
    }

    for (i=1; i<=qryLen; i++)
    {
        /** We retrieve the score vector for the current query letter. */
        int8_t* matrixrow = MATRIX [(int)qryStr[i-1]];

        /** Note +1 and -1, used for tagging with -1000 the most left and right diagonals. */
        int j1 = MAX (1,      i-delta+1);
        int j2 = MIN (subLen, i+delta-1);

        /** Here, we put the fences on the most left and right diagonals. */
        H1 [j1-1] = -1000;
        E1 [j1-1] = -1000;
        H0 [j2]   = -1000;
        F0 [j2]   = -1000;

        /** We use some temporary variables in order to avoid some memory access
         *  (which is time costly) and can't be easily optimized by compiler.
         */
        int16_t e1 = E1 [j1-1];
        int16_t h1 = H1 [j1-1];
        int16_t f1 = 0;

        /** The inner loop should be at most of length L:
         *   L = (i+delta-1) - (i-delta+1)
         *     = 2*delta - 2
         *     = 2.ABS(qryLen - sbjLen) + 2._band
         *
         *  WARNING ! such inner loop does not allow to get all actual gaps
         *  found in the previous step (by a SemiGapAlign object for instance), step
         *  that provided only the alignments bounds. Here, the space search is restricted
         *  to some constant bandwidth (ie. '_band' attribute) plus the difference between
         *  the query and subject lengths.
         *  Consider the following sample: an alignment holds a gap of size 10 in the query
         *  and also a gap of size 10 in the subject. As a result, the query and subject lengths
         *  are the same but our inner loop on j can search only for gaps of size '_band'.
         *  An improvement would be to dynamically provide the "true" value for '_band', value
         *  found by the SemiGapAlign object during the previous step.
         *  Since we don't explore the true search space, we could have some differences with blast
         *  on values like identity percentage, number of mismatches and number of gaps, although
         *  the alignment bounds would be the same between the two tools.
         */
        for (j=j1; j<=j2; j++)
        {
            E1[j] = e1 = MAX (h1    - opengap, e1    - extendgap);
            F1[j] = f1 = MAX (H0[j] - opengap, F0[j] - extendgap);

            h1 = H0[j-1] + matrixrow [(int)subStr[j-1]];

            if (e1 > h1)  { h1 = e1; }
            if (f1 > h1)  { h1 = f1; }

            H1[j] = h1;
        }

        /** Recurrence on vectors: getting i+1 from i. */
        H0 = H1;    H1 = NEXT (H1, delta);
        E0 = E1;    E1 = NEXT (E1, delta);
        F0 = F1;    F1 = NEXT (F1, delta);

    } /* end of for (i=1; ... */

    /** Now, all scores have been computed, we have to trace back the information. */
    i           = qryLen;
    j           = subLen;
    alignLength = 0;
    nbHspOpen   = 0;
    isHspOpened = 1;

    /** SHORTCUTS. */
    LETTER anyLetter  = EncodingManager::singleton().getAlphabet(SUBSEED)->any;

    if (nbHspOpen+2 < splitVectorSize)
    {
        splittab [nbHspOpen++] = qryLen - 1;
        splittab [nbHspOpen++] = subLen - 1;
    }

    /** We reset some alignments fields to be completed. */
    output.identity = 0;
    output.positive = 0;
    output.nbGapQry = 0;
    output.nbGapSbj = 0;
    output.nbMis    = 0;

    /** We setup the rows for the 'i'th index. */
    H1 = ROW (_vector_H, i, delta);     H0 = PREVIOUS (H1, delta);
    E1 = ROW (_vector_E, i, delta);     E0 = PREVIOUS (E1, delta);
    F1 = ROW (_vector_F, i, delta);     F0 = PREVIOUS (F1, delta);

    /** Here we go for the trace back. */
    while ( (i>0) && (j>0) )
    {
        /** Shortcuts. */
        int l0 = qryStr[i-1];
        int l1 = subStr[j-1];

        /** We get the score for the two current residues. */
        s = MATRIX [l0] [l1];

        if (s > 0)  { output.positive++; }

        /** We increase the size of the alignment. */
        alignLength++;

        /******************** SUBSTITUTION ********************/
        if ((H0[j-1] + s) >= MAX (E1[j],F1[j]))
        {
            if ( (l0 == l1) || (l0==anyLetter) || (l1==anyLetter))  { output.identity ++; }
            else           { output.nbMis    ++; }

            i--;
            j--;

            /** If a gap was opened, we finish it. */
            if (isHspOpened == 0)
            {
                if (nbHspOpen+2 < splitVectorSize)
                {
                    splittab[nbHspOpen++] = i;
                    splittab[nbHspOpen++] = j;
                }

                isHspOpened = 1;
            }

            /** i changed, we have to retrieve the previous lines. */
            H1 = H0;    H0 = PREVIOUS (H0, delta);
            E1 = E0;    E0 = PREVIOUS (E0, delta);
            F1 = F0;    F0 = PREVIOUS (F0, delta);
        }

        /******************** GAP IN QUERY ********************/
        else if (E1[j] > F1[j])
        {
            /** If no gap was opened, we open a new one. */
            if (isHspOpened==1)
            {
                if (nbHspOpen+2 < splitVectorSize)
                {
                    splittab[nbHspOpen++] = i;
                    splittab[nbHspOpen++] = j;
                }

                output.nbGapQry ++;

                isHspOpened = 0;
            }
            j--;
        }

        /******************** GAP IN SUBJECT ********************/
        else
        {
            /** If no gap was opened, we open a new one. */
            if (isHspOpened==1)
            {
                if (nbHspOpen+2 < splitVectorSize)
                {
                    splittab[nbHspOpen++] = i;
                    splittab[nbHspOpen++] = j;
                }

                output.nbGapSbj ++;

                isHspOpened = 0;
            }
            i--;

            /** i changed, we have to retrieve the previous lines. */
            H1 = H0;    H0 = PREVIOUS (H0, delta);
            E1 = E0;    E0 = PREVIOUS (E0, delta);
            F1 = F0;    F0 = PREVIOUS (F0, delta);
        }

    } /* end of while ( (i>0) && (j>0) ) */

    /** We add a final entry in the list of splits. */
    if (nbHspOpen+2 < splitVectorSize)
    {
        splittab[nbHspOpen++] = i;
        splittab[nbHspOpen++] = j;
    }

    output.alignSize = alignLength;

#if 0
    dump (output, qryRange, sbjRange, qryLocal, subLocal);
#endif

    /** We return the result. Note that the value has to be divided by 4 to get the actual
     *  number of found gaps. */
    return nbHspOpen;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlignmentSplitterBanded::dumpMatrix (int qryLen, int subLen, int delta)
{
    printf ("\n");
    printf ("AlignmentSplitterBanded::dumpMatrix   qryLen=%d  subLen=%d  delta=%d   (2*delta+1)=%d   band=%d\n",
        qryLen, subLen, delta, (2*delta+1), _band
    );

    printf ("\n");
    printf ("---------------------------------------- H ----------------------------------------\n");
    for (int i=0, c=0; i<=qryLen; i++)  {  printf ("[%3d]  ",i); for (int j=0; j<(2*delta+1); j++)  {  printf ("%5d ", _vector_H[c++]);  }  printf ("\n");   }

    printf ("\n");
    printf ("---------------------------------------- E ----------------------------------------\n");
    for (int i=0, c=0; i<qryLen; i++)  {  for (int j=0; j<(2*delta+1); j++)  {  printf ("%5d ", _vector_E[c++]);  }  printf ("\n");   }

    printf ("\n");
    printf ("---------------------------------------- F ----------------------------------------\n");
    for (int i=0, c=0; i<qryLen; i++)  {  for (int j=0; j<(2*delta+1); j++)  {  printf ("%5d ", _vector_F[c++]);  }  printf ("\n");   }
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
