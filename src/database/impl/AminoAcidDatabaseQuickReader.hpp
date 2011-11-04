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

#ifndef _AMINO_ACID_DATABASE_QUICK_READER_HPP_
#define _AMINO_ACID_DATABASE_QUICK_READER_HPP_

/********************************************************************************/

#include "IDatabaseQuickReader.hpp"

/********************************************************************************/
namespace database {
/********************************************************************************/

class AminoAcidDatabaseQuickReader : public IDatabaseQuickReader
{
public:

    AminoAcidDatabaseQuickReader (IDatabaseQuickReader* ref)  : _ref(0)   {  setRef (ref);  }

    virtual ~AminoAcidDatabaseQuickReader  ()  {  setRef (0);  }

    /** */
    void read (u_int64_t  maxblocksize)  { _ref->read (maxblocksize);  }

    std::string& getUri         () { return _ref->getUri();          }
    u_int64_t    getTotalSize   () { return _ref->getTotalSize();    }
    u_int64_t    getDataSize    () { return _ref->getDataSize() / 3; }
    u_int32_t    getNbSequences () { return _ref->getNbSequences();  }

    std::vector<u_int64_t>& getOffsets ()  { return _ref->getOffsets(); }

    DatabaseKind_e getKind ()  { return ENUM_AMINO_ACID; }

private:

    IDatabaseQuickReader* _ref;
    void setRef (IDatabaseQuickReader* ref)  { SP_SETATTR(ref); }
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _AMINO_ACID_DATABASE_QUICK_READER_HPP_  */
