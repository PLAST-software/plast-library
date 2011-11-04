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

#include "WindowsFile.hpp"
#include <stdio.h>

/********************************************************************************/
namespace os {
/********************************************************************************/

/** */
class WindowsFile : public IFile
{
public:

    WindowsFile (const char* path, const char* mode)
    {
        _handle = ::fopen (path, mode);
    }

    virtual ~WindowsFile ()
    {
        if (_handle)  { ::fclose (_handle); }
    }

    bool isEOF () { return feof (_handle); }

    int seeko (u_int64_t offset, int whence)  { return ::fseeko64 (_handle, offset, whence); }

    char* gets (char *s, int size)  { return fgets (s, size, _handle); }

protected:

    FILE* _handle;
};

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IFile* WindowsFileFactory::newFile (const char* path, const char* mode)
{
    return new WindowsFile (path, mode);
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /*  __LINUX__  */
