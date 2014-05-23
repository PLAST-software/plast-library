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

#include <database/impl/BasicSequenceBuilder.hpp>

#include <misc/api/macros.hpp>
#include <string.h>
#include <stdlib.h>

#include <stdio.h>
#define DEBUG(a)  //printf a
#define INFO(a)   printf a

using namespace os;

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
BasicSequenceBuilder::BasicSequenceBuilder (Encoding destEncoding, size_t maxDataSize)
    : _sourceEncoding(UNKNOWN), _destEncoding(destEncoding), _convertTable(0),
      _currentSize(_sequence.data.letters.size)
{
    /** We allocate an inner buffer for storing sequence data. */
    _data.letters.resize (maxDataSize);

    /** We set the information of '_sequence' as a reference of information of '_data' */
    _sequence.data.setReference (0, _data.letters.data);

    /** We set the encoding of the ISequence to be returned. */
    _data.encoding = _destEncoding;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BasicSequenceBuilder::~BasicSequenceBuilder ()
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
void BasicSequenceBuilder::setComment (const char* buffer, size_t length)
{
    _comment.assign (buffer, length);
    _sequence.comment = _comment.c_str();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BasicSequenceBuilder::setCommentUri (const char* filename, u_int32_t offsetHeader,size_t commentMaxSize)
{
	char bufferNumber[50];

	_comment.assign (SEQUENCE_COMMENT_DETECTION, strlen(SEQUENCE_COMMENT_DETECTION));
	_comment.append (",", 1);
	_comment.append (filename, strlen(filename));
    sprintf(bufferNumber,",%d,%ld",offsetHeader,commentMaxSize);
    _comment.append (bufferNumber, strlen(bufferNumber));
    _sequence.comment = _comment.c_str();

}


/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BasicSequenceBuilder::addData (const LETTER* data, size_t size, Encoding encoding)
{
	DEBUG (("BasicSequenceBuilder::addData: data=%p  size=%ld  encoding=%d \n", data, size, encoding));

    /** We configure (if needed) the conversion table. */
    if (encoding != _sourceEncoding)
    {
        _sourceEncoding = encoding;
        _convertTable   = EncodingManager::singleton().getEncodingConversion (_sourceEncoding, _destEncoding);
    }

    DEBUG (("BasicSequenceBuilder::addData: going to fill the table with _currentSize=%d \n", _currentSize));

    /** We add the letters to the data, with a potential conversion (no conversion if no table). */
    if (encoding == ASCII)
    {
        if (_currentSize + size >= _data.letters.size)
        {
            /** We resize the buffer. Note that we add an extra size for avoiding too many reallocs. */
            _data.letters.resize (_currentSize + size + 1024);

            /** The data buffer may have a modified address due to the 'resize'
             *  => we have to update the '_sequence' reference to the new data.  */
            _sequence.data.setReference (_currentSize, _data.letters.data);
        }
        for (size_t i=0; i<size; i++)
    	{
    		char c = data[i];
    		if ((c<'A' || c>'Z') &&  (c<'a' || c>'z') )  { continue; };

    		LETTER l = (_convertTable ? _convertTable [(int)c] : c);
    		if (l != CODE_BAD)  {  _data.letters.data [_currentSize++] = l; }
    	}
    }
    else if (encoding == NCBI_DNA_NO_AMB)
    {
    	int32_t 	s;
    	u_int32_t 	remainder;
    	u_int32_t 	byteCount = 0;
    	size_t 	  	res_cnt = 0;

 		res_cnt = (size+1)* 4;

		/* Memory allocation for the output buffer */
        if (_currentSize + res_cnt >= _data.letters.size)
        {
            /** We resize the buffer. Note that we add an extra size for avoiding too many reallocs. */
            _data.letters.resize (_currentSize + res_cnt + 1024);

            /** The data buffer may have a modified address due to the 'resize'
             *  => we have to update the '_sequence' reference to the new data.  */
            _sequence.data.setReference (_currentSize, _data.letters.data);
        }


		/* loop on the number of data */
		for (byteCount = 0; byteCount < size; ++byteCount)
		{
			s = (data[byteCount] >> 6) & 0x03;
			_data.letters.data [_currentSize++] = (_convertTable ? _convertTable [(int)s] : s);
			s = (data[byteCount] >> 4) & 0x03;
			_data.letters.data [_currentSize++] = (_convertTable ? _convertTable [(int)s] : s);
			s = (data[byteCount] >> 2) & 0x03;
			_data.letters.data [_currentSize++] = (_convertTable ? _convertTable [(int)s] : s);
			s = (data[byteCount]) & 0x03;
			_data.letters.data [_currentSize++] = (_convertTable ? _convertTable [(int)s] : s);
		}

	   	/* the least two significant bits indicate how many residues are encoded in the last byte.	*/
		remainder = data[byteCount] & 0x03;
		for (u_int32_t y=0;y<remainder;y++)
		{
			s = (3 - (y % 4)) * 2;
			s = (data[byteCount] >> s) & 0x03;
			_data.letters.data [_currentSize++] = (_convertTable ? _convertTable [(int)s] : s);
		}
    }
    else if (encoding == NCBI_DNA_WITH_AMB)
    {
    	int32_t s;
    	int32_t remainder;
    	int32_t amb_res = 0;
    	int32_t byteCount = -1;

    	u_int64_t x;
    	u_int64_t soff = 0, eoff = 0;

    	u_int32_t res_cnt = 0;
    	u_int32_t amb_cnt = 0;
    	u_int32_t large_amb = 0;
    	u_int32_t amb_index = 0;

    	unsigned char *amb_ptr = NULL;

    	unsigned char lastByte;
    	const LETTER* noAmbConvertTable;
    	const LETTER* ambConvertTable;
    	noAmbConvertTable = EncodingManager::singleton().getEncodingConversion(NCBI_DNA_NO_AMB,SUBSEED);
    	ambConvertTable = EncodingManager::singleton().getEncodingConversion(NCBI_DNA_WITH_AMB,SUBSEED);

    	/* find the size of the ambiguity table */
    	amb_cnt = CHAR_TO_INT32(data[size],data[size+1],data[size+2],data[size+3]);

    	/* if the most significant bit is set on the count, then each correction
    	 * will take two entries in the table.  the layout is described below. */
    	large_amb = amb_cnt >> 31;
    	amb_cnt = amb_cnt & 0x7fffffff;

    	amb_index = size+4;
    	amb_ptr = (unsigned char*)&data[amb_index];

    	/* read the last byte of the sequence, so we can calculate the
    	* number of residues in the last byte of the sequence (0-3).
    	*/
    	lastByte = data[(size - 1)];

    	/* the least two significant bits indicate how many residues
    	* are encoded in the last byte.
    	*/
    	remainder = lastByte & 0x03;
    	res_cnt = (size - 1) * 4 + remainder;

		/* Memory allocation for the output buffer */
        if (_currentSize + res_cnt >= _data.letters.size)
        {
            /** We resize the buffer. Note that we add an extra size for avoiding too many reallocs. */
            _data.letters.resize (_currentSize + res_cnt + 1024);

            /** The data buffer may have a modified address due to the 'resize'
             *  => we have to update the '_sequence' reference to the new data.  */
            _sequence.data.setReference (_currentSize, _data.letters.data);
        }

    	byteCount = -1;

    	/* loop on the number of data */
    	for (x = 0; x < res_cnt; ++x)
    	{
    		/* decode the ambiguity table */
    		if (x == 0 || x > eoff)
    		{
    			/* get the residue symbol */
				amb_res = (int32_t) (*amb_ptr >> 4);

				/* the layout of the ambiguity table differs if it is using
				 * large offsets, i.e. offsets > 16 million.
				 *
				 * for small offsets the layout is:
				 *    4 bits - nucleotide
				 *    4 bits - repeat count
				 *   24 bits - offset
				 *
				 * for large offsets the layout is:
				 *    4 bits - nucleotide
				 *   12 bits - repeat count
				 *   48 bits - offset
				 */
				if (large_amb)
				{
					/* get the repeat count */
					eoff  = (((u_int64_t) (*amb_ptr & 0x0f)) << 8) + (((u_int64_t) *(amb_ptr+1)) << 0);

					/* get the offset */
					soff  = (((u_int64_t) *(amb_ptr+2)) << 40);
					soff += (((u_int64_t) *(amb_ptr+3)) << 32);
					soff += (((u_int64_t) *(amb_ptr+4)) << 24);
					soff += (((u_int64_t) *(amb_ptr+5)) << 16);
					soff += (((u_int64_t) *(amb_ptr+6)) << 8);
					soff += (((u_int64_t) *(amb_ptr+7)) << 0);

					amb_ptr += 8;
					amb_cnt -= 2;
				}
				else
				{
					/* get the repeat count */
					eoff  = (u_int64_t) (*amb_ptr & 0x0f);

					/* get the offset */
					soff  = (((u_int64_t) *(amb_ptr+1)) << 16);
					soff += (((u_int64_t) *(amb_ptr+2)) << 8);
					soff += (((u_int64_t) *(amb_ptr+3)) << 0);

					amb_ptr += 4;
					amb_cnt -= 1;
				}
				eoff += soff;
    		}

    		/* read the next byte if necessary */
    		if ((x % 4) == 0)
    		{
    			byteCount ++;
    		}

    		if (x >= soff && x <= eoff)
    		{
    			_data.letters.data [_currentSize++] = ambConvertTable[(u_int32_t)amb_res];
    		}
    		else
    		{
    			s = (3 - (x % 4)) * 2;
    			s = (data[byteCount] >> s) & 0x03;
    			_data.letters.data [_currentSize++] = noAmbConvertTable[s];
    		}
    	}
    }
    else
    {
        if (_currentSize + size >= _data.letters.size)
        {
            /** We resize the buffer. Note that we add an extra size for avoiding too many reallocs. */
            _data.letters.resize (_currentSize + size + 1024);

            /** The data buffer may have a modified address due to the 'resize'
             *  => we have to update the '_sequence' reference to the new data.  */
            _sequence.data.setReference (_currentSize, _data.letters.data);
        }
    	for (size_t i=0; i<size; i++)
        {
    	   LETTER l = (_convertTable ? _convertTable [(int)data[i]] : data[i]);

            if (l != CODE_BAD)  {  _data.letters.data [_currentSize++] = l; }
        }
    }

    DEBUG (("BasicSequenceBuilder::addData: table filled, now _currentSize=%d \n", _currentSize));
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

