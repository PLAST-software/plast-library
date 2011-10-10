/*
 * Database.hpp
 *
 *  Created on: 7 juin 2011
 *      Author: edrezen
 */

#ifndef _DATABASE_INDEX_HPP_
#define _DATABASE_INDEX_HPP_

/********************************************************************************/

#include "AbstractDatabaseIndex.hpp"
#include <list>
#include <map>
#include <stdio.h>

/** Forward declarations. */
struct _retr_t;

/********************************************************************************/
namespace indexation {
/********************************************************************************/

/** Define the offset of a seed occurrence in the database. */
typedef u_int32_t SequenceOffset;

/********************************************************************************/

class DatabaseIndexHash : public AbstractDatabaseIndex
{
public:

    /** */
    DatabaseIndexHash (database::ISequenceDatabase* database, seed::ISeedModel* model);
    virtual ~DatabaseIndexHash ();

    /** Builds the index. */
    void build ();

    /** Creates an iterator on occurrences for a given seed key. */
    IOccurrenceIterator* createOccurrenceIterator (const seed::ISeed* seed, size_t neighbourhoodSize);

    /** */
    IOccurrenceBlockIterator* createOccurrenceBlockIterator (
        const seed::ISeed* seed,
        size_t neighbourhoodSize,
        size_t blockSize
    );

    /** Returns the number of occurrences for a given seed. */
    size_t getOccurrenceNumber (const seed::ISeed* seed)  { return 0; }

    /** Merge children indexes. */
    void merge (void);

private:

    typedef std::vector<SequenceOffset> IndexEntry;

    /** The index itself as a table of IndexEntry instances. */
    unsigned int* _index;
    std::vector<IndexEntry*> _indexVector;

    /** */
    void dump (void);

    /** */
    struct _retr_t* _hashTable;
    void initHashTable ();

    class DatabaseOccurrenceIterator : public IOccurrenceIterator
    {
    public:
        DatabaseOccurrenceIterator (database::ISequenceDatabase* database, IndexEntry* offsets, size_t deltaSequence)
            : _database(database), _offsets(offsets), _deltaSequence(deltaSequence)
        {
            _item.database = database;
        }

        void first()
        {
            _currentIdx = 0;
            _lastIdx    = _offsets->size();

            if (!isDone())  {  _item.offsetInDatabase  = (*_offsets)[_currentIdx];  }
        }

        dp::IteratorStatus next()
        {
            _currentIdx ++;
            if (_currentIdx < _lastIdx)  {  _item.offsetInDatabase  = (*_offsets)[_currentIdx];  }
            return dp::ITER_UNKNOWN;
        }

        bool isDone()  { return _currentIdx >= _lastIdx; }

        const ISeedOccurrence* currentItem()    { return &_item;    }

    private:
        database::ISequenceDatabase* _database;
        ISeedOccurrence              _item;
        size_t _currentIdx;
        size_t _lastIdx;
        IndexEntry*                  _offsets;
        size_t                       _deltaSequence;

        void reset (IndexEntry* offsets, size_t deltaSequence)
        {
            _offsets       = offsets;
            _deltaSequence = deltaSequence;
            _item.database = _database;
        }

        friend class DatabaseIndexHash;
        friend class SeedHitIterator;  // for optimization
    };

    friend class DatabaseOffsetIterator;  // need to access the index guts.
    friend class SeedHitIterator;  // for optimization
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _DATABASE_INDEX_HPP_  */
