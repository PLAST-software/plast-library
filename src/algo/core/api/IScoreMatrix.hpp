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

/** \file IScoreMatrix.hpp
 *  \brief Interfaces for score matrix management.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef ISCOREMATRIX_HPP_
#define ISCOREMATRIX_HPP_

/********************************************************************************/

#include <designpattern/api/SmartPointer.hpp>
#include <misc/api/types.hpp>
#include <database/api/IAlphabet.hpp>

#include <string>

/********************************************************************************/
/** \brief PLAST algorithm concepts. */
namespace algo {
/** \brief Concept for configuring and running PLAST. */
namespace core {
/********************************************************************************/

/** \brief Definition of a score matrix
 *
 *  We define what a score matrix is, i.e. a (square) matrix that gives some similarity score
 *  between two letters of a genomic alphabet (nucleotids or amino acids).
 */
class IScoreMatrix : public dp::SmartPointer
{
public:

    /** Getter for the encoding scheme.
     * \return the encoding scheme.
     */
    virtual database::Encoding  getEncoding() = 0;

    /** Getter for the size of the (square) matrix.
     * \return the matrix size.
     */
    virtual size_t              getN () = 0;

    /** Getter for matrix content as two dimension array.
     * \return the matrix.
     */
    virtual int8_t**            getMatrix() = 0;

    /** Getter for matrix content as one dimension array.
     * \return the matrix.
     */
    virtual int8_t*             getMatrixAsVector() = 0;

    /** Getter for the default score.
     * \return the default score.
     */
    virtual int8_t              getDefaultScore() = 0;
};

/********************************************************************************/

/** \brief Factory for score matrix
 *
 * The IScoreMatrixManager interface is a factory for creating IScoreMatrix instances
 * according to one name (like "BLOSUM62") and an encoding scheme.
 */
class IScoreMatrixManager
{
public:

    /** Destructor. */
    virtual ~IScoreMatrixManager () {}

    /** Factory.
     * \param[in] matrixName : kind of the score matrix (likely "BLOSUM62")
     * \param[in] encoding   : encoding for the matrix (likely SUBSEED)
     */
    virtual IScoreMatrix* getMatrix (const char* matrixName, database::Encoding encoding, int reward, int penalty) = 0;
};

/********************************************************************************/

/** \brief Exception for score matrix management.
 *
 * Define an exception when something goes wrong while using score matrix.
 * Likely to be sent when an unknown matrix kind (ie. not "BLOSUM62" for instance) is required.
 */
class ScoreMatrixFailure
{
public:

    /** Constructor.
     * \param[in] message : textual information explaining the exception.
     * */
    ScoreMatrixFailure (const char* message) : _message(message)  {}

    /** Returns some textual explanation message.
     * \return the message.
     */
    const char* getMessage ()  { return _message.c_str(); }

private:
    std::string _message;
};

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/

#endif /* ISCOREMATRIX_HPP_ */
