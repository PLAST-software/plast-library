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

/** \file IMemoryFile.hpp
 *  \date 28/03/2014
 *  \author sbrillet
 *  \brief Operating System abstraction of memory file management.
 */

#ifndef IMEMORYFILE_HPP_
#define IMEMORYFILE_HPP_

/********************************************************************************/

#include <os/api/IResource.hpp>
#include <misc/api/types.hpp>

/********************************************************************************/
/** \brief Operating System abstraction layer */
 namespace os {
/********************************************************************************/

/** \brief Abstraction of what we need about memory file system
 *
 *  We define here a few methods we need for handling the memory file which permit to
 *  win some tiem
 */
class IMemoryFile : public IResource
{
public:
    /** Destructor. */
    virtual ~IMemoryFile () {}

    /** Permits to map the file if we want to do that "manually"
     *  it is used to read the sequence file splitted by block of max-database-size
     *  For the  Windows point of view it isn't possible to map the complete file
     *  \param[in] start : start offset to map the file in the virtual memory process
     *  \param[in] length : length of the data to map
     */
    virtual void mapFile(off_t start, size_t length) = 0;

    /** unmap the file which was "manually" map with the mapFile function
     */
    virtual void unmapFile() = 0;

    /** Get a pointer on the data mapped in the virtual memory process page
     * \return a pointer on the data.
     */
   virtual const char* getData() = 0;

    /** Get the size of a file.
     * \return the size of the file.
     */
     virtual u_int64_t getSize() = 0;

    /** Get the file path.
     * \return file path
     */
    virtual std::string getPath () = 0;

};

/********************************************************************************/

/** \brief factory that creates IMemoryFile instance.
 *
 *  Factory that creates IMemoryFile instances. Different implementations may rely on
 *  different operating systems.
 */
class IMemoryFileFactory : public IResource
{
public:

    /** Destructor. */
    virtual ~IMemoryFileFactory () {}

    /** Creates a new IFile instance.
     * \param[in] map : this boolean permits to know if we want to map the file directly or used the mapFile function
     * \param[in] path : uri of the file to be created.
     * \return instance of IMemoryFile, 0 otherwise.
     */
    virtual IMemoryFile* newFile (const char* path, bool map=true) = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* IMEMORYFILE_HPP_ */
