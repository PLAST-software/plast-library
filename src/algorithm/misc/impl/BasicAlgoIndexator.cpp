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

#include "BasicAlgoIndexator.hpp"

#include "DatabaseIndex.hpp"
#include "SeedHitIteratorCached.hpp"
#include "Property.hpp"

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace dp;
using namespace database;
using namespace indexation;
using namespace seed;

/********************************************************************************/
namespace algo  {
/********************************************************************************/

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

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BasicIndexator::BasicIndexator (seed::ISeedModel* model, algo::IParameters* params)
    : _model(0), _params(0),
      _subjectDatabase(0),  _queryDatabase(0),
      _subjectIndex(0),     _queryIndex(0)
{
    /** We use some resources. */
    setModel  (model);
    setParams (params);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BasicIndexator::~BasicIndexator ()
{
    /** We release some resources. */
    setModel  (0);
    setParams (0);
    setSubjectDatabase (0);
    setQueryDatabase   (0);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BasicIndexator::setSubjectDatabase (ISequenceDatabase* db)
{
    /** We first check that this is not the current database. */
    if (_subjectDatabase != db)
    {
        if (_subjectDatabase != 0)
        {
            _subjectDatabase->forget();

            /** We also get rid of previous index. */
            if (_subjectIndex != 0)
            {
                _subjectIndex->forget ();
                _subjectIndex = 0;
            }
        }

        _subjectDatabase = db;

        if (_subjectDatabase != 0)  { _subjectDatabase->use(); }
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
void BasicIndexator::setQueryDatabase (ISequenceDatabase* db)
{
    /** We first check that this is not the current database. */
    if (_queryDatabase != db)
    {
        if (_queryDatabase != 0)
        {
            _queryDatabase->forget();

            /** We also get rid of previous index. */
            if (_queryIndex != 0)
            {
                _queryIndex->forget ();
                _queryIndex = 0;
            }
        }

        _queryDatabase = db;

        if (_queryDatabase != 0)  { _queryDatabase->use(); }
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
void BasicIndexator::build (dp::ICommandDispatcher* dispatcher)
{
    if (_subjectIndex == 0)  { _subjectIndex = buildIndex (_subjectDatabase, _model, dispatcher); }
    if (_queryIndex == 0)    {  _queryIndex  = buildIndex (_queryDatabase,   _model, dispatcher); }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IDatabaseIndex* BasicIndexator::buildIndex (ISequenceDatabase* database, ISeedModel* model, ICommandDispatcher* dispatcher)
{
    /** We create the index and use it. */
    IDatabaseIndex* index = new DatabaseIndex (database, model);
    index->use ();

    /** We get the number of possible execution units from the command dispatcher. */
    size_t nbSplits = dispatcher->getExecutionUnitsNumber();

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
        dispatcher->dispatchCommands (commands, new IndexMergeCommand (index));
    }
    else
    {
        /** We create the command list. In this case, only one item. */
        list<ICommand*> commands;
        commands.push_back (new IndexBuildCommand (index) );

        /** We dispatch the commands. No merge needed here because we have only one index. */
        dispatcher->dispatchCommands (commands, 0);
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
IHitIterator* BasicIndexator::createHitIterator ()
{
    IHitIterator* result = 0;

    result =  new SeedHitIteratorCached (
        _subjectIndex,
        _queryIndex,
        _params->ungapNeighbourLength
    );

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
dp::IProperties* BasicIndexator::getProperties ()
{
    dp::IProperties* props = new dp::Properties();

    props->add (0, "indexes");

    props->add (1, getSubjectIndex()->getProperties("subject"));
    props->add (1, getQueryIndex()->getProperties  ("query"));

    return props;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BasicSortedIndexator::BasicSortedIndexator (seed::ISeedModel* model, algo::IParameters* params)
    : BasicIndexator (model, params)
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
IHitIterator* BasicSortedIndexator::createHitIterator ()
{
    IHitIterator* result = 0;

    result =  new SeedHitIteratorCachedWithSortedSeeds (
        _subjectIndex,
        _queryIndex,
        _params->ungapNeighbourLength
    );

    return result;
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
