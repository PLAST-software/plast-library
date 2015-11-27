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
#include <index/impl/DatabaseNucleotidIndexOptim.hpp>

#include <algo/core/impl/BasicAlgoIndexator.hpp>
#include <algo/core/impl/AlgoIndexatorNucleotide.hpp>
#include <algo/core/impl/ScoreMatrix.hpp>
#include <algo/core/impl/DatabasesProvider.hpp>

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
	/** We call the parent method. */
    IParameters* params = DefaultConfiguration::createDefaultParameters (algoName);

    DEBUG ((cout << "PlastnConfiguration::createDefaultParameters : algoName='" << algoName << "'"  << endl));

    params->algoKind      = ENUM_PLASTN;
    params->seedModelKind = ENUM_BasicSeedModel;
    params->seedSpan      = 11;

    params->matrixKind           = ENUM_NUCLEOTIDE_IDENTITY_BLAST;
    params->subjectUri           = string ("foo");
    params->subjectRange         = Range64(0,0);
    params->queryUri             = string ("bar");
    params->queryRange           = Range64(0,0);
    params->filterQuery          = true;
    params->filterQueryThreshold = 1;      // not used for dust
    params->ungapNeighbourLength = 0;
    params->ungapScoreThreshold  = 35;
//    params->ungapScoreThreshold  = 51;
    params->smallGapBandLength   = 0;
    params->smallGapBandWidth    = 0;
    params->smallGapThreshold    = 0;
    params->openGapCost          = 5;
    params->extendGapCost        = 2;
    params->evalue               = 10.0;
    params->XdroppofUnGap        = 20;
    params->index_neighbor_threshold  = 0;
    //params->XdroppofGap          = 25;
    params->XdroppofGap          = 30;
    params->finalXdroppofGap     = 100;
    params->outputfile           = "stdout";
    params->reward               =  2;
    params->penalty              = -3;
    params->strand               = 0;
    params->kmersPerSequence      = 0;

    IProperty* strandProp = _properties->getProperty (STR_OPTION_STRAND);
    if (strandProp != 0)
    {
             if (strandProp->getValue().compare ("plus")  == 0)  {  params->strand =  1;  }
        else if (strandProp->getValue().compare ("minus") == 0)  {  params->strand = -1;  }
    }

    IProperty* evalue = _properties->getProperty (STR_OPTION_EVALUE);
    if (evalue != 0)
    {
    	double evalue_val = misc::atof (evalue->value.c_str());
     	double evalue_step[6]				={1e-50,	1e-30,	1e-10,	1e-3,	1,		10};
    	int index_neighbor_threshold_step[6]={50, 		40,		30,		15,		10,		5};
    	int ungapScoreThreshold_step[6]		={46, 		45,		40,		35,		35,		35};
    	u_int32_t i=0;
    	double a_neighbor = 0;
    	double b_neighbor = index_neighbor_threshold_step[0];
    	double a_threshold = 0;
    	double b_threshold = ungapScoreThreshold_step[0];
    	do
    	{
			if (i>0)
			{
				a_neighbor=((double)(index_neighbor_threshold_step[i]-index_neighbor_threshold_step[(i-1)])/
						(double)(log(evalue_step[i])-log(evalue_step[(i-1)])));
				b_neighbor = (double)index_neighbor_threshold_step[i] - a_neighbor*log(evalue_step[i]);
				a_threshold=((double)(ungapScoreThreshold_step[i]-ungapScoreThreshold_step[(i-1)])/
						(double)(log(evalue_step[i])-log(evalue_step[(i-1)])));
				b_threshold = (double)ungapScoreThreshold_step[i] - a_threshold*log(evalue_step[i]);
			}
			i++;
    	}while((i<6)&&(evalue_val>=evalue_step[(i-1)]));
		params->ungapScoreThreshold  = a_threshold*log(evalue_val) + b_threshold;
		params->index_neighbor_threshold = a_neighbor*log(evalue_val) + b_neighbor;

/*    	if (evalue_val>1)
    	{
    	    params->ungapScoreThreshold  = 35;
    		params->index_neighbor_threshold = 10;
    	}
		else if (evalue_val>1e-3)
		{
    	    params->ungapScoreThreshold  = 35;
			params->index_neighbor_threshold = 20;
		}
		else if (evalue_val>1e-10)
		{
    	    params->ungapScoreThreshold  = 35;
			params->index_neighbor_threshold = 30;
		}
		else if (evalue_val>1e-30)
		{
    	    params->ungapScoreThreshold  = 40;
			params->index_neighbor_threshold = 40;
		}
		else
		{
    	    params->ungapScoreThreshold  = 45;
			params->index_neighbor_threshold = 50;
		}*/
    }

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

    return createDispatcher();
    /** We use a serial dispatcher. */
    //return new SerialCommandDispatcher ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
algo::core::IDatabasesProvider* PlastnConfiguration::createDatabaseProvider ()
{
    return new DatabasesProviderReverse (this);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
statistics::IGlobalParameters*  PlastnConfiguration::createGlobalParameters (algo::core::IParameters* params, size_t subjectDbLength)
{
    DEBUG ((cout << "PlastnConfiguration::createGlobalParameters" << endl));

    return new statistics::impl::GlobalParametersPlastn (params, subjectDbLength);
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
    IProperty* prop = 0;
    DEBUG ((cout << "PlastnConfiguration::createSeedModel : span=" << span << endl));

    if ( (prop = _properties->getProperty (STR_OPTION_INDEX_FILTER_SEED)) != 0)
        return new BasicSeedModel (SUBSEED, span, prop->getInt());
    else
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
IScoreMatrix* PlastnConfiguration::createScoreMatrix (ScoreMatrixKind_e kind, database::Encoding encoding, int reward, int penalty)
{
    DEBUG ((cout << "PlastnConfiguration::createScoreMatrix" << endl));
    IScoreMatrix* result = 0;

    switch (kind)
    {
		case ENUM_NUCLEOTIDE_IDENTITY:
		{
			result = ScoreMatrixManager::singleton().getMatrix ("IDENTITY", encoding, reward, penalty);
			break;
		}

		case ENUM_NUCLEOTIDE_IDENTITY_BLAST:
		{
			result = ScoreMatrixManager::singleton().getMatrix ("IDENTITY_BLAST", encoding, reward, penalty);
			break;
		}

		default:
			/** We should not be here... */
			break;
    }

    return result;
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

#if 0
    return new BasicIndexator (seedsModel, params, new DatabaseNucleotidIndexOptimFactory (), 1.0, isRunning);
#else
    return new IndexatorNucleotide (seedsModel, params, new DatabaseNucleotidIndexOptimFactory (), 1.0, isRunning);
#endif
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

	IProperty* prop = 0;

    size_t nbHitPerQuery = (prop = _properties->getProperty (STR_OPTION_MAX_HIT_PER_QUERY)) != 0 ?  prop->getInt() : 500;
    size_t nbAlignPerHit = (prop = _properties->getProperty (STR_OPTION_MAX_HSP_PER_HIT))   != 0 ?  prop->getInt() : 0;

//    return new BasicAlignmentContainerBis (nbHitPerQuery, nbAlignPerHit);
    return new BasicAlignmentContainer (nbHitPerQuery, nbAlignPerHit);
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
