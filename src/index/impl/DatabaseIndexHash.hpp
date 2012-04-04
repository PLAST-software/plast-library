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

/** \file DatabaseIndexHash.hpp
 *  \brief Implementation of interfaces for genomic database indexation.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _DATABASE_INDEX_HASH_HPP_
#define _DATABASE_INDEX_HASH_HPP_

/********************************************************************************/

#include <index/impl/AbstractDatabaseIndex.hpp>

#include <list>
#include <map>
#include <stdio.h>

/** Forward declarations. */
struct _retr_t;

/********************************************************************************/
namespace indexation {
/** \brief Implementation of genomic database indexation concepts. */
namespace impl {
/********************************************************************************/

/** Define the offset of a seed occurrence in the database. */
typedef u_int32_t SequenceOffset;

/********************************************************************************/

/** \brief Implementation of IDatabaseIndex by means of perfect hash tables. DON'T USE IT, NEEDS IMPROVEMENTS
 *
 * WARNING !!! This implementation must be improved before using it.
 */
class DatabaseIndexHash : public AbstractDatabaseIndex
{
public:

    DatabaseIndexHash (database::ISequenceDatabase* database, seed::ISeedModel* model);
    virtual ~DatabaseIndexHash ();

    void build ();

    IOccurrenceIterator* createOccurrenceIterator (const seed::ISeed* seed, size_t neighbourhoodSize);

    IOccurrenceBlockIterator* createOccurrenceBlockIterator (
        const seed::ISeed* seed,
        size_t neighbourhoodSize,
        size_t blockSize
    );

    size_t getOccurrenceNumber (const seed::ISeed* seed)  { return 0; }

    u_int64_t getTotalOccurrenceNumber () { return 0; }

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
        }

        friend class DatabaseIndexHash;
        friend class SeedHitIterator;  // for optimization
    };

    friend class DatabaseOffsetIterator;  // need to access the index guts.
    friend class SeedHitIterator;  // for optimization
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _DATABASE_INDEX_HASH_HPP_  */
