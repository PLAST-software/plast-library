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

#include <string>

#include <os/api/IFile.hpp>

#include <log4cpp/Category.hh>
#include <log4cpp/Priority.hh>

/********************************************************************************/
/** \brief Miscellaneous definitions */
namespace misc {
/********************************************************************************/
namespace PLogger {

  /**
    * Initilize the logger system. To be called once at application startup. If not
    * called, then no log traces of any kind will be produced.
    */
  void initialize(log4cpp::Priority::Value priority);

  /**
    * Figure out wheter or not the logger system has been initialized.
    */
  bool isInitialized();

/********************************************************************************/
}}  /* end of namespaces. */
/********************************************************************************/

#define LOG_DEBUG_S(msg) { \
    log4cpp::Category& rootLogger = log4cpp::Category::getRoot(); \
    if (misc::PLogger::isInitialized() && rootLogger.getPriority()>=log4cpp::Priority::DEBUG){ \
        std::string fileName = std::string(__FILE__); \
        std::size_t start = fileName.find_last_of(FILE_SEPARATOR); \
        rootLogger << log4cpp::Priority::DEBUG << fileName.substr(start+1) << ":" << __func__ << ":" << __LINE__ << ": " << msg ; \
    } \
  }

#define LOG_DEBUG(logger, msg) { \
    if (misc::PLogger::isInitialized() && logger.getPriority()>=log4cpp::Priority::DEBUG){ \
        std::string fileName = std::string(__FILE__); \
        std::size_t start = fileName.find_last_of(FILE_SEPARATOR); \
        logger << log4cpp::Priority::DEBUG << fileName.substr(start+1) << ":" << __func__ << ":" << __LINE__ << ": " << msg ; \
    } \
  }

#define LOG_INFO(logger, msg) { \
    if (misc::PLogger::isInitialized() && logger.getPriority()>=log4cpp::Priority::INFO){ \
        logger.info(msg); \
    } \
  }

#endif // PLOGGER

