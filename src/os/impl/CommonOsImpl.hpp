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

/** \file CommonOsImpl.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Implementations common to various OS..
 */

#ifndef _COMMON_IMPL_HPP_
#define _COMMON_IMPL_HPP_

/********************************************************************************/

#include <os/api/IFile.hpp>
#include <os/api/IMemory.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/********************************************************************************/
namespace os {
/** \brief Implementation of Operating System abstraction layer */
namespace impl {
/********************************************************************************/

/** \brief Abstract implementation of IFile interface
 *
 *  This implementation provides a few methods implementations (but not all) common
 *  to all operating systems.
 */
class CommonFile : public IFile
{
public:

    /** Constructor. */
    CommonFile (const char* path, const char* mode) : _handle(0)  {  _handle = fopen (path, mode);  }

    /** Destructor. */
    virtual ~CommonFile ()  { if (_handle)  { fclose (_handle); }  }

    /** \copydoc IFile::isEOF */
    bool isEOF ()  {  return (_handle ? feof (_handle) : true); }

    /** \copydoc IFile::gets */
    char* gets (char *s, int size)  { return (_handle==0 ? 0 : fgets (s, size, _handle)); }

    /** \copydoc IFile::print */
    void print   (const char* buffer)  { if (_handle)  {  fprintf (_handle, "%s", buffer); } }

    /** \copydoc IFile::println */
    void println (const char* buffer)  { if (_handle)  {  fprintf (_handle, "%s\n", buffer); } }

protected:
    FILE* _handle;
};

/********************************************************************************/

/** \brief Abstract implementation of IMemory interface
 *
 *  This implementation provides a few methods common to all operating systems.
 */
class CommonMemory : public IMemoryAllocator
{
public:

    /** Constructor. */
    CommonMemory () {}

    /** Destructor. */
    virtual ~CommonMemory () {}

    /** \copydoc IMemoryAllocator::malloc */
     void* malloc  (Size_t size)
     {
         void* res = ::malloc (size);
         if (!res)  {  throw MemoryFailure ("no memory for malloc"); }
         return res;
     }

     /** \copydoc IMemoryAllocator::calloc */
     void* calloc  (Size_t nmemb, Size_t size)
     {
         void* res = ::calloc (nmemb, size);
         if (!res)  {  throw MemoryFailure ("no memory for calloc"); }
         return res;
     }

     /** \copydoc IMemoryAllocator::realloc */
     void* realloc (void *ptr, Size_t size)
     {
         void* res = ::realloc (ptr, size);
         if (!res)  {  throw MemoryFailure ("no memory for realloc"); }
         return res;
     }

     /** \copydoc IMemoryAllocator::free */
     void  free (void *ptr)
     {
         if (ptr != 0)  {  ::free (ptr);  }
     }

     /** \copydoc IMemoryAllocator::dup */
     void* dup (const void *ptr, Size_t size)
     {
         void* result = this->malloc (size);
         if (result != 0)
         {
             if (ptr != 0)   {   memcpy (result, ptr, size);  }
             else            {   memset (result, 0,   size);  }
         }
         return result;
     }

     /** \copydoc IMemoryAllocator::strdup */
     char* strdup (const char* s)
     {
         char* res = ::strdup (s);
         if (!res)  {  throw MemoryFailure ("no memory for strdup"); }
         return res;
     }

     /** \copydoc IMemoryAllocator::getMemUsage */
     u_int32_t getMemUsage () { return 0; }
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _COMMON_IMPL_HPP_ */
