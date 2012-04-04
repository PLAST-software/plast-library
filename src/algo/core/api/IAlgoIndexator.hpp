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

/** \file IAlgoIndexator.hpp
 *  \brief Interface for subject and query databases indexing and providing hits iterator
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _IALGO_INDEXATOR_HPP_
#define _IALGO_INDEXATOR_HPP_

/********************************************************************************/

#include <designpattern/api/SmartPointer.hpp>
#include <designpattern/api/ICommand.hpp>

#include <database/api/ISequenceDatabase.hpp>

#include <index/api/IDatabaseIndex.hpp>

#include <algo/hits/api/IHitIterator.hpp>

/********************************************************************************/
/** \brief PLAST algorithm concepts. */
namespace algo {
/** \brief Concept for configuring and running PLAST. */
namespace core {
/********************************************************************************/

/** \brief Interface providing the source IHitIterator instance
 *
 * The PLAST design is based on the IHitIterator interface. As a starting point,
 * the PLAST algorithm needs an initial IHitIterator, provided by the IIndexator
 * interface.
 *
 * The IIndexator interface takes as input the subject and query databases, indexes them
 * and then can create IHitIterator instances (with createHitIterator()),
 * used throughout the algorithm execution.
 *
 *  \see IHitIterator
 */
class IIndexator : public dp::SmartPointer
{
public:

    /** Getter on the subject database.
     * \return the subject database.
     */
    virtual database::ISequenceDatabase* getSubjectDatabase () = 0;

    /** Getter on the query database.
     * \return the query database.
     */
    virtual database::ISequenceDatabase* getQueryDatabase   () = 0;

    /** Setter on the subject database.
     * \param[in] db : the subject database.
     */
    virtual void setSubjectDatabase (database::ISequenceDatabase* db) = 0;

    /** Setter on the query database.
     * \param[in] db : the query  database.
     */
    virtual void setQueryDatabase   (database::ISequenceDatabase* db) = 0;

    /** Indexation of the subject and query databases. Smart implementation
     *  could check whether indexations is already done or not.
     *
     *  A provided ICommandDispatcher instance is used for carrying out the
     *  indexation; it could be a parallel implementation for parallelization
     *  of the indexation process (with potential time speed up)
     *
     * param[in] dispatcher : dispatcher used for indexing the databases
     */
    virtual void build (dp::ICommandDispatcher* dispatcher) = 0;

    /** Getter on the subject database index.
     * \return the subject database index
     */
    virtual indexation::IDatabaseIndex* getSubjectIndex () = 0;

    /** Getter on the query database index.
     * \return the query database index
     */
    virtual indexation::IDatabaseIndex* getQueryIndex   () = 0;

    /** Factory method that creates IHitIterator instances.
     * \return the new IHitIterator instance
     */
    virtual algo::hits::IHitIterator* createHitIterator () = 0;

    /** Return properties about the instance.
     * \return the properties
     */
    virtual dp::IProperties* getProperties () = 0;
};

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/

#endif /* _IALGO_INDEXATOR_HPP_ */
