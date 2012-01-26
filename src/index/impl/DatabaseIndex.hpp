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

/** \file DatabaseIndex.hpp
 *  \brief Implemtation of interfaces for genomic database indexation.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _DATABASE_INDEX_HPP_
#define _DATABASE_INDEX_HPP_

/********************************************************************************/

#include <index/impl/AbstractDatabaseIndex.hpp>
#include <seed/api/ISeed.hpp>
#include <misc/api/Vector.hpp>

#include <list>
#include <map>
#include <stdio.h>

/********************************************************************************/
namespace indexation {
/** \brief Implementation of genomic database indexation concepts. */
namespace impl {
/********************************************************************************/

/** \brief Simple implementation of the IDatabaseIndex interface.
 *
 * This implementation is based on seeds hash code, ie. an integer that identifies
 * a seed.
 *
 * Such an integer is used as a key of a vector whose values are the lists of occurrences
 * (in the database) for a given seed.
 *
 * So, the index data is a vector of vectors.
 *
 * The size of this data is:
 *    - proportional to the number of seeds found in the database (roughly the size of the database itself)
 *    - proportional to the data type size for storing a seed occurrence in the database.
 *
 * Since genomic databases may be huge, the data type for storing a seed occurrence must be able to hold
 * big integers values; right now, the data type choice is u_int32_t which means that we can deal with databases
 * of about 4 GBytes. We could use u_int64_t instead but it would imply a big memory usage (twice the size
 * used with u_int32_t). Note that we could make this class a template one with the template type being
 * u_int32_t or u_int64_t for instance.
 *
 * Note that a smarter implementation could be implemented by storing not the absolute offsets in the database
 * (which is memory expensive), but rather relative offsets (ie. difference between two successive offsets);
 * in this case, the data type for storing these (relative) offsets could be u_int16_t (and adding 'fake' offsets
 * if needed in case when two successive offsets are separated by more than 2^16 characters).
 */
class DatabaseIndex : public AbstractDatabaseIndex
{
public:

    /** Constructor.
     * \param[in] database : the database to be indexed.
     * \param[in] model : the seed model to be used for indexation.
     */
    DatabaseIndex (database::ISequenceDatabase* database, seed::ISeedModel* model);
    virtual ~DatabaseIndex ();

    /** \copydoc AbstractDatabaseIndex::build */
    void build ();

    /** \copydoc AbstractDatabaseIndex::createOccurrenceIterator */
    IOccurrenceIterator* createOccurrenceIterator (const seed::ISeed* seed, size_t neighbourhoodSize=0);

    /** \copydoc AbstractDatabaseIndex::createOccurrenceBlockIterator */
    IOccurrenceBlockIterator* createOccurrenceBlockIterator (
        const seed::ISeed* seed,
        size_t neighbourhoodSize,
        size_t blockSize
    );

    /** \copydoc AbstractDatabaseIndex::getOccurrenceNumber */
    size_t getOccurrenceNumber (const seed::ISeed* seed);

    /** \copydoc AbstractDatabaseIndex::getTotalOccurrenceNumber */
    u_int64_t getTotalOccurrenceNumber ();

    /** \copydoc AbstractDatabaseIndex::merge */
    void merge (void);

protected:

    /** Data type for storing an seed offset. */
    typedef u_int32_t SequenceOffset;

    /** Data type for storing a lsit of offsets. */
    typedef std::vector<SequenceOffset> IndexEntry;

    /** The index itself. Defined as a vector of vectors. */
    std::vector <IndexEntry>  _index;

    /** Data type that holds a seed hash code. */
    typedef u_int32_t SeedHashCode;

    /** Compute a seed hash code. Can be called in case a ISeed instance has no defined hash code
     * (probably computed by the iterator that provides it)
     * \param[in] kmer : the data for which we want a hash code
     * \return the hash code.
     */
    SeedHashCode getHashCode (const database::IWord& kmer);

    /** Current sequence parsed during index build. */
    const database::ISequence* _currentSequence;

