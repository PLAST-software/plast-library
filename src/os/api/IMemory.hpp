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

/** \file IMemory.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Implementation of an abstraction for dynamic memory allocation.
 *
 * We define abstraction for malloc,realloc... functions. Note that failures
 * during execution of the methods should throw a specific exception.
 */

#ifndef IMEMORY_HPP_
#define IMEMORY_HPP_

/********************************************************************************/

#include <os/api/IResource.hpp>
#include <misc/api/types.hpp>

#include <exception>
#include <string>

/********************************************************************************/
/** \brief Operating System abstraction layer */
namespace os  {
/********************************************************************************/

typedef u_int32_t Size_t;

/********************************************************************************/

/** \brief Exception class for memory failure
 */
class MemoryFailure : public IResource
{
public:

    /** Constructor with some informative string.
     * \param[in] message : a description of the exception.
     */
    MemoryFailure (const char* message) : _message(message)  {}

    /** Returns the description message.
     * \return the message
     */
    const char* getMessage ()  { return _message.c_str(); }

private:

    /** The informative message. */
    std::string _message;
};

/********************************************************************************/

/** \brief Interface providing methods for dynamic allocation.
 *
 *  This interface provides most common methods for creating/deleting dynamic allocation
 *  buffers.
 *
 *  In the future, we could think about specific implementations:
 *      - allocator that knows in advance the bunch of needed memory, so 'free' does nothing
 *      - statistical information
 *      - etc...
 */
class IMemoryAllocator : public IResource
{
public:

    /** Destructor. */
    virtual ~IMemoryAllocator () {}

    /** See malloc documentation. */
    virtual void* malloc  (Size_t size) = 0;

    /** See calloc documentation. */
    virtual void* calloc  (Size_t nmemb, Size_t size) = 0;

    /** See realloc documentation. */
    virtual void* realloc (void *ptr, Size_t size) = 0;

    /** See free documentation. */
    virtual void  free    (void *ptr) = 0;

    /** Duplicates the provided buffer.
     * \param[in] ptr  : address of the buffer to be duplicated.
     * \param[in] size : size of the buffer to be duplicated.
     * \return the duplicated buffer if successful, 0 otherwise.
     */
    virtual void* dup (const void* ptr, Size_t size) = 0;

    /** Duplicate a string.
     * \param[in] s : the string to be duplicated
     * \return the duplicated string if success, 0 otherwise.
     */
    virtual char* strdup (const char* s) = 0;

    /** Returns the currently used memory by the running process.
     * \return the currently used memory*/
    virtual u_int32_t getMemUsage () = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* IMEMORY_HPP_ */
