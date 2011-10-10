/*
 * Database.hpp
 *
 *  Created on: 7 juin 2011
 *      Author: edrezen
 */

#ifndef _IOCCURRENCE_ITERATOR_HPP_
#define _IOCCURRENCE_ITERATOR_HPP_

/********************************************************************************/

#include "Iterator.hpp"
#include "ISequenceDatabase.hpp"
#include "ISeed.hpp"

#include <stdio.h>

/********************************************************************************/
namespace indexation {
/********************************************************************************/

/** Definition of a seed occurrence. According to the algorithm that provides such objects,
 *  some attributes may not be filled.
 *  Actually, what is wanted is the Sequence instance and the offset in this sequence of the
 *  occurrence. For instance, only the couple [database,offsetInDatabase] may be provided. In
 *  such a case, this is up to the client to recover the couple [sequence,offsetInSequence]
 *  from this information.
 */
struct ISeedOccurrence
{
    /** The sequence. */
    database::ISequence sequence;

    /** Offset of the seed within the sequence. */
    u_int32_t   offsetInSequence;

    /** A reference of the sequence database. */
    database::ISequenceDatabase* database;

    /** Offset of the seed within the database. */
    u_int64_t   offsetInDatabase;

    /** Global neighbourhood of the seed occurrence. Stored as IWord whose content is
     *  seed + right neighbourhood + left neighbourhood
     *  For instance, (seed of 3 letters, neighbourhoods of 5 letters):
     *      NEKKQQMGREKIEAELQDICNDVLELLDKYLIPNA...
     *                 ^ seed start
     *  In this sample, the global neighbourhood would be:
     *      IEAELQDIKERGM  (ie 3+5+5=13 letters)
     *
     *  If there is not enough letters for building a neighbourhood, we fill X letters instead.
     *
     *  Note that this information may be not provided by some implementation.
     *  In such a case, the 'size' attribute of the neighbourhood would be 0.
     */
    database::IWord neighbourhood;

    /** */
    ISeedOccurrence ()
        : offsetInSequence(0), database(0), offsetInDatabase(0), neighbourhood(0)  {}

    /** */
    ISeedOccurrence (size_t neighbourhoodSize)
        : offsetInSequence(0), database(0), offsetInDatabase(0), neighbourhood(neighbourhoodSize)
    {
    }

    /** */
    ISeedOccurrence (
        const database::ISequence&      aSequence,
        u_int32_t                       aOffsetInSequence,
        database::ISequenceDatabase*    aDatabase,
        u_int64_t                       aOffsetInDatabase,
        const database::IWord&          aNeighbourhood
    )
        : sequence          (aSequence),
          offsetInSequence  (aOffsetInSequence),
          database          (aDatabase),
          offsetInDatabase  (aOffsetInDatabase),
          neighbourhood     (aNeighbourhood)
    {
    }

    /** Clone of the instance. */
    ISeedOccurrence* clone () const
    {
        return new ISeedOccurrence (sequence, offsetInSequence, database, offsetInDatabase, neighbourhood);
    }
};

/********************************************************************************/

/** Iterates over seed occurrences for a given seed in a given sequence.
 */
class IOccurrenceIterator : public dp::Iterator<const ISeedOccurrence*>
{
public:

    /** Implementation of the Iterator interface. */
    virtual void first() = 0;
    virtual dp::IteratorStatus  next() = 0;
    virtual bool isDone()  = 0;
    virtual const ISeedOccurrence* currentItem() = 0;
};

/********************************************************************************/

/** Iterates over seed occurrences for a given seed in a given sequence.
 *  Returned items are blocks (ie vectors) of ISeedOccurrence instances.
 */
class IOccurrenceBlockIterator : public dp::Iterator<os::Vector<const ISeedOccurrence*>& >
{
public:

    /** Implementation of the Iterator interface. */
    virtual void first() = 0;
    virtual dp::IteratorStatus  next() = 0;
    virtual bool isDone()  = 0;
    virtual os::Vector<const ISeedOccurrence*>& currentItem() = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _IOCCURRENCE_ITERATOR_HPP_  */
