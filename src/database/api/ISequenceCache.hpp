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

/** \file ISequenceCache.hpp
 *  \brief Memory cached database
 *  \date 07/11/2011
 *  \author edrezen
 *
 *   Define an interface of a memory cache that holds a database content. This
 *   may be used for keeping in memory the genomic sequences read from some file.
 */

#ifndef _ISEQUENCE_CACHE_HPP_
#define _ISEQUENCE_CACHE_HPP_

/********************************************************************************/

#include <designpattern/api/SmartPointer.hpp>
#include <misc/api/Vector.hpp>

/********************************************************************************/
/** \brief Definition of concepts related to genomic databases. */
namespace database {
/********************************************************************************/

/** \brief Provides a data structure for caching a database content into memory.
 *
 *  This class provides attributes containing sequences information in a flat format, which means:
 *     - all sequences comments are contained in a vector
 *     - the whole sequences datas are concatenated in a single table
 *     - an array of offsets allows to find the start of a sequence in the whole sequences data table.
 */
class ISequenceCache : public dp::SmartPointer
{
public:

    /** Constructor. An estimation of the global data size is provided for configuring vectors sizes. Note that
     * these vectors may be resized if needed.
     * \param[in] estimatedSize : estimation of the data size.
     */
    ISequenceCache (Offset estimatedSize) : dataSize(0), nbSequences(0), sequenceAverageA(0), sequenceAverageB(0)
    {
        database.resize (estimatedSize);
        offsets.resize  (estimatedSize/500);
        comments.resize (estimatedSize/500);

        /** We add a small shift in the data buffer to let extra room for blast algorithm to look at. */
        shift = 5;

        /** We initialize the extra data with default values. */
        for (int i=0; i<shift; i++) {   database.data [i] = CODE_X;  }

        /** The initial size of the data is the size of the shift. */
        dataSize = shift;
    }

    /** Shift in the data buffer to let extra room for blast algorithm to look at. */
    u_int8_t shift;

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
    misc::Vector<database::LETTER> database;

    /** Vector that stores offsets of the sequences data. */
    misc::Vector<Offset> offsets;

    /** Vector that stores the comments. */
    std::vector<std::string> comments;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ISEQUENCE_CACHE_HPP_ */
