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

/*****************************************************************************
 *   Linux abstraction of time retrieval.
 *****************************************************************************/

#ifndef _LINUX_TIME_HPP_
#define _LINUX_TIME_HPP_

/********************************************************************************/

#include "ITime.hpp"

/********************************************************************************/
namespace os {
/********************************************************************************/

/** */
class LinuxTime : public ITime
{
public:

    static ITime& singleton ();

    virtual ~LinuxTime() {}

    /** Returns (in msec) the time. */
    u_int32_t gettime ();

    /** */
    u_int32_t getclock();
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _LINUX_TIME_HPP_ */
