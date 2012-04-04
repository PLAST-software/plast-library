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

/** \file BasicSequenceBuilder.hpp
 *  \brief Basic implementation of the ISequenceBuilder interface
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _BASIC_SEQUENCE_BUILDER_HPP_
#define _BASIC_SEQUENCE_BUILDER_HPP_

/********************************************************************************/

#include <database/api/ISequenceBuilder.hpp>
#include <database/api/IWord.hpp>

/********************************************************************************/
namespace database {
/** \brief Implementation of concepts related to genomic databases. */
namespace impl {
/********************************************************************************/

/** \brief Basic implementation of the ISequenceBuilder interface
 *
 *  This implementation knows how to fill ISequence attributes. It is supposed to
 *  be called by some sequence iterator that gather information.
 */
class BasicSequenceBuilder : public ISequenceBuilder
{
public:

    /** Constructor.
     * \param[in] destEncoding : encoding scheme of the destination data.
     * \param[in] maxDataSize  : initial size of the data attribute (may be increase during building if needed).
     */
    BasicSequenceBuilder (Encoding destEncoding, size_t maxDataSize=580);

    /** Desctructor. */
    virtual ~BasicSequenceBuilder ();

    /** \copydoc ISequenceBuilder::getSequence  */
    ISequence* getSequence ()  { return &_sequence; }

    /** \copydoc ISequenceBuilder::getEncoding  */
    Encoding getEncoding ()  { return _destEncoding; }

    /** \copydoc ISequenceBuilder::setComment  */
    void setComment (const char* buffer, size_t length);

    /** \copydoc ISequenceBuilder::resetData  */
    void resetData  ()  { _currentSize = 0; }

    /** \copydoc ISequenceBuilder::addData  */
    void addData    (const LETTER* data, size_t size, Encoding encoding);

    /** Some post treatment capabilities. Does nothing for this implementation. */
    void postTreamtment ()  { /* nothing to do. */ }

private:

    /** Source encoding scheme. */
    Encoding      _sourceEncoding;

    /** Destination encoding scheme. */
    Encoding      _destEncoding;

    /** Conversion table from the source to the encoding scheme. */
    const LETTER* _convertTable;

    /** Hold the comment of the currently built sequence. */
    std::string _comment;

    /** We need some containers for acquiring letters. */
    IWord _data;

    /** Sequence provided by the builder. */
    ISequence   _sequence;

    /** Current size of the built data. */
    Size&   _currentSize;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _BASIC_SEQUENCE_BUILDER_HPP_ */
