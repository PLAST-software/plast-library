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

#include "DefaultAlgoConfig.hpp"

#include "BufferedSequenceDatabase.hpp"
#include "FastaSequenceIterator.hpp"

#include "BasicSeedModel.hpp"
#include "SubSeedModel.hpp"

#include "ScoreMatrix.hpp"

#include "UngapHitIterator.hpp"
#include "UngapHitIteratorSSE8.hpp"
#include "UngapHitIteratorSSE16.hpp"
#include "UngapHitIteratorNull.hpp"

#include "SmallGapHitIterator.hpp"
#include "SmallGapHitIteratorSSE8.hpp"
#include "SmallGapHitIteratorNull.hpp"

#include "FullGapHitIterator.hpp"

#include "CompositionHitIterator.hpp"

#include "BasicAlignmentResult.hpp"
#include "BasicAlignmentResult2.hpp"
#include "BasicAlignmentResult3.hpp"


#include "CommandDispatcher.hpp"

#include "BasicAlgoIndexator.hpp"

#include "Statistics.hpp"

#include "AlignmentResultVisitors.hpp"

#include "PlastStrings.hpp"

#include "Property.hpp"

using namespace std;
using namespace types;
using namespace os;
using namespace dp;
using namespace database;
using namespace seed;
using namespace indexation;
using namespace statistics;

#include <iostream>
#include <sstream>
#include <stdarg.h>
#include "macros.hpp"

