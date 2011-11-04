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

#ifndef _BUFFERED_SEQUENCE_DATABASE_HPP_
#define _BUFFERED_SEQUENCE_DATABASE_HPP_

/********************************************************************************/

#include "ISequenceDatabase.hpp"
#include "ISequenceCache.hpp"
#include "ISequenceBuilder.hpp"
#include "AbstractSequenceIterator.hpp"

#include <vector>
#include <string>
#include <stdio.h>

/********************************************************************************/
namespace database {
/********************************************************************************/

/** Sequence iterator that uses memory cached information for iterating sequences.
 */
class BufferedSequenceDatabase : public ISequenceDatabase
{
public:

    /** Constructor that builds the cache from a sequence iterator. */
    BufferedSequenceDatabase (ISequenceIterator* refIterator, bool filterLowComplexity);

    /** Destructor. */
    virtual ~BufferedSequenceDatabase ();

    /** Returns the number of sequences in the database. */
    size_t getSequencesNumber ()  { getCache();  return _nbSequences; }

    /** Retrieve the database size. Use offset table for computing. */
    u_int64_t getSize ()  {  return (getCache()->offsets.data)[_lastIdx+1] - (getCache()->offsets.data)[_firstIdx];  }

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
    ISequenceIterator* createSequenceIterator () { return new BufferedSequenceIterator (this, getCache(), _firstIdx, _lastIdx); }

    /** Split the database. */
    std::vector<ISequenceDatabase*> split (size_t nbSplit);

    /** Return properties about the instance. */
    dp::IProperties* getProperties (const std::string& root);

private:

    /** Constructor that uses a provided cache and an index range for iterating the cache. */
    BufferedSequenceDatabase (ISequenceCache* cache, size_t firstIdx, size_t lastIdx);

    /** Number of sequences of the database. */
    size_t _nbSequences;

    /** */
    bool isIndexValid (size_t idx)  { return idx < _nbSequences; }

    ISequenceIterator* _refIterator;
    void setRefSequenceIterator (ISequenceIterator* refIterator)  { SP_SETATTR (refIterator); }

    /** We need a sequences cache instance. */
    ISequenceCache* _cache;
    ISequenceCache* getCache();
    void setCache (ISequenceCache* cache)  { SP_SETATTR(cache); }

    /** Method for building the cache. */
    ISequenceCache* buildCache (ISequenceIterator* refIterator);

    size_t _firstIdx;
    size_t _lastIdx;

    /** Update the provided sequence with information of sequence given by an index. */
    void updateSequence (size_t idx, ISequence& sequence);

    /** */
    bool _filterLowComplexity;

    /********************************************************************************/
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
/** We need a sequence builder that fills a SequenceCache instance during referenced iteration. */
class BufferedSequenceBuilder : public ISequenceBuilder
{
public:
    BufferedSequenceBuilder (ISequenceCache* cache);
    virtual ~BufferedSequenceBuilder () {}

    /** Note: we don't need to get a sequence, we just need iterating for building some product. */
    ISequence* getSequence ()  { return 0; }

    Encoding getEncoding ()  { return _destEncoding; }

    void setComment (const char* buffer, size_t length);
    void resetData  ();
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

    /** */
    void postTreamtment ()  { /* does nothing. */ }

protected:
    ISequenceCache* _cache;

    Offset _currentDataCapacity;
    size_t _currentSequencesCapacity;

    Encoding      _sourceEncoding;
    Encoding      _destEncoding;
    const LETTER* _convertTable;
};

/********************************************************************************/
/** Use segmentation for removing low informative regions of the database sequences.
 *  The segmentation is supposed to be done in ASCII, so a post treatment will be
 *  necessary for encoding the result in SUBSEED.
 */
class BufferedSegmentSequenceBuilder : public BufferedSequenceBuilder
{
public:

    BufferedSegmentSequenceBuilder (ISequenceCache* cache);

    void postTreamtment ();
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _BUFFERED_SEQUENCE_DATABASE_HPP_ */
