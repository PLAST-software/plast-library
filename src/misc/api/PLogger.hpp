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

/** \file PLogger.hpp
 *  \brief Simple Logger gateway to log4cpp library.
 *  \date 22/04/2016
 *  \author pdurand
 *
 * We define here a simple set of DEFINEs to use log4cpp loggers easily
 *
 *
 */
#ifndef PLOGGER
#define PLOGGER

/********************************************************************************/
/** \brief Miscellaneous definitions */
namespace misc {
/********************************************************************************/
namespace PLogger {

    void initialize();
    void debug(const char* message);

/********************************************************************************/
}}  /* end of namespaces. */
/********************************************************************************/


#endif // PLOGGER

