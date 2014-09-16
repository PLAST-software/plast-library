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

#ifndef _DATABASE_NUCLEOTID_INDEX_OPTIM_HPP_
#define _DATABASE_NUCLEOTID_INDEX_OPTIM_HPP_

/********************************************************************************/

#include <index/impl/AbstractDatabaseIndex.hpp>
#include <seed/api/ISeed.hpp>
#include <misc/api/Vector.hpp>
#include <designpattern/impl/IteratorGet.hpp>

#include <list>
#include <map>
#include <stdio.h>

/********************************************************************************/
namespace indexation {
/** \brief Implementation of genomic database indexation concepts. */
namespace impl {
/********************************************************************************/

/** \brief Nucleotide implementation of the IDatabaseIndex interface.
 *
 * The indexation here is done in three steps:
 *   - 1) counting the occurrences number for each possible seed.
 *   - 2) resizing the structures with the found occurrences numbers
 *   - 3) filling the structures for all possible seeds.
 *
 * The interesting remark here is that steps 1 and 3 can be parallelized. Indeed, once the
 * structures are correctly sized in step 2, all found occurrences can be filled in the
 * related structures, each occurrence using a specific memory location. The only potential
 * issue is to determine this specific memory location. Here, care must be taken about the
 * concurrent accesses from the different threads (done with intrinsics '__sync_fetch_and_add')
 *
 * This implementation works for nucleotides databases but could be generalized for
 * amino acids databases. Right now, the only difficulty for doing so is that the nucleotides
 * hash codes can be easily (and efficiently) computed on the fly, which is not the case for
 * the subset seeds models.
 *
 *  The parallelization scheme is the following: each sequence data to be indexated is done in a
 *  specific thread, so if we have 8 cores, we can deal with 8 sequences at the same time. For this
 *  purpose, we use a single IteratorGet instance that iterates on the sequences of the provided database.
 *  The N commands that do the actual indexation will use this single iterator and that's the way we get
 *  our parallelization scheme.
 */
class DatabaseNucleotidIndexOptim : public AbstractDatabaseIndex
{
public:

    /** Constructor.
     * \param[in] database : the database to be indexed.
     * \param[in] model : the seed model to be used for indexation.
     * \param[in] otherIndex : the index which is used to filtered the current database (query index for example)
     * \param[in] dispatcher : the dispatcher to be used for multi threading.
     */
	DatabaseNucleotidIndexOptim (database::ISequenceDatabase* database, seed::ISeedModel* model, IDatabaseIndex*  otherIndex, dp::ICommandDispatcher* dispatcher);
    virtual ~DatabaseNucleotidIndexOptim ();

    /** \copydoc AbstractDatabaseIndex::build */
    void build ();

    /** \copydoc AbstractDatabaseIndex::createOccurrenceIterator */
    IOccurrenceIterator* createOccurrenceIterator (const seed::ISeed* seed, size_t neighbourhoodSize=0) { return 0; }

    /** \copydoc AbstractDatabaseIndex::createOccurrenceBlockIterator */
    IOccurrenceBlockIterator* createOccurrenceBlockIterator (
        const seed::ISeed* seed,
        size_t neighbourhoodSize,
        size_t blockSize
    )  { return 0; }

    /** \copydoc AbstractDatabaseIndex::getEntry */
    IndexEntry& getEntry (const seed::ISeed* seed);

    /** \copydoc AbstractDatabaseIndex::getOccurrenceNumber */
    size_t getOccurrenceNumber (const seed::ISeed* seed);

    /** \copydoc AbstractDatabaseIndex::getTotalOccurrenceNumber */
    u_int64_t getTotalOccurrenceNumber ();

    /** \copydoc AbstractDatabaseIndex::merge */
    void merge (void) {}

    u_int8_t* getMask ()  { return (u_int8_t*) _maskOut; }

    /**  */
    typedef u_int64_t word_t;

    /** */
    void setDatabase (database::ISequenceDatabase* database);

protected:

    /** */
    struct IndexEntry : public IDatabaseIndex::IndexEntry
    {
        IndexEntry (size_t size=0, SeedOccurrence* occurs=0)  : _size(size), _occurs(occurs) {}
        size_t          _size;
        SeedOccurrence* _occurs;

        size_t size() const { return _size; }
        SeedOccurrence& operator[] (size_t idx)  { return _occurs[idx]; }

        bool empty()  { return _size == 0; }

        void push_back (const SeedOccurrence& item) {}
        void clear () {}
    };


    u_int32_t* _counter;

    /* Shortcut & optimization. */
    size_t  _span;
    size_t  _extraSpan;
    int32_t _bitshift;

    word_t* _maskIn;
    word_t* _maskOut;

    dp::ICommandDispatcher* _dispatcher;
    void setDispatcher (dp::ICommandDispatcher* dispatcher)  { SP_SETATTR(dispatcher); }

    SeedOccurrence* _occurrences;
    u_int64_t       _occurrencesSize;

    /** The index itself. Defined as a vector of vectors. */
    IndexEntry*     _index;

    IDatabaseIndex* _otherIndex;

    bool _isBuilt;

    friend struct CountFunctor;
    friend struct FillFunctor;
    template <typename Functor> friend class SequenceSeedsCmd;
};

/********************************************************************************/

/** \brief Factory that creates DatabaseIndexCodonStopOptim instances.
  */
class DatabaseNucleotidIndexOptimFactory : public IDatabaseIndexFactory
{
public:

    /** \copydoc IDatabaseIndexFactory::newDatabaseIndex */
    IDatabaseIndex* newDatabaseIndex (
        database::ISequenceDatabase* database,
        seed::ISeedModel*            model,
        IDatabaseIndex*              otherIndex,
        dp::ICommandDispatcher* 	 dispatcher
    )
    {
        return new DatabaseNucleotidIndexOptim (database, model, otherIndex, dispatcher);
    }
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _DATABASE_NUCLEOTID_INDEX_OPTIM_HPP_  */
