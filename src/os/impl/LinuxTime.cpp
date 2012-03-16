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

#ifdef __LINUX__

#include <os/impl/LinuxTime.hpp>
#include <ctime>
#include <unistd.h>
#include <sys/time.h>

/********************************************************************************/
namespace os { namespace impl {
/********************************************************************************/

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

    timeval t;

    int err = gettimeofday (&t, NULL);

    if (err == 0)  {  result = 1000*t.tv_sec +  t.tv_usec / 1000;  }

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

    timeval t;

    int err = gettimeofday (&t, NULL);

    if (err == 0)  {  result = 1000*t.tv_sec +  t.tv_usec / 1000;  }

    return result;
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* __LINUX__ */
