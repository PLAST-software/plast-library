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

#include <database/impl/BufferedSequenceDatabase.hpp>
#include <designpattern/impl/Property.hpp>
#include <misc/api/macros.hpp>

#include <stdlib.h>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace dp;
using namespace dp::impl;
using namespace os;
using namespace os::impl;

extern "C" void seg_segSequence (char* sequence, size_t length);

u_int64_t nb_getSequenceByOffset = 0;

/********************************************************************************/
namespace database { namespace impl {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BufferedSequenceDatabase::BufferedSequenceDatabase (ISequenceIterator* refIterator, bool filterLowComplexity)
    : _nbSequences(0),
      _refIterator(0),
      _cache(0),
      _firstIdx(0), _lastIdx(0),
      _filterLowComplexity (filterLowComplexity)
{
    DEBUG (("BufferedSequenceDatabase::BufferedSequenceDatabase  this=%p  iter=%p\n", this, refIterator));

    /** We just keep a reference on the provided sequence iterator. The cache should be built on the first call
     * to some public API method. */
    setRefSequenceIterator (refIterator);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BufferedSequenceDatabase::BufferedSequenceDatabase (ISequenceCache* cache, size_t firstIdx, size_t lastIdx)
    : _nbSequences(0), _refIterator(0), _cache(0), _firstIdx(firstIdx), _lastIdx(lastIdx)
{
    DEBUG (("BufferedSequenceDatabase::BufferedSequenceDatabase  this=%p  [%ld,%ld] \n", this, _firstIdx, _lastIdx));

    /** We set the current cache. */
    setCache (cache);

    /** We use a shortcut for time optimization. */
    _nbSequences = _lastIdx - _firstIdx + 1;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BufferedSequenceDatabase::~BufferedSequenceDatabase ()
{
    DEBUG (("BufferedSequenceDatabase::~BufferedSequenceDatabase  this=%p\n", this));

    /** We release instances. */
    setCache (0);
    setRefSequenceIterator (0);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ISequenceCache* BufferedSequenceDatabase::getCache()
{
    if (_cache == 0)
    {
        /** We build the cache. */
        setCache (buildCache(_refIterator));

        /** We can now release the referenced iterator. */
        setRefSequenceIterator (0);
    }

    return _cache;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ISequenceCache* BufferedSequenceDatabase::buildCache (ISequenceIterator* refIterator)
{
    ISequenceCache* result = 0;

    DEBUG (("BufferedSequenceDatabase::buildCache BEGIN\n"));

    /** We create a new cache. */
    result = new ISequenceCache (10*1024);

    /** We change the sequence builder. We initialize it with the vectors of the cache to be filled during iteration. */
    ISequenceBuilder* builder = 0;
    if (_filterLowComplexity == false)   { builder = new BufferedSequenceBuilder        (result);  }
    else                                 { builder = new BufferedSegmentSequenceBuilder (result);  }
    refIterator->setBuilder (builder);

    /** We just loop through the ref iterator => the builder will fill the cache vectors. */
    for (refIterator->first(); !refIterator->isDone(); refIterator->next())  {}

    DEBUG (("BufferedSequenceDatabase::buildCache  iteration done: dataSize=%d  nbSequences=%ld \n",
        result->dataSize, result->nbSequences
    ));

    /** We may have no result; just return. */
    if (result->dataSize == 0)  { return result; }

    /** We can resize the containers with true sizes. */
    result->database.resize (result->dataSize + 1);
    result->comments.resize (result->nbSequences);
    result->offsets.resize  (result->nbSequences + 1);

    result->database.data [result->dataSize] = CODE_X;

    /** Note that we add an extra offset that matches the total size of the data.
     *  => useful for computing the last sequence size by difference of two offsets. */
    (result->offsets.data) [result->nbSequences] = result->dataSize;

    /** We may have to do some post treatment. */
    if (builder != 0)  { builder->postTreamtment(); }

    /** We compute the sequence average size. */
    size_t sizeAverage = (result->nbSequences > 0 ? result->database.size / result->nbSequences : 1);

    /** We increase this average by a factor (avoids too many interpolation failures in the getSequenceByOffset method). */
    sizeAverage = (sizeAverage*115) / 100;

    /** We compute coeffs [a,b] giving an estimate of this average as the number 2^b/a.
     *  The computation tries to achieve some relative error. */

    size_t a=0, b=0;

    /** We loop over powers of 2. */
    for (b=1; b<32; b++)
    {
        /** We look for a b such as 2^b >= N. */
        if ( (1<<b) >= sizeAverage)
        {
            /** 'a' should be > 0. */
            a = (1<<b) / sizeAverage;

            /** We look for a small relative error. */
            float err = 1.0 - (float)(1<<b) / (float)a / (float)sizeAverage  ;

            if (ABS(err) < 0.03)  {  break;  }
        }
    }

    result->sequenceAverageA = a;
    result->sequenceAverageB = b;

    /** We memorize the number of sequences found during iteration. */
    _nbSequences = result->nbSequences;

    /** We can set the [first,last] indexes for iterators. */
    if (_nbSequences > 0)
    {
        _firstIdx = 0;
        _lastIdx  = _nbSequences - 1;
    }
    else
    {
        _firstIdx = 1;
        _lastIdx  = 0;
    }

    DEBUG (("BufferedSequenceDatabase::buildCache  dataSize=%lld  nbSeq=%ld  average=[%d,%d]  first=%ld  last=%ld \n",
        result->dataSize, result->nbSequences,
        result->sequenceAverageA, result->sequenceAverageB,
        _firstIdx, _lastIdx
    ));

    /** We return the result. */
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
void BufferedSequenceDatabase::updateSequence (size_t idx, ISequence& sequence)
{
    DEBUG (("BufferedSequenceDatabase::updateSequence  idx=%ld\n", idx));

    /** Shortcut. */
    ISequenceCache* cache = getCache ();

    /** We set the index of the sequence in the database. */
    sequence.index = idx;

    /** A little shortcut (avoid two memory accesses). */
    Offset currentOffset = cache->offsets.data [idx];

    /** We set 'database' attribute. */
    sequence.database = this;

    /** We set the offset in database attribute. */
    sequence.offsetInDb = currentOffset;

    /** We get a pointer to the comment of the current sequence. */
    sequence.comment  = cache->comments[idx].c_str();

    /** We get a pointer to the data of the current sequence. */
    sequence.data.setReference (
            cache->offsets.data[idx+1] - currentOffset,
        (LETTER*) (cache->database.data + currentOffset)
    );
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool BufferedSequenceDatabase::getSequenceByIndex (size_t index, ISequence& sequence)
{
    DEBUG (("BufferedSequenceDatabase::getSequenceByIndex  index=%ld\n", index));

    bool result = false;

    if (isIndexValid(index) == true)
    {
        updateSequence (_firstIdx + index, sequence);
        result = true;
    }

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
bool BufferedSequenceDatabase::getSequenceByOffset (
    u_int64_t  offset,
    ISequence& sequence,
    u_int32_t& offsetInSequence,
    u_int64_t& actualOffsetInDatabase
)
{
    nb_getSequenceByOffset++;

    /** Shortcut. */
    ISequenceCache* cache = getCache ();

    /** We reset the argument to be filled. */
    offsetInSequence = 0;

    /** Shortcut. */
    Offset* offsets = cache->offsets.data;
    size_t  nb      = cache->offsets.size;

    u_int8_t A = cache->sequenceAverageA;
    u_int8_t B = cache->sequenceAverageB;

    /** We first look for the sequence index from the provided offset (use dichotomy search) */
    size_t smin = 0;
    size_t smax = nb - 1;

    /** We initialize the first index for beginning the dichotomy process.
     *  A classical choice may be idx = (smax+smin)/2  but we can try better initial guess. */
    size_t idx = (offset *A) >> B;
    if (idx >= smax)  { idx = (smax+smin) >> 1;  }

    /** We use a temporary variable in order not to do a comparison twice. */
    bool b = false;

    /** We compute the delta between the provided offset and the highest position in the current range.
     *  If this delta is >= 0, it means that we have still not found the idx.
     *  In this case, we now that the min index will be updated and that the delta can be used for computing
     *  the idx interpolation.
     */
    int32_t delta = 0;

    while ( (b = offsets[idx] > offset)  ||  ((delta = offset-offsets[idx+1]) >= 0) )
    {
        if (b)  {  smax = idx - 1;  }
        else    {  smin = idx + 1;  }

        /** We interpolate the index. Ideally, we should compute: idx = smin + delta / averageSize
         *  Because of the division time cost, we try instead to perform a bits shift; this will be
         *  less accurate in terms of interpolation but much quicker in terms of speed.
         */
        idx = smin + ((delta * A) >> B);

        /** If interpolation fails, we use classical dichotomy approach. */
        if (idx >= smax)  {  idx = (smin+smax) >> 1;  }
    }

    if (false)
    {
        if (offset < offsets[idx]  ||  offset >= offsets[idx+1])
        {
            printf ("BufferedSequenceDatabase::getSequenceByOffset: ERROR IN SEQUENCE SEARCH...\n");
        }
    }

    /** Optimization that avoids several memory acces. */
    size_t off0 = offsets[idx];

    /** Now, 'idx' should contain the index of the wanted sequence. */
    offsetInSequence = offset - off0;

    /** We set the actual offset in db. */
    actualOffsetInDatabase = offset;

    /** We set the database field. */
    sequence.database = this;

    /** We set the offset in database attribute. */
    sequence.offsetInDb = off0;

    /** We get a pointer to the comment of the current sequence. */
    sequence.comment  = cache->comments[idx].c_str();

    /** We get a pointer to the data of the current sequence. */
    sequence.data.setReference (
        offsets[idx+1] - off0,
        (LETTER*) (cache->database.data + off0)
    );

    sequence.index = idx;

    /** Always true because the dichotomy is not supposed to fail. */
    return true;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
bool BufferedSequenceDatabase::getSequenceByName (
    const std::string& id,
    ISequence& sequence
)
{
    bool result = false;

    /** Shortcut. */
    vector<string>& comments = _cache->comments;

    size_t i=0;
    for (i=0; i<comments.size(); i++)
    {
        result = comments[i].find (id) != string::npos;
        if (result)  { break; }
    }

    if (result)  {  result = getSequenceByIndex (i, sequence);  }

    /** We return the result. */
    return result;
}

/*********************************************************************
** METHOD  : CacheIterator::changeSequence
** PURPOSE : fill the attributes of the sequence returned by 'currentItem'
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS : uses cached information for setting the sequence attributes
*********************************************************************/
void BufferedSequenceDatabase::BufferedSequenceIterator::updateItem()
{
    /** A little shortcut (avoid two memory accesses). */
    Offset currentOffset = _cache->offsets.data[_currentIdx];

    /** We set the database field. */
    _item.database = _db;

    /** We get a pointer to the comment of the current sequence. */
    _item.comment  = _cache->comments[_currentIdx].c_str();

    /** We set the index. */
    _item.index = _currentIdx;

    /** We get a pointer to the data of the current sequence. */
    _item.data.setReference (
        _cache->offsets.data[_currentIdx+1] - currentOffset,
        (LETTER*) (_cache->database.data + currentOffset)
    );
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
vector<ISequenceDatabase*> BufferedSequenceDatabase::split (size_t nbSplit)
{
    vector<ISequenceDatabase*> result;

    if (nbSplit > 0)
    {
        /** Shortcut. */
        ISequenceCache* cache = getCache ();

        /** We compute the average sequences number for each iterator. */
        size_t averageNbSequences =  cache->nbSequences / nbSplit;

        /** We may have to add one if the remainder is not null. */
        if (cache->nbSequences % nbSplit != 0)  {  averageNbSequences++; }

        DEBUG (("BufferedIterator::split  nbSplit=%ld  => average=%ld  (nbSeq=%ld) \n",
            nbSplit, averageNbSequences, cache->nbSequences
        ));

        size_t first = 0;
        size_t last  = 0;

        for (size_t i=0;  first<cache->nbSequences  && i<nbSplit; i++)
        {
            last = min (first + averageNbSequences - 1, cache->nbSequences-1);

            /** We create a new iterator. */
            ISequenceDatabase* it = new BufferedSequenceDatabase (cache, first, last);

            /** We add it into the list of split iterators. */
            result.push_back (it);

            /** We increase the first index of the next iterator. */
            first += averageNbSequences;
        }
    }

    /** We return the result. */
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
IProperties* BufferedSequenceDatabase::getProperties (const std::string& root)
{
    IProperties* props = new Properties();

    props->add (0, root, "%lld bytes, %ld sequences", getSize(), getSequencesNumber());

    props->add (1, "size",           "%lld", getSize());
    props->add (1, "nb_sequences",   "%ld",  getSequencesNumber());

    /** We look for the shortest and longest sequences. */
    size_t minSeq = ~0;
    size_t maxSeq =  0;
    ISequenceIterator* it = createSequenceIterator();
    LOCAL(it);
    for (it->first(); !it->isDone(); it->next() )
    {
        const ISequence* seq = it->currentItem();
        if (seq != 0)
        {
            size_t size = seq->data.letters.size;

            if (size > maxSeq)  {  maxSeq = size; }
            if (size < minSeq)  {  minSeq = size; }
        }
    }

    props->add (1, "shortest",   "%ld",  minSeq);
    props->add (1, "largest",    "%ld",  maxSeq);

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
BufferedSequenceBuilder::BufferedSequenceBuilder (ISequenceCache* cache)
    : _cache(0),  _sourceEncoding(UNKNOWN), _destEncoding(SUBSEED), _convertTable(0)
{
    setCache (cache);
    _currentDataCapacity      = _cache->database.size;
    _currentSequencesCapacity = _cache->offsets.size;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BufferedSequenceBuilder::setComment (const char* buffer, size_t length)
{
    if (_currentSequencesCapacity <= _cache->nbSequences)
    {
        _currentSequencesCapacity += _currentSequencesCapacity/2;
        _cache->offsets.resize  (_currentSequencesCapacity);
        _cache->comments.resize (_currentSequencesCapacity);
    }

    //DEBUG (("BufferedSequenceDatabase::BufferedSequenceBuilder::setComment:  len=%ld\n", length));

    _cache->comments [_cache->nbSequences].assign (buffer, length);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BufferedSequenceBuilder::resetData (void)
{
    _cache->offsets.data [_cache->nbSequences++] = _cache->dataSize;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BufferedSegmentSequenceBuilder::BufferedSegmentSequenceBuilder (ISequenceCache* cache)
    : BufferedSequenceBuilder (cache)
{
    _destEncoding = ASCII;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BufferedSegmentSequenceBuilder::postTreamtment (void)
{
    /** Shortcut. */
    LETTER* data = _cache->database.data;

    //printf ("BufferedSegmentSequenceBuilder::postTreamtment 1. : size=%ld\n", _cache->dataSize);
    //for (size_t i=0; i<_cache->dataSize; i++)  {  printf ("%c", data[i]); }  printf("\n");

#if 0
    /** We launch low complexity removal. */
    seg_segSequence (data, _cache->dataSize);
#else
    /** We launch low complexity removal for each sequence.
     *  Note that this post treatment could be parallelized in several threads. */
    for (size_t i=0; i<_cache->nbSequences; i++)
    {
        seg_segSequence (
            data + _cache->offsets.data[i],
            _cache->offsets.data[i+1] - _cache->offsets.data[i]
        );
    }
#endif

    //printf ("BufferedSegmentSequenceBuilder::postTreamtment 2. : size=%ld\n", _cache->dataSize);
    //for (size_t i=0; i<_cache->dataSize; i++)  {  printf ("%c", data[i]); }  printf("\n");

    const LETTER* convert = EncodingManager::singleton().getEncodingConversion (ASCII, SUBSEED);

    /** We convert the cache from ASCII to SUBSEED. */
    for (size_t i=0; i<_cache->dataSize; i++)  {  data[i] = convert [(int)data[i]];  }

    //printf ("BufferedSegmentSequenceBuilder::postTreamtment 3. : size=%ld\n", _cache->dataSize);
    //for (size_t i=0; i<_cache->dataSize; i++)  {  printf ("%d ", data[i]); }  printf("\n");
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

