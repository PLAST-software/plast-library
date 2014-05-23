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
#include <os/api/IThread.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

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
    CommonFile (const char* path, const char* mode, bool temporary) : _path(path), _handle(0), _isStdout(false), _temporary(temporary)
    {
        _isStdout = path && strcmp(path,"stdout")==0;
        _handle   = _isStdout ? stdout : fopen (path, mode);
    }

    /** Destructor. */
    virtual ~CommonFile ()  {  if (_handle && !_isStdout)  {  fclose (_handle);  }  }

    /** \copydoc IFile::isEOF */
    bool isEOF ()  {  return (_handle ? feof (_handle) : true); }

    /** \copydoc IFile::gets */
    int gets (char *s, int size)
    {
        int result = 0;

        /** We read the current line, up to 'size' characters. */
        char* tmp = (_handle==0 ? 0 : fgets (s, size, _handle));

        /** Note: it may happen that the line is longer than the 'size' parameter.
         * Since this function is intended to read a line, we have to skip characters until the end of the line. */
        if (tmp != 0)
        {
            result = strlen (tmp);

            /** we skip all characters until we reach the next '\n'. */
            if (result > 0)  {  for (char c = tmp[result-1];  c !='\n' &&  c!=EOF;  c = fgetc (_handle))  {}  }
        }

        /** We return the result. */
        return result;
    }

    /** \copydoc IFile::print */
    void print   (const char* buffer)  { if (_handle)  {  fprintf (_handle, "%s", buffer); } }

    /** \copydoc IFile::println */
    void println (const char* buffer)  { if (_handle)  {  fprintf (_handle, "%s\n", buffer); } }

    /** \copydoc IFile::print */
    void print (const char* format, ...)
    {
    	if (_handle)
    	{
    		  va_list args;
    		  va_start (args, format);
    		  vfprintf (_handle, format, args);
    		  va_end (args);
    	}
    }

    /** \copydoc IFile::flush */
    void flush ()  { if (_handle)  {  fflush (_handle); } }

    /** \copydoc IFile::read */
    size_t read (void * buffer, size_t size, size_t count)
    {
    	size_t readsize=0;
    	if (_handle)
    	{
    		readsize = fread (buffer,size,count,_handle);
    	}
    	return readsize;
    }

    /** \copydoc IFile::write */
    size_t write (void * buffer, size_t size, size_t count)
    {
    	size_t writesize=0;
    	if (_handle)
    	{
    		writesize = fwrite (buffer,size,count,_handle);
    	}
    	return writesize;
    }

    /** \copydoc IFile::getSize */
    u_int64_t getSize ()
    {
        u_int64_t result = 0;
        if (_handle)
        {
            seeko (0L, SEEK_END);
            return tell ();
        }
        return result;
    }

    /** \copydoc IFile::getPath */
    std::string getPath ()   { return _path; }

    static bool isFileExist (const std::string& filename)
    {
        bool result = false;

        FILE* file = fopen (filename.c_str(), "rb");
        if (file != 0)
        {
            fclose (file);
        	result=true;
        }
        return result;
    }

protected:
    std::string _path;
    FILE*       _handle;
    bool        _isStdout;
    bool        _temporary;
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

/** \brief Factory that creates IThread instances in current context (ie. "null" thread).
 *
 */
class SerialThreadFactory : public IThreadFactory
{
public:

    /** Singleton. */
    static SerialThreadFactory& singleton () { static SerialThreadFactory instance; return instance; }

    /** Destructor. */
    virtual ~SerialThreadFactory () {}

    /** \copydoc IThreadFactory::newThread */
    IThread* newThread (void* (*mainloop) (void*), void* data)
    {
        /** Just launch the mainloop. */
        if (mainloop != 0)  {  mainloop (data);  }
        return new SerialThread ();
    }

    /** \copydoc IThreadFactory::newSynchronizer */
    ISynchronizer* newSynchronizer (void)
    {
        return new SerialSynchronizer ();
    }

    /** \copydoc IThreadFactory::getNbCores */
    size_t getNbCores ()  { return 1; }

    /** \copydoc IThreadFactory::getHostName */
    std::string getHostName ()  { return "127.0.0.1"; }

private:

    class SerialThread : public IThread
    {
    public:
        void join () { /* do nothing */ }
    };

    class SerialSynchronizer : public ISynchronizer
    {
    public:
        void   lock () { /* do nothing */ }
        void unlock () { /* do nothing */ }
    };
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _COMMON_IMPL_HPP_ */
