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

/** \file IHitIterator.hpp
 *  \brief Define the concept of hits iteration.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef IHITITERATOR_HPP_
#define IHITITERATOR_HPP_

/********************************************************************************/

#include <designpattern/api/Iterator.hpp>
#include <designpattern/api/SmartPointer.hpp>
#include <designpattern/api/IProperty.hpp>
#include <designpattern/impl/Observer.hpp>

#include <misc/api/Vector.hpp>

#include <index/api/IOccurrenceIterator.hpp>

#include <vector>
#include <list>

/********************************************************************************/
/** \brief PLAST algorithm concepts. */
namespace algo   {
/** \brief Concept for hits iteration. */
namespace hits   {
/********************************************************************************/

/* We define a macro for enabling/disabling statistics statements.
 *  It should be activated/deactivated through compilation flag.
 */
#if 1
    #define HIT_STATS(a)            a
    #define HIT_STATS_VERBOSE(a)    //a
#else
    #define HIT_STATS(a)
    #define HIT_STATS_VERBOSE(a)
#endif

#define HIT_BAD_SCORE  -100

typedef std::pair<size_t,size_t> IdxCouple;

/********************************************************************************/

/** \brief Define a match in two databases for a given seed
 *
 * The Hit struct is a core structure of the PLAST design.
 * For a given seed, it defines where in the subject and query databases the given
 * seed can be found.
 *
 * For instance, we may find 100 times the "PQFT" seed in the subject database and
 * 50 in the query database. In this example, a Hit instance will potentially
 * defines 100*50=5000 matches of the seed in the two databases.
 *
 * Actually, the Hit struct holds:
 *      - the seed occurrences vector for the subject database ('occur1' attribute)
 *      - the seed occurrences vector for the query database   ('occur2' attribute)
 *
 * The idea of the PLAST design is the following:
 *      - iterate each seed of a seed model
 *      - for each seed, find all the occurrences in the subject and in the query;
 *        in other words, create a Hit instance
 *      - at each step of the algorithm, we iterate all possible matches for the
 *        Hit instance and then potentially filter out some matches.
 *
 * Since we want to filter out some unwanted matches throughout the algorithm, we have
 * to remember which matches to be skipped for further steps. This is achieved with
 * the 'indexes' attribute of the Hit struct; it keeps each
 * [index in vector 'occur1', index in vector 'occur2'] couple to be kept at each step
 * of the algorithm. For instance, it is likely that entries of the 'indexes' attribute
 * will be removed between the beginning of the small gap algorithm and its end.
 *
 * Hit instances should be through iterators, in particular through the IHitIterator
 * interface.
 *
 * For avoiding unwanted copy, the two vectors 'occur1' and 'occur2' are managed as
 * references to other data sources (information coming from databases indexations,
 * see IOccurrenceBlockIterator interface), which is done by calling setOccurrencesRef().
 *
 * \see IHitIterator
 * \see IOccurrenceBlockIterator
 */
struct Hit
{
    /** Constructor. */
    Hit ()  : _code(0), occur1(1), neighbourhoodsOccur1(0), occur2(1), neighbourhoodsOccur2(0),
		  _table(0), _tableIdx(0), _iterIdx(0), _usedMask(0)
    {
        _table    = new IdxCouple[32];
        _tableIdx = 0;
        _usedMask = 0;
    }

    /** Destructor. */
    ~Hit ()
    {
        resetIndexes();

        delete[] _table;
    }

    /** */
    void setSeedHashCode (seed::SeedHashCode code)  { _code = code; }
    seed::SeedHashCode getSeedHashCode () { return _code; }
    seed::SeedHashCode _code;

    /** Set occurrences references. Note that the referenced arguments are supposed
     * to live longer to the Hit instance.
     * \param[in] ref1 : vector of seed occurrences in the subject database
     * \param[in] ref2 : vector of seed occurrences in the query database
     */
    void setOccurrencesRef (
        const misc::Vector<const indexation::ISeedOccurrence*>& ref1,
        const misc::Vector<const indexation::ISeedOccurrence*>& ref2
    )
    {
        size_t nb1 = ref1.size;
        size_t nb2 = ref2.size;

        if (nb1 > 0 && nb2 > 0)
        {
            /** We reset the indexes. */
            resetIndexes();

            /** We set the references. */
            occur1.setReference (nb1, ref1.data);
            occur2.setReference (nb2, ref2.data);
        }
    }

    /** Keep a reference on the two buffer holding the neighbourhoods of all hits.
     * \param[in] neighbourhoods1 : reference to be memorized
     * \param[in] neighbourhoods2 : reference to be memorized
     */
    void setNeighbourhoods (const database::LETTER* neighbourhoods1, const database::LETTER* neighbourhoods2)
    {
        neighbourhoodsOccur1 = neighbourhoods1;
        neighbourhoodsOccur2 = neighbourhoods2;
    }

    /** Vector of occurrences of the defining seed in the subject database. */
    misc::Vector<const indexation::ISeedOccurrence*> occur1;

    /** Buffer holding the neighbourhoods of all occurrences in the subject database. */
    const database::LETTER* neighbourhoodsOccur1;

    /** Vector of occurrences of the defining seed in the query database. */
    misc::Vector<const indexation::ISeedOccurrence*> occur2;

    /** Buffer holding the neighbourhoods of all occurrences in the subject database. */
    const database::LETTER* neighbourhoodsOccur2;

