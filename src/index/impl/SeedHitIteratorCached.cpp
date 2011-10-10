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

#include "SeedHitIteratorCached.hpp"
#include "MemoryAllocator.hpp"

using namespace std;
using namespace os;
using namespace dp;
using namespace database;
using namespace seed;

#include <stdio.h>
#define DEBUG(a)  //printf a

/********************************************************************************/
namespace indexation  {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
SeedHitIteratorCached::SeedHitIteratorCached (
    IDatabaseIndex* indexDb,
    IDatabaseIndex* indexQuery,
    size_t          neighbourhoodSize,
    ISeedIterator*  seedIterator
)
    : SeedHitIterator (indexDb, indexQuery, neighbourhoodSize, seedIterator),
      _currentIdx1(0), _currentIdx2(0)
{
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
SeedHitIteratorCached::~SeedHitIteratorCached ()
{
    /** We have to release the cached ISeedOccurrence instances. */
    for (size_t i=0; i<_occurDb1.size(); i++)  {  delete _occurDb1[i];  }
    for (size_t i=0; i<_occurDb2.size(); i++)  {  delete _occurDb2[i];  }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void SeedHitIteratorCached::first ()
{
    /** We go to the first seed of the seed model. */
    _seedIterator->first();

    /** We retrieve the first key matched by the two indexes. */
    if (nextSeed (true) == true)
    {
        _isLocalDone  = false;
        _isGlobalDone = false;
    }
    else
    {
        _isLocalDone  = true;
        _isGlobalDone = true;
    }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool SeedHitIteratorCached::nextSeed (bool first)
{
    bool found = false;

    if (!first)  {  _seedIterator->next(); }

    /** We look for the next seed that has occurrences in both databases. */
    for ( ; !found  &&  !_seedIterator->isDone(); _seedIterator->next())
    {
        found = _indexDb1->getOccurrenceNumber(_seedIterator->currentItem()) > 0  &&
                _indexDb2->getOccurrenceNumber(_seedIterator->currentItem()) > 0;

        if (found)  { break; }
    }

    /** We may have to configure the occurrences iterators. */
    if (found == true)
    {
        /** Little shortcut. */
        const ISeed* seed = _seedIterator->currentItem();

        /** We have to release the cached ISeedOccurrence instances. */
        for (size_t i=0; i<_occurDb1.size(); i++)  {  delete _occurDb1[i];  }
        for (size_t i=0; i<_occurDb2.size(); i++)  {  delete _occurDb2[i];  }

        _occurDb1.clear();
        _occurDb2.clear();

        /** We build (for each database) an array of ISeedOccurrence instances. */
        IOccurrenceIterator* itOccurDb1 = _indexDb1->createOccurrenceIterator (seed);
        LOCAL (itOccurDb1);
        for (itOccurDb1->first(); !itOccurDb1->isDone(); itOccurDb1->next())
        {
            _occurDb1.push_back (itOccurDb1->currentItem()->clone());
        }

        IOccurrenceIterator* itOccurDb2 = _indexDb2->createOccurrenceIterator (seed);
        LOCAL (itOccurDb2);
        for (itOccurDb2->first(); !itOccurDb2->isDone(); itOccurDb2->next())
        {
            _occurDb2.push_back (itOccurDb2->currentItem()->clone());
        }

        /** We reset the current indexes on these two arrays. */
        _currentIdx1 = _currentIdx2 = 0;

        _hit.occur1.data[0] = (_occurDb1[_currentIdx1]);
        _hit.occur2.data[0] = (_occurDb2[_currentIdx2]);

        _isLocalDone = false;
    }
    else
    {
        /** We update the global isDone attribute. */
        _isGlobalDone = true;
    }

    /** We return true if we found a correct seed. */
    return found;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
dp::IteratorStatus SeedHitIteratorCached::next()
{
    _currentIdx2++;

    /** We check that it is not done. */
    if (_currentIdx2 >= _occurDb2.size())
    {
        /** We go to the next item of the first iterator. */
        _currentIdx1++;

        if (_currentIdx1 < _occurDb1.size() )
        {
            /** The first iterator is not done, we can reset the second to its beginning. */
            _currentIdx2 = 0;

            _hit.occur1.data[0] = (_occurDb1[_currentIdx1]);
        }
        else
        {
            /** The first iterator is also done, the global iterator is therefore done. */
            _isLocalDone = true;
        }
    }
    else
    {
        _hit.occur2.data[0] = (_occurDb2[_currentIdx2]);
    }

    /** We may have to check whether another seed has to be iterated. */
    if (_isLocalDone == true)
    {
        _isGlobalDone = ! nextSeed (false);
    }

    return dp::ITER_UNKNOWN;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void SeedHitIteratorCached::iterate (void* aClient, Iterator::Method method)
{
    IteratorClient* client = (IteratorClient*) aClient;   // not very nice...

    /** Some checks. */
    if (!client ||  !method)  { return; }

    size_t nbSeeds       = 0;
    size_t nbActualSeeds = 0;

    /** We reset the number of iterations. */
    _outputHitsNumber = 0;

    DEBUG (("SeedHitIteratorCached::iterate (%p): BEGIN SEEDS ITERATION  (seedIterator=%p) \n", this, _seedIterator));

    u_int64_t nbTotal     = _seedIterator->getNbTotal();
    u_int64_t nbRetrieved = 0;

    size_t splitSize = 1;

    /** We loop over the possible seeds. */
    for (ISeed seed; _seedIterator->retrieve (seed, nbRetrieved);  nbSeeds++)
    {
        /** We notify potential clients that we start the iteration. */
        if (nbRetrieved==1)
        {
            this->notify (new IterationStatusEvent (ITER_STARTING, nbRetrieved, nbTotal, "starting iterating seeds..."));
        }

        /** We notify potential clients that we have made some progress in the iteration. */
        this->notify (new IterationStatusEvent (
            ITER_ON_GOING,
            nbRetrieved,
            nbTotal,
            "iterating all possible seeds...  (%ld/%ld)",
            nbRetrieved, nbTotal
        ));

        /** we retrieve the number of occurrences for the current seed. */
        size_t nbOccur1 = _indexDb1->getOccurrenceNumber (&seed);
        size_t nbOccur2 = _indexDb2->getOccurrenceNumber (&seed);

        DEBUG (("ITERATE SEED '%s' [%ld,%ld]  nb1=%ld  nb2=%ld (%ld) \n",
            seed.kmer.toString().c_str(), nbRetrieved, nbTotal, nbOccur1, nbOccur2, nbOccur1*nbOccur2
        ));

        /** We test whether this seed has occurrences in both databases. */
        if ( ! (nbOccur1 > 0  &&  nbOccur2 > 0) )  { continue; }

        /** We increase the number of iterations. */
        _outputHitsNumber += nbOccur1 * nbOccur2;

        /** We use a statements block for locally allocate our iterators. */
        {
            IOccurrenceBlockIterator* itOccurBlockDb1 = _indexDb1->createOccurrenceBlockIterator (
                &seed,
                _neighbourhoodSize,
                nbOccur1 / splitSize
            );
            if (itOccurBlockDb1 == 0)    { continue; }
            LOCAL (itOccurBlockDb1);

            IOccurrenceBlockIterator* itOccurBlockDb2 = _indexDb2->createOccurrenceBlockIterator (
                &seed,
                _neighbourhoodSize,
                nbOccur2 / splitSize
            );
            if (itOccurBlockDb2 == 0)    { continue; }
            LOCAL (itOccurBlockDb2);

            /** We loop over ISeedOccurrence instances. */
            for (itOccurBlockDb1->first(); !itOccurBlockDb1->isDone(); itOccurBlockDb1->next())
            {
                Vector<const ISeedOccurrence*>& table1 = itOccurBlockDb1->currentItem ();

                for (itOccurBlockDb2->first(); !itOccurBlockDb2->isDone(); itOccurBlockDb2->next())
                {
                    Vector<const ISeedOccurrence*>& table2 = itOccurBlockDb2->currentItem ();

                    _hit.setOccurrencesRef (table1, table2);

                    /** We call the callback. */
                    (client->*method) (&_hit);
                }
            }
        }

        /** We notify potential clients that we finish the iteration. */
        if (nbRetrieved==nbTotal)
        {
            this->notify (new IterationStatusEvent (ITER_DONE, nbRetrieved, nbTotal, "finishing iterating seeds..."));
        }

    } /* end of for (_seedIterator... */

    DEBUG (("SeedHitIteratorCached::iterate (%p): END SEEDS ITERATION (found %ld seeds and %ld actual seeds, %ld hits)\n",
        this, nbSeeds, nbActualSeeds, _outputHitsNumber
    ));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
std::vector<IHitIterator*> SeedHitIteratorCached::split (size_t nbSplit)
{
    std::vector<IHitIterator*> result;

    /** We may have to sort the seeds according to some criteria. */
    this->sortSeeds ();

    /** We have to reinit the seeds iterator. */
    _seedIterator->first ();

    /** We split the current iterator. */
    for (size_t i=0; i<nbSplit; i++)
    {
        /** We clone the instance. */
        SeedHitIterator* clone = this->clone (_seedIterator);

        /** We add a new iterator into the result list. */
        result.push_back (clone);
    }

    /** We link the created split to the current instance. */
    this->setSplitIterators (result);

    /** We return the result. */
    return result;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
SeedHitIteratorCachedWithSortedSeeds::SeedHitIteratorCachedWithSortedSeeds (
    IDatabaseIndex* indexDb,
    IDatabaseIndex* indexQuery,
    size_t          neighbourhoodSize,
    ISeedIterator*  seedIterator
)
    : SeedHitIteratorCached (indexDb, indexQuery, neighbourhoodSize, seedIterator)
{
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void SeedHitIteratorCachedWithSortedSeeds::sortSeeds ()
{
    /** We get information about seeds for the current indexes.
     *  In particular, we want to know 'useful' seeds, i.e. seeds that have
     *  occurrences in both subject and query indexes.
     *  We want also to have a useful seeds list sorted by decreasing hits number.
     */
    vector<size_t> seedsIdx;
    vector<size_t> hitsNbList;
    getSeedsCode (seedsIdx, hitsNbList);

    /** We create a new seed iterator that filters the current one with 'useful' seeds. */
    ISeedIterator* filteredSeedIt = _seedIterator->createFilteredIterator (seedsIdx);

    /** We set the filtered iterator as the new one used for hits iteration. */
    setSeedIterator (filteredSeedIt);
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
