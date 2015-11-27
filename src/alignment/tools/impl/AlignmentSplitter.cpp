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

#include <os/impl/DefaultOsFactory.hpp>

#include <misc/api/macros.hpp>

#include <designpattern/impl/RangeIterator.hpp>

#include <alignment/tools/impl/AlignmentSplitter.hpp>

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

//#define VERBOSE

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
AlignmentSplitter::AlignmentSplitter (IScoreMatrix* scoreMatrix, int openGapCost, int extendGapCost)
 : _scoreMatrix(0),
   _openGapCost(openGapCost), _extendGapCost(extendGapCost), _band(2),
   _matrix_H (0), _matrix_E(0), _matrix_F(0),
   _DefaultAlignSize(1), _MaxAlignSize(6000)
{
    setScoreMatrix (scoreMatrix);

    _matrix_H = newMatrix (_DefaultAlignSize, _DefaultAlignSize);
    _matrix_E = newMatrix (_DefaultAlignSize, _DefaultAlignSize);
    _matrix_F = newMatrix (_DefaultAlignSize, _DefaultAlignSize);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AlignmentSplitter::~AlignmentSplitter ()
{
    setScoreMatrix (0);

    freeMatrix (&_matrix_H);
    freeMatrix (&_matrix_E);
    freeMatrix (&_matrix_F);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
size_t AlignmentSplitter::splitAlign (
    const database::LETTER* sbjSeq,
    const database::LETTER* qrySeq,
    const misc::Range32&    sbjRange,
    const misc::Range32&    qryRange,
    SplitOutput& output
)
{
    int s,x,y,i,j,d,lg,nbg,qryLen,subLen;
    int  j1,j2;

    int jj = qryRange.begin - sbjRange.begin;
    jj = jj - (qryRange.end - sbjRange.end);

    int delta = (jj>=0 ? (jj + _band) : (-jj + _band));

    qryLen = qryRange.getLength();
    subLen = sbjRange.getLength();

    /** A little check. */
    if (qryLen >= _MaxAlignSize  ||  subLen >= _MaxAlignSize)
    {
        /** Alignments are too big for computing information. */
        return 0;
    }

    /** SHORTCUTS. */
    LETTER anyLetter  = EncodingManager::singleton().getAlphabet(SUBSEED)->any;
    LETTER dashLetter = EncodingManager::singleton().getAlphabet(SUBSEED)->gap;

    int        splitVectorSize = output.splittab.size;
    u_int32_t* splittab        = output.splittab.data;

    /** Shortcuts. */
    int8_t** MATRIX = _scoreMatrix->getMatrix();

    const char* qryStr = qrySeq + qryRange.begin;
    const char* subStr = sbjSeq + sbjRange.begin;

#if 0
    const LETTER* convert = EncodingManager::singleton().getEncodingConversion(SUBSEED, ASCII);
    for (int ii=0; ii<qryLen; ii++)  { printf ("%c", convert[qryStr[ii]]);  }  printf("\n");
    for (int ii=0; ii<subLen; ii++)  { printf ("%c", convert[subStr[ii]]);  }  printf("\n");
#endif

    int opengap   = _openGapCost;
    int extendgap = _extendGapCost;

    /** We configure the matrix for the dynamic programming. */
    int16_t** H = 0;
    int16_t** E = 0;
    int16_t** F = 0;

    if (qryLen >= _DefaultAlignSize ||  subLen >= _DefaultAlignSize)
    {
        /** We release the currently allocated matrixes. */
        freeMatrix (&_matrix_H);
        freeMatrix (&_matrix_E);
        freeMatrix (&_matrix_F);

        /** We set the default to the max size. */
        _DefaultAlignSize = _MaxAlignSize;

        /** We reallocate our matrixes. */
        _matrix_H = newMatrix (_DefaultAlignSize, _DefaultAlignSize);
        _matrix_E = newMatrix (_DefaultAlignSize, _DefaultAlignSize);
        _matrix_F = newMatrix (_DefaultAlignSize, _DefaultAlignSize);
    }

    /** We use a shortcut. */
    H = _matrix_H;
    E = _matrix_E;
    F = _matrix_F;

    for (i=0; i<=delta+_band; i++)  {    H[0][i]=0; F[0][i]=0;  }

    for (i=1; i<=qryLen; i++)
    {
        j1 = MAX (1,      i-delta);
        j2 = MIN (subLen, i+delta);

        /** Shortcuts (and optimization) */
        int16_t* H0 = H[i];
        int16_t* H1 = H[i-1];
        int16_t* E0 = E[i];
        int16_t* F0 = F[i];
        int16_t* F1 = F[i-1];

        int8_t* matrixrow = MATRIX [(int)qryStr[i-1]];

        H0 [j1-1] = -1000;
        E0 [j1-1] = -1000;
        H1 [j2]   = -1000;
        F1 [j2]   = -1000;

        /** We use some temporary variables in order to avoid some memory access
         *  (which is time costly) and can't be easily optimized by compiler.
         */
        int16_t e0 = E0 [j1-1];
        int16_t h0 = H0 [j1-1];
        int16_t f0 = 0;

        for (j=j1; j<=j2; j++)
        {
            E0[j] = e0 = MAX (h0    - opengap, e0    - extendgap);
            F0[j] = f0 = MAX (H1[j] - opengap, F1[j] - extendgap);

            d = H1[j-1] + matrixrow [(int)subStr[j-1]];

            H0[j] = h0 = MAX (MAX (e0,f0), d);
        }
    }

    i=qryLen;  j=subLen;  x=0;  y=0;
    lg=0; nbg=0;

    if (y+2 < splitVectorSize)
    {
        splittab[y++] = qryLen - 1;
        splittab[y++] = subLen - 1;
    }

    char qryLocal [10000];
    char subLocal [10000];
    int status = 0;

    /** We reset some alignments fields to be completed. */
    output.identity = 0;
    output.positive = 0;
    output.nbGapQry = 0;
    output.nbGapSbj = 0;
    output.nbMis    = 0;

    while ( (i>0) && (j>0) )
    {
        s = MATRIX [(int)qryStr[i-1]] [(int)subStr[j-1]];

        if (s > 0)  { output.positive++; }

        if ((H[i-1][j-1] + s) > MAX (E[i][j],F[i][j]))
        {
            qryLocal[x] = qryStr[i-1];
            subLocal[x] = subStr[j-1];

            i--;
            j--;
            x++;

            status = 0;

            if (lg == 1)
            {
                if (y+2 < splitVectorSize)
                {
                    splittab[y++]=i;
                    splittab[y++]=j;
                }
            }

            lg=0;
        }
        else
        {
            if (E[i][j] > F[i][j])
            {
                qryLocal[x] = dashLetter;
                subLocal[x] = subStr[j-1];
                x++;

                if (lg==0)
                {
                    if (y+2 < splitVectorSize)
                    {
                        splittab[y++] = i;
                        splittab[y++] = j;
                    }
                    nbg++;
                    lg=1;
                }
                j--;

                if  (status ==0)  {  output.nbGapQry ++;  }
            }

            else
            {
                qryLocal[x] = qryStr[i-1];
                subLocal[x] = dashLetter;
                x++;

                if (lg==0)
                {
                    if (y+2 < splitVectorSize)
                    {
                        splittab[y++] = i;
                        splittab[y++] = j;
                    }
                    nbg++;
                    lg=1;
                }
                i--;

                if  (status ==0)  {  output.nbGapSbj ++;  }
            }

            status = 1;
        }

    } /* end of while ( (i>0) && (j>0) ) */


    for (i=x-1; i>=0; i--)
    {
        /** Shortcuts (and optimization). */
        char l1 = qryLocal[i];
        char l2 = subLocal[i];

#if 1
             if (l1==l2          || (l1==anyLetter || l2==anyLetter))       {  output.identity ++;  }
        else if (l1!=dashLetter  &&  l2!=dashLetter)    {  output.nbMis++;      }
#else
             if (l1==l2         &&  l1!=CODE_X)       {  output.identity ++;  }
        else if (l1!=CODE_DASH  &&  l2!=CODE_DASH)    {  output.nbMis++;      }
#endif
    }

    if (y+2 < splitVectorSize)
    {
        splittab[y++] = 0;
        splittab[y++] = 0;
    }
    output.alignSize = x;

    if (output.subjectAlign != 0)  { memcpy (output.subjectAlign, subLocal, output.alignSize); }
    if (output.queryAlign   != 0)  { memcpy (output.queryAlign,   qryLocal, output.alignSize); }

#if 0
    dump (output, qryRange, sbjRange, qryLocal, subLocal);
#endif

    /** We return the result. */
    return y;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
size_t AlignmentSplitter::splitAlign (Alignment& align,  SplitOutput& output)
{
    size_t result = 0;

    result = splitAlign (
        align.getSequence(Alignment::SUBJECT)->data.letters.data,
        align.getSequence(Alignment::QUERY)->data.letters.data,
        align.getRange(Alignment::SUBJECT),
        align.getRange(Alignment::QUERY),
        output
    );

    /** We return the result. */
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
int16_t** AlignmentSplitter::newMatrix (int nrows, int ncols)
{
    int i;             /* iteration index */
    int16_t ** mat;      /* the new matrix */

    mat = (int16_t **) DefaultFactory::memory().calloc(nrows, sizeof(int16_t *));
    if (mat != NULL)
    {
        mat[0] = (int16_t *) DefaultFactory::memory().malloc((size_t) nrows * (size_t) ncols * sizeof(int16_t));
        if (mat[0] != NULL)
        {
            for (i = 1;  i < nrows;  i++) {  mat[i] = &mat[0][i * ncols];  }
        }
        else
        {
            DefaultFactory::memory().free(mat);
            mat = NULL;
        }
    }

    return mat;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlignmentSplitter::freeMatrix (int16_t *** mat)
{
    if(*mat != NULL)
    {
        DefaultFactory::memory().free((*mat)[0]);
        DefaultFactory::memory().free(*mat);
    }
    *mat = NULL;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlignmentSplitter::dump (
	const SplitOutput& output,
    const misc::Range32&    qryRange,
    const misc::Range32&    sbjRange,
    char* qryLocal,
    char* subLocal
)
{
    /** SHORTCUTS. */
    LETTER anyLetter  = EncodingManager::singleton().getAlphabet(SUBSEED)->any;
    LETTER dashLetter = EncodingManager::singleton().getAlphabet(SUBSEED)->gap;
    u_int32_t x = output.alignSize;

    INFO ((" Identities = %d/%d (%d %%), Positives = %d/%d (%d %%), Gaps = %d/%d (%d %%)\n",
        output.identity, x, (int) (100.0 * (float) output.identity / (float)x),
        output.positive, x, (int) (100.0 * (float) output.positive/ (float)x),
        0, x, (int) (100.0 * (float) 0/ (float)x)
    ));
    INFO (("\n"));

    const LETTER* convert = EncodingManager::singleton().getEncodingConversion(SUBSEED, ASCII);

    dp::impl::RangeIterator<int> it (misc::Range<int> (0, output.alignSize-1), 60, 0);

    int nbQryChar = 0;
    int nbSbjChar = 0;

    misc::Range<int> r;
    size_t nbRetrieved = 0;

    while (it.retrieve (r, nbRetrieved))
    {

        INFO (("Query  %-3d  ", qryRange.begin + 1 + nbQryChar));
        for (int ii=r.begin; ii<=r.end; ii++)  { INFO (("%c", convert[(int)qryLocal[x-1-ii]]));  if (qryLocal[x-1-ii]!=dashLetter) {nbQryChar++;} }
        INFO (("  %d", qryRange.begin + nbQryChar));
        INFO (("\n"));

        printf ("            ");
        for (int ii=r.begin; ii<=r.end; ii++)
        {
            bool isTheSame = qryLocal[x-1-ii]==subLocal[x-1-ii];

            if (isTheSame && qryLocal[x-1-ii] != anyLetter)
            {
                INFO (("%c", convert[(int)qryLocal[x-1-ii]]));
            }
            else if (qryLocal[x-1-ii]==dashLetter  ||  subLocal[x-1-ii]==dashLetter)
            {
                INFO (("%c", ' '));
            }
            else if ((_scoreMatrix->getMatrix())[(int)qryLocal[x-1-ii]][(int)subLocal[x-1-ii]] > 0)
            {
                INFO (("%c", '+'));
            }
            else
            {
                INFO (("%c", ' '));
            }
        }
        INFO (("\n"));

        INFO (("Sbjct  %-3d  ", sbjRange.begin + 1 + nbSbjChar));
        for (int ii=r.begin; ii<=r.end; ii++)  { INFO (("%c", convert[(int)subLocal[x-1-ii]])); if (subLocal[x-1-ii]!=dashLetter) {nbSbjChar++;} }
        INFO (("  %d", sbjRange.begin + nbSbjChar));
        INFO (("\n"));

        INFO (("\n"));
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
void AlignmentSplitter::dumpMatrix (int qryLen, int subLen, int delta)
{
    printf ("\n");
    printf ("AlignmentSplitter::dumpMatrix   qryLen=%d  subLen=%d  delta=%d   (2*delta+1)=%d   band=%d\n",
        qryLen, subLen, delta, (2*delta+1), _band
    );

    printf ("\n");
    printf ("---------------------------------------- H ----------------------------------------\n");
    for (int i=0; i<=qryLen; i++)
    {
        int j1 = MAX (1,      i-delta) - 1;
        int j2 = MIN (subLen, i+delta) + 1;
        for (int j=j1; j<=j2; j++)  {  printf ("%7d ", _matrix_H [i][j]);  }
        printf ("\n");
    }
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
