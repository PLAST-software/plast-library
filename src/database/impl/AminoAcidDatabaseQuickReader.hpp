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

/** \file AminoAcidDatabaseQuickReader.hpp
 *  \brief IDatabaseQuickReader implementation for amino acid database.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _AMINO_ACID_DATABASE_QUICK_READER_HPP_
#define _AMINO_ACID_DATABASE_QUICK_READER_HPP_

/********************************************************************************/

#include <database/api/IDatabaseQuickReader.hpp>

/********************************************************************************/
namespace database {
/** \brief Implementation of concepts related to genomic databases. */
namespace impl {
/********************************************************************************/

/** \brief Implementation of IDatabaseQuickReader for amino acid database.
 *
 *  This class takes a IDatabaseQuickReader instance as a reference (reference supposed
 *  to point to a nucleotid database). The different getters are then proxies to the
 *  reference. Most of the getters return the same information than the referenced instance
 *  but note for instance the 'getDataSize' method that divides by 3 the data size (which
 *  means that 3 nucleotids are needed to code an amino acid).
 *
 *  This class is a Proxy Design Pattern.
 */
class AminoAcidDatabaseQuickReader : public IDatabaseQuickReader
{
public:

    /** Constructor.
     * \param[in] ref : a IDatabaseQuickReader instance on a nucleotid database.
     */
    AminoAcidDatabaseQuickReader (IDatabaseQuickReader* ref)  : _ref(0)   {  setRef (ref);  }

    /** Destructor. */
    virtual ~AminoAcidDatabaseQuickReader  ()  {  setRef (0);  }

    /** \copydoc IDatabaseQuickReader::read */
    void read (u_int64_t  maxblocksize)  { _ref->read (maxblocksize);  }

    /** \copydoc IDatabaseQuickReader::getUri */
    std::string& getUri         () { return _ref->getUri();          }

    /** \copydoc IDatabaseQuickReader::getTotalSize */
    u_int64_t    getTotalSize   () { return _ref->getTotalSize();    }

    /** Returns the total read size (including only sequences data). This is the data size of the nucleotid
     * reference divided by 3.
     * \return the total size.
     */
    u_int64_t    getDataSize    () { return _ref->getDataSize() / 3; }

    /** \copydoc IDatabaseQuickReader::getNbSequences */
    u_int32_t    getNbSequences () { return _ref->getNbSequences();  }

    /** \copydoc IDatabaseQuickReader::getOffsets */
    std::vector<u_int64_t>& getOffsets ()  { return _ref->getOffsets(); }

    /** Returns the kind of database (ADN or protein).
     * \return always ENUM_AMINO_ACID.
     */
    DatabaseKind_e getKind ()  { return ENUM_AMINO_ACID; }

private:

    /** Refered nucleotid instance. */
    IDatabaseQuickReader* _ref;

    /** Smart setter for the _ref attribute. */
    void setRef (IDatabaseQuickReader* ref)  { SP_SETATTR(ref); }
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _AMINO_ACID_DATABASE_QUICK_READER_HPP_  */
