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
 * On AlgorithmReportEvent events reception, this observer builds a Properties instance
 * and configures it with the information in the AlgorithmReportEvent instance.
 *
 * In particular, it keeps hits iterators information (input/output hits) and aggregate
 * results numbers.
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

    /** Smart visitor for the \ref _visitor attribute. */
    void setVisitor (dp::IPropertiesVisitor* visitor)  { SP_SETATTR (visitor); }

    /** */
    void dump (void) {}

    /** */
    std::list<std::string>           _names;
    std::map<std::string, u_int64_t> _theMap;

    /** */
    void fillMiscInfo ();

    /** */
    void fillPropsFromDbInfo (database::IDatabaseQuickReader* db, dp::IProperties* props, const std::string& name, size_t depth);

    /** */
    void fillParamsInfo (dp::IProperties* props, dp::IProperties& destination, size_t depth);
};

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/

#endif /* _ALGO_HITS_RESULT_OBSERVER_HPP */
