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

#include "AbstractAlgorithm.hpp"

#include "ReadingFrameSequenceDatabase.hpp"

#include "ListIterator.hpp"
#include "ProductIterator.hpp"

#include "IAlgoEvents.hpp"

#include "DefaultOsFactory.hpp"

#include "FastaDatabaseQuickReader.hpp"

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace os;
using namespace database;
using namespace dp;
using namespace indexation;


/********************************************************************************/
namespace algo  {
/********************************************************************************/

ReadingFrame_e AbstractAlgorithm::allframes[]    = {FRAME_1, FRAME_2, FRAME_3, FRAME_4, FRAME_5, FRAME_6};
ReadingFrame_e AbstractAlgorithm::topframes[]    = {FRAME_1, FRAME_2, FRAME_3};
ReadingFrame_e AbstractAlgorithm::bottomframes[] = {FRAME_4, FRAME_5, FRAME_6};

/********************************************************************************/
class HitIterationCommand : public ICommand
{
public:
    HitIterationCommand (IHitIterator* it, void* client, Iterator<Hit*>::Method method)
        :  _it(it), _client(client), _method(method), _nbHits(0)  {  if (_it)  { _it->use(); }  }
    virtual ~HitIterationCommand ()  {  if (_it)  { _it->forget(); } }
    void execute ()  {  _it->iterate (_client, _method);  }
    u_int64_t getNbHits  () { return _nbHits; }
private:
    IHitIterator* _it;
    void*                        _client;
    Iterator<Hit*>::Method _method;
    size_t _nbHits;
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
    IConfiguration*         config,
    IDatabaseQuickReader*   reader,
    IParameters*            params,
    AlignmentResultVisitor* resultVisitor
)
    : _config(0), _reader(0), _params(0), _resultVisitor(0),
      _seedsModel(0), _scoreMatrix(0), _globalStats(0), _queryInfo(0),
      _indexator(0), _hitIterator(0),
      _ungapAlignmentResult(0), _gapAlignmentResult(0)
{
    /** We use the provided arguments. */
    setConfig        (config);
    setReader        (reader);
    setParams        (params);
    setResultVisitor (resultVisitor);

    /** We create the seeds model. */
    setSeedsModel (getConfig()->createSeedModel (getParams()->seedModelKind, getParams()->subseedStrings));

    /** We set the score matrix. */
    setScoreMatrix (getConfig()->createScoreMatrix (getParams()->matrixKind, SUBSEED));

    /** We set the global statistic parameters. */
    setGlobalStatistics (getConfig()->createGlobalParameters (getParams()) );
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
    setConfig           (0);
    setReader           (0);
    setParams           (0);
    setResultVisitor    (0);
    setSeedsModel       (0);
    setScoreMatrix      (0);
    setGlobalStatistics (0);
    setQueryInfo        (0);
    setIndexator        (0);
    setHitIterator      (0);

    setUngapAlignmentResult (0);
    setGapAlignmentResult   (0);
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
    /** We want to have some time statistics. */
    TimeInfo* timeStats = createTimeInfo ();
    LOCAL (timeStats);

    /** We create a command dispatcher. */
    ICommandDispatcher* dispatcher = getConfig()->createDispatcher ();
    LOCAL (dispatcher);

    /** We create the subject database (more than one for tplastn) */
    ListIterator<ISequenceDatabase*> subjectDbIt = createDatabaseIterator (
        getConfig(),
        getParams()->subjectUri,
        getParams()->subjectRange,
        false,
        getSubjectFrames()
    );

    /** We create the query database (more than one for plastx) */
    ListIterator<ISequenceDatabase*> queryDbIt = createDatabaseIterator (
        getConfig(),
        getParams()->queryUri,
        getParams()->queryRange,
        getParams()->filterQuery,
        getQueryFrames()
    );

    /** We create an object for indexing subject and query databases. */
    setIndexator (getConfig()->createIndexator (getSeedsModel(), getParams()));

    /********************************************************************************/
    /**********                     FIRST LOOP ON QUERY PARTS              **********/
    /********************************************************************************/
    for (queryDbIt.first(); !queryDbIt.isDone(); queryDbIt.next())
    {
        /** Shortcuts. */
        ISequenceDatabase* queryDb = queryDbIt.currentItem();

        /** We update the subject/query databases for the indexation. Note that one or two of the databases
         *  may be the same as the previous iteration.
         */
        getIndexator()->setQueryDatabase (queryDb);

        /** We can compute query statistics information. */
        setQueryInfo (getConfig()->createQueryInformation (
            getGlobalStatistics (),
            getParams(),
            queryDb,
            _reader->getDataSize(),
            _reader->getNbSequences()
        ));

        /********************************************************************************/
        /**********                  SECOND LOOP ON SUBJECT PARTS              **********/
        /********************************************************************************/
        for (subjectDbIt.first(); !subjectDbIt.isDone(); subjectDbIt.next())
        {
            timeStats->addEntry ("algorithm");

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

            /** We want to have indexation execution time. */
            timeStats->addEntry ("indexation");

            /** We build the indexes (if needed). */
            getIndexator()->build (dispatcher);

            timeStats->stopEntry ("indexation");

            /** We create an ungap alignment result. */
            IAlignmentResult* ungapAlignmentResult = getConfig()->createUnapAlignmentResult (queryDb->getSize());
            LOCAL (ungapAlignmentResult);
            setUngapAlignmentResult (ungapAlignmentResult);

            /** We create an alignment result. */
            IAlignmentResult* alignmentResult  = getConfig()->createGapAlignmentResult (subjectDb, queryDb);
            LOCAL (alignmentResult);
            setGapAlignmentResult (alignmentResult);

            /** We create the Hit iterator to be used by the algorithm. */
            setHitIterator (createHitIterator (getConfig(), getIndexator()->createHitIterator(), ungapAlignmentResult, alignmentResult) );

            /** We split the iterator in several iterators. */
            std::vector<IHitIterator*> its = getHitIterator()->split (dispatcher->getExecutionUnitsNumber());

            /** We create a list of commands for iterating the hits. */
            list<ICommand*> commands;
            for (size_t i=0; i<its.size(); i++)
            {
                /** We create a new command. */
                commands.push_back (new HitIterationCommand (
                    its[i],
                    this,
                    (Iterator<Hit*>::Method) & AbstractAlgorithm::hitUpdate
                ));
            }

            /** We want to have iteration execution time. */
            timeStats->addEntry ("iteration");

            /** We run the commands through a dispatcher. */
            dispatcher->dispatchCommands (commands, 0);

            /** We may have to post process the alignments result. */
            alignmentResult->shrink();

            timeStats->stopEntry ("iteration");

            /** We create a visitor for dumping the alignments. */
            timeStats->addEntry ("output");
            alignmentResult->accept (getResultVisitor());
            timeStats->stopEntry ("output");

            timeStats->stopEntry ("algorithm");

            /** We notify some report to potential listeners. */
            this->notify (new AlgorithmReportEvent (
                this,
                subjectDb,
                queryDb,
                timeStats,
                ungapAlignmentResult,
                alignmentResult
            ) );

        }  /* end of for (subjectDbIt.first(); ... */

    }  /* end of for (queryDbIt.first(); ... */

    /** Now, gapped alignments should have been computed. */
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ListIterator<ISequenceDatabase*> AbstractAlgorithm::createDatabaseIterator (
    IConfiguration*     config,
    const std::string&  uri,
    const Range&        range,
    bool                filtering,
    const std::vector<database::ReadingFrame_e>& frames
)
{
    list<ISequenceDatabase*> dbList;

    bool isORF = frames.empty() == false;

    /** We create the source database. */
    ISequenceDatabase* db = config->createDatabase (uri, range, isORF ? false : filtering);

    if (frames.empty() == false)
    {
        for (size_t i=0; i<frames.size(); i++)
        {
            dbList.push_back (new ReadingFrameSequenceDatabase (frames[i], db, filtering));
        }
    }
    else
    {
        dbList.push_back (db);
    }

    DEBUG (("AbstractAlgorithm::createDatabaseIterator: uri='%s'  filter=%d  => db=%p  nbSeq=%ld  listSize=%ld \n",
        uri.c_str(), filtering, db, db->getSequencesNumber(), dbList.size()
    ));

    /** We return the result. */
    return ListIterator<ISequenceDatabase*> (dbList);
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
    IConfiguration*         config,
    IHitIterator*           hitSource,
    algo::IAlignmentResult* ungapAlignResult,
    algo::IAlignmentResult* alignResult
)
{
    DEBUG (("AbstractAlgorithm::createHitIterator: config=%p  source=%p\n", config, hitSource));

    /** We create Hit iterators for each algorithm step and link them. */
    IHitIterator* ungapHitIterator = getConfig()->createUngapHitIterator (
        hitSource, getSeedsModel(), getScoreMatrix(), getParams(), ungapAlignResult
    );
    DEBUG (("AbstractAlgorithm::createHitIterator => ungapHitIterator=%p\n", ungapHitIterator));

    IHitIterator* smallGapIterator = getConfig()->createSmallGapHitIterator (
        ungapHitIterator, getSeedsModel(), getScoreMatrix(), getParams(), ungapAlignResult
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

    /** We subscribe to the seeds hit iteration events. */
    hitSource->addObserver (this);

    /** We should link some builder to the final iterator for gathering useful information about gap alignments. */


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
    /** We just forward event. */
    if (this != subject)
    {
        /** We decorate an event with other information. */
        IterationStatusEvent* e1 = dynamic_cast<IterationStatusEvent*> (evt);
        if (e1 != 0)
        {
            if (e1->getStatus() == ITER_ON_GOING)
            {
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
AbstractAlgorithm::AlgoTimeInfo::AlgoTimeInfo (AbstractAlgorithm* algo)
    : TimeInfo (os::DefaultFactory::singleton().getTimeFactory()), _algo(algo)
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
} /* end of namespaces. */
/********************************************************************************/

