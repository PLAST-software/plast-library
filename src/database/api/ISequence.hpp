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

/** \file ISequence.hpp
 *  \brief Definition of what a genomic sequence is
 *  \date 07/11/2011
 *  \author edrezen
 *
 *   Definition of what a genomic sequence is
 */

#ifndef _ISEQUENCE_HPP_
#define _ISEQUENCE_HPP_

/********************************************************************************/

#include <database/api/IWord.hpp>

#include <string>
#include <iostream>
#include <sstream>

/********************************************************************************/
/** \brief Definition of concepts related to genomic databases. */
namespace database {
/********************************************************************************/

class ISequenceDatabase;

/** \brief Genomic sequence definition.
 *
 *  We define here a central concept used by PLAST: a genomic sequence holding
 *  a series of nucleotides or amino acids.
 *
 *  Such a sequence has also some kind of description (named 'comment' here) that
 *  can hold an identifier and a short descriptive text.
 *
 *  Instances of this interface are used by ISequenceDatabase objects that act as containers of
 *  such instances with some means to find specific sequences and loop over all sequences.
 *
 *  Note the following:
 *     - the way the information is created is delegated to the interface ISequenceBuilder.
 *     - the way the information is provided to clients is delegated to the interface ISequenceIterator
 */
struct ISequence
{
    /** Default constructor. */
    ISequence () : database(0), comment(""), index(0), offsetInDb(0), length(0) {}

    /** Constructor. */
    ISequence (const char* comment) : database(0), comment(comment), index(0), offsetInDb(0), length(0) {}

    /** Reference to the database that holds this sequence. */
    ISequenceDatabase*  database;

    /** Comment of this sequence. */
    const char*         comment;

    /** Data of the sequence, coded as a IWord (ie a table of letters in a specific encoding). */
    IWord               data;

    /** Index of the sequence in its containing database. Has sense in ordered databases. */
    u_int32_t           index;

    /** Global offset of the sequence in the containing database. */
    u_int64_t           offsetInDb;

    /** Length of the sequence (shortcut). */
    u_int32_t           length;

    /** Returns the length of the sequence.
     * \return the sequence length. */
    u_int32_t getLength ()  const {  return data.letters.size > 0 ? data.letters.size : length;  }

    /** Reutnrs a constant pointer to the raw data.
     * \return the data pointer. */
    const LETTER* getData () const  { return data.letters.data; }

    /** Return the identifier of the sequence, extracted from the full comment.
     * Note: this is not an optimal implementation because we have to find each time
     * where is the first separator. One could imagine some caching procedure for
     * keeping this information after the database creation.
     * \return the identifier.
     */
    std::string getIdentifier () const
    {
        /** Not optimal... */
        std::string result;
        char buffer[128];
        if (retrieveId (buffer, sizeof(buffer)) > 0)  {  result.assign (buffer);  }
        return result;
    }

    /** Get id and definition of the sequence.
     * \param[out] bufId  : the string holding the identifier
     * \param[out] lenId  : the length of the identifier
     * \param[out] bufDef : the string holding the definition
     * \param[out] lenDef : the length of the definition
     */
    void retrieveIdAndDefinition (
        char* bufId,  size_t& lenId,
        char* bufDef, size_t& lenDef
    ) const
    {
        /** A little check. */
        if (!bufId || !bufDef)  { return; }

        if (comment != 0)
        {
            const char* lookup = searchIdSeparator (comment);
            if (lookup != 0)
            {
                size_t l = (size_t) (lookup-comment);
                lenId = lenId > l ? l : lenId;

                strncpy (bufId, comment, lenId);
                bufId [lenId] = 0;

                strncpy (bufDef, lookup+1, lenDef);
                bufDef [lenDef-1] = 0;
            }
            else
            {
                /** We have only id, no def. */
                strncpy (bufId, comment, lenId);
                bufId [lenId-1] = 0;

                *bufDef = 0;
                lenDef  = 0;
            }
        }
        else
        {
            lenId  = 0;     *bufId  = 0;
            lenDef = 0;     *bufDef = 0;
        }
    }

    /** Get id of the sequence.
     * \param[out] bufId : the buffer holding the identifier
     * \param[in] lenId : maximum size of the buffer
     * \return actual length of the retrieved buffer
     */
    size_t  retrieveId (char* bufId,  size_t lenId) const
    {
        if (bufId && comment)
        {
            *bufId = 0;
            const char* lookup = searchIdSeparator (comment);

            size_t l =  (lookup != 0 ? (size_t) (lookup-comment) : strlen(comment));
            lenId = lenId > l ? l : lenId-1;

            strncpy (bufId, comment, lenId);
            bufId [lenId] = 0;
        }
        return lenId;
    }

    /** Tool for dumping a sequence content. Useful for debug purpose.
     * \return the string representing the sequence.
     */
    std::string toString () const
    {
        std::stringstream ss;
        //ss << "[SEQUENCE #" << index << "  offsetInDb=" << offsetInDb << "  " << data.toString() << "]";
        ss << "[SEQUENCE #" << index << "  offsetInDb=" << offsetInDb << "  len=" << getLength() << "]";
        return ss.str ();
    }

    /********************************************************************************/

    /** Input stream overload.
     * \param[in] is : the input stream
     * \param[in] seq : the sequence to be filled
     * \return the input stream
     */
    friend std::istream& operator>> (std::istream& is, ISequence& seq)
    {
        char c;
        u_int32_t   len = 0;
        std::string str;

        /** We read from the stream. */
        is >> len >> c >> str;

        /** We set a fake reference (just interested in size). */
        seq.data.setReference (len, 0);

        /** We set a dummy comment. */
        seq.comment = 0;

        return is;
    }

    /********************************************************************************/

    /** Output stream overload.
     * \param[in] os : the output stream
     * \param[in] seq : the sequence to be dumped
     * \return the output stream
     */
    friend std::ostream& operator<< (std::ostream& os, const ISequence& seq)
    {
        char c = ' ';
        os << seq.getLength() << c << seq.comment;
        return os;
    }

    /********************************************************************************/

    /** Looks for the first separator character in the sequence comment
     * \param[in] comment : buffer to search the separator from
     * \return the buffer beginning by the found separator, NULL if not found.
     */
    static char* searchIdSeparator (const char* comment)
    {
		/** A basic implementation would be to return strchr (comment, ' ');
		 *  BUT! Some silly databases have \t as separators (instead of ' ')
		 *  See also bug 14459.
		 */
    	char c = 0;

    	for (const char* loop = comment; (c = *loop); loop++)
    	{
    		if (c <= ' ')  {  return (char*)loop; }
    	}
    	return 0;
    }
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ISEQUENCE_HPP_ */
