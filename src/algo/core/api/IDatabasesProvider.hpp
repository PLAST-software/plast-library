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

/** \file IDatabasesProvider.hpp
 *  \brief Some kind of cache for subject and query databases
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _IDATABASES_PROVIDER_HPP_
#define _IDATABASES_PROVIDER_HPP_

/********************************************************************************/

#include <algo/core/api/IAlgoParameters.hpp>

#include <designpattern/impl/ListIterator.hpp>

#include <database/api/ISequenceIterator.hpp>
#include <database/api/ISequenceDatabase.hpp>

/********************************************************************************/
namespace algo {
namespace core {
/********************************************************************************/

/** \brief Provides means for getting the subject and query databases.
 *
 * The purpose of this interface is to propose a way to retrieve (through iterators)
 * the subject and query databases in a single shot.
 *
 * The idea behind the scene is that implementors can provide some cache mechanism
 * during several to 'createDatabases'.
 *
 * For instance, when subject and query databases are too big and must be split (through
 * the -max-database-size), the Plast algorithm has then two loops, the outer one being
 * a loop over the query fragments, and the inner one being a loop over the subject fragments.
 *
 * It is so interesting in this case to keep the current query part and distribute it over
 * all subject parts.
 */
class IDatabasesProvider : public dp::SmartPointer
{
public:

    /** */
    virtual void createDatabases (
        algo::core::IParameters* params,
        const std::vector<misc::ReadingFrame_e>&    sbjFrames,
        const std::vector<misc::ReadingFrame_e>&    qryFrames,
        database::ISequenceIteratorFactory*         sbjFactory,
        database::ISequenceIteratorFactory*         qryFactory
    ) = 0;

    /** */
    virtual dp::Iterator<database::ISequenceDatabase*>* getSubjectDbIterator () = 0;

    /** */
    virtual dp::Iterator<database::ISequenceDatabase*>* getQueryDbIterator () = 0;

    /** Create a database object (with means for retrieving sequence within the database) from an uri (likely
     *  a local file, but it should be a location on a remote computer). A Range can be provided for using only
     *  a part of the database.
     *  \param[in] uri : uri of the database file to be read
     *  \param[in] range : a range to be read in the file (ie. a starting and ending offsets)
     *  \param[in] filtering : tells whether low informative regions have to be filtered out from the database
     *  \param[in] sequenceIteratorFactory : a factory can be provided. If null, should use createSequenceIteratorFactory
     *  \return a new ISequenceDatabase instance
     */
    virtual database::ISequenceDatabase* createDatabase (
            const std::string& uri,
            const misc::Range64& range,
            int filtering,
            database::ISequenceIteratorFactory* sequenceIteratorFactory,
            std::set<u_int64_t>* blacklist = NULL
            ) = 0;
};

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/

#endif /* _IDATABASES_PROVIDER_HPP_ */
