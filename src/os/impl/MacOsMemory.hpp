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

/** \file MacOsMemory.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief MacOs abstraction of memory management.
 */

#ifndef MACOS_MEMORY_HPP_
#define MACOS_MEMORY_HPP_

/********************************************************************************/

#include <os/impl/CommonOsImpl.hpp>

/********************************************************************************/
namespace os {
/** \brief Implementation of Operating System abstraction layer */
namespace impl {
/********************************************************************************/

/** \brief MacOs abstraction of memory management.
 */
class MacOsMemoryFactory : public CommonMemory
{
public:

    /** \copydoc CommonMemory::getMemUsage */
    u_int32_t getMemUsage ();
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* MACOS__MEMORY_HPP_ */
