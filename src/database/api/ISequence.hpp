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
 *  a series of nucleotids or amino acids.
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
    /** Constructor. */
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

    /** */
    u_int32_t getLength ()  const {  return data.letters.size > 0 ? data.letters.size : length;  }

    /** Get id and definition of the sequence.
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
            const char* lookup = strchr (comment, ' ');
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
     */
    size_t  retrieveId (char* bufId,  size_t lenId) const
    {
        if (bufId && comment)
        {
            *bufId = 0;
            const char* lookup = strchr (comment, ' ');

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
    friend std::ostream& operator<< (std::ostream& os, const ISequence& seq)
    {
        char c = ' ';
        os << seq.getLength() << c << seq.comment;
        return os;
    }
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ISEQUENCE_HPP_ */
