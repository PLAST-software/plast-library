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


#ifndef _ALGO_INITIALIZER_HPP_
#define _ALGO_INITIALIZER_HPP_

/********************************************************************************/

#include "IAlgoInitializer.hpp"
#include "IAlgoParameters.hpp"
#include "ICommand.hpp"
#include "ISequenceDatabase.hpp"
#include "ISeedModel.hpp"
#include "IDatabaseIndex.hpp"
#include "IStatistics.hpp"
#include "IScoreMatrix.hpp"

/********************************************************************************/
namespace algo  {
/********************************************************************************/

class AlgoConfigurator : public IAlgoConfigurator
{
public:

    AlgoConfigurator (
        algo::IParameters*  parameters,
        dp::ICommandDispatcher* indexBuildDispatcher
    );

    virtual ~AlgoConfigurator ();

    indexation::IHitIterator* createHitIterator ();

    database::ISequenceDatabase* getSubjectDatabase () { return _subjectDatabase;    }
    database::ISequenceDatabase* getQueryDatabase   () { return _queryDatabase; }

    IParameters*  getParameters  () { return _parameters;   }
    seed::ISeedModel* getSeedModel   () { return _model;        }
    IScoreMatrix*     getScoreMatrix () { return _scoreMatrix;  }

    indexation::IDatabaseIndex* getIndexDatabase () { buildIndexes();  return _indexDb;    }
    indexation::IDatabaseIndex* getIndexQuery    () { buildIndexes();  return _indexQuery; }

    statistics::IQueryInformation* getQueryInformation ()  { return _queryInfo; }

    void setSubjectDatabase (database::ISequenceDatabase* db);
    void setQueryDatabase   (database::ISequenceDatabase* query);

protected:

    algo::IParameters*  _parameters;
    void setParameters (algo::IParameters* parameters);

    database::ISequenceDatabase* _subjectDatabase;
    database::ISequenceDatabase* _queryDatabase;

    /** We need a model for the indexation. */
    seed::ISeedModel* _model;
    void setModel (seed::ISeedModel* model);

    /** */
    IScoreMatrix* _scoreMatrix;
    void setScoreMatrix (IScoreMatrix* scoreMatrix);

    /** We need indexes for the two databases. */
    indexation::IDatabaseIndex* _indexDb;
    indexation::IDatabaseIndex* _indexQuery;

    /** We use a command dispatcher for building the indexes. */
    dp::ICommandDispatcher* _indexBuildDispatcher;
    void setCommandDispatcher (dp::ICommandDispatcher* dispatcher);

    bool _areIndexesBuilt;
    void buildIndexes (void);
    indexation::IDatabaseIndex* buildIndex (database::ISequenceDatabase* database, seed::ISeedModel* model);

    statistics::IQueryInformation* _queryInfo;
    void setQueryInfo (statistics::IQueryInformation* queryInfo);

    bool _isConfigured;
    void configure ();

    /********************************************************************************/
    class IndexBuildCommand : public dp::ICommand
    {
    public:
        IndexBuildCommand (indexation::IDatabaseIndex* index) :  _index(index)  {  if (_index)  { _index->use(); } }
        virtual ~IndexBuildCommand ()  {  if (_index)  { _index->forget(); } }
        void execute ()  {  _index->build ();  }
    private:
        indexation::IDatabaseIndex*_index;
    };

    /********************************************************************************/
    class IndexMergeCommand : public dp::ICommand
    {
    public:
        IndexMergeCommand (indexation::IDatabaseIndex* index) : _index(index)  {  if (_index)  { _index->use(); } }
        virtual ~IndexMergeCommand ()  {  if (_index)  { _index->forget(); } }
        void execute ()  {  _index->merge ();  }
    private:
        indexation::IDatabaseIndex* _index;
    };
};

/********************************************************************************/

/** Implementation that provides hits iterator iterated with sorted seeds: seeds that will
 *  generate most hits will be iterated first.
 */
class AlgoInitializerWithSortedSeeds : public AlgoConfigurator
{
public:

    AlgoInitializerWithSortedSeeds (
        algo::IParameters*  parameters,
        dp::ICommandDispatcher* indexBuildDispatcher
    );

    indexation::IHitIterator* createHitIterator ();
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ALGO_INITIALIZER_HPP_ */
