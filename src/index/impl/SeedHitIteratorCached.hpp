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

#ifndef _SEED_HITITERATOR_CACHED_HPP_
#define _SEED_HITITERATOR_CACHED_HPP_

/********************************************************************************/

#include "ProductIterator.hpp"
#include "IDatabaseIndex.hpp"
#include "DatabaseIndex.hpp"
#include "SeedHitIterator.hpp"

/********************************************************************************/
namespace indexation  {
/********************************************************************************/

/** Iterates over hits for successive seeds. */
class SeedHitIteratorCached : public SeedHitIterator
{
public:

    SeedHitIteratorCached (
        IDatabaseIndex* indexDb,
        IDatabaseIndex*  indexQuery,
        size_t neighbourhoodSize,
        seed::ISeedIterator* seedIterator=0
    );

    virtual ~SeedHitIteratorCached ();

    /** */
    const char* getName ()  { return "SeedHitIterator"; }

    void first();

    dp::IteratorStatus next();

    /** Optimize the iteration. */
    void iterate (void* aClient, Iterator::Method method);

    /** Split process. */
    std::vector<IHitIterator*> split (size_t nbSplit);

protected:

    virtual void sortSeeds (void) { /* does nothing by default. */  }

private:

    /** */
    std::vector<const ISeedOccurrence*> _occurDb1;
    std::vector<const ISeedOccurrence*> _occurDb2;

    /** */
    size_t _currentIdx1;
    size_t _currentIdx2;

    bool nextSeed (bool first);

    /** Clone the instance with a specific seeds iterator. */
    SeedHitIterator* clone (seed::ISeedIterator* seedIterator)
    {
        return new SeedHitIteratorCached (_indexDb1, _indexDb2, _neighbourhoodSize, seedIterator);
    }
};

/********************************************************************************/

/** Iterates over hits for successive sorted seeds. */
class SeedHitIteratorCachedWithSortedSeeds : public SeedHitIteratorCached
{
public:

    SeedHitIteratorCachedWithSortedSeeds (
        IDatabaseIndex* indexDb,
        IDatabaseIndex*  indexQuery,
        size_t neighbourhoodSize,
        seed::ISeedIterator* seedIterator=0
    );

    /** */
    const char* getName ()  { return "SorteedSeedHitIterator"; }

protected:

    virtual void sortSeeds (void);

private:

    /** Clone the instance with a specific seeds iterator. */
    SeedHitIterator* clone (seed::ISeedIterator* seedIterator)
    {
        return new SeedHitIteratorCachedWithSortedSeeds (_indexDb1, _indexDb2, _neighbourhoodSize, seedIterator);
    }
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _SEED_HITITERATOR_CACHED_HPP_ */
