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

#ifndef _FAST_DATABASE_QUICK_READER_HPP_
#define _FAST_DATABASE_QUICK_READER_HPP_

/********************************************************************************/

#include "IDatabaseQuickReader.hpp"
#include "FileLineIterator.hpp"

/********************************************************************************/
namespace database {
/********************************************************************************/

/** Define an interface that allows to read a database uri and gives a few information.
 *  Note that the intent is not to provide access to specific sequence but to provide
 *  global information on the database.
 */
class FastaDatabaseQuickReader : public IDatabaseQuickReader
{
public:

    FastaDatabaseQuickReader (const std::string& uri);
    virtual ~FastaDatabaseQuickReader  ();

    /** */
    void read (u_int64_t  maxblocksize);

    std::string& getUri         () { return _uri;          }
    u_int64_t    getTotalSize   () { return _totalSize;    }
    u_int64_t    getDataSize    () { return _dataSize;     }
    u_int32_t    getNbSequences () { return _nbSequences;  }

    std::vector<u_int64_t>& getOffsets ()  { return _offsets; }

private:

    dp::FileLineIterator _iterator;

    std::string _uri;
    u_int64_t   _totalSize;
    u_int64_t   _dataSize;
    u_int32_t   _nbSequences;

    std::vector<u_int64_t> _offsets;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _FAST_DATABASE_QUICK_READER_HPP_  */
