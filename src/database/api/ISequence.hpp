/*
 * Database.hpp
 *
 *  Created on: 7 juin 2011
 *      Author: edrezen
 */

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

/** This structure is just a container for 3 information.
 *
 * Note that:
 *
 *  => the way the information is created is delegated to the interface ISequenceBuilder.
 *  => the way the information is provided to clients is delegated to the interface ISequenceIterator
 */
struct ISequence
{
    ISequence () : comment(""), index(0) {}

    const char* comment;
    IWord       data;
    size_t      index;

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
