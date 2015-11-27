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

/** \file BlastdbFileIndexReader.hpp
 *  \brief Quick reader for index file of the Blast db format
 *  \date 19/03/2014
 *  \author sbrillet
 *
 *  Define a reader for Blastdb index file format.
 */

#ifndef _BLASTDB_FILE_INDEX_READER_HPP_
#define _BLASTDB_FILE_INDEX_READER_HPP_

/********************************************************************************/

#include <database/api/IDatabaseQuickReader.hpp>
#include <os/api/IFile.hpp>
#include <os/api/IMemoryFile.hpp>
#include <misc/api/macros.hpp>
#include <misc/api/types.hpp>
#include <vector>
#include <string>

/********************************************************************************/
namespace database {
/** \brief Implementation of concepts related to genomic databases. */
namespace impl {
/********************************************************************************/

/** \brief Implementation of an interface to read the file index for Blast db format.
  */
class BlastdbFileIndexReader
{
public:

    /** Constructor.
     * \param[in] filename : filename of the blast index file (without extension)
     */
	BlastdbFileIndexReader (const char* filename);

    /** Destructor. */
    virtual ~BlastdbFileIndexReader  ();

    /** read function for the index file.
     */
    void read ();

    /** Returns the filename of the sequence file.
     * \return string name with the path and extension.
     */
    std::string    getSequenceFilename    ()
    {
    	if (_dbKind == IDatabaseQuickReader::ENUM_AMINO_ACID)
    		return (_filenameWithoutExt + ".psq");
    	else
    		return (_filenameWithoutExt + ".nsq");
    };

    /** Returns the filename of the header file.
     * \return string name with the path and extension.
     */
    std::string    getHeaderFilename    ()
    {
    	if (_dbKind == IDatabaseQuickReader::ENUM_AMINO_ACID)
    		return (_filenameWithoutExt + ".phr");
    	else
    		return (_filenameWithoutExt + ".nhr");
    };

    /** Returns the kind of database (ADN or protein).
     * \return the database kind.
     */
    IDatabaseQuickReader::DatabaseKind_e getKind () { return _dbKind; };

    /** Returns the maximum sequence size.
     * \return the maximum sequence size.
     */
    u_int64_t    getMaxSeqSize    () { return _maxSeqSize; };

    /** Returns the data read size (including only sequences data).
     * \return the data size.
     */
    u_int64_t    getDataSize    () { return _dataSize; };

    /** Returns the sequence file size
     * \return the sequence file size
     */
    u_int64_t    getSeqFileSize    () { return _sequenceFilesize; };

    /** Returns the number of read sequences.
     * \return the number of read sequences.
     */
    u_int32_t    getNbSequences () { return _nbSequences; };

    /** Returns the header offset in the header file.
     * \param[in] offsetIndex : sequence index in the index file
     * \return the header offsets in the header file.
     */
    u_int32_t getOffsetsHeader (u_int32_t offsetIndex)
    {
        u_int32_t index = _startIndexOffsetTable + _startSequenceIndex+ offsetIndex*4;

   		return CHAR_TO_INT32(_data[index],_data[index+1],_data[index+2],_data[index+3]);
    };

    /** Returns the sequence offset in the sequence file.
     * Useful for splitting a database into sequences subsets for dealing with huge genomic databases.
     * \param[in] offsetIndex : sequence index in the index file
     * \return the sequence offsets.
     */
    u_int32_t getOffsetsSequence (u_int32_t offsetIndex)
    {
        u_int32_t index = _startIndexOffsetTable + _startSequenceIndex + ((_nbSequences+1)*4)+offsetIndex*4;

   		return CHAR_TO_INT32(_data[index],_data[index+1],_data[index+2],_data[index+3]);
    };

    /** Returns the ambiguity offset in the sequence file.
     * This offset table is only used for the DNA database, if the sequence
     * hasn't any ambiguity residues, then the offset points to the beginning
     * of the next sequence
     * \param[in] offsetIndex : sequence index in the index file
     * \return the offset in the sequence file.
     */
    u_int32_t getOffsetsAmbiguity (u_int32_t offsetIndex)
    {
        u_int32_t index = _startIndexOffsetTable+_startSequenceIndex+2*((_nbSequences+1)*4)+ offsetIndex*4;
		if (index>_fileindex->getSize())
			return 0;
		else
			return CHAR_TO_INT32(_data[index],_data[index+1],_data[index+2],_data[index+3]);
    };

    /** Search the beginning offset in the index file depending of the offsetstart
     * \param[in] offsetStart : offset in the sequence file
     */
    void setOffsetsStart (u_int32_t offsetStart) {
    	u_int32_t index=0;
    	/** Read firstly the sequence offset list */
    	for (u_int32_t i=0;i<=_nbSequences;i++)
    	{
    		if (getOffsetsSequence(i)>=offsetStart)
    			break;
    		index+=4;
    	}
    	_startSequenceIndex = index;
    };

private:

    /** URI of the file to be iterated. */
    std::string _filename;

    /** file name without extension. */
    std::string	_filenameWithoutExt;

    /** files to be read. */
    os::IMemoryFile* _fileindex;

    /** Database version. */
    u_int32_t   _version;

    /** Kind of the database. */
    IDatabaseQuickReader::DatabaseKind_e _dbKind;

    /** Title of the database. */
    std::string   _title;

    /** Timestamp of the database. */
    std::string   _timestamp;

    /** Total size of the data (sequences data). */
    u_int64_t   _dataSize;

    /** Number of sequences. */
    u_int32_t   _nbSequences;

    /** Length of the longuest sequence in the database. */
    u_int32_t   _maxSeqSize;

    /** Offset in the index file to read the offset tables. */
    u_int64_t   _startIndexOffsetTable;

    /** Size of the sequence file. */
    u_int64_t   _sequenceFilesize;

    /** Start index in the sequence offset table, it is used to get directly
     * by the header and ambiguity offset table (only the range selected).
     * it is an optimization to read only the interesting data */
    u_int32_t _startSequenceIndex;

    /** local pointer to read the data, it is usefull to keep the address locally and skip the polymorphism cost. */
    const char* _data;

};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _BLASTDB_FILE_INDEX_READER_HPP_  */
