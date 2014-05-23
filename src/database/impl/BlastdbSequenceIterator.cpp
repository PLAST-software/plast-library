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


#include <database/api/ISequence.hpp>
#include <database/impl/BlastdbSequenceIterator.hpp>
#include <misc/api/PlastStrings.hpp>
#include <database/impl/BasicSequenceBuilder.hpp>
#include <designpattern/impl/TokenizerIterator.hpp>
#include <os/impl/CommonOsImpl.hpp>

#include <misc/api/macros.hpp>
#include <os/impl/TimeTools.hpp>

#include <iostream>
#include <sstream>

#include <string.h>
#include <stdlib.h>

#define DEBUG(a)  //printf a
#define INFO(a)   printf a

using namespace std;
using namespace dp;
using namespace dp::impl;
using namespace os;
using namespace os::impl;

/********************************************************************************/
namespace database { namespace impl  {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BlastdbSequenceIterator::BlastdbSequenceIterator (
    const char* filename,
    size_t commentMaxSize,
    u_int64_t offset0,
    u_int64_t offset1
)
: _offset0(offset0), _offset1(offset1), _commentMaxSize(commentMaxSize),
  _readTotalSize(0), _readCurrentSize(0),_fileCurrentSize(0), _cummulatedFilesLength(0), _currentIndexFile(NULL),
  _currentSequenceFile(NULL),_offsetReadIndex(0), _data(NULL),_isDone(false),_eof(false),_firstOffset(0)

{
    DEBUG (("BlastdbSequenceIterator::BlastdbSequenceIterator:  filename='%s'  range=[%ld,%ld] \n", filename, offset0, offset1));

    _dbType = DatabaseLookupType::quickReaderType(filename);
	/** loop on the sequence file to open this file and use it as iterator **/
	TokenizerIterator tokenizer (filename, ",");
    for (tokenizer.first (); !tokenizer.isDone(); tokenizer.next())
    {
    	_filesIndex.push_back (new BlastdbFileIndexReader(tokenizer.currentItem()));
    }

	setBuilder (new BasicSequenceBuilder(SUBSEED));

	/** We set the id for the iterator. */
	stringstream ss;
	ss << filename << ":" << offset0 << ":" << offset1;
	setId (ss.str());
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BlastdbSequenceIterator::~BlastdbSequenceIterator ()
{
	DEBUG ("BlastdbSequenceIterator::~BlastdbSequenceIterator");
	setBuilder (0);
	/** We delete each IFile instance of the list of files. */
	for (list<BlastdbFileIndexReader*>::iterator it= _filesIndex.begin(); it != _filesIndex.end(); it++)
	{
		delete (*it);
	}

	if(_currentSequenceFile)
	{
		_currentSequenceFile->unmapFile();
		delete _currentSequenceFile;
	}
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BlastdbSequenceIterator::first()
{
	_isDone = false;

    DEBUG (("BlastdbSequenceIterator::first:  range=[%ld,%ld] \n", offset0, offset1));
    /* Read firstly the index table to get the index table
     * for the header and sequence file
     */
	if (_filesIndex.empty() == false)
	{
		/** We reset some attributes. */
	    _cummulatedFilesLength 	= 0;
		_readTotalSize         	= 0;
		_readCurrentSize       	= 0;
		_offsetReadIndex		= 0;
		_fileCurrentSize		= 0;

	    /** We have to find the file that matches the provided beginning offset. */
        for (_filesIterator = _filesIndex.begin(); _filesIterator != _filesIndex.end();  _filesIterator++)
        {
            /** We set the current file shortcut. */
        	_currentIndexFile = *_filesIterator;
        	if(_currentSequenceFile) { delete _currentSequenceFile; }
        	_currentSequenceFile = DefaultFactory::fileMem().newFile (_currentIndexFile->getSequenceFilename().c_str(), false);
        	_fileCurrentSize = _currentSequenceFile->getSize();

        	/*_currentIndexFile->read();
        	_fileCurrentSize = _currentIndexFile->getSeqFileSize();*/

            /** find if the _offset0 is in the _currentIndexFile */
            if ((_cummulatedFilesLength + _fileCurrentSize) >= _offset0)
            {
            	_readCurrentSize       	= _offset0 - _cummulatedFilesLength;

            	readIndexFileAndCreateHeaderFile(_readCurrentSize);

                /** We leave the loop over the files. */
                break;
            }

            /** We increase the cumulated sizes of files. */
            _cummulatedFilesLength += _fileCurrentSize;

        } /* for (_filesIterator = _files.begin()... */

        /** We can set the end of file attribute. */
        _eof = (_filesIterator == _filesIndex.end());

		/** We force retrieval of the first line. */
		next ();
	}
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool BlastdbSequenceIterator::retrieveNextFile ()
{
    /** We go to the next file. */
    _filesIterator++;

    if (_filesIterator != _filesIndex.end())
    {
        /** We add this size to the current aggregated files length. */
        _cummulatedFilesLength += _fileCurrentSize;

        /** We update the current file. */
    	_currentIndexFile = *_filesIterator;
    	if(_currentSequenceFile)
    	{
    		_currentSequenceFile->unmapFile();
    		delete _currentSequenceFile;
    	}
    	_currentSequenceFile = DefaultFactory::fileMem().newFile (_currentIndexFile->getSequenceFilename().c_str(),false);
    	_fileCurrentSize = _currentSequenceFile->getSize();

        readIndexFileAndCreateHeaderFile(0);

        _readCurrentSize 		= 0;
    }
    else
    {
    	_currentSequenceFile->unmapFile();
    	_eof = true;
    }

    /** we return the current eof status. */
    return !_eof;
}


/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
dp::IteratorStatus BlastdbSequenceIterator::next()
{
	 u_int32_t seqLength 	= 0;
	 u_int32_t offsetSeqBegin = 0;
	 u_int32_t offsetHdrBegin = 0;
	 u_int32_t offsetAmbBegin = 0;
	 u_int32_t offsetSeqEnd = 0;

 	 if (!_eof && _currentIndexFile)
     {
 		 /** We retrieve the builder => shortcut and optimization (avoid method call) */
		ISequenceBuilder* builder = getBuilder();
		if (builder)
		{
			/** compare the current size with the sequence file size
			 * to know if we need to change of file  */
			if (_readCurrentSize>=(_fileCurrentSize-1))
			{
				/** We have to look for possible other files. */
				if (retrieveNextFile())
				{
					/** The current file has been updated, we try again the 'next' method. */
					return next ();
				}
				else
				{
					_currentSequenceFile->unmapFile();
					_eof = true;
					_isDone = true;
				}
			}
			else
			{
				/** finish the read iteration if the readindex is bigger than the number of sequence offset */
				if (_offset1 !=0)
					_isDone = (_currentIndexFile->getOffsetsSequence(_offsetReadIndex) > (_offset1-_cummulatedFilesLength));


				if(!_isDone)
				{
					/** Read the first offset in the sequence and header table
					 *  */
					offsetHdrBegin = _currentIndexFile->getOffsetsHeader(_offsetReadIndex);
					offsetSeqBegin = _currentIndexFile->getOffsetsSequence(_offsetReadIndex);

					builder->setCommentUri(_currentIndexFile->getHeaderFilename().c_str(),offsetHdrBegin,_commentMaxSize);

					/** We reset the data size. */
					builder->resetData ();

					/** Add the buffer in the sequence Builder */
					/** check the file data type, if it is a DNA sequence, need to decode this sequence
					 * with the ambiguity table
					 */
					if (_dbType == DatabaseLookupType::ENUM_BLAST_PIN)
					{
						/** Read the second offset and calculate the data length
						*/
						_offsetReadIndex++;

						/** calculate the sequence end offset and the length of the data
						 *  sequence. if the offset start is bigger than offset end => error
						 */
						offsetSeqEnd = _currentIndexFile->getOffsetsSequence(_offsetReadIndex);
						if (offsetSeqEnd>offsetSeqBegin){	seqLength = offsetSeqEnd - offsetSeqBegin -1; }
						else {throw MSG_FILE_BLAST_MSG2; }

						builder->addData ((LETTER*)&_data[(offsetSeqBegin-_firstOffset)], seqLength, NCBI);
					}
					else
					{
						offsetAmbBegin = _currentIndexFile->getOffsetsAmbiguity(_offsetReadIndex);

						_offsetReadIndex++;

						/** calculate the sequence end offset and the length of the data
						 *  sequence. if the offset start is bigger than offset end => error
						 */
						offsetSeqEnd = _currentIndexFile->getOffsetsSequence(_offsetReadIndex);
						if (offsetSeqEnd>offsetSeqBegin){	seqLength = offsetSeqEnd - offsetSeqBegin -1; }
						else {throw MSG_FILE_BLAST_MSG2; }

						if ((offsetAmbBegin != offsetSeqEnd)&&(offsetAmbBegin>offsetSeqBegin))
						{
							builder->addData ((LETTER*)&_data[(offsetSeqBegin-_firstOffset)], (offsetAmbBegin-offsetSeqBegin), NCBI_DNA_WITH_AMB);
						}
						else
						{
							builder->addData ((LETTER*)&_data[(offsetSeqBegin-_firstOffset)], seqLength, NCBI_DNA_NO_AMB);
						}
					}

					/** size of the sequence + 1 because the sequence is finished by a null value */
					_readCurrentSize += seqLength+1;
				}
				else
				{
					_currentSequenceFile->unmapFile();
				}
			}
		}
     }

    return dp::ITER_UNKNOWN;
}
/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BlastdbSequenceIterator::readIndexFileAndCreateHeaderFile (u_int64_t firstOffset)
{
	/** Create a File index reader to read the file index */
	_firstOffset=firstOffset;
	if (_offset1>0)
		_currentSequenceFile->mapFile(_firstOffset, _offset1-_firstOffset+1);
	else
		_currentSequenceFile->mapFile(_firstOffset, _currentSequenceFile->getSize());
	_data = _currentSequenceFile->getData();

	_currentIndexFile->read();
	_currentIndexFile->setOffsetsStart(firstOffset);
	_offsetReadIndex = 0;
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