    /** Add a couple of vector indexes in the list of valid matches. */
    size_t addIndexes   (size_t i, size_t j)
    {
        IdxCouple& current = _table[_tableIdx];
        current.first = i;  current.second = j;
        _usedMask |= (1 << _tableIdx);
        return ++_tableIdx;
    }

    /** Clear the list of valid matches. */
    size_t resetIndexes (void)
    {
        _tableIdx = 0;
        _usedMask = 0;
        return _tableIdx;
    }

    /** Returns the number of indexes. */
    u_int32_t size (void)
    {
        /** See http://graphics.stanford.edu/~seander/bithacks.html */
        u_int32_t v = _usedMask;
        v = v - ((v >> 1) & 0x55555555);
        v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
        return  ( ((v + (v >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24;
    }

    /** */
    void first ()  {  _iterIdx = -1;  bool removable=false;  next (removable);  }

    /** */
    bool isDone ()  { return _iterIdx >= _tableIdx; }

    /** */
    void next (bool& removeCurrent)
    {
        if (removeCurrent)  { _usedMask &= ~(1<<_iterIdx);  removeCurrent=false; }

        do  {  _iterIdx++;  }   while (_iterIdx < _tableIdx &&  ( !(_usedMask & (1<<_iterIdx)) ) );
    }

    /** */
    IdxCouple& currentItem() { return _table[_iterIdx]; }

    /** */
    bool empty()  { return _tableIdx == 0; }

private:
    IdxCouple*  _table;
    int16_t     _tableIdx;
    int16_t     _iterIdx;
    u_int32_t   _usedMask;
};

/********************************************************************************/

/** \brief Iteration over all seed matches in both subject and query databases.
 *
 * The PLAST design core concept is the IHitIterator interface. A IHitIterator
 * iterates over hits, ie. a location in a subject database and a location in a
 * query database where some similarity has been found.
 *
 * Such an iterator can be split in small parts and this is exactly what is used
 * for the thread-parallelization of the whole PLAST algorithm. From a global hit
 * iterator that provides hits for each possible seed of a given seed model, one
 * can split this global iterator in N iterators, each of them iterating a sub set
 * of the whole possible seeds. In other words, iterating all the small iterators
 * will iterate the same hits set than iterating the global hits iterator.
 * The difference is that it is possible to iterate each small iterator in a specific
 * thread; if the algorithm is executed on a multicore architecture, several small
 * iterators will be iterated in a parallel way, with a great improvement in time
 * execution.
 *
 * Note that the global iterator should behave like a "composite" iterator in the
 * Design Pattern sense; if we ask some information about the global iterator,
 * the information will be aggregated from the information of the split iterators.
 *
 * Another key point concerns the fact that several hits iterators can be linked.
 * For instance, the small gap hits iterator can rely on the hits previously found
 * by the ungap hits iterator. In this example, the small gap hits iterator will iterate
 * a filtered sub set of the ungap hits iterator. Another way to tell it is that
 * IHitIterator instances can be linked like unix pipes.
 *
 * Note that an initial hits iterator is needed and that doesn't rely on any initial
 * source hits iterator. A good candidate for such an iterator is the one provided
 * by the IIndexator interface (for a given seed, hits are generated by looking for
 * the seed occurrences in the subject and query databases).
 *
 * From the global PLAST algorithm point of view, a single IHitIterator instance is needed;
 * this instance is built as the following pipe:
 *      - seed hits iterator -> ungap hits iterator -> small gap hits iterator -> full gap hits iterator ...
 *
 * Each sub iterator (seed, ungap,...) corresponds to a part of the global algorithm,
 * designed as a filter of the previous part. So iterating this special "pipe" instance
 * is like performing the PLAST algorithm in its globality. Note that this special "pipe"
 * instance is split in small parts which can be executed as said above in a parallel fashion
 * on a multicore architecture.
 *
 * The actual iteration is done through a call to the iterate() method; actually, the pipe-like
 * iteration is achieved through a specific implementation of the iterate() method (see
 * AbstractScoredHitIterator implementation).
 *
 * The IHitIterator interface is both a subject (for sending information about its execution)
 * and an observer (forward of notification of sub components)
 */
class IHitIterator : public dp::Iterator<Hit*>, public dp::impl::Subject, public dp::IObserver
{
public:

    /** Split the current iterator in several iterators. All the returned
     *  iterators should represent the same set of Hit instances than the source iterator.
     *  \param[in] nbSplit : number of split iterators; likely to be the number of cores in a multicore architecture
     *  \return a vector of IHitIterator instances
     */
    virtual std::vector<IHitIterator*> split (size_t nbSplit) = 0;

    /** Returns the source hit iterator that provides initial set of hits to the current iterator.
     *  \return the source iterator if any, NULL if the current iterator has no source.
     */
    virtual IHitIterator* getSourceIterator () = 0;

    /** Returns the number of incoming hits from a source iterator (information purpose).
     *  \return the number of incoming hits
     */
    virtual u_int64_t getInputHitsNumber  () = 0;

    /** Returns the number of outcoming hits (information purpose).
     *  \return the number of outcoming hits
     */
    virtual u_int64_t getOutputHitsNumber () = 0;

    /** Returns a name (class name for instance) for reporting information.
     *  \return the name of the iterator
     */
    virtual const char* getName () = 0;

    /** Returns information (as tagged value) about the iterator.
     *  It may be for instance for statistic purposes.
     *  \return a new IProperties instance.
     */
    virtual dp::IProperties* getProperties () = 0;
};

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/

#endif /* IHITITERATOR_HPP_ */
