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

/** \file IFile.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Operating System abstraction of file management.
 */

#ifndef IFILE_HPP_
#define IFILE_HPP_

/********************************************************************************/

#include <os/api/IResource.hpp>
#include <misc/api/types.hpp>

/********************************************************************************/
/** \brief Operating System abstraction layer */
 namespace os {
/********************************************************************************/

/** \brief Abstraction of what we need about file system
 *
 *  We define here a few methods we need for handling file system (there is not a lot
 *  of methods right now).
 */
class IFile : public IResource
{
public:

    /** Destructor. */
    virtual ~IFile () {}

    /** Tells whether or not we are at the end of a file.
     *  \return true if we are at the end of the file, false otherwise.
     */
    virtual bool isEOF () = 0;

    /** Locates the cursor into the file (similar to 'fseeko' functions)
     * \param[in] offset : bytes number we want to go, relatively from the 'whence' paramater
     * \param[in] whence : SEEK_SET, SEEK_END, or SEEK_CUR
     * \return 0 if successful, -1 otherwise
     */
    virtual int seeko (u_int64_t offset, int whence) = 0;

    /** Get the position in a file.
     * \return the current position
     */
    virtual u_int64_t tell () = 0;

    /** Reads a line in the file.
     *  \param[in] s : buffer where to put the read line.
     *  \param[in] size : maximum number of characters to be read
     *  \return the actual size of the read buffer (0 if nothing read)
     */
    virtual int gets (char* s, int size) = 0;

    /** Writes a buffer (0 terminated) into the file.
     *  \param[in] buffer : buffer to be dumped into the file.
     */
    virtual void print (const char* buffer) = 0;

    /** Writes a buffer (0 terminated) into the file with a new line.
     *  \param[in] buffer : buffer to be dumped into the file.
     */
    virtual void println (const char* buffer) = 0;

    /** Writes a buffer (0 terminated) into the file with a new line.
     *  \param[in] format : format of the data to be dumped into the file.
     *  \param[in] ...    : arguments (as an ellipsis) to b dumped
     */
    virtual void print (const char* format, ...) = 0;

    /** Flush the file.
     */
    virtual void flush () = 0;

    /** Get the size of a file.
     * \return the size of the file.
     */
    virtual u_int64_t getSize () = 0;
};

/********************************************************************************/

/** \brief factory that creates IFile instance.
 *
 *  Factory that creates IFile instances. Different implementations may rely on
 *  different operating systems.
 */
class IFileFactory : public IResource
{
public:

    /** Destructor. */
    virtual ~IFileFactory () {}

    /** Creates a new IFile instance.
     * \param[in] path : uri of the file to be created.
     * \param[in] mode : mode of the file (like fprintf)
     * \param[in] temporary : file is removed from filesystem if true
     * \return instance of IFile, 0 otherwise.
     */
    virtual IFile* newFile (const char* path, const char* mode, bool temporary=false) = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* IFILE_HPP_ */
