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

#ifndef _BASIC_ALGO_INDEXATOR_HPP_
#define _BASIC_ALGO_INDEXATOR_HPP_

/********************************************************************************/

#include "IAlgoIndexator.hpp"
#include "IAlgoParameters.hpp"
#include "IDatabaseIndex.hpp"

/********************************************************************************/
namespace algo  {
/********************************************************************************/

class BasicIndexator : public IIndexator
{
public:

    BasicIndexator (seed::ISeedModel* model, algo::IParameters* params);
    virtual ~BasicIndexator ();

    /** */
    database::ISequenceDatabase* getSubjectDatabase ()  { return _subjectDatabase; }
    database::ISequenceDatabase* getQueryDatabase   ()  { return _queryDatabase;   }

    /** */
    void setSubjectDatabase (database::ISequenceDatabase* db);
    void setQueryDatabase   (database::ISequenceDatabase* db);

    /** */
    indexation::IDatabaseIndex* getSubjectIndex ()  { return _subjectIndex; }
    indexation::IDatabaseIndex* getQueryIndex   ()  { return _queryIndex;   }

    /** */
    void build (dp::ICommandDispatcher* dispatcher);

    /** */
    indexation::IHitIterator* createHitIterator ();

    /** Return properties about the instance. */
    dp::IProperties* getProperties ();

protected:

    seed::ISeedModel* _model;
    void setModel (seed::ISeedModel* model)  {  SP_SETATTR(model); }

    algo::IParameters* _params;
    void setParams (algo::IParameters* params)  {  SP_SETATTR(params); }

    database::ISequenceDatabase* _subjectDatabase;
    database::ISequenceDatabase* _queryDatabase;

    /** We need indexes for the two databases. */
    indexation::IDatabaseIndex* _subjectIndex;
    indexation::IDatabaseIndex* _queryIndex;

    indexation::IDatabaseIndex* buildIndex (
        database::ISequenceDatabase* database,
        seed::ISeedModel*            model,
        dp::ICommandDispatcher*      dispatcher
    );
};


/********************************************************************************/
class BasicSortedIndexator : public BasicIndexator
{
public:

    /** */
    BasicSortedIndexator (seed::ISeedModel* model, algo::IParameters* params);

    /** */
    indexation::IHitIterator* createHitIterator ();
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _BASIC_ALGO_INDEXATOR_HPP_ */
