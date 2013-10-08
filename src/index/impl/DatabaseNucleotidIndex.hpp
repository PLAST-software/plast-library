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

#ifndef _DATABASE_NUCLEOTID_INDEX_HPP_
#define _DATABASE_NUCLEOTID_INDEX_HPP_

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
class DatabaseNucleotidIndex : public AbstractDatabaseIndex
{
public:

    /** Constructor.
     * \param[in] database : the database to be indexed.
     * \param[in] model : the seed model to be used for indexation.
     */
	DatabaseNucleotidIndex (database::ISequenceDatabase* database, seed::ISeedModel* model, IDatabaseIndex*  otherIndex);
    virtual ~DatabaseNucleotidIndex ();

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

protected:

    /** Shortcut. */
    typedef std::vector<SeedOccurrence> IndexEntry;

    /** The index itself. Defined as a vector of vectors. */
    std::vector <IndexEntry>  _index;

    u_int32_t* _counter;

    /* Shortcut & optimization. */
    size_t  _span;
    int32_t _bitshift;

    typedef u_int64_t word_t;

    word_t* _maskIn;
    word_t* _maskOut;

    /*********************************************************************************/
    /** We create a command that will count the number of occurrences for each seed. */
    class CountSeedsCmd : public dp::impl::IteratorCommand<const database::ISequence*>
    {
    private:
        DatabaseNucleotidIndex* _ref;

    public:
        CountSeedsCmd (
            dp::impl::IteratorGet<const database::ISequence*>* it,
            DatabaseNucleotidIndex* ref
        ) : dp::impl::IteratorCommand<const database::ISequence*> (it), _ref(ref)  {}

        void execute (const database::ISequence*& currentSequence, size_t& nbGot)
        {
            _ref->countSeedsOccurrences (currentSequence);
        }
    };

    /*********************************************************************************/
    /** We create a command that will fill the occurrences index structure for each seed. */
    class FillSeedsCmd : public dp::impl::IteratorCommand<const database::ISequence*>
    {
    private:
        DatabaseNucleotidIndex* _ref;

    public:
        FillSeedsCmd (dp::impl::IteratorGet<const database::ISequence*>* it, DatabaseNucleotidIndex* ref)
            : dp::impl::IteratorCommand<const database::ISequence*> (it), _ref(ref){}

        void execute (const database::ISequence*& currentSequence, size_t& nbGot)
        {
            _ref->fillSeedsOccurrences (currentSequence);
        }
    };

    /** Count the number of occurrences for each seed found in the provided sequence. This method
     * updates the '_counter' attribute of the current instance.
     * \param[in] sequence : the sequence to extract the seeds occurrences number from.
     */
    void countSeedsOccurrences (const database::ISequence*& sequence);

    /** Fill the index with the occurrences information for each seed found in the provided sequence.
     * This method updates the '_index' attribute of the current instance. It also relies on the '_counter'
     * attribute for finding the correct cell to be filled in the '_index' attribute.
     * \param[in] sequence : the sequence to extract the seeds occurrences number from.
     */
    void fillSeedsOccurrences  (const database::ISequence*& sequence);
};

/********************************************************************************/

/** \brief Factory that creates DatabaseIndexCodonStopOptim instances.
  */
class DatabaseNucleotidIndexFactory : public IDatabaseIndexFactory
{
public:

    /** \copydoc IDatabaseIndexFactory::newDatabaseIndex */
    IDatabaseIndex* newDatabaseIndex (
        database::ISequenceDatabase* database,
        seed::ISeedModel*            model,
        IDatabaseIndex*              otherIndex
    )
    {
        return new DatabaseNucleotidIndex (database, model, otherIndex);
    }
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _DATABASE_NUCLEOTID_INDEX_HPP_  */
