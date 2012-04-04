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

#include <launcher/core/PlastCmd.hpp>
#include <misc/api/PlastStrings.hpp>
#include <designpattern/impl/Property.hpp>

#include <launcher/observers/BargraphObserver.hpp>
#include <launcher/observers/FileProgressionObserver.hpp>
#include <launcher/observers/AlgoExecutionObserver.hpp>
#include <launcher/observers/AlgoResultObserver.hpp>
#include <launcher/observers/AlgoHitsResultObserver.hpp>

#include <algo/core/api/IAlgoEvents.hpp>
#include <algo/core/impl/DefaultAlgoEnvironment.hpp>

using namespace std;

using namespace dp;
using namespace dp::impl;

using namespace algo::core;
using namespace algo::core::impl;

using namespace alignment::core;

using namespace launcher::observers;

/********************************************************************************/
namespace launcher {
namespace core     {
/********************************************************************************/

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
PlastCmd::PlastCmd (IProperties* properties)
    : _env(0), _properties(0)
{
    setEnv        (new DefaultEnvironment (properties, _isRunning));
    setProperties (properties);
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
PlastCmd::~PlastCmd ()
{
    setEnv        (0);
    setProperties (0);
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void PlastCmd::execute ()
{
    _isRunning = true;

    /** We subscribe ourself as listener. */
    _env->addObserver (this);

    /** We may have to configure some observers according to the options provided by the user. */
    list<AbstractObserver*> observers;
    configureObservers (_properties, observers);

    /** We may attach listeners to the algo environment. */
    for (list<AbstractObserver*>::iterator it = observers.begin(); it != observers.end(); it++)
    {
        _env->addObserver (*it);
    }

    /** We run the algorithm through the environment instance. */
    _env->run ();

    /** We may remove listeners from the algo environment. */
    for (list<AbstractObserver*>::iterator it = observers.begin(); it != observers.end(); it++)
    {
        _env->removeObserver (*it);
        delete *it;
    }

    /** We unsubscribe ourself as listener. */
    _env->removeObserver (this);

    _isRunning = false;
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void PlastCmd::configureObservers (
    dp::IProperties* properties,
    list<AbstractObserver*>& observers
)
{
    IProperty* prop = 0;

    /** We may want to have a progress bar displayed in the console. */
    if ( (prop = properties->getProperty(STR_OPTION_INFO_BARGRAPH)) != 0)
    {
        size_t bargraphSize = 0;
        IProperty* p = properties->getProperty(STR_OPTION_INFO_BARGRAPH_SIZE);
        if (p != 0)  { bargraphSize = atoi (p->value.c_str()); }

        observers.push_back (new BargraphObserver ("seeds", stdout, bargraphSize));
    }

    /** We may want to have dynamic information during algorithm execution. */
    if ( (prop = properties->getProperty(STR_OPTION_INFO_VERBOSE)) != 0 )   { observers.push_back (new AlgoVerboseObserver ());    }

    if ( (prop = properties->getProperty(STR_OPTION_INFO_FULL_STATS)) != 0)
    {
        /** We create a properties visitor that will dump properties into an XML file. */
        IPropertiesVisitor* visitor = getPropertiesVisitor(properties, prop->value);

        /** We create an observer that is notified on algorithm result. Note that we will visit the
         *  notified information through our XML visitor. */
        observers.push_back (new AlgoPropertiesObserver (visitor));
    }

    /** We look for a 'stats' option. If none, we add it with a default name. */
    if (   properties->getProperty (STR_OPTION_INFO_STATS)      == 0
       &&  properties->getProperty (STR_OPTION_INFO_STATS_AUTO) != 0 )
    {
        time_t timet;
        time (&timet);
        struct tm* date = localtime (&timet);

        properties->add (0, STR_OPTION_INFO_STATS, ".stats_%02d%02d%02d_%02d%02d%02d",
            1900+date->tm_year, 1+date->tm_mon, date->tm_mday, date->tm_hour, date->tm_min, date->tm_sec
        );
    }

    if ( (prop = properties->getProperty(STR_OPTION_INFO_STATS)) != 0)
    {
        /** We create a properties visitor that will dump properties into an XML file. */
        IPropertiesVisitor* visitor = getPropertiesVisitor(properties, prop->value);

        /** We create an observer that is notified on algorithm result. Note that we will visit the
         *  notified information through our XML visitor. */
        observers.push_back (new AlgoHitsPropertiesObserver (visitor));
    }

    /** We may want to have dynamic information during algorithm execution. */
    if ( (prop = properties->getProperty(STR_OPTION_INFO_PROGRESSION)) != 0)
    {
        observers.push_back (new FileProgressionObserver (prop->value) );
    }

    /** We may want to have dynamic information during algorithm execution. */
    if ( (prop = properties->getProperty(STR_OPTION_INFO_ALIGNMENT_PROGRESS)) != 0)
    {
        observers.push_back (new AlignmentProgressionObserver (prop->value) );
    }

    if ( (prop = properties->getProperty(STR_OPTION_INFO_RESOURCES_PROGRESS)) != 0)
    {
        observers.push_back (new ResourcesObserver (prop->value) );
    }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IPropertiesVisitor* PlastCmd::getPropertiesVisitor (IProperties* props, const string& filename)
{
    IPropertiesVisitor* result = 0;

    IProperty* statsFmtProp = props->getProperty (STR_OPTION_INFO_STATS_FORMAT);

    if (statsFmtProp == 0)
    {
        result = new RawDumpPropertiesVisitor (filename);
    }
    else
    {
        string fmt = statsFmtProp->getValue();

        if (fmt.compare("xml") == 0)
        {
            result = new XmlDumpPropertiesVisitor (filename);
        }
        else if (fmt.compare("raw") == 0)
        {
            result = new RawDumpPropertiesVisitor (filename);
        }
        else
        {
            result = new RawDumpPropertiesVisitor (filename);
        }
    }

    return result;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void PlastCmd::update (dp::EventInfo* evt, dp::ISubject* subject)
{
    /** We just forward the notification to potential listener. */
    if (subject != this  &&  _isRunning==true)  {  notify (evt);  }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void PlastCmd::cancel ()
{
    _isRunning = false;
}

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/
