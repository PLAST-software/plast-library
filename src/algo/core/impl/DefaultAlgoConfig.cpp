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

#include <designpattern/impl/CommandDispatcher.hpp>
#include <designpattern/impl/Property.hpp>

#include <misc/api/PlastStrings.hpp>
#include <misc/api/macros.hpp>

#include <database/impl/BufferedSequenceDatabase.hpp>
#include <database/impl/FastaSequenceIterator.hpp>
#include <database/impl/FastaDatabaseQuickReader.hpp>
#include <database/impl/BlastdbSequenceIterator.hpp>
#include <database/impl/BlastdbDatabaseQuickReader.hpp>
#include <database/impl/DatabaseUtility.hpp>

#include <seed/impl/BasicSeedModel.hpp>
#include <seed/impl/SubSeedModel.hpp>

#include <index/impl/DatabaseIndex.hpp>

#include <algo/core/impl/DefaultAlgoConfig.hpp>
#include <algo/core/impl/ScoreMatrix.hpp>
#include <algo/core/impl/BasicAlgoIndexator.hpp>
#include <algo/core/impl/DatabasesProvider.hpp>

#include <algo/stats/impl/Statistics.hpp>
#include <algo/stats/impl/StatisticsSpouge.hpp>

#include <algo/hits/ungap/UngapHitIterator.hpp>
#include <algo/hits/ungap/UngapHitIteratorSSE8.hpp>
#include <algo/hits/ungap/UngapHitIteratorSSE16.hpp>
#include <algo/hits/ungap/UngapHitIteratorNull.hpp>
#include <algo/hits/ungap/UngapExtendHitIterator.hpp>

#include <algo/hits/gap/SmallGapHitIterator.hpp>
#include <algo/hits/gap/SmallGapHitIteratorSSE8.hpp>
#include <algo/hits/gap/SmallGapHitIteratorNull.hpp>
#include <algo/hits/gap/FullGapHitIterator.hpp>
#include <algo/hits/gap/CompositionHitIterator.hpp>

#include <alignment/core/impl/BasicAlignmentContainer.hpp>
#include <alignment/core/impl/NullAlignmentContainer.hpp>

#include <alignment/tools/impl/AlignmentSplitter.hpp>
#include <alignment/tools/impl/AlignmentSplitterBanded.hpp>

#include <alignment/tools/impl/SemiGappedAlign.hpp>
#include <alignment/tools/impl/SemiGappedAlignTraceback.hpp>

#include <alignment/visitors/impl/OstreamVisitor.hpp>
#include <alignment/visitors/impl/TabulatedOutputVisitor.hpp>
#include <alignment/visitors/impl/RawOutputVisitor.hpp>
#include <alignment/visitors/impl/XmlOutputVisitor.hpp>
#include <alignment/visitors/impl/ShrinkContainerVisitor.hpp>
#include <alignment/visitors/impl/FilterContainerVisitor.hpp>
#include <alignment/visitors/impl/ProxyVisitor.hpp>
#include <alignment/visitors/impl/NucleotidConversionVisitor.hpp>
#include <alignment/visitors/impl/QueryReorderVisitor.hpp>

using namespace std;
using namespace misc;
using namespace os;
using namespace os::impl;
using namespace dp;
using namespace dp::impl;
using namespace database;
using namespace database::impl;
using namespace seed;
using namespace seed::impl;
using namespace indexation;
using namespace indexation::impl;
using namespace indexation;
using namespace statistics;
using namespace statistics::impl;
using namespace algo::hits;
using namespace algo::hits::ungapped;
using namespace algo::hits::gapped;

