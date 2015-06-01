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

#include <algo/core/impl/AlgoIndexatorNucleotide.hpp>

#include <designpattern/impl/Property.hpp>
#include <designpattern/impl/CommandDispatcher.hpp>

#include <index/impl/DatabaseIndex.hpp>
#include <index/impl/FakeDatabaseNucleotideIndex.hpp>

#include <algo/hits/seed/SeedHitIteratorCached.hpp>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace dp;
using namespace dp::impl;
using namespace database;
using namespace indexation;
using namespace indexation::impl;
using namespace seed;
using namespace algo::hits;
using namespace algo::hits::seed;

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
IndexatorNucleotide::IndexatorNucleotide (
    ISeedModel* model,
    algo::core::IParameters* params,
    indexation::IDatabaseIndexFactory* factory,
    float seedsUseRatio,
    bool& isRunning
)
    : _model(0), _params(0), _factory(0),
      _subjectDatabase(0),  _queryDatabase(0),
      _subjectIndex(0),     _queryIndex(0),
      _seedsUseRatio (seedsUseRatio),
      _isRunning (isRunning), _sbjHasChanged(true), _qryHasChanged(true)
{
    /** We use some resources. */
    setModel   (model);
    setParams  (params);
    setFactory (factory);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IndexatorNucleotide::~IndexatorNucleotide ()
{
    /** We release some resources. */
    setModel           (0);
    setParams          (0);
    setFactory         (0);

    if (_subjectDatabase != 0)  { _subjectDatabase->forget (); }
    if (_queryDatabase   != 0)  { _queryDatabase->forget   (); }
    if (_subjectIndex    != 0)  { _subjectIndex->forget    (); }
    if (_queryIndex      != 0)  { _queryIndex->forget      (); }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void IndexatorNucleotide::setSubjectDatabase (ISequenceDatabase* subjectDatabase)
{
    _sbjHasChanged =  (_subjectDatabase==0) || (_subjectDatabase && subjectDatabase && _subjectDatabase->getId() != subjectDatabase->getId());

    SP_SETATTR (subjectDatabase);

    DEBUG (("IndexatorNucleotide::setSubjectDatabase:  _subjectIndex=%p  hasChanged=%d \n", _subjectIndex, _sbjHasChanged));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void IndexatorNucleotide::setQueryDatabase (ISequenceDatabase* queryDatabase)
{
    _qryHasChanged = (_queryDatabase==0) || (_queryDatabase && queryDatabase && _queryDatabase->getId() != queryDatabase->getId());

    SP_SETATTR (queryDatabase);

    DEBUG (("IndexatorNucleotide::setQueryDatabase:  _queryIndex=%p  hasChanged=%d \n", _queryIndex, _qryHasChanged));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void IndexatorNucleotide::build (dp::ICommandDispatcher* dispatcher)
{
    if (_queryDatabase && _subjectDatabase)
    {
        DEBUG (("IndexatorNucleotide::build:  (qry=%ld  sbj=%ld) \n",  _queryDatabase->getSize(),_subjectDatabase->getSize() ));

        if (_qryHasChanged)  {  setQueryIndex(0);  setSubjectIndex(0);    _qryHasChanged=false; }

        if (!_params->kmersBitsetPath.empty())
        {
		/** create a new fake indexator to create a mask */
            FakeDatabaseNucleotideIndex* indexforMask = new FakeDatabaseNucleotideIndex (_subjectDatabase, _model, _params->kmersBitsetPath);
		indexforMask->build();
            buildIndex (_queryIndex ,  _queryDatabase,   _model, dispatcher, indexforMask);
            buildIndex (_subjectIndex, _subjectDatabase, _model, dispatcher, indexforMask);
        }
        else
        {
		buildIndex (_queryIndex ,  _queryDatabase,   _model, dispatcher, 0);
		buildIndex (_subjectIndex, _subjectDatabase, _model, dispatcher, _queryIndex);
        }
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
void IndexatorNucleotide::buildIndex (IDatabaseIndex*& index, ISequenceDatabase* database, ISeedModel* model, ICommandDispatcher* dispatcher, IDatabaseIndex* otherIndex)
{
    DEBUG (("IndexatorNucleotide::buildIndex:  index=%p  database=%p  model=%p  \n", index, database, model));

    /** We create the index and use it. */
    if (index == 0)  { index = _factory->newDatabaseIndex (0, model, otherIndex, dispatcher);  index->use ();  }

    /** We build the index. */
    if (index != 0)
    {
        index->setDatabase (database);
        index->build ();
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
IHitIterator* IndexatorNucleotide::createHitIterator ()
{
    IHitIterator* result = 0;

    result =  new SeedHitIteratorCached (
        _subjectIndex,
        _queryIndex,
        _params->ungapNeighbourLength,
        _seedsUseRatio,
        _isRunning
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
IProperties* IndexatorNucleotide::getProperties ()
{
    IProperties* props = new Properties();

    props->add (0, "indexes");

    props->add (1, getSubjectIndex()->getProperties("subject"));
    props->add (1, getQueryIndex()->getProperties  ("query"));

    return props;
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
