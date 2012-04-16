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

/** \file IAlgoEnvironment.hpp
 *  \brief Define high level concepts for configuring and running PLAST algorithm.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _IALGO_ENVIRONMENT_HPP_
#define _IALGO_ENVIRONMENT_HPP_

/********************************************************************************/

#include <designpattern/api/SmartPointer.hpp>
#include <designpattern/api/IObserver.hpp>
#include <designpattern/api/IProperty.hpp>

#include <database/api/IDatabaseQuickReader.hpp>

#include <algo/core/api/IAlgoParameters.hpp>
#include <algo/core/api/IAlgoConfig.hpp>
#include <algo/core/api/IAlgorithm.hpp>

#include <string>
#include <list>
#include <vector>

/********************************************************************************/
/** \brief PLAST algorithm concepts. */
namespace algo {
/** \brief Concept for configuring and running PLAST. */
namespace core {
/********************************************************************************/

/** \brief High level interface for configuring/launching the plast algorithm
 *
 *  This interface provides an entry point for running PLAST, given some properties
 *  (coming from the command line or from a configuration file)
 *
 *  An important feature of this interface: it is both an observer and a subject:
 *      - it receives a lot of notifications sent by instances of inner components of PLAST.
 *      - it sends back these notifications to higher-level listeners (ie. the end-user)
 *
 *  The IEnvironment interface therefore centralizes many information about the algorithm
 *  execution (progression, alignments number, ellapsed time, ...) and can notifies them
 *  to the end user.
 *
 *  Code sample:
 *  \code
 *  void sample (IProperties* properties)
 *  {
 *      // we create a new environment instance
 *      IEnvironment* env = new ConcreteEnvironment ();
 *
 *      // we create an observer for receiving information about algorithm execution.
 *      IObserver* myObserver = new MyPlastObserver();
 *
 *      // we add this observer to this instance.
 *      env->addObserver (myObserver);
 *
 *      // we lauch PLAST with the provided properties (coming from command line interface for instance)
 *      env->run (properties);
 *
 *      // now, PLAST should be running, we are waiting here until it is finished.
 *      // the observer may receive some information during the running.
 *
 *      // we remove the observer
 *      env->removeObserver (myObserver);
 *  }
 *  \endcode
 */
class IEnvironment : public dp::SmartPointer, public dp::impl::Subject, public dp::IObserver
{
public:

    /** Entry point method for plasting (given some properties).
     */
    virtual void run () = 0;

    /** Getters. */
    virtual database::IDatabaseQuickReader* getQuickSubjectDbReader () = 0;
    virtual database::IDatabaseQuickReader* getQuickQueryDbReader   () = 0;

protected:

    /** Create a configuration instance from some properties (gathered by command line
     *  options or by configuration file for instance).
     *  \param[in] properties : the properties used for configuring the result
     *  \return a new IConfiguration instance
     */
    virtual IConfiguration* createConfiguration (dp::IProperties* properties) = 0;

    /** Factory method for creating a IAlgorithm instance. Actually, a user request
     * (through the run method) may build several IAlgorithm creation
     * through this factory method; it should happen for instance when subject and/or
     * query databases are very big an need to be segmented => they are then segmented
     * and each segment is used as entry for a specific algorithm instance.
     * \param[in] config : configuration instance
     * \param[in] reader : supposed to represent the read subject database
     * \param[in] params : parameters for configuring the algorithm
     * \param[in] resultVisitor : the visitor that visits all found alignments during algo execution
     * \return a new IAlgorithm instance
     */
    virtual IAlgorithm* createAlgorithm (
        IConfiguration*                                 config,
        database::IDatabaseQuickReader*                 reader,
        IParameters*                                    params,
        alignment::filter::IAlignmentFilter*            filter,
        alignment::core::IAlignmentContainerVisitor*    resultVisitor,
        algo::core::IDatabasesProvider*                 dbProvider,
        bool&                                           isRunning
    ) = 0;
};

/********************************************************************************/

/** \brief Provides algorithm progression information
 */
class AlgorithmConfigurationEvent : public dp::EventInfo
{
public:
    /** Constructor.
     * \param[in] current
     * \param[in] total
     */
    AlgorithmConfigurationEvent (dp::IProperties* props, size_t current, size_t total)
    :  dp::EventInfo(0), _props(0), _current(current), _total(total) {  setProps(props); }

    /** */
    ~AlgorithmConfigurationEvent ()  { setProps(0); }

    /** */
    dp::IProperties* _props;

    /** Index of the current running algorithm part. */
    size_t _current;

    /** Number of algorithm parts. */
    size_t _total;

private:
    void setProps (dp::IProperties* props)  { SP_SETATTR (props); }
};

/********************************************************************************/

/** \brief Provides information about the two compared databases.
 *
 * This class provides notification information about the subject and query databases.
 */
class DatabasesInformationEvent : public dp::EventInfo
{
public:

    /** Constructor.
     * \param[in] subjectDb : subject database information
     * \param[in] queryDb   : query database information
     * */
    DatabasesInformationEvent (
        database::IDatabaseQuickReader* subjectDb,
        database::IDatabaseQuickReader* queryDb
    )
    :  dp::EventInfo(0), _subjectDb(subjectDb), _queryDb(queryDb)  {}

    /** Subject database */
    database::IDatabaseQuickReader* _subjectDb;

    /** Query database */
    database::IDatabaseQuickReader* _queryDb;
};

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/

#endif /* _IALGO_ENVIRONMENT_HPP_ */
