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

#ifndef _DEFAULT_ALGO_FACTORIES_HPP_
#define _DEFAULT_ALGO_FACTORIES_HPP_

/********************************************************************************/

#include "IAlgoFactories.hpp"
#include "IAlgoParameters.hpp"

/********************************************************************************/
namespace algo  {
/********************************************************************************/

class DefaultAlgoFactories : public IAlgoFactories
{
public:

    /** Singleton. */
    static DefaultAlgoFactories& singleton()  { static DefaultAlgoFactories instance; return instance; }

    /** Implementation of IAlgoFactories interface. */
    void setAlgoInitializerKind     (AlgoInitializerKind_e     val)   { _algoInitializerKind  = val; }
    void setUngapHitIteratorKind    (UngapHitIteratorKind_e    val)   { _ungapIteratorKind    = val; }
    void setSmallGapHitIteratorKind (SmallGapHitIteratorKind_e val)   { _smallGapIteratorKind = val; }
    void setDispatcherKind          (CommandDispatcherKind_e   val)   { _dispatcherKind       = val; }

    AlgoInitializerKind_e       getAlgoInitializerKind     ()   { return _algoInitializerKind;  }
    UngapHitIteratorKind_e      getUngapHitIteratorKind    ()   { return _ungapIteratorKind;  }
    SmallGapHitIteratorKind_e   getSmallGapHitIteratorKind ()   { return _smallGapIteratorKind;  }
    CommandDispatcherKind_e     getDispatcherKind          ()   { return _dispatcherKind;  }

    /** Implementation of IAlgoFactories interface. */
    IParameters* createParameters (const char* algoname);

    /** Implementation of IAlgoFactories interface. */
    seed::ISeedModel* createSeedModel (SeedModelKind_e kind, size_t span, ...);

    IScoreMatrix* createScoreMatrix (ScoreMatrixKind_e matrixKind, database::Encoding encoding);

    IAlgoConfigurator* createAlgoConfigurator (
        algo::IParameters*  parameters,
        dp::ICommandDispatcher* indexBuildDispatcher
    );

    /** */
    indexation::IHitIterator* createUngapHitIterator(
        indexation::IHitIterator*   inputIterator,
        algo::IAlgoConfigurator*     algoInitializer
    );

    /** */
    indexation::IHitIterator* createSmallGapHitIterator(
        indexation::IHitIterator*   inputIterator,
        algo::IAlgoConfigurator*     algoInitializer
    );

    dp::ICommandDispatcher* createCommandDispatcher ();

private:

    DefaultAlgoFactories ();
    virtual ~DefaultAlgoFactories () {}

    AlgoInitializerKind_e     _algoInitializerKind;
    UngapHitIteratorKind_e    _ungapIteratorKind;
    SmallGapHitIteratorKind_e _smallGapIteratorKind;
    CommandDispatcherKind_e   _dispatcherKind;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _DEFAULT_ALGO_FACTORIES_HPP_ */
