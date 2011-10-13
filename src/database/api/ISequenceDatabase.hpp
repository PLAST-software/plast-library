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
 *
 *  Implements also a Design Pattern Composite; a database can be structure as a
 *  tree of sub databases.
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

    /** Returns a sequence given its offset in the database.
     *  Also returns the offset in the returned sequence.*/
    virtual bool getSequenceByOffset (u_int64_t offsetInDatabase, ISequence& sequence, u_int32_t& offsetInSequence) = 0;

    /** Returns actual information (database and offset) from a given offset. May be used for implementation
     *  that references serveral ISequenceDatabase instances (the given offset allows to compute the ISequenceDatabase*
     *  instance of interest). */
    virtual void getActualInfoFromOffset (u_int64_t offset, ISequenceDatabase*& actualDb, u_int64_t& actualOffset) = 0;

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
