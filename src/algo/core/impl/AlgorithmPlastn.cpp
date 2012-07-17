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

#include <alignment/core/impl/HspContainer.hpp>
#include <algo/hits/hsp/HspGeneratorCmd.hpp>
#include <algo/hits/hsp/HspExtensionCmd.hpp>
#include <algo/hits/hsp/AlignmentGeneratorCmd.hpp>

#include <alignment/tools/impl/SemiGappedAlign.hpp>
#include <alignment/tools/impl/AlignmentSplitter.hpp>

#include <alignment/visitors/impl/FilterContainerVisitor.hpp>

#include <stdio.h>
#define DEBUG(a)  //printf a

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
using namespace algo::hits::hsp;

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
    DEBUG (("AlgorithmPlastn::computeAlignments BEGIN\n"));

    IHspContainer* hspContainerPass0 = new HspContainer (queryDb->getSize());
    hspContainerPass0->use ();

    IHspContainer* hspContainerPass1 = new HspContainer (queryDb->getSize());
    hspContainerPass1->use ();

    IHspContainer* hspContainerPass2 = new HspContainer (queryDb->getSize());
    hspContainerPass2->use ();

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
            getIndexator(),  hspContainerPass0,  rangeIterator,
            _params->ungapScoreThreshold,
            _params->reward,
            _params->penalty,
            ABS (_params->penalty * 1)
        ));
    }

    /** We dispatch the commands. */
    dispatcher->dispatchCommands (commands, 0);

    timesVec.push_back (DefaultFactory::time().gettime());

    DEBUG (("AlgorithmPlastn::computeAlignments: PASS 0: %ld HSP generated in %d msec\n",
        hspContainerPass0->getItemsNumber(),
        timesVec[timesVec.size()-1] - timesVec[timesVec.size()-2]
    ));

    /**********************************************************************/
    /***************************   PASS 1  ********************************/
    /**********************************************************************/
    commands.clear();
    for (size_t i=0; i<nbcpu; i++)
    {
        commands.push_back (new HspExtensionCmd(
            subjectDb,  queryDb,
            getQueryInfo(),
            hspContainerPass0, hspContainerPass1,
            new SemiGapAlign (getScoreMatrix(), _params->openGapCost, _params->extendGapCost, _params->XdroppofGap),
            _params
        ));
    }
    dispatcher->dispatchCommands (commands, 0);

    /** We get rid of the input HSP container. */
    hspContainerPass0->forget ();

    timesVec.push_back (DefaultFactory::time().gettime());

    DEBUG (("AlgorithmPlastn::computeAlignments: PASS 1: %ld HSP generated in %d msec\n",
        hspContainerPass1->getItemsNumber(),
        timesVec[timesVec.size()-1] - timesVec[timesVec.size()-2]
    ));

    /**********************************************************************/
    /***************************   PASS 2  ********************************/
    /**********************************************************************/
    commands.clear();
    for (size_t i=0; i<nbcpu; i++)
    {
        commands.push_back (new HspExtensionCmd(
            subjectDb,  queryDb,
            getQueryInfo(),
            hspContainerPass1, hspContainerPass2,
            new SemiGapAlign (getScoreMatrix(), _params->openGapCost, _params->extendGapCost, _params->finalXdroppofGap),
            _params
        ));
    }
    dispatcher->dispatchCommands (commands, 0);

    /** We get rid of the input HSP container. */
    hspContainerPass1->forget ();

    timesVec.push_back (DefaultFactory::time().gettime());

    DEBUG (("AlgorithmPlastn::computeAlignments: PASS 2: %ld HSP generated in %d msec\n",
        hspContainerPass2->getItemsNumber(),
        timesVec[timesVec.size()-1] - timesVec[timesVec.size()-2]
    ));

    /**********************************************************************/
    /***************************   PASS 3  ********************************/
    /**********************************************************************/
    commands.clear();
    for (size_t i=0; i<nbcpu; i++)
    {
        commands.push_back (new AlignmentGeneratorCmd(
            subjectDb,  queryDb,
            getQueryInfo(),
            getGlobalStatistics(),
            hspContainerPass2,
            alignmentResult,
            getScoreMatrix(),
            _params
        ));
    }
    dispatcher->dispatchCommands (commands, 0);

    /** We get rid of the input HSP container. */
    hspContainerPass2->forget ();

    timesVec.push_back (DefaultFactory::time().gettime());

    DEBUG (("AlgorithmPlastn::computeAlignments: PASS 3: %d alignments generated in %d msec\n",
        alignmentResult->getAlignmentsNumber(),
        timesVec[timesVec.size()-1] - timesVec[timesVec.size()-2]
    ));

    /**********************************************************************/
    /***************************   FINISH  ********************************/
    /**********************************************************************/

    DEBUG (("\n===> FOUND %d alignments in total time %d msec\n\n",
        alignmentResult->getAlignmentsNumber(),
        timesVec[timesVec.size()-1] - timesVec[0]
    ));
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
