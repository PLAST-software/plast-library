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

#include <alignment/visitors/impl/ModelBuilderVisitor.hpp>
#include <designpattern/impl/Vectorterator.hpp>

#include <typeinfo>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace dp;
using namespace dp::impl;
using namespace alignment;
using namespace alignment::core;

/********************************************************************************/
namespace alignment {
namespace visitors  {
namespace impl      {
/********************************************************************************/

class RootLevel : public IRootLevel
{
public:
    RootLevel () : it (_items)
    {
        DEBUG (("RootLevel::RootLevel  this=%p\n", this));
    }

    ~RootLevel ()
    {
        for (vector<IQueryLevel*>::iterator it = _items.begin(); it != _items.end(); it++)  {  delete (*it);  }

        DEBUG (("RootLevel::~RootLevel  this=%p\n", this));
    }

    void            first()       { it.first();                 }
    IteratorStatus  next()        { return it.next();           }
    bool            isDone()      { return it.isDone();         }
    IQueryLevel*    currentItem() { return it.currentItem();    }
    u_int32_t       size ()       { return it.size();           }

private:
    vector<IQueryLevel*> _items;
    void add (IQueryLevel* item)  { _items.push_back(item); }

    VectorIterator<IQueryLevel*> it;

    friend class ModelBuilderVisitor;
};

/********************************************************************************/

class QueryLevel : public IQueryLevel
{
public:
    QueryLevel (const database::ISequence* sequence)  : it(_items), _sequence (sequence) {}

    ~QueryLevel ()
    {
        for (vector<ISubjectLevel*>::iterator it = _items.begin(); it != _items.end(); it++)  {  delete (*it);  }
    }

    void            first()       { it.first();                 }
    IteratorStatus  next()        { return it.next();           }
    bool            isDone()      { return it.isDone();         }
    ISubjectLevel*  currentItem() { return it.currentItem();    }
    u_int32_t       size ()       { return it.size();           }

    const database::ISequence* getSequence ()  { return _sequence; }

private:
    vector<ISubjectLevel*> _items;
    void add (ISubjectLevel* item)  { _items.push_back(item); }

    VectorIterator<ISubjectLevel*> it;

    const database::ISequence* _sequence;

    friend class ModelBuilderVisitor;
};

/********************************************************************************/

class SubjectLevel : public ISubjectLevel
{
public:
    SubjectLevel (const database::ISequence* sequence)  : it(_items), _sequence (sequence) {}

    ~SubjectLevel ()
    {
    }

    void            first()       { it.first();                 }
    IteratorStatus  next()        { return it.next();           }
    bool            isDone()      { return it.isDone();         }
    Alignment*      currentItem() { return it.currentItem();    }
    u_int32_t       size ()       { return it.size();           }

    const database::ISequence* getSequence ()  { return _sequence; }

private:
    vector<Alignment*> _items;
    void add (Alignment* item)  { _items.push_back(item); }

    VectorIterator<Alignment*> it;

    const database::ISequence* _sequence;

    friend class ModelBuilderVisitor;
};

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ModelBuilderVisitor::ModelBuilderVisitor () : _model (0)
{
    setModel ((IRootLevel*) new RootLevel());
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ModelBuilderVisitor::~ModelBuilderVisitor ()
{
    setModel (0);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void ModelBuilderVisitor::visitQuerySequence   (
    const database::ISequence*  seq,
    const misc::ProgressInfo&   progress
)
{
    DEBUG (("ModelBuilderVisitor::visitQuerySequence 1\n"));

    _currentQuery = new QueryLevel (seq);

    ((RootLevel*)_model)->add (_currentQuery);

    DEBUG (("ModelBuilderVisitor::visitQuerySequence 2\n"));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void ModelBuilderVisitor::visitSubjectSequence (
    const database::ISequence*  seq,
    const misc::ProgressInfo&   progress
)
{
    DEBUG (("ModelBuilderVisitor::visitSubjectSequence 1\n"));

    _currentSubject = (ISubjectLevel*) (new SubjectLevel (seq));

    ((QueryLevel*)_currentQuery)->add (_currentSubject);

    DEBUG (("ModelBuilderVisitor::visitSubjectSequence 2\n"));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void ModelBuilderVisitor::visitAlignment (
    core::Alignment*            align,
    const misc::ProgressInfo&   progress
)
{
    DEBUG (("ModelBuilderVisitor::visitAlignment 1\n"));

    ((SubjectLevel*)_currentSubject)->add (align);

    DEBUG (("ModelBuilderVisitor::visitAlignment 2\n"));
}

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/
