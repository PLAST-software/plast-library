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

/** \file AbstractAlgorithm.hpp
 *  \brief Basic implementation of the IIndexator interface
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _BASIC_ALGO_INDEXATOR_HPP_
#define _BASIC_ALGO_INDEXATOR_HPP_

/********************************************************************************/

#include <index/api/IDatabaseIndex.hpp>

#include <algo/core/api/IAlgoIndexator.hpp>
#include <algo/core/api/IAlgoParameters.hpp>

/********************************************************************************/
namespace algo {
namespace core {
/** \brief Implementation of concepts for configuring and running PLAST. */
namespace impl {
/********************************************************************************/

/** \brief Default implementation of the IIndexator interface
 *
 * This implementation relies on the indexation::IDatabaseIndex interface (and its
 * implementations).
 *
 * It merely builds the indexations for the subject and query databases. It has then
 * the needed information for creating  IHitIterator instances through the
 * createHitIterator() method.
 *
 * In particular, SeedHitIteratorCached instances (or subclasses) will be created, which
 * makes sense here because such iterators takes as input databases indexations.
 */
class BasicIndexator : public IIndexator
{
public:

    /** Constructor.
     * \param[in] model : the seed model to be used for indexation
     * \param[in] params : holds parameters for customization
     * */
    BasicIndexator (
        seed::ISeedModel* model,
        algo::core::IParameters* params,
        indexation::IDatabaseIndexFactory* factory,
        bool& isRunning
    );

    /** Destructor. */
    virtual ~BasicIndexator ();

    /** \copydoc IIndexator::getSubjectDatabase */
    database::ISequenceDatabase* getSubjectDatabase ()  { return _subjectDatabase; }

    /** \copydoc IIndexator::getQueryDatabase */
    database::ISequenceDatabase* getQueryDatabase   ()  { return _queryDatabase;   }

    /** \copydoc IIndexator::setSubjectDatabase */
    void setSubjectDatabase (database::ISequenceDatabase* db);

    /** \copydoc IIndexator::setQueryDatabase */
    void setQueryDatabase   (database::ISequenceDatabase* db);

    /** \copydoc IIndexator::getSubjectIndex */
    indexation::IDatabaseIndex* getSubjectIndex ()  { return _subjectIndex; }

    /** \copydoc IIndexator::getQueryIndex */
    indexation::IDatabaseIndex* getQueryIndex   ()  { return _queryIndex;   }

    /** \copydoc IIndexator::build */
    void build (dp::ICommandDispatcher* dispatcher);

    /** \copydoc IIndexator::createHitIterator */
    algo::hits::IHitIterator* createHitIterator ();

    /** \copydoc IIndexator::getProperties */
    dp::IProperties* getProperties ();

protected:

    /** The seed model to be used for indexing the subject and query databases. */
    seed::ISeedModel* _model;

    /** Smart setter for the _model attribute. */
    void setModel (seed::ISeedModel* model)  {  SP_SETATTR(model); }

    /** Parameters for customization. */
    algo::core::IParameters* _params;

    /** Smart setter for the _params attribute. */
    void setParams (algo::core::IParameters* params)  {  SP_SETATTR(params); }

    /** Factory for building IDatabaseIndex instances. */
    indexation::IDatabaseIndexFactory* _factory;

    /** Smart setter for the _factory attribute. */
    void setFactory (indexation::IDatabaseIndexFactory* factory)  {  SP_SETATTR(factory); }

    /** Subject database. */
    database::ISequenceDatabase* _subjectDatabase;

    /** Query database. */
    database::ISequenceDatabase* _queryDatabase;

    /** Subject database index. */
    indexation::IDatabaseIndex* _subjectIndex;

    /** Query database index. */
    indexation::IDatabaseIndex* _queryIndex;

    /** */
    bool& _isRunning;

    /** Build an index for the some database.
     * \param[in] database : the database to be indexed
     * \param[in] model : the seed model used for the indexation
     * \param[in] dispatcher : command dispatcher for running the indexation.
     */
    indexation::IDatabaseIndex* buildIndex (
        database::ISequenceDatabase* database,
        seed::ISeedModel*            model,
        dp::ICommandDispatcher*      dispatcher
    );
};

/********************************************************************************/

/** \brief  Default implementation of the IIndexator interface with sorted seeds
 *
 * This implementation creates IHitIterator instances that have no classic seeds ordering.
 */
class BasicSortedIndexator : public BasicIndexator
{
public:

    /** \copydoc BasicIndexator::BasicIndexator */
    BasicSortedIndexator (
        seed::ISeedModel* model,
        algo::core::IParameters* params,
        indexation::IDatabaseIndexFactory* factory,
        bool& isRunning
    );

    /** \copydoc BasicIndexator::createHitIterator */
    algo::hits::IHitIterator* createHitIterator ();
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _BASIC_ALGO_INDEXATOR_HPP_ */
