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

/** \file IThread.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Operating System abstraction of a thread
 *
 *   Operating System abstraction of what a thread is: something that can
 *   launch a function in a specific execution context.
 *
 *   It provides also means for synchronization between threads.
 *
 *   A thread factory abstraction is also defined.
 */

#ifndef ITHREAD_HPP_
#define ITHREAD_HPP_

#include <os/api/IResource.hpp>
#include <stddef.h>
#include <string>

/********************************************************************************/
/** \brief Operating System abstraction layer */
namespace os {
/********************************************************************************/

/** \brief Define what a thread is.
 *
 * Definition of a thread in an OS independant fashion. See below how to create a thread through a factory.
 */
class IThread : public IResource
{
public:

    /** Destructor. */
    virtual ~IThread () {}

    /** Wait the end of the thread. */
    virtual void join () = 0;
};

/********************************************************************************/

/** \brief Define a synchronization abstraction
 *
 *  This is an abstraction layer of what we need for handling synchronization.
 *  Actual implementations may use mutex for instance.
 */
class ISynchronizer : public IResource
{
public:

    /** Destructor. */
    virtual ~ISynchronizer () {}

    /** Lock the synchronizer. */
    virtual void   lock () = 0;

    /** Unlock the synchronizer. */
    virtual void unlock () = 0;
};

/********************************************************************************/

/** \brief Factory that creates IThread instances.
 *
 *  Thread creation needs merely the main loop function that will be called.
 *
 *  Note the method that can return the number of cores in case a multi-cores
 *  architecture is used. This is useful for automatically configure PLAST for
 *  using the maximum number of available cores for speeding up the algorithm.
 */
class IThreadFactory : public IResource
{
public:

    /** Destructor. */
    virtual ~IThreadFactory ()  {}

    /** Creates a new thread.
     * \param[in] mainloop : the function the thread shall execute
     * \param[in] data :  data provided to the mainloop when launched
     * \return the created thread.
     */
    virtual IThread* newThread (void* (*mainloop) (void*), void* data) = 0;

    /** Creates a new synchronization object.
     * \return the created ISynchronizer instance
     */
    virtual ISynchronizer* newSynchronizer (void) = 0;

    /** Returns the number of available cores.
     * \return the number of cores. */
    virtual size_t getNbCores () = 0;

    /** Returns the host name.
     * \return the host name. */
    virtual std::string getHostName () = 0;
};

/********************************************************************************/

/** \brief Tool for locally managing synchronization.
 *
 *  Instances of this class reference a ISynchronizer instance. When created, they lock
 *  their refered ISynchronizer and when destroyed, they unlock it.
 *
 *  For instance, it is a convenient way to lock/unlock a ISynchronizer instance in the scope
 *  of a statements block (where the LocalSynchronizer instance lives), a particuliar case being
 *  the statements block of a method.
 *
 *  Code sample:
 *  \code
 *  void sample (ISynchronizer* synchronizer)
 *  {
 *      // we create a local synchronizer from the provided argument
 *      LocalSynchronizer localsynchro (synchronizer);
 *
 *      // now, all the statements block is locked for the provided synchronizer
 *      // in other word, this sample function is protected against concurrent accesses.
 *  }
 *  \endcode
 */
class LocalSynchronizer : public IResource
{
public:

    /** Constructor.
     * \param[in] ref : the ISynchronizer instance to be controlled.
     */
    LocalSynchronizer (ISynchronizer* ref) : _ref(ref)  {  if (_ref)  { _ref->lock (); }  }

    /** Destructor. */
    ~LocalSynchronizer ()  {  if (_ref)  {  _ref->unlock (); } }

private:

    /** The refered ISynchronizer instance. */
    ISynchronizer* _ref;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* ITHREAD_HPP_ */
