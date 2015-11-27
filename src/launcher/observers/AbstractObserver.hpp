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

/** \file AbstractObserver.hpp
 *  \brief Define common class for target observer implementations.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ABSTRACT_OBSERVER_HPP
#define _ABSTRACT_OBSERVER_HPP

/********************************************************************************/

#include <designpattern/impl/Observer.hpp>

/********************************************************************************/
/** \brief PLAST command line. */
namespace launcher {
/** \brief Observers definitions */
namespace observers {
/********************************************************************************/

/** \brief  Define common class for target observer implementations
 *
 * We just assemble the dp::IObserver and the dp::SmartPointer interfaces as skeleton
 * for the concrete observers we want to implement.
 */
class AbstractObserver : public dp::IObserver, public dp::SmartPointer
{
public:

    /** Method called by Subject for some notification.
     *  \param[in] evt     : specific information related to the notification.
     *  \param[in] subject : the subject (ie. the caller of the 'update' method).
     */
    virtual void update (dp::EventInfo* evt, dp::ISubject* subject) = 0;
};

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/

#endif /* _ABSTRACT_OBSERVER_HPP */
