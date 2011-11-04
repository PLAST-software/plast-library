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

#ifndef _COMPOSITE_SEQUENCE_DATABASE_HPP_
#define _COMPOSITE_SEQUENCE_DATABASE_HPP_

/********************************************************************************/

#include "ISequenceDatabase.hpp"
#include "AbstractSequenceIterator.hpp"

#include <list>
#include <vector>

/********************************************************************************/
namespace database {
/********************************************************************************/

class CompositeSequenceDatabase : public ISequenceDatabase
{
public:

    CompositeSequenceDatabase (std::list<ISequenceDatabase*> children);

    /** Destructor. */
    virtual ~CompositeSequenceDatabase ();

    /** Retrieve the database size. Use offset table for computing. */
    u_int64_t getSize ()  { return _totalSize; }

    /** Returns the number of sequences in the database. */
    size_t getSequencesNumber ()  { return _sequencesTotalNb; }

    /** Returns a sequence given its index. */
    bool getSequenceByIndex (size_t index, ISequence& sequence);

    /** Retrieve a sequence given its offset in the database. */
    bool getSequenceByOffset (
        u_int64_t  offset,
        ISequence& sequence,
        u_int32_t& offsetInSequence,
        u_int64_t& offsetInDatabase
    );

    /** Creates a Sequence iterator. */
    ISequenceIterator* createSequenceIterator ();

    /** Split the database. */
    std::vector<ISequenceDatabase*> split (size_t nbSplit);

    /** Return properties about the instance. */
    dp::IProperties* getProperties (const std::string& root);

private:

    std::vector<ISequenceDatabase*> _children;

    size_t    _sequencesTotalNb;
    u_int64_t _totalSize;

    std::vector <u_int32_t> _sequencesOffsets;
    std::vector <u_int64_t> _databasesOffsets;
};


/********************************************************************************/
class CompositeSequenceIterator : public AbstractSequenceIterator
{
public:

    CompositeSequenceIterator (std::list<ISequenceIterator*>& itList)
        : _itList (itList)
    {
    }

    virtual ~CompositeSequenceIterator ()  {  }

    void first()  {  _it = _itList.begin();   if (_it != _itList.end())  {  (*_it)->first(); } }

    dp::IteratorStatus  next()
    {
        (*_it)->next ();
        if ((*_it)->isDone())
        {
            _it++;
            if (_it != _itList.end())  {  (*_it)->first(); }
        }
        return dp::ITER_UNKNOWN;
    }

    bool isDone()
    {
        bool running = (_it != _itList.end()) && (*_it)->isDone()==false;
        return !running;
    }

    const ISequence* currentItem ()  { return (*_it)->currentItem();   }

    void iterate (void* aClient, Method method);

    ISequenceIterator* clone ()  {  return new CompositeSequenceIterator (_itList);  }

private:
    std::list<ISequenceIterator*> _itList;
    std::list<ISequenceIterator*>::iterator _it;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _COMPOSITE_SEQUENCE_DATABASE_HPP_ */
