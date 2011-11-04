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
 *   Linux abstraction of file management.
 *****************************************************************************/

#ifndef _LINUX_FILE_HPP_
#define _LINUX_FILE_HPP_

/********************************************************************************/

#include "IFile.hpp"

/********************************************************************************/
namespace os {
/********************************************************************************/

class LinuxFileFactory : public IFileFactory
{
public:

    static LinuxFileFactory& singleton() { static LinuxFileFactory instance;  return instance; }

    IFile* newFile (const char *path, const char *mode);
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _LINUX_FILE_HPP_ */
