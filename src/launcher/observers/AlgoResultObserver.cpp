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

#include <os/api/ITime.hpp>

#include <designpattern/api/Iterator.hpp>
#include <designpattern/impl/Property.hpp>

#include <algo/core/api/IAlgoEvents.hpp>

#include <launcher/observers/AlgoResultObserver.hpp>

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
AlgoPropertiesObserver::AlgoPropertiesObserver (dp::IPropertiesVisitor* visitor)
    : _visitor (0)
{
    setVisitor (visitor);
}

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
AlgoPropertiesObserver::~AlgoPropertiesObserver ()
{
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
void AlgoPropertiesObserver::update (dp::EventInfo* evt, dp::ISubject* subject)
{
    /** Check. */
    if (_visitor == 0)  { return; }

    /** We cast the received event in the type we are interested in. */
    AlgorithmReportEvent* e = dynamic_cast<AlgorithmReportEvent*> (evt);
    if (e != 0)
    {
        /** Shortcuts. */
        IAlgorithm*             algo             = e->getAlgo();
        ISequenceDatabase*      subjectDb        = e->getSubjectDb();
        ISequenceDatabase*      queryDb          = e->getQueryDb();
        IIndexator*             indexator        = algo->getIndexator();
        TimeInfo*               timeInfo         = e->getTimeInfo();
        IAlignmentContainer*    ungapResult      = e->getUngapContainer();
        IAlignmentContainer*    alignmentResult  = e->getAlignmentContainer();

        /** We define a Properties object that will collect all wanted information. */
        Properties properties;

        /** We define the root of our properties. */
        properties.add (0, "algorithm");

        if (algo && algo->getParams())
        {
            string kind;
            switch (algo->getParams()->algoKind)
            {
                case misc::ENUM_PLASTP:     kind = "plastp";    break;
                case misc::ENUM_TPLASTN:    kind = "tplastn";   break;
                case misc::ENUM_PLASTX:     kind = "plastx";    break;
                case misc::ENUM_TPLASTX:    kind = "tplastx";   break;
                default:                    kind = "?";         break;
            }

            properties.add (1, algo->getParams()->getProperties());
        }

        if (algo && algo->getSeedsModel() != 0)
        {
            properties.add (1, algo->getSeedsModel()->getProperties());
        }

        properties.add (1, "databases");

        if (subjectDb != 0)  {  properties.add (2, subjectDb->getProperties("subject"));  }
        if (queryDb   != 0)  {  properties.add (2, queryDb->getProperties  ("query"));    }

        if (indexator != 0)  {  properties.add (1, indexator->getProperties()); }

        if (algo && algo->getHitIterator())
        {
            properties.add (1, "hits");
            properties.add (2, algo->getHitIterator()->getProperties());
        }

        if (ungapResult)      { properties.add (1, ungapResult->getProperties("ungap")); }
        if (alignmentResult)  { properties.add (1, alignmentResult->getProperties("alignments")); }

        if (timeInfo)         {  properties.add (1, timeInfo->getProperties("exec_time"));  }

        /** We visit the properties through the provided visitor. According to the visitor type, it could
         *  for instance dump the properties into a file. */
        if (_visitor != 0)  {  properties.accept (_visitor);  }

        return;
    }
}

/********************************************************************************/
}} /* end of namespaces. */
/********************************************************************************/
