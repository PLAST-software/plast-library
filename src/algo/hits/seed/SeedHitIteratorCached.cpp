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

#include <os/impl/DefaultOsFactory.hpp>

#include <algo/hits/seed/SeedHitIteratorCached.hpp>
#include <algo/core/api/IAlgoEvents.hpp>

using namespace std;
using namespace misc;
using namespace os;
using namespace os::impl;
using namespace dp;
using namespace database;
using namespace seed;
using namespace indexation;

#include <stdio.h>
#define DEBUG(a)    //printf a
#define VERBOSE(a)  //printf a

/********************************************************************************/
namespace algo   {
namespace hits   {
namespace seed   {
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
    bool&           isRunning,
    ISeedIterator*  seedIterator
)
    : SeedHitIterator (indexDb, indexQuery, neighbourhoodSize, isRunning, seedIterator)
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
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void SeedHitIteratorCached::iterate (void* aClient, Method method)
{
    IteratorClient* client = (IteratorClient*) aClient;   // not very nice...

    /** Some checks. */
    if (!client ||  !method)  { return; }

    size_t nbSeeds = 0;

    /** We reset the number of iterations. */
    _outputHitsNumber = 0;

    DEBUG (("SeedHitIteratorCached::iterate (%p): BEGIN SEEDS ITERATION  (seedIterator=%p) \n", this, _seedIterator));

    u_int64_t nbTotal     = _seedIterator->getNbTotal();
    u_int64_t nbRetrieved = 0;

    /** We define a threshold that determines how many seeds occurrences will be handled during an iteration
     *  of the IOccurrenceBlockIterator (see below).
     *
     *  Note that this threshold has a direct influence on the memory usage since IOccurrenceBlockIterator makes
     *  a copy of all needed neighbourhoods for the N seeds occurrences to be processed (where the copied data size
     *  is typically 4+2*22 = 48 bytes). We must also count 112 bytes for each ISeedOccurrence created instance.
     *  As a result, we need about 48+112 = 160 bytes for each seed occurrence.
     *
     *  We have also to multiply by the number of CPUs running since each CPU deals with one seed.
     *
     *  We have also to multiply by 2 because we instanciate twice IOccurrenceBlockIterator.
     *
     *  Thus, we may choose our threshold for having a controlled amount of used memory. For instance, if we use
     *  8 CPUs and deal with a seed having about 20000 occurrences, we will need something like:
     *      20000 * 160 * 8 * 2 = 51.200.000 bytes
     *
     *  We have also to keep in mind that these neighbourhoods may be extended by further steps
     *  (like small gap extension step).
     */
    size_t maxNbSeedsOccurPerIteration = 20000;

    /** We notify potential clients that we start the iteration. */
    this->notify (new IterationStatusEvent (ITER_STARTING, nbRetrieved, nbTotal, "starting iterating seeds..."));

    /** We loop over the possible seeds. Note that we use the 'retrieve' method that is protected from
     *  concurrent access by different threads. */
    for (ISeed seed; _isRunning && _seedIterator->retrieve (seed, nbRetrieved);  nbSeeds++)
    {
        _hit.setSeedHashCode (seed.code);

        /** we retrieve the number of occurrences for the current seed. */
        size_t nbOccur1 = _indexDb1->getOccurrenceNumber (&seed);
        size_t nbOccur2 = _indexDb2->getOccurrenceNumber (&seed);

        /** We notify potential clients that we have made some progress in the iteration. */
        this->notify (new IterationStatusEvent (
            ITER_ON_GOING,
            nbRetrieved,
            nbTotal,
            "iterating all possible seeds...  (%ld/%ld)",
            nbRetrieved, nbTotal
        ));

        VERBOSE (("ITERATE SEED '%s' [%ld,%ld]  nb1=%ld  nb2=%ld (%ld) \n",
            seed.kmer.toString().c_str(), nbRetrieved, nbTotal, nbOccur1, nbOccur2, nbOccur1*nbOccur2
        ));

        /** We test whether this seed has occurrences in both databases. */
        if ( ! (nbOccur1 > 0  &&  nbOccur2 > 0) )  { continue; }

        /** We increase the number of iterations. */
        HIT_STATS (_outputHitsNumber += nbOccur1 * nbOccur2;)

        /** We use a statements block for locally allocate our iterators. */
        {
            /** For the currenly iterated seed, We retrieve the occurrences that seed in the subject database.
             *  The information is retrieved as a an iterator that loops over ISeedOccurrence instance (holding
             *  sequence, offset of the seed in the sequence, ...)
             */
            IOccurrenceBlockIterator* itOccurBlockDb1 = _indexDb1->createOccurrenceBlockIterator (
                &seed,
                _neighbourhoodSize,
                MIN (nbOccur1, maxNbSeedsOccurPerIteration)
            );
            if (itOccurBlockDb1 == 0)    { continue; }
            LOCAL (itOccurBlockDb1);

            /** For the currenly iterated seed, We retrieve the occurrences that seed in the query database.
             *  The information is retrieved as a an iterator that loops over ISeedOccurrence instance (holding
             *  sequence, offset of the seed in the sequence, ...)
             */
            IOccurrenceBlockIterator* itOccurBlockDb2 = _indexDb2->createOccurrenceBlockIterator (
                &seed,
                _neighbourhoodSize,
                MIN (nbOccur2, maxNbSeedsOccurPerIteration)
            );
            if (itOccurBlockDb2 == 0)    { continue; }
            LOCAL (itOccurBlockDb2);

            /** We loop over ISeedOccurrence instances (both from subject and query db). */
            for (itOccurBlockDb1->first(); _isRunning &&  !itOccurBlockDb1->isDone(); itOccurBlockDb1->next())
            {
                Vector<const ISeedOccurrence*>& table1 = itOccurBlockDb1->currentItem ();

                for (itOccurBlockDb2->first(); _isRunning && !itOccurBlockDb2->isDone(); itOccurBlockDb2->next())
                {
                    Vector<const ISeedOccurrence*>& table2 = itOccurBlockDb2->currentItem ();

                    /** We have now two containers holding occurrences of the current seed in both
                     *  subject and query databases. We set them as reference to the Hit instance. */
                    _hit.setOccurrencesRef (table1, table2);

                    /** We also get a reference on the two buffers holding the neighbourhoods. */
                    _hit.setNeighbourhoods (itOccurBlockDb1->getNeighbourhoods(), itOccurBlockDb2->getNeighbourhoods());

                    /** We call the callback of a potential client; this is likely another IHitIterator,
                     *  used for filtering out all possible hits (ie. table1 x table2) for the current seed. */
                    (client->*method) (&_hit);
                }
            }

            /** Here we are done with hits iteration for the current seed. */
        }

    } /* end of for (_seedIterator... */

    /** We notify potential clients that we finish the iteration. */
    this->notify (new IterationStatusEvent (ITER_DONE, nbRetrieved, nbTotal, "finishing iterating seeds..."));

    DEBUG (("SeedHitIteratorCached::iterate (%p): END SEEDS ITERATION (found %ld seeds, %ld hits)\n",
        this, nbSeeds, _outputHitsNumber
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
    bool&           isRunning,
    ISeedIterator*  seedIterator
)
    : SeedHitIteratorCached (indexDb, indexQuery, neighbourhoodSize, isRunning, seedIterator)
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
}}} /* end of namespaces. */
/********************************************************************************/
