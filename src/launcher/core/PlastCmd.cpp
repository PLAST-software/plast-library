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
#include <misc/api/types.hpp>
#include <algo/stats/api/IStatistics.hpp>
#include <algo/stats/impl/Statistics.hpp>
#include <algo/stats/impl/StatisticsPlastn.hpp>

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

using namespace statistics;
using namespace statistics::impl;

using namespace alignment::core;

using namespace launcher::observers;
using namespace misc;

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
    : _env(0), _properties(0), _isRunning(false), _isFinished(false)
{
    setProperties (properties);
    setEnv        (new DefaultEnvironment (properties, _isRunning));

    /** We subscribe ourself as listener. */
    _env->addObserver (this);

    _env->configure ();
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
    _isRunning  = true;
    _isFinished = false;

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

    _isRunning  = false;
    _isFinished = true;
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
        if (p != 0)  { bargraphSize = misc::atoi (p->value.c_str()); }

        string head = "algo";
        p = properties->getProperty(STR_OPTION_ALGO_TYPE);
        if (p != 0)  { head = p->value; }
        observers.push_back (new BargraphObserver (head, "seeds", stdout, bargraphSize));
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
    if (subject != this)
    {
        AlgorithmConfigurationEvent* e2 = dynamic_cast<AlgorithmConfigurationEvent*> (evt);
        if (e2 != 0  &&  e2->_current == e2->_total)  {   _isFinished = true;  }

        EnvironmentParameterEvent* e3 = dynamic_cast<EnvironmentParameterEvent*> (evt);
        if (e3 != 0)
        {
        	IParameters* params = e3->_params;
        	if (_properties != 0)
        	{
        		IGlobalParameters* result = 0;
        	    IProperty* algoProp = _properties->getProperty (STR_OPTION_ALGO_TYPE);
        	    IParameters* params2 = params->clone();LOCAL(params2);
        		if (algoProp == 0 ||  algoProp->getValue().compare ("plastn")!=0)
        			result = new GlobalParameters (params2,0);
        		else
            	    result = new GlobalParametersPlastn (params2,0);

        		LOCAL(result);
        		/** We retrieve the type of algorithm (may be not set). */
        		IProperties* paramsProps = params->getProperties();  LOCAL(paramsProps);

        	    if (_properties->getProperty(STR_OPTION_NB_PROCESSORS)==0)
        		{
        		    size_t nbProc = os::impl::DefaultFactory::thread().getNbCores();
        			_properties->add(0,STR_OPTION_NB_PROCESSORS,"%d",nbProc);
        		}

        		if (_properties->getProperty(STR_OPTION_OUTPUT_FILE)==0)
        			_properties->add(0,STR_OPTION_OUTPUT_FILE,params2->outputfile);

        		if (_properties->getProperty(STR_OPTION_EVALUE)==0)
        			_properties->add(0,STR_OPTION_EVALUE,paramsProps->getProperty(STR_PARAM_evalue)->getValue());

        		if (_properties->getProperty(STR_OPTION_FILTER_QUERY)==0)
        			_properties->add(0,STR_OPTION_FILTER_QUERY,"%s",params2->filterQuery?"T":"F");

        		if (_properties->getProperty(STR_OPTION_OPEN_GAP_COST)==0)
       				_properties->add(0,STR_OPTION_OPEN_GAP_COST,"%d",params2->openGapCost);

        		if (_properties->getProperty(STR_OPTION_EXTEND_GAP_COST)==0)
        			_properties->add(0,STR_OPTION_EXTEND_GAP_COST,"%d",params2->extendGapCost);

        		if (algoProp != 0  &&  algoProp->getValue().compare ("plastn")==0)
        		{
            		if (_properties->getProperty(STR_OPTION_X_DROPOFF_UNGAPPED)==0)
            			_properties->add(0,STR_OPTION_X_DROPOFF_UNGAPPED,paramsProps->getProperty(STR_PARAM_XdroppofUngap)->getValue());

            		if (_properties->getProperty(STR_OPTION_X_DROPOFF_GAPPED)==0)
            			_properties->add(0,STR_OPTION_X_DROPOFF_GAPPED,paramsProps->getProperty(STR_PARAM_XdroppofGap)->getValue());

            		if (_properties->getProperty(STR_OPTION_X_DROPOFF_FINAL)==0)
            			_properties->add(0,STR_OPTION_X_DROPOFF_FINAL,paramsProps->getProperty(STR_PARAM_finalXdroppofGap)->getValue());

        			if (_properties->getProperty(STR_OPTION_STRAND)==0)
					{
						if (params->strand==1)
							_properties->add(0,STR_OPTION_STRAND,"plus");
						else if (params->strand==-1)
							_properties->add(0,STR_OPTION_STRAND,"minus");
						else
							_properties->add(0,STR_OPTION_STRAND,"both");
					}

					if (_properties->getProperty(STR_OPTION_REWARD)==0)
						_properties->add(0,STR_OPTION_REWARD,"%d",params2->reward);

					if (_properties->getProperty(STR_OPTION_PENALTY)==0)
						_properties->add(0,STR_OPTION_PENALTY,"%d",params2->penalty);

					if (_properties->getProperty(STR_OPTION_INDEX_FILTER_SEED)==0)
	        			_properties->add(0,STR_OPTION_INDEX_FILTER_SEED,"%d",_properties->getProperty (STR_OPTION_ALGO_TYPE)->getInt());
        		}
        		else
        		{
            		if (_properties->getProperty(STR_OPTION_X_DROPOFF_GAPPED)==0)
            			_properties->add(0,STR_OPTION_X_DROPOFF_GAPPED,"%d",15);

            		if (_properties->getProperty(STR_OPTION_X_DROPOFF_FINAL)==0)
            			_properties->add(0,STR_OPTION_X_DROPOFF_FINAL,"%d",25);

            		if (_properties->getProperty(STR_OPTION_SCORE_MATRIX)==0)
            		{
            			switch (paramsProps->getProperty(STR_PARAM_matrixKind)->getInt())
            			{
            			case ENUM_BLOSUM62:
            				_properties->add(0,STR_OPTION_SCORE_MATRIX,"BLOSUM62");
            				break;
            			case ENUM_BLOSUM50:
            				_properties->add(0,STR_OPTION_SCORE_MATRIX,"BLOSUM50");
            				break;
            			case ENUM_NUCLEOTIDE_IDENTITY:
            				_properties->add(0,STR_OPTION_SCORE_MATRIX,"IDENTITY");
            				break;
            			case ENUM_NUCLEOTIDE_IDENTITY_BLAST:
            				_properties->add(0,STR_OPTION_SCORE_MATRIX,"IDENTITY_BLAST");
            				break;
            			}
            		}
            		if (_properties->getProperty(STR_OPTION_UNGAP_NEIGHBOUR_LENGTH)==0)
						_properties->add(0,STR_OPTION_UNGAP_NEIGHBOUR_LENGTH,"%d",params2->ungapNeighbourLength);

					if (_properties->getProperty(STR_OPTION_UNGAP_SCORE_THRESHOLD)==0)
						_properties->add(0,STR_OPTION_UNGAP_SCORE_THRESHOLD,"%d",params2->ungapScoreThreshold);

					if (_properties->getProperty(STR_OPTION_SMALLGAP_THRESHOLD)==0)
						_properties->add(0,STR_OPTION_SMALLGAP_THRESHOLD,"%d",params2->smallGapThreshold);

					if (_properties->getProperty(STR_OPTION_SMALLGAP_BAND_WITH)==0)
						_properties->add(0,STR_OPTION_SMALLGAP_BAND_WITH,"%d",params2->smallGapBandWidth);

					if (_properties->getProperty(STR_OPTION_SEEDS_USE_RATIO)==0)
	        			_properties->add(0,STR_OPTION_SEEDS_USE_RATIO,"%g",1.0);
        		}

        		if (_properties->getProperty(STR_OPTION_FORCE_QUERY_ORDERING)==0)
        			_properties->add(0,STR_OPTION_FORCE_QUERY_ORDERING,"%d",0);

        		if (_properties->getProperty(STR_OPTION_MAX_HIT_PER_QUERY)==0)
        			_properties->add(0,STR_OPTION_MAX_HIT_PER_QUERY,"%d",0);

        		if (_properties->getProperty(STR_OPTION_MAX_HSP_PER_HIT)==0)
        			_properties->add(0,STR_OPTION_MAX_HSP_PER_HIT,"%d",0);

        		if (_properties->getProperty(STR_OPTION_OUTPUT_FORMAT)==0)
        			_properties->add(0,STR_OPTION_OUTPUT_FORMAT,"%d",1);

        		if (_properties->getProperty(STR_OPTION_WORD_SIZE)==0)
        			_properties->add(0,STR_OPTION_WORD_SIZE,"%d",params2->seedSpan);
        	}
        }

        this->notify (evt);
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
void PlastCmd::cancel ()
{
    _isRunning = false;
}

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/
