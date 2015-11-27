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

/** \file Iterator.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Iterator Design Pattern definition
 *
 *   This file holds interfaces related to the Design Pattern Iterator.
 *   It mainly provides abstraction for iterating objects hold in some
 *   container.
 */

#ifndef _ITERATOR_HPP_
#define _ITERATOR_HPP_

#include <designpattern/api/IObserver.hpp>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

/********************************************************************************/
/** \brief Design Pattern tools (Observer, SmartPointer, Command...) */
namespace dp {
/********************************************************************************/

/** It may offer some time optimization when a 'isDone' is required after a 'next' call.
 *  Note that the feature may be not implemented since the isdone information may not be
 *  known at the end of a 'next' call.
 */
enum IteratorStatus
{
    ITER_STARTING,
    ITER_ON_GOING,
    ITER_DONE,
    ITER_UNKNOWN
};

/********************************************************************************/

/** \brief  Definition of the Design Pattern Iterator interface.
 *
 *  Iteration is an important concept, which is specially true for PLAST whose core algorithm
 *  iterates over some seeds set. A very important point in PLAST parallelization is that the
 *  full seeds iterator can be split into several ones, each of them being iterated in a specific
 *  thread (and therefore on specific core if we have a multi-cores computer).
 *
 *  So the Iterator concept is here reified as a template class that knows how to iterate some set of objects.
 *
 *  Actually, the interface has two ways for iterating instances:
 *    1- the 'classic' one in terms of Iterator Design Pattern (see first/next/isDone methods)
 *    2- a callback way (see 'iterate' methods) where some client provides a callback that will be called for each object
 *
 *  There may be good reasons for using one way or another. For instance, the first one may be easier to use by clients
 *  (no extra method to be defined) but may be less efficient because more methods calls will be carried out.
 *
 *  Note that 'iterate' has great subclassing potentiality; by default, its implementation relies on first/isDone/next/currentItem
 *  methods and so can't be faster that the 'classic' way. But specific implementations of Iterator may implement 'iterate'
 *  directly by using inner state of the class, and so without call to  first/isDone/next/currentItem methods.
 *
 *  Note that for optimization point of view, PLAST algorithm prefers to use the 'iterate' way with optimized implementations.
 *
 *  Sample of use:
 *  \code
 *   dp::Iterator<MyType*>* it = new MyIterator ();
 *   for (it->first(); ! it->isDone(); it->next() )
 *   {
 *      // retrieve the current item of some type
 *      MyType* item = it->currentItem ();
 *   }
 *  \endcode
 *
 *
 *  A remark on the STL: the Standard Template Library provides also the iterator concept. For instance, one could write:
 *  \code
 *   std::list<MyType*> l;
 *
 *   // we suppose here that we add some items to the list.
 *
 *   for (std::list<MyType*>::iterator it = l.begin(); it != l.end(); it++)
 *   {
 *      // retrieve the current item of some type
 *      MyType* item = *it;
 *   }
 *  \endcode
 *
 *  We can see the following differences with our own iterator.
 *   \li the iteration loop with the STL needs to know the iterated list, through the beginning iterator 'l.begin()'
 *   and the ending iterator 'l.end()'. In our case, we don't have any reference of the iterated container, we can see
 *   only a reference on the iterator itself.
 *   \li the iteration loop with the STL makes apparent the actual type of the container ('list' in the example), which
 *   is not the case for our iterator, where we can see only the type of the iterated items.
 *   \li the STL iterators can only iterate contents of containers, ie. some items already in memory. Our iterator concept
 *   is more general because it can iterate items that it builds on the fly, without having the whole items mounted in memory.
 *   A typical use is the parsing of a huge FASTA database (say 8 GBytes for instance). With a STL iterator, we should have
 *   the whole sequences lying in some containers before iterating them; with our iterator, it is enough to know a sequence
 *   at a given time. Note however that clients of such iterators must not reference a sequence that is transient by nature.
 *   In such a case, clients have to copy, if needed, the iterated sequence for some local work.
 *
 *  In brief, our iterator encapsulates more information that the STL iterator, allows to iterate potential containers (versus
 *  actual containers) and may be easier to use.
 *
 *  Moreover, we can use our iterator as a basis for other ways for iteration. See \ref dp::impl::IteratorGet
 */
template <class Item> class Iterator : public SmartPointer
{
protected:
    class IteratorClient  {};

public:

    /** Method that initializes the iteration. */
	virtual void first() = 0;

    /** Method that goes to the next item in the iteration.
     * \return status of the iteration
     */
	virtual IteratorStatus next()  = 0;

	/** Method telling whether the iteration is finished or not.
     * \return true if iteration is finished, false otherwise.
	 */
    virtual bool isDone()      = 0;

    /** Method that returns the current iterated item. Note that the returned type is the template type.
        \return the current item in the iteration.
    */
    virtual Item currentItem() = 0;

    /** Define the prototype of the callback method (see 'iterate' method). */
    typedef void (IteratorClient::*Method) (Item t);

    /** Method for iterating items through a method of a client.
     * \param[in] aClient : client instance (as a void*) that will be called through the 'method' called.
     * \param[in] method : callback method
     */
    virtual void iterate (void* aClient, Method method)
    {
        IteratorClient* client = (IteratorClient*) aClient;   // not very nice...
        if (client &&  method)  {  for (first(); !isDone(); next())  {  (client->*method) (currentItem());  }  }
    }
};

/********************************************************************************/

/** \brief Iterators that can return the number of items it can iterate.
 *
 *  In general, an iterator is not supposed to know the number of items it is about
 *  to iterate. For instance, when parsing a FASTA file with a FastaSequenceIterator,
 *  we are aware of the number of items at the end of the iteration.
 *
 *  In some case, when the set of items to be iterated is known (read "is in memory"),
 *  we can reach the number of items before the iteration.
 *
 *  This SmartIterator interface improves the Iterator interface in that way by providing
 *  a 'size' method returning the number of items.
 */
template <class Item> class SmartIterator : public Iterator<Item>
{
public:

    /** Destructor. */
    virtual ~SmartIterator () {}

    /** Return the number of iterated items.
     * \return number of iterated items*/
    virtual u_int32_t size () = 0;
};

/********************************************************************************/

/** \brief Null implementation of the Iterator interface.
 *
 * This iterator iterates no item at all. This is achieved by returning always true
 * in the isDone method implementation.
 */
template <class Item> class NullIterator : public Iterator<Item>
{
public:

    /** Destructor. */
    virtual ~NullIterator () {}

    /** \copydoc Iterator::first  */
    void first() {}

    /** \copydoc Iterator::next  */
    dp::IteratorStatus next() { return dp::ITER_UNKNOWN; }

    /** \copydoc Iterator::isDone  */
    bool isDone() { return true; }

    /** \copydoc Iterator::isDone  */
    Item currentItem() { return 0; }
};

/********************************************************************************/

/** We define an id for the iteration events. */
const InterfaceId ITERATION_NOTIF_ID = (1<<0);

/** \brief Notification class specific to iteration
 *
 *  Define an event (see Observer design pattern) that provides information about the iteration.
 */
class IterationStatusEvent : public dp::EventInfo
{
public:

