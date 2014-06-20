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

/** \file ISequenceBuilder.hpp
 *  \brief Define how sequences content is built.
 *  \date 07/11/2011
 *  \author edrezen
 *
 *   Define how sequences content is built.
 */

#ifndef _ISEQUENCE_BUILDER_HPP_
#define _ISEQUENCE_BUILDER_HPP_

/********************************************************************************/

#include <database/api/ISequence.hpp>
#include <designpattern/api/SmartPointer.hpp>

/********************************************************************************/
/** \brief Definition of concepts related to genomic databases. */
namespace database {
/********************************************************************************/

/* Forward references. */
class ISequence;

/********************************************************************************/
/** \brief Interface that can modify a ISequence instance internals
 *
 * The ISequence structure just holds information but doesn't explain how to instantiate objects.
 *
 * This interface provides means for building the content (comment and data) of a ISequence instance.
 * In that sense, it can be seen as a Builder Design Pattern, where instances of ISequence are built
 * during some process.
 *
 * Actually, this process corresponds to the reading of a genomic database (reading of a FASTA file for
 * instance); a builder is attached to the reader and each time the reader finds a piece of a sequence,
 * it calls the builder which completes the ISequence building with this read information.
 *
 *  For instance, during a FASTA file iteration, the iterator finds a comment (beginning
 *  by '>'). It can then call the builder through the 'setComment' method, and so the
 *  builder can fill the appropriate comment attribute of the ISequence instance it builds. When the
 *  iterator finds a line of data, it calls the 'addData' method of the builder.
 */
class ISequenceBuilder: public dp::SmartPointer
{
public:

    /** Accessor to the ISequence.
     * \return the built sequence.
     */
    virtual ISequence* getSequence  () = 0;

    /** Encoding of the data to be built.
     * \return the encoding scheme.
     */
    virtual Encoding getEncoding () = 0;

    /** Set the comment for the current sequence being built.
     * \param[in] buffer : buffer holding the textual commentary.
     * \param[in] length : size of the buffer
     */
    virtual void setComment (const char* buffer, size_t length) = 0;

    /** Set the comment for the current sequence being built.
     * \param[in] filename : file name of the header file for blast format
     * \param[in] offsetHeader : offset in the header file
     * \param[in] size : comment size
     */
    virtual void setCommentUri (const char* filename, u_int32_t offsetHeader, u_int32_t size) = 0;

    /** Reset the data for the current sequence being built. */
    virtual void resetData  () = 0;

    /** Add some information for the current sequence being built.
     *  Note that a potential encoding conversion will have to be done from
     *  the encoding scheme of the provided data to the encoding scheme of the
     *  builder.
     *  \param[in] data : the data
     *  \param[in] size : size of the data
     *  \param[in] encoding : encoding type of the data
     */
    virtual void addData (const LETTER* data, size_t size, Encoding encoding) = 0;

    /** Some post treatment capabilities. */
    virtual void postTreamtment () = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ISEQUENCE_BUILDER_HPP_ */
