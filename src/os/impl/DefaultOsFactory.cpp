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

#include "DefaultOsFactory.hpp"

#include "LinuxThread.hpp"
#include "LinuxTime.hpp"
#include "LinuxFile.hpp"

#include "WindowsThread.hpp"
#include "WindowsTime.hpp"
#include "WindowsFile.hpp"

#include "MacOsThread.hpp"
#include "MacOsTime.hpp"
#include "MacOsFile.hpp"

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
IThreadFactory&  DefaultFactory::getThreadFactory ()
{
#ifdef __LINUX__
    return LinuxThreadFactory::singleton();
#endif

#ifdef __WINDOWS__
    return WindowsThreadFactory::singleton();
#endif

#ifdef __DARWIN__
    return MacOsThreadFactory::singleton();
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
ITime& DefaultFactory::getTimeFactory   ()
{
#ifdef __LINUX__
    return LinuxTime::singleton();
#endif

#ifdef __WINDOWS__
    return WindowsTime::singleton();
#endif

#ifdef __DARWIN__
    return MacOsTime::singleton();
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
IFileFactory& DefaultFactory::getFileFactory   ()
{
#ifdef __LINUX__
    return LinuxFileFactory::singleton();
#endif

#ifdef __WINDOWS__
    return WindowsFileFactory::singleton();
#endif

#ifdef __DARWIN__
    return MacOsFileFactory::singleton();
#endif
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
