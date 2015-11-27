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

/** \file AlgoHitsResultObserver.hpp
 *  \brief Define common class for target observer implementations.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ALGO_HITS_RESULT_OBSERVER_HPP
#define _ALGO_HITS_RESULT_OBSERVER_HPP

/********************************************************************************/

#include <designpattern/api/IProperty.hpp>

#include <launcher/observers/AbstractProgressionObserver.hpp>

#include <misc/api/types.hpp>

#include <os/impl/TimeTools.hpp>

#include <time.h>
#include <string>
#include <list>
#include <map>

/********************************************************************************/
/** \brief PLAST command line. */
namespace launcher {
/** \brief Observers definitions */
namespace observers {
/********************************************************************************/

/** \brief Observer that dumps statistics about hits iterators.
 *
 * On several events reception, this observer builds a Properties instance
 * according to the actual kind of notification information.
 *
 * Then the built properties accepts the properties visitor (provided at construction).
 * For instance, if the visitor is a file dump visitor, we have a mean to dump different
 * information about the algorithm execution while this latter is running. A good choice of
 * visitor is the dp::impl::XmlDumpPropertiesVisitor; with this implementation, we will get
 * at the end of the PLAST execution an XML file gathering many information about the PLAST
 * execution.
 *
 * By now, the kind of supported notifications are:
 *  - algo::core::AlgorithmReportEvent
 *  - algo::core::AlgorithmConfigurationEvent
 *  - algo::core::DatabasesInformationEvent
 */
class AlgoHitsPropertiesObserver : public AbstractProgressionObserver
{
public:

    /** Constructor.
     * \param[in] visitor : visitor that will visit the properties holding algorithm information. */
    AlgoHitsPropertiesObserver (dp::IPropertiesVisitor* visitor);

     /** Destructor. */
    virtual ~AlgoHitsPropertiesObserver ();

    /** \copydoc AbstractObserver::update */
    void update (dp::EventInfo* evt, dp::ISubject* subject);

protected:

    /** Visitor to be used for visiting algorithm properties. */
    dp::IPropertiesVisitor* _visitor;

    /** */
    u_int32_t _t0;
    u_int32_t _t1;

    time_t _time0;
    time_t _time1;

    dp::IProperties* _timeProps;
    void setTimeProps (dp::IProperties* timeProps)  { SP_SETATTR(timeProps); }

    /** Smart visitor for the \ref _visitor attribute. */
    void setVisitor (dp::IPropertiesVisitor* visitor)  { SP_SETATTR (visitor); }

    /** Null implementation of dump method. */
    void dump (void) {}

    /** List of names of the IHitIterator instances. */
    std::list<std::string>           _names;

    /** Map associating IHitIterator names with the output hits number. */
    std::map<std::string, u_int64_t> _theMap;

    /** Creates and visit a IProperties with miscellaneous information (tool name, version...)
     */
    void fillMiscInfo ();

    /** Fills a provided IProperties with information related with a database.
     * \param[in] db : a quick reader holding information about the database
     * \param[in] props : the IProperties instance to be filled
     * \param[in] name  : name of the node to be added to the provided IProperties instance
     * \param[in] depth : depth of the node to be added to the provided IProperties instance
     */
    void fillPropsFromDbInfo (database::IDatabaseQuickReader* db, dp::IProperties* props, const std::string& name, size_t depth);

    /** Extract some property instances from a IProperties for copying them in the destination IProperties instance.
     * \param[in] props : the source properties
     * \param[in] destination : the destination properties
     * \param[in] depth : the depth of the nodes copied in destination
     */
    void fillParamsInfo (dp::IProperties* props, dp::IProperties& destination, size_t depth);
};

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/

#endif /* _ALGO_HITS_RESULT_OBSERVER_HPP */
