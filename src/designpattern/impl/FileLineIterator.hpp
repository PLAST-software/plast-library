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

/** \file FileLineIterator.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Implementation of an Iterator that loops over the line of a file.
 */

#ifndef _FILE_ITERATOR_HPP_
#define _FILE_ITERATOR_HPP_

#include <designpattern/api/Iterator.hpp>
#include <os/impl/DefaultOsFactory.hpp>
#include <string.h>
#include <list>

/********************************************************************************/
namespace dp {
/** \brief Implementation of Design Pattern tools (Observer, SmartPointer, Command...) */
namespace impl {
/********************************************************************************/

/** \brief Iterator that loops over the line of a file.
 *
 *  This iterator loops over the line of a file.
 *  One must give the name of the file to be iterated and the maximum size of a read line.
 *
 *  It inherits from Iterator class by expliciting the type (char*) of the iterated items.
 *  Some inlined methods for optimization.
 *
 *  Code sample:
 *  \code
 *  void sample ()
 *  {
 *      // We create a file iterator. Maximum line size is 256 characters
 *      FileLineIterator it ("/tmp/afile.txt", 256);
 *
 *      // We loop each line of the file
 *      for (it.first(); !it.isDone(); it.next())
 *      {
 *          // We get the current read line
 *          char* line = it.currentItem ();
 *      }
 *  }
 *  \endcode
 *
 *  This is used for instance for parsing FASTA files.
 */
class FileLineIterator : public Iterator<char*>
{
public:

    /** Constructor. Note that one can provide a range [offset0,offset1] of characters to be read within the file.
     * \param[in]  filename     : file name of the file to be iterated
     * \param[in]  lineMaxSize  : number of characters to be read in a line
     * \param[in]  offset0      : if not 0, provides the first character index to be read in the file
     * \param[in]  offset1      : if not 0, provides the last character index to be read in the file
     */
    FileLineIterator (const char* filename, size_t lineMaxSize=64*1024, u_int64_t offset0=0, u_int64_t offset1=0);

    /** Destructor. */
    virtual ~FileLineIterator ();

    /** \copydoc Iterator<char*>::first */
    void first();

    /** \copydoc Iterator<char*>::next */
    dp::IteratorStatus next()
    {
        if (!_eof && _currentFile)
        {
            if (_currentFile->gets (_line, _lineMaxSize) == NULL)
            {
                /** We have to look for possible other files. */
                if (retrieveNextFile())
                {
                    /** The current file has been updated, we try again the 'next' method. */
                    return next ();
                }
                else
                {
                    _eof = true;
                }
            }
            else
            {
                /** We get the size of the read line. */
                _readCurrentSize = strlen (_line);
                _readTotalSize  += _readCurrentSize;
                _eof = (_readTotalSize > _range);

                /** We remove the unwanted ending characters. */
                while (_readCurrentSize > 0)
                {
                    char c = _line[_readCurrentSize-1];

                    if (c!=10 && c!=13)  { break; }

                    _line[--_readCurrentSize] = 0;
                }
            }
        }
        return ITER_UNKNOWN;
    }

    /** \copydoc Iterator<char*>::isDone */
    bool isDone()          { return _eof;  }

    /** \copydoc Iterator<char*>::currentItem */
    char* currentItem()  {  return _line;  }

    /** Returns the size of the currently read line.
     * \return current read line size
     */
    u_int64_t getCurrentReadSize ()  { return _readCurrentSize; }

    /** */
    void setRange (u_int64_t offset0, u_int64_t offset1)
    {
        _offset0 = offset0;
        _offset1 = offset1;
        _range   = _offset1 - _offset0 + 1;

        first ();
    }

private:

    /** URI of the file to be iterated. */
    std::string _filename;

    /** Maximum size of a line. */
    size_t      _lineMaxSize;

    /** Read line. */
    char*  _line;

    /** List of files to be read. */
    std::list<os::IFile*> _files;

    /** First character to be read in the file. */
    u_int64_t _offset0;

    /** Last character to be read in the file. */
    u_int64_t _offset1;

    /** Number of characters to be read in the file. */
    u_int64_t _range;

    /** Total number of read characters. */
    u_int64_t _readTotalSize;

    /** Size of the currently read line. */
    u_int64_t _readCurrentSize;

    /** Tells if the iteration is finished or not. */
    bool _eof;

    /** */
    os::IFile* _currentFile;
    os::IFile* getCurrentFile ()   { return _currentFile; }

    /** */
    u_int64_t _currentFileLength;

    /** */
    std::list<os::IFile*>::iterator _filesIterator;

    /** Returns false if eof. */
    bool retrieveNextFile ();
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _FILE_ITERATOR_HPP_ */
