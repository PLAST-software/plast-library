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

#include <os/impl/DefaultOsFactory.hpp>

#include <os/impl/LinuxThread.hpp>
#include <os/impl/LinuxTime.hpp>
#include <os/impl/LinuxFile.hpp>
#include <os/impl/LinuxMemory.hpp>
#include <os/impl/LinuxMemoryFile.hpp>

#include <os/impl/WindowsThread.hpp>
#include <os/impl/WindowsTime.hpp>
#include <os/impl/WindowsFile.hpp>
#include <os/impl/WindowsMemory.hpp>
#include <os/impl/WindowsMemoryFile.hpp>

#include <os/impl/MacOsThread.hpp>
#include <os/impl/MacOsTime.hpp>
#include <os/impl/MacOsFile.hpp>
#include <os/impl/MacOsMemory.hpp>
#include <os/impl/MacOsMemoryFile.hpp>

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
DefaultFactory::DefaultFactory ()
{
#ifdef __LINUX__
    _thread = new LinuxThreadFactory ();
    _time   = new LinuxTime ();
    _file   = new LinuxFileFactory ();
    _memory = new LinuxMemoryAllocator ();
    _fileMem = new LinuxMemoryFileFactory();
#endif

#ifdef __WINDOWS__
    _thread = new WindowsThreadFactory ();
    _time   = new WindowsTime ();
    _file   = new WindowsFileFactory ();
    _memory = new WindowsMemoryFactory ();
    _fileMem = new WindowsMemoryFileFactory ();
#endif

#ifdef __DARWIN__
    _thread = new MacOsThreadFactory ();
    _time   = new MacOsTime ();
    _file   = new MacOsFileFactory ();
    _memory = new MacOsMemoryFactory ();
    _fileMem = new MacOsMemoryFileFactory ();
#endif
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
DefaultFactory::~DefaultFactory ()
{
    if (_thread)    { delete _thread; }
    if (_time)      { delete _time;   }
    if (_file)      { delete _file;   }
    if (_memory)    { delete _memory; }
    if (_fileMem)    { delete _fileMem; }
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
