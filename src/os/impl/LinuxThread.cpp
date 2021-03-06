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

#ifdef __LINUX__

#include <os/impl/LinuxThread.hpp>
#include <misc/api/macros.hpp>
#include <memory>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include <unistd.h>

using namespace std;

/********************************************************************************/
namespace os { namespace impl {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
class LinuxThread : public IThread
{
public:
    LinuxThread (void* (mainloop) (void*), void* data)  { pthread_create (&_thread, NULL,  mainloop, data); }
    ~LinuxThread ()  { /* pthread_detach (_thread); */  }
    void join ()     { pthread_join   (_thread, NULL);  }
private:
    pthread_t  _thread;
};

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
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

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IThread* LinuxThreadFactory::newThread (void* (*mainloop) (void*), void* data)
{
    return new LinuxThread (mainloop, data);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ISynchronizer* LinuxThreadFactory::newSynchronizer (void)
{
    return new LinuxSynchronizer ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
size_t LinuxThreadFactory::getNbCores ()
{
    size_t result = 0;

    /** We open the "/proc/cpuinfo" file. */
    FILE* file = fopen ("/proc/cpuinfo", "r");
    if (file)
    {
        char buffer[256];

        while (fgets(buffer, sizeof(buffer), file))
        {
            if (strstr(buffer, "processor") != NULL)  { result ++;  }
        }

        fclose (file);
    }

    if (result==0)  { result = 1; }

    return result;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
string LinuxThreadFactory::getHostName ()
{
    string result;

    char hostname[1024];
    hostname[1023] = '\0';
    gethostname (hostname, sizeof(hostname)-1);

    result.assign (hostname, strlen(hostname));

    return result;
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* __LINUX__ */
