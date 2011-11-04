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

#ifndef _ISEQUENCE_HPP_
#define _ISEQUENCE_HPP_

/********************************************************************************/

#include <stddef.h>
#include "IAlphabet.hpp"
#include "IWord.hpp"

#include <iostream>
#include <sstream>

/********************************************************************************/
namespace database {
/********************************************************************************/

class ISequenceDatabase;

/** This structure is just a container for 3 information.
 *
 * Note that:
 *
 *  => the way the information is created is delegated to the interface ISequenceBuilder.
 *  => the way the information is provided to clients is delegated to the interface ISequenceIterator
 */
struct ISequence
{
    ISequence () : database(0), comment(""), index(0) {}

    ISequenceDatabase*  database;
    const char*         comment;
    IWord               data;
    size_t              index;

    /** */
    std::string toString () const
    {
        std::stringstream ss;
        ss << "[SEQUENCE " << index << "  " << data.toString() << "]";
        return ss.str ();
    }
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ISEQUENCE_HPP_ */
