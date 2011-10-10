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

#include "DefaultAlgoFactories.hpp"

#include "BasicSeedModel.hpp"
#include "SubSeedModel.hpp"

#include "ScoreMatrix.hpp"

#include "AlgoInitializer.hpp"

#include "UngapHitIterator.hpp"
#include "UngapHitIteratorSSE8.hpp"
#include "UngapHitIteratorSSE16.hpp"
#include "UngapHitIteratorNull.hpp"

#include "SmallGapHitIterator.hpp"
#include "SmallGapHitIteratorSSE8.hpp"
#include "SmallGapHitIteratorNull.hpp"

#include "CommandDispatcher.hpp"

using namespace std;
using namespace os;
using namespace dp;
using namespace database;
using namespace seed;
using namespace indexation;

#include <stdarg.h>

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
DefaultAlgoFactories::DefaultAlgoFactories () :
    _algoInitializerKind    (ENUM_AlgoInitializerSortedSeeds),
    _ungapIteratorKind      (ENUM_UngapHitIteratorSSE16),
    _smallGapIteratorKind   (ENUM_SmallGapHitIteratorSSE8),
    _dispatcherKind         (ENUM_DispatcherParallel)
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
IParameters* DefaultAlgoFactories::createParameters (const char* algoname)
{
    IParameters* result = new IParameters ();

    if (strcmp (algoname, "plastp")==0)
    {
        result->seedModelKind = ENUM_SubSeedModel;
        result->seedSpan      = 4;
        result->subseedStrings.push_back ("H,FY,W,IV,LM,C,RK,Q,E,N,D,A,S,T,G,P");
        result->subseedStrings.push_back ("HFYWIVLMC,RKQENDASTGP");
        result->subseedStrings.push_back ("H,FYW,IVLM,C,RK,QE,ND,A,ST,G,P");
        result->subseedStrings.push_back ("H,FY,W,IV,LM,C,R,K,Q,E,N,D,A,S,T,G,P");

        result->matrixKind           = ENUM_BLOSUM62;
        result->subjectUri           = string ("tursiops.fa");
        result->queryUri             = string ("query.fa");
        result->filterQuery          = true;
        result->ungapNeighbourLength = 22;
        result->ungapScoreThreshold  = 38;
        result->smallGapBandLength   = 64;
        result->smallGapBandWidth    = 16;
        result->smallGapThreshold    = 54;
        result->openGapCost          = 11;
        result->extendGapCost        = 1;
        result->evalue               = 10.0;
    }

    else if (strcmp (algoname, "tplastn")==0)
    {
        result->seedModelKind = ENUM_SubSeedModel;
        result->seedSpan      = 4;
        result->subseedStrings.push_back ("A,C,D,E,F,G,H,I,K,L,M,N,P,Q,R,S,T,V,W,Y");
        result->subseedStrings.push_back ("CFYWMLIV,GPATSNHQEDRK");
        result->subseedStrings.push_back ("A,C,FYW,G,IV,ML,NH,P,QED,RK,TS");
        result->subseedStrings.push_back ("A,C,D,E,F,G,H,I,K,L,M,N,P,Q,R,S,T,V,W,Y");

        result->matrixKind           = ENUM_BLOSUM62;
        result->subjectUri           = string ("sapiens_1Mo.fa");
        result->queryUri             = string ("tursiops.fa");
        result->filterQuery          = true;
        result->ungapNeighbourLength = 22;
        result->ungapScoreThreshold  = 38;
        result->smallGapBandLength   = 64;
        result->smallGapBandWidth    = 16;
        result->smallGapThreshold    = 40;
        result->openGapCost          = 11;
        result->extendGapCost        = 1;
        result->evalue               = 10.0;
    }

    else if (strcmp (algoname, "plastx")==0)
    {
        result->seedModelKind = ENUM_SubSeedModel;
        result->seedSpan      = 4;
        result->subseedStrings.push_back ("H,FY,W,IV,L,M,C,R,K,Q,E,N,D,A,S,T,G,P");
        result->subseedStrings.push_back ("HFYWIVLMC,RKQENDASTGP");
        result->subseedStrings.push_back ("H,FYW,IVLM,C,RK,QE,ND,A,ST,G,P");
        result->subseedStrings.push_back ("H,FY,W,I,V,L,M,C,R,K,Q,E,N,D,A,S,T,G,P");

        result->matrixKind           = ENUM_BLOSUM62;
        result->subjectUri           = string ("tursiops.fa");
        result->queryUri             = string ("sapiens_1Mo.fa");
        result->filterQuery          = true;
        result->ungapNeighbourLength = 22;
        result->ungapScoreThreshold  = 38;
        result->smallGapBandLength   = 64;
        result->smallGapBandWidth    = 16;
        result->smallGapThreshold    = 54;
        result->openGapCost          = 11;
        result->extendGapCost        = 1;
        result->evalue               = 10.0;
    }

    else if (strcmp (algoname, "tplastx")==0)
    {
        // TO BE DONE...
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
ISeedModel* DefaultAlgoFactories::createSeedModel (SeedModelKind_e kind, size_t span, ...)
{
    ISeedModel* result = 0;

    switch (kind)
    {
        case ENUM_BasicSeedModel:
            result = new BasicSeedModel (SUBSEED, span);
            break;

        case ENUM_SubSeedModel:
        {
            va_list va;

            va_start (va, span);
            result = new SubSeedModel (span, va);
            va_end (va);
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
IScoreMatrix* DefaultAlgoFactories::createScoreMatrix (
    ScoreMatrixKind_e matrixKind,
    database::Encoding encoding
)
{
    IScoreMatrix* result = 0;

    switch (matrixKind)
    {
        case ENUM_BLOSUM62:
            result = ScoreMatrixManager::singleton().getMatrix ("BLOSUM62", SUBSEED);
            break;

        case ENUM_BLOSUM50:
        {
            result = ScoreMatrixManager::singleton().getMatrix ("BLOSUM50", SUBSEED);
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
IAlgoConfigurator* DefaultAlgoFactories::createAlgoConfigurator (
    algo::IParameters*  parameters,
    dp::ICommandDispatcher* dispatcher
)
{
    IAlgoConfigurator* result = 0;

    switch (_algoInitializerKind)
    {
        case ENUM_AlgoInitializer:
            result = new AlgoConfigurator (parameters, dispatcher);
            break;

        case ENUM_AlgoInitializerSortedSeeds:
            result = new AlgoInitializerWithSortedSeeds (parameters, dispatcher);
            break;

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
IHitIterator* DefaultAlgoFactories::createUngapHitIterator (
    indexation::IHitIterator*   inputIterator,
    algo::IAlgoConfigurator*    init
)
{
    IHitIterator* result = 0;

    switch (_ungapIteratorKind)
    {
        case ENUM_UngapHitIterator:
            result = new UngapHitIterator  (
                inputIterator,
                init->getSeedModel(),
                init->getScoreMatrix(),
                init->getParameters(),
                0
            );
            break;

        case ENUM_UngapHitIteratorSSE16:
            result = new UngapHitIteratorSSE16 (
                inputIterator,
                init->getSeedModel(),
                init->getScoreMatrix(),
                init->getParameters(),
                0
            );
            break;

        case ENUM_UngapHitIteratorSSE8:
            result = new UngapHitIteratorSSE8 (
                inputIterator,
                init->getSeedModel(),
                init->getScoreMatrix(),
                init->getParameters(),
                0
            );
            break;

        case ENUM_UngapHitIteratorNull:
            result = new UngapHitIteratorNull (
                inputIterator,
                init->getSeedModel(),
                init->getScoreMatrix(),
                init->getParameters(),
                0
            );
            break;

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
IHitIterator* DefaultAlgoFactories::createSmallGapHitIterator (
    indexation::IHitIterator*   inputIterator,
    algo::IAlgoConfigurator*     init
)
{
    IHitIterator* result = 0;

    switch (_smallGapIteratorKind)
    {
        case ENUM_SmallGapHitIterator:
            result = new SmallGapHitIterator (
                    inputIterator,
                    init->getSeedModel(),
                    init->getScoreMatrix(),
                    init->getParameters(),
                    0
                );
            break;

        case ENUM_SmallGapHitIteratorSSE8:
            result = new SmallGapHitIteratorSSE8 (
                    inputIterator,
                    init->getSeedModel(),
                    init->getScoreMatrix(),
                    init->getParameters(),
                    0
                );
            break;

        case ENUM_SmallGapHitIteratorNull:
            result = new SmallGapHitIteratorNull (
                    inputIterator,
                    init->getSeedModel(),
                    init->getScoreMatrix(),
                    init->getParameters(),
                    0
                );
            break;

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
dp::ICommandDispatcher* DefaultAlgoFactories::createCommandDispatcher ()
{
    dp::ICommandDispatcher* result = 0;

    switch (_dispatcherKind)
    {
        case ENUM_DispatcherSerial:
            result = new SerialCommandDispatcher ();
            break;

        case ENUM_DispatcherParallel:
            result = new ParallelCommandDispatcher (DefaultCommandInvokerFactory::singleton());
            break;

        default:
            /** We should not be here... */
            break;
    }

    return result;

}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
