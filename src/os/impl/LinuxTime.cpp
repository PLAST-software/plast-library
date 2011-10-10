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

#include "LinuxTime.hpp"
#include <time.h>

#define CLOCK_REALTIME 0

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
ITime& LinuxTime::singleton ()
{
    static LinuxTime instance;
    return instance;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
u_int32_t LinuxTime::gettime ()
{
    u_int32_t result = 0;

    struct timespec tp;

    int err = clock_gettime (CLOCK_REALTIME, &tp);

    if (err == 0)
    {
        result = tp.tv_sec * 1000  +  tp.tv_nsec / 1000000;
    }

    return result;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
u_int32_t LinuxTime::getclock()
{
    u_int32_t result = 0;

    struct timespec tp;

    int err = clock_gettime (CLOCK_REALTIME, &tp);

    if (err == 0)
    {
        result = tp.tv_sec * 1000000  +  tp.tv_nsec / 1000;
    }

    return result;
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
