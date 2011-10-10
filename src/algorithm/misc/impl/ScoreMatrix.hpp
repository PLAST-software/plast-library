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

#ifndef SCOREMATRIX_HPP_
#define SCOREMATRIX_HPP_

/********************************************************************************/

#include "IScoreMatrix.hpp"

/********************************************************************************/
namespace algo  {
/********************************************************************************/

class ScoreMatrix : public IScoreMatrix
{
public:

    ScoreMatrix (database::Encoding encod, size_t n, int8_t score);
    virtual ~ScoreMatrix ();

    database::Encoding  getEncoding()       { return _encoding; }
    size_t              getN ()             { return _N;        }
    int8_t**            getMatrix()         { return _matrix;   }
    int8_t*             getMatrixAsVector() { return _matrixAsVector;   }
    int8_t              getDefaultScore()   { return _defaultScore; }

private:
    database::Encoding  _encoding;
    size_t              _N;
    int8_t**            _matrix;
    int8_t*             _matrixAsVector;
    int8_t              _defaultScore;

    friend class ScoreMatrixManager;
};

/********************************************************************************/

class ScoreMatrixManager : public IScoreMatrixManager
{
public:

    static ScoreMatrixManager& singleton ();

    IScoreMatrix* getMatrix (const char* matrixName, database::Encoding encoding);

private:
    virtual ~ScoreMatrixManager () {}

    void fillMatrix (ScoreMatrix* sm, const int8_t source[28][28]);
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* SCOREMATRIX_HPP_ */