#include <stdio.h>
#define DEBUG(a)  //printf a

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
DefaultConfiguration::DefaultConfiguration (dp::IProperties* properties) : _properties(0)
{
    setProperties (properties ? properties : new Properties() );
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
        params->subjectRange         = Range(0,0);
        params->queryUri             = string ("bar");
        params->queryRange           = Range(0,0);
        params->filterQuery          = true;
        params->ungapNeighbourLength = 22;
        params->ungapScoreThreshold  = 38;
        params->smallGapBandLength   = 64;
        params->smallGapBandWidth    = 16;
        params->smallGapThreshold    = 54;
        params->openGapCost          = 11;
        params->extendGapCost        = 1;
        params->evalue               = 10.0;
        params->XdroppofGap          = 0;
        params->finalXdroppofGap     = 0;
        params->outputfile           = "out";
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
        params->subjectRange         = Range(0,0);
        params->queryUri             = string ("bar");
        params->queryRange           = Range(0,0);
        params->filterQuery          = true;
        params->ungapNeighbourLength = 22;
        params->ungapScoreThreshold  = 38;
        params->smallGapBandLength   = 64;
        params->smallGapBandWidth    = 16;
        params->smallGapThreshold    = 40;
        params->openGapCost          = 11;
        params->extendGapCost        = 1;
        params->evalue               = 10.0;
        params->XdroppofGap          = 0;
        params->finalXdroppofGap     = 0;
        params->outputfile           = "out";
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
        params->subjectRange         = Range(0,0);
        params->queryUri             = string ("bar");
        params->queryRange           = Range(0,0);
        params->filterQuery          = true;
        params->ungapNeighbourLength = 22;
        params->ungapScoreThreshold  = 38;
        params->smallGapBandLength   = 64;
        params->smallGapBandWidth    = 16;
        params->smallGapThreshold    = 54;
        params->openGapCost          = 11;
        params->extendGapCost        = 1;
        params->evalue               = 10.0;
        params->XdroppofGap          = 0;
        params->finalXdroppofGap     = 0;
        params->outputfile           = "out";
    }

    else if (algoName.compare ("tplastx")==0)
    {
        // TO BE DONE...
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
ICommandDispatcher* DefaultConfiguration::createDispatcher ()
{
    ICommandDispatcher* result = 0;

    /** We try to get the property giving the number of wanted processors to be used. */
    IProperty* propNbProcessors = _properties->getProperty (STR_OPTION_NB_PROCESSORS);
    size_t nbProc = (propNbProcessors ? atoi(propNbProcessors->value.c_str()) : 0);

    /** We retrieve the property. */
    IProperty* prop = _properties->getProperty (STR_OPTION_FACTORY_DISPATCHER);

    if (prop && prop->value.compare("SerialCommandDispatcher")==0)
    {
        result = new SerialCommandDispatcher ();
    }
    else if (prop && prop->value.compare("ParallelCommandDispatcher")==0)
    {
        result = new ParallelCommandDispatcher (DefaultCommandInvokerFactory::singleton(), nbProc);
    }
    else
    {
        result = new ParallelCommandDispatcher (DefaultCommandInvokerFactory::singleton(), nbProc);
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
ISequenceDatabase*  DefaultConfiguration::createDatabase (const string& uri, const Range& range, bool filtering)
{
    database::ISequenceDatabase* result = 0;

    result = new BufferedSequenceDatabase (
        new FastaSequenceIterator (uri.c_str(), 100, range.first, range.second),
        filtering
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
IGlobalParameters*  DefaultConfiguration::createGlobalParameters (IParameters* params)
{
    IGlobalParameters* result = 0;

    result = new GlobalParameters (params);

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
ISeedModel* DefaultConfiguration::createSeedModel (SeedModelKind_e modelKind, const vector<string>& subseedStrings)
{
    ISeedModel* result = 0;

    result = new SubSeedModel (subseedStrings);

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
IIndexator*  DefaultConfiguration::createIndexator (seed::ISeedModel* seedsModel, algo::IParameters* params)
{
    IIndexator* result = 0;

    /** We retrieve the property. */
    IProperty* prop = _properties->getProperty (STR_OPTION_FACTORY_INDEXATION);

    if (prop && prop->value.compare("BasicIndexator")==0)
    {
        result = new BasicIndexator (seedsModel, params);
    }
    else if (prop && prop->value.compare("BasicSortedIndexator")==0)
    {
        result = new BasicSortedIndexator (seedsModel, params);
    }
    else
    {
        result = new BasicSortedIndexator (seedsModel, params);
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
IScoreMatrix* DefaultConfiguration::createScoreMatrix (ScoreMatrixKind_e kind, database::Encoding encoding)
{
    IScoreMatrix* result = 0;

    switch (kind)
    {
        case ENUM_BLOSUM62:
            result = ScoreMatrixManager::singleton().getMatrix ("BLOSUM62", encoding);
            break;

        case ENUM_BLOSUM50:
        {
            result = ScoreMatrixManager::singleton().getMatrix ("BLOSUM50", encoding);
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
    IHitIterator*       source,
    ISeedModel*         model,
    IScoreMatrix*       matrix,
    IParameters*        params,
    IAlignmentResult*   ungapResult
)
{
    IHitIterator* result = 0;

    IAlignmentResult* actualUngapResult = ungapResult;

    IProperty* optimProp = _properties->getProperty (STR_OPTION_OPTIM_FILTER_UNGAP);
    if (optimProp && optimProp->value.compare("F")==0)  { actualUngapResult = 0; }

    IProperty* maxhitsPerIterProp = _properties->getProperty (STR_OPTION_MAX_HIT_PER_ITERATION);
    u_int32_t maxHitsPerIter = maxhitsPerIterProp ? maxhitsPerIterProp->getInt() : 0;

    /** We retrieve the property. */
    IProperty* prop = _properties->getProperty (STR_OPTION_FACTORY_HIT_UNGAP);

    if (prop && prop->value.compare("UngapHitIteratorNull")==0)
    {
        result = new UngapHitIteratorNull (source, model, matrix, params, actualUngapResult);
    }
    else if (prop && prop->value.compare("UngapHitIterator")==0)
    {
        result = new UngapHitIterator (source, model, matrix, params, actualUngapResult);
    }
    else if (prop && prop->value.compare("UngapHitIteratorSSE16")==0)
    {
        result = new UngapHitIteratorSSE16 (source, model, matrix, params, actualUngapResult, maxHitsPerIter);
    }
    else
    {
        result = new UngapHitIteratorSSE16 (source, model, matrix, params, actualUngapResult, maxHitsPerIter);
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
IHitIterator* DefaultConfiguration::createSmallGapHitIterator (
    IHitIterator*       source,
    ISeedModel*         model,
    IScoreMatrix*       matrix,
    IParameters*        params,
    IAlignmentResult*   ungapResult
)
{
    IHitIterator* result = 0;

    /** We retrieve the property. */
    IProperty* prop = _properties->getProperty (STR_OPTION_FACTORY_HIT_SMALLGAP);

    if (prop && prop->value.compare("SmallGapHitIteratorNull")==0)
    {
        result = new SmallGapHitIteratorNull (source, model, matrix, params, ungapResult);
    }
    else if (prop && prop->value.compare("SmallGapHitIteratorSSE8")==0)
    {
        result = new SmallGapHitIteratorSSE8 (source, model, matrix, params, ungapResult);
    }
    else
    {
        result = new SmallGapHitIteratorSSE8 (source, model, matrix, params, ungapResult);
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
    IAlignmentResult*       ungapResult,
    IAlignmentResult*       alignmentResult
)
{
    IHitIterator* result = 0;

    /** We retrieve the property. */
    IProperty* prop = _properties->getProperty (STR_OPTION_FACTORY_HIT_FULLGAP);

    if (prop && prop->value.compare("FullGapHitIterator")==0)
    {
        result = new FullGapHitIterator (
            source,
            model,
            matrix,
            params,
            queryInfo,
            globalStats,
            ungapResult,
            alignmentResult
        );
    }
    if (prop && prop->value.compare("FullGapHitIteratorNull")==0)
    {
        result = new FullGapHitIteratorNull (
            source,
            model,
            matrix,
            params,
            queryInfo,
            globalStats,
            ungapResult,
            alignmentResult
        );
    }
    else
    {
        result = new FullGapHitIterator (
            source,
            model,
            matrix,
            params,
            queryInfo,
            globalStats,
            ungapResult,
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
indexation::IHitIterator* DefaultConfiguration::createCompositionHitIterator  (
    indexation::IHitIterator*       source,
    seed::ISeedModel*               model,
    algo::IScoreMatrix*             matrix,
    algo::IParameters*              params,
    statistics::IQueryInformation*  queryInfo,
    statistics::IGlobalParameters*  globalStats,
    algo::IAlignmentResult*         ungapResult,
    algo::IAlignmentResult*         alignmentResult
)
{
    IHitIterator* result = 0;

    /** We retrieve the property. */
    IProperty* prop = _properties->getProperty (STR_OPTION_FACTORY_HIT_COMPOSITION);

    if (prop && prop->value.compare("CompositionHitIterator")==0)
    {
        result = new CompositionHitIterator (
            source,
            model,
            matrix,
            params,
            queryInfo,
            globalStats,
            ungapResult,
            alignmentResult
        );
    }
    if (prop && prop->value.compare("CompositionHitIteratorNull")==0)
    {
        result = new CompositionHitIteratorNull (
            source,
            model,
            matrix,
            params,
            queryInfo,
            globalStats,
            ungapResult,
            alignmentResult
        );
    }
    else
    {
        result = new CompositionHitIterator (
            source,
            model,
            matrix,
            params,
            queryInfo,
            globalStats,
            ungapResult,
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
IAlignmentResult* DefaultConfiguration::createGapAlignmentResult  (ISequenceDatabase* subject, ISequenceDatabase* query)
{
    IAlignmentResult* result = 0;

    /** We retrieve the property. */
    IProperty* prop = _properties->getProperty (STR_OPTION_FACTORY_GAP_RESULT);

    if (prop && prop->value.compare("BasicAlignmentResult")==0)
    {
        result = new BasicAlignmentResult (subject, query);
    }
    else
    {
        result = new BasicAlignmentResult (subject, query);
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
IAlignmentResult* DefaultConfiguration::createUnapAlignmentResult (size_t querySize)
{
    IAlignmentResult* result = 0;

    /** We retrieve the property. */
    IProperty* prop = _properties->getProperty (STR_OPTION_FACTORY_UNGAP_RESULT);

    if (prop && prop->value.compare("UngapAlignmentResult")==0)
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
AlignmentResultVisitor* DefaultConfiguration::createResultVisitor ()
{
    IProperty* prop = 0;

    AlignmentResultVisitor* result = 0;

    /** We need an uri. We take the one provided by the properties. */
    string uri ("out");
    if ( (prop = _properties->getProperty (STR_OPTION_OUTPUT_FILE)) != 0)   { uri = prop->value;  }

    if ( (prop = _properties->getProperty (STR_OPTION_OUTPUT_FORMAT)) != 0)
    {
        switch (atoi (prop->value.c_str()))
        {
        case 1:
            result = new  AlignmentResultOutputTabulatedVisitor (uri);
            break;

        case 2:
            result = new  AlignmentResultRawDumpVisitor (uri);
            break;

        case 3:
            result = new  AlignmentResultXmlDumpVisitor (uri);
            break;

        default:
            result = new  AlignmentResultOutputTabulatedVisitor (uri);
            break;
        }
    }
    else
    {
        result = new  AlignmentResultOutputTabulatedVisitor (uri);
    }

    /** We may want to restrict the number of dumped alingments. We use a Proxy (see [GOF94]) for
     *  controlling this aspect.
     */
    IProperty* maxHitsPerQueryProp = _properties->getProperty (STR_OPTION_MAX_HIT_PER_QUERY);
    if (maxHitsPerQueryProp != 0)
    {
        size_t maxHitsPerQuery = atoi (maxHitsPerQueryProp->value.c_str());
        if (maxHitsPerQuery > 0)
        {
            result = new MaxHitsPerQueryAlignmentResultVisitor (result, maxHitsPerQuery);
        }
    }

    /** We return the result. */
    return result;
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
