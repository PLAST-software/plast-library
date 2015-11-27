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

/** \file LinuxMemory.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Linux abstraction of memory management.
 */

#ifndef LINUX_MEMORY_HPP_
#define LINUX_MEMORY_HPP_

/********************************************************************************/

#include <os/impl/CommonOsImpl.hpp>

/********************************************************************************/
namespace os {
/** \brief Implementation of Operating System abstraction layer */
namespace impl {
/********************************************************************************/

/** \brief Linux abstraction of memory management.
 */
class LinuxMemoryAllocator : public CommonMemory
{
public:

    /** \copydoc CommonMemory::getMemUsage */
    u_int32_t getMemUsage ();

private:

    /** */
    u_int32_t getPageSize();
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* LINUX_MEMORY_HPP_ */
