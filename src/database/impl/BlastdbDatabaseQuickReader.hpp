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

/** \file BlastdbDatabaseQuickReader.hpp
 *  \brief Quick reader for Blast db format
 *  \date 13/03/2014
 *  \author sbrillet
 *
 *  Define a quick reader for Blastdb files.
 */

#ifndef _BLASTDB_DATABASE_QUICK_READER_HPP_
#define _BLASTDB_DATABASE_QUICK_READER_HPP_

/********************************************************************************/

#include <database/api/IDatabaseQuickReader.hpp>
#include <database/impl/BlastdbFileIndexReader.hpp>
#include <os/impl/DefaultOsFactory.hpp>


/********************************************************************************/
namespace database {
/** \brief Implementation of concepts related to genomic databases. */
namespace impl {
/********************************************************************************/

/** \brief Implementation of IDatabaseQuickReader interface for Blast db format.
 *
 *  Implementation of IDatabaseQuickReader interface for Blast db format. Should be
 *  as fast as possible.
 *
 *  Note that we read the index file with the NCBI BLAST format, this file permits to set the
 *  sequence number and the total size
 *
 *  The following sample shows how to read a Blast db database with a quick reader by blocks
 *  of 10 MBytes, then creating Blast db iterators for each found ranges of blocks (about)
 *  10 MBytes.
 *
 *  \code
 *  void foo ()
 *  {
 *      // We create the database reader
 *      IDatabaseQuickReader* reader = new BlastdbDatabaseQuickReader ("myDb", true, false);
 *
 *      // We read the database by blocks of 10 MBytes
 *      reader.read (10*1024*1024);
 *
 *      vector<u_int64_t>& offsets = reader.getOffsets();
 *
 *      // Now, we loop over all found offsets ranges
 *      for (size_t i=0; i<offsets.size()-1; i++)
 *      {
 *          // We create a BLAST db iterator for the current range.
 *          BlastdbSequenceIterator itSeq ("myDb", 64*1024, offsets[i], offsets[i+1]);
 *          for (itSeq.first(); !itSeq.isDone(); itSeq.next())
 *          {
 *              // We can retrieve the current sequence
 *              const ISequence* seq = itSeq.currentItem();
 *          }
 *      }
 *  }
 *  \endcode
 */
class BlastdbDatabaseQuickReader : public IDatabaseQuickReader
{
public:

    /** Constructor.
     * \param[in] uri : uri of the Blast file to be read.
     * \param[in] shouldInferType : tells whether we should try to find the kind of genomic database we read.
     * \param[in]  getOnlyType : if true, parse only a few residues in order to get the type; if false, parse the whole database.
     */
	BlastdbDatabaseQuickReader (const std::string& uri, bool shouldInferType = false, bool getOnlyType=false);

    /** Destructor. */
    virtual ~BlastdbDatabaseQuickReader  ();

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
    DatabaseKind_e getKind ()  { return _dbKind; }

    /** Max block size of a bank. */
    u_int64_t  getMaxBlockSize()  { return _maxblocksize; }

    /** */
    dp::IProperties* getProperties () { return NULL; }

    /** \copydoc IDatabaseQuickReader::load */
    int load (const std::string& uri) { return -1; }

    /** \copydoc IDatabaseQuickReader::save */
    int save (const std::string& uri) { return -1; }

private:

    std::string transformUriForAlias (const std::string& uri);

    /** Uri of the file to be read. */
    std::string _uri;

    /** List of files to be read. */
    std::list<BlastdbFileIndexReader*> _files;

    /** Total size of the data (comments + sequences data). */
    u_int64_t   _totalSize;

    /** Total size of the data (sequences data). */
    u_int64_t   _dataSize;

    /** Number of sequences. */
    u_int32_t   _nbSequences;

    /** Offsets of beginning of sequences in the file. */
    std::vector<u_int64_t> _offsets;

    /** Kind of the database. */
    DatabaseKind_e _dbKind;

    /** Max block size of a bank. */
    u_int64_t  _maxblocksize;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _BLASTDB_DATABASE_QUICK_READER_HPP_  */
