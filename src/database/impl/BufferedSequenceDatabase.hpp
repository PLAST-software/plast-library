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
#include <misc/api/macros.hpp>

#include <vector>
#include <string>
#include <stdio.h>

/********************************************************************************/
namespace database {
/** \brief Implementation of concepts related to genomic databases. */
namespace impl {
/********************************************************************************/

/** \brief ISequenceDatabase implementation with memory cache.
 *
 *  This class implements the ISequenceDatabase interface by keeping the whole database
 *  in memory.
 *
 *  The whole database information is kept in a cache (instance of ISequenceCache) which is
 *  built once when some client call some method of the class.
 *
 *  The cache is built from some sequence iterator by providing to this iterator a ISequenceBuilder instance.
 *  When the iteration is done, the builder has finished to build the full cache. Note that the iterator
 *  is released once the cache has been fully built.
 *
 *  From this cache, the BufferedSequenceDatabase class can extract information that fulfills the
 *  ISequenceDatabase interface.
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
     * \param[in] filterLowComplexity : tells whether one should filter out low informative regions from sequences. (0 means no filtering)
     */
    BufferedSequenceDatabase (ISequenceIterator* refIterator, int filterLowComplexity);

    /** Constructor that uses a provided cache and an index range for iterating the cache.
     * \param[in] id : identifier of the database
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

    /** \copydoc ISequenceDatabase::getSequenceByName
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

    /** Return the direction (PLUS or MINUS) of the strand for the sequences (meaningful only for nucleotides databases). */
    StrandId_e getDirection () { return _direction; }

    /** Change the strand of the sequences (meaningful only for nucleotides databases). */
    void reverse ();

protected:

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

    /** Tells whether the sequence have to be filtered out (ie. removing low informative regions).
     *  O means no filtering.*/
    int _filterLowComplexity;

    /** */
    StrandId_e _direction;

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

    /** \copydoc ISequenceBuilder::setCommentUri  */
    void setCommentUri (const char* filename, u_int32_t offsetHeader,size_t commentMaxSize);

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

