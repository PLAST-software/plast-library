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

#ifndef _ABSTRACT_ALIGNMENT_RESULT_HPP_
#define _ABSTRACT_ALIGNMENT_RESULT_HPP_

/********************************************************************************/

#include "IAlignementResult.hpp"

/********************************************************************************/
namespace algo  {
/********************************************************************************/

class AbstractAlignmentResult : public IAlignmentResult
{
public:

    virtual ~AbstractAlignmentResult () {}

    /** Return properties about the instance. */
    dp::IProperties* getProperties (const std::string& root, size_t nbDump);

    void readFromFile (
        const char* fileuri,
        std::map<std::string,int>& subjectComments,
        std::map<std::string,int>& queryComments
    );

    bool doesExist (const Alignment& align) { return false; }

};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ABSTRACT_ALIGNMENT_RESULT_HPP_ */
