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

#ifndef _BASIC_SEQUENCE_BUILDER_HPP_
#define _BASIC_SEQUENCE_BUILDER_HPP_

/********************************************************************************/

#include "ISequenceBuilder.hpp"
#include "IAlphabet.hpp"
#include "IWord.hpp"

#include <string>

/********************************************************************************/
namespace database {
/********************************************************************************/

/** We need a sequence builder that fills a SequenceCache instance during referenced iteration. */
class BasicSequenceBuilder : public ISequenceBuilder
{
public:

    BasicSequenceBuilder (Encoding destEncoding, size_t maxDataSize=580);
    virtual ~BasicSequenceBuilder ();

    ISequence* getSequence ()  { return &_sequence; }

    Encoding getEncoding ()  { return _destEncoding; }

    void setComment (const char* buffer, size_t length);

    void resetData  ()  { _currentSize = 0; }
    void addData    (const LETTER* data, size_t size, Encoding encoding);

    void postTreamtment ()  { /* nothing to do. */ }

private:

    Encoding      _sourceEncoding;
    Encoding      _destEncoding;
    const LETTER* _convertTable;

    std::string _comment;

    /** We need some containers for acquiring letters. */
    IWord _data;

    /** Sequence provided by the builder. */
    ISequence   _sequence;
    os::Size&   _currentSize;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _BASIC_SEQUENCE_BUILDER_HPP_ */
