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

#include <designpattern/impl/ListIterator.hpp>

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
        const std::vector<misc::ReadingFrame_e>&  sbjFrames,
        const std::vector<misc::ReadingFrame_e>&  qryFrames
    ) = 0;

    /** */
    virtual dp::impl::ListIterator<database::ISequenceDatabase*> getSubjectDbIterator () = 0;

    /** */
    virtual dp::impl::ListIterator<database::ISequenceDatabase*> getQueryDbIterator () = 0;
};

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/

#endif /* _IDATABASES_PROVIDER_HPP_ */
