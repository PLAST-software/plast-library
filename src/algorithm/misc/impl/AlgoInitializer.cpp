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

#include "AlgoInitializer.hpp"
#include "DatabaseIndex.hpp"

#include "BasicSeedModel.hpp"
#include "SubSeedModel.hpp"

#include "FastaSequenceIterator.hpp"
#include "StringSequenceIterator.hpp"
#include "ReadingFrameSequenceIterator.hpp"

#include "BufferedSequenceDatabase.hpp"
#include "ReadingFrameSequenceDatabase.hpp"

#include "SeedHitIterator.hpp"
#include "SeedHitIteratorCached.hpp"

#include "Statistics.hpp"

#include "DefaultAlgoFactories.hpp"

#include "IAlgoEvents.hpp"

#include <list>

using namespace std;
using namespace dp;
using namespace database;
using namespace seed;
using namespace indexation;
using namespace statistics;

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
AlgoConfigurator::AlgoConfigurator (
    algo::IParameters*  parameters,
    dp::ICommandDispatcher* indexBuildDispatcher
)
    : _parameters(0),
      _subjectDatabase(0),
      _queryDatabase(0),
      _model (0),
      _scoreMatrix (0),
      _indexDb(0),
      _indexQuery(0),
      _indexBuildDispatcher(0),
      _queryInfo (0),
      _isConfigured (false)
{
    /** We use the provided parameters. */
    setParameters  (parameters);

    /** We use the provided dispatcher. */
    setCommandDispatcher (indexBuildDispatcher);

    /** We postpone the end of the configuration. */
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AlgoConfigurator::~AlgoConfigurator ()
{
    setSubjectDatabase    (0);
    setQueryDatabase (0);
    setModel (0);
    setCommandDispatcher (0);
    setQueryInfo (0);
    setScoreMatrix (0);

    if (_indexDb    != 0)  { _indexDb->forget (); }
    if (_indexQuery != 0)  { _indexQuery->forget (); }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlgoConfigurator::configure (void)
{
    if (_isConfigured == false)
    {
        /** We create the model. */
        setModel ( DefaultAlgoFactories::singleton().createSeedModel (
            _parameters->seedModelKind,
            _parameters->seedSpan,
            _parameters->subseedStrings[0].c_str(),
            _parameters->subseedStrings[1].c_str(),
            _parameters->subseedStrings[2].c_str(),
            _parameters->subseedStrings[3].c_str()
        ));

        /** We set the score matrix. */
        setScoreMatrix (DefaultAlgoFactories::singleton().createScoreMatrix (_parameters->matrixKind, SUBSEED));

        /** We send some informative notification for potential listeners. */
//        notify (new AlgoEventDatabaseRead (ENUM_STARTED));

        /** We create the subject database. */
        setSubjectDatabase ( new BufferedSequenceDatabase (
            new FastaSequenceIterator (_parameters->subjectUri.c_str(),  100),
            false // no complexity filter on subject database
        ));

        /** We create the query database. */
        setQueryDatabase ( new BufferedSequenceDatabase (
            new FastaSequenceIterator (_parameters->queryUri.c_str(),  100),
            _parameters->filterQuery
        ));

        /** We can compute query statistics information. */
        setQueryInfo (new QueryInformation (
            new GlobalParameters (_parameters),
            _parameters,
            _queryDatabase,
            _subjectDatabase->getSize(),
            _subjectDatabase->getSequencesNumber()
        ));

        /** We send some informative notification for potential listeners. */
//        notify (new AlgoEventDatabaseRead (ENUM_FINISHED));

        /** We finally build the indexes for both databases. */
        buildIndexes ();

        /** We remember that we are now configured. */
        _isConfigured = true;
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
void AlgoConfigurator::setParameters (algo::IParameters* parameters)
{
    if (_parameters != 0)  { _parameters->forget (); }
    _parameters = parameters;
    if (_parameters != 0)  { _parameters->use ();    }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlgoConfigurator::setSubjectDatabase (database::ISequenceDatabase* db)
{
    if (_subjectDatabase != 0)
    {
        _subjectDatabase->forget();

        /** We also get rid of previous index. */
        if (_indexDb != 0)
        {
            _indexDb->forget ();
            _indexDb = 0;
        }
    }

    _subjectDatabase = db;

    if (_subjectDatabase != 0)  { _subjectDatabase->use(); }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlgoConfigurator::setQueryDatabase (database::ISequenceDatabase* query)
{
    if (_queryDatabase != 0)
    {
        _queryDatabase->forget();

        /** We also get rid of previous index. */
        if (_indexQuery != 0)
        {
            _indexQuery->forget ();
            _indexQuery = 0;
        }
    }

    _queryDatabase = query;

    if (_queryDatabase != 0)  { _queryDatabase->use(); }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlgoConfigurator::setModel (ISeedModel* model)
{
    if (_model != 0)  { _model->forget(); }
    _model = model;
    if (_model != 0)  { _model->use(); }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlgoConfigurator::setScoreMatrix (IScoreMatrix* scoreMatrix)
{
    if (_scoreMatrix != 0)  { _scoreMatrix->forget (); }
    _scoreMatrix = scoreMatrix;
    if (_scoreMatrix != 0)  { _scoreMatrix->use ();    }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlgoConfigurator::setCommandDispatcher (ICommandDispatcher* dispatcher)
{
    if (_indexBuildDispatcher != 0)  { _indexBuildDispatcher->forget (); }
    _indexBuildDispatcher = dispatcher;
    if (_indexBuildDispatcher != 0)  { _indexBuildDispatcher->use();     }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlgoConfigurator::setQueryInfo (statistics::IQueryInformation* queryInfo)
{
    if (_queryInfo != 0)  { _queryInfo->forget (); }
    _queryInfo = queryInfo;
    if (_queryInfo != 0)  { _queryInfo->use();     }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AlgoConfigurator::buildIndexes (void)
{
    /** We send some informative notification for potential listeners. */
//    notify (new AlgoEventIndexation (ENUM_STARTED));

    /** We create subject database index. */
    if (_indexDb == 0)  { _indexDb = buildIndex (_subjectDatabase, _model); }

    /** We create query database index. */
    if (_indexQuery == 0)
    {
        _indexQuery = buildIndex (_queryDatabase, _model);

        DEBUG (("AlgoInitializer::buildIndexes:  scoremin=%d\n", _parameters->smallGapThreshold ));
    }

    /** We send some informative notification for potential listeners. */
//    notify (new AlgoEventIndexation (ENUM_FINISHED));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IDatabaseIndex* AlgoConfigurator::buildIndex (ISequenceDatabase* database, ISeedModel* model)
{
    /** We create the index and use it. */
    IDatabaseIndex* index = new DatabaseIndex (database, model);
    index->use ();

    /** We get the number of possible execution units from the command dispatcher. */
    size_t nbSplits = _indexBuildDispatcher->getExecutionUnitsNumber();

    /** Note that we distingish two cases for optimization concerns. */

    if (nbSplits > 1)
    {
        /** We try to split the base in several smaller ones. */
        vector<ISequenceDatabase*> splits = database->split (nbSplits);

        list<ICommand*> commands;
        for (size_t i=0; i<splits.size(); i++)
        {
            /** We create an index for the current frame. */
            IDatabaseIndex* chidlIndex = new DatabaseIndex (splits[i], model);

            /** We add the index to the global index. */
            index->addChildIndex (chidlIndex);

            /** We create a command for building the index from the sequence iterator. */
            commands.push_back (new IndexBuildCommand (chidlIndex) );
        }

        /** We dispatch the commands. */
        _indexBuildDispatcher->dispatchCommands (commands, new IndexMergeCommand (index));
    }
    else
    {
        /** We create the command list. In this case, only one item. */
        list<ICommand*> commands;
        commands.push_back (new IndexBuildCommand (index) );

        /** We dispatch the commands. No merge needed here because we have only one index. */
        _indexBuildDispatcher->dispatchCommands (commands, 0);
    }

    return index;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IHitIterator* AlgoConfigurator::createHitIterator ()
{
    /** We may have to configure the instance. */
    configure ();

    return new SeedHitIteratorCached (_indexDb, _indexQuery, _parameters->ungapNeighbourLength);
    //return new HitIterator (_indexDb, _indexQuery);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AlgoInitializerWithSortedSeeds::AlgoInitializerWithSortedSeeds (
    algo::IParameters*  parameters,
    dp::ICommandDispatcher* indexBuildDispatcher
)
 : AlgoConfigurator (parameters, indexBuildDispatcher)
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
IHitIterator* AlgoInitializerWithSortedSeeds::createHitIterator ()
{
    /** We may have to configure the instance. */
    configure ();

    return new SeedHitIteratorCachedWithSortedSeeds (_indexDb, _indexQuery, _parameters->ungapNeighbourLength);
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
