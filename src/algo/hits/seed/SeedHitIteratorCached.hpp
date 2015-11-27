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

/** \file SeedHitIteratorCached.hpp
 *  \brief Implementation of IHitIterator interface from databases indexations
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _SEED_HITITERATOR_CACHED_HPP_
#define _SEED_HITITERATOR_CACHED_HPP_

/********************************************************************************/

#include <designpattern/impl/ProductIterator.hpp>

#include <index/api/IDatabaseIndex.hpp>
#include <index/impl/DatabaseIndex.hpp>

#include <algo/hits/seed/SeedHitIterator.hpp>

/********************************************************************************/
namespace algo   {
namespace hits   {
/** \brief Implementations for IHitIterator interface from databases indexations. */
namespace seed   {
/********************************************************************************/

/** \brief Hits iterator from two database indexes with a memory cache
 *
 * This implementation is a memory cached improvement of the SeedHitIterator class.
 *
 * For a given seed, one retrieves a IOccurrenceBlockIterator instance for the subject
 * database and another one for the query database.
 *
 * Then, the actual Hit iteration can be carried out by using the information within
 * these two IOccurrenceBlockIterator instances.
 *
 * This is an improvement compared to SeedHitIterator, where the same information may
 * have to be retrieved several times.
 *
 * This implementation uses the seed iterator through the seed::ISeedIterator::retrieve()
 * method. Since the same seed iterator instance is used by different SeedHitIterator
 * instances (because of the parallelization scheme), it is important that the
 * seed::ISeedIterator::retrieve() method is robust with regard to concurrent thread accesses.
 */
class SeedHitIteratorCached : public SeedHitIterator
{
public:

    /** \copydoc SeedHitIterator::SeedHitIterator */
    SeedHitIteratorCached (
        indexation::IDatabaseIndex* indexSubject,
        indexation::IDatabaseIndex* indexQuery,
        size_t                      neighbourhoodSize,
        float                       seedsUseRatio,
        bool&                       isRunning,
        ::seed::ISeedIterator*      seedIterator=0
    );

    /** Destructor. */
    virtual ~SeedHitIteratorCached ();

    /** \copydoc SeedHitIterator::getName */
    const char* getName ()  { return "SeedHitIteratorCached"; }

    /** \copydoc SeedHitIterator::iterate */
    void iterate (void* aClient, Method method);

    /** \copydoc SeedHitIterator::split */
    std::vector<IHitIterator*> split (size_t nbSplit);

protected:

    /** Sort the seeds to be iterated.
     */
    virtual void sortSeeds (void) { /* does nothing by default. */  }

private:

    /** \copydoc SeedHitIterator::clone */
    SeedHitIterator* clone (::seed::ISeedIterator* seedIterator)
    {
        return new SeedHitIteratorCached (_indexDb1, _indexDb2, _neighbourhoodSize, _seedsUseRatio, _isRunning, seedIterator);
    }
};

/********************************************************************************/

/** \brief Hits iterator from two database indexes with optimized seeds order
 *
 * By default, the ISeedModel interface creates ISeedIterator instances that iterates
 * seeds with an alphabetic order.
 *
 * Actually, the last iterated seed (say "YYYY" for instance) may be have a big number
 * of occurrences in the two databases, so processing this last seed may take a long time.
 *
 * If we process one seed at a time, this is no big deal: the algorithm will take a long time
 * for processing this last seed. However, if we have a multicore architecture and process
 * several seeds on different cores, such a configuration may make some cores starving because
 * the last seed to proceed uses only one core. We don't have a seed order that balances the
 * matches number for each seed.
 *
 * This class propose a solution to this core-starving issue: we order the seeds to be iterated
 * in such a way that the most greedy (biggest matches number) are processed first.
 *
 * The SeedHitIteratorCachedWithSortedSeeds class inherits from the SeedHitIteratorCached class
 * and merely reorder the seeds before launching the seed based iteration over the hits.
 */
class SeedHitIteratorCachedWithSortedSeeds : public SeedHitIteratorCached
{
public:

    /** \copydoc SeedHitIteratorCached::SeedHitIteratorCached */
    SeedHitIteratorCachedWithSortedSeeds (
        indexation::IDatabaseIndex* indexSubject,
        indexation::IDatabaseIndex* indexQuery,
        size_t                      neighbourhoodSize,
        float                       seedsUseRatio,
        bool&                       isRunning,
        ::seed::ISeedIterator*      seedIterator=0
    );

    /** \copydoc SeedHitIteratorCached::getName */
    const char* getName ()  { return "SeedHitIteratorCachedWithSortedSeeds"; }

protected:

    /** \copydoc SeedHitIteratorCached::sortSeeds */
    virtual void sortSeeds (void);

private:

    /** \copydoc SeedHitIteratorCached::clone */
    SeedHitIterator* clone (::seed::ISeedIterator* seedIterator)
    {
        return new SeedHitIteratorCachedWithSortedSeeds (_indexDb1, _indexDb2, _neighbourhoodSize, _seedsUseRatio, _isRunning, seedIterator);
    }
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _SEED_HITITERATOR_CACHED_HPP_ */
