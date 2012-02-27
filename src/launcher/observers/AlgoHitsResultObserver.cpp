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

#include <misc/api/PlastStrings.hpp>
#include <misc/api/version.hpp>

#include <designpattern/api/Iterator.hpp>
#include <designpattern/impl/Property.hpp>

#include <algo/core/api/IAlgoEvents.hpp>
#include <algo/core/api/IAlgoEnvironment.hpp>

#include <database/api/IDatabaseQuickReader.hpp>

#include <os/api/ITime.hpp>
#include <os/impl/DefaultOsFactory.hpp>

#include <launcher/observers/AlgoHitsResultObserver.hpp>

#include <stdio.h>
#include <string.h>
#include <stack>

using namespace std;
using namespace os;
using namespace os::impl;
using namespace dp;
using namespace dp::impl;
using namespace database;
using namespace algo::core;
using namespace algo::hits;
using namespace alignment::core;

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
AlgoHitsPropertiesObserver::AlgoHitsPropertiesObserver (dp::IPropertiesVisitor* visitor)
    : _visitor (0), _t0(0), _t1(0)
{
    /** We get a reference on the provided visitor. */
    setVisitor (visitor);

    /** We get the current time. */
    _t0    = DefaultFactory::time().gettime();
    time (&_time0);
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
AlgoHitsPropertiesObserver::~AlgoHitsPropertiesObserver ()
{
    /** We add some basic properties and visit them.*/
    fillMiscInfo ();

    setVisitor (0);
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void AlgoHitsPropertiesObserver::update (dp::EventInfo* evt, dp::ISubject* subject)
{
    /** We first call the parent method. */
    AbstractProgressionObserver::update (evt, subject);

    /** We cast the received event in the type we are interested in. */
    AlgorithmReportEvent* e1 = dynamic_cast<AlgorithmReportEvent*> (evt);
    if (e1 != 0)
    {
        if (e1->getAlgo() != 0)
        {
            /** At first call, we have to keep the names (the order actually). */
            if (_names.empty())
            {
                for (IHitIterator* it = e1->getAlgo()->getHitIterator(); it != 0; it = it->getSourceIterator())
                {
                    _names.push_front (it->getName());
                }
            }

            /** We loop the chain of iterators. */
            for (IHitIterator* it = e1->getAlgo()->getHitIterator(); it != 0; it = it->getSourceIterator())
            {
                /** We increase the number of outgoing hits from this iterator. */
                _theMap [it->getName()] += it->getOutputHitsNumber();
            }

			if (e1->getAlignmentFilter() != 0)
			{
	            /** We define the root of our properties. */
	            Properties props;
	            props.add (0, "filters", e1->getAlignmentFilter()->getTitle());
	            props.add (1,e1->getAlignmentFilter()->getProperties());

	            /** We accept the visitor. */
	            props.accept (_visitor);
	        }
        }

        return;
    }

    AlgorithmConfigurationEvent* e2 = dynamic_cast<AlgorithmConfigurationEvent*> (evt);
    if (e2 != 0)
    {
        if (e2->_current == e2->_total)
        {
            /** We get properties. */
            IProperties* currentProps = e2->_props;
            LOCAL (currentProps);

            /** We get the current time. */
            _t1 = DefaultFactory::time().gettime();
            time (&_time1);

            /** We get the result file size. Note mode "rb" important to get the real size. */
            IProperty* prop = currentProps->getProperty(STR_OPTION_OUTPUT_FILE);
            u_int64_t outputSize = 0;
            if (prop)
            {
                IFile* outputFile = DefaultFactory::file().newFile (prop->getString(), "rb", false);
                if (outputFile)
                {
                    outputSize = outputFile->getSize();
                    delete outputFile;
                }
            }

            /** We get the database segmentation size (if any). */
            u_int32_t dbSegment = 0;
            prop = currentProps->getProperty(STR_OPTION_MAX_DATABASE_SIZE);
            if (prop)  { dbSegment = prop->getInt(); }

            /** We get the number of cores. */
            int nbCores = -1;
            prop = currentProps->getProperty(STR_OPTION_NB_PROCESSORS);
            if (prop)  { nbCores = prop->getInt(); }

            /** We define the root of our properties. */
            Properties props;

            props.add (0, "parameters");
            fillParamsInfo (currentProps, props, 1);

            props.add (0, "resources");
            props.add (1, "exec_time",          getTimeString (_t1-_t0));
            props.add (1, "nb_cores",           "%d",           nbCores);
            props.add (1, "total_memory",       "%.1f Gb",      (float)_totalUsedMemory / 1024.0 / 1024.0);
            props.add (1, "max_memory",         "%.1f Mb",      (float)_maxUsedMemory / 1024.0);
            props.add (1, "database_split",     "%ld segment(s) of block size %ld bytes", e2->_total, dbSegment);
            props.add (1, "output_file",        "%lld bytes",   outputSize);

            props.add (0, "hits");

            /** We can now dump the aggregated information. */
            for (list<string>::iterator it = _names.begin(); it != _names.end(); it++)
            {
                map<string, u_int64_t>::iterator look = _theMap.find (*it);
                if (look != _theMap.end())
                {
                    props.add (1, look->first, "%lld" , look->second);

                }
            }
            props.add (1, "FinalAlignments", "%lld", _nbAlignments);

            /** We accept the visitor. */
            props.accept (_visitor);
        }

        return;
    }

    DatabasesInformationEvent* e3 = dynamic_cast<DatabasesInformationEvent*> (evt);
    if (e3 != 0)
    {
        /** We define the root of our properties. */
        Properties props;
        props.add (0, "databases");

        fillPropsFromDbInfo (e3->_subjectDb, &props, "subject", 1);
        fillPropsFromDbInfo (e3->_queryDb,   &props, "query",   1);

        /** We accept the visitor. */
        props.accept (_visitor);

        return;
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
void AlgoHitsPropertiesObserver::fillMiscInfo  ()
{
    struct tm* date = 0;

    /** We create a properties object with several information. */
    Properties props;
    props.add (0, "misc");

    props.add (1, "tool",     PLAST_NAME);
    props.add (1, "version",  PLAST_VERSION);

    if ( (date = localtime (&_time0)) != 0)
    {
        props.add (1, "date_start",     "%02d/%02d/%02d  %02d:%02d:%02d",
            1900+date->tm_year, 1+date->tm_mon, date->tm_mday,
            date->tm_hour, date->tm_min, date->tm_sec
        );
    }

    if ( (date = localtime (&_time1)) != 0)
    {
        props.add (1, "date_stop",     "%02d/%02d/%02d  %02d:%02d:%02d",
            1900+date->tm_year, 1+date->tm_mon, date->tm_mday,
            date->tm_hour, date->tm_min, date->tm_sec
        );
    }

    /** We visit the properties .*/
    props.accept (_visitor);
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void AlgoHitsPropertiesObserver::fillPropsFromDbInfo  (IDatabaseQuickReader* db, IProperties* props, const string& name, size_t depth)
{
    string kind;

    switch (db->getKind())
    {
        case IDatabaseQuickReader::ENUM_NUCLOTID:       kind = "nucleotid";  break;
        case IDatabaseQuickReader::ENUM_AMINO_ACID:     kind = "amino acid"; break;
        default:                                        kind = "unknown";    break;
    };

    props->add (depth + 0, name);
    props->add (depth + 1, "kind", kind);
    props->add (depth + 1, "uri",  db->getUri());
    props->add (depth + 1, "total_size",   "%lld", db->getTotalSize());
    props->add (depth + 1, "data_size",    "%lld", db->getDataSize());
    props->add (depth + 1, "nb_sequences", "%ld",  db->getNbSequences());
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
void AlgoHitsPropertiesObserver::fillParamsInfo (IProperties* props, IProperties& destination, size_t depth)
{
    const char* table[] =
    {
//        STR_OPTION_OUTPUT_FILE,
        STR_OPTION_EVALUE,
        STR_OPTION_UNGAP_NEIGHBOUR_LENGTH,
        STR_OPTION_UNGAP_SCORE_THRESHOLD,
        STR_OPTION_SMALLGAP_THRESHOLD,
        STR_OPTION_NB_PROCESSORS,
        STR_OPTION_OPEN_GAP_COST,
        STR_OPTION_OPEN_GAP_COST,
        STR_OPTION_X_DROPOFF_GAPPED,
        STR_OPTION_X_DROPOFF_FINAL,
        STR_OPTION_FILTER_QUERY,
        STR_OPTION_SCORE_MATRIX,
//        STR_OPTION_OUTPUT_FORMAT,
        STR_OPTION_MAX_DATABASE_SIZE,
        STR_OPTION_MAX_HIT_PER_QUERY,
        STR_OPTION_MAX_HIT_PER_ITERATION,
        STR_OPTION_STRANDS_LIST,
        STR_OPTION_CODON_STOP_OPTIM,
//        STR_OPTION_FACTORY_DISPATCHER,
//        STR_OPTION_FACTORY_INDEXATION,
//        STR_OPTION_FACTORY_HIT_UNGAP,
//        STR_OPTION_FACTORY_HIT_SMALLGAP,
//        STR_OPTION_FACTORY_HIT_FULLGAP,
//        STR_OPTION_FACTORY_HIT_COMPOSITION,
//        STR_OPTION_FACTORY_GAP_RESULT,
//        STR_OPTION_FACTORY_UNGAP_RESULT,
//        STR_OPTION_OPTIM_FILTER_UNGAP,
//        STR_OPTION_INFO_BARGRAPH,
//        STR_OPTION_INFO_BARGRAPH_SIZE,
//        STR_OPTION_INFO_VERBOSE,
//        STR_OPTION_INFO_FULL_STATS,
//        STR_OPTION_INFO_STATS,
//        STR_OPTION_INFO_STATS_FORMAT,
//        STR_OPTION_INFO_STATS_AUTO,
//        STR_OPTION_INFO_PROGRESSION,
//        STR_OPTION_INFO_ALIGNMENT_PROGRESS,
//        STR_OPTION_INFO_CONFIG_FILE,
        STR_OPTION_XML_FILTER_FILE
    };

    IProperty* prop = 0;

    for (size_t i=0; i<ARRAYSIZE(table); i++)
    {
        prop = props->getProperty (table[i]);

        if (prop != 0)  {  destination.add (depth, table[i], prop->getValue());  }
    }
}

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/
