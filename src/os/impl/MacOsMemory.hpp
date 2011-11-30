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

#endif /* WINDOWS_MEMORY_HPP_ */
