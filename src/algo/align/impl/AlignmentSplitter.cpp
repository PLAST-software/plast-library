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

#include <algo/align/impl/AlignmentSplitter.hpp>

#include <iostream>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace database;
using namespace dp;
using namespace os;
using namespace os::impl;
using namespace algo::core;

//#define VERBOSE

/********************************************************************************/
namespace algo   {
namespace align {
namespace impl   {
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
   _openGapCost(openGapCost), _extendGapCost(extendGapCost),
   _matrix_H (0), _matrix_E(0), _matrix_F(0),
   _DefaultAlignSize(1000), _MaxAlignSize(3000)
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
    const database::LETTER* subjectSeq,
    const database::LETTER* querySeq,
    u_int32_t subjectStartInSeq,
    u_int32_t subjectEndInSeq,
    u_int32_t queryStartInSeq,
    u_int32_t queryEndInSeq,
    int* splittab,
    size_t& identity,
    size_t& nbGap,
    size_t& nbMis,
    size_t& alignSize,
    database::LETTER* subjectAlign,
    database::LETTER* queryAlign
)
{
    int s,x,y,i,j,d,lg,nbg,nbi,qryLen,subLen;
    int  j1,j2;

    int jj = queryStartInSeq - subjectStartInSeq;
    jj = jj - (queryEndInSeq - subjectEndInSeq);

    int delta = (jj>=0 ? (jj + 2) : (-jj + 2));

    qryLen = queryEndInSeq   - queryStartInSeq   + 1;
    subLen = subjectEndInSeq - subjectStartInSeq + 1;

    /** A little check. */
    if (qryLen > _MaxAlignSize  ||  subLen > _MaxAlignSize)
    {
        /** Alignments are too big for computing information. */
        return 0;
    }

    /** Shortcuts. */
    int8_t** MATRIX = _scoreMatrix->getMatrix();

    const char* qryStr = querySeq   + queryStartInSeq;
    const char* subStr = subjectSeq + subjectStartInSeq;

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

    for (i=0; i<=delta+2; i++)  {    H[0][i]=0; F[0][i]=0;  }

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
    lg=0; nbg=0; nbi=0;

    if (splittab)  {  splittab[y++] = qryLen - 1;  }
    if (splittab)  {  splittab[y++] = subLen - 1;  }

    char qryLocal [10000];
    char subLocal [10000];
    int status = 0;

    /** We reset some alignments fields to be completed. */
    identity = 0;
    nbGap    = 0;
    nbMis    = 0;

    while ( (i>0) && (j>0) )
    {
        s = MATRIX [(int)qryStr[i-1]] [(int)subStr[j-1]];

        if ((H[i-1][j-1] + s) >= MAX (E[i][j],F[i][j]))
        {
            qryLocal[x] = qryStr[i-1];
            subLocal[x] = subStr[j-1];

            i--;
            j--;
            x++;

            status = 0;

            if (lg == 1)
            {
                if (splittab)  {  splittab[y++]=i;  }
                if (splittab)  {  splittab[y++]=j;  }
            }

            lg=0;
        }
        else
        {
            if (E[i][j] > F[i][j])
            {
                qryLocal[x] = CODE_DASH;
                subLocal[x] = subStr[j-1];
                x++;

                if (lg==0)
                {
                    if (splittab)  {  splittab[y++]=i;  }
                    if (splittab)  {  splittab[y++]=j;  }
                    nbg++;
                    lg=1;
                }
                j--;
            }
            else
            {
                qryLocal[x] = qryStr[i-1];
                subLocal[x] = CODE_DASH;
                x++;

                if (lg==0)
                {
                    if (splittab)  {  splittab[y++]=i;  }
                    if (splittab)  {  splittab[y++]=j;  }
                    nbg++;
                    lg=1;
                }
                i--;
            }

            if  (status ==0)  {  nbGap ++;  }
            status = 1;
        }

    } /* end of while ( (i>0) && (j>0) ) */

    for (i=x-1; i>=0; i--)
    {
        /** Shortcuts (and optimization). */
        char l1 = qryLocal[i];
        char l2 = subLocal[i];

             if (l1==l2)                        {  identity ++;  }
        else if (l1==CODE_X  ||  l2==CODE_X)    {  identity ++;  }
        else                                    {  nbMis++;      }
    }

    if (splittab)  {  splittab[y++]=0;  }
    if (splittab)  {  splittab[y++]=0;  }

    alignSize = x;
    if (subjectAlign != 0)  { memcpy (subjectAlign, subLocal, alignSize); }
    if (queryAlign   != 0)  { memcpy (queryAlign,   qryLocal, alignSize); }

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
size_t AlignmentSplitter::splitAlign (Alignment& align,  int* splittab)
{
    size_t alignSize = 0;

    return splitAlign (
        align._occur1->sequence.data.letters.data,
        align._occur2->sequence.data.letters.data,
        align._subjectStartInSeq,
        align._subjectEndInSeq,
        align._queryStartInSeq,
        align._queryEndInSeq,
        splittab,
        align._identity,
        align._nbGap,
        align._nbMis,
        alignSize
    );
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlignmentSplitter::computeInfo (Alignment& align)
{
    size_t identity  = 0;
    size_t nbGap     = 0;
    size_t nbMis     = 0;
    size_t alignSize = 0;

    splitAlign (
        align._occur1->sequence.data.letters.data,
        align._occur2->sequence.data.letters.data,
        align._subjectStartInSeq,
        align._subjectEndInSeq,
        align._queryStartInSeq,
        align._queryEndInSeq,
        0,
        identity, nbGap, nbMis, alignSize
    );

    align._identity = identity;
    align._nbGap    = nbGap;
    align._nbMis    = nbMis;
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

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
