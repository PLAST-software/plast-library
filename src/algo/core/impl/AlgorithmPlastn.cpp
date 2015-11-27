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

#include <algo/core/impl/AlgorithmPlastn.hpp>

#include <designpattern/impl/RangeIterator.hpp>

#include <alignment/core/impl/HspContainer.hpp>
#include <alignment/core/impl/BasicAlignmentContainer.hpp>

#include <algo/hits/hsp/HspGeneratorCmd.hpp>
#include <algo/hits/hsp/HspExtensionCmd.hpp>
#include <algo/hits/hsp/AlignmentGeneratorCmd.hpp>

#include <alignment/tools/impl/SemiGappedAlign.hpp>
#include <alignment/tools/impl/AlignmentSplitter.hpp>

#include <alignment/visitors/impl/FilterContainerVisitor.hpp>
#include <alignment/visitors/impl/SortContainerVisitor.hpp>
#include <alignment/visitors/impl/ReverseStrandVisitor.hpp>

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
static const char* keyPass0      = "Algo Pass0";
static const char* keyPass1      = "Algo Pass1";
static const char* keyPass2      = "Algo Pass2";

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
    seed::ISeedModel*                               seedModel,
    algo::core::IDatabasesProvider*                 dbProvider,
    algo::core::IIndexator*                         indexator,
    statistics::IGlobalParameters*                  statistics,
    os::impl::TimeInfo*                             timeStats,
    bool&                                           isRunning,
    std::vector<misc::ReadingFrame_e>&              subjectFrames
)
	: AbstractAlgorithm (config, reader, params, filter, resultVisitor, seedModel, dbProvider, indexator, statistics, timeStats, isRunning),
  	  _hspContainer(0), _nbPasses(4), _currentPass(0)
{
    DEBUG (("AlgorithmPlastn::AlgorithmPlastn\n"));

    /** We memorize which frames we want to use. */
    _subjectFrames = subjectFrames;

#if 0
    /** We may have to consider twice the number of passes in case we work on both strands. */
    if (params->strand == 0)
    {
        if (actualStrand==-1)  { _currentPass += _nbPasses; }

        _nbPasses *= 2;
    }
#else
    _nbPasses = 4*subjectFrames.size ();
#endif


    /** We may change the ungap score threshold according to the reward value. */
//    if (_params->ungapScoreThreshold == 0)
//    {
//        /** Empirical formula that optimizes the threshold, compared to blastn. */
//        _params->ungapScoreThreshold = 28 * _params->reward - 1;
//    }
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

    setHspContainer (0);
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

    /** We need a range iterator for getting successive seeds hashcode ranges. */
    u_int32_t maxSeedsNumber = 1 << (2*getSeedsModel()->getSpan());

    misc::Range<u_int32_t> range (0, maxSeedsNumber - 1);
    RangeIterator<u_int32_t> rangeIterator (range, 1000, DefaultFactory::thread().newSynchronizer());

    timesVec.push_back (DefaultFactory::time().gettime());
    _timeStats->addEntry (keyPass0);

    /**********************************************************************/
    /***************************   PASS 0  ********************************/
    /**********************************************************************/
    setHspContainer (pass0 (subjectDb, queryDb, dispatcher, rangeIterator));

    _timeStats->stopEntry(keyPass0);
    timesVec.push_back (DefaultFactory::time().gettime());

    DEBUG (("AlgorithmPlastn::computeAlignments: PASS 0:  %ld HSP generated in %d msec\n",
		_hspContainer->getItemsNumber(), timesVec[timesVec.size()-1] - timesVec[timesVec.size()-2]
    ));

    /**********************************************************************/
    /***************************   PASS 1  ********************************/
    /**********************************************************************/
    _timeStats->addEntry (keyPass1);
    list<int> xdropoffs;
    //xdropoffs.push_back (_params->XdroppofGap / 2);
    xdropoffs.push_back (_params->XdroppofGap);
    xdropoffs.push_back (_params->finalXdroppofGap);

    for (list<int>::iterator it = xdropoffs.begin(); it != xdropoffs.end(); ++it)
    {
        _currentPass++;
        setHspContainer (pass1 (subjectDb, queryDb, dispatcher, _hspContainer, *it));

        timesVec.push_back (DefaultFactory::time().gettime());

        DEBUG (("AlgorithmPlastn::computeAlignments: PASS 1:  %ld HSP generated in %d msec with xdropoff=%d\n",
            _hspContainer->getItemsNumber(), timesVec[timesVec.size()-1] - timesVec[timesVec.size()-2], *it
        ));
    }
    _timeStats->stopEntry(keyPass1);

    /**********************************************************************/
    /***************************   PASS 2  ********************************/
    /**********************************************************************/
    _timeStats->addEntry (keyPass2);
    _currentPass++;
    pass2 (subjectDb, queryDb, dispatcher, _hspContainer, alignmentResult);

    timesVec.push_back (DefaultFactory::time().gettime());
    _timeStats->stopEntry (keyPass2);

    DEBUG (("AlgorithmPlastn::computeAlignments: PASS 2:  %d alignments generated in %d msec\n",
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
    timesVec.push_back (DefaultFactory::time().gettime());

    /** Now, our alignment result instance should hold found alignments, we order the alignments. */
	// SortContainerVisitor sortVisitor;
	// alignmentResult->accept (&sortVisitor);
	// DEBUG (("AlgorithmPlastn::finalizeAlignments : sort done... \n"));

    /** We filter the alignments. */
    FilterContainerVisitor filterVisitor (_filter);
    alignmentResult->accept (&filterVisitor);

    /** We shrink and sort the alignments and the hits. */
    alignmentResult->shrink ();

    /** We create a visitor for dumping the resulting alignments. The used visitor has been provided from a higher layer
     *  but it is likely a 'file dump' visitor that will dump all the alignments into a file. Note by the way that
     *  the actual format of the output file has not to be known here (it could be tabulated columns or xml) and relies
     *  on the actual type of the getResultVisitor. */
    timeStats->addEntry ("output");
    alignmentResult->accept (getResultVisitor());
    timeStats->stopEntry ("output");

    timesVec.push_back (DefaultFactory::time().gettime());
    DEBUG (("AlgorithmPlastn::finalizeAlignments DONE in %d msec\n",
		timesVec[timesVec.size()-1] - timesVec[timesVec.size()-2]
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
IHspContainer* AlgorithmPlastn::pass0 (
	ISequenceDatabase*              subjectDb,
	ISequenceDatabase*              queryDb,
	ICommandDispatcher*             dispatcher,
	RangeIterator<u_int32_t>&       rangeIterator
)
{
	IHspContainer* result = new HspContainer (queryDb->getSize());

	/** Shortcuts. */
    size_t nbcpu = dispatcher->getExecutionUnitsNumber();

    vector<IHspContainer*>  containers (nbcpu);
    for (size_t i=0; i<nbcpu; i++)  {  (containers[i] = new HspContainer (queryDb->getSize()))->use (); }

    list<ICommand*> commands;
    for (size_t i=0; i<nbcpu; i++)
    {
        commands.push_back (new HSPGenerator(
            getIndexator(),
            getQueryInfo(),
            containers[i],
            rangeIterator,
            _params->ungapScoreThreshold,
            _params->reward,
            _params->penalty,
            _params->XdroppofUnGap,
            _params->index_neighbor_threshold,
            //ABS (_params->penalty * 1),
            this
        ));
    }

    /** We dispatch the commands. */
    dispatcher->dispatchCommands (commands, 0);

    /** We merge the collected containers into our result. */
    result->merge (containers);

    /** We release the children containers. */
    for (size_t i=0; i<nbcpu; i++)  {  delete containers[i];  }

    /** We return the result. */
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
IHspContainer* AlgorithmPlastn::pass1 (
	ISequenceDatabase*  subjectDb,
	ISequenceDatabase*  queryDb,
	ICommandDispatcher* dispatcher,
	IHspContainer*		sourceHsp,
	int					xdrop
)
{
	IHspContainer* result = new HspContainer (queryDb->getSize());

	/** Shortcuts. */
    size_t nbcpu = dispatcher->getExecutionUnitsNumber();

    vector<IHspContainer*>  containers (nbcpu);
    for (size_t i=0; i<nbcpu; i++)  {  (containers[i] = new HspContainer (queryDb->getSize()))->use (); }

    list<ICommand*> commands;
    for (size_t i=0; i<nbcpu; i++)
    {
        commands.push_back (new HspExtensionCmd(
            subjectDb,  queryDb,
            getQueryInfo(),
            sourceHsp, containers[i],
            new SemiGapAlign (getScoreMatrix(), _params->openGapCost, _params->extendGapCost, xdrop),
            _params,
            this
        ));
    }
    dispatcher->dispatchCommands (commands, 0);

    /** We merge the collected containers into our result. */
    result->merge (containers);

    /** We release the children containers. */
    for (size_t i=0; i<nbcpu; i++)  {  delete containers[i];  }

    /** We return the result. */
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
void AlgorithmPlastn::pass2 (
	ISequenceDatabase*    subjectDb,
	ISequenceDatabase*    queryDb,
	ICommandDispatcher*   dispatcher,
	IHspContainer*		  sourceHsp,
	IAlignmentContainer*  alignContainer
)
{
	/** Shortcuts. */
    size_t nbcpu = dispatcher->getExecutionUnitsNumber();

    vector<IAlignmentContainer*>  containers (nbcpu);
    for (size_t i=0; i<nbcpu; i++)  {  (containers[i] = _config->createGapAlignmentResult())->use (); }

    list<ICommand*> commands;
    for (size_t i=0; i<nbcpu; i++)
    {
        commands.push_back (new AlignmentGeneratorCmd (
            subjectDb,  queryDb,
            getQueryInfo(),
            getGlobalStatistics(),
            _hspContainer,
            containers[i],
            _config->createAlignmentSplitter (getScoreMatrix(), _params->openGapCost, _params->extendGapCost),
            getScoreMatrix(),
            _params,
            this
        ));
    }
    dispatcher->dispatchCommands (commands, 0);

    /** We merge the collected containers into our result. */
    alignContainer->merge (containers);

    /** We release the children containers. */
    for (size_t i=0; i<nbcpu; i++)  {  delete containers[i];  }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlgorithmPlastn::preTreatment (
    dp::Iterator<database::ISequenceDatabase*>* qryDatabases,
    dp::Iterator<database::ISequenceDatabase*>* sbjDatabases
)
{
    if (_subjectFrames[0] == FRAME_2)
    {
        reverse (sbjDatabases->currentItem());
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
void AlgorithmPlastn::postTreatment (
    dp::Iterator<database::ISequenceDatabase*>* qryDatabases,
    dp::Iterator<database::ISequenceDatabase*>* sbjDatabases
)
{
    if (sbjDatabases->isDone() == false)
    {
        reverse (sbjDatabases->currentItem());
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
void AlgorithmPlastn::reverse (database::ISequenceDatabase* database)
{
    database->reverse ();

    ReverseStrandVisitor* v = dynamic_cast<ReverseStrandVisitor*> (getResultVisitor());
    if (v)   { v->reverse (); }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlgorithmPlastn::update (dp::EventInfo* evt, dp::ISubject* subject)
{
	if (subject != this)
	{
        IterationStatusEvent* e1 = dynamic_cast<IterationStatusEvent*> (evt);
        if (e1 != 0)
        {
        	/** Since we have several successive passes, we rationalize the incoming notifications
        	 *  in order to look like we have only one phase.
        	 */
        	float currentPercent = e1->getTotalNumber() > 0 ? (float)(e1->getCurrentIndex()) / (float)(e1->getTotalNumber())  : 0;

            u_int64_t currentIndex = 100 * (currentPercent + _currentPass);
            u_int64_t totalNumber  = 100 * (_nbPasses);

        	notify (new IterationStatusEvent (ITER_ON_GOING, currentIndex, totalNumber, e1->getMessage(), currentIndex, totalNumber));
        }
        /** We forward the event to potential listeners. */
        this->notify (evt);
	}
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
