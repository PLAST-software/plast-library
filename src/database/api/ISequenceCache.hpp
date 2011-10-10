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

#ifndef _ISEQUENCE_CACHE_HPP_
#define _ISEQUENCE_CACHE_HPP_

/********************************************************************************/

#include "types.hpp"
#include "SmartPointer.hpp"
#include "MemoryAllocator.hpp"
#include <string>
#include <vector>

/********************************************************************************/
namespace database {
/********************************************************************************/

/** Provides means to cache a database into memory.
 */
class ISequenceCache : public dp::SmartPointer
{
public:
    ISequenceCache (Offset estimatedSize) : dataSize(0), nbSequences(0), sequenceAverageA(0), sequenceAverageB(0)
    {
        database.resize (estimatedSize);
        offsets.resize  (estimatedSize/500);
        comments.resize (estimatedSize/500);
    }

    /** Gives the data size of all sequences. */
    Offset dataSize;

    /** Gives the number of sequences. */
    size_t nbSequences;

    /** Gives an approximation of the average size of sequences.
     *  This average is given by two numbers [a,b]. An estimate of the average size is then given by: 2^b / a
     *  The trick is useful for computing quickly the division of an offset N by this average size:
     *      N / (2^b/a) = (N*a) / 2^b  =  (N*a) >> b
     *  So, we avoid an euclidian division and have only a multiplication and a bits shift. */
    size_t sequenceAverageA;
    size_t sequenceAverageB;

    /** Vector that stores the concatenation of the sequences data. */
    os::Vector<database::LETTER> database;

    /** Vector that stores offsets of the sequences data. */
    os::Vector<Offset> offsets;

    /** Vector that stores the comments. */
    std::vector<std::string> comments;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ISEQUENCE_CACHE_HPP_ */
