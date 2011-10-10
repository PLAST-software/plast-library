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
 *   Operating System abstraction of what a thread is: something that can
 *   launch a function in a specific execution context.
 *
 *   A thread factory abstraction is also defined.
 *****************************************************************************/

#ifndef ITHREAD_HPP_
#define ITHREAD_HPP_

#include <stddef.h>

/********************************************************************************/
namespace os {
/********************************************************************************/

/** Definition of a thread. See below how to create a thread through a factory.
 */
class IThread
{
public:

    /** */
    virtual ~IThread () {}

    /** Wait the end of the thread. */
    virtual void join () = 0;
};

/********************************************************************************/

/** Factory that creates IThread instances.
 *  Thread creation needs merely the main loop function that will be called.
 */
class IThreadFactory
{
public:
    virtual IThread* newThread (void* (mainloop) (void*), void* data) = 0;

    virtual size_t getNbCores () = 0;
};

/********************************************************************************/

/** Define a synchronization class (like mutex) */
class ISynchronizer
{
public:

    virtual void   lock () = 0;
    virtual void unlock () = 0;
};

/********************************************************************************/

class LocalSynchronizer
{
public:
    LocalSynchronizer (ISynchronizer* ref) : _ref(ref)  {  if (_ref)  { _ref->lock (); }  }

    ~LocalSynchronizer ()  {  if (_ref)  {  _ref->unlock (); } }

private:
    ISynchronizer* _ref;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* ITHREAD_HPP_ */
