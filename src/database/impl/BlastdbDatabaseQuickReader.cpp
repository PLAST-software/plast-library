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

#include <database/impl/BlastdbDatabaseQuickReader.hpp>
#include <database/impl/BlastdbFileIndexReader.hpp>
#include <database/impl/DatabaseUtility.hpp>
#include <designpattern/impl/TokenizerIterator.hpp>
#include <designpattern/impl/FileLineIterator.hpp>
#include <misc/api/PlastStrings.hpp>
#include <os/impl/CommonOsImpl.hpp>
#include <misc/api/macros.hpp>


#define DEBUG(a)  //printf a

using namespace std;
using namespace dp;
using namespace dp::impl;
using namespace os;
using namespace os::impl;


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
BlastdbDatabaseQuickReader::BlastdbDatabaseQuickReader (const std::string& uri, bool shouldInferType, bool getOnlyType)
    : _uri(uri),
      _totalSize(0), _dataSize(0), _nbSequences(0),
      _dbKind (ENUM_UNKNOWN),
      _maxblocksize(0)
{
    DEBUG (("BlastdbDatabaseQuickReader::BlastdbDatabaseQuickReader  _uri=%s _getOnlyType=%d  _readThreshold=%d \n",
    		_uri.c_str(), _getOnlyType, _readThreshold));

	DatabaseLookupType::QuickReaderType_e databaseType = DatabaseLookupType::ENUM_TYPE_UNKNOWN;
	databaseType = DatabaseLookupType::quickReaderType(_uri);
	if ((databaseType==DatabaseLookupType::ENUM_BLAST_PAL)||(databaseType==DatabaseLookupType::ENUM_BLAST_NAL))
		_uri = transformUriForAlias(uri);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BlastdbDatabaseQuickReader::~BlastdbDatabaseQuickReader  ()
{
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BlastdbDatabaseQuickReader::read (u_int64_t  maxblocksize)
{

    list<BlastdbFileIndexReader*>::iterator filesIterator;
    size_t oldIdx     = ~0;
    size_t currentIdx =  0;
    u_int64_t globalOffset = 0;
    u_int64_t offsetSize = 0;
    u_int64_t seqFileSize = 0;

    DEBUG (("BlastdbDatabaseQuickReader::read  maxblocksize=%ld  _getOnlyType=%d  _readThreshold=%d \n",
        maxblocksize, _getOnlyType, _readThreshold
    ));

    TokenizerIterator tokenizer (_uri.c_str(), ",");

    for (tokenizer.first (); !tokenizer.isDone(); tokenizer.next())
    {
    	_files.push_back (new BlastdbFileIndexReader(tokenizer.currentItem()));
    }

    /** We clear the provided arguments. */
    _totalSize    = 0;
    _dataSize     = 0;
    _nbSequences  = 0;
    _maxblocksize = maxblocksize;
    _offsets.clear ();

	if (_files.empty() == false)
	{

		/** We have to find the file that matches the provided beginning offset. */
		for (filesIterator = _files.begin(); filesIterator != _files.end();  filesIterator++)
		{
			(*filesIterator)->read();

			_dbKind = (*filesIterator)->getKind();
			_nbSequences += (*filesIterator)->getNbSequences();
			_dataSize += (*filesIterator)->getDataSize();

			/** The sequence file size is the last sequence offset table
			 * in the index file */
			seqFileSize = (*filesIterator)->getSeqFileSize();
			_totalSize += seqFileSize;

			/** Read the sequence data offset table if maxblocksize !=0 */
			if (maxblocksize > 0)
			{
				/** read all offset table to calculate the global offset */
				//(*filesIterator)->readSequenceOffsetTable(0, 0);
				offsetSize = (*filesIterator)->getNbSequences();
				/** loop on the sequence offset to calculate the different split */
				//for (u_int32_t i=0;i<(offsetSize-1);i++)
				for (u_int32_t i=0;i<offsetSize;i++)
				{
					/** calculate the global offset cumulated with on the different file*/
					globalOffset = (*filesIterator)->getOffsetsSequence(i) + (_totalSize - seqFileSize);

					/** calculate an index to check to know the modulo value of globaloffset for maxblocsize */
					currentIdx = globalOffset / maxblocksize;

					if (currentIdx != oldIdx)
					{
						_offsets.push_back (globalOffset);
						oldIdx = currentIdx;
					}
				}
			}
		 }
		_offsets.push_back (_totalSize);
	}
    for (list<BlastdbFileIndexReader*>::iterator it= _files.begin(); it != _files.end(); it++)
    {
        delete (*it);
    }

    DEBUG (("BlastdbDatabaseQuickReader::read : _dbKind=%d  _nbSequences=%d => _totalSize=%d\n",
    		_dbKind, _nbSequences, _totalSize
    ));
}


/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
std::string BlastdbDatabaseQuickReader::transformUriForAlias (const std::string& uri)
{
	string Extension;
	string buffer ("");
	string path ("");
	DatabaseLookupType::QuickReaderType_e databaseType = DatabaseLookupType::ENUM_TYPE_UNKNOWN;

	/** We parse the uri to find the different AL file separated by comma. */
	TokenizerIterator tokenizer (uri.c_str(), ",");
	for (tokenizer.first (); !tokenizer.isDone(); tokenizer.next())
	{
		databaseType = DatabaseLookupType::quickReaderType(tokenizer.currentItem());
		if (databaseType==DatabaseLookupType::ENUM_BLAST_PAL)
			Extension = ".pin";
		else if (databaseType==DatabaseLookupType::ENUM_BLAST_NAL)
			Extension = ".nin";
		else
			throw MSG_FILE_BLAST_MSG1;

		/** get the path of the file **/
		u_int32_t found = string(tokenizer.currentItem()).find_last_of("/\\");
		path = string(tokenizer.currentItem()).substr(0,found+1);

		/** open the al file and iterate by line to find the DBLIST**/
		FileLineIterator fileAl(string(tokenizer.currentItem()).c_str());
	    for (fileAl.first (); !fileAl.isDone(); fileAl.next())
	    {
	    	/** search the DBLIST string in the file to read the index list file **/
	    	if (strncmp(fileAl.currentItem(),"DBLIST",6)==0)
			{
	    		TokenizerIterator tokenizerSpace (&fileAl.currentItem()[6], " ");
				for (tokenizerSpace.first (); !tokenizerSpace.isDone(); tokenizerSpace.next())
				{
					string tempFilename = string(tokenizerSpace.currentItem());
					if (tempFilename.find("\"")!=std::string::npos)
						buffer += path + tempFilename.substr(1,(tempFilename.size()-2))+ Extension +",";
					else
						buffer += path + tempFilename+ Extension +",";
				}
			}
	    }
	}
	// create the quick reader and remove the last comma
	if (!buffer.empty())
		buffer.replace(buffer.end()-1,buffer.end(),"\0");
    return buffer;
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

