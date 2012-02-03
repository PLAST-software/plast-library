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

#include <os/impl/WindowsFile.hpp>
#include <os/impl/CommonOsImpl.hpp>

/********************************************************************************/
namespace os { namespace impl {
/********************************************************************************/

/** */
class WindowsFile : public CommonFile
{
public:

    WindowsFile (const char* path, const char* mode, bool temporary) : CommonFile(path,mode,temporary) {}

    int seeko (u_int64_t offset, int whence)  {  return (_handle==0 ? -1 : fseeko64 (_handle, offset, whence));  }

    u_int64_t tell ()  { return (_handle==0 ? 0 : ftello64 (_handle)); }
};

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IFile* WindowsFileFactory::newFile (const char* path, const char* mode, bool temporary)
{
    return new WindowsFile (path, mode, temporary);
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /*  __LINUX__  */
