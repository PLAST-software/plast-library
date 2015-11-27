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

#include <designpattern/api/Iterator.hpp>

#include <algo/core/api/IAlgoEvents.hpp>

#include <launcher/observers/AlgoExecutionObserver.hpp>

#include <string.h>

using namespace std;
using namespace dp;
using namespace algo::core;

/********************************************************************************/
namespace launcher {
namespace observers {
/********************************************************************************/

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
AlgoVerboseObserver::AlgoVerboseObserver (FILE* file)
    : _file(file)
{
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void AlgoVerboseObserver::update (dp::EventInfo* evt, dp::ISubject* subject)
{
    /** A little check. */
    if (_file == 0)  { return; }

    AlgoEventWithStatus* e2 = dynamic_cast<AlgoEventWithStatus*> (evt);

    if (e2 != 0  &&  e2->_status == ENUM_STARTED)
    {
        fprintf (_file, "\nstarting %s...", e2->_message.c_str());
        fflush  (_file);
        return;
    }

    if (e2 != 0  &&  e2->_status == ENUM_FINISHED)
    {
        return;
    }
}

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/
