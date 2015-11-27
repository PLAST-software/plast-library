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

/** \file DirectoryIterator.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Implementation of an Iterator that iterates the files names of a given directory
 */

#ifndef _DIRECTORY_ITERATOR_HPP_
#define _DIRECTORY_ITERATOR_HPP_

#include <designpattern/api/Iterator.hpp>
#include <string>
#include <list>

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
    DirectoryIterator (const char* dirname, const char* match=0, bool dirOnly=false, bool recursive=false);

    /** Destructor. */
    virtual ~DirectoryIterator ();

    /** \copydoc Iterator<const char*>::first */
    void first();

    /** \copydoc Iterator<const char*>::next */
    dp::IteratorStatus next()  { _entriesIterator++;  return ITER_UNKNOWN; }

    /** \copydoc Iterator<const char*>::isDone */
    bool isDone()  { return _entriesIterator == _entries.end(); }

    /** \copydoc Iterator<const char*>::currentItem */
    const char* currentItem()  { return (*_entriesIterator).c_str(); }

private:

    std::string _dirname;
    std::string _match;
    bool        _dirOnly;
    bool        _recursive;

    std::list<std::string>           _entries;
    std::list<std::string>::iterator _entriesIterator;

    /** */
    void buildList (
        const std::string& dirname,
        const std::string& match,
        bool recursive,
        std::list<std::string>& entries
    );
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _DIRECTORY_ITERATOR_HPP_ */
