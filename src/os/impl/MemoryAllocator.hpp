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
 *   Implementation of IMemoryAllocator interface.
 *****************************************************************************/

#ifndef _MEMORY_ALLOCATOR_HPP_
#define _MEMORY_ALLOCATOR_HPP_

/********************************************************************************/

#include "IMemory.hpp"

#include <stdlib.h>
#include <string.h>
#include <exception>

#include <stdio.h>

#include <list>

//#define WITH_MEM_STATS 1

/********************************************************************************/
namespace os {
/********************************************************************************/

/** Default implementation of memory allocator. Use inline for speed.
 */
class MemoryAllocator // : public IMemoryAllocator
{
public:

    static MemoryAllocator& singleton() { static MemoryAllocator instance; return instance; }

    /** */
    void* malloc  (Size_t size)
    {
        void* res = ::malloc (size);
        if (!res)  {  throw MemoryFailure ("no memory for malloc"); }
#ifdef WITH_MEM_STATS
        __sync_fetch_and_add (&_nbMalloc, 1);
        __sync_fetch_and_add (&_totalAllocated, size);
#endif
        return res;
    }

    /** */
    void* calloc  (Size_t nmemb, Size_t size)
    {
        void* res = ::calloc (nmemb, size);
        if (!res)  {  throw MemoryFailure ("no memory for calloc"); }
#ifdef WITH_MEM_STATS
        __sync_fetch_and_add (&_nbCalloc, 1);
        __sync_fetch_and_add (&_totalAllocated, nmemb*size);
#endif
        return res;
    }

    /** */
    void* realloc (void *ptr, Size_t size)
    {
        void* res = ::realloc (ptr, size);
        if (!res)  {  throw MemoryFailure ("no memory for realloc"); }
#ifdef WITH_MEM_STATS
        if (res != ptr)
        {
            __sync_fetch_and_add (&_nbRealloc, 1);
            __sync_fetch_and_add (&_totalAllocated, size);
        }
#endif
        return res;
    }

    /** */
    void  free (void *ptr)
    {
        if (ptr != 0)
        {
            ::free (ptr);
#ifdef WITH_MEM_STATS
            __sync_fetch_and_add (&_nbFree, 1);
#endif
        }
    }

    /** */
    void* dup (const void *ptr, Size_t nmemb, Size_t size)
    {
        void* result = this->calloc (nmemb, size);
        if (result != 0)
        {
            if (ptr != 0)   {   memcpy (result, ptr, nmemb*size);  }
            else            {   memset (result, 0,   nmemb*size);  }
        }
        return result;
    }

    /** */
    char* strdup (const char* s)
    {
        char* res = ::strdup (s);
        if (!res)  {  throw MemoryFailure ("no memory for strdup"); }
        return res;
    }


    /** */
    void  resetStats ()
    {
        _nbMalloc       = 0;
        _nbCalloc       = 0;
        _nbRealloc      = 0;
        _nbFree         = 0;
        _totalAllocated = 0;
    }

    u_int64_t getNbMalloc       () const { return _nbMalloc;          }
    u_int64_t getNbCalloc       () const { return _nbCalloc;          }
    u_int64_t getNbRealloc      () const { return _nbRealloc;         }
    u_int64_t getNbFree         () const { return _nbFree;            }
    u_int64_t getTotalAllocated () const { return _totalAllocated;    }

private:

    MemoryAllocator ()  { resetStats();  }

    u_int64_t _nbMalloc;
    u_int64_t _nbCalloc;
    u_int64_t _nbRealloc;
    u_int64_t _nbFree;

    u_int64_t _totalAllocated;
};

/********************************************************************************/

class MemoryUsage : public IMemoryUsage
{
public:
    static MemoryUsage& singleton ()  { static  MemoryUsage instance; return instance; }

    u_int32_t getMemUsage ();

    void reset ()
    {
        _init = 0;
        _init = getMemUsage ();
    }

private:

    MemoryUsage () : _init(0), _page(0)  {  reset ();  }
    virtual ~MemoryUsage () {}

    u_int32_t _init;

    u_int32_t _page;
    u_int32_t getPageSize();
};

/********************************************************************************/

typedef u_int32_t Size;

/** std::vector-like class.
 *  The idea is to directly provide the data buffer instead of accessing data through
 *  the refine [] operator like in std::vector. This simple fact may speed up intensive
 *  memory access.
 *
 *  WARNING !!! Use only with basic types and not with class objects.
 */
template<typename T> class Vector
{
public:

    /** */
    Vector () : size(0), data(0), _reference(false)  {}

    /** */
    Vector (Size aSize) : size(aSize), data(0), _reference(false)
    {
        if (size > 0) {  data = (T*) MemoryAllocator::singleton().calloc (size, sizeof(T)); }
    }

    /** */
    Vector (const Vector<T>& v) : size(0), data(0), _reference(false)
    {
        if (v._reference == true)
        {
            setReference (v.size, v.data);
        }
        else
        {
            resize (v.size);
            memcpy (data, v.data, size);
        }
    }

    /** */
    ~Vector ()
    {
        if (_reference==false)  {   MemoryAllocator::singleton().free (data);  }
    }

    /** */
    T* resize (Size aSize)
    {
        if (_reference==false)
        {
            size = aSize;
            data = (T*) MemoryAllocator::singleton().realloc (data, size*sizeof(T));
            return data;
        }
        else
        {
            /** We used a reference, just forget this fact. */
            _reference = false;

            size = aSize;
            data = (T*) MemoryAllocator::singleton().calloc (size, sizeof(T));
            return data;
        }
    }

    /** */
    Vector<T>& operator= (const Vector<T>& v)
    {
        if (this != &v)
        {
            if( v._reference == true)
            {
                setReference (v.size, v.data);
            }
            else
            {
                resize (v.size);
                memcpy (data, v.data, size);
            }
        }

        return *this;
    }

    bool operator== (const Vector<T>& v) const
    {
        if (size != v.size)  { return false; }

        for (size_t i=0; i<size; i++)
        {
            if (data[i] != v.data[i])  { return false; }
        }
        return true;
    }

    /** We points to some external information. */
    void setReference (size_t aSize, T* reference)
    {
        /** We may get rid of previous data. */
        if (_reference==false)  {   MemoryAllocator::singleton().free (data);  }

        /** We change the information. */
        size = aSize;
        data = reference;

        /** We keep in mind that we refer a buffer that doesn't belong to us. */
        _reference = true;
    }

    std::list<Vector<T> > split (u_int32_t sizeMax)
    {
        std::list<Vector<T> > result;

        /** A little check. */
        if (sizeMax==0)  { sizeMax = this->size;  }

        size_t nmax = this->size / sizeMax;

        size_t total = 0;
        for (size_t n=0;  n < nmax; n++)
        {
            Vector<T>  item;
            item.setReference (sizeMax, this->data + total);
            result.push_back (item);

            total += sizeMax;
        }

        /** The remainder. */
        if (total < this->size)
        {
            Vector<T>  item;
            item.setReference (this->size - total, this->data + total);
            result.push_back (item);
        }

        return result;
    }

    /** */
    Size size;
    T*   data;

private:
    bool _reference;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _MEMORY_ALLOCATOR_HPP_ */
