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
 *     - the whole sequences data are concatenated in a single table
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
        for (int i=0; i<shift; i++)
        {
            database.data [i] = EncodingManager::singleton().getAlphabet(SUBSEED)->any;
        }

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

    /** We reverse the data to the other strand (only for nucl. requests). */
    void reverse ()
    {
        /** 1) We reorder the residues for each sequence. */
        for (size_t i=0; i<nbSequences; i++)
        {
            Offset a = offsets.data[i+0];
            Offset b = offsets.data[i+1];

            size_t nbResidues = b - a;
            LETTER* data = database.data + a;

            /** We mirror (from the middle) the residues of the sequence. */
            for (size_t i=0; i<nbResidues/2; i++)  {  std::swap (data[i], data [nbResidues - 1 - i]);  }
        }

        /** 2) We get the complement of each nucleotide. Note that we can do it for the whole buffer
         * sequentially => could use SSE here (x16). */
        size_t nbTotalResidues = offsets.data[nbSequences];
        LETTER* data = database.data;
        for (size_t i=0; i<nbTotalResidues; i++)
        {
            /** Residues above value 4 (like N) are not reversed. */
            if (data[i]<4)
            {
                /** Note here the way we get the complement of a nucleotide: comp(c) -> (c+2)%4
                 * It is possible since we have A=0, C=1, G=3, T=2. You can check:
                 *      comp(A) = (0+2)%4 = 2 = T
                 *      comp(C) = (1+2)%4 = 3 = G
                 *      comp(G) = (3+2)%4 = 1 = C
                 *      comp(T) = (2+2)%4 = 0 = A
                 */
                data[i] = (data[i] + 2) & 3 ;
            }
        }
    }
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ISEQUENCE_CACHE_HPP_ */
