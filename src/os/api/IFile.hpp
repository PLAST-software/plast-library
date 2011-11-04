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
 *   Operating System abstraction of file management.
 *****************************************************************************/

#ifndef IFILE_HPP_
#define IFILE_HPP_

/********************************************************************************/

#include "SmartPointer.hpp"
#include "types.hpp"

/********************************************************************************/
namespace os {
/********************************************************************************/

/** */
class IFile : public dp::SmartPointer
{
public:

    virtual bool isEOF () = 0;

    virtual int seeko (u_int64_t offset, int whence) = 0;

    virtual char* gets (char *s, int size) = 0;
};

/********************************************************************************/

/** Factory that creates IFile instances.
 */
class IFileFactory
{
public:
    virtual IFile* newFile (const char* path, const char* mode) = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* IFILE_HPP_ */