    /* Shortcut & optimization. */
    size_t _span;
    size_t _alphabetSize;

    /** Offset in the database of the current sequence. */
    u_int64_t _sequenceOffset;

    /** Callback to be called each time a seed is found in some data (sequence data for instance).
     * \param[in] seed : the iterated seed
     */
    void iterateSeed (const seed::ISeed* seed);

    /** */
    Offset _lastValidOffset;

    /** */
    virtual seed::ISeedIterator* createSeedsIterator (const database::IWord& data);

    /********************************************************************************/
    /** \brief IOccurrenceIterator implementation used by DatabaseIndex::createOccurrenceIterator class.
     */
    class DatabaseOccurrenceIterator : public IOccurrenceIterator
    {
    public:
        DatabaseOccurrenceIterator (database::ISequenceDatabase* database, size_t span, IndexEntry* offsets, size_t neighbourSize)
            : _database(database), _span(span), _offsets(offsets), _neighbourSize(neighbourSize), _item(span+2*neighbourSize)
        {
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
    /** \brief IOccurrenceIterator implementation used by DatabaseIndex::createOccurrenceBlockIterator class.
     */
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

        misc::Vector<const indexation::ISeedOccurrence*>& currentItem()    { return *_vectorsListIterator;    }

        database::LETTER* getNeighbourhoods ()  { return _neighbourhoods; }

    private:
        database::ISequenceDatabase* _database;
        size_t                       _span;
        IndexEntry*                  _offsets;
        size_t                       _neighbourSize;
        size_t                       _blockSize;
        size_t                       _neighbourTotalSize;

        misc::Vector<const indexation::ISeedOccurrence*>  _occurrences;

        std::list<misc::Vector<const indexation::ISeedOccurrence*> >           _vectorsList;
        std::list<misc::Vector<const indexation::ISeedOccurrence*> >::iterator _vectorsListIterator;

        indexation::ISeedOccurrence* _table;
        database::LETTER*            _neighbourhoods;
    };
};

/********************************************************************************/

/** \brief Simple implementation of the IDatabaseIndex interface.
 *
 * This implementation is based on seeds hash code, ie. an integer that identifies
 * a seed.
 *
 * It also tries to filter out some unwanted index because of presence of STOP codon.
 */
class DatabaseIndexCodonStopOptim : public DatabaseIndex
{
public:

    /** Constructor.
     * \param[in] database : the database to be indexed.
     * \param[in] model : the seed model to be used for indexation.
     */
    DatabaseIndexCodonStopOptim (
        database::ISequenceDatabase* database,
        seed::ISeedModel* model,
        size_t skipRange
    ) : DatabaseIndex (database, model), _range(skipRange)  {}

protected:

    /** */
    size_t _range;

    /** */
    seed::ISeedIterator* createSeedsIterator (const database::IWord& data);
};

/********************************************************************************/

/** \brief Factory that creates DatabaseIndexCodonStopOptim instances.
  */
class DatabaseIndexFactory : public IDatabaseIndexFactory
{
public:

    /** \copydoc IDatabaseIndexFactory::newDatabaseIndex */
    IDatabaseIndex* newDatabaseIndex (
        database::ISequenceDatabase* database,
        seed::ISeedModel*            model
    )
    {
        return new DatabaseIndex (database, model);
    }
};

/********************************************************************************/

/** \brief Factory that creates DatabaseIndexCodonStopOptim instances.
  */
class DatabaseIndexCodonStopOptimFactory : public IDatabaseIndexFactory
{
public:

    /** Constructor. */
    DatabaseIndexCodonStopOptimFactory (size_t range)  : _range(range) {}

    /** \copydoc IDatabaseIndexFactory::newDatabaseIndex */
    IDatabaseIndex* newDatabaseIndex (
        database::ISequenceDatabase* database,
        seed::ISeedModel*            model
    )
    {
        return new DatabaseIndexCodonStopOptim (database, model, _range);
    }

private:

    /** */
    size_t _range;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _DATABASE_INDEX_HPP_  */
