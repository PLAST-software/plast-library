/*
 * Database.hpp
 *
 *  Created on: 7 juin 2011
 *      Author: edrezen
 */

#ifndef _DATABASE_INDEX_HASH_CODED_HPP_
#define _DATABASE_INDEX_HASH_CODED_HPP_

/********************************************************************************/

#include "AbstractDatabaseIndex.hpp"
#include "ISeed.hpp"
#include <list>
#include <map>
#include <stdio.h>

/********************************************************************************/
namespace indexation {
/********************************************************************************/

/********************************************************************************/

class DatabaseIndex : public AbstractDatabaseIndex
{
public:

    /** */
    DatabaseIndex (database::ISequenceDatabase* database, seed::ISeedModel* model);
    virtual ~DatabaseIndex ();

    /** Builds the index. */
    void build ();

    /** Creates an iterator on occurrences for a given seed key. */
    IOccurrenceIterator* createOccurrenceIterator (const seed::ISeed* seed, size_t neighbourhoodSize=0);

    /** */
    IOccurrenceBlockIterator* createOccurrenceBlockIterator (
        const seed::ISeed* seed,
        size_t neighbourhoodSize,
        size_t blockSize
    );

    /** Returns the number of occurrences for a given seed. */
    size_t getOccurrenceNumber (const seed::ISeed* seed);

    /** Merge children indexes. */
    void merge (void);

    /** */
    void dump (void);

private:

    typedef u_int32_t SequenceOffset;
    typedef std::vector<SequenceOffset> IndexEntry;

    typedef u_int32_t SeedHashCode;
    SeedHashCode getHashCode (const database::IWord& kmer);

    /** The index itself. */
    std::vector <IndexEntry>  _index;

    /** Current sequence parsed during index build. */
    const database::ISequence* _currentSequence;

    /** Shortcut & optimization. */
    size_t _span;
    size_t _alphabetSize;

    u_int64_t _sequenceOffset;

    /** */
    void iterateSeed (const seed::ISeed* seed);


    /********************************************************************************/
    class DatabaseOccurrenceIterator : public IOccurrenceIterator
    {
    public:
        DatabaseOccurrenceIterator (database::ISequenceDatabase* database, size_t span, IndexEntry* offsets, size_t neighbourSize)
            : _database(database), _span(span), _offsets(offsets), _neighbourSize(neighbourSize), _item(span+2*neighbourSize)
        {
            _item.database = database;
            _lastIdx       = _offsets->size() - 1;
        }

        void first()
        {
            _currentIdx = 0;
            if (!isDone())  {  updateItem ();  }
        }

        dp::IteratorStatus next()
        {
            _currentIdx ++;
            if (!isDone())
            {
                updateItem ();
                return dp::ITER_ON_GOING;
            }
            else
            {
                return dp::ITER_DONE;
            }
        }

        bool isDone()  { return _currentIdx > _lastIdx; }

        const ISeedOccurrence* currentItem()    { return &_item;    }

    private:
        database::ISequenceDatabase* _database;
        size_t                       _span;
        IndexEntry*                  _offsets;
        size_t                       _neighbourSize;
        ISeedOccurrence              _item;
        size_t                       _currentIdx;
        size_t                       _lastIdx;

        void updateItem ();
    };

    /********************************************************************************/
    class DatabaseOccurrenceBlockIterator : public IOccurrenceBlockIterator
    {
    public:
        DatabaseOccurrenceBlockIterator (
            database::ISequenceDatabase* database,
            size_t span,
            IndexEntry* offsets,
            size_t neighbourSize,
            size_t blockSize
        );

        ~DatabaseOccurrenceBlockIterator ();

        void first()  {  _vectorsListIterator = _vectorsList.begin();  }

        dp::IteratorStatus next()
        {
            _vectorsListIterator ++;
            return dp::ITER_UNKNOWN;
        }

        bool isDone()  { return _vectorsListIterator ==  _vectorsList.end(); }

        os::Vector<const indexation::ISeedOccurrence*>& currentItem()    { return *_vectorsListIterator;    }

    private:
        database::ISequenceDatabase* _database;
        size_t                       _span;
        IndexEntry*                  _offsets;
        size_t                       _neighbourSize;
        size_t                       _blockSize;
        size_t                       _neighbourTotalSize;

        os::Vector<const indexation::ISeedOccurrence*>  _occurrences;

        std::list<os::Vector<const indexation::ISeedOccurrence*> >           _vectorsList;
        std::list<os::Vector<const indexation::ISeedOccurrence*> >::iterator _vectorsListIterator;

        indexation::ISeedOccurrence* _table;
        database::LETTER*            _neighbourhoods;
    };
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _DATABASE_INDEX_HASH_CODED_HPP_  */
