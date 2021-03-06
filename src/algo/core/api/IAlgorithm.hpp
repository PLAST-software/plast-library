/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.3, released November 2015                                     *
 *   Copyright (c) 2009-2015 Inria-Cnrs-Ens                                  *
 *                                                                           *
 *   PLAST is free software; you can redistribute it and/or modify it under  *
 *   the Affero GPL ver 3 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   Affero GPL ver 3 License for more details.                              *
 *****************************************************************************/

/** \file IAlgorithm.hpp
 *  \brief Interface defining the PLAST algorithm.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _IALGORITHM_HPP_
#define _IALGORITHM_HPP_

/********************************************************************************/

#include <designpattern/api/ICommand.hpp>
#include <designpattern/impl/Observer.hpp>
#include <designpattern/impl/ListIterator.hpp>

#include <os/impl/TimeTools.hpp>

#include <database/api/IDatabaseQuickReader.hpp>
#include <database/impl/ReadingFrameSequenceIterator.hpp>

#include <seed/api/ISeedModel.hpp>

#include <algo/core/api/IAlgoConfig.hpp>
#include <algo/core/api/IAlgoParameters.hpp>
#include <algo/core/api/IAlgoIndexator.hpp>
#include <algo/core/api/IScoreMatrix.hpp>
#include <algo/core/api/IDatabasesProvider.hpp>

#include <algo/hits/api/IHitIterator.hpp>

#include <alignment/core/api/IAlignmentContainer.hpp>

#include <vector>
#include <string>

/********************************************************************************/
/** \brief PLAST algorithm concepts. */
namespace algo {
/** \brief Concept for configuring and running PLAST. */
namespace core {
/********************************************************************************/

/** \brief Interface that defines an algorithm
 *
 * We define here what a plast algorithm is: a mere command (due to dp::ICommand
 * inheritance) supposed to provide the end user service, which consists in finding
 * alignments between a subject and a query databases.
 *
 * The algorithm needs a lot of inputs that can be set through a bunch of setters.
 * Note that the corresponding getters exist.
 *
 * The actual job the algorithm does has to be implemented as the ICommand::execute()
 * method. The method implementation can rely on the various instances attached to
 * the IAlgorithm instance.
 *
 * Defining an algorithm as a ICommand eases the way to run an algorithm: one has just
 * to use a ICommandDispatcher for executing the IAlgorithm instance.
 *
 * the IAlgorithm interface defines two factory methods:
 *     - createDatabaseIterator() : creates one (or several) database instance (to be called twice, one for subject and one for query)
 *     - createHitIterator()      : creates the hits iterator to be used throughout the PLAST algorithm.
 *
 * Instances of IAlgorithm are (should be) created through the IEnvironment interface.
 *
 * IAlgorithm is both a subject and an observer; it can listen to notifications from
 * inner parts and forward them to potential listeners (likely a IEnvironment instance).
 *
 * \see IEnvironment
 */

class IAlgorithm : public dp::ICommand, public dp::impl::Subject, public dp::IObserver
{
public:

    /** Getter for the IConfiguration instance. */
    virtual IConfiguration*                         getConfig           () = 0;

    /** Getter for the IDatabaseQuickReader instance. */
    virtual database::IDatabaseQuickReader*         getReader           () = 0;

    /** Getter for the IParameters instance. */
    virtual IParameters*                            getParams           () = 0;

    /** Getter for the IAlignmentFilter instance. */
    virtual alignment::filter::IAlignmentFilter*    getFilter           () = 0;

    /** Getter for the IAlignmentResultVisitor instance. */
    virtual alignment::core::IAlignmentContainerVisitor*  getResultVisitor    () = 0;

    /** Getter for the IDatabasesProvider instance. */
    virtual algo::core::IDatabasesProvider*         getDatabasesProvider () = 0;

    /** Getter for the ISeedModel instance. */
    virtual seed::ISeedModel*                       getSeedsModel       () = 0;

    /** Getter for the IScoreMatrix instance. */
    virtual algo::core::IScoreMatrix*               getScoreMatrix      () = 0;

    /** Getter for the IGlobalParameters instance. */
    virtual statistics::IGlobalParameters*          getGlobalStatistics () = 0;

    /** Getter for the IQueryInformation instance. */
    virtual statistics::IQueryInformation*          getQueryInfo        () = 0;

