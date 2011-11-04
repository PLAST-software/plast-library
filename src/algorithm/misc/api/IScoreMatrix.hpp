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

#ifndef ISCOREMATRIX_HPP_
#define ISCOREMATRIX_HPP_

/********************************************************************************/

#include "types.hpp"
#include <string>
#include "IAlphabet.hpp"
#include "SmartPointer.hpp"

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/** We define what a score matrix is, i.e. some matrix with values encoded in some
 *  provided encoding scheme. */
class IScoreMatrix : public dp::SmartPointer
{
public:
    virtual database::Encoding  getEncoding() = 0;
    virtual size_t              getN () = 0;
    virtual int8_t**            getMatrix() = 0;
    virtual int8_t*             getMatrixAsVector() = 0;
    virtual int8_t              getDefaultScore() = 0;
};

/********************************************************************************/

class IScoreMatrixManager
{
public:
    virtual IScoreMatrix* getMatrix (const char* matrixName, database::Encoding encoding) = 0;
};

/********************************************************************************/

class ScoreMatrixFailure
{
public:
    ScoreMatrixFailure (const char* message) : _message(message)  {}
    const char* getMessage ()  { return _message.c_str(); }
private:
    std::string _message;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* ISCOREMATRIX_HPP_ */
