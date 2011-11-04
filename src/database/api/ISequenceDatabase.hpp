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

/*****************************************************************************
 *   Define an interface of what a sequence database is.
 *****************************************************************************/

#ifndef _ISEQUENCE_DATABASE_HPP_
#define _ISEQUENCE_DATABASE_HPP_

/********************************************************************************/

#include "SmartPointer.hpp"
#include "IProperty.hpp"
#include "ISequenceIterator.hpp"

/********************************************************************************/
namespace database {
/********************************************************************************/

/********************************************************************************/

/** Define a database as a container of ISequence instances.
 *  A sequence can be retrieved directly from this container (i.e. like a vector).
 *
 *  It can also create ISequenceIterator instances, so it has both direct access to
 *  a specific sequence and also iterative access to sequences through a created
 *  iterator.
 */

class ISequenceDatabase : public dp::SmartPointer
{
public:

    /** Retrieve the number of sequences. */
    virtual size_t getSequencesNumber () = 0;

    /** Retrieve the database size. */
    virtual u_int64_t getSize () = 0;

    /** Returns a sequence given its index. */
    virtual bool getSequenceByIndex (size_t index, ISequence& sequence) = 0;

    /** Returns a sequence given an offset (in the database).
     *  Also returns the offset in the returned sequence and the actual offset in the database.*/
    virtual bool getSequenceByOffset (
        u_int64_t offset,
        ISequence& sequence,
        u_int32_t& offsetInSequence,
        u_int64_t& offsetInDatabase
    ) = 0;

    /** Creates a Sequence iterator. */
    virtual ISequenceIterator* createSequenceIterator () = 0;

    /** Split the current database in several database. All the returned
     *  databases should represent the same set of ISequence instances than the source database. */
    virtual std::vector<ISequenceDatabase*> split (size_t nbSplit) = 0;

    /** Return properties about the instance. */
    virtual dp::IProperties* getProperties (const std::string& root) = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ISEQUENCE_DATABASE_HPP_  */