        if (encoding == ASCII)
        {
            /** We may have to resize the vector. */
            if (_currentDataCapacity <= _cache->dataSize + size)
            {
                _currentDataCapacity += size + _currentDataCapacity/2;
                _cache->database.resize (_currentDataCapacity);
            }

            /** We memorize the character and increase the data size of the current sequence. */
			for (size_t i=0; i<size; i++)
			{
				char c = data[i];

				if ((c<'A' || c>'Z') &&  (c<'a' || c>'z') )  { continue; };

				_cache->database.data [_cache->dataSize ++] = (_convertTable ? _convertTable[(int)c] : c);
			}
        }
        else if (encoding == NCBI_DNA_NO_AMB)
        {
        	int32_t 	s;
        	u_int32_t 	remainder;
        	u_int32_t 	byteCount = 0;
        	size_t 	  	res_cnt = 0;

     		res_cnt = (size+1)* 4;

            /** We may have to resize the vector. */
            if (_currentDataCapacity <= _cache->dataSize + res_cnt)
            {
                _currentDataCapacity += res_cnt + _currentDataCapacity/2;
                _cache->database.resize (_currentDataCapacity);
            }

    		/* loop on the number of data */
    		for (byteCount = 0; byteCount < size; ++byteCount)
    		{
    			s = (data[byteCount] >> 6) & 0x03;
    			_cache->database.data [_cache->dataSize ++] = (_convertTable ? _convertTable [(int)s] : s);
    			s = (data[byteCount] >> 4) & 0x03;
    			_cache->database.data [_cache->dataSize ++] = (_convertTable ? _convertTable [(int)s] : s);
    			s = (data[byteCount] >> 2) & 0x03;
    			_cache->database.data [_cache->dataSize ++] = (_convertTable ? _convertTable [(int)s] : s);
    			s = (data[byteCount]) & 0x03;
    			_cache->database.data [_cache->dataSize ++] = (_convertTable ? _convertTable [(int)s] : s);
    		}

    	   	/* the least two significant bits indicate how many residues
    			* are encoded in the last byte.
    			*/
    		remainder = data[byteCount] & 0x03;
    		for (u_int32_t y=0;y<remainder;y++)
    		{
    			s = (3 - (y % 4)) * 2;
    			s = (data[byteCount] >> s) & 0x03;
    			_cache->database.data [_cache->dataSize ++] = (_convertTable ? _convertTable [(int)s] : s);
    		}
        }
        else if (encoding == NCBI_DNA_WITH_AMB)
        {
        	int32_t s;
        	int32_t remainder;
        	int32_t amb_res = 0;
        	int32_t byteCount = -1;

        	u_int64_t x;
        	u_int64_t soff = 0, eoff = 0;

        	u_int32_t res_cnt = 0;
        	u_int32_t amb_cnt = 0;
        	u_int32_t large_amb = 0;
        	u_int32_t amb_index = 0;

        	unsigned char *amb_ptr = NULL;

        	unsigned char lastByte;
        	const LETTER* noAmbConvertTable;
        	const LETTER* ambConvertTable;
        	noAmbConvertTable = EncodingManager::singleton().getEncodingConversion(NCBI_DNA_NO_AMB,SUBSEED);
        	ambConvertTable = EncodingManager::singleton().getEncodingConversion(NCBI_DNA_WITH_AMB,SUBSEED);

        	/* find the size of the ambiguity table */
        	amb_cnt = CHAR_TO_INT32(data[size],data[size+1],data[size+2],data[size+3]);

        	/* if the most significant bit is set on the count, then each correction
        	 * will take two entries in the table.  the layout is described below. */
        	large_amb = amb_cnt >> 31;
        	amb_cnt = amb_cnt & 0x7fffffff;

        	amb_index = size+4;
        	amb_ptr = (unsigned char*)&data[amb_index];

        	/* read the last byte of the sequence, so we can calculate the
        	* number of residues in the last byte of the sequence (0-3).
        	*/
        	lastByte = data[(size - 1)];

        	/* the least two significant bits indicate how many residues
        	* are encoded in the last byte.
        	*/
        	remainder = lastByte & 0x03;
        	res_cnt = (size - 1) * 4 + remainder;

            /** We may have to resize the vector. */
            if (_currentDataCapacity <= _cache->dataSize + res_cnt)
            {
                _currentDataCapacity += res_cnt + _currentDataCapacity/2;
                _cache->database.resize (_currentDataCapacity);
            }

        	byteCount = -1;

        	/* loop on the number of data */
        	for (x = 0; x < res_cnt; ++x)
        	{
        		/* decode the ambiguity table */
        		if (x == 0 || x > eoff)
        		{
					/* get the residue symbol */
					amb_res = (int32_t) (*amb_ptr >> 4);

					/* the layout of the ambiguity table differs if it is using
					 * large offsets, i.e. offsets > 16 million.
					 *
					 * for small offsets the layout is:
					 *    4 bits - nucleotide
					 *    4 bits - repeat count
					 *   24 bits - offset
					 *
					 * for large offsets the layout is:
					 *    4 bits - nucleotide
					 *   12 bits - repeat count
					 *   48 bits - offset
					 */
					if (large_amb)
					{
						/* get the repeat count */
						eoff  = (((u_int64_t) (*amb_ptr & 0x0f)) << 8) + (((u_int64_t) *(amb_ptr+1)) << 0);

						/* get the offset */
						soff  = (((u_int64_t) *(amb_ptr+2)) << 40);
						soff += (((u_int64_t) *(amb_ptr+3)) << 32);
						soff += (((u_int64_t) *(amb_ptr+4)) << 24);
						soff += (((u_int64_t) *(amb_ptr+5)) << 16);
						soff += (((u_int64_t) *(amb_ptr+6)) << 8);
						soff += (((u_int64_t) *(amb_ptr+7)) << 0);

						amb_ptr += 8;
						amb_cnt -= 2;
					}
					else
					{
						/* get the repeat count */
						eoff  = (u_int64_t) (*amb_ptr & 0x0f);

						/* get the offset */
						soff  = (((u_int64_t) *(amb_ptr+1)) << 16);
						soff += (((u_int64_t) *(amb_ptr+2)) << 8);
						soff += (((u_int64_t) *(amb_ptr+3)) << 0);

						amb_ptr += 4;
						amb_cnt -= 1;
					}
					eoff += soff;
        		}

        		/* read the next byte if necessary */
        		if ((x % 4) == 0)
        		{
        			byteCount ++;
        		}

        		if (x >= soff && x <= eoff)
        		{
        			_cache->database.data [_cache->dataSize ++] = ambConvertTable[(u_int32_t)amb_res];
        		}
        		else
        		{
        			s = (3 - (x % 4)) * 2;
        			s = (data[byteCount] >> s) & 0x03;
        			_cache->database.data [_cache->dataSize ++] = noAmbConvertTable[s];
        		}
        	}
        }
        else
        {
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
    BufferedSegmentSequenceBuilder (ISequenceCache* cache, int segMinSize);

    /** Post treatment; it consists here to convert from ASCII to SUBSEED encoding schemes. */
    void postTreamtment ();

private:

    /** We may remove low informative region (seg or dust algorithm). */
    void (*_filterSequenceCallback) (char* seq, int len);

    /** Theshold size of a sequence for launching the 'seg' algorithm. */
    int _segMinSize;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _BUFFERED_SEQUENCE_DATABASE_HPP_ */
