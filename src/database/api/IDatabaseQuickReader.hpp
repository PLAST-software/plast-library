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

/** \file IDatabaseQuickReader.hpp
 *  \brief Quickly retrieval of databases information
 *  \date 07/11/2011
 *  \author edrezen
 *
 *  We define an interface for quickly read databases an extract some global
 *  information about it (like sequences number, data size, etc...).
 *
 *  This is useful for instance for computing statistical information on the
 *  query database.
 */

#ifndef _IDATABASE_QUICK_READER_HPP_
#define _IDATABASE_QUICK_READER_HPP_

/********************************************************************************/

#include <designpattern/api/SmartPointer.hpp>
#include <designpattern/api/IProperty.hpp>
#include <designpattern/impl/TokenizerIterator.hpp>
#include <os/impl/CommonOsImpl.hpp>
#include <misc/api/types.hpp>
#include <vector>
#include <string>

/********************************************************************************/
/** \brief Definition of concepts related to genomic databases. */
namespace database {
/********************************************************************************/

/** \brief Extract information about genomic database.
 *
 *  Define an interface that allows to read a database uri and gives a few information.
 *  Note that the intent is not to provide access to specific sequence but to provide
 *  global information on the database.
 *
 *  Note: we can obtain a vector of offsets, each offset pointing in the file to the
 *  beginning of a sequence. A 'maxblocksize' paramater can be provided for giving a
 *  threshold for the difference between two successive offsets. For instance, if we
 *  provide 10*1024*1024 for this parameter, we will get a vector of offsets that
 *  split the database into sequences subsets of less than 10 MBytes.
 *
 *  We should at least write an implementation of this interface for the FASTA format.
 */
class IDatabaseQuickReader : public dp::SmartPointer
{
public:

    /** */
    enum DatabaseKind_e
    {
        ENUM_NUCLOTID,
        ENUM_AMINO_ACID,
        ENUM_UNKNOWN
    };

     /** Read the database. Must be called before using getters.
     *  A 'maxblocksize' parameter can be provided; if not null, it is used for splitting the database
     *  into small sequences sets of maximum size; as a result, one can retrieve a vector of offsets,
     *  each offset pointing to a set of sequences.
     * \param[in] maxblocksize : maximum size of a sequences block.
     */
    virtual void read (u_int64_t  maxblocksize = 0) = 0;

    /** Returns the URI of the database.
     * \return the uri.
     */
    virtual std::string& getUri         () = 0;

    /** Returns the total read size (including both sequences comments and sequences data.
     * \return the total size.
     */
    virtual u_int64_t    getTotalSize   () = 0;

    /** Returns the total read size (including only sequences data).
     * \return the total size.
     */
    virtual u_int64_t    getDataSize    () = 0;

    /** Returns the number of read sequences.
     * \return the number of read sequences.
     */
    virtual u_int32_t    getNbSequences () = 0;

    /** Returns a vector of offsets, each offset pointing to the beginning of a sequence into the file.
     * Useful for splitting a database into sequences subsets for dealing with huge genomic databases.
     * \return the array of offsets.
     */
    virtual std::vector<u_int64_t>& getOffsets () = 0;

    /** Returns the kind of database (ADN or protein).
     * \return the database kind.
     */
    virtual DatabaseKind_e getKind () = 0;

    /** Max block size of a bank. */
    virtual u_int64_t  getMaxBlockSize() = 0;

    /** */
    virtual dp::IProperties* getProperties () = 0;

    /** */
    virtual int load (const std::string& uri) = 0;

    /** */
    virtual int save (const std::string& uri) = 0;

};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _IDATABASE_QUICK_READER_HPP_  */
