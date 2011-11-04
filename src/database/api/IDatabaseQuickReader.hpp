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
 *****************************************************************************/

#ifndef _IDATABASE_QUICK_READER_HPP_
#define _IDATABASE_QUICK_READER_HPP_

/********************************************************************************/

#include "SmartPointer.hpp"

#include "types.hpp"
#include <vector>
#include <string>

/********************************************************************************/
namespace database {
/********************************************************************************/

/** Define an interface that allows to read a database uri and gives a few information.
 *  Note that the intent is not to provide access to specific sequence but to provide
 *  global information on the database.
 */
class IDatabaseQuickReader : public dp::SmartPointer
{
public:

    enum DatabaseKind_e
    {
        ENUM_NUCLOTID,
        ENUM_AMINO_ACID,
        ENUM_UNKNOWN
    };

    /** */
    virtual void read (u_int64_t  maxblocksize = 0) = 0;

    virtual std::string& getUri         () = 0;
    virtual u_int64_t    getTotalSize   () = 0;
    virtual u_int64_t    getDataSize    () = 0;
    virtual u_int32_t    getNbSequences () = 0;

    virtual std::vector<u_int64_t>& getOffsets () = 0;

    virtual DatabaseKind_e getKind () = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _IDATABASE_QUICK_READER_HPP_  */
