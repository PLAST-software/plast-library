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

/** \file WindowsTime.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Windows abstraction of time management.
 */

#ifndef _WINDOWS_TIME_HPP_
#define _WINDOWS_TIME_HPP_

/********************************************************************************/

#include <os/api/ITime.hpp>

/********************************************************************************/
namespace os {
/** \brief Implementation of Operating System abstraction layer */
namespace impl {
/********************************************************************************/

/** \brief Windows implementation that provides methods returning time.
 */
class WindowsTime : public ITime
{
public:

    /** Destructor. */
    virtual ~WindowsTime() {}

    /** \copydoc ITime::gettime */
    u_int32_t gettime ();

    /** \copydoc ITime::getclock */
    u_int32_t getclock();
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _WINDOWS_TIME_HPP_ */
