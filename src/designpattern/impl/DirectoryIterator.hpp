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

/** \file DirectoryIterator.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Implementation of an Iterator that iterates the files names of a given directory
 */

#ifndef _DIRECTORY_ITERATOR_HPP_
#define _DIRECTORY_ITERATOR_HPP_

#include <designpattern/api/Iterator.hpp>
#include <string>
#include <dirent.h>

/********************************************************************************/
namespace dp {
/** \brief Implementation of Design Pattern tools (Observer, SmartPointer, Command...) */
namespace impl {
/********************************************************************************/

/** \brief Iterator that loops over the files of a directory
 *
 *  This iterator loops over the filenames of a directory.
 *  One must give the name of the directory to be iterated.
 *
 *  Code sample:
 *  \code
 *  void sample ()
 *  {
 *      // We create a directory iterator.
 *      DirectoryIterator it ("/usr/lib");
 *
 *      // We loop each line of the file
 *      for (it.first(); !it.isDone(); it.next())
 *      {
 *          // We get the current filename
 *          const char* line = it.currentItem ();
 *      }
 *  }
 *  \endcode
 *
 *  \see Iterator
 */
class DirectoryIterator : public Iterator<const char*>
{
public:

    /** Constructor.
     * \param[in]  dirname : directory name
     * \param[in]  match   : string to be contained in the filename
     */
    DirectoryIterator (const char* dirname, const char* match=0);

    /** Destructor. */
    virtual ~DirectoryIterator ();

    /** \copydoc Iterator<const char*>::first */
    void first();

    /** \copydoc Iterator<const char*>::next */
    dp::IteratorStatus next();

    /** \copydoc Iterator<const char*>::isDone */
    bool isDone();

    /** \copydoc Iterator<const char*>::currentItem */
    const char* currentItem();

private:

    std::string _dirname;
    std::string _match;

    DIR*           _dp;
    struct dirent* _dirp;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _DIRECTORY_ITERATOR_HPP_ */
