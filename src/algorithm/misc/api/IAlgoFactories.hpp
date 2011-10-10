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


#ifndef _IALGO_FACTORIES_HPP_
#define _IALGO_FACTORIES_HPP_

/********************************************************************************/

#include "SmartPointer.hpp"

#include "IAlgoTypes.hpp"

#include "IHitIterator.hpp"
#include "ISequenceDatabase.hpp"
#include "ISeedModel.hpp"
#include "IScoreMatrix.hpp"
#include "IDatabaseIndex.hpp"
#include "ICommand.hpp"
#include "IAlgoInitializer.hpp"
#include "ICommand.hpp"

#include "IAlgoParameters.hpp"

#include <iostream>

/********************************************************************************/
namespace algo  {
/********************************************************************************/

class IAlgoFactories : public dp::SmartPointer
{
public:

    /********************************************************************************/

    virtual void setAlgoInitializerKind     (AlgoInitializerKind_e     val) = 0;
    virtual void setUngapHitIteratorKind    (UngapHitIteratorKind_e    val) = 0;
    virtual void setSmallGapHitIteratorKind (SmallGapHitIteratorKind_e val) = 0;
    virtual void setDispatcherKind          (CommandDispatcherKind_e   val) = 0;

    virtual AlgoInitializerKind_e       getAlgoInitializerKind     () = 0;
    virtual UngapHitIteratorKind_e      getUngapHitIteratorKind    () = 0;
    virtual SmallGapHitIteratorKind_e   getSmallGapHitIteratorKind () = 0;
    virtual CommandDispatcherKind_e     getDispatcherKind          () = 0;

    /********************************************************************************/

    virtual IParameters* createParameters (const char* algoname) = 0;

    virtual seed::ISeedModel* createSeedModel (SeedModelKind_e kind, size_t span, ...) = 0;

    virtual IScoreMatrix* createScoreMatrix (ScoreMatrixKind_e matrixKind, database::Encoding encoding) = 0;

    virtual IAlgoConfigurator* createAlgoConfigurator (
        algo::IParameters*  parameters,
        dp::ICommandDispatcher* indexBuildDispatcher
    ) = 0;

    virtual indexation::IHitIterator* createUngapHitIterator (
        indexation::IHitIterator*   previousIterator,
        algo::IAlgoConfigurator*     algoInitializer
    ) = 0;

    virtual indexation::IHitIterator* createSmallGapHitIterator (
        indexation::IHitIterator*   previousIterator,
        algo::IAlgoConfigurator*     algoInitializer
    ) = 0;

    virtual dp::ICommandDispatcher* createCommandDispatcher () = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _IALGO_FACTORIES_HPP_ */
