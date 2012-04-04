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

#include <os/impl/LinuxMemory.hpp>

#include <stdio.h>
#include <unistd.h>

/********************************************************************************/
namespace os {
/** \brief Implementation of Operating System abstraction layer */
namespace impl {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
u_int32_t LinuxMemoryAllocator::getMemUsage ()
{
    u_int32_t val = 0;

    FILE* file = fopen ("/proc/self/statm", "r");
    if (file)
    {
        char buffer[128];
        fgets (buffer, sizeof(buffer), file);
        char* locate = strchr (buffer, ' ');
        if (locate)  {  val = atol(locate); }
        fclose (file);
    }

    u_int32_t tmp = (val*getPageSize()) / 1024;

    return tmp;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
u_int32_t LinuxMemoryAllocator::getPageSize()
{
    static u_int32_t page = 0;
    if (page==0)  { page = sysconf(_SC_PAGE_SIZE); }
    return page;
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* __LINUX__ */
