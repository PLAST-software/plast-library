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

/** \file AlgoResultObserver.hpp
 *  \brief Define common class for target observer implementations.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ALGO_RESULT_OBSERVER_HPP
#define _ALGO_RESULT_OBSERVER_HPP

/********************************************************************************/

#include <designpattern/api/IProperty.hpp>

#include <launcher/observers/AbstractObserver.hpp>

#include <stdio.h>
#include <string>

/********************************************************************************/
/** \brief PLAST command line. */
namespace launcher {
/** \brief Observers definitions */
namespace observers {
/********************************************************************************/

/** \brief Observer that dumps algorithm properties.
 *
 * On AlgorithmReportEvent events reception, this observer builds a Properties instance
 * and configures it with the information in the AlgorithmReportEvent instance. Then,
 * the properties are visited through a visitor (provided to the constructor).
 *
 * This is useful to have statistics about the algorithm that just finished.
 * For instance, one can retrieve the number of hits having been filtered out during
 * the small gap algorithm, or any other information like this. It is therefore
 * useful for post analyzing the global behavior of the algorithm.
 */
class AlgoPropertiesObserver : public AbstractObserver
{
public:

    /** Constructor.
     * \param[in] visitor : visitor that will visit the properties holding algorithm information. */
     AlgoPropertiesObserver (dp::IPropertiesVisitor* visitor);

     /** Destructor. */
    virtual ~AlgoPropertiesObserver ();

    /** \copydoc AbstractObserver::update */
    void update (dp::EventInfo* evt, dp::ISubject* subject);

protected:

    /** Visitor to be used for visiting algorithm properties. */
    dp::IPropertiesVisitor* _visitor;

    /** Smart visitor for the \ref _visitor attribute. */
    void setVisitor (dp::IPropertiesVisitor* visitor)  { SP_SETATTR (visitor); }
};

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/

#endif /* _ALGO_RESULT_OBSERVER_HPP */
