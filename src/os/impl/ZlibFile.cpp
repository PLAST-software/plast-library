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

#include <os/impl/ZlibFile.hpp>
#include <os/impl/CommonOsImpl.hpp>

#include <iostream>
#include <sstream>

#include <zlib.h>

/********************************************************************************/
namespace os { namespace impl {
/********************************************************************************/

/** */
class ZlibFile : public IFile
{
public:

    ZlibFile (const char* path, const char* mode, bool temporary) : _path(path), _handle(0)
    {
        _handle = gzopen (path, mode);
//        printf ("path='%s'  handle=%p\n", path, _handle);
    }

    ~ZlibFile ()
    {
        if (_handle != 0)  {  gzclose (_handle); }
    }

    /** \copydoc IFile::isEOF */
    bool isEOF ()  { return gzeof (_handle); }

    /** \copydoc IFile::seeko */
    int seeko (u_int64_t offset, int whence)
    {
        int result = gzseek64 (_handle, offset, whence);
//        printf ("ZlibFile::seeko:  offset=%lld  whence=%d  result=%d\n", offset, whence, result);
        return  result;
    }

    /** \copydoc IFile::tell */
    u_int64_t tell ()   { return gztell64 (_handle);  }

    /** \copydoc IFile::gets */
    int gets (char* s, int size)
    {
        int result = 0;

        /** We read the current line, up to 'size' characters. */
        char* tmp = (_handle==0 ? 0 : gzgets (_handle, s, size));

        /** Note: it may happen that the line is longer than the 'size' parameter.
         * Since this function is intended to read a line, we have to skip characters until the end of the line. */
        if (tmp != 0)
        {
            result = strlen (tmp);

            /** we skip all characters until we reach the next '\n'. */
            if (result > 0)  {  for (char c = tmp[result-1];  c !='\n' &&  c!=EOF;  c = gzgetc (_handle))  {}  }
        }

        /** We return the result. */
        return result;
    }

    /** \copydoc IFile::print */
    void print (const char* buffer)  {  gzprintf (_handle, buffer);  }

    /** \copydoc IFile::println*/
    void println (const char* buffer)  { gzprintf (_handle, "%s\n", buffer); }

    /** \copydoc IFile::printf */
    void print (const char* format, ...)  { gzprintf (_handle, format);  }

    /** \copydoc IFile::printf */
    void flush ()  { gzflush (_handle, Z_FINISH); }

    /** \copydoc IFile::getSize */
    u_int64_t getSize ()
    {
printf ("---------------> getSize \n");
        u_int64_t result = 0;
        if (_handle)
        {
            seeko (0L, SEEK_END);
            return tell ();
        }
        return result;
    }

private:

    std::string _path;
    gzFile      _handle;
};

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IFile* ZlibFileFactory::newFile (const char *path, const char *mode, bool temporary)
{
    return new ZlibFile (path, mode, temporary);
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /*  __LINUX__  */
