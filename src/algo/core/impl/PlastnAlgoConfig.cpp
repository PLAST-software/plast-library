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

#include <algo/core/impl/PlastnAlgoConfig.hpp>

#include <misc/api/PlastStrings.hpp>

#include <designpattern/impl/CommandDispatcher.hpp>

#include <seed/impl/BasicSeedModel.hpp>

#include <database/impl/FastaSequenceIterator.hpp>
#include <database/impl/BufferedCachedSequenceDatabase.hpp>
#include <database/impl/ReverseStrandSequenceIterator.hpp>

#include <index/impl/DatabaseIndex.hpp>
#include <index/impl/DatabaseNucleotidIndex.hpp>

#include <algo/core/impl/BasicAlgoIndexator.hpp>
#include <algo/core/impl/ScoreMatrix.hpp>

#include <algo/stats/impl/StatisticsPlastn.hpp>

#include <alignment/core/impl/BasicAlignmentContainer.hpp>
#include <alignment/filter/api/IAlignmentFilter.hpp>

using namespace std;
using namespace misc;

using namespace os;
using namespace os::impl;

using namespace dp;
using namespace dp::impl;

using namespace seed;
using namespace seed::impl;

using namespace indexation;
using namespace indexation::impl;

using namespace database;
using namespace database::impl;

using namespace algo::core;
using namespace algo::core::impl;

using namespace alignment::filter;

using namespace alignment::core;
using namespace alignment::core::impl;

#include <iostream>
#include <sstream>
#include <stdarg.h>

#include <stdio.h>
#define DEBUG(a)  //a

/********************************************************************************/
namespace algo {
namespace core {
namespace impl {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
PlastnConfiguration::PlastnConfiguration (IEnvironment* environment, dp::IProperties* properties)
    : DefaultConfiguration (environment, properties)
{
    DEBUG ((cout << "PlastnConfiguration::PlastnConfiguration" << endl));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
PlastnConfiguration::~PlastnConfiguration ()
{
    DEBUG ((cout << "PlastnConfiguration::~PlastnConfiguration" << endl));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IParameters* PlastnConfiguration::createDefaultParameters (const std::string& algoName)
{
    IParameters* params = new IParameters ();

    DEBUG ((cout << "PlastnConfiguration::createDefaultParameters : algoName='" << algoName << "'"  << endl));

    params->algoKind      = ENUM_PLASTN;
    params->seedModelKind = ENUM_BasicSeedModel;
    params->seedSpan      = 11;

    params->matrixKind           = ENUM_BLOSUM62;
    params->subjectUri           = string ("foo");
    params->subjectRange         = Range64(0,0);
    params->queryUri             = string ("bar");
    params->queryRange           = Range64(0,0);
    params->filterQuery          = false;  // Don't do that for nucleotid/nucleotid comparisons
    params->ungapNeighbourLength = 0;
    params->ungapScoreThreshold  = 27;
    params->smallGapBandLength   = 0;
    params->smallGapBandWidth    = 0;
    params->smallGapThreshold    = 0;
    params->openGapCost          = 0;
    params->extendGapCost        = 0;
    params->evalue               = 10.0;
    params->XdroppofGap          = 25;
    params->finalXdroppofGap     = 100;
    params->outputfile           = "stdout";
    params->reward               =  1;
    params->penalty              = -2;
    params->strand               = 0;

    IProperty* strandProp = _properties->getProperty (STR_OPTION_STRAND);
    if (strandProp != 0)
    {
             if (strandProp->getValue().compare ("plus")  == 0)  {  params->strand =  1;  }
        else if (strandProp->getValue().compare ("minus") == 0)  {  params->strand = -1;  }
    }

    /** We may want to restrict the number of dumped alignments. */
    IProperty* maxHspPerHitProp = _properties->getProperty (STR_OPTION_MAX_HSP_PER_HIT);
    if (maxHspPerHitProp != 0)  { params->nbAlignPerHit = atoi (maxHspPerHitProp->value.c_str());  }
    else                        { params->nbAlignPerHit = 0; }

    return params;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
dp::ICommandDispatcher* PlastnConfiguration::createIndexationDispatcher ()
{
    DEBUG ((cout << "PlastnConfiguration::createIndexationDispatcher" << endl));

    /** We use a serial dispatcher. */
    return new SerialCommandDispatcher ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ISequenceDatabase*  PlastnConfiguration::createDatabase (
    const string& uri,
    const Range64& range,
    bool filtering,
    database::ISequenceIteratorFactory* sequenceIteratorFactory
)
{
    DEBUG ((cout << "PlastnConfiguration::createDatabase : "
        << "  uri='" << uri << "'  range=" << range
        << "  filtering=" << filtering << "  sequenceIteratorFactory=" << sequenceIteratorFactory << endl
    ));

    LOCAL (sequenceIteratorFactory);

    /** We create the sequence iterator. */
    ISequenceIterator* seqIterator =  sequenceIteratorFactory ?
        sequenceIteratorFactory->createSequenceIterator (uri, range) :
        new FastaSequenceIterator (uri.c_str(), 64*1024, range.begin, range.end);

    return new BufferedCachedSequenceDatabase (seqIterator, filtering);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
statistics::IGlobalParameters*  PlastnConfiguration::createGlobalParameters (algo::core::IParameters* params)
{
    DEBUG ((cout << "PlastnConfiguration::createGlobalParameters" << endl));

    return new statistics::impl::GlobalParametersPlastn (params);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ISeedModel* PlastnConfiguration::createSeedModel (SeedModelKind_e modelKind, size_t span, const vector<string>& subseedStrings)
{
    DEBUG ((cout << "PlastnConfiguration::createSeedModel : span=" << span << endl));

    return new BasicSeedModel (SUBSEED, span);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IScoreMatrix* PlastnConfiguration::createScoreMatrix (ScoreMatrixKind_e kind, database::Encoding encoding)
{
    DEBUG ((cout << "PlastnConfiguration::createScoreMatrix" << endl));

    return ScoreMatrixManager::singleton().getMatrix ("IDENTITY", encoding);
}

/*********************************************************************
** METHOD  :IAlignmentContainer
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IIndexator*  PlastnConfiguration::createIndexator (
    seed::ISeedModel*           seedsModel,
    algo::core::IParameters*    params,
    bool&                       isRunning
)
{
    DEBUG ((cout << "PlastnConfiguration::createIndexator" <<  endl));

    return new BasicIndexator (seedsModel, params, new DatabaseNucleotidIndexFactory (), 1.0, isRunning);
    //return new BasicIndexator (seedsModel, params, new DatabaseIndexFactory (), 1.0, isRunning);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IAlignmentContainer* PlastnConfiguration::createGapAlignmentResult  ()
{
    DEBUG ((cout << "PlastnConfiguration::createGapAlignmentResult" <<  endl));

    return new BasicAlignmentContainerBis ();
    //return new BasicAlignmentContainer ();
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
