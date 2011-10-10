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

#include "LinuxThread.hpp"
#include "macros.hpp"
#include <memory>
#include <stdio.h>
#include <string.h>

/********************************************************************************/
namespace os {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IThreadFactory& LinuxThreadFactory::singleton ()
{
    static LinuxThreadFactory instance;
    return instance;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IThread* LinuxThreadFactory::newThread (void* (mainloop) (void*), void* data)
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
LinuxThreadFactory::LinuxThread::LinuxThread (void* (mainloop) (void*), void* data)
{
    pthread_create (&_thread, NULL,  mainloop, data);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
LinuxThreadFactory::LinuxThread::~LinuxThread ()
{
    pthread_detach (_thread);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void LinuxThreadFactory::LinuxThread::join ()
{
    pthread_join (_thread, NULL);
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
