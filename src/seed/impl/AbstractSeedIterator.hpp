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

#ifndef _ABSTRACT_SEED_ITERATOR_HPP_
#define _ABSTRACT_SEED_ITERATOR_HPP_

/********************************************************************************/

#include "ISeedIterator.hpp"
#include "ISeedModel.hpp"

#include "IThread.hpp"

/********************************************************************************/
namespace seed {
/********************************************************************************/

/** Define a null seed iterator, i.e. that iterates an empty set.
 */
class NullSeedIterator : public ISeedIterator
{
public:

    virtual ~NullSeedIterator () {}

    void first()  {}
    dp::IteratorStatus next()   { return dp::ITER_DONE; }
    bool isDone() { return true; }
    const ISeed* currentItem()  { return 0; }

    u_int64_t getNbTotal ()  { return 0; }

    void setData (const database::IWord& data) {}

    /** Create a new iterator that is a subset of the current one. */
    ISeedIterator* extract (size_t firstIdx, size_t lastIdx) { return 0; }

    bool retrieve (ISeed& item, u_int64_t& nbRetrieved)  { return false; }

    ISeedIterator* createFilteredIterator (const std::vector<size_t>& seedsIdx)  { return 0; }
};

/********************************************************************************/


/** Define seed iterator with a seed model
 */
class AbstractSeedIterator : public ISeedIterator
{
public:

    AbstractSeedIterator (ISeedModel* model, size_t firstIdx, size_t lastIdx, bool hasNextValidMethod);
    ~AbstractSeedIterator ();

    void first()  { _nbRetrieved=0;  _currentIdx = _firstIdx;   if (findNextValidItem() && !isDone())  { updateItem(); } }

    dp::IteratorStatus next()
    {
        _currentIdx++;
        if (findNextValidItem() && !isDone())  { updateItem(); }
        return dp::ITER_UNKNOWN;
    }

    bool         isDone()       { return (_currentIdx > _lastIdx);  }
    const ISeed* currentItem()  { return &_currentItem;             }

    /** Method for iterating items through a method of a client. */
    void iterate (void* aClient, Iterator::Method method);

    /** Default implementation doesn't kwow how many items can be iterated.
     *  In such a case, we return 0 by convention.  */
    u_int64_t getNbTotal ()  { return 0; }

    /** */
    void setData (const database::IWord& data);

    /** Create a new iterator that is a subset of the current one.
     *  Note that the default implementation returns a null iterator. */
    ISeedIterator* extract (size_t firstIdx, size_t lastIdx) { return new NullSeedIterator (); }

    /** */
    bool retrieve (ISeed& item, u_int64_t& nbRetrieved)
    {
        bool result = false;

        if (_synchro)  { _synchro->lock(); }
        if (isDone()==false)   {   item = *currentItem();   nbRetrieved = ++_nbRetrieved;   next ();    result = true;   }
        if (_synchro)  { _synchro->unlock(); }

        return result;
    }

    /** Creation of filtered iterator. */
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
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ABSTRACT_SEED_ITERATOR_HPP_  */
