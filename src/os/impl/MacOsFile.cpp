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

#ifdef __DARWIN__

#include <os/impl/MacOsFile.hpp>
#include <os/impl/CommonOsImpl.hpp>

/********************************************************************************/
namespace os { namespace impl {
/********************************************************************************/

/** */
class MacOsFile : public CommonFile
{
public:

    MacOsFile (const char* path, const char* mode, bool temporary) : CommonFile(path,mode,temporary)  {}

    int seeko (u_int64_t offset, int whence)
    {
        /* MacOs is supposed to handle 64 bits file system by default, so no need to use fseeko64  */
        return (_handle==0 ? -1 : fseeko (_handle, offset, whence));
    }

    u_int64_t tell ()  { return (_handle==0 ? 0 : ftello (_handle)); }
};

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IFile* MacOsFileFactory::newFile (const char* path, const char* mode, bool temporary)
{
    return new MacOsFile (path, mode, temporary);
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /*  __LINUX__  */
