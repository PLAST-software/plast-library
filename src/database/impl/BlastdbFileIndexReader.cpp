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

#include <database/impl/BlastdbFileIndexReader.hpp>
#include <database/impl/DatabaseUtility.hpp>
#include <misc/api/PlastStrings.hpp>
#include <os/impl/CommonOsImpl.hpp>
#include <os/impl/DefaultOsFactory.hpp>
#include <misc/api/macros.hpp>


#define DEBUG(a)  //printf a

using namespace std;
using namespace dp;
using namespace os::impl;
using namespace database::impl;

/********************************************************************************/
namespace database { namespace impl {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BlastdbFileIndexReader::BlastdbFileIndexReader (const char* filename)
    :  _filename(filename),_version(0),
       _dbKind (IDatabaseQuickReader::ENUM_UNKNOWN),
       _title(""),_timestamp(""),
       _dataSize(0), _nbSequences(0), _maxSeqSize(0),_startIndexOffsetTable(0),
       _sequenceFilesize(0),_startSequenceIndex(0), _data(NULL)
{
	u_int32_t foundPoint;

	foundPoint = std::string(filename).find_last_of(".");
	_filenameWithoutExt = std::string(filename).substr(0,foundPoint);

	/** Open the File index depending of the extension nin = Nucleotides, pin = Protein*/
	DatabaseLookupType::QuickReaderType_e databaseType;
    databaseType = DatabaseLookupType::quickReaderType(filename);
	_fileindex = DefaultFactory::fileMem().newFile (filename);

	if ((databaseType == DatabaseLookupType::ENUM_BLAST_PIN)||(databaseType == DatabaseLookupType::ENUM_BLAST_PAL))
		_dbKind = IDatabaseQuickReader::ENUM_AMINO_ACID;
	else if ((databaseType == DatabaseLookupType::ENUM_BLAST_NIN)||(databaseType == DatabaseLookupType::ENUM_BLAST_NAL))
		_dbKind = IDatabaseQuickReader::ENUM_NUCLOTID;
	else
		_dbKind = IDatabaseQuickReader::ENUM_UNKNOWN;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BlastdbFileIndexReader::~BlastdbFileIndexReader  ()
{
	delete (_fileindex);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BlastdbFileIndexReader::read ()
{
    u_int32_t templength=0;
    u_int32_t index=0;


	_data = _fileindex->getData();

	_dataSize     = 0;
    _nbSequences  = 0;

    /** We read the 3 first 32 bytes in order to get :
	 * 	Version 		: Int32
	 * 	Database Type 	: Int32
	 * 	Title Length  	: Int32. */
    index=3;
    _version = _data[index];
    index+=4;
	_dbKind = (_data[index]==0) ? IDatabaseQuickReader::ENUM_NUCLOTID : IDatabaseQuickReader::ENUM_AMINO_ACID;;
	index++;

	/** Read the title length */
	templength = CHAR_TO_INT32(_data[index],_data[index+1],_data[index+2],_data[index+3]);
	index+=4;

	/** Read the timestamp length */
	index+=templength;
	templength = CHAR_TO_INT32(_data[index],_data[index+1],_data[index+2],_data[index+3]);
	index+=4;

	/** We read the 4 next 32 bytes in order to get :
	 * 	Sequences number	: Int32
	 * 	Data size	 		: Int64
	 * 	Sequence max size	: Int32. */
 	index+=templength;
	_nbSequences = CHAR_TO_INT32(_data[index],_data[index+1],_data[index+2],_data[index+3]);
	index+=4;
	_dataSize = CHAR_TO_INT64(_data[index+7],_data[index+6],_data[index+5],_data[index+4],
			_data[index+3],_data[index+2],	_data[index+1],_data[index+0]);
	index+=8;
	_maxSeqSize = CHAR_TO_INT32(_data[index],_data[index+1],_data[index+2],_data[index+3]);
	index+=4;

	/** Save the index to start to read the offset table */
	_startIndexOffsetTable = index;

	/** Read the last offset of the offset sequence table to get the sequence file size */
	index+=2*((_nbSequences+1)*4)-4;
	_sequenceFilesize = CHAR_TO_INT32(_data[index],_data[index+1],_data[index+2],_data[index+3]);
}
/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

