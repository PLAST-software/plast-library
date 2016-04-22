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

  /**
    * Return a logger given its name.
    */
  log4cpp::Category& getLoggerByName(const char* loggerName);

/********************************************************************************/
}}  /* end of namespaces. */
/********************************************************************************/
#define ROOT_LOGGER_NAME "root"

#define LOG_DEBUG(loggerName, msg) { \
    if (misc::PLogger::isInitialized()){ \
      log4cpp::Category& logger = misc::PLogger::getLoggerByName(loggerName); \
      if(logger.getPriority()>=log4cpp::Priority::DEBUG){ \
        std::string fileName = std::string(__FILE__); \
        std::size_t start = fileName.find_last_of(FILE_SEPARATOR); \
        logger << log4cpp::Priority::DEBUG << fileName.substr(start+1) << ":" << __func__ << ":" << __LINE__ << ": " << msg ; \
      } \
    } \
  }

#define LOG_INFO(loggerName, msg) { \
    if (misc::PLogger::isInitialized()){ \
      log4cpp::Category& logger = misc::PLogger::getLoggerByName(loggerName); \
      if(logger.getPriority()>=log4cpp::Priority::INFO){ \
        logger.info(msg); \
      } \
    } \
  }

#define LOG_WARN(loggerName, msg) { \
    if (misc::PLogger::isInitialized()){ \
      log4cpp::Category& logger = misc::PLogger::getLoggerByName(loggerName); \
      if(logger.getPriority()>=log4cpp::Priority::WARN){ \
        logger.info(msg); \
      } \
    } \
  }

#define LOG_ERROR(loggerName, msg) { \
    if (misc::PLogger::isInitialized()){ \
      log4cpp::Category& logger = misc::PLogger::getLoggerByName(loggerName); \
      if(logger.getPriority()>=log4cpp::Priority::ERROR){ \
        logger.info(msg); \
      } \
    } \
  }

#endif // PLOGGER