    /** Constructor of the iteration information.
     * \param[in] status : status of the iteration (not started, ongoing, finished...)
     * \param[in] currentIndex : index in the iteration
     * \param[in] totalNumber : number of iterations
     * \param[in] message : message related to the notification (printf-like prototype)
     */
    IterationStatusEvent (
        IteratorStatus status,
        u_int64_t currentIndex,
        u_int64_t totalNumber,
        const char* message,
        ...
    )
        : dp::EventInfo(ITERATION_NOTIF_ID),
          _status(status), _currentIndex(currentIndex), _totalNumber(totalNumber)
    {
        va_list ap;
        va_start (ap, message);
        vsnprintf (_buffer, sizeof(_buffer), message, ap);
        va_end(ap);
    }

    /** Returns the status of the iteration.
     * \return the iteration status. */
    IteratorStatus  getStatus       () { return _status;        }

    /** Return the current index of the iteration.
     * \return current index in the iteration. */
    u_int64_t       getCurrentIndex () { return _currentIndex;  }

    /** Returns the total number of iterated items.
     * \return  total number of iterated items */
    u_int64_t       getTotalNumber  () { return _totalNumber;   }

    /** Return an informative message associated to the notification.
     * \return the message. */
    const char*     getMessage      () { return _buffer;        }

private:
    IteratorStatus  _status;
    u_int64_t       _currentIndex;
    u_int64_t       _totalNumber;
    char            _buffer[256];
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ITERATOR_HPP_ */
