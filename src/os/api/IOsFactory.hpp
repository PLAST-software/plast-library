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

/** \file IOsFactory.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Interface of operating system factories.
 *
 * It provides factories for:
 *   - threads
 *   - time
 *   - file
 *
 * Implementations of this factory should provide a singleton that can be used
 * as an entry point for managing os resources. It means that the PLAST code
 * should rely on such a singleton without knowing the actual implementation
 * (linux, windows, macos...). We therefore should not have any direct specific
 * OS dependency in the PLAST code.
 */

#ifndef I_OS_FACTORY_HPP_
#define I_OS_FACTORY_HPP_

/********************************************************************************/

#include <os/api/IResource.hpp>
#include <os/api/IThread.hpp>
#include <os/api/ITime.hpp>
#include <os/api/IFile.hpp>
#include <os/api/IMemory.hpp>

/********************************************************************************/
/** \brief Operating System abstraction layer */
namespace os {
/********************************************************************************/

/** \brief Interface providing factories for managing operating system resources.
 *
 *  The IFactory class provides getters on factories that create instances of OS
 *  related resources (threads, time, file, etc...).
 *
 *  Normally, the PLAST algorithm should use such an instance for getting OS resources
 *  instead of direcly call system dependant functions. This is important because it
 *  will ease the build of PLAST tools for different OS/architecture; the only thing
 *  to do is to use a specific instance of IFactory for matching the correct OS.
 */
class IFactory : public IResource
{
public:

    /** Destructor. */
    virtual ~IFactory () {}

    /** Provide a instance of a factory for dealing with threads.
     * \return the factory.
     */
    virtual IThreadFactory&    getThreadFactory () = 0;

    /** Provide a instance of a factory for dealing with files.
     * \return the factory.
     */
    virtual IFileFactory&      getFileFactory   () = 0;

    /** Provide a instance of a factory for dealing with time.
     * \return the factory.
     */
    virtual ITime&             getTimeFactory   () = 0;

    /** Provide a instance of a factory for dealing with memory.
     * \return the factory.
     */
    virtual IMemoryAllocator&  getMemoryFactory () = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* I_OS_FACTORY_HPP_ */
