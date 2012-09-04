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
 */
class DatabaseNucleotidIndex : public AbstractDatabaseIndex
{
public:

    /** Constructor.
     * \param[in] database : the database to be indexed.
     * \param[in] model : the seed model to be used for indexation.
     */
	DatabaseNucleotidIndex (database::ISequenceDatabase* database, seed::ISeedModel* model);
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

protected:

    /** The index itself. Defined as a vector of vectors. */
    std::vector <IndexEntry>  _index;

    u_int32_t* _counter;

    /* Shortcut & optimization. */
    size_t  _span;
    int32_t _bitshift;

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
        ) : IteratorCommand (it), _ref(ref)  {}

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
            : IteratorCommand (it), _ref(ref){}

        void execute (const database::ISequence*& currentSequence, size_t& nbGot)
        {
            _ref->fillSeedsOccurrences (currentSequence);
        }
    };

    /** */
    void countSeedsOccurrences (const database::ISequence*& sequence);
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
        seed::ISeedModel*            model
    )
    {
        return new DatabaseNucleotidIndex (database, model);
    }
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _DATABASE_NUCLEOTID_INDEX_HPP_  */
