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

#ifndef _IALGO_ENVIRONMENT_HPP_
#define _IALGO_ENVIRONMENT_HPP_

/********************************************************************************/

#include "SmartPointer.hpp"
#include "Observer.hpp"

#include "IAlgoParameters.hpp"
#include "IAlgoConfig.hpp"
#include "IAlgorithm.hpp"
#include "IProperty.hpp"
#include "IDatabaseQuickReader.hpp"

#include <string>
#include <list>
#include <vector>

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/** High level interface for configuring/launching the plast algorithm.
 *  Implementations of these interface are likely to be present in the 'main' function
 *  of the program that wants to plast something.
 */
class IEnvironment : public dp::SmartPointer, public dp::Subject, public dp::IObserver
{
public:

    /** Create a configuration instance from some properties (gathered by command line
     *  options or by configuration file for instance).
     */
    virtual IConfiguration* createConfiguration (dp::IProperties* properties) = 0;

    /** Entry point method for plasting (given some properties).
     */
    virtual void run (dp::IProperties* properties) = 0;

protected:

    /** Factory method for creating a IAlgorithm instance. Actually, a user request
     * (through the 'IEnvironment::run' method) may build several IAlgorithm creation
     * through this factory method; it should happen for instance when subject and/or
     * query databases are very big an need to be segmented => they are then segmented
     * and each segment is used as entry for a specific algorithm instance.
     */
    virtual IAlgorithm* createAlgorithm (
        IConfiguration* config,
        database::IDatabaseQuickReader* reader,
        IParameters* params,
        AlignmentResultVisitor* resultVisitor
    ) = 0;
};

/********************************************************************************/

class AlgorithmConfigurationEvent : public dp::EventInfo
{
public:
    AlgorithmConfigurationEvent (size_t current, size_t total)
    :  dp::EventInfo(0), _current(current), _total(total) {}

    size_t _current;
    size_t _total;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _IALGO_ENVIRONMENT_HPP_ */
