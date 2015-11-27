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

/** \file AbstractSeedIterator.hpp
 *  \brief Abstract implementation of ISeedIterator interface
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ABSTRACT_SEED_ITERATOR_HPP_
#define _ABSTRACT_SEED_ITERATOR_HPP_

/********************************************************************************/

#include <seed/api/ISeedIterator.hpp>
#include <seed/api/ISeedModel.hpp>

#include <os/api/IThread.hpp>

/********************************************************************************/
namespace seed {
/** \brief Implementation of seed based concepts. */
namespace impl {
/********************************************************************************/

/** \brief Null implementation of the ISeedIterator interface
 *
 *  Define a null seed iterator, i.e. that iterates an empty set.
 */
class NullSeedIterator : public ISeedIterator
{
public:

    /** Destructor. */
    virtual ~NullSeedIterator () {}

    /** \copydoc ISeedIterator::first */
    void first()  {}

    /** \copydoc ISeedIterator::next */
    dp::IteratorStatus next()   { return dp::ITER_DONE; }

    /** \copydoc ISeedIterator::isDone */
    bool isDone() { return true; }

    /** \copydoc ISeedIterator::currentItem */
    const ISeed* currentItem()  { return 0; }

    /** \copydoc ISeedIterator::getNbTotal
     * Implementations that don't know this number should return 0 by convention.
     */
    u_int64_t getNbTotal ()  { return 0; }

    /** \copydoc ISeedIterator::setData */
    void setData (const database::IWord& data) {}

    /** \copydoc ISeedIterator::extract */
    ISeedIterator* extract (size_t firstIdx, size_t lastIdx) { return 0; }

    /** \copydoc ISeedIterator::createFilteredIterator */
    ISeedIterator* createFilteredIterator (const std::vector<size_t>& seedsIdx)  { return 0; }

    /** \copydoc ISeedIterator::retrieve */
    bool retrieve (ISeed& item, u_int64_t& nbRetrieved)  { return false; }
};

/********************************************************************************/

/** \brief Define seed iterator with a seed model
 *
 *  This class factorizes some generic methods implementations; it remains abstract and has
 *  to be subclassed.
 */
class AbstractSeedIterator : public ISeedIterator
{
public:

    /** Constructor.
     * \param[in] model : the seed model the iterator is linked to
     * \param[in] firstIdx : first index to be iterated
     * \param[in] lastIdx  : last index to be iterated
     * \param[in] hasNextValidMethod : tells whether the 'findNextValidItem' is valid or not.
     */
    AbstractSeedIterator (ISeedModel* model, size_t firstIdx, size_t lastIdx, bool hasNextValidMethod);

    /** Destructor. */
    ~AbstractSeedIterator ();

    /** \copydoc ISeedIterator::first */
    void first()  { _nbRetrieved=0;  _currentIdx = _firstIdx;   if (findNextValidItem() && !isDone())  { updateItem(); } }

    /** \copydoc ISeedIterator::next */
    dp::IteratorStatus next()
    {
        _currentIdx++;
        if (findNextValidItem() && !isDone())  { updateItem(); }
        return dp::ITER_UNKNOWN;
    }

    /** \copydoc ISeedIterator::isDone */
    bool isDone()       { return (_currentIdx > _lastIdx);  }

    /** \copydoc ISeedIterator::currentItem */
    const ISeed* currentItem()  { return &_currentItem;             }

    /** \copydoc ISeedIterator::iterate */
    void iterate (void* aClient, Method method);

    /** \copydoc ISeedIterator::getNbTotal */
    u_int64_t getNbTotal ()  { return 0; }

    /** \copydoc ISeedIterator::setData */
    void setData (const database::IWord& data);

    /** \copydoc ISeedIterator::extract */
    ISeedIterator* extract (size_t firstIdx, size_t lastIdx) { return new NullSeedIterator (); }

    /** \copydoc ISeedIterator::retrieve */
    bool retrieve (ISeed& item, u_int64_t& nbRetrieved)
    {
        bool result = false;

        if (_synchro)  { _synchro->lock(); }
        if (isDone()==false)   {   item = *currentItem();   nbRetrieved = ++_nbRetrieved;   next ();    result = true;   }
        if (_synchro)  { _synchro->unlock(); }

        return result;
    }

    /** \copydoc ISeedIterator::createFilteredIterator */
    ISeedIterator* createFilteredIterator (const std::vector<size_t>& seedsIdx)  { return 0; }

protected:

    /** Seed model management. */
    ISeedModel* _model;

    /** Next mode; useful for optimizing the 'iterate' method. */
    bool _hasNextValidMethod;

    /** data management. */
    database::IWord _data;

    /** A little optimization (for avoiding too many methods calls and/or memory accesses.). */
    size_t _span;
    size_t _alphabetSize;

    /** Structure returned as current item of the iterator. */
    ISeed  _currentItem;

    /** Indexes used for iteration. */
    size_t _firstIdx;
    size_t _lastIdx;
    size_t _currentIdx;

    /** */
    os::ISynchronizer* _synchro;

    /** */
    u_int64_t  _nbRetrieved;

    /** Find the next valid item. May add some increment to the current position. */
    virtual bool findNextValidItem (void) = 0;

    /** Update the current item.  */
    virtual void updateItem (void) = 0;
};

/********************************************************************************/
} }  /* end of namespaces. */
/********************************************************************************/

#endif /* _ABSTRACT_SEED_ITERATOR_HPP_  */
