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

#ifdef __DARWIN__

#include "MacOsThread.hpp"
#include "macros.hpp"
#include <memory>
#include <stdio.h>
#include <string.h>

#define DEBUG(a)  //printf a

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
class MacOsThread : public IThread
{
public:

    MacOsThread (void* (mainloop) (void*), void* data)
    {
    }

    ~MacOsThread ()
    {
    }

    void join ()
    {
    }

private:
};

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
class MacOsSynchronizer : public ISynchronizer
{
public:
    MacOsSynchronizer ()
    {
    }

    virtual ~MacOsSynchronizer()
    {
    }

    void lock ()
    {
    }

    void unlock ()
    {
    }

private:
};

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IThreadFactory& MacOsThreadFactory::singleton ()
{
    static MacOsThreadFactory instance;
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
IThread* MacOsThreadFactory::newThread (void* (mainloop) (void*), void* data)
{
    return new MacOsThread (mainloop, data);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ISynchronizer* MacOsThreadFactory::newSynchronizer (void)
{
    return new MacOsSynchronizer ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
size_t MacOsThreadFactory::getNbCores ()
{
    size_t result = 0;

    if (result==0)  { result = 1; }

    return result;
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* __DARWIN__ */
