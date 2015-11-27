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

/** \file ITime.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Interface for time retrieval.
 *
 * This is mainly used for debug/statistical information about the PLAST algorithm.
 */

#ifndef ITIME_HPP_
#define ITIME_HPP_

/********************************************************************************/

#include <os/api/IResource.hpp>
#include <misc/api/types.hpp>

/********************************************************************************/
/** \brief Operating System abstraction layer */
namespace os {
/********************************************************************************/

/** \brief Interface that provides methods returning time.
 */
class ITime : public IResource
{
public:

    /** Destructor. */
    virtual ~ITime () {}

    /** Returns (in msec) the time.
     * \return time
     */
    virtual u_int32_t gettime () = 0;

    /** Returns the clock.
     * \return clock
     */
    virtual u_int32_t getclock() = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* ITIME_HPP_ */
