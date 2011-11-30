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

#include <algo/hits/seed/SeedHitIterator.hpp>

#include <algorithm>

using namespace std;
using namespace dp;
using namespace database;
using namespace seed;
using namespace indexation;

#include <stdio.h>
#define DEBUG(a)  //printf a

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
SeedHitIterator::SeedHitIterator (
    IDatabaseIndex* indexDb,
    IDatabaseIndex* indexQuery,
    size_t          neighbourhoodSize,
    ISeedIterator*  seedIterator
)
    : _indexDb1(indexDb), _indexDb2(indexQuery),  _model(0),
      _neighbourhoodSize (neighbourhoodSize),
      _seedIterator (0),
      _isLocalDone(true), _isGlobalDone(true),
      _nbOccurMaxDb(0), _nbOccurMaxQuery(0),
      _itOccurDb1 (0),  _itOccurDb2 (0)
{
    DEBUG (("SeedHitIterator::SeedHitIterator:  _seedIterator=%p\n", _seedIterator));

    /** We use the two provided indexes. */
    if (_indexDb1)  { _indexDb1->use ();  }
    if (_indexDb2)  { _indexDb2->use ();  }

    /** A little shortcut. */
    _model = _indexDb1->getModel();

    /** We may have to set default interval of seeds values. */
    if (seedIterator == 0)   { seedIterator = _model->createAllSeedsIterator();  }
    setSeedIterator (seedIterator);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
SeedHitIterator::~SeedHitIterator ()
{
    if (_indexDb1)    { _indexDb1->forget ();  }
    if (_indexDb2)    { _indexDb2->forget ();  }

    setSeedIterator (0);

    setItOccur (_itOccurDb1, 0);
    setItOccur (_itOccurDb2, 0);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void SeedHitIterator::setItOccur (IOccurrenceIterator*& dest, IOccurrenceIterator* source)
{
    if (dest != 0)  { dest->forget (); }
    dest = source;
    if (dest != 0)  { dest->use();  dest->first(); }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void SeedHitIterator::first ()
{
    /** We go to the first seed of the seed model. */
    _seedIterator->first();

    /** We retrieve the first key matched by the two indexes. */
    if (nextSeed (true) == true)
    {
        /** We create the two occurrences iterators. */
        setItOccur (_itOccurDb1, _indexDb1->createOccurrenceIterator (_seedIterator->currentItem()) );
        setItOccur (_itOccurDb2, _indexDb2->createOccurrenceIterator (_seedIterator->currentItem()) );

        _hit.occur1.data[0] = _itOccurDb1->currentItem();
        _hit.occur2.data[0] = _itOccurDb2->currentItem();

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
bool SeedHitIterator::nextSeed (bool first)
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
        /** Optimization (avoid two calls). */
        const ISeed* seed = _seedIterator->currentItem();

        /** We create the two occurrences iterators. */
        setItOccur (_itOccurDb1, _indexDb1->createOccurrenceIterator (seed));
        setItOccur (_itOccurDb2, _indexDb2->createOccurrenceIterator (seed));

        _hit.occur1.data[0] = _itOccurDb1->currentItem();
        _hit.occur2.data[0] = _itOccurDb2->currentItem();

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
dp::IteratorStatus SeedHitIterator::next()
{
    /** We go to the next item of the second iterator. */
    _itOccurDb2->next ();

    /** We check that it is not done. */
    if (_itOccurDb2->isDone())
    {
        /** We go to the next item of the first iterator. */
        _itOccurDb1->next();

        if (! _itOccurDb1->isDone() )
        {
            /** The first iterator is not done, we can reset the second to its beginning. */
            _itOccurDb2->first ();
        }
        else
        {
            /** The first iterator is also done, the global iterator is therefore done. */
            _isLocalDone = true;
        }
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
void SeedHitIterator::iterate (void* aClient, Method method)
{
    IteratorClient* client = (IteratorClient*) aClient;   // not very nice...
    if (client &&  method)
    {
        first ();

        while (! _isGlobalDone)
        {
            /** We increase the number of iterated items. */
            _outputHitsNumber ++;

            /** We call the callback. */
            (client->*method) (&_hit);

            /** We go to the next item of the second iterator. */
            dp::IteratorStatus status2 = _itOccurDb2->next();

            /** We check that it is not done. */
            if (status2==dp::ITER_DONE  || (status2 == dp::ITER_UNKNOWN && _itOccurDb2->isDone()) )
            {
                /** We go to the next item of the first iterator. */
                dp::IteratorStatus status1 = _itOccurDb1->next();

                if ( (status1 == dp::ITER_ON_GOING) || (status1 == dp::ITER_UNKNOWN && !_itOccurDb1->isDone()) )
                {
                    /** The first iterator is not done, we can reset the second to its beginning. */
                    _itOccurDb2->first ();
                }
                else
                {
                    /** The first iterator is also done, the global iterator is therefore done. */
                    _isLocalDone = true;
                }
            }

            /** We may have to check whether another seed has to be iterated. */
            if (_isLocalDone == true)   {  _isGlobalDone = ! nextSeed (false);  }
        }
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
std::vector<IHitIterator*> SeedHitIterator::split (size_t nbSplit)
{
    std::vector<IHitIterator*> result;

    if (nbSplit > 0)
    {
        std::vector<size_t> seedsIdx;
        std::vector<size_t> hitsNbList;

        /** We retrieve (as a vector) the possible seeds with each hits number for each seed. */
        u_int64_t nbHits = getSeedsCode (seedsIdx, hitsNbList);

        DEBUG (("SeedHitIterator::split (this=%p):  nbSplit=%ld  nbSeedsFound=%ld  nbHits=%ld  nbMaxSeedsModel=%ld\n",
            this, nbSplit, seedsList.size(), nbHits, _model->getSeedsMaxNumber()
        ));

        size_t firstIdx = 0;
        size_t lastIdx  = 0;

        u_int64_t foundHits      = 0;
        u_int64_t foundTotalHits = 0;

        for (size_t i=0; i<nbSplit && firstIdx<seedsIdx.size(); i++)
        {
            /** We compute the average number of hits we want for the current split.
             *  Note that we readjusts this average at each iteration, which allows to have some
             *  balanced hits number for all resulting split iterators. Note also that this may be
             *  not optimal in case of seeds have big hits number.
             */
            u_int64_t nbAverageHits = (nbHits - foundTotalHits) / (nbSplit - i);

            /** We look for the first and last indexes in the seedsList for which we have the average hits number. */
            foundHits = 0;
            for (lastIdx=firstIdx; lastIdx < seedsIdx.size(); lastIdx++)
            {
                /** We increase the number of found hits. */
                foundHits += hitsNbList[lastIdx];

                if (foundHits >= nbAverageHits)  { break; }
            }

            /** We increase the total number of hits found. */
            foundTotalHits += foundHits;

            /** Special case for last split. */
            if (i == nbSplit -1)  { lastIdx = seedsIdx.size() - 1; }

            /** We create a new seed iterator. */
            ISeedIterator* extractSeedIterator = _seedIterator->extract (seedsIdx[firstIdx], seedsIdx[lastIdx]);

            /** We clone the current instance with a specific seed iterator. */
            SeedHitIterator* cloneInstance = this->clone (extractSeedIterator);

            DEBUG (("SeedHitIterator::split [%ld]: => foundHits=%ld  nbAverageHits=%ld  foundTotalHits=%ld  first=%ld  last=%ld  (diff=%ld)  [%d,%d]  clone=%p  createSeedIterator=%p\n",
                i, foundHits, nbAverageHits, foundTotalHits,
                firstIdx, lastIdx, (lastIdx-firstIdx),
                seedsIdx[firstIdx],
                seedsIdx[lastIdx],
                cloneInstance,
                extractSeedIterator
            ));

            /** We add a new iterator into the result list. */
            result.push_back (cloneInstance);

            /** We can prepare for the next iteration. */
            firstIdx = lastIdx + 1;
        }

        /** We link the created split to the current instance. */
        this->setSplitIterators (result);
    }

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
static bool sortByHitsNb (const pair<size_t,size_t>& d1, const pair<size_t,size_t>& d2)
{
    return d1.second > d2.second;
}

/** */
u_int64_t SeedHitIterator::getSeedsCode (std::vector<size_t>& seedsIdx, std::vector<size_t>& hitsNbList)
{
    u_int64_t result = 0;
    size_t seedNumber = 0;

    /** We need to sort seeds list by decreasing hits number. We need therefore a pair structure. */
    vector < pair<size_t,size_t> > seedsList;

    /** We loop over the seed iterator. */
    for (_seedIterator->first(); ! _seedIterator->isDone(); _seedIterator->next(), seedNumber++)
    {
        const ISeed* seed = _seedIterator->currentItem();

        size_t nbOccurDb    = _indexDb1->getOccurrenceNumber (seed);
        size_t nbOccurQuery = _indexDb2->getOccurrenceNumber (seed);

        if (nbOccurDb > 0  && nbOccurQuery > 0)
        {
            /** We compute the hits number. */
            size_t hitsNb = nbOccurDb * nbOccurQuery;

            /** We create a new entry and add it to the list. */
            std::pair<size_t,size_t> p (seedNumber, hitsNb);

            /** We memorize the current useful seed.  */
            seedsList.push_back (p);

            /** We increase the total number of found hits. */
            result += hitsNb;

            /** We compute the max occurs nb for each index. */
            if (nbOccurDb    > _nbOccurMaxDb)     { _nbOccurMaxDb    = nbOccurDb;    }
            if (nbOccurQuery > _nbOccurMaxQuery)  { _nbOccurMaxQuery = nbOccurQuery; }
        }
    }

    /** We sort the resulting vector by decreasing hits number. */
    std::sort (seedsList.begin(), seedsList.end(), sortByHitsNb);

    /** We resize the two provided vectors with the number of 'useful' seeds. */
    size_t nbSeeds = seedsList.size();

    seedsIdx.resize (nbSeeds);
    hitsNbList.resize (nbSeeds);

    /** We fill the two provided arguments. */
    for (size_t n=0; n<nbSeeds; n++)
    {
        std::pair<size_t,size_t>& current = seedsList[n];

        seedsIdx[n]   = current.first;
        hitsNbList[n] = current.second;
    }

    /** We return the result. */
    return result;
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
