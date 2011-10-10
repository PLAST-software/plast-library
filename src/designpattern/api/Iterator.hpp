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

/*****************************************************************************
 *   This file holds interfaces related to the Design Pattern Iterator.
 *   It mainly provides abstraction for iterating objects hold in some
 *   container.
 *
 *   Sample of use:
 *
 *   dp::Iterator* it = new MyIterator ();
 *   for (it->first(); ! it->isDone(); it->next() )
 *   {
 *      // retrieve the current item of some type
 *      MyType* item = it->currentItem ();
 *   }
 *****************************************************************************/

#ifndef _ITERATOR_HPP_
#define _ITERATOR_HPP_

#include "SmartPointer.hpp"
#include "IObserver.hpp"
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

/********************************************************************************/
namespace dp {
/********************************************************************************/

/** Enum definition for the result of 'next' method. It may offer some time optimization
 *  when a 'isDone' is required after a 'next' call.
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

/** Definition of the Design Pattern Iterator interface.
 *  Defined as a template class.
 */
template <class Item> class Iterator : public SmartPointer
{
protected:
    class IteratorClient  {};

public:

    typedef void (IteratorClient::*Method) (Item t);

	virtual void first() = 0;
    virtual IteratorStatus next()  = 0;
    virtual bool isDone()      = 0;
    virtual Item currentItem() = 0;

    /** Method for iterating items through a method of a client. */
    virtual void iterate (void* aClient, Method method)
    {
        IteratorClient* client = (IteratorClient*) aClient;   // not very nice...
        if (client &&  method)  {  for (first(); !isDone(); next())  {  (client->*method) (currentItem());  }  }
    }
};

/********************************************************************************/

/** Class that counts the number of iterated items.
 *  Note that the iteration is actually done for computing the number.
 */

template <class Item> class IteratorCounter : public SmartPointer
{
public:
    IteratorCounter (Iterator<Item>* iterator) : _iterator(0), _counter(0), _isComputed(false)  { setIterator(iterator);  }
    virtual ~IteratorCounter ()  { setIterator(0); }

    size_t getCount ()  { compute();  return _counter; }

private:
    Iterator<Item>* _iterator;
    void setIterator (Iterator<Item>* iterator)
    {
        if (_iterator != 0)  {  _iterator->forget(); }
        _iterator = iterator;
        if (_iterator != 0)  {  _iterator->use(); }
    }

    size_t _counter;
    bool   _isComputed;

    void compute ()
    {
        for (_iterator->first(); ! _iterator->isDone(); _iterator->next())  { _counter++; }
        _isComputed = true;
    }
};

/********************************************************************************/

/** We define an id for the iteration events. */
const InterfaceId ITERATION_NOTIF_ID = (1<<0);

/** Define an event (see Observer design pattern) that provides information
 *  about the iteration. */

class IterationStatusEvent : public dp::EventInfo
{
public:

    /** */
    IterationStatusEvent (IteratorStatus status, u_int64_t currentIndex, u_int64_t totalNumber, const char* message, ...)
        : dp::EventInfo(ITERATION_NOTIF_ID),
          _status(status), _currentIndex(currentIndex), _totalNumber(totalNumber)
    {
        va_list ap;
        va_start (ap, message);
        vsnprintf (_buffer, sizeof(_buffer), message, ap);
        va_end(ap);
    }

    IteratorStatus  getStatus       () { return _status;        }
    u_int64_t       getCurrentIndex () { return _currentIndex;  }
    u_int64_t       getTotalNumber  () { return _totalNumber;   }
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
