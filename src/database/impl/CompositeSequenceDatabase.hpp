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

/** \file CompositeSequenceDatabase.hpp
 *  \brief Composite database
 *  \date 07/11/2011
 *  \author edrezen
 *
 *  Define a sequences database class that is composed of several children
 *  ISequenceDatabase instances.
 */

#ifndef _COMPOSITE_SEQUENCE_DATABASE_HPP_
#define _COMPOSITE_SEQUENCE_DATABASE_HPP_

/********************************************************************************/

#include <database/api/ISequenceDatabase.hpp>
#include <database/impl/AbstractSequenceIterator.hpp>

#include <list>
#include <vector>

/********************************************************************************/
namespace database {
/** \brief Implementation of concepts related to genomic databases. */
namespace impl {
/********************************************************************************/

/** \brief Composite database made of children ISequenceDatabase instances.
 *
 *  Sometimes, one may have to deal with several ISequenceDatabase instances.
 *  Instead of having several databases, one may want to have a 'parent' database
 *  that seems to be composed of our initial several databases. In that sense, we
 *  want to have a composite database.
 *
 *  With this feature, iterating the 'parent' database consists in iterating each
 *  child database.
 */
class CompositeSequenceDatabase : public ISequenceDatabase
{
public:

    /** Constructor.
     * \param[in] children : list of children ISequenceDatabase instances.
     */
    CompositeSequenceDatabase (std::list<ISequenceDatabase*> children);

    /** Destructor. */
    virtual ~CompositeSequenceDatabase ();

    /** \copydoc ISequenceDatabase::getSize
     * Computed as the sum of all children databases sizes.
     */
    u_int64_t getSize ()  { return _totalSize; }

    /** \copydoc ISequenceDatabase::getSequencesNumber
     * Computed as the sum of all children databases sequences number.
     */
    size_t getSequencesNumber ()  { return _sequencesTotalNb; }

    /** \copydoc ISequenceDatabase::getSequenceByIndex
     *  Note that the filled ISequence instance will refer
     *  the child database it belongs to and not the composite database.
     */
    bool getSequenceByIndex (size_t index, ISequence& sequence);

    /** \copydoc ISequenceDatabase::getSequenceRefByIndex
     * The cache is supposed to be already built. */
    ISequence* getSequenceRefByIndex (size_t index);

    /** \copydoc ISequenceDatabase::getSequenceByOffset
     *  Note that the filled ISequence instance will refer
     *  the child database it belongs to and not the composite database.
     *  Note also that the returned 'offsetInDatabase' parameter will refer the offset
     *  in the child database.
     */
    bool getSequenceByOffset (
        u_int64_t  offset,
        ISequence& sequence,
        u_int32_t& offsetInSequence,
        u_int64_t& offsetInDatabase
    );

    /** \copydoc ISequenceDatabase::getSequenceByName
     * The cache is supposed to be already built. */
    bool getSequenceByName (
        const std::string& id,
        ISequence& sequence
    );

    /** \copydoc ISequenceDatabase::createSequenceIterator */
    ISequenceIterator* createSequenceIterator ();

    /** \copydoc ISequenceDatabase::split */
    std::vector<ISequenceDatabase*> split (size_t nbSplit);

    /** \copydoc ISequenceDatabase::getProperties */
    dp::IProperties* getProperties (const std::string& root);

    /** \copydoc ISequenceDatabase::getId */
    std::string getId ();

    /** \copydoc ISequenceDatabase::retrieveSequencesIdentifiers */
    void retrieveSequencesIdentifiers (std::set<std::string>& ids);

private:

    /** Vector of children ISequenceDatabase instances. */
    std::vector<ISequenceDatabase*> _children;

    /** Total number of sequences. */
    size_t    _sequencesTotalNb;

    /** Total data sequences sizes. */
    u_int64_t _totalSize;

    std::vector <u_int32_t> _sequencesOffsets;
    std::vector <u_int64_t> _databasesOffsets;
};

/********************************************************************************/

/** \brief Sequence iterator for a list of ISequenceIterator instances.
 *
 *  This sequence iterator will iterate all provided sequences iterator.
 *  It is designed to be used by the CompositeSequenceDatabase class.
 */
class CompositeSequenceIterator : public AbstractSequenceIterator
{
public:

    /** Constructor.
     * \param itList : list of iterators to be iterated.
     */
    CompositeSequenceIterator (std::list<ISequenceIterator*>& itList);

    /** Destructor. */
    virtual ~CompositeSequenceIterator ();

    /** \copydoc AbstractSequenceIterator::first */
    void first()  {  _it = _itList.begin();   if (_it != _itList.end())  {  (*_it)->first(); } }

    /** \copydoc AbstractSequenceIterator::next */
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

    /** \copydoc AbstractSequenceIterator::isDone */
    bool isDone()
    {
        bool running = (_it != _itList.end()) && (*_it)->isDone()==false;
        return !running;
    }

    /** \copydoc AbstractSequenceIterator::currentItem */
    const ISequence* currentItem ()  { return (*_it)->currentItem();   }

    /** \copydoc AbstractSequenceIterator::iterate */
    void iterate (void* aClient, Method method);

    /** \copydoc AbstractSequenceIterator::clone */
    ISequenceIterator* clone ()  {  return new CompositeSequenceIterator (_itList);  }

private:

    /** List of the sequences iterator to be iterated. */
    std::list<ISequenceIterator*> _itList;

    /** Iterator on the list of iterators. */
    std::list<ISequenceIterator*>::iterator _it;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _COMPOSITE_SEQUENCE_DATABASE_HPP_ */
