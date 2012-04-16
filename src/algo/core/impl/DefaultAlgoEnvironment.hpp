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

/** \file DefaultAlgoEnvironment.hpp
 *  \brief Default implementation of the IEnvironment interface
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _DEFAULT_ALGO_ENVIRONMENT_HPP_
#define _DEFAULT_ALGO_ENVIRONMENT_HPP_

/********************************************************************************/

#include <algo/core/api/IAlgoEnvironment.hpp>

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
    IConfiguration* createConfiguration (dp::IProperties* properties);

    /** \copydoc IEnvironment::run */
    void run ();

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
    void configure ();

    /** */
    std::vector<IParameters*> _parametersList;

    IConfiguration* _config;
    void setConfig (IConfiguration* config)  { SP_SETATTR(config); }

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

    /** \copydoc IEnvironment::createAlgorithm */
    IAlgorithm* createAlgorithm (
        IConfiguration*                                 config,
        database::IDatabaseQuickReader*                 reader,
        IParameters*                                    params,
        alignment::filter::IAlignmentFilter*            filter,
        alignment::core::IAlignmentContainerVisitor*    resultVisitor,
        algo::core::IDatabasesProvider*                 dbProvider,
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
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _DEFAULT_ALGO_ENVIRONMENT_HPP_ */
