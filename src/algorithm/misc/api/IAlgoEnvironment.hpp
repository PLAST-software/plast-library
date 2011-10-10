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

class IEnvironment : public dp::SmartPointer, public dp::Subject, public dp::IObserver
{
public:

    /** */
    virtual IConfiguration* createConfiguration (dp::IProperties* properties) = 0;

    /** */
    virtual void run (dp::IProperties* properties) = 0;

protected:

    /** */
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
