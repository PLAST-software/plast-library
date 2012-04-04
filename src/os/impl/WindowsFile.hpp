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

/** \file WindowsFile.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Windows abstraction of file management.
 */

#ifndef _WINDOWS_FILE_HPP_
#define _WINDOWS_FILE_HPP_

/********************************************************************************/

#include <os/api/IFile.hpp>

/********************************************************************************/
namespace os {
/** \brief Implementation of Operating System abstraction layer */
namespace impl {
/********************************************************************************/

/** \brief factory that creates IFile instance for Windows OS.
 *
 *  Factory that creates IFile instances.
 */
class WindowsFileFactory : public IFileFactory
{
public:

    /** Destructor. */
    virtual ~WindowsFileFactory () {}

   /** \copydoc IFileFactory::newFile */
    IFile* newFile (const char* path, const char* mode, bool temporary);
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _WINDOWS_FILE_HPP_ */
