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

#include <os/impl/DefaultOsFactory.hpp>

#include <designpattern/impl/ListIterator.hpp>
#include <designpattern/impl/ProductIterator.hpp>
#include <designpattern/impl/CommandDispatcher.hpp>

#include <database/impl/ReadingFrameSequenceDatabase.hpp>
#include <database/impl/CompositeSequenceDatabase.hpp>
#include <database/impl/FastaDatabaseQuickReader.hpp>

#include <algo/core/impl/AbstractAlgorithm.hpp>
#include <algo/core/impl/DatabasesProvider.hpp>

#include <algo/core/api/IAlgoEvents.hpp>

#include <alignment/visitors/impl/ShrinkContainerVisitor.hpp>
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

using namespace statistics;

using namespace alignment::core;
using namespace alignment::filter;
using namespace alignment::visitors::impl;

/********************************************************************************/
namespace algo {
namespace core {
namespace impl {
/********************************************************************************/

ReadingFrame_e AbstractAlgorithm::allframes[]    = {FRAME_1, FRAME_2, FRAME_3, FRAME_4, FRAME_5, FRAME_6};
ReadingFrame_e AbstractAlgorithm::topframes[]    = {FRAME_1, FRAME_2, FRAME_3};
ReadingFrame_e AbstractAlgorithm::bottomframes[] = {FRAME_4, FRAME_5, FRAME_6};

static const char* keyRead      = "reading";
static const char* keyIndex     = "indexation";
static const char* keyIter      = "iteration";
static const char* keyOutput    = "output";
static const char* keyAlgorithm = "algorithm";

/********************************************************************************/

/** Command that launch 'iterate' method on a IHitIterator instance. Using a Command allows to
 *  launch this iteration in a specific thread.*/
class HitIterationCommand : public ICommand
{
public:
    HitIterationCommand (IHitIterator* it, void* client, Iterator<Hit*>::Method method)
        :  _it(it), _client(client), _method(method) {  if (_it)  { _it->use(); }  }
    virtual ~HitIterationCommand ()  {  if (_it)  { _it->forget(); } }
    void execute ()  {  _it->iterate (_client, _method);  }
private:
    IHitIterator*          _it;
    void*                  _client;
    Iterator<Hit*>::Method _method;
};

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AbstractAlgorithm::AbstractAlgorithm (
    IConfiguration*             config,
    IDatabaseQuickReader*       reader,
    IParameters*                params,
    IAlignmentFilter*           filter,
    IAlignmentContainerVisitor* resultVisitor,
    IDatabasesProvider*         dbProvider,
    IGlobalParameters*          globalStats,
    os::impl::TimeInfo*         timeStats,
    bool&                       isRunning
)
    : _config(0), _reader(0), _params(0), _filter(0), _resultVisitor(0), _dbProvider(0),
      _seedsModel(0), _scoreMatrix(0), _globalStats(0), _queryInfo(0),
      _indexator(0), _hitIterator(0), _timeStats(0),
      _ungapAlignmentResult(0), _gapAlignmentResult(0),
      _isRunning (isRunning)
{
    /** We use the provided arguments. */
    setConfig            (config);
    setReader            (reader);
    setParams            (params);
    setFilter            (filter);
    setResultVisitor     (resultVisitor);
    setDatabasesProvider (dbProvider);
    setTimeStats         (timeStats);

    /** We create the seeds model. */
    setSeedsModel (getConfig()->createSeedModel (
        getParams()->seedModelKind,
        getParams()->seedSpan,
        getParams()->subseedStrings
    ));

    /** We set the score matrix. */
    setScoreMatrix (getConfig()->createScoreMatrix (
        getParams()->matrixKind,
        SUBSEED,
        getParams()->reward,
        getParams()->penalty
    ));

    /** We set the global statistic parameters. */
    setGlobalStatistics (globalStats);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AbstractAlgorithm::~AbstractAlgorithm (void)
{
    DEBUG (("AbstractAlgorithm::~AbstractAlgorithm : releasing instances.\n"));

    /** We get rid of the used instances. */
    setConfig            (0);
    setReader            (0);
    setParams            (0);
    setFilter            (0);
    setResultVisitor     (0);
    setDatabasesProvider (0);
    setSeedsModel        (0);
    setScoreMatrix       (0);
    setGlobalStatistics  (0);
    setQueryInfo         (0);
    setIndexator         (0);
    setHitIterator       (0);
    setTimeStats         (0);

    setUngapAlignmentResult (0);
    setGapAlignmentResult   (0);

    DEBUG (("AbstractAlgorithm::~AbstractAlgorithm : releasing instances  DONE.\n"));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AbstractAlgorithm::execute (void)
{
    /** Here is the heart of the plast algorithm. We create all needed objects, configure
     *  them and launch the algorithm which consists in iterating over hits through different
     *  steps, each step filtering out hits.
     *
     *  The result of the algorithm is a IAlignmentResult instance that is visited by some Visitor
     *  (usually for dumping alignments into a file).
     *
     *  The algorithm is named 'abstract' because it provides a skeleton of the algorithm, some
     *  parts of the algorithm can be refined in sub classes. Therefore, this 'execute' method
     *  can be seen as a Template Method (template primitives are for instance 'createHitIterator'
     *  and 'createDatabaseIterator')
     *
     *  Note also that many instances are created through the IConfiguration instance, so the behavior
     *  of the algorithm may change according to the kind of configuration is used.
     */

    /** We want to have some time statistics Note that we need here specific TimeInfo instances here that are
     *  1) retrieve time information for statistics and
     *  2) send some information notification for telling to potential listeners that we begin some
     *  particular part of the algorithm.
     */

    DEBUG (("AbstractAlgorithm::execute : starting...\n"));

    /** We create a command dispatcher used for indexation commands. */
    ICommandDispatcher* indexationDispatcher = getConfig()->createIndexationDispatcher();
    LOCAL (indexationDispatcher);

    /** We create a command dispatcher used for dispatching Hit iteration commands. */
    ICommandDispatcher* dispatcher = getConfig()->createDispatcher ();
    LOCAL (dispatcher);

    _timeStats->addEntry (keyRead);

    /** We create the subject database (more than one for tplastn) and the
     *  query database   (more than one for plastx) */
    getDatabasesProvider()->createDatabases (getParams(), getSubjectFrames(), getQueryFrames(), 0, 0);

    /** We retrieve two iterators for the subject and query databases. */
    ListIterator<ISequenceDatabase*> subjectDbIt = getDatabasesProvider()->getSubjectDbIterator();
    ListIterator<ISequenceDatabase*> queryDbIt   = getDatabasesProvider()->getQueryDbIterator();

    _timeStats->stopEntry (keyRead);

    DEBUG (("AbstractAlgorithm::execute : databases created...\n"));

    /** We notify some report to potential listeners. */
    this->notify (new AlgorithmReadingFrameEvent (this, getSubjectFrames(), getQueryFrames()));

    /** We create an object for indexing subject and query databases. This object will be in
     * charge to feed the algorithm with the source Hit Iterator, ie the one that provides for
     * a given seed all the occurrences in subject and query databases.
     */
    setIndexator (getConfig()->createIndexator (getSeedsModel(), getParams(), _isRunning) );

    DEBUG (("AbstractAlgorithm::execute : indexator created...\n"));

    /** Now, we have two loops that loop on 1) query databases and 2) subject databases.
     *  The reason why we can have more than query database for instance is that the algorithm works
     *  only on protein/protein comparison. So an protein/ADN comparison request is understood as
     *  a comparison between a protein database and 6 possible protein databases, which means that
     *  the used provided query nucleotid database is transformed into 6 amino acids databases which
     *  explains that we can have more than one database for query (cf plastx) and more than one
     *  datase for subject (tplastn).
     */

    /********************************************************************************/
    /**********                     FIRST LOOP ON QUERY PARTS              **********/
    /********************************************************************************/
    for (queryDbIt.first(); !queryDbIt.isDone(); queryDbIt.next())
    {
        DEBUG (("AbstractAlgorithm::execute : QUERY LOOP...\n"));

        /** Shortcuts. */
        ISequenceDatabase* queryDb = queryDbIt.currentItem();

        /** We update the subject/query databases for the indexation. Note that one or two of the databases
         *  may be the same as the previous iteration.
         */
        getIndexator()->setQueryDatabase (queryDb);

        /** We can compute query statistics information. Note that we use the reader (holding information
         *  about subject database) for providing subject database size and its number of sequences. */
        setQueryInfo (getConfig()->createQueryInformation (
            getGlobalStatistics (),
            getParams(),
            queryDb,
            _reader->getDataSize(),
            _reader->getNbSequences()
        ));

        DEBUG (("AbstractAlgorithm::execute : query statistics computed...\n"));

        /********************************************************************************/
        /**********                  SECOND LOOP ON SUBJECT PARTS              **********/
        /********************************************************************************/
        for (subjectDbIt.first(); !subjectDbIt.isDone(); subjectDbIt.next())
        {
            DEBUG (("AbstractAlgorithm::execute : SUBJECT LOOP...\n"));

            _timeStats->addEntry (keyAlgorithm);

            /** Shortcuts. */
            ISequenceDatabase* subjectDb = subjectDbIt.currentItem();

            DEBUG (("AbstractAlgorithm::execute : subjectSeqNb=%ld  querySeqNb=%ld\n",
                subjectDb->getSequencesNumber(),
                queryDb->getSequencesNumber()
            ));

            /** We update the subject/query databases for the indexation. Note that one or two of the databases
             *  may be the same as the previous iteration.
             */
            getIndexator()->setSubjectDatabase (subjectDb);

            /** We want to have indexation execution time statistics. */
            _timeStats->addEntry (keyIndex);

            DEBUG (("AbstractAlgorithm::execute : indexation start...\n"));

            /** We build the indexes (if needed). */
            getIndexator()->build (indexationDispatcher);

            _timeStats->stopEntry (keyIndex);

            DEBUG (("AbstractAlgorithm::execute : indexation finished in %d msec...\n", _timeStats->getEntryByKey(keyIndex) ));

            /** We create an ungap alignment result. This ungap alignment will be shared betweed different Hit
             * iterators, in particular for filtering out already processed hits.
             * Warning! This instance will have concurrent accesses both for reading and writing, so the implementation
             * must be protected by some operating system synchronization procedure (mutex for instance). */
            IAlignmentContainer* ungapAlignmentResult = getConfig()->createUnapAlignmentResult (queryDb->getSize());
            LOCAL (ungapAlignmentResult);
            setUngapAlignmentResult (ungapAlignmentResult);
            DEBUG (("AbstractAlgorithm::execute : ungap alignments container created...\n"));

            /** We create an alignment result. This is the actual artefact generated by the algorithm and of interest for
             *  the end user. Like its ungap brother, it will be concurently accessed and therefore must be protected
             *  specifically. */
            IAlignmentContainer* alignmentResult  = getConfig()->createGapAlignmentResult ();
            LOCAL (alignmentResult);
            setGapAlignmentResult (alignmentResult);
            DEBUG (("AbstractAlgorithm::execute : gap alignments container created...\n"));

            /** Now, everything should be configured, we can compute the alignments. */
            computeAlignments (alignmentResult, ungapAlignmentResult, subjectDb, queryDb, dispatcher, _timeStats);

            DEBUG (("AbstractAlgorithm::execute : dispatching done  nbAlign=%d  firstLevelNb=%d...\n",
                alignmentResult->getAlignmentsNumber(),
                alignmentResult->getFirstLevelNumber()
            ));

            /** We may have specific post treatment on the found alignments (in particular serialization into a file). */
            finalizeAlignments (alignmentResult, _timeStats);

            _timeStats->stopEntry (keyAlgorithm);

            /** We notify some report to potential listeners. */
            this->notify (new AlgorithmReportEvent (
                this,
                subjectDb,
                queryDb,
                _timeStats,
                ungapAlignmentResult,
                alignmentResult,
                _filter
            ) );

        }  /* end of for (subjectDbIt.first(); ... */

    }  /* end of for (queryDbIt.first(); ... */
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AbstractAlgorithm::computeAlignments (
    IAlignmentContainer*    alignmentResult,
    IAlignmentContainer*    ungapAlignmentResult,
    ISequenceDatabase*      subjectDb,
    ISequenceDatabase*      queryDb,
    ICommandDispatcher*     dispatcher,
    TimeInfo*               timeStats
)
{
    /** We create the Hit iterator to be used by the algorithm, a Hit being an occurrence of a kmer in both subject
     *  and query databases.
     *  See the 'createHitIterator' to see how the different created iterators are connected (seed, ungap, small gap...)  */
    setHitIterator (createHitIterator (getConfig(), getIndexator()->createHitIterator(), ungapAlignmentResult, alignmentResult) );

    DEBUG (("AbstractAlgorithm::execute : hits iterator created...\n"));

    /** We split the iterator in several iterators.
      * This split reflects the parallelization scheme of the algorithm, ie several thread will execute their own iterator.
      * In our case, a split iterator will iterate a subset of the whole possible seeds set (given a seeds model). For
      * instance, a thread will deal with seeds 'PQR' 'PQS' 'PQT'..., another one with 'AFG' 'AFH' 'AFI'...
      * Note that the number of splits relies on the number of execution units of the command dispatcher. In particular,
      * this will likely be the number of CPU cores available on the used computer (note however that it could also be
      * the number of nodes of a network dedicated for some grid computing network).
      */
    std::vector<IHitIterator*> its = getHitIterator()->split (dispatcher->getExecutionUnitsNumber());

    DEBUG (("AbstractAlgorithm::execute : hits iterator split...\n"));

    /** We create a list of commands for iterating the hits. */
    list<ICommand*> commands;
    for (size_t i=0; i<its.size(); i++)
    {
        /** We create a new command, each command having a specific Hit iterator (so a specific subset of seeds) */
        commands.push_back (new HitIterationCommand (
            its[i],
            this,
            (Iterator<Hit*>::Method) & AbstractAlgorithm::hitUpdate
        ));
    }

    DEBUG (("AbstractAlgorithm::execute : commands created...\n"));

    /** We want to have iteration execution time. */
    timeStats->addEntry (keyIter);

    DEBUG (("AbstractAlgorithm::execute : dispatching %ld commands for execution...\n", commands.size()));

    /** We run the commands through a dispatcher. Here is the big picture where most of the work will be done.
     *  This is also a synchronization point; if the dispatcher creates many threads for job parallelization (or
     *  if the job has been sent on a network for using remote computers), this 'dispatchCommands' message ensures
     *  that all commands must be finished before we can go to the next instruction.
     */
    dispatcher->dispatchCommands (commands, 0);

    timeStats->stopEntry (keyIter);

    DEBUG (("AbstractAlgorithm::execute : dispatching done  nbAlign=%d  firstLevelNb=%d...\n",
        alignmentResult->getAlignmentsNumber(),
        alignmentResult->getFirstLevelNumber()
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
void AbstractAlgorithm::finalizeAlignments (IAlignmentContainer* alignmentResult, TimeInfo* timeStats)
{
    timeStats->addEntry (keyOutput);

    /** Now, our alignment result instance should hold found alignments, with possible redundancies, so we try to
     * remove redundant alignments now. */
    ShrinkContainerVisitor shrinker (_params->nbAlignPerHit);
    alignmentResult->accept (&shrinker);
    DEBUG (("AbstractAlgorithm::finalizeAlignments : shrink done with nbAlignPerHit=%ld...\n", _params->nbAlignPerHit));

    /** We filter the alignments. */
    FilterContainerVisitor filterVisitor (_filter);
    alignmentResult->accept (&filterVisitor);
    DEBUG (("AbstractAlgorithm::finalizeAlignments : filtering done...\n"));

    /** We create a visitor for dumping the resulting alignments. The used visitor has been provided from a higher layer
     *  but it is likely a 'file dump' visitor that will dump all the alignments into a file. Note by the way that
     *  the actual format of the output file has not to be known here (it could be tabulated columns or xml) and relies
     *  on the actual type of the getResultVisitor. */
    alignmentResult->accept (getResultVisitor());

    timeStats->stopEntry (keyOutput);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IHitIterator* AbstractAlgorithm::createHitIterator (
    IConfiguration*      config,
    IHitIterator*        hitSource,
    IAlignmentContainer* ungapAlignResult,
    IAlignmentContainer* alignResult
)
{
    DEBUG (("AbstractAlgorithm::createHitIterator: config=%p  source=%p\n", config, hitSource));

    /** We create Hit iterators for each algorithm step and link them. Note that different kinds of iterators
     *  may need different information for their construction. */

    IHitIterator* ungapHitIterator = getConfig()->createUngapHitIterator (
        hitSource, getSeedsModel(), getScoreMatrix(), getParams(), ungapAlignResult
    );
    DEBUG (("AbstractAlgorithm::createHitIterator => ungapHitIterator=%p\n", ungapHitIterator));

    IHitIterator* smallGapIterator = getConfig()->createSmallGapHitIterator (
        ungapHitIterator, getSeedsModel(), getScoreMatrix(), getParams(), ungapAlignResult, alignResult
    );
    DEBUG (("AbstractAlgorithm::createHitIterator => smallGapIterator=%p\n", smallGapIterator));

    IHitIterator* fullGapIterator  = getConfig()->createFullGapHitIterator  (
        smallGapIterator,
        getSeedsModel(),
        getScoreMatrix(),
        getParams(),
        getQueryInfo(),
        getGlobalStatistics(),
        ungapAlignResult,
        alignResult
    );

    IHitIterator* compositionHitIterator  = getConfig()->createCompositionHitIterator  (
        fullGapIterator,
        getSeedsModel(),
        getScoreMatrix(),
        getParams(),
        getQueryInfo(),
        getGlobalStatistics(),
        ungapAlignResult,
        alignResult
    );

    /** We subscribe to the seeds hit iteration events. Therefore, we will get as many number
     * of seeds as the seeds model is able to generate (not too many in the end (a few thousands),
     * so we won't be notified too often due to this subscription). The event kind we should
     * receive is 'IterationStatusEvent'
     */
    hitSource->addObserver (this);

    /** We return the result. */
    return compositionHitIterator;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AbstractAlgorithm::update (dp::EventInfo* evt, dp::ISubject* subject)
{
    /** Note this important check: a class can be both Subject and Observer, so if it
     *  notifies an event, it must be sure not to re-send it on the update method, otherwise
     *  and endless recursion begins. */
    if (this != subject)
    {
        IterationStatusEvent* e1 = dynamic_cast<IterationStatusEvent*> (evt);
        if (e1 != 0)
        {
            if (e1->getStatus() == ITER_ON_GOING)
            {
                /** We decorate the event with additional information. */
               notify (new AlignmentProgressionEvent (e1, _ungapAlignmentResult, _gapAlignmentResult));
            }
        }

        /** We forward the event to potential listeners. */
        this->notify (evt);
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
IHitIterator* AlgorithmTplastx::createHitIterator (
    IConfiguration*      config,
    IHitIterator*        hitSource,
    IAlignmentContainer* ungapAlignResult,
    IAlignmentContainer* alignResult
)
{
    IHitIterator* ungapHitIterator = getConfig()->createUngapHitIterator (
        hitSource, getSeedsModel(), getScoreMatrix(), getParams(), ungapAlignResult
    );

    IHitIterator* ungapExtendHitIterator =  getConfig()->createUngapExtendHitIterator (
        ungapHitIterator, getSeedsModel(), getScoreMatrix(), getParams(), alignResult,
        getGlobalStatistics(), getQueryInfo()
    );

    return ungapExtendHitIterator;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AbstractAlgorithm::AlgoTimeInfo::AlgoTimeInfo (AbstractAlgorithm* algo)
    : TimeInfo (DefaultFactory::time()), _algo(algo)
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
void AbstractAlgorithm::AlgoTimeInfo::addEntry (const char* name)
{
    /** We notify some informative event to potential listeners. */
    if (_algo)  { _algo->notify (new AlgoEventWithStatus (name, ENUM_STARTED)); }

    /** We call parent method. */
    TimeInfo::addEntry (name);
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

