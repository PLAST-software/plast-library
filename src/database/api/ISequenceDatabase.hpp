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

/** \file ISequenceDatabase.hpp
 *  \brief Definition of a genomic database
 *  \date 07/11/2011
 *  \author edrezen
 *
 *  We define here a sequences database mainly as a container of ISequence instances.
 *
 *  Two different ways for retrieving ISequence instances are provided:
 *     - retrieval of a specific instance, by index or by global offset in the database
 *     - usage of a ISequenceIterator that can loop over all ISequence instances of the database.
 */

#ifndef _ISEQUENCE_DATABASE_HPP_
#define _ISEQUENCE_DATABASE_HPP_

/********************************************************************************/

#include <designpattern/api/SmartPointer.hpp>
#include <designpattern/api/IProperty.hpp>
#include <database/api/ISequenceIterator.hpp>

/********************************************************************************/
/** \brief Definition of concepts related to genomic databases. */
namespace database {
/********************************************************************************/

/** \brief Define a database as a container of ISequence instances.
 *
 *  A sequence can be retrieved directly from this container (i.e. like a vector).
 *
 *  It can also create ISequenceIterator instances, so it has both direct access to
 *  a specific sequence and also iterative access to sequences through a created
 *  iterator.
 */
class ISequenceDatabase : public dp::SmartPointer
{
public:

    /** Retrieve the number of sequences.
     * \return the number of sequences in the database.
     */
    virtual size_t getSequencesNumber () = 0;

    /** Retrieve the database size.
     * \return the aggregated size of all sequences data. */
    virtual u_int64_t getSize () = 0;

    /** Returns a sequence given its index. Note that one can get the number of sequences with the
     * getSequencesNumber method.
     * \param[in]  index : the index of the wanted sequence
     * \param[out] sequence : the filled sequence if successful
     * \return true if the sequence has been retrieved, false otherwise.
     */
    virtual bool getSequenceByIndex (size_t index, ISequence& sequence) = 0;

    /** Returns a sequence given an offset (in the database).
     *  Also returns the offset in the returned sequence and the actual offset in the database.
     *  \param[in] offset : offset of the sequence in the database
     *  \param[in] sequence : the filled sequence if successful
     *  \param[out] offsetInSequence : offset of the given offset in the sequence
     *  \param[out] offsetInDatabase : offset of the given offset in the database
     * \return true if the sequence has been retrieved, false otherwise.
     */
    virtual bool getSequenceByOffset (
        u_int64_t offset,
        ISequence& sequence,
        u_int32_t& offsetInSequence,
        u_int64_t& offsetInDatabase
    ) = 0;

    /** Returns a sequence given a part of its string identifer (in the database).
     *  \param[in] id : part of the identifier
     *  \param[in] sequence : the filled sequence if successful
     * \return true if the sequence has been retrieved, false otherwise.
     */
    virtual bool getSequenceByName (
        const std::string& id,
        ISequence& sequence
    ) = 0;

    /** Creates a Sequence iterator.
     * \return the created iterator.
     */
    virtual ISequenceIterator* createSequenceIterator () = 0;

    /** Split the current database in several database. All the returned
     *  databases should represent the same set of ISequence instances than the source database.
     *  \param[in] nbSplit : number of parts we want to split the database
     *  \return a vector of split ISequenceDatabase instances.
     */
    virtual std::vector<ISequenceDatabase*> split (size_t nbSplit) = 0;

    /** Return properties about the instance. These properties can provide information for statistical
     * purpose for instance.
     * \param[in] root : root string
     * \return a created IProperties instance
     */
    virtual dp::IProperties* getProperties (const std::string& root) = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ISEQUENCE_DATABASE_HPP_  */
