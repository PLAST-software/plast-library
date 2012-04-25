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

/** \file SeedHitIterator.hpp
 *  \brief Implementation of IHitIterator interface from databases indexations
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _SEED_HITITERATOR_HPP_
#define _SEED_HITITERATOR_HPP_

/********************************************************************************/

#include <designpattern/impl/ProductIterator.hpp>

#include <index/api/IDatabaseIndex.hpp>
#include <index/impl/DatabaseIndex.hpp>

#include <algo/hits/common/AbstractHitIterator.hpp>

/********************************************************************************/
namespace algo   {
namespace hits   {
/** \brief Implementations for IHitIterator interface from databases indexations. */
namespace seed   {
/********************************************************************************/

/** \brief Hits iterator from two database indexes.
 *
 * This class implements the IHitIterator interface by using the indexations of the
 * subject and query databases.
 *
 * This is a seed based iterator since it uses a ISeedIterator instance for iterating
 * all the seeds of some seed model; for each iterated seed, it finds in the two
 * provided databases indexes where the seed occurs. It is then possible to build a
 * Hit instance and iterate all the matches of the current seed in the both databases.
 *
 * This implementation relies on two imbricated loops (subject index loop inside query
 * index loop), with the issue that the information of the inner loop is retrieved
 * several times. A better approach is to cache in memory the two loops content (see
 * SeedHitIteratorCached implementation) which can lead to a significant speed up.
 *
 * \see SeedHitIteratorCached
 * \see indexation::IDatabaseIndex
 */
class SeedHitIterator : public algo::hits::common::AbstractHitIterator
{
public:

    /** Constructor.
     * \param[in] indexSubject : indexation of the subject database
     * \param[in] indexQuery   : indexation of the query database
     * \param[in] neighbourhoodSize : size of the neighbourhood
     * \param[in] seedIterator : seed iterator to be used.
     */
    SeedHitIterator (
        indexation::IDatabaseIndex* indexSubject,
        indexation::IDatabaseIndex* indexQuery,
        size_t                      neighbourhoodSize,
        float                       seedsUseRatio,
        bool&                       isRunning,
        ::seed::ISeedIterator*      seedIterator=0
    );

    /** Destructor. */
    virtual ~SeedHitIterator ();

    /** \copydoc common::AbstractHitIterator::getName */
    const char* getName ()  { return "SeedHitIterator"; }

    /** \copydoc common::AbstractHitIterator::first */
    void first();

    /** \copydoc common::AbstractHitIterator::next */
    dp::IteratorStatus next();

    /** \copydoc common::AbstractHitIterator::currentItem */
    Hit* currentItem() {  return &_hit;  }

    /** \copydoc common::AbstractHitIterator::isDone */
    bool isDone()  { return _isGlobalDone; }

    /** \copydoc common::AbstractHitIterator::split */
    std::vector<IHitIterator*> split (size_t nbSplit);

    /** \copydoc common::AbstractHitIterator::iterate */
    void iterate (void* aClient, Method method);

    /** \copydoc common::AbstractHitIterator::getSourceIterator
     * This is the source iterator, so have no source itself.
     */
    IHitIterator* getSourceIterator ()  { return 0; }

protected:

    /** The 'current' item to be provided to the client through the 'currentItem' method. */
    algo::hits::Hit _hit;

    /** Indexes for the subject databases. */
    indexation::IDatabaseIndex* _indexDb1;

    /** Indexes for the query databases. */
    indexation::IDatabaseIndex* _indexDb2;

    /** A shortcut; points to the model of one of the two indexes. */
    ::seed::ISeedModel* _model;

    /** We may need to get neighbourhoods. */
    size_t _neighbourhoodSize;

    /** */
    float _seedsUseRatio;

    /** */
    bool& _isRunning;

    /** The iterator that loops over seeds.
     * \return the seeds iterator
     */
    ::seed::ISeedIterator* _seedIterator;

    /** Smart setter for the _seedIterator attribute.
     * \param[in] seedIterator : the seed iterator to be referenced.
     */
    void setSeedIterator (::seed::ISeedIterator* seedIterator)  { SP_SETATTR(seedIterator); }

    /** Get the list of used seeds (i.e. matched by both databases).
     * \param[in] seedsIdx
     * \param[in] hitsNbList
     * \return total number of hits for all seeds
     */
    u_int64_t getSeedsCode (std::vector<size_t>& seedsIdx, std::vector<size_t>& hitsNbList);

    /** */
    bool nextSeed (bool first);

    /** Keep track of iteration local completion (i.e. for a given seed code). */
    bool _isLocalDone;

    /** Keep track of iteration global completion. */
    bool _isGlobalDone;

    /** Clone the instance with a specific seeds iterator. */
    virtual SeedHitIterator* clone (::seed::ISeedIterator* seedIterator)
    {
        return new SeedHitIterator (_indexDb1, _indexDb2, _neighbourhoodSize, _seedsUseRatio, _isRunning, seedIterator);
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
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _SEED_HITITERATOR_HPP_ */
