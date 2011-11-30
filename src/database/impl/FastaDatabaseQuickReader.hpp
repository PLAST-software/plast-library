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

/** \file FastaDatabaseQuickReader.hpp
 *  \brief Quick reader for FASTA format
 *  \date 07/11/2011
 *  \author edrezen
 *
 *  Define a quick reader for FASTA files.
 */

#ifndef _FAST_DATABASE_QUICK_READER_HPP_
#define _FAST_DATABASE_QUICK_READER_HPP_

/********************************************************************************/

#include <database/api/IDatabaseQuickReader.hpp>
#include <designpattern/impl/FileLineIterator.hpp>

/********************************************************************************/
namespace database {
/** \brief Implementation of concepts related to genomic databases. */
namespace impl {
/********************************************************************************/

/** \brief Implementation of IDatabaseQuickReader interface for FASTA format.
 *
 *  Implementation of IDatabaseQuickReader interface for FASTA format. Should be
 *  as fast as possible.
 *
 *  Note that we can try to find what kind of genomic database it is (nucleotids or
 *  amino acids). This may be useful for PLAST users because it allows not to specify
 *  the kind of algorithm (protein/protein, ADN/protein...) only by analyzing subject
 *  and query databases contents.
 */
class FastaDatabaseQuickReader : public IDatabaseQuickReader
{
public:

    /** Constructor.
     * \param[in] uri : uri of the FASTA file to be read.
     * \param[in] shouldInferType : tells whether we should try to find the kind of genomic database we read. */
    FastaDatabaseQuickReader (const std::string& uri, bool shouldInferType);

    /** Destructor. */
    virtual ~FastaDatabaseQuickReader  ();

    /** \copydoc IDatabaseQuickReader::read
     * Must be called before using getters.
     */
    void read (u_int64_t  maxblocksize=0);

    /** \copydoc IDatabaseQuickReader::getUri */
    std::string& getUri         () { return _uri;          }

    /** \copydoc IDatabaseQuickReader::getTotalSize */
    u_int64_t    getTotalSize   () { return _totalSize;    }

    /** \copydoc IDatabaseQuickReader::getDataSize */
    u_int64_t    getDataSize    () { return _dataSize;     }

    /** \copydoc IDatabaseQuickReader::getNbSequences */
    u_int32_t    getNbSequences () { return _nbSequences;  }

    /** \copydoc IDatabaseQuickReader::getOffsets */
    std::vector<u_int64_t>& getOffsets ()  { return _offsets; }

    /** \copydoc IDatabaseQuickReader::getKind */
    DatabaseKind_e getKind ();

private:

    /** File iterator for reading the file line by line. */
    dp::impl::FileLineIterator _iterator;

    /** Uri of the file to be read. */
    std::string _uri;

    /** Total size of the data (comments + sequences data). */
    u_int64_t   _totalSize;

    /** Total size of the data (sequences data). */
    u_int64_t   _dataSize;

    /** Number of sequences. */
    u_int32_t   _nbSequences;

    /** Offsets of beginning of sequences in the file. */
    std::vector<u_int64_t> _offsets;

    /** Threshold for infering database kind. */
    int32_t _readThreshold;

    /** Number of nucleotids. */
    u_int32_t _nbNucleotids;

    /** Number of amino acids. */
    u_int32_t _nbAminoAcids;

    /** Kind of the database. */
    DatabaseKind_e _dbKind;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _FAST_DATABASE_QUICK_READER_HPP_  */
