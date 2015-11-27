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

#ifdef __WINDOWS__

#include <os/impl/WindowsThread.hpp>
#include <misc/api/macros.hpp>
#include <memory>
#include <stdio.h>
#include <string.h>

#include <windows.h>

#include <unistd.h>

using namespace std;

#define DEBUG(a)  //printf a

/********************************************************************************/
namespace os { namespace impl {
/********************************************************************************/

typedef DWORD (WINAPI * THREADROUTINE)(void *);

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
class WindowsThread : public IThread
{
public:

    WindowsThread (THREADROUTINE mainloop, void* data)
    {
         _thread = CreateThread (NULL, 0, (THREADROUTINE) mainloop, data, 0, NULL);
    }

    ~WindowsThread ()
    {
        TerminateThread (_thread, 0);
        CloseHandle (_thread);
    }

    void join ()
    {
        WaitForSingleObject (_thread, INFINITE);
        CloseHandle (_thread);
    }

private:
    HANDLE  _thread;
};

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
class WindowsSynchronizer : public ISynchronizer
{
public:
    WindowsSynchronizer ()
    {
        _mutex = CreateMutex (NULL, FALSE, NULL);
    }

    virtual ~WindowsSynchronizer()
    {
        CloseHandle (_mutex);
    }

    void lock ()
    {
        WaitForSingleObject (_mutex, INFINITE);
    }

    void unlock ()
    {
        ReleaseMutex (_mutex);
    }

private:
    HANDLE  _mutex;
};

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IThread* WindowsThreadFactory::newThread (void* (*mainloop) (void*), void* data)
{
    return new WindowsThread ((THREADROUTINE)mainloop, data);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ISynchronizer* WindowsThreadFactory::newSynchronizer (void)
{
    return new WindowsSynchronizer ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
size_t WindowsThreadFactory::getNbCores ()
{
    size_t result = 0;

    SYSTEM_INFO sysinfo;
    GetSystemInfo (&sysinfo);
    result = sysinfo.dwNumberOfProcessors;

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
string WindowsThreadFactory::getHostName ()
{
    string result;

    TCHAR  infoBuf[1024];
    DWORD  bufCharCount = sizeof(infoBuf)/sizeof(infoBuf[0]);

    if (GetComputerName( infoBuf, &bufCharCount ) )
    {
        result.assign (infoBuf, strlen(infoBuf));
    }

    return result;
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* __WINDOWS__ */
