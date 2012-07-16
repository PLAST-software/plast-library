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

/** \file BufferedSequenceDatabase.hpp
 *  \brief Sequence database in memory
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _BUFFERED_SEQUENCE_DATABASE_HPP_
#define _BUFFERED_SEQUENCE_DATABASE_HPP_

/********************************************************************************/

#include <database/api/ISequenceDatabase.hpp>
#include <database/api/ISequenceCache.hpp>
#include <database/api/ISequenceBuilder.hpp>
#include <database/impl/AbstractSequenceIterator.hpp>

#include <vector>
#include <string>
#include <stdio.h>

/********************************************************************************/
namespace database {
/** \brief Implementation of concepts related to genomic databases. */
namespace impl {
/********************************************************************************/

/** \brief ISequenceDatabase implementation with all data kept in RAM.
 *
 *  This class implements the ISequenceDatabase interface by keeping the whole database
 *  in memory.
 *
 *  The whole database information is kept in a cache (instance of ISequenceCache) which is
 *  built once when some client call some method of the class.
 *
 *  The cache is built from some sequence iterator by providing to this iterator a ISequenceBuilder instance.
 *  When the iteration is done, the builder has finished to build the full cache. Note that the iterator
 *  is release once the cache has been fully built.
 *
 *  From this cache, the BufferedSequenceDatabase class can extract information to be compliant
 *  to the ISequenceDatabase interface.
 *
 *  Code sample:
 *  \code
 *  void sample ()
 *  {
 *      // we create a sequence iterator that reads FASTA file
 *      ISequenceIterator* seqIt = new FastaSequenceIterator ("tursiops.fa", 100);
 *
 *      // we create a memory cached database from this sequence iterator
 *      ISequenceDatabase* database = new BufferedSequenceDatabase (seqIt, false);
 *
 *      // we try to retrieve the third sequence
 *      ISequence sequence;
 *      if (database->getSequenceByIndex (2, sequence) == true)
 *      {
 *          // we can do anything we want with the retrieved sequence.
 *      }
 *  }
 *  \endcode
 */
class BufferedSequenceDatabase : public ISequenceDatabase
{
public:

    /** Constructor that builds the cache from a sequence iterator.
     * \param[in] refIterator : iterator from which the memory cache is built
     * \param[in] filterLowComplexity : tells whether one should filter out low informative regions from sequences.
     */
    BufferedSequenceDatabase (ISequenceIterator* refIterator, bool filterLowComplexity);

    /** Constructor that uses a provided cache and an index range for iterating the cache.
     * \param[in] cache : cache to be used
     * \param[in] firstIdx : first index to be used in the cache
     * \param[in] lastIdx  : last index to be used in the cache
     */
    BufferedSequenceDatabase (
        const std::string& id,
        ISequenceCache* cache,
        size_t firstIdx,
        size_t lastIdx
    );

    /** Destructor. */
    virtual ~BufferedSequenceDatabase ();

    /** \copydoc ISequenceDatabase::getSequencesNumber
     * The cache is supposed to be already built. */
    size_t getSequencesNumber ()  { getCache();  return _nbSequences; }

    /** \copydoc ISequenceDatabase::getSize
     * The cache is supposed to be already built. */
    u_int64_t getSize ()  {  return (getCache()->offsets.data)[_lastIdx+1] - (getCache()->offsets.data)[_firstIdx];  }

    /** \copydoc ISequenceDatabase::getSequenceByIndex
     * The cache is supposed to be already built. */
    bool getSequenceByIndex (size_t index, ISequence& sequence);

    /** \copydoc ISequenceDatabase::getSequenceRefByIndex
     * The cache is supposed to be already built. */
    ISequence* getSequenceRefByIndex (size_t index)  { return 0; }

    /** \copydoc ISequenceDatabase::getSequenceByOffset
     * The cache is supposed to be already built. */
    bool getSequenceByOffset (
        u_int64_t  offset,
        ISequence& sequence,
        u_int32_t& offsetInSequence,
        u_int64_t& offsetInDatabase
    );

    /** \copydoc ISequenceDatabase::getSequenceByOffset
     * The cache is supposed to be already built. */
    bool getSequenceByName (
        const std::string& id,
        ISequence& sequence
    );

    /** \copydoc ISequenceDatabase::createSequenceIterator
     * The cache is supposed to be already built. */
    ISequenceIterator* createSequenceIterator () { return new BufferedSequenceIterator (this, getCache(), _firstIdx, _lastIdx); }

    /** \copydoc ISequenceDatabase::split
     * The cache is supposed to be already built. */
    std::vector<ISequenceDatabase*> split (size_t nbSplit);

    /** \copydoc ISequenceDatabase::getProperties */
    dp::IProperties* getProperties (const std::string& root);

    /** \copydoc ISequenceDatabase::getId */
    std::string getId ()  { return _id; }

    /** \copydoc ISequenceDatabase::retrieveSequencesIdentifiers */
    void retrieveSequencesIdentifiers (std::set<std::string>& ids);

private:

    /** Identifier */
    std::string _id;
    void setId (const std::string& id)  { _id=id; }

    /** Number of sequences of the database. */
    size_t _nbSequences;

    /** Check that the provided index is correct.
     * \param[in] idx : the index to be checked.
     * \return true if ok, false otherwise.
     */
    bool isIndexValid (size_t idx)  { getCache(); return idx < _nbSequences; }

    /** Reference on the sequence iterator to be used for building the cache. */
    ISequenceIterator* _refIterator;

    /** Smart setter for the refIterator attribute.
     * \param[in] refIterator : the value to be set.
     */
    void setRefSequenceIterator (ISequenceIterator* refIterator)  { SP_SETATTR (refIterator); }

