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

/** \file ScoreMatrix.hpp
 *  \brief Default implementation of the IScoreMatrix interface
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef SCOREMATRIX_HPP_
#define SCOREMATRIX_HPP_

/********************************************************************************/

#include <algo/core/api/IScoreMatrix.hpp>

/********************************************************************************/
namespace algo {
namespace core {
/** \brief Implementation of concepts for configuring and running PLAST. */
namespace impl {
/********************************************************************************/

/** \brief Implementation of IScoreMatrix interface
 */
class ScoreMatrix : public IScoreMatrix
{
public:

    /** Constructor.
     * \param[in] encod : encoding scheme for the matrix
     * \param[in] n : size of the matrix
     * \param[in] score : default bad score
     */
    ScoreMatrix (database::Encoding encod, size_t n, int8_t score);

    /** Destructor. */
    virtual ~ScoreMatrix ();

    /** \copydoc IScoreMatrix::getEncoding */
    database::Encoding  getEncoding()       { return _encoding; }

    /** \copydoc IScoreMatrix::getN */
    size_t              getN ()             { return _size;        }

    /** \copydoc IScoreMatrix::getMatrix */
    int8_t**            getMatrix()         { return _matrix;   }

    /** \copydoc IScoreMatrix::getMatrixAsVector */
    int8_t*             getMatrixAsVector() { return _matrixAsVector;   }

    /** \copydoc IScoreMatrix::getDefaultScore */
    int8_t              getDefaultScore()   { return _defaultScore; }

private:
    database::Encoding  _encoding;
    size_t              _size;
    int8_t**            _matrix;
    int8_t*             _matrixAsVector;
    int8_t              _defaultScore;

    friend class ScoreMatrixManager;
};

/********************************************************************************/

/** \brief Factory that creates IScoreMatrix instances
 *
 * Defined as a Singleton.
 */
class ScoreMatrixManager : public IScoreMatrixManager
{
public:

    /** Singleton.
     * \return the singleton instance.
     */
    static ScoreMatrixManager& singleton ();

    /** \copydoc IScoreMatrixManager::getMatrix */
    IScoreMatrix* getMatrix (const char* matrixName, database::Encoding encoding, int reward, int penalty);

private:
    virtual ~ScoreMatrixManager () {}

    void fillMatrix (ScoreMatrix* sm, const int8_t source[28][28]);

    /** */
    void setupNucleotideMatrix (int8_t source[28][28], int reward, int penalty);

    /** */
    void setupNucleotideMatrixBlast (int8_t source[28][28], int reward, int penalty);

};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* SCOREMATRIX_HPP_ */
