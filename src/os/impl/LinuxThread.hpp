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

/** \file LinuxThread.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Linux abstraction of thread management.
 */

#ifndef LINUX_THREAD_HPP_
#define LINUX_THREAD_HPP_

/********************************************************************************/

#include <os/api/IThread.hpp>

/********************************************************************************/
namespace os {
/** \brief Implementation of Operating System abstraction layer */
namespace impl {
/********************************************************************************/

/** \brief Factory that creates IThread instances.
 *
 *  Thread creation needs merely the main loop function that will be called.
 */
class LinuxThreadFactory : public IThreadFactory
{
public:

    /** Destructor. */
    virtual ~LinuxThreadFactory() {}

    /** \copydoc IThreadFactory::newThread */
    IThread* newThread (void* (*mainloop) (void*), void* data);

    /** \copydoc IThreadFactory::newSynchronizer */
    ISynchronizer* newSynchronizer (void);

    /** \copydoc IThreadFactory::getNbCores */
    size_t getNbCores ();

    /** \copydoc IThreadFactory::getHostName */
    std::string getHostName ();
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* LINUX_THREAD_HPP_ */
