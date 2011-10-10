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
 *   Implementation of IThread interface for Linux OS.
 *****************************************************************************/

#ifndef LINUX_THREAD_HPP_
#define LINUX_THREAD_HPP_

/********************************************************************************/

#include "IThread.hpp"
#include <string>
#include <pthread.h>

/********************************************************************************/
namespace os {
/********************************************************************************/

class LinuxThreadFactory : public IThreadFactory
{
public:

    static IThreadFactory& singleton ();

    virtual ~LinuxThreadFactory() {}

    IThread* newThread (void* (mainloop) (void*), void* data);

    size_t getNbCores ();

private:

    /** */
    class LinuxThread : public IThread
    {
    public:
        LinuxThread (void* (mainloop) (void*), void* data);
        ~LinuxThread ();
        void join ();
    private:
        pthread_t  _thread;
    };
};

/********************************************************************************/

class LinuxSynchronizer : public ISynchronizer
{
public:
    LinuxSynchronizer ()            {  pthread_mutex_init (&_mutex, NULL);  }
    virtual ~LinuxSynchronizer()    {  pthread_mutex_destroy (&_mutex);     }

    void   lock ()  { pthread_mutex_lock   (&_mutex); }
    void unlock ()  { pthread_mutex_unlock (&_mutex); }

private:
    pthread_mutex_t  _mutex;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* LINUX_THREAD_HPP_ */
