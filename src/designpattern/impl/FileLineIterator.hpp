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

#define	SEQUENCE_MAX_COMMENT_SIZE	2*1024

/********************************************************************************/
namespace dp {
/** \brief Implementation of Design Pattern tools (Observer, SmartPointer, Command...) */
namespace impl {
/********************************************************************************/

/** \brief Iterator that loops over the lines of a file.
 *
 *  This iterator loops over the lines of a file.
 *  One must give the name of the file to be iterated and the maximum size of a read line.
 *
 * This is the object oriented version of the classical way of parsing lines in a file
 * with the 'fgets' function. Note that we use our Iterator interface.
 *
 *  It inherits from Iterator class by expliciting the type (char*) of the iterated items.
 *  Some inlined methods for optimization.
 *
 *  It is possible to provide a range of two offsets [begin,end] that will be used for
 *  reading only a part of the file (namely from offset 'begin' to offset 'end'). This possibility
 *  is hugely used by PLAST for partitioning huge FASTA databases in blocks of given size.
 *  A first pass of reading is done for finding offsets couples that split the database in similar
 *  given block size. Then the PLAST algorithm can read each block separately by providing the
 *  offsets range to a FileLineIterator.
 *
 *  Another feature is the possibility to provide, for the filename, a comma separated list of actual
 *  filenames. The FileLineIterator will parse all actual files. This is useful for instance to parse
 *  a set of FASTA files that come from the split of a huge database.
 *
 *  This is class is used in several parts of PLAST; in particular, it is used for parsing:
 *    - FASTA files
 *    - IProperties files
 *    - FastaDatabaseQuickReader class
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
 *  \see Iterator
 *  \see database::impl::FastaDatabaseQuickReader
 */
class FileLineIterator : public Iterator<char*>
{
public:

    /** Constructor. Note that one can provide a range [offset0,offset1] of characters to be read within the file.
     * \param[in]  filename     : file name of the file to be iterated; can be a list of filenames separated by a comma
     * \param[in]  lineMaxSize  : number of characters to be read in a line
     * \param[in]  offset0      : if not 0, provides the first character offset to be read in the file
     * \param[in]  offset1      : if not 0, provides the last character offset to be read in the file
     */
    FileLineIterator (const char* filename, size_t lineMaxSize=SEQUENCE_MAX_COMMENT_SIZE, u_int64_t offset0=0, u_int64_t offset1=0);

    /** Destructor. */
    virtual ~FileLineIterator ();

    /** \copydoc Iterator<char*>::first */
    void first();

    /** \copydoc Iterator<char*>::next */
    dp::IteratorStatus next()
    {
        if (!_eof && _currentFile)
        {
            /** We read the current line, up to '_lineMaxSize' characters. */
            _readCurrentSize = _currentFile->gets (_line, _lineMaxSize);

            if (_readCurrentSize == 0)
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
                _readTotalSize  = _currentFile->tell();
                _eof = ((_offset0 < _offset1)&&(_readTotalSize > (_offset1+1)));

                /** We remove the unwanted ending characters. */
                while (_readCurrentSize > 0)
                {
                    /** Shortcut to the last character of the string. */
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
     * Note that the end of line (code 0x10 and/or 0x13) character is not taken into account.
     * \return current read line size
     */
    u_int64_t getLineSize ()  { return _readCurrentSize; }

    /** Returns the current position in the file.
     * Warning ! when the file is a 'composite' file (ie. comma separated list of files), we must
     * return the aggregated length.
     * \return the position in the file
     */
    u_int64_t tell ()  { return _cummulatedFilesLength + _currentFile->tell(); }

    /** Reinitialize the offsets range to be parsed in the file.
     * \param[in] offset0 : begin offset
     * \param[in] offset1 : end offset
     */
    void setRange (u_int64_t offset0, u_int64_t offset1)
    {
        _offset0 = offset0;
        _offset1 = offset1;
        _range   = _offset1 - _offset0 + 1;

        first ();
    }

    /** Reinitialize the beginning offset from where the file will be parsed.
     * \param[in] offset : begin offset
     */
    void setRange (u_int64_t offset)
    {
        _offset0 = offset;
        _offset1 = offset;
        _range   = ~0;

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

    /** Size of all files already parsed. */
    u_int64_t _cummulatedFilesLength;

    /** Tells if the iteration is finished or not. */
    bool _eof;

    /** */
    os::IFile* _currentFile;
    os::IFile* getCurrentFile ()   { return _currentFile; }

    /** */
    std::list<os::IFile*>::iterator _filesIterator;

    /** Returns false if eof. */
    bool retrieveNextFile ();
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _FILE_ITERATOR_HPP_ */
