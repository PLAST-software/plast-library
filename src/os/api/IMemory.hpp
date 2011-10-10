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
 *   Implementation of an abstraction for dynamic memory allocation.
 *****************************************************************************/

#ifndef IMEMORY_HPP_
#define IMEMORY_HPP_

/********************************************************************************/

#include <exception>
#include <string>
#include <sys/types.h>

/********************************************************************************/
namespace os  {
/********************************************************************************/

typedef u_int32_t Size_t;

/********************************************************************************/

class MemoryFailure
{
public:
    MemoryFailure (const char* message) : _message(message)  {}
    const char* getMessage ()  { return _message.c_str(); }
private:
    std::string _message;
};

/********************************************************************************/

class IMemoryAllocator
{
public:

    virtual void* malloc  (Size_t size) = 0;
    virtual void* calloc  (Size_t nmemb, Size_t size) = 0;
    virtual void* realloc (void *ptr, Size_t size) = 0;
    virtual void  free    (void *ptr) = 0;

    virtual void* dup (void *ptr, Size_t size) = 0;
};

/********************************************************************************/

class IMemoryUsage
{
public:
    virtual u_int32_t getMemUsage () = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* IMEMORY_HPP_ */
