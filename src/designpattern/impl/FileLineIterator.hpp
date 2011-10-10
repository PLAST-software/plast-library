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
 *   Implementation of an Iterator that loops over the line of a file.
 *****************************************************************************/

#ifndef _FILE_ITERATOR_HPP_
#define _FILE_ITERATOR_HPP_

#include "Iterator.hpp"
#include <string>
#include <stdio.h>
#include <string.h>

/********************************************************************************/
namespace dp {
/********************************************************************************/

/** Iterator that loops over the line of a file.
 *  One must give the name of the file to be iterated and the maximum size of a read line.
 *
 *  Some inlined methods for optimization.
 *
 */
class FileLineIterator : public Iterator<char*>
{
public:

    FileLineIterator (const char* filename, size_t lineMaxSize, u_int64_t offset0=0, u_int64_t offset1=0);

    virtual ~FileLineIterator ();

    /** */
    void first();

    /** */
    dp::IteratorStatus next()
    {
        if (_file)
        {
            if (fgets (_line, _lineMaxSize, _file) == NULL)
            {
                _eof = true;
            }
            else
            {
                _readCurrentSize = strlen (_line);
                _readTotalSize  += _readCurrentSize;
                _eof = (_readTotalSize > _range);

                // don't take the ending '\n'
                _line[--_readCurrentSize] = 0;
            }
        }
        return ITER_UNKNOWN;
    }

    /** */
    bool isDone()          { return _eof;  }

    /** */
    char* currentItem()  {  return _line;  }

    /** */
    u_int64_t getCurrentReadSize ()  { return _readCurrentSize; }

private:
    std::string _filename;
    size_t      _lineMaxSize;

    FILE* _file;
    char* _line;

    u_int64_t _offset0;
    u_int64_t _offset1;
    u_int64_t _range;

    u_int64_t _readTotalSize;
    u_int64_t _readCurrentSize;

    bool _eof;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _FILE_ITERATOR_HPP_ */
