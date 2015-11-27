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

/** \file DefaultOsFactory.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Default implementation of operating system factories.
 *
 * This implementation is used through most of the PLAST code, which makes this
 * code dependant on a small OS abstraction layer.
 *
 * The actual factories are returned according to some compilation flag that reflects
 * the operating system (__LINUX__, __WINDOWS__, etc...)
 */

#ifndef DEFAULT_OS_FACTORY_HPP_
#define DEFAULT_OS_FACTORY_HPP_

/********************************************************************************/

#include <os/api/IOsFactory.hpp>

/********************************************************************************/
namespace os {
/** \brief Implementation of Operating System abstraction layer */
namespace impl {
/********************************************************************************/

/** \brief Default implementation of OS abstraction layer.
 *
 *  It provides actual factories instances for managing OS related resources (threads, time, file).
 *
 *  These factories are returned according to some compilation flag that reflects
 *  the operating system (__LINUX__, __WINDOWS__, etc...)
 *
 *  The DefaultFactory class is usable through a singleton() method. Note that some static methods
 *  have been also defined as shortcuts.
 *
 *  Code sample:
 *  \code
 *  void sample ()
 *  {
 *      // creation of a thread (version 1)
 *      IThread* t1 = DefaultFactory::singleton().getThreadFactory().newThread (mainloop, NULL);
 *
 *      // creation of a thread (version 2)
 *      IThread* t2 = DefaultFactory::thread().newThread (mainloop, NULL);
 *  }
 *  \endcode
 */
class DefaultFactory : public IFactory
{
public:

    /** Singleton.
     * \return the singleton
     */
    static DefaultFactory& singleton ()  { static DefaultFactory instance;  return instance; }

    /** \copydoc IFactory::getThreadFactory */
    IThreadFactory&    getThreadFactory ()  { return *_thread; }

    /** \copydoc IFactory::getTimeFactory */
    ITime&             getTimeFactory   ()  { return *_time; }

    /** \copydoc IFactory::getFileFactory */
    IFileFactory&      getFileFactory   ()  { return *_file; }

    /** \copydoc IFactory::getMemoryFactory */
    IMemoryAllocator&  getMemoryFactory ()  { return *_memory; }

    /** \copydoc IFactory::getFileMemFactory */
    IMemoryFileFactory&      getFileMemFactory   ()  { return *_fileMem; }

    /** Shorcut for thread methods. */
    static IThreadFactory&    thread ()  { return singleton().getThreadFactory();   }

    /** Shorcut for time methods. */
    static ITime&             time   ()  { return singleton().getTimeFactory();     }

    /** Shorcut for file methods. */
    static IFileFactory&      file   ()  { return singleton().getFileFactory();     }

    /** Shorcut for memory methods. */
    static IMemoryAllocator&  memory ()  { return singleton().getMemoryFactory();   }

    /** Shorcut for file methods. */
    static IMemoryFileFactory&  fileMem   ()  { return singleton().getFileMemFactory();     }

private:

    /** Constructor. */
    DefaultFactory ();

    /** Destructor. */
    virtual ~DefaultFactory ();

    /** */
    IThreadFactory*   _thread;

    /** */
    ITime*            _time;

    /**  */
    IFileFactory*     _file;

    /** */
    IMemoryAllocator* _memory;

    /**  */
    IMemoryFileFactory* _fileMem;

};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* I_OS_FACTORY_HPP_ */
