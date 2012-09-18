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

/** \file IDatabaseIndex.hpp
 *  \brief Definition of interfaces for genomic database indexation.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _IDATABASE_INDEX_HPP_
#define _IDATABASE_INDEX_HPP_

/********************************************************************************/

#include <seed/api/ISeedModel.hpp>
#include <database/api/ISequenceDatabase.hpp>
#include <index/api/IOccurrenceIterator.hpp>

/********************************************************************************/
/** \brief Genomic database indexation concepts. */
namespace indexation {
/********************************************************************************/

/** \brief Interface of an index for a database.
 *
 *  PLAST algorithm relies on an initial indexation of the two genomic databases
 *  to be compared.
 *
 *  An index in this context is a seed, ie. a short word of nucleotides or amino acids.
 *
 *  We need also a way to enumerate all the seeds that can serve as indexes. For this
 *  purpose, we use a seeds model that is able to iterate all wanted seeds.
 *
 *  Having a database and a seed model, it becomes easy process the database indexation:
 *  for each seed, memorize all its occurrences in the database. The actual indexation
 *  work should be done in the build() method of the IDatabaseIndex interface.
 *
 *  As a result, the IDatabaseIndex interface provides an efficient way to retrieve all
 *  occurrences of a given seed by creating IOccurrenceBlockIterator instances.
 *  This kind of iterator will iterate over ISeedOccurrence instances; such an instance
 *  gathers information about the seed, the sequence where it occurs, the offset of
 *  the occurrence within the sequence and the left and right sequence neighborhoods of
 *  the seed occurrence.
 *
 *  Note: actually, the IDatabaseIndex interface can create two kinds of iterator: the first
 *  iterator (IOccurrenceIterator) will iterate over one occurrence at a time,
 *  the second iterator (IOccurrenceBlockIterator) will iterate over a vector of
 *  occurrences at a time. This second kind has been introduced for optimization matter
 *  since it allows to retrieve more information in a single shot.
 *
 *  From here, we can give the sketch of the PLAST algorithm in its globality: it builds
 *  the indexes of the subject and query databases for a given seed model, then for each
 *  seed of the model, we can reach all the couples [occur in subject, occur in query]
 *  from which PLAST begins its scores computations.
 *
 *  Note that an index can be composed of sub indexes (addChildIndex() and removeChildIndex()
 *  methods). This is useful for parallelization of the index building: one splits the database
 *  into small parts, we build an index for each part and merge the result in a final
 *  IDatabaseIndex instance.
 *
 *  \see seed::ISeed
 *  \see seed::ISeedModel
 *  \see ISeedOccurrence
 */
class IDatabaseIndex : public dp::SmartPointer
{
public:

    /** Data type for storing a list of <offsets,sequenceIndex>. Note that this choice may lead
     *  to huge usage of memory, ie. if we note N the size in bytes of the database to be indexated,
     *  we will need about 8.N bytes for storing the full index, namely for each seed occurrence:
     *    - we need 4 bytes for its offset in the database
     *    - we need 4 bytes for the index of the sequence it belongs to.
     *
     *  Note that the sequence index may be computed from the offset in database (by dichotomy), but this
     *  process may take a lot of time. So, the presence of 'sequenceIdx' is here for speeding up the
     *  algorithm, with the memory big usage drawback.
     *
     *  If we cope with the time to recompute sequence index, we would use about 4.N bytes, but we can do
     *  better: instead of keeping the absolute offset in the whole database, we can keep the difference
     *  between two offsets, number likely to be lower that the offset itself and hopefully this number
     *  could be coded on 2 bytes instead of 4. In such a scheme, this difference could be big enough to
     *  require a 4 bytes storage. We could then keep the most significant bit to tell if the difference
     *  requires 2 or 4 bytes of storage. A crude estimation shows that we could need something lower than
     *  3.N (maybe close to 2.5 N)
     */
    struct SeedOccurrence
    {
        SeedOccurrence () : offsetInDatabase(0), sequenceIdx(0) {}
        SeedOccurrence (u_int32_t off, u_int32_t seqIdx) : offsetInDatabase(off), sequenceIdx(seqIdx) {}

        u_int32_t  offsetInDatabase;
        u_int32_t  sequenceIdx;
    };

    /** Data type for storing a list of offsets. */
    typedef std::vector<SeedOccurrence> IndexEntry;

    /** Returns the sequences database.
     * \return the database.
     */
    virtual database::ISequenceDatabase* getDatabase () = 0;

    /** Returns the seed model used for the indexation.
     * \return the seed model.
     */
    virtual seed::ISeedModel* getModel () = 0;

    /** Builds the index for the database with the choosen seed model. */
    virtual void build () = 0;

    /** Creates an iterator on occurrences for a given seed key.
     * \param[in] seed : the seed we want to iterate all occurrences
     * \param[in] neighbourhoodSize : size (in characters) of the left and right neighbourhoods
     * \return the IOccurrenceIterator instance
     * \see ISeedOccurrence
     */
    virtual IOccurrenceIterator* createOccurrenceIterator (const seed::ISeed* seed, size_t neighbourhoodSize=0) = 0;

    /** Creates an iterator on occurrences (retrieved by blocks) for a given seed key.
     * \param[in] seed : the seed we want to iterate all occurrences
     * \param[in] neighbourhoodSize : size (in characters) of the left and right neighbourhoods
     * \param[in] blockSize : number of occurrences retrieved at each iteration step
     * \see ISeedOccurrence
     */
    virtual IOccurrenceBlockIterator* createOccurrenceBlockIterator (
        const seed::ISeed* seed,
        size_t neighbourhoodSize,
        size_t blockSize
    ) = 0;

    /** Returns the entries for a given seed.
     * param[in] seed : the seed for which we want to know the occurrences number
     * \return the entries
     */
    virtual IndexEntry& getEntry (const seed::ISeed* seed) = 0;

    /** Returns the number of occurrences for a given seed.
     * param[in] seed : the seed for which we want to know the occurrences number
     * \return the seed occurrences number
     */
    virtual size_t getOccurrenceNumber (const seed::ISeed* seed) = 0;

    /** Returns the number of occurrences for all seeds.
     * \return the seed occurrences number
     */
    virtual u_int64_t getTotalOccurrenceNumber () = 0;

    /** Add a child index (Design Pattern Composite).
     * \param[in] child : the child to be added to the current index
     */
    virtual void addChildIndex    (IDatabaseIndex* child) = 0;

    /** Remove a child index (Design Pattern Composite).
     * \param[in] child : the child to be removed from the current index
     */
    virtual void removeChildIndex (IDatabaseIndex* child) = 0;

    /** Merge children indexes. */
    virtual void merge (void) = 0;

    /** Return properties about the instance.
     * \param root : the root string
     * \return a created IProperties instance.
     */
    virtual dp::IProperties* getProperties (const std::string& root) = 0;
};

/********************************************************************************/

/** \brief Interface of a factory for creating IDatabaseIndex instances
 */
class IDatabaseIndexFactory : public dp::SmartPointer
{
public:

    /** Creates a new instance of IDatabaseIndexFactory
     */
    virtual IDatabaseIndex* newDatabaseIndex (
        database::ISequenceDatabase* database,
        seed::ISeedModel*            model
    ) = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _IDATABASE_INDEX_HPP_  */
