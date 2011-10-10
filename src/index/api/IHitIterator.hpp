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


#ifndef IHITITERATOR_HPP_
#define IHITITERATOR_HPP_

/********************************************************************************/

#include "Iterator.hpp"
#include "SmartPointer.hpp"
#include "Observer.hpp"
#include "IOccurrenceIterator.hpp"
#include "MemoryAllocator.hpp"
#include "IProperty.hpp"

#include <vector>
#include <list>

/********************************************************************************/
namespace indexation  {
/********************************************************************************/

/** We define a macro for enabling/disabling statistics statements.
 *  It should be activated/deactivated through compilation flag.
 */
#if 1
    #define HIT_STATS(a)  a
#else
    #define HIT_STATS(a)
#endif

#define HIT_BAD_SCORE  -100

typedef std::pair<size_t,size_t> IdxCouple;

/** We define a hit: two sets of seed occurrences that have at least one word in common
 *  in two sequences.
 */
struct Hit
{
    Hit ()
    {
    }

    Hit (size_t nbOccurs1, size_t nbOccurs2)
    {
        resize (nbOccurs1, nbOccurs2);
    }

    ~Hit ()
    {
        resetIndexes();
    }

    /** Set occurrences 1 and 2 a first vector position. */
    void setOccurrencesRef (
        const os::Vector<const ISeedOccurrence*>& ref1,
        const os::Vector<const ISeedOccurrence*>& ref2
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

    /** Resize a Hit instance. */
    void resize (size_t nb1, size_t nb2)
    {
        if (nb1 > 0  &&  nb2 > 0)
        {
            /** We reset the indexes. */
            resetIndexes();

            /** We resize each vector of ISeedOccurrence. */
            occur1.resize (nb1);
            occur2.resize (nb2);
        }
    }

    /** */
    void addIndexes   (size_t i, size_t j)  {   indexes.push_back (std::pair<size_t,size_t> (i,j));  }

    void resetIndexes (void)
    {
        indexes.clear ();
    }

    os::Vector<const ISeedOccurrence*> occur1;
    os::Vector<const ISeedOccurrence*> occur2;

    std::list<IdxCouple> indexes;
};

/********************************************************************************/

/** Define what a property for a hit can be. This could be generalized to other components. */
struct HitProperty
{
    HitProperty (size_t aDepth, const std::string& aKey, int64_t aValue)
        : depth(aDepth), key(aKey), value(aValue)  {}

    size_t      depth;
    std::string key;
    int64_t     value;
};

/********************************************************************************/

/** Iterates over hits.
 */
class IHitIterator : public dp::Iterator<Hit*>, public dp::Subject, public dp::IObserver
{
public:

    /** Implementation of the Iterator interface. */
    virtual void first()   = 0;             // go to the first item to iterate
    virtual dp::IteratorStatus next() = 0;  // go to the next item and returns the isDone status
    virtual bool isDone()  = 0;             // true if iteration is done, false otherwise
    virtual Hit* currentItem() = 0;         // returns the current item of the iteration

    /** Split the current iterator in several iterators. All the returned
     *  iterators should represent the same set of Hit instances than the source iterator. */
    virtual std::vector<IHitIterator*> split (size_t nbSplit) = 0;

    /** Returns the number of iterated items (debug purpose). */
    virtual u_int64_t getInputHitsNumber  () = 0;
    virtual u_int64_t getOutputHitsNumber () = 0;

    /** Returns a name (class name for instance) for reporting information. */
    virtual const char* getName () = 0;

    /** Returns information (as tagged value) about the iterator.
     *  It may be for instance for statistic purposes. */
    virtual dp::IProperties* getProperties () = 0;

    /** Returns the source hit iterator that provides hits to the current iterator.
     *  Returns NULL if the iterator has no source. */
    virtual IHitIterator* getSourceIterator () = 0;
};

/********************************************************************************/

/** Factory that creates IHitIterator instances.
 */
class IHitIteratorFactory : public dp::SmartPointer
{
public:

    /** Creates a new IHitIterator instance. Note that a IHitIterator can be provided as argument;
     *  In such a case, the newly created iterator acts like a filter on the provided iterator (which
     *  acts as a Hit provider to the newly created iterator).
     */
    virtual IHitIterator* createHitIterator (IHitIterator* previous) = 0;

    /** Get/Set next factory. */
    virtual IHitIteratorFactory* getNextFactory () = 0;
    virtual void setNextFactory (IHitIteratorFactory* nextFactory) = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* IHITITERATOR_HPP_ */
