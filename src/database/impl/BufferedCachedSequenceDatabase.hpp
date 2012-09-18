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

/** \file BufferedCachedSequenceDatabase.hpp
 *  \brief Sequence database in memory
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _BUFFERED_CACHED_SEQUENCE_DATABASE_HPP_
#define _BUFFERED_CACHED_SEQUENCE_DATABASE_HPP_

/********************************************************************************/

#include <database/impl/BufferedSequenceDatabase.hpp>
#include <vector>

/********************************************************************************/
namespace database {
/** \brief Implementation of concepts related to genomic databases. */
namespace impl {
/********************************************************************************/

/** \brief ISequenceDatabase implementation with all data kept in memory.
 *
 * This implementation inherits from BufferedSequenceDatabase, which maintains
 * all data in a cache, but doesn't keep a vector of ISequence in order to provide
 * the getSequenceRefByIndex service.
 *
 * Here, we implement the getSequenceRefByIndex by using such a vector that is built
 * during construction.
 *
 * Note: it should be the implementation of choice for the ISequenceDatabase interface
 * in terms of speed, in particular due to the direct access to a ISequence through the
 * getSequenceRefByIndex method. As a matter of fact, this method may be called quite
 * often by some indexation implementations.
 *
 * \see BufferedSequenceDatabase
 */
class BufferedCachedSequenceDatabase : public BufferedSequenceDatabase
{
public:

    /** Constructor.
     * \param[in] refIterator : sequence iterator used for building the database.
     * \param[in] filterLowComplexity : true means that we want to remove regions with low complexity.
     */
    BufferedCachedSequenceDatabase (ISequenceIterator* refIterator, bool filterLowComplexity);

    /** Destructor. */
    virtual ~BufferedCachedSequenceDatabase ();

    /** \copydoc BufferedSequenceDatabase::getSequenceRefByIndex */
    ISequence* getSequenceRefByIndex (size_t index)
    {
        return (index < _sequences.size() ? _sequences[index] : NULL);
    }

    /** \copydoc ISequenceDatabase::createSequenceIterator
     * The cache is supposed to be already built. */
    ISequenceIterator* createSequenceIterator () { return new BufferedCachedSequenceIterator (this, _firstIdx, _lastIdx); }

private:

    /** Boolean telling whether the ISequence vector has been built or not. */
    bool _isBuilt;

    /** Build the ISequence vector. */
    void buildSequencesCache (void);

    /** Vector holding all ISequence instances of the database. */
    std::vector<ISequence*> _sequences;

    /********************************************************************************/

    /** \brief Sequence iterator that uses information of cache.
     */
    class BufferedCachedSequenceIterator : public AbstractSequenceIterator
    {
    public:

        /** Constructor.
         * \param[in] db : the database to be iterated.
         * \param[in] firstIdx : index of the first sequence to be iterated
         * \param[in] lastIdx  : index of the last sequence to be iterated.
         */
        BufferedCachedSequenceIterator (BufferedCachedSequenceDatabase* db, size_t firstIdx, size_t lastIdx)
            : _db(db), _firstIdx(firstIdx), _lastIdx(lastIdx), _currentIdx(0)
        {
        }

        /** Destructor. */
        ~BufferedCachedSequenceIterator ()  { }

        /** \copydoc AbstractSequenceIterator::first */
        void first()
        {
            _currentIdx = _firstIdx;
        }

        /** \copydoc AbstractSequenceIterator::next */
        dp::IteratorStatus  next()
        {
            _currentIdx ++;
            return dp::ITER_UNKNOWN;
        }

        /** \copydoc AbstractSequenceIterator::isDone */
        bool isDone()  { return _currentIdx > _lastIdx;                           }

        /** \copydoc AbstractSequenceIterator::currentItem */
        const ISequence* currentItem ()  { return _db->_sequences[_currentIdx];   }

        /** \copydoc AbstractSequenceIterator::clone */
        ISequenceIterator* clone ()  {  return new BufferedCachedSequenceIterator (_db, _firstIdx, _lastIdx);  }

    private:

        /** Reference on the database to be iterated. */
        BufferedCachedSequenceDatabase* _db;

        size_t _firstIdx;
        size_t _lastIdx;
        size_t _currentIdx;
    };

    friend class BufferedCachedSequenceIterator;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _BUFFERED_CACHED_SEQUENCE_DATABASE_HPP_ */
