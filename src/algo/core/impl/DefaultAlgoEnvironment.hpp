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

/** \file DefaultAlgoEnvironment.hpp
 *  \brief Default implementation of the IEnvironment interface
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _DEFAULT_ALGO_ENVIRONMENT_HPP_
#define _DEFAULT_ALGO_ENVIRONMENT_HPP_

/********************************************************************************/

#include <algo/core/api/IAlgoEnvironment.hpp>
#include <algo/core/api/IResultVisitorsFactory.hpp>

#include <os/impl/TimeTools.hpp>

/********************************************************************************/
namespace algo {
namespace core {
/** \brief Implementation of concepts for configuring and running PLAST. */
namespace impl {
/********************************************************************************/

/** \brief Default implementation of the IEnvironment interface
 *
 * This implementation first reads the two genomic databases. It allows to have an
 * idea if the databases need to be partionned for not consumming to much memory
 * (see buildUri()).
 *
 * Note that this (quick) reading of the subject and query databases allows also
 * to infer the kind of algorithm (plastp, plastx...) to be executed. This is a
 * small facility for end user (no need to use '-p' option).
 *
 * Defined as a Singleton (easier to use).
 */
class DefaultEnvironment : public IEnvironment
{
public:

    /** Constructor. */
    DefaultEnvironment (dp::IProperties* properties, bool& isRunning);

    /** Destructor. */
    virtual ~DefaultEnvironment ();

    /** \copydoc IEnvironment::createConfiguration */
    virtual IConfiguration* createConfiguration (dp::IProperties* properties);

    /** \copydoc IEnvironment::configure */
    void configure ();

    /** \copydoc IEnvironment::run */
    virtual void run ();

    /** */
    database::IDatabaseQuickReader* getQuickSubjectDbReader ()  { return _quickSubjectDbReader; }
    database::IDatabaseQuickReader* getQuickQueryDbReader   ()  { return _quickQueryDbReader;   }

protected:

    /** */
    dp::IProperties* _properties;
    void setProperties (dp::IProperties* properties)  { SP_SETATTR (properties); }

    /** */
    bool& _isRunning;

    /** */
    std::vector<IParameters*> _parametersList;

    IConfiguration* _config;
    virtual void setConfig (IConfiguration* config)  { SP_SETATTR(config); }

    alignment::filter::IAlignmentFilter* _filter;
    void setFilter (alignment::filter::IAlignmentFilter* filter)  { SP_SETATTR(filter); }

    database::IDatabaseQuickReader* _quickSubjectDbReader;
    void setQuickSubjectDbReader (database::IDatabaseQuickReader* quickSubjectDbReader)  { SP_SETATTR(quickSubjectDbReader); }

    database::IDatabaseQuickReader* _quickQueryDbReader;
    void setQuickQueryDbReader (database::IDatabaseQuickReader* quickQueryDbReader)  { SP_SETATTR(quickQueryDbReader); }

    alignment::core::IAlignmentContainerVisitor* _resultVisitor;
    void setResultVisitor (alignment::core::IAlignmentContainerVisitor* resultVisitor)  { SP_SETATTR(resultVisitor); }

    algo::core::IDatabasesProvider* _dbProvider;
    void setDatabasesProvider  (algo::core::IDatabasesProvider* dbProvider)  { SP_SETATTR(dbProvider); }

    os::impl::TimeInfo* _timeInfo;
    void setTimeInfo (os::impl::TimeInfo* timeInfo)  { SP_SETATTR(timeInfo); }

    os::impl::TimeInfo* _timeInfoAlgo;
    void setTimeInfoAlgo (os::impl::TimeInfo* timeInfoAlgo)  { SP_SETATTR(timeInfoAlgo); }

    /** \copydoc IEnvironment::createAlgorithm */
    std::list<IAlgorithm*> createAlgorithm (
        IConfiguration*                                 config,
        database::IDatabaseQuickReader*                 reader,
        IParameters*                                    params,
        alignment::filter::IAlignmentFilter*            filter,
        alignment::core::IAlignmentContainerVisitor*    resultVisitor,
        seed::ISeedModel*                               seedModel,
        algo::core::IDatabasesProvider*                 dbProvider,
        algo::core::IIndexator*                         indexator,
        statistics::IGlobalParameters*                  globalStats,
        os::impl::TimeInfo*                             timeInfo,
        bool&                                           isRunning
    );

    /** \copydoc IEnvironment::update */
    void update (dp::EventInfo* evt, dp::ISubject* subject);

    /** Define how database have to be partionned.
     * \param[in] subjectReader : information about subject database
     * \param[in] queryReader : information about query database
     * \return vector of offsets partitions for further databases reading
     */
    std::vector <std::pair <misc::Range64,misc::Range64> > buildUri (
        database::IDatabaseQuickReader* subjectReader,
        database::IDatabaseQuickReader* queryReader
    );

    /** Create IParameters instances that will be used for the algorithm customization.
     * \param[in] config : used for creating the parameters
     * \param[in] properties : used for parametrization
     * \param[out] uri : databases partitions
     * \return vector of created IParameters instances.
     */
    std::vector<IParameters*> createParametersList (
        IConfiguration* config,
        dp::IProperties* properties,
        std::vector <std::pair <misc::Range64,misc::Range64> >& uri
    );

    /** */
    void setSubjectBank (dp::IProperties* properties, u_int64_t  maxblocksize);

    virtual IConfiguration* getConfig();

    /** Finish writing the results to disk (perform result visitor flush) */
    virtual void flushResults();

    /** Obtain an instance of a factory for result visitor creation */
    virtual IResultVisitorsFactory* getResultsVisitorFactory();
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _DEFAULT_ALGO_ENVIRONMENT_HPP_ */
