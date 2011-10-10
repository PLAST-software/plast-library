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


#ifndef _SEED_HITITERATOR_HPP_
#define _SEED_HITITERATOR_HPP_

/********************************************************************************/

#include "ProductIterator.hpp"
#include "IDatabaseIndex.hpp"
#include "DatabaseIndex.hpp"
#include "AbstractHitIterator.hpp"

/********************************************************************************/
namespace indexation  {
/********************************************************************************/

/** Iterates over hits.
 */
class SeedHitIterator : public AbstractHitIterator
{
public:

    SeedHitIterator (
        IDatabaseIndex*      indexDb,
        IDatabaseIndex*      indexQuery,
        size_t               neighbourhoodSize,
        seed::ISeedIterator* seedIterator=0
    );

    virtual ~SeedHitIterator ();

    /** */
    const char* getName ()  { return "SeedHitIterator"; }

    void first();

    dp::IteratorStatus next();

    bool isDone()  { return _isGlobalDone; }

    /** Split process. */
    std::vector<IHitIterator*> split (size_t nbSplit);

    /** Optimize the iteration. */
    void iterate (void* aClient, Method method);

protected:

    /** Indexes for the two databases. */
    IDatabaseIndex* _indexDb1;
    IDatabaseIndex* _indexDb2;

    /** A shortcut; points to the model of one of the two indexes. */
    seed::ISeedModel* _model;

    /** We may need to get neighbourhoods. */
    size_t _neighbourhoodSize;

    /** The iterator that loops over seeds. */
    seed::ISeedIterator* _seedIterator;
    void setSeedIterator (seed::ISeedIterator* seedIterator)
    {
        if (_seedIterator != 0)  { _seedIterator->forget (); }
        _seedIterator = seedIterator;
        if (_seedIterator != 0)  { _seedIterator->use ();    }
    }

    /** Get the list of used seeds (i.e. matched by both databases). */
    u_int64_t getSeedsCode (std::vector<size_t>& seedsIdx, std::vector<size_t>& hitsNbList);

    bool nextSeed (bool first);

    /** Keep track of iteration local completion (i.e. for a given seed code). */
    bool _isLocalDone;

    /** Keep track of iteration global completion. */
    bool _isGlobalDone;

    /** Clone the instance with a specific seeds iterator. */
    virtual SeedHitIterator* clone (seed::ISeedIterator* seedIterator)
    {
        return new SeedHitIterator (_indexDb1, _indexDb2, _neighbourhoodSize, seedIterator);
    }

    size_t _nbOccurMaxDb;
    size_t _nbOccurMaxQuery;

private:

    /** We need two occurrences iterator, one for the database and one for the query. */
    indexation::IOccurrenceIterator* _itOccurDb1;
    indexation::IOccurrenceIterator* _itOccurDb2;

    void setItOccur (indexation::IOccurrenceIterator*& dest, indexation::IOccurrenceIterator* source);
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _SEED_HITITERATOR_HPP_ */