    /** Getter for the IIndexator instance. */
    virtual IIndexator*                             getIndexator        () = 0;

    /** Getter for the IHitIterator instance. */
    virtual algo::hits::IHitIterator*               getHitIterator      () = 0;

    /** Getter for the TimeInfo attribute. */
    virtual os::impl::TimeInfo*                     getTimeStats        () = 0;

    /** Setter for the IConfiguration attribute. */
    virtual void setConfig           (IConfiguration*                 config) = 0;

    /** Setter for the IDatabaseQuickReader attribute. */
    virtual void setReader           (database::IDatabaseQuickReader* reader) = 0;

    /** Setter for the IParameters attribute. */
    virtual void setParams           (IParameters*                    params) = 0;

    /** Setter for the IAlignmentFilter instance. */
    virtual void setFilter           (alignment::filter::IAlignmentFilter* filter)  = 0;

    /** Setter for the IAlignmentResultVisitor attribute. */
    virtual void setResultVisitor    (alignment::core::IAlignmentContainerVisitor* visitor) = 0;

    /** Setter for the IDatabasesProvider instance. */
    virtual void setDatabasesProvider (algo::core::IDatabasesProvider* dbProvider) = 0;

    /** Setter for the ISeedModel attribute. */
    virtual void setSeedsModel       (seed::ISeedModel*               model)  = 0;

    /** Setter for the IScoreMatrix attribute. */
    virtual void setScoreMatrix      (algo::core::IScoreMatrix*       matrix) = 0;

    /** Setter for the IGlobalParameters attribute. */
    virtual void setGlobalStatistics (statistics::IGlobalParameters*  params) = 0;

    /** Setter for the IQueryInformation attribute. */
    virtual void setQueryInfo        (statistics::IQueryInformation*  info) = 0;

    /** Setter for the IIndexator attribute. */
    virtual void setIndexator        (IIndexator*                     indexator) = 0;

    /** Setter for the IHitIterator attribute. */
    virtual void setHitIterator      (algo::hits::IHitIterator*       iterator) = 0;

    /** Setter for the TimeInfo attribute. */
    virtual void setTimeStats        (os::impl::TimeInfo*             timeStats) = 0;

protected:

    /** Define the Hit iterator that will be used for building the alignments.
     *
     * This method creates the IHitIterator instance that will be iterated during the
     * PLAST algorithm. The created instance is likely to be a list of linked IHitIterator
     * instances, with a source IHitIterator instance given as a 'sourceHits' attribute.
     * For instance, the created instance should be a link of ungap -> small gap -> full gap
     * iterator.
     *
     * \param[in] config     : factory used for creating needed instances
     * \param[in] sourceHits : initial hits iterator (likely created by some IIndexator instance)
     * \param[in] ungapAlignResult : list of resulting ungap alignments (will be filled during algo execution)
     * \param[in] alignResult      : list of resulting gap alignments (will be filled during algo execution)
     * \result the IHitIterator instance to be used by the PLAST algorithm
     */
    virtual algo::hits::IHitIterator* createHitIterator (
        IConfiguration*                         config,
        algo::hits::IHitIterator*               sourceHits,
        alignment::core::IAlignmentContainer*   ungapAlignResult,
        alignment::core::IAlignmentContainer*   alignResult
    ) = 0;

    /** Define (for subject and query) the list of reading frames to be used. This is the way
     *  for differentiating the algorithm 'plastp', 'tplastn' and 'plastx'. For instance,
     *      'plastp' will return empty lists as result for the both method.
     *      'tplastn' should return a 6 frames list for getSubjectFrames and empty for the other
     *      'plastx'  should return a 6 frames list for getQueryFrames and empty for the other
     */
    virtual const std::vector<misc::ReadingFrame_e>&  getSubjectFrames () = 0;

    /** \copydoc getSubjectFrames */
    virtual const std::vector<misc::ReadingFrame_e>&  getQueryFrames   () = 0;

    /** */
    virtual void preTreatment (
        dp::Iterator<database::ISequenceDatabase*>* qryDatabases,
        dp::Iterator<database::ISequenceDatabase*>* sbjDatabases
    )  = 0;

    /** */
    virtual void postTreatment (
        dp::Iterator<database::ISequenceDatabase*>* qryDatabases,
        dp::Iterator<database::ISequenceDatabase*>* sbjDatabases
    ) = 0;
};

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/

#endif /* _IALGORITHM_HPP_ */
