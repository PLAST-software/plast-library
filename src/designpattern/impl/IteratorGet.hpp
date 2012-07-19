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

/** \file IteratorGet.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Implementation of an Iterator as a unique "get" method
 */

#ifndef _ITERATOR_GET_HPP_
#define _ITERATOR_GET_HPP_

#include <designpattern/api/Iterator.hpp>

#include <os/impl/DefaultOsFactory.hpp>

/********************************************************************************/
namespace dp {
/** \brief Implementation of Design Pattern tools (Observer, SmartPointer, Command...) */
namespace impl {
/********************************************************************************/

/** \brief Iterator get.
 */
template <class T> class IteratorGet : public dp::SmartPointer
{
public:

    /** Constructor. */
    IteratorGet (Iterator<T>* iterator)  : _iterator(0), _synchro(0), _nbRetrieved(0)
    {
        setIterator (iterator);

        _synchro = os::impl::DefaultFactory::singleton().thread().newSynchronizer();

        _iterator->first ();
    }

    /** Destructor. */
    virtual ~IteratorGet ()
    {
        setIterator (0);
        if (_synchro)  { delete _synchro; }
    }

    /** Retrieve the current object in the iteration.
     * \param[out] item: the retrieved object
     * \param[out] nbRetrieved: the number of objects retrieved so far.
     * \return true if the iteration is running, false if the iteration is over. */
    bool get (T& item, size_t& nbRetrieved)
    {
        bool result = false;

        if (_synchro)  { _synchro->lock (); }

        result = ! _iterator->isDone();
        if (result)
        {
            item = _iterator->currentItem();
            _iterator->next ();
            nbRetrieved = ++ _nbRetrieved;
        }

        if (_synchro)  { _synchro->unlock (); }

        return result;
    }

private:

    /** Referenced iterator. */
    Iterator<T>* _iterator;
    void setIterator (Iterator<T>* iterator)  { SP_SETATTR (iterator); }

    /** Synchronizer. */
    os::ISynchronizer* _synchro;

    size_t _nbRetrieved;
};

/********************************************************************************/

/** */
template <class T> class IteratorCommand : public dp::ICommand
{
public:

    /** Constructor. */
    IteratorCommand (Iterator<T>* iterator)  : _iteratorGet(0)  {  setIteratorGet (new IteratorGet<T> (iterator));  }

    /** Destructor. */
    virtual ~IteratorCommand ()  {  setIteratorGet (0);  }

    /** */
    void execute ()
    {
        preExecute ();

        size_t nbGot;
        for (T current ;  _iteratorGet->get (current, nbGot); )   {  execute (current, nbGot);  }

        postExecute ();
    }

protected:

    /** */
    virtual void execute (T& current, size_t& nbGot) = 0;

    /** */
    virtual void preExecute  ()  {}
    virtual void postExecute ()  {}

private:

    IteratorGet<T>* _iteratorGet;
    void setIteratorGet (IteratorGet<T>* iteratorGet)  { SP_SETATTR (iteratorGet); }
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _ITERATOR_GET_HPP_ */
