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

#include <index/impl/DatabaseIndex.hpp>
#include <misc/api/macros.hpp>
#include <misc/api/PlastStrings.hpp>

using namespace std;
using namespace dp;
using namespace database;
using namespace seed;

#include <stdio.h>
#define DEBUG(a)  //printf a
#define VERBOSE(a)

/********************************************************************************/
namespace indexation { namespace impl {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
DatabaseIndex::DatabaseIndex (ISequenceDatabase* database, ISeedModel* model)
    : AbstractDatabaseIndex (database, model), _currentSequence(0), _span(0), _alphabetSize(0), _sequenceOffset(0)
{
    DEBUG (("DatabaseIndex::DatabaseIndex: _maxSeedsNumber=%ld\n", _maxSeedsNumber));

    _span           = getModel()->getSpan();
    _alphabetSize   = getModel()->getAlphabet()->size;

    _maxSeedsNumber = 1;
    for (size_t i=1; i<=_span; i++)  { _maxSeedsNumber *= _alphabetSize; }

    /** We set the size of the index. */
    _index.resize (_maxSeedsNumber);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
DatabaseIndex::~DatabaseIndex ()
{
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void DatabaseIndex::iterateSeed (const ISeed* seed)
{
    /** We compute the hashcode for the current kmer. */
    SeedHashCode hashCode = seed->code;

    /** It may happen that the SeedModel doesn't compute a hash code, so we do it now. */
    if (hashCode == BAD_SEED_HASH_CODE)
    {
        LETTER* word = seed->kmer.letters.data;

        /** Optimization: direct computation according to the span size, full loop otherwise. */
             if (_span == 4)  { hashCode = word[0] + _alphabetSize*(word[1] + _alphabetSize*(word[2] + _alphabetSize*word[3])); }
        else if (_span == 3)  { hashCode = word[0] + _alphabetSize*(word[1] + _alphabetSize*word[2]); }
        else if (_span == 2)  { hashCode = word[0] + _alphabetSize*word[1]; }
        else
        {
            hashCode = word[_span-1];
            for (int i=_span-2; i>=0; i--)  {  hashCode = _alphabetSize * hashCode + word[i];  }
        }
    }

    if (hashCode != BAD_SEED_HASH_CODE)
    {
        /** We retrieve the index corresponding to the seed. */
        IndexEntry& entry = _index[hashCode];

        VERBOSE (("DatabaseIndex::iterateSeed: this=%p  seed='%s' (code=%ld) => offset=%ld\n",  this,
            seed->kmer.toString().c_str(),
            seed->code,
            _sequenceOffset + seed->offset
        ));

        /** We add the offset in the database for the current seed. */
        entry.push_back (SeedOccurrence (_sequenceOffset + seed->offset, _currentSequence->index));
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
ISeedIterator* DatabaseIndex::createSeedsIterator (const database::IWord& data)
{
    return getModel()->createSeedsIterator (data);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void DatabaseIndex::build ()
{
    DEBUG (("DatabaseIndex::build : START ! \n"));

    /** The intent of this method is to fill the _index attribute; this attribute is designed to hold (for each possible seed
     *  of the seeds model) the vector of offset occurrences. These offsets are relative to the provided sequences iterator.
     */

    /** We create a sequence iterator that iterates the database. */
    ISequenceIterator* seqIter = getDatabase()->createSequenceIterator();
    LOCAL(seqIter);

    size_t nbSequences   = 0;

    _sequenceOffset = 0;

    DEBUG (("DatabaseIndex::build : BEGIN SEQUENCES LOOP\n"));

    /** We loop over all the sequences. */
    for (seqIter->first(); !seqIter->isDone(); seqIter->next())
    {
        /** A little shortcut for the currently iterated sequence. */
        _currentSequence  = seqIter->currentItem();

        VERBOSE (("DatabaseIndex::build : current sequence '%s'\n", _currentSequence->data.toString().c_str()));

        /** We create a seed iterator. */
        ISeedIterator* itSeed = createSeedsIterator (_currentSequence->data);
        LOCAL (itSeed);

        /** We iterate the seed iterator. */
        itSeed->iterate (this, (Iterator<const ISeed*>::Method) &DatabaseIndex::iterateSeed);

        /** We update the current sequence offset (ie offset in the whole database). */
        _sequenceOffset += _currentSequence->data.letters.size;

        /** We increase the number of found sequences. */
        nbSequences++;
    }

    DEBUG (("DatabaseIndex::build : END SEQUENCES LOOP\n"));

    DEBUG (("DatabaseIndex::build : %ld sequences\n", nbSequences));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
DatabaseIndex::SeedHashCode DatabaseIndex::getHashCode (const IWord& word)
{
    SeedHashCode result = 0;

    /** Shortcut. */
    LETTER* buffer = word.letters.data;

    /** Optimization: direct computation according to the span size, full loop otherwise. */
         if (_span == 4)  { return  buffer[0] + _alphabetSize*(buffer[1] + _alphabetSize*(buffer[2] + _alphabetSize*buffer[3])); }
    else if (_span == 3)  { return  buffer[0] + _alphabetSize*(buffer[1] + _alphabetSize*buffer[2]); }
    else if (_span == 2)  { return  buffer[0] + _alphabetSize*buffer[1]; }
    else
    {
        SeedHashCode result = buffer[_span-1];
        for (int i=_span-2; i>=0; i--)
        {
            result = _alphabetSize * result + buffer[i];
        }
        return result;
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
IOccurrenceIterator* DatabaseIndex::createOccurrenceIterator (const ISeed* seed, size_t neighbourhoodSize)
{
    SeedHashCode code = (seed->code != BAD_SEED_HASH_CODE ? seed->code : getHashCode (seed->kmer));

    if (code >= _index.size())  {  throw MSG_INDEXATION_MSG1;  }

    /** A little shortcut. */
	IndexEntry& offsets = _index[code];

    return (offsets.size() > 0 ?
        new DatabaseOccurrenceIterator (getDatabase(), _span, &offsets, neighbourhoodSize)
        :
        (IOccurrenceIterator*)  new NullIterator<const ISeedOccurrence*> ()
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
IOccurrenceBlockIterator* DatabaseIndex::createOccurrenceBlockIterator (
    const seed::ISeed* seed,
    size_t neighbourhoodSize,
    size_t blockSize
)
{
    IOccurrenceBlockIterator* result = 0;

    SeedHashCode code = (seed->code != BAD_SEED_HASH_CODE ? seed->code : getHashCode (seed->kmer));

    if (code >= _index.size())  {  throw MSG_INDEXATION_MSG1;  }
    else
    {
    	/** A little shortcut. */
		IndexEntry& offsets = _index[code];

        if (offsets.size() > 0)
        {
            result = new DatabaseOccurrenceBlockIterator (
                getDatabase(),
                _span,
                &offsets,
                neighbourhoodSize,
                blockSize
            );
        }
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
IDatabaseIndex::IndexEntry& DatabaseIndex::getEntry (const seed::ISeed* seed)
{
    SeedHashCode code = (seed->code != BAD_SEED_HASH_CODE ? seed->code : getHashCode (seed->kmer));

    if (code >= _index.size())  {  throw MSG_INDEXATION_MSG1;  }
    else
    {
        return (IDatabaseIndex::IndexEntry&)_index[code];
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
size_t DatabaseIndex::getOccurrenceNumber (const seed::ISeed* seed)
{
    SeedHashCode code = (seed->code != BAD_SEED_HASH_CODE ? seed->code : getHashCode (seed->kmer));

    if (code >= _index.size())  {  throw MSG_INDEXATION_MSG1;  }
    else
    {
        return _index[code].size();
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
u_int64_t DatabaseIndex::getTotalOccurrenceNumber ()
{
    u_int64_t result = 0;

    for (size_t i=0; i<_index.size(); i++)
    {
        result += _index[i].size();
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
void DatabaseIndex::merge (void)
{
    DEBUG (("DatabaseIndex::merge : BEGIN (nbChildren=%ld) \n", _children.size() ));

    /** We loop over children indexes. */
    for (std::list<IDatabaseIndex*>::iterator it=_children.begin(); it != _children.end(); it++)
    {
        DEBUG (("child=%p  dbSize=%ld  nbSeq=%ld \n",
            *it, (*it)->getDatabase()->getSize(), (*it)->getDatabase()->getSequencesNumber()
        ));
    }

    /** We create an iterator that loops all seeds of the model. */
    ISeedIterator* itSeed = _model->createAllSeedsIterator();
    LOCAL (itSeed);

    /** We loop over all possible seeds for the given model. */
    for (itSeed->first(); !itSeed->isDone(); itSeed->next())
    {
        /** */
        const ISeed* seed = itSeed->currentItem();

        SeedHashCode code = (seed->code != BAD_SEED_HASH_CODE ? seed->code : getHashCode (seed->kmer));
        if (code >= _index.size())  {  throw MSG_INDEXATION_MSG1;  }

        /** A little shortcut. */
        IndexEntry& globalOccur = _index[code];

        /** We clear the occurrences vector of the 'this' instance for the current seed. */
        globalOccur.clear();

        /** We need to sum the sizes of the database for shifting the offsets in the merged index. */
        u_int64_t databasesSize = 0;

        /** We loop over children indexes. */
        for (std::list<IDatabaseIndex*>::iterator it=_children.begin(); it != _children.end(); it++)
        {
            /** A little shortcut. */
            DatabaseIndex* child = dynamic_cast<DatabaseIndex*> (*it);

            if (CHECKPTR(child))
            {
            	IndexEntry& childOccur =  child->_index [code];

                /** We have to align the offsets. */
                for (IndexEntry::iterator it = childOccur.begin(); it != childOccur.end(); it++)
                {
                    it->offsetInDatabase += databasesSize;
                }

                /** We add the child index into the parent index. */
                globalOccur.insert (globalOccur.end(), childOccur.begin(), childOccur.end());

                /** We increase the sum of the databases sizes. */
                databasesSize += child->getDatabase()->getSize();
            }
        }
    }

    size_t nbSeeds  = 0;
    size_t nbOccurs = 0;
    for (size_t i=0; i<_index.size(); i++)  {
        if (_index[i].size() > 0)
        {
            nbSeeds ++;
            nbOccurs += _index[i].size();
        }
    }
    DEBUG (("DatabaseIndex::merge : nbSeeds=%ld  nbOccurs=%ld \n",nbSeeds, nbOccurs));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void DatabaseIndex::DatabaseOccurrenceIterator::updateItem ()
{
    /** We retrieve other information: sequence and offset in sequence. */
    _database->getSequenceByOffset (
        (*_offsets)[_currentIdx].offsetInDatabase,
        _item.sequence,
        _item.offsetInSequence,
        _item.offsetInDatabase
    );

    DEBUG (("DatabaseIndex::DatabaseOccurrenceIterator::updateItem : seq='%s'\n",
        _item.sequence.data.toString().c_str()
    ));

    /** We may have to build the neighbourhood. */
    if (_neighbourSize > 0)
    {
        size_t imin1 = 0;
        size_t imin2 = 0;

        /** Shortcuts. */
        LETTER* bufIn  = _item.sequence.data.letters.data + _item.offsetInSequence;
        LETTER* bufOut = _item.neighbourhood.letters.data;

        /** We fill the word with default letter. */
        memset (bufOut, CODE_X, _item.neighbourhood.letters.size);

        /** We fill the seed + right neighbour. */
        imin1 = MIN (_span+_neighbourSize,  _item.sequence.data.letters.size - _item.offsetInSequence);
        memcpy (bufOut, bufIn, imin1);

        /** We fill the left neighbour.
         * Note that we copy from left to right; algorithms computing left scores will have to read it from right to left.
         */
        bufOut += (_span + _neighbourSize);
        imin2 = MIN (_neighbourSize, _item.offsetInSequence);

#if 1
        while (imin2-- > 0)
        {
            *(bufOut++) = *(--bufIn);
        }
#else
        memcpy (bufOut, bufIn - imin2, imin2);
#endif
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
DatabaseIndex::DatabaseOccurrenceBlockIterator::DatabaseOccurrenceBlockIterator (
    database::ISequenceDatabase* database,
    size_t span,
    IndexEntry* offsets,
    size_t neighbourSize,
    size_t nbSeedOccurPerIteration
)
    : _database(database),
      _span(span),
      _offsets(offsets),
      _neighbourSize(neighbourSize),
      _nbSeedOccurPerIteration (nbSeedOccurPerIteration),
      _table (0), _neighbourhoods(0), _isDone(true)
{
    /** We compute the size of a complete neighbourhood (seed + left + right). */
    _neighbourTotalSize = (_span + 2*_neighbourSize);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
DatabaseIndex::DatabaseOccurrenceBlockIterator::~DatabaseOccurrenceBlockIterator ()
{
    if (_table)             { delete[] _table;          }
    if (_neighbourhoods)    { delete[] _neighbourhoods; }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void DatabaseIndex::DatabaseOccurrenceBlockIterator::first ()
{
    if (_offsets && _offsets->size() > 0 &&  _nbSeedOccurPerIteration > 0)
    {
        /** We initialize the range to be iterated. */
        _range.begin = 0;
        _range.end   = MIN (_nbSeedOccurPerIteration, _offsets->size()) - 1;

        /** We update the isdone attribute. */
        _isDone = false;

        /** We reconfigure the seeds occurrences table. */
        configure ();
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
dp::IteratorStatus DatabaseIndex::DatabaseOccurrenceBlockIterator::next()
{
    /** We update the current range of seeds occurrences. */
    _range.begin += _nbSeedOccurPerIteration;
    _range.end   += _nbSeedOccurPerIteration;

    /** We update the isdone attribute. */
    _isDone = _range.begin >= _offsets->size();

    if (!_isDone)
    {
        /** We have to check that the end doesn't exceed the offsets size. */
        if (_range.end >= _offsets->size())    {  _range.end = _offsets->size() - 1;  }

        /** We reconfigure with the new range. */
        configure ();
    }

    return dp::ITER_UNKNOWN;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void DatabaseIndex::DatabaseOccurrenceBlockIterator::configure ()
{
    /** Shortcut. */
    size_t nb = _range.getLength();

    /** We resize the 'current item'. */
    _seedOccurVector.resize (nb);

    /** We create a table holding all the wanted occurrences. */
    if (_table)  { delete[] _table;  }
    _table = new ISeedOccurrence [nb];

    /** We create a buffer holding all neighbourhoods for the occurrences. Note that we
     *  allocate this array only at first call. */
    if (_neighbourhoods == 0)  {  _neighbourhoods = new LETTER [nb * _neighbourTotalSize];  }
    memset (_neighbourhoods, CODE_X, nb * _neighbourTotalSize);

    /** We need a cursor for iterating this buffer. */
    LETTER* cursor = _neighbourhoods;

    /** We fill the vector. */
    for (size_t currentIdx=0; currentIdx < nb; currentIdx++)
    {
        /** Shortcut. */
        ISeedOccurrence* occur = & (_table[currentIdx]);

        _seedOccurVector.data [currentIdx] = occur;

        /** We complete the information of the ISeedOccurrence instance.
         *  Note: in case the database is a composed database, we should be able now
         *  to retrieve the actual database and actual offset from the global offset
         *  we memorized during index building. Therefore, the ISeedOccurrence instance
         *  will reference information of the actual database. For instance, this may
         *  be useful when we have a single database composed of 6 ReadingFrame databases
         *  (when dealing with nucleotid databases transcripted in 6 reading frames); the
         *  index has been built with the composed database but here we want to know which
         *  sub database is actually the wanted one (ie one of the 6 reading frame database).
         */

        /** We retrieve other information: sequence and offsets in sequence and db. */
        _database->getSequenceByOffset (
            (*_offsets)[_range.begin + currentIdx].offsetInDatabase,
            occur->sequence,
            occur->offsetInSequence,
            occur->offsetInDatabase
        );

        /** We may have to build the neighbourhood. */
        if (_neighbourSize > 0)
        {
            size_t imin1 = 0;
            size_t imin2 = 0;

            /** Shortcuts. */
            LETTER* bufIn  = occur->sequence.data.letters.data + occur->offsetInSequence;
            LETTER* bufOut = cursor;

            /** We fill the seed + right neighbour. */
            imin1 = MIN (_span+_neighbourSize,  occur->sequence.data.letters.size - occur->offsetInSequence);
            memcpy (bufOut, bufIn, imin1);

            /** We fill the left neighbour.
             * Note that we copy from left to right; algorithms computing left scores will have to read it from right to left.
             */
            bufOut += (_span + _neighbourSize);
            imin2 = MIN (_neighbourSize, occur->offsetInSequence);

            while (imin2-- > 0)  {  *(bufOut++) = *(--bufIn);  }
        }

        /** We set the current ISeedOccurrence neighbourhood referenced buffer. */
        occur->neighbourhood.letters.setReference (_neighbourTotalSize, cursor);

        /** We move the cursor on the neighbourhoods buffer. */
        cursor += _neighbourTotalSize;

    } /* end of for (size_t currentIdx... */
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ISeedIterator* DatabaseIndexCodonStopOptim::createSeedsIterator (const database::IWord& data)
{
    ISeedIterator* result = 0;

    /** Shortcut. */
    size_t span = getModel()->getSpan();

    /** We create a local seeds iterator. */
    ISeedIterator* itSeed = getModel()->createSeedsIterator (data);
    LOCAL (itSeed);

    Offset previousOffset = 0;
    vector<Offset> badIntervals;

    /** We loop the seeds iterator for tagging intervals of holding bad letters. */
    for (itSeed->first(); !itSeed->isDone(); itSeed->next())
    {
        Offset currentOffset = itSeed->currentItem()->offset;

        if ( (currentOffset > span) && (currentOffset != previousOffset + 1) )
        {
            badIntervals.push_back (previousOffset + span);
            badIntervals.push_back (currentOffset  - 1);
        }

        previousOffset = currentOffset;
    }

    /** We duplicate the data we got as parameter. */
    IWord filteredData (data.letters.size, data.letters.data);

    LETTER* theData = filteredData.letters.data;

    size_t nbInarowLettersRequired = _range;

    for (size_t i=2; i<badIntervals.size(); i+=2)
    {
        Offset off0 = badIntervals[i-1];
        Offset off1 = badIntervals[i];

        /** We may have to invalidate some intervals holding not enough letters until reaching a stop codon. */
        if ( (theData[off1] == CODE_STAR)  &&  (off1 - off0 < nbInarowLettersRequired) )
        {
            memset (theData+off0+1, CODE_X, off1-off0-1);
        }
    }

    /** We create the result with the potentially filtered out data. */
    result = getModel()->createSeedsIterator (filteredData);

    /** We return the result. */
    return result;
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
