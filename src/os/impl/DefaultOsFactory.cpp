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

#include <os/impl/DefaultOsFactory.hpp>

#include <os/impl/LinuxThread.hpp>
#include <os/impl/LinuxTime.hpp>
#include <os/impl/LinuxFile.hpp>
#include <os/impl/LinuxMemory.hpp>

#include <os/impl/WindowsThread.hpp>
#include <os/impl/WindowsTime.hpp>
#include <os/impl/WindowsFile.hpp>
#include <os/impl/WindowsMemory.hpp>

#include <os/impl/MacOsThread.hpp>
#include <os/impl/MacOsTime.hpp>
#include <os/impl/MacOsFile.hpp>
#include <os/impl/MacOsMemory.hpp>

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
#endif

#ifdef __WINDOWS__
    _thread = new WindowsThreadFactory ();
    _time   = new WindowsTime ();
    _file   = new WindowsFileFactory ();
    _memory = new WindowsMemoryFactory ();
#endif

#ifdef __DARWIN__
    _thread = new MacOsThreadFactory ();
    _time   = new MacOsTime ();
    _file   = new MacOsFileFactory ();
    _memory = new MacOsMemoryFactory ();
#endif
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
