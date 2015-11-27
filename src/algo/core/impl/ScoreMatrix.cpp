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

#include <algo/core/impl/ScoreMatrix.hpp>

#include <misc/api/types.hpp>

#include <string.h>
#include <stdlib.h>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace os;
using namespace os::impl;
using namespace database;

/********************************************************************************/
namespace algo {
namespace core {
namespace impl {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ScoreMatrix::ScoreMatrix (database::Encoding encoding, size_t N, int8_t defaultScore)
    : _encoding(encoding), _size(N), _matrix(0), _matrixAsVector(0), _defaultScore(defaultScore)
{
    /** We allocate the matrix. */
    int8_t* tmp = (int8_t*) DefaultFactory::memory().malloc (N*N*sizeof(int8_t));
    _matrix = (int8_t**) DefaultFactory::memory().calloc (N, sizeof (int8_t*));
    for (size_t i=0;  i<N;  i++)   {  _matrix[i] = & (tmp[i*N]);  }

    /** We allocate the matrix as vector. */
    _matrixAsVector = (int8_t*) DefaultFactory::memory().calloc (32*32, sizeof(int8_t));

    DEBUG (("ScoreMatrix::getMatrix: this=%p   n=%d  score=%d  matrix=%p \n", this, N, _defaultScore, _matrix));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ScoreMatrix::~ScoreMatrix ()
{
    /** We deallocate the matrix. */
    if (_matrix != 0)
    {
        DefaultFactory::memory().free (_matrix[0]);
        DefaultFactory::memory().free (_matrix);
    }

    DefaultFactory::memory().free (_matrixAsVector);
}

/********************************************************************************/

static const int8_t  BLOSUM62_matrix [28][28] = {

   /*  -   A   B   C   D   E   F   G   H   I   K   L   M   N   P   Q   R   S   T   V   W   X   Y   Z   U   *   O   J  */
    { -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, },
    { -4,  4, -2,  0, -2, -1, -2,  0, -2, -1, -1, -1, -1, -2, -1, -1, -1,  1,  0,  0, -3, -1, -2, -1, -1, -4, -1, -1, },
    { -4, -2,  4, -3,  4,  1, -3, -1,  0, -3,  0, -4, -3,  4, -2,  0, -1,  0, -1, -3, -4, -1, -3,  0, -1, -4, -1, -3, },
    { -4,  0, -3,  9, -3, -4, -2, -3, -3, -1, -3, -1, -1, -3, -3, -3, -3, -1, -1, -1, -2, -1, -2, -3, -1, -4, -1, -1, },
    { -4, -2,  4, -3,  6,  2, -3, -1, -1, -3, -1, -4, -3,  1, -1,  0, -2,  0, -1, -3, -4, -1, -3,  1, -1, -4, -1, -3, },
    { -4, -1,  1, -4,  2,  5, -3, -2,  0, -3,  1, -3, -2,  0, -1,  2,  0,  0, -1, -2, -3, -1, -2,  4, -1, -4, -1, -3, },
    { -4, -2, -3, -2, -3, -3,  6, -3, -1,  0, -3,  0,  0, -3, -4, -3, -3, -2, -2, -1,  1, -1,  3, -3, -1, -4, -1,  0, },
    { -4,  0, -1, -3, -1, -2, -3,  6, -2, -4, -2, -4, -3,  0, -2, -2, -2,  0, -2, -3, -2, -1, -3, -2, -1, -4, -1, -4, },
    { -4, -2,  0, -3, -1,  0, -1, -2,  8, -3, -1, -3, -2,  1, -2,  0,  0, -1, -2, -3, -2, -1,  2,  0, -1, -4, -1, -3, },
    { -4, -1, -3, -1, -3, -3,  0, -4, -3,  4, -3,  2,  1, -3, -3, -3, -3, -2, -1,  3, -3, -1, -1, -3, -1, -4, -1,  3, },
    { -4, -1,  0, -3, -1,  1, -3, -2, -1, -3,  5, -2, -1,  0, -1,  1,  2,  0, -1, -2, -3, -1, -2,  1, -1, -4, -1, -3, },
    { -4, -1, -4, -1, -4, -3,  0, -4, -3,  2, -2,  4,  2, -3, -3, -2, -2, -2, -1,  1, -2, -1, -1, -3, -1, -4, -1,  3, },
    { -4, -1, -3, -1, -3, -2,  0, -3, -2,  1, -1,  2,  5, -2, -2,  0, -1, -1, -1,  1, -1, -1, -1, -1, -1, -4, -1,  2, },
    { -4, -2,  4, -3,  1,  0, -3,  0,  1, -3,  0, -3, -2,  6, -2,  0,  0,  1,  0, -3, -4, -1, -2,  0, -1, -4, -1, -3, },
    { -4, -1, -2, -3, -1, -1, -4, -2, -2, -3, -1, -3, -2, -2,  7, -1, -2, -1, -1, -2, -4, -1, -3, -1, -1, -4, -1, -3, },
    { -4, -1,  0, -3,  0,  2, -3, -2,  0, -3,  1, -2,  0,  0, -1,  5,  1,  0, -1, -2, -2, -1, -1,  4, -1, -4, -1, -2, },
    { -4, -1, -1, -3, -2,  0, -3, -2,  0, -3,  2, -2, -1,  0, -2,  1,  5, -1, -1, -3, -3, -1, -2,  0, -1, -4, -1, -2, },
    { -4,  1,  0, -1,  0,  0, -2,  0, -1, -2,  0, -2, -1,  1, -1,  0, -1,  4,  1, -2, -3, -1, -2,  0, -1, -4, -1, -2, },
    { -4,  0, -1, -1, -1, -1, -2, -2, -2, -1, -1, -1, -1,  0, -1, -1, -1,  1,  5,  0, -2, -1, -2, -1, -1, -4, -1, -1, },
    { -4,  0, -3, -1, -3, -2, -1, -3, -3,  3, -2,  1,  1, -3, -2, -2, -3, -2,  0,  4, -3, -1, -1, -2, -1, -4, -1,  2, },
    { -4, -3, -4, -2, -4, -3,  1, -2, -2, -3, -3, -2, -1, -4, -4, -2, -3, -3, -2, -3, 11, -1,  2, -2, -1, -4, -1, -2, },
    { -4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -4, -1, -1, },
    { -4, -2, -3, -2, -3, -2,  3, -3,  2, -1, -2, -1, -1, -2, -3, -1, -2, -2, -2, -1,  2, -1,  7, -2, -1, -4, -1, -1, },
    { -4, -1,  0, -3,  1,  4, -3, -2,  0, -3,  1, -3, -1,  0, -1,  4,  0,  0, -1, -2, -2, -1, -2,  4, -1, -4, -1, -3, },
    { -4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -4, -1, -1, },
    { -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4,  1, -4, -4, },
    { -4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -4, -1, -1, },
    { -4, -1, -3, -1, -3, -3,  0, -4, -3,  3, -3,  3,  2, -3, -3, -2, -2, -2, -1,  2, -2, -1, -1, -3, -1, -4, -1,  3, }
};

static const int8_t  BLOSUM50_matrix [28][28] = {

   /*  -   A   B   C   D   E   F   G   H   I   K   L   M   N   P   Q   R   S   T   V   W   X   Y   Z   U   *   O   J  */
    { -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, },
    { -5,  5, -2, -1, -2, -1, -3,  0, -2, -1, -1, -2, -1, -1, -1, -1, -2,  1,  0,  0, -3, -1, -2, -1, -1, -5, -1, -2, },
    { -5, -2,  6, -3,  6,  1, -4, -1,  0, -4,  0, -4, -3,  5, -2,  0, -1,  0,  0, -3, -5, -1, -3,  1, -1, -5, -1, -4, },
    { -5, -1, -3, 13, -4, -3, -2, -3, -3, -2, -3, -2, -2, -2, -4, -3, -4, -1, -1, -1, -5, -1, -3, -3, -1, -5, -1, -2, },
    { -5, -2,  6, -4,  8,  2, -5, -1, -1, -4, -1, -4, -4,  2, -1,  0, -2,  0, -1, -4, -5, -1, -3,  1, -1, -5, -1, -4, },
    { -5, -1,  1, -3,  2,  6, -3, -3,  0, -4,  1, -3, -2,  0, -1,  2,  0, -1, -1, -3, -3, -1, -2,  5, -1, -5, -1, -3, },
    { -5, -3, -4, -2, -5, -3,  8, -4, -1,  0, -4,  1,  0, -4, -4, -4, -3, -3, -2, -1,  1, -1,  4, -4, -1, -5, -1,  1, },
    { -5,  0, -1, -3, -1, -3, -4,  8, -2, -4, -2, -4, -3,  0, -2, -2, -3,  0, -2, -4, -3, -1, -3, -2, -1, -5, -1, -4, },
    { -5, -2,  0, -3, -1,  0, -1, -2, 10, -4,  0, -3, -1,  1, -2,  1,  0, -1, -2, -4, -3, -1,  2,  0, -1, -5, -1, -3, },
    { -5, -1, -4, -2, -4, -4,  0, -4, -4,  5, -3,  2,  2, -3, -3, -3, -4, -3, -1,  4, -3, -1, -1, -3, -1, -5, -1,  4, },
    { -5, -1,  0, -3, -1,  1, -4, -2,  0, -3,  6, -3, -2,  0, -1,  2,  3,  0, -1, -3, -3, -1, -2,  1, -1, -5, -1, -3, },
    { -5, -2, -4, -2, -4, -3,  1, -4, -3,  2, -3,  5,  3, -4, -4, -2, -3, -3, -1,  1, -2, -1, -1, -3, -1, -5, -1,  4, },
    { -5, -1, -3, -2, -4, -2,  0, -3, -1,  2, -2,  3,  7, -2, -3,  0, -2, -2, -1,  1, -1, -1,  0, -1, -1, -5, -1,  2, },
    { -5, -1,  5, -2,  2,  0, -4,  0,  1, -3,  0, -4, -2,  7, -2,  0, -1,  1,  0, -3, -4, -1, -2,  0, -1, -5, -1, -4, },
    { -5, -1, -2, -4, -1, -1, -4, -2, -2, -3, -1, -4, -3, -2, 10, -1, -3, -1, -1, -3, -4, -1, -3, -1, -1, -5, -1, -3, },
    { -5, -1,  0, -3,  0,  2, -4, -2,  1, -3,  2, -2,  0,  0, -1,  7,  1,  0, -1, -3, -1, -1, -1,  4, -1, -5, -1, -3, },
    { -5, -2, -1, -4, -2,  0, -3, -3,  0, -4,  3, -3, -2, -1, -3,  1,  7, -1, -1, -3, -3, -1, -1,  0, -1, -5, -1, -3, },
    { -5,  1,  0, -1,  0, -1, -3,  0, -1, -3,  0, -3, -2,  1, -1,  0, -1,  5,  2, -2, -4, -1, -2,  0, -1, -5, -1, -3, },
    { -5,  0,  0, -1, -1, -1, -2, -2, -2, -1, -1, -1, -1,  0, -1, -1, -1,  2,  5,  0, -3, -1, -2, -1, -1, -5, -1, -1, },
    { -5,  0, -3, -1, -4, -3, -1, -4, -4,  4, -3,  1,  1, -3, -3, -3, -3, -2,  0,  5, -3, -1, -1, -3, -1, -5, -1,  2, },
    { -5, -3, -5, -5, -5, -3,  1, -3, -3, -3, -3, -2, -1, -4, -4, -1, -3, -4, -3, -3, 15, -1,  2, -2, -1, -5, -1, -2, },
    { -5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -5, -1, -1, },
    { -5, -2, -3, -3, -3, -2,  4, -3,  2, -1, -2, -1,  0, -2, -3, -1, -1, -2, -2, -1,  2, -1,  8, -2, -1, -5, -1, -1, },
    { -5, -1,  1, -3,  1,  5, -4, -2,  0, -3,  1, -3, -1,  0, -1,  4,  0,  0, -1, -3, -2, -1, -2,  5, -1, -5, -1, -3, },
    { -5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -5, -1, -1, },
    { -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5,  1, -5, -5, },
    { -5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -5, -1, -1, },
    { -5, -2, -4, -2, -4, -3,  1, -4, -3,  4, -3,  4,  2, -4, -3, -3, -3, -3, -1,  2, -2, -1, -1, -3, -1, -5, -1,  4, }
};

/********************************************************************************/

static int8_t  IDENTITY_NUCLEOTID_matrix [28][28];

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ScoreMatrixManager& ScoreMatrixManager::singleton ()
{
    static ScoreMatrixManager instance;
    return instance;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IScoreMatrix* ScoreMatrixManager::getMatrix (const char* matrixName, Encoding encoding, int reward, int penalty)
{
    ScoreMatrix* result = 0;

    if (strcmp(matrixName,"BLOSUM62")==0)
    {
        DEBUG (("ScoreMatrixManager::getMatrix => BLOSUM62\n"));

        /** We create the score matrix. */
        result = new ScoreMatrix (encoding, 28, -4);

        /** We fill the matrix. */
        fillMatrix (result, BLOSUM62_matrix);
    }
    else if (strcmp(matrixName,"BLOSUM50")==0)
    {
        DEBUG (("ScoreMatrixManager::getMatrix => BLOSUM50\n"));

        /** We create the score matrix. */
        result = new ScoreMatrix (encoding, 28, -5);

        /** We fill the matrix. */
        fillMatrix (result, BLOSUM50_matrix);
    }
    else if (strcmp(matrixName,"IDENTITY")==0)
    {
        DEBUG (("ScoreMatrixManager::getMatrix => IDENTITY\n"));

        /** We create the score matrix. */
        result = new ScoreMatrix (encoding, 6, -3);

        /** We set up the nucleotide matrix with reward and penalty. */
        setupNucleotideMatrix (IDENTITY_NUCLEOTID_matrix, reward, penalty);

        /** We fill the matrix. */
        fillMatrix (result, IDENTITY_NUCLEOTID_matrix);
    }
    else if (strcmp(matrixName,"IDENTITY_BLAST")==0)
    {
        DEBUG (("ScoreMatrixManager::getMatrix => IDENTITY\n"));

        /** We create the score matrix. */
        result = new ScoreMatrix (encoding, 6, -3);

        /** We set up the nucleotide matrix with reward and penalty like blast algorithm. In this case a specific score is calculated
         * for the ambiguity */
        setupNucleotideMatrixBlast (IDENTITY_NUCLEOTID_matrix, reward, penalty);

        /** We fill the matrix. */
        fillMatrix (result, IDENTITY_NUCLEOTID_matrix);
    }

    /** We may have no matrix: we send an exception. */
    if (result == 0)  {   throw ScoreMatrixFailure ("unknown matrix");   }

    DEBUG (("ScoreMatrixManager::getMatrix: name='%s'  result=%p  matrix=%p\n", matrixName, result, result->getMatrix() ));

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
void ScoreMatrixManager::fillMatrix (ScoreMatrix* sm, const int8_t source[28][28])
{
    /** We get a conversion table. */
    const LETTER* convert = EncodingManager::singleton().getEncodingConversion (sm->getEncoding(), NCBI);

    /** A little shortcut. */
    int8_t** m  = sm->_matrix;
    int8_t*  mv = sm->_matrixAsVector;
    size_t   N  = sm->getN();

    /** We fill the matrix. */
    for (size_t i=0; i<N; i++)
    {
        int8_t* matrixRow = m[i];

        for (size_t j=0; j<N; j++)
        {
            if (convert != 0)
            {
                matrixRow[j] = source [(int)convert[i]] [(int)convert[j]];
            }
            else
            {
                matrixRow[j] = source [i] [j];
            }
        }
    }

    /** We fill the "vector" matrix. */
    size_t c=0;
    for (size_t i=0; i<N; i++, c+=(32-N))
    {
        for (size_t j=0; j<N; j++)
        {
            mv[c++] = m[i][j]; // + (m[i][j] > 0 ? 1<<6 : 0) ;
        }
    }

    DEBUG (("ScoreMatrix::fillMatrix : DONE (N=%d  c=%d)\n", N, c));
}


/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void ScoreMatrixManager::setupNucleotideMatrix (int8_t m[28][28], int reward, int penalty)
{
    /** We set by default the penalty score for all matrix items. */
    for (int i=0; i<28; i++)  {  for (int j=0; j<28; j++)  {  m[i][j] = penalty;  }  }

    /** We set the reward score on diagonal.*/
    for (int i=0; i<28; i++)  {  m[i][i] = reward; }

}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void ScoreMatrixManager::setupNucleotideMatrixBlast (int8_t m[28][28], int reward, int penalty)
{
    /** We set by default the penalty score for all matrix items. */
    for (int i=0; i<28; i++)  {  for (int j=0; j<28; j++)  {  m[i][j] = penalty;  }  }

    /** We set the reward score on diagonal.*/
    for (int i=0; i<28; i++)  {  m[i][i] = reward; }

    /** Set the score value for the ambiguity **/
    for (int i=0; i<28; i++)  {  m[i][4] = misc::aroundInt((double)((3*penalty+reward)/4.0)); m[4][i] = m[i][4];}

}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