using namespace alignment::core;
using namespace alignment::core::impl;
using namespace alignment::filter;
using namespace alignment::visitors;
using namespace alignment::visitors::impl;
using namespace alignment::tools;
using namespace alignment::tools::impl;

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
DefaultConfiguration::DefaultConfiguration (IEnvironment* environment, IProperties* properties)
    : _environment(environment), _properties(0)
{
    /** We may have to create empty properties if no one is provided. */
    if (properties == 0)  {  properties = new Properties(); }

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
DefaultConfiguration::~DefaultConfiguration ()
{
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
database::IDatabaseQuickReader* DefaultConfiguration::createDefaultQuickReader (const std::string& uri, bool shouldInferType)
{
	DatabaseLookupType::QuickReaderType_e databaseType = DatabaseLookupType::ENUM_TYPE_UNKNOWN;

	if (uri!="foo")
	{
		databaseType = DatabaseLookupType::quickReaderType(uri);
		if ((databaseType==DatabaseLookupType::ENUM_BLAST_PIN)||(databaseType==DatabaseLookupType::ENUM_BLAST_NIN)
				||(databaseType==DatabaseLookupType::ENUM_BLAST_PAL)||(databaseType==DatabaseLookupType::ENUM_BLAST_NAL))
			return new BlastdbDatabaseQuickReader (uri, shouldInferType);
		else
			return new FastaDatabaseQuickReader (uri, shouldInferType);
	}
	else
		return new FastaDatabaseQuickReader (uri, shouldInferType);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IParameters* DefaultConfiguration::createDefaultParameters (const std::string& algoName)
{
    IParameters* params = new IParameters ();

    if (algoName.compare ("plastp")==0)
    {
        params->algoKind      = ENUM_PLASTP;
        params->seedModelKind = ENUM_SubSeedModel;
        params->seedSpan      = 4;
        params->subseedStrings.push_back ("H,FY,W,IV,LM,C,RK,Q,E,N,D,A,S,T,G,P");
        params->subseedStrings.push_back ("HFYWIVLMC,RKQENDASTGP");
        params->subseedStrings.push_back ("H,FYW,IVLM,C,RK,QE,ND,A,ST,G,P");
        params->subseedStrings.push_back ("H,FY,W,IV,LM,C,R,K,Q,E,N,D,A,S,T,G,P");

        params->matrixKind           = ENUM_BLOSUM62;
        params->subjectUri           = string ("foo");
        params->subjectRange         = Range64(0,0);
        params->queryUri             = string ("bar");
        params->queryRange           = Range64(0,0);
        params->filterQuery          = true;
        params->filterQueryThreshold = 50;
        params->ungapNeighbourLength = 22;
        params->ungapScoreThreshold  = 38;
        params->smallGapBandLength   = 64;
        params->smallGapBandWidth    = 16;
        params->smallGapThreshold    = 54;
        params->openGapCost          = 0;  // 0 means default value; actual value will be set later
        params->extendGapCost        = 0;  // 0 means default value; actual value will be set later
        params->evalue               = 10.0;
        params->XdroppofUnGap        = 0;
        params->XdroppofGap          = 0;
        params->finalXdroppofGap     = 0;
        params->outputfile           = "stdout";
    }

    else if (algoName.compare ("tplastn")==0)
    {
        params->algoKind      = ENUM_TPLASTN;
        params->seedModelKind = ENUM_SubSeedModel;
        params->seedSpan      = 4;
        params->subseedStrings.push_back ("A,C,D,E,F,G,H,I,K,L,M,N,P,Q,R,S,T,V,W,Y");
        params->subseedStrings.push_back ("CFYWMLIV,GPATSNHQEDRK");
        params->subseedStrings.push_back ("A,C,FYW,G,IV,ML,NH,P,QED,RK,TS");
        params->subseedStrings.push_back ("A,C,D,E,F,G,H,I,K,L,M,N,P,Q,R,S,T,V,W,Y");

        params->matrixKind           = ENUM_BLOSUM62;
        params->subjectUri           = string ("foo");
        params->subjectRange         = Range64(0,0);
        params->queryUri             = string ("bar");
        params->queryRange           = Range64(0,0);
        params->filterQuery          = true;
        params->filterQueryThreshold = 50;
        params->ungapNeighbourLength = 22;
        params->ungapScoreThreshold  = 38;
        params->smallGapBandLength   = 64;
        params->smallGapBandWidth    = 16;
        params->smallGapThreshold    = 54;
        params->openGapCost          = 0;  // 0 means default value; actual value will be set later
        params->extendGapCost        = 0;  // 0 means default value; actual value will be set later
        params->evalue               = 10.0;
        params->XdroppofUnGap        = 0;
        params->XdroppofGap          = 0;
        params->finalXdroppofGap     = 0;
        params->outputfile           = "stdout";
    }

    else if (algoName.compare ("plastx")==0)
    {
        params->algoKind      = ENUM_PLASTX;
        params->seedModelKind = ENUM_SubSeedModel;
        params->seedSpan      = 4;
        params->subseedStrings.push_back ("H,FY,W,IV,L,M,C,R,K,Q,E,N,D,A,S,T,G,P");
        params->subseedStrings.push_back ("HFYWIVLMC,RKQENDASTGP");
        params->subseedStrings.push_back ("H,FYW,IVLM,C,RK,QE,ND,A,ST,G,P");
        params->subseedStrings.push_back ("H,FY,W,I,V,L,M,C,R,K,Q,E,N,D,A,S,T,G,P");

        params->matrixKind           = ENUM_BLOSUM62;
        params->subjectUri           = string ("foo");
        params->subjectRange         = Range64(0,0);
        params->queryUri             = string ("bar");
        params->queryRange           = Range64(0,0);
        params->filterQuery          = true;
        params->filterQueryThreshold = 20;
        params->ungapNeighbourLength = 22;
        params->ungapScoreThreshold  = 38;
        params->smallGapBandLength   = 64;
        params->smallGapBandWidth    = 16;
        params->smallGapThreshold    = 54;
        params->openGapCost          = 0; // 0 means default value; actual value will be set later
        params->extendGapCost        = 0; // 0 means default value; actual value will be set later
        params->evalue               = 10.0;
        params->XdroppofUnGap        = 0;
        params->XdroppofGap          = 0;
        params->finalXdroppofGap     = 0;
        params->outputfile           = "stdout";
    }

    else if (algoName.compare ("tplastx")==0)
    {
        params->algoKind      = ENUM_TPLASTX;
        params->seedModelKind = ENUM_SubSeedModel;
        params->seedSpan      = 4;
        params->subseedStrings.push_back ("A,C,D,E,F,G,H,I,K,L,M,N,P,Q,R,S,T,V,W,Y");
        params->subseedStrings.push_back ("CFYWMLIV,GPATSNHQEDRK");
        params->subseedStrings.push_back ("A,C,FYW,G,IV,ML,NH,P,QED,RK,TS");
        params->subseedStrings.push_back ("A,C,D,E,F,G,H,I,K,L,M,N,P,Q,R,S,T,V,W,Y");

        params->matrixKind           = ENUM_BLOSUM62;
        params->subjectUri           = string ("foo");
        params->subjectRange         = Range64(0,0);
        params->queryUri             = string ("bar");
        params->queryRange           = Range64(0,0);
        params->filterQuery          = true;
        params->filterQueryThreshold = 20;
        params->ungapNeighbourLength = 22;
        params->ungapScoreThreshold  = 38;
        params->smallGapBandLength   = 64;
        params->smallGapBandWidth    = 16;
        params->smallGapThreshold    = 54;
        params->openGapCost          = 0; // 0 means default value; actual value will be set later
        params->extendGapCost        = 0; // 0 means default value; actual value will be set later
        params->evalue               = 10.0;
        params->XdroppofUnGap        = 0;
        params->XdroppofGap          = 0;
        params->finalXdroppofGap     = 0;
        params->outputfile           = "stdout";
    }

    params->kmersPerSequence = 0;
    params->querySequencesBlacklist = NULL;

    IProperty* prop = 0;

    /** We may want to restrict the number of dumped alingments. */
    params->nbAlignPerHit = (prop = _properties->getProperty (STR_OPTION_MAX_HSP_PER_HIT))   != 0 ?  prop->getInt() : 0;
    params->nbHitPerQuery = (prop = _properties->getProperty (STR_OPTION_MAX_HIT_PER_QUERY)) != 0 ?  prop->getInt() : 500;

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
ICommandDispatcher* DefaultConfiguration::createDispatcher ()
{
    ICommandDispatcher* result = 0;

    /** We get the default number of usable cores. */
    size_t nbProc = DefaultFactory::thread().getNbCores();

    /** We try to get the property giving the number of wanted processors to be used. */
    IProperty* propNbProcessors = _properties->getProperty (STR_OPTION_NB_PROCESSORS);

    if (propNbProcessors != 0)  {  nbProc = propNbProcessors->getInt(); }
    else
    {
        _properties->add (0, STR_OPTION_NB_PROCESSORS, "%d", nbProc);
    }

    /** We retrieve the property. */
    IProperty* prop = _properties->getProperty (STR_OPTION_FACTORY_DISPATCHER);

    if (prop && prop->value.compare(STR_CONFIG_CLASS_SerialCommandDispatcher)==0)
    {
        result = new SerialCommandDispatcher ();
    }
    else if (prop && prop->value.compare(STR_CONFIG_CLASS_ParallelCommandDispatcher)==0)
    {
        result = new ParallelCommandDispatcher (nbProc);
    }
    else
    {
        result = new ParallelCommandDispatcher (nbProc);
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
dp::ICommandDispatcher* DefaultConfiguration::createIndexationDispatcher ()
{
    /** By default, we return the same stuff as "createDispatcher". */
    return createDispatcher();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
os::impl::TimeInfo* DefaultConfiguration::createTimeInfo ()
{
    return new TimeInfo ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
algo::core::IDatabasesProvider* DefaultConfiguration::createDatabaseProvider ()
{
    return new DatabasesProvider (this);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IQueryInformation* DefaultConfiguration::createQueryInformation (
    IGlobalParameters*  globalStats,
    IParameters*        parameters,
    ISequenceDatabase*  queryDb,
    size_t              subjectSize,
    size_t              subjectNbSequences
)
{
    IQueryInformation* result = 0;

    result = new QueryInformation (
        globalStats,
        parameters,
        queryDb,
        subjectSize,
        subjectNbSequences
    );

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
IGlobalParameters*  DefaultConfiguration::createGlobalParameters (IParameters* params, size_t subjectDbLength)
{
    IGlobalParameters* result = 0;
    IProperty* prop = 0;

    /** We retrieve the property. */
    prop = _properties->getProperty (STR_OPTION_FACTORY_STATISTICS);

    if (prop && prop->value.compare(STR_CONFIG_CLASS_SpougeStats)==0)
    {
        result = new GlobalParametersSpouge (params, subjectDbLength);
    }
    else
    {
        result = new GlobalParameters (params, subjectDbLength);
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
ISeedModel* DefaultConfiguration::createSeedModel (SeedModelKind_e modelKind, size_t span, const vector<string>& subseedStrings)
{
    ISeedModel* result = 0;

    switch (modelKind)
    {
        case ENUM_BasicSeedModel:
            result = new BasicSeedModel (SUBSEED, span);
            break;

        case ENUM_SubSeedModel:
            result = new SubSeedModel (subseedStrings);
            break;

        default:
            result = new SubSeedModel (subseedStrings);
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
IIndexator*  DefaultConfiguration::createIndexator (
    seed::ISeedModel*           seedsModel,
    algo::core::IParameters*    params,
    bool&                       isRunning
)
{
    IIndexator* result = 0;
    IProperty* prop = 0;

    /** We will need a factory for creating IDatabaseIndex instances. */
    IDatabaseIndexFactory* dbIndexFactory = 0;

    if (params->algoKind == ENUM_PLASTP)
    {
        dbIndexFactory = new DatabaseIndexFactory ();
    }
    else if ( (prop = _properties->getProperty (STR_OPTION_CODON_STOP_OPTIM)) != 0)
    {
        if (prop->getInt() != 0)
        {
            dbIndexFactory = new DatabaseIndexCodonStopOptimFactory (prop->getInt());
        }
        else
        {
            dbIndexFactory = new DatabaseIndexFactory ();
        }
    }
    else
    {
        dbIndexFactory = new DatabaseIndexCodonStopOptimFactory (params->ungapNeighbourLength);
    }

    /** We get the ratio of seeds to be used for hit iteration. */
    float seedsUseRatio = 1.0;
    if ( (prop = _properties->getProperty (STR_OPTION_SEEDS_USE_RATIO)) != 0)
    {
        seedsUseRatio = misc::atof (prop->getString());

        /** A little check. */
        if (seedsUseRatio<0.0 || seedsUseRatio>1.0)  { seedsUseRatio = 1.0; }
    }

    /** We retrieve the property. */
    prop = _properties->getProperty (STR_OPTION_FACTORY_INDEXATION);

    if (prop && prop->value.compare(STR_CONFIG_CLASS_BasicIndexator)==0)
    {
        result = new BasicIndexator (seedsModel, params, dbIndexFactory, seedsUseRatio, isRunning);
    }
    else if (prop && prop->value.compare(STR_CONFIG_CLASS_BasicSortedIndexator)==0)
    {
        result = new BasicSortedIndexator (seedsModel, params, dbIndexFactory, seedsUseRatio, isRunning);
    }
    else
    {
        result = new BasicSortedIndexator (seedsModel, params, dbIndexFactory, seedsUseRatio, isRunning);
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
IScoreMatrix* DefaultConfiguration::createScoreMatrix (
    ScoreMatrixKind_e kind,
    database::Encoding encoding,
    int reward,
    int penalty
)
{
    IScoreMatrix* result = 0;

    switch (kind)
    {
        case ENUM_BLOSUM62:
            result = ScoreMatrixManager::singleton().getMatrix ("BLOSUM62", encoding, reward, penalty);
            break;

        case ENUM_BLOSUM50:
        {
            result = ScoreMatrixManager::singleton().getMatrix ("BLOSUM50", encoding, reward, penalty);
            break;
        }

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
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IHitIterator* DefaultConfiguration::createUngapHitIterator (
    IHitIterator*           source,
    ISeedModel*             model,
    IScoreMatrix*           matrix,
    IParameters*            params,
    IAlignmentContainer*    ungapResult,
    bool&                   isRunning
)
{
    IHitIterator* result = 0;

    IAlignmentContainer* actualUngapResult = ungapResult;

    IProperty* optimProp = _properties->getProperty (STR_OPTION_OPTIM_FILTER_UNGAP);
    if (optimProp && optimProp->value.compare("F")==0)  { actualUngapResult = new NullAlignmentResult(); }

    IProperty* maxhitsPerIterProp = _properties->getProperty (STR_OPTION_MAX_HIT_PER_ITERATION);
    u_int32_t maxHitsPerIter = maxhitsPerIterProp ? maxhitsPerIterProp->getInt() : 0;

    /** We retrieve the property. */
    IProperty* prop = _properties->getProperty (STR_OPTION_FACTORY_HIT_UNGAP);

    if (prop && prop->value.compare(STR_CONFIG_CLASS_UngapHitIteratorNull)==0)
    {
        result = new UngapHitIteratorNull (source, model, matrix, params, actualUngapResult);
    }
    else if (prop && prop->value.compare(STR_CONFIG_CLASS_UngapHitIterator)==0)
    {
        result = new UngapHitIterator (source, model, matrix, params, actualUngapResult);
    }
    else if (prop && prop->value.compare(STR_CONFIG_CLASS_UngapHitIteratorSSE16)==0)
    {
        result = new UngapHitIteratorSSE16 (source, model, matrix, params, actualUngapResult, maxHitsPerIter, isRunning);
    }
    else
    {
        result = new UngapHitIteratorSSE16 (source, model, matrix, params, actualUngapResult, maxHitsPerIter, isRunning);
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
algo::hits::IHitIterator* DefaultConfiguration::createUngapExtendHitIterator (
    algo::hits::IHitIterator*               source,
    seed::ISeedModel*                       model,
    algo::core::IScoreMatrix*               matrix,
    algo::core::IParameters*                params,
    alignment::core::IAlignmentContainer*   ungapResult,
    statistics::IGlobalParameters*          globalStats,
    statistics::IQueryInformation*          queryInfo,
    bool&                                   isRunning
)
{
    return new UngapExtendHitIterator (
        source, model, matrix, params, ungapResult, globalStats, queryInfo
    );
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IHitIterator* DefaultConfiguration::createSmallGapHitIterator (
    IHitIterator*           source,
    ISeedModel*             model,
    IScoreMatrix*           matrix,
    IParameters*            params,
    IAlignmentContainer*    ungapResult,
    IAlignmentContainer*    alignmentResult,
    bool&                   isRunning
)
{
    IHitIterator* result = 0;

    /** We retrieve the property. */
    IProperty* prop = _properties->getProperty (STR_OPTION_FACTORY_HIT_SMALLGAP);

    if (prop && prop->value.compare(STR_CONFIG_CLASS_SmallGapHitIterator)==0)
    {
        result = new SmallGapHitIterator (source, model, matrix, params, ungapResult);
    }
    else if (prop && prop->value.compare(STR_CONFIG_CLASS_SmallGapHitIteratorNull)==0)
    {
        result = new SmallGapHitIteratorNull (source, model, matrix, params, ungapResult);
    }
    else if (prop && prop->value.compare(STR_CONFIG_CLASS_SmallGapHitIteratorSSE8)==0)
    {
        result = new SmallGapHitIteratorSSE8 (source, model, matrix, params, ungapResult, alignmentResult);
    }
    else
    {
        result = new SmallGapHitIteratorSSE8 (source, model, matrix, params, ungapResult, alignmentResult);
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
IHitIterator* DefaultConfiguration::createFullGapHitIterator (
    IHitIterator*           source,
    ISeedModel*             model,
    IScoreMatrix*           matrix,
    IParameters*            params,
    IQueryInformation*      queryInfo,
    IGlobalParameters*      globalStats,
    IAlignmentContainer*    ungapResult,
    IAlignmentContainer*    alignmentResult,
    bool&                   isRunning
)
{
    IHitIterator* result = 0;

    /** We retrieve the property. */
    IProperty* prop = _properties->getProperty (STR_OPTION_FACTORY_HIT_FULLGAP);

    if (prop && prop->value.compare(STR_CONFIG_CLASS_FullGapHitIterator)==0)
    {
        result = new FullGapHitIterator (
            source,
            this,
            model,
            matrix,
            params,
            ungapResult,
            queryInfo,
            globalStats,
            alignmentResult
        );
    }
    if (prop && prop->value.compare(STR_CONFIG_CLASS_FullGapHitIteratorNull)==0)
    {
        result = new FullGapHitIteratorNull (
            source,
            model,
            matrix,
            params,
            ungapResult,
            queryInfo,
            globalStats,
            alignmentResult
        );
    }
    else
    {
        result = new FullGapHitIterator (
            source,
            this,
            model,
            matrix,
            params,
            ungapResult,
            queryInfo,
            globalStats,
            alignmentResult
        );
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
IHitIterator* DefaultConfiguration::createCompositionHitIterator  (
    IHitIterator*           source,
    ISeedModel*             model,
    IScoreMatrix*           matrix,
    IParameters*            params,
    IQueryInformation*      queryInfo,
    IGlobalParameters*      globalStats,
    IAlignmentContainer*    ungapResult,
    IAlignmentContainer*    alignmentResult,
    bool&                   isRunning
)
{
    IHitIterator* result = 0;

    /** We retrieve the property. */
    IProperty* prop = _properties->getProperty (STR_OPTION_FACTORY_HIT_COMPOSITION);

    if (prop && prop->value.compare(STR_CONFIG_CLASS_CompositionHitIterator)==0)
    {
        result = new CompositionHitIterator (
            source,
            this,
            model,
            matrix,
            params,
            ungapResult,
            queryInfo,
            globalStats,
            alignmentResult
        );
    }
    if (prop && prop->value.compare(STR_CONFIG_CLASS_CompositionHitIteratorNull)==0)
    {
        result = new CompositionHitIteratorNull (
            source,
            model,
            matrix,
            params,
            ungapResult,
            queryInfo,
            globalStats,
            alignmentResult
        );
    }
    else
    {
        result = new CompositionHitIterator (
            source,
            this,
            model,
            matrix,
            params,
            ungapResult,
            queryInfo,
            globalStats,
            alignmentResult
        );
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
IAlignmentContainer* DefaultConfiguration::createGapAlignmentResult  ()
{
    IAlignmentContainer* result = 0;

    /** We retrieve the property. */
    IProperty* prop = _properties->getProperty (STR_OPTION_FACTORY_GAP_RESULT);

    size_t nbHitPerQuery = (prop = _properties->getProperty (STR_OPTION_MAX_HIT_PER_QUERY)) != 0 ?  prop->getInt() : 500;
    size_t nbAlignPerHit = (prop = _properties->getProperty (STR_OPTION_MAX_HSP_PER_HIT))   != 0 ?  prop->getInt() : 0;

    if (prop && prop->value.compare(STR_CONFIG_CLASS_BasicAlignmentResult)==0)
    {
        result = new BasicAlignmentContainer (nbHitPerQuery, nbAlignPerHit);
    }
    else
    {
        result = new BasicAlignmentContainer (nbHitPerQuery, nbAlignPerHit);
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
IAlignmentContainer* DefaultConfiguration::createUnapAlignmentResult (size_t querySize)
{
    IAlignmentContainer* result = 0;

    /** We retrieve the property. */
    IProperty* prop = _properties->getProperty (STR_OPTION_FACTORY_UNGAP_RESULT);

    if (prop && prop->value.compare(STR_CONFIG_CLASS_UngapAlignmentResult)==0)
    {
        result = new UngapAlignmentResult (querySize);
    }
    else
    {
        result = new UngapAlignmentResult (querySize);
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
alignment::tools::IAlignmentSplitter* DefaultConfiguration::createAlignmentSplitter (
    algo::core::IScoreMatrix* scoreMatrix,
    int openGapCost,
    int extendGapCost
)
{
    IProperty* prop = _properties->getProperty (STR_OPTION_FACTORY_SPLITTER);

    if (prop && prop->value.compare("normal")==0)
    {
        return new AlignmentSplitter (scoreMatrix, openGapCost, extendGapCost);
    }
    else if (prop && prop->value.compare("banded")==0)
    {
        return new AlignmentSplitterBanded (scoreMatrix, openGapCost, extendGapCost);
    }
    else
    {
        return new AlignmentSplitterBanded (scoreMatrix, openGapCost, extendGapCost);
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
alignment::tools::ISemiGapAlign* DefaultConfiguration::createSemiGapAlign (
    algo::core::IScoreMatrix* scoreMatrix,
    int openGapCost,
    int extendGapCost,
    int Xdropoff
)
{
    return new SemiGapAlign (scoreMatrix, openGapCost, extendGapCost, Xdropoff);
    //return new SemiGapAlignTraceback (scoreMatrix, openGapCost, extendGapCost, Xdropoff);
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