    /** Cache instance. */
    ISequenceCache* _cache;

    /** Accessor to the cache instance.
     * \return the cache.
     */
    ISequenceCache* getCache();

    /** Smart setter for the cache attribute.
     * \param[in] cache : the cache to be set.
     */
    void setCache (ISequenceCache* cache)  { SP_SETATTR(cache); }

    /** Method for building the cache.
     * \param[in] refIterator : the sequence iterator to be used for building the cache.
     * \return the built cache.
     */
    ISequenceCache* buildCache (ISequenceIterator* refIterator);

    /** First index to be used in the cache. */
    size_t _firstIdx;

    /** Last index to be used in the cache. */
    size_t _lastIdx;

    /** Update the provided sequence with information of sequence given by an index.
     * \param[in] idx : index of the sequence to be used
     * \param[in] sequence : sequence to be filled.
     */
    void updateSequence (size_t idx, ISequence& sequence);

    /** Tells whether the sequence have to be filtered out (ie. removing low informative regions). */
    bool _filterLowComplexity;

    /********************************************************************************/

    /** \brief Sequence iterator that uses information of cache.
     */
    class BufferedSequenceIterator : public AbstractSequenceIterator
    {
    public:

        BufferedSequenceIterator (ISequenceDatabase* db, ISequenceCache* cache, size_t firstIdx, size_t lastIdx)
            : _db(db), _cache(0), _firstIdx(firstIdx), _lastIdx(lastIdx)
        {
            setCache (cache);
        }

        ~BufferedSequenceIterator ()  {  setCache (0); }

        void first()
        {
            _currentIdx = _firstIdx;
            if (_currentIdx<=_lastIdx)
            {
                updateItem ();
            }
        }

        dp::IteratorStatus  next()
        {
            _currentIdx ++;
            if (_currentIdx<=_lastIdx)  {  updateItem ();  return dp::ITER_ON_GOING;  }
            else                        {  return dp::ITER_UNKNOWN;  }
        }

        bool isDone()  { return _currentIdx > _lastIdx;            }
        const ISequence* currentItem ()  { return &_item;   }

        ISequenceIterator* clone ()  {  return new BufferedSequenceIterator (_db, _cache, _firstIdx, _lastIdx);  }

    private:

        /** */
        ISequenceDatabase* _db;

        ISequenceCache* _cache;
        void setCache (ISequenceCache* cache)  { SP_SETATTR(cache); }

        size_t _firstIdx;
        size_t _lastIdx;
        size_t _currentIdx;

        ISequence _item;
        void updateItem ();
    };
};

/********************************************************************************/
/** \brief SequenceBuilder that builds memory cache.
 *
 *  Sequence builder that fills a SequenceCache instance during iteration of some sequences iterator.
 */
class BufferedSequenceBuilder : public ISequenceBuilder
{
public:

    /** Constructor.
     * \param[in] cache : the cache to be built.
     */
    BufferedSequenceBuilder (ISequenceCache* cache);

    /** Destructor. */
    virtual ~BufferedSequenceBuilder () { setCache (0); }

    /** \copydoc ISequenceBuilder::getSequence
     * Return always 0.
     */
    ISequence* getSequence ()  { return 0; }

    /** \copydoc ISequenceBuilder::getEncoding */
    Encoding getEncoding ()  { return _destEncoding; }

    /** \copydoc ISequenceBuilder::setComment */
    void setComment (const char* buffer, size_t length);

    /** \copydoc ISequenceBuilder::resetData */
    void resetData  ();

    /** \copydoc ISequenceBuilder::addData */
    void addData (const LETTER* data, size_t size, Encoding encoding)
    {
        /** We configure (if needed) the conversion table. */
        if (encoding != _sourceEncoding)
        {
            _sourceEncoding = encoding;
            _convertTable = EncodingManager::singleton().getEncodingConversion (_sourceEncoding, getEncoding());
        }

        /** We may have to resize the vector. */
        if (_currentDataCapacity <= _cache->dataSize + size)
        {
            _currentDataCapacity += size + _currentDataCapacity/2;
            _cache->database.resize (_currentDataCapacity);
        }

        /** We memorize the character and increase the data size of the current sequence. */
        for (size_t i=0; i<size; i++)
        {
            _cache->database.data [_cache->dataSize ++] = (_convertTable ? _convertTable[(int)data[i]] : data[i]);
        }
    }

    /** \copydoc ISequenceBuilder::postTreamtment
     * Nothing done in this implementation. */
    void postTreamtment ()  { /* does nothing. */ }

protected:

    /** Reference on the cache to be built. */
    ISequenceCache* _cache;
    void setCache (ISequenceCache* cache)  { SP_SETATTR(cache); }

    Offset _currentDataCapacity;
    size_t _currentSequencesCapacity;

    /** Source encoding scheme. */
    Encoding      _sourceEncoding;

    /** Destination encoding scheme. */
    Encoding      _destEncoding;

    /** Conversion table. */
    const LETTER* _convertTable;
};

/********************************************************************************/
/** \brief Sequence building with sequence filtering out.
 *
 * Use segmentation for removing low informative regions of the database sequences.
 *  The segmentation is supposed to be done in ASCII, so a post treatment will be
 *  necessary for encoding the result in SUBSEED.
 */
class BufferedSegmentSequenceBuilder : public BufferedSequenceBuilder
{
public:

    /** Constructor.
     * \param cache : the cache to be built.
     */
    BufferedSegmentSequenceBuilder (ISequenceCache* cache);

    /** Post treatment; it consists here to convert from ASCII to SUBSEED encoding schemes. */
    void postTreamtment ();
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _BUFFERED_SEQUENCE_DATABASE_HPP_ */
