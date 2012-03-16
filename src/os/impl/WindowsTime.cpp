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

#ifdef __WINDOWS__

#include <os/impl/WindowsTime.hpp>
#include <time.h>

#define CLOCK_REALTIME 0

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
u_int32_t WindowsTime::gettime ()
{
    return (u_int32_t) (float)(1000.0*clock()) / (float)CLOCKS_PER_SEC;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
u_int32_t WindowsTime::getclock()
{
    return (u_int32_t) (float)(1000.0*clock()) / (float)CLOCKS_PER_SEC;
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* __WINDOWS__ */
