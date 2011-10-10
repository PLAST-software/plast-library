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

#include "BasicSequenceBuilder.hpp"
#include "MemoryAllocator.hpp"

#include <string.h>
#include <stdlib.h>

#include <stdio.h>
#define DEBUG(a)  //printf a
#define INFO(a)   printf a

using namespace os;

/********************************************************************************/
namespace database {
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
void BasicSequenceBuilder::addData (const LETTER* data, size_t size, Encoding encoding)
{
    DEBUG (("BasicSequenceBuilder::addData: data=%p  size=%ld  encoding=%d \n", data, size, encoding));

    /** We configure (if needed) the conversion table. */
    if (encoding != _sourceEncoding)
    {
        _sourceEncoding = encoding;
        _convertTable   = EncodingManager::singleton().getEncodingConversion (_sourceEncoding, _destEncoding);
    }

    if (_currentSize + size >= _data.letters.size)
    {
        DEBUG (("BasicSequenceBuilder::addData: realloc needed : current=%ld  wanted=%ld  actualSize=%d \n",
            _currentSize, size, _data.letters.size
        ));

        /** We resize the buffer. Note that we add an extra size for avoiding too many reallocs. */
        _data.letters.resize (_data.letters.size + 1024);
    }

    DEBUG (("BasicSequenceBuilder::addData: going to fill the table with _currentSize=%ld \n", _currentSize));

    /** We add the letters to the data, with a potential conversion (no conversion if no table). */
    for (size_t i=0; i<size; i++)
    {
        LETTER l = (_convertTable ? _convertTable [(int)data[i]] : data[i]);

        if (l != CODE_BAD)  {  _data.letters.data [_currentSize++] = l; }
    }

    DEBUG (("BasicSequenceBuilder::addData: table filled, now _currentSize=%d \n", _currentSize));
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

