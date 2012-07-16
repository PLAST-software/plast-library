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

#include <algo/core/impl/AlgorithmPlastn.hpp>

#include <designpattern/impl/RangeIterator.hpp>

#if 0
#include <alignment/core/impl/HspContainer.hpp>
#include <alignment/core/impl/HspGenerator.hpp>

#include <alignment/core/impl/HspAlignmentGenerator.hpp>
#include <alignment/core/impl/HspAlignmentGenerator2.hpp>
#endif

#include <alignment/tools/impl/SemiGappedAlign.hpp>
#include <alignment/tools/impl/AlignmentSplitter.hpp>

#include <alignment/visitors/impl/FilterContainerVisitor.hpp>

#include <stdio.h>
#define DEBUG(a)  printf a

using namespace std;
using namespace dp;
using namespace dp::impl;
using namespace os;
using namespace os::impl;
using namespace misc;
using namespace database;
using namespace database::impl;
using namespace indexation;
using namespace algo::hits;

using namespace alignment::core;
using namespace alignment::core::impl;

using namespace alignment::tools;
using namespace alignment::tools::impl;

using namespace alignment::visitors;
using namespace alignment::visitors::impl;

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
AlgorithmPlastn::AlgorithmPlastn (
    IConfiguration*                                 config,
    database::IDatabaseQuickReader*                 reader,
    IParameters*                                    params,
    alignment::filter::IAlignmentFilter*            filter,
    alignment::core::IAlignmentContainerVisitor*    resultVisitor,
    algo::core::IDatabasesProvider*                 dbProvider,
    bool&                                           isRunning
)
: AbstractAlgorithm (config, reader, params, filter, resultVisitor, dbProvider, isRunning)
{
    DEBUG (("AlgorithmPlastn::AlgorithmPlastn\n"));

    /** WARNING !  We first switch to nucleotide alphabet before creating the instance. */
    EncodingManager::singleton().setKind (EncodingManager::ALPHABET_NUCLEOTID);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AlgorithmPlastn::~AlgorithmPlastn ()
{
    DEBUG (("AlgorithmPlastn::~AlgorithmPlastn\n"));

    /** WARNING !  We switch back to amino acid alphabet. */
    EncodingManager::singleton().setKind (EncodingManager::ALPHABET_AMINO_ACID);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlgorithmPlastn::computeAlignments (
    IAlignmentContainer*   alignmentResult,
    IAlignmentContainer*   ungapAlignmentResult,
    ISequenceDatabase*     subjectDb,
    ISequenceDatabase*     queryDb,
    ICommandDispatcher*    dispatcher,
    TimeInfo*              timeStats
)
{
    int32_t MATCH     = 1;
    int32_t MISMATCH  = 2;
    int32_t XDROP     = 1*MISMATCH;
    int32_t THRESHOLD = 27;

    DEBUG (("AlgorithmPlastn::computeAlignments BEGIN\n"));
#if 0
    IHspContainer* hspContainer = new HspContainer (queryDb->getSize());
    LOCAL (hspContainer);

    IHspContainer* hspContainer2 = new HspContainer (queryDb->getSize());
    LOCAL (hspContainer2);

    IHspContainer* hspContainer3 = new HspContainer (queryDb->getSize());
    LOCAL (hspContainer3);

    IHspContainer* hspOther = new HspContainer (queryDb->getSize());
    LOCAL (hspOther);


    /** We need a range iterator for getting successive seeds hashcode ranges. */
    u_int32_t maxSeedsNumber = 1 << (2*getSeedsModel()->getSpan());
    misc::Range<u_int32_t> range (0, maxSeedsNumber - 1);
    RangeIterator<u_int32_t> rangeIterator (range, 100, DefaultFactory::thread().newSynchronizer());

    size_t nbcpu = dispatcher->getExecutionUnitsNumber();

    vector<u_int32_t> timesVec;
    list<ICommand*> commands;

    timesVec.push_back (DefaultFactory::time().gettime());

    /**********************************************************************/
    /***************************   PASS 0  ********************************/
    /**********************************************************************/
    commands.clear ();
    for (size_t i=0; i<nbcpu; i++)
    {
        commands.push_back (new HSPGenerator(
            getIndexator(),  hspContainer,  rangeIterator,
            THRESHOLD, MATCH, MISMATCH, XDROP
        ));
    }

    /** We dispatch the commands. */
    dispatcher->dispatchCommands (commands, 0);

    timesVec.push_back (DefaultFactory::time().gettime());

    DEBUG (("AlgorithmPlastn::computeAlignments: PASS 0: %ld HSP generated in %d msec\n",
        hspContainer->getItemsNumber(),
        timesVec[timesVec.size()-1] - timesVec[timesVec.size()-2]
    ));

#if 1
    /**********************************************************************/
    /***************************   PASS 1  ********************************/
    /**********************************************************************/
    commands.clear();
    for (size_t i=0; i<nbcpu; i++)
    {
        commands.push_back (new HspAlignmentGenerator(
            subjectDb,  queryDb,
            getQueryInfo(),
            getGlobalStatistics(),
            hspContainer, hspContainer2, hspOther,
            new SemiGapAlign (getScoreMatrix(), _params->openGapCost, _params->extendGapCost, _params->XdroppofGap),
            getScoreMatrix(),
            _params
        ));
    }
    dispatcher->dispatchCommands (commands, 0);

    timesVec.push_back (DefaultFactory::time().gettime());

    DEBUG (("AlgorithmPlastn::computeAlignments: PASS 1: %ld HSP generated in %d msec\n",
        hspContainer2->getItemsNumber(),
        timesVec[timesVec.size()-1] - timesVec[timesVec.size()-2]
    ));

#endif

#if 1
    /**********************************************************************/
    /***************************   PASS 2  ********************************/
    /**********************************************************************/
    commands.clear();
    for (size_t i=0; i<nbcpu; i++)
    {
        commands.push_back (new HspAlignmentGenerator(
            subjectDb,  queryDb,
            getQueryInfo(),
            getGlobalStatistics(),
            hspContainer2, hspContainer3, hspOther,
            new SemiGapAlign (getScoreMatrix(), _params->openGapCost, _params->extendGapCost, _params->finalXdroppofGap),
            getScoreMatrix(),
            _params
        ));
    }
    dispatcher->dispatchCommands (commands, 0);

    timesVec.push_back (DefaultFactory::time().gettime());

    DEBUG (("AlgorithmPlastn::computeAlignments: PASS 2: %ld HSP generated in %d msec\n",
        hspContainer3->getItemsNumber(),
        timesVec[timesVec.size()-1] - timesVec[timesVec.size()-2]
    ));

#endif

#if 1
    /**********************************************************************/
    /***************************   PASS 3  ********************************/
    /**********************************************************************/
    commands.clear();
    for (size_t i=0; i<nbcpu; i++)
    {
        commands.push_back (new HspAlignmentGenerator2(
            subjectDb,  queryDb,
            getQueryInfo(),
            getGlobalStatistics(),
            hspContainer3,  alignmentResult,
            new SemiGapAlign (getScoreMatrix(), _params->openGapCost, _params->extendGapCost, _params->finalXdroppofGap),
            getScoreMatrix(),
            _params
        ));
    }
    hspContainer = hspContainer2;
    dispatcher->dispatchCommands (commands, 0);

    timesVec.push_back (DefaultFactory::time().gettime());

    DEBUG (("AlgorithmPlastn::computeAlignments: PASS 3: %d alignments generated in %d msec\n",
        alignmentResult->getAlignmentsNumber(),
        timesVec[timesVec.size()-1] - timesVec[timesVec.size()-2]
    ));

#endif

    /**********************************************************************/
    /***************************   FINISH  ********************************/
    /**********************************************************************/

    DEBUG (("\n===> FOUND %d alignments in total time %d msec\n\n",
        alignmentResult->getAlignmentsNumber(),
        timesVec[timesVec.size()-1] - timesVec[0]
    ));

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
void AlgorithmPlastn::finalizeAlignments (alignment::core::IAlignmentContainer* alignmentResult, os::impl::TimeInfo* timeStats)
{
    DEBUG (("AlgorithmPlastn::finalizeAlignments BEGIN\n"));

    /** We filter the alignments. */
    FilterContainerVisitor filterVisitor (_filter);
    alignmentResult->accept (&filterVisitor);
    DEBUG (("AbstractAlgorithm::finalizeAlignments : filtering done...\n"));

    /** We create a visitor for dumping the resulting alignments. The used visitor has been provided from a higher layer
     *  but it is likely a 'file dump' visitor that will dump all the alignments into a file. Note by the way that
     *  the actual format of the output file has not to be known here (it could be tabulated columns or xml) and relies
     *  on the actual type of the getResultVisitor. */
    timeStats->addEntry ("output");
    alignmentResult->accept (getResultVisitor());
    timeStats->stopEntry ("output");
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
