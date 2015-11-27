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

/** \file IOccurrenceIterator.hpp
 *  \brief Definition of interfaces for genomic database indexation.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _IOCCURRENCE_ITERATOR_HPP_
#define _IOCCURRENCE_ITERATOR_HPP_

/********************************************************************************/

#include <designpattern/api/Iterator.hpp>
#include <database/api/ISequenceDatabase.hpp>
#include <seed/api/ISeed.hpp>
#include <misc/api/Vector.hpp>
#include <stdio.h>

/********************************************************************************/
/** \brief Genomic database indexation concepts. */
namespace indexation {
/********************************************************************************/

/** \brief Occurrence of a seed in a genomic sequence
 *
 *  We define here what we need to know about a seed occurrence:
 *      - the sequence where the seed has been found,
 *      - the offset in the sequence  where the seed has been found
 *      - the offset in the database where the seed has been found
 *      - left and right neighborhoods of the seed in the sequence
 *
 *  Since we have the seed occurrence offset in the sequence, we could retrieve
 *  these neighborhoods from the sequence; the ISeedOccurrence interface directly
 *  provides the neighborhoods without needing to read the neighborhoods from
 *  the sequence.
 *
 *  This structure doesn't tell how the attributes are filled, which should be done
 *  through the IOccurrenceIterator and IOccurrenceBlockIterator interfaces.
 *
 *  Note that some attributes of ISeedOccurrence instances may not be filled, according
 *  to the implementation of the iterators that provide them.
 *
 *  Actually, what is wanted is the Sequence instance and the offset in this sequence of the
 *  occurrence. For instance, only the couple [database,offsetInDatabase] may be provided. In
 *  such a case, this is up to the client to recover the couple [sequence,offsetInSequence]
 *  from this information.
 *
 *  Note that the ISeedOccurrence knows how to clone itself with the clone() method.
 *
 *  \see IOccurrenceIterator
 *  \see IOccurrenceBlockIterator
 */
struct ISeedOccurrence
{
    /** The sequence where the seed occurrence occurs. */
    database::ISequence sequence;

    /** Offset of the seed within the sequence. */
    u_int32_t   offsetInSequence;

    /** Offset of the seed within the database. */
    u_int64_t   offsetInDatabase;

    /** Global neighborhood of the seed occurrence. Stored as IWord whose content is
     *  seed + right neighborhood + left neighborhood. Note that the left neighborhood
     *  is coded from right to left for easing scores computing by the PLAST algorithm.
     *
     *  For instance, (seed of 3 letters, neighborhoods of 5 letters):
     *  \code
     *      NEKKQQMGREKIEAELQDICNDVLELLDKYLIPNA...
     *                 ^ seed start
     *  \endcode
     *  In this sample, the global neighborhood would be:
     *      - IEAELQDIKERGM  (ie 3+5+5=13 letters)
     *
     *  If there is not enough letters for building a neighborhood, we fill X letters instead.
     *
     *  Note that this information may be not provided by some implementation.
     *  In such a case, the 'size' attribute of the neighborhood would be 0.
     */
    database::IWord neighbourhood;

    /** Constructor. */
    ISeedOccurrence ()
        : offsetInSequence(0), offsetInDatabase(0), neighbourhood(0)  {}

    /** Constructor.
     * \param[in] neighbourhoodSize : size of the left and right neighborhood
     */
    ISeedOccurrence (size_t neighbourhoodSize)
        : offsetInSequence(0), offsetInDatabase(0), neighbourhood(neighbourhoodSize)
    {
    }

    /** Constructor. Mainly here for clone() implementation.
     */
    ISeedOccurrence (
        const database::ISequence&      aSequence,
        u_int32_t                       aOffsetInSequence,
        u_int64_t                       aOffsetInDatabase,
        const database::IWord&          aNeighbourhood
    )
        : sequence          (aSequence),
          offsetInSequence  (aOffsetInSequence),
          offsetInDatabase  (aOffsetInDatabase),
          neighbourhood     (aNeighbourhood)
    {
    }

    /** Clone of the instance.
     * \return the cloned instance.
     */
    ISeedOccurrence* clone () const
    {
        return new ISeedOccurrence (sequence, offsetInSequence, offsetInDatabase, neighbourhood);
    }
};

/********************************************************************************/

/** \brief Iterates over seed occurrences for a given seed in a given sequence.
 *
 * Returned items are ISeedOccurrence instances.
 *
 * We define no interface here; it is just defined as a shorter name than the full template name.
 *
 * \see IOccurrenceBlockIterator
 */
class IOccurrenceIterator : public dp::Iterator<const ISeedOccurrence*>
{
};

/********************************************************************************/

/** \brief Iterates over seed occurrences for a given seed in a given sequence.
 *
 *  Returned items are blocks (ie vectors) of ISeedOccurrence instances.
 *
 * We define no interface here; it is just defined as a shorter name than the full template name.
 *
 * \see IOccurrenceIterator
 */
class IOccurrenceBlockIterator : public dp::Iterator<misc::Vector<const ISeedOccurrence*>& >
{
public:

    /** Returns the all the neighborhoods in a single buffer. */
    virtual database::LETTER* getNeighbourhoods () = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _IOCCURRENCE_ITERATOR_HPP_  */
