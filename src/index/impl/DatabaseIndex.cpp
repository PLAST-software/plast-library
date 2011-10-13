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

#include "DatabaseIndex.hpp"
#include "macros.hpp"

using namespace std;
using namespace dp;
using namespace database;
using namespace seed;

#include <stdio.h>
#define DEBUG(a) // printf a

/********************************************************************************/
namespace indexation {
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
    : AbstractDatabaseIndex (database, model), _currentSequence(0), _span(0), _alphabetSize(0)
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

        DEBUG (("DatabaseIndex::iterateSeed: this=%p  seed='%s' (code=%ld) => offset=%ld\n",  this,
            seed->kmer.toString().c_str(),
            seed->code,
            _sequenceOffset + seed->offset
        ));

        /** We add the offset in the database for the current seed. */
        entry.push_back (_sequenceOffset + seed->offset);
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

        DEBUG (("DatabaseIndex::build : current sequence '%s'\n", _currentSequence->data.toString().c_str()));

        /** We create a seed iterator. */
        ISeedIterator* itSeed = getModel()->createSeedsIterator (_currentSequence->data);
        LOCAL (itSeed);

        /** We iterate the seed iterator. */
        itSeed->iterate (this, (Iterator<const ISeed*>::Method) &DatabaseIndex::iterateSeed);

        /** We update the current sequence offset (ie offset in the whole database). */
        _sequenceOffset += _currentSequence->data.letters.size;

        /** We increase the number of found sequences. */
        nbSequences++;
    }

    DEBUG (("DatabaseIndex::build : END SEQUENCES LOOP\n"));

    DEBUG (("DatabaseIndex::build : found %ld seeds occurrences. %ld sequences\n", nbOccurrences, nbSequences));
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

    if (code > _index.size())
    {
        printf ("ERROR!!! : BAD HASH CODE\n");
        return 0;
    }

    /** A little shortcut. */
    std::vector<SequenceOffset>& offsets = _index[code];

    return (offsets.size() > 0 ? new DatabaseOccurrenceIterator (getDatabase(), _span, &offsets, neighbourhoodSize) : 0);
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

    if (code > _index.size())
    {
        printf ("ERROR!!! : BAD HASH CODE\n");
    }
    else
    {
        /** A little shortcut. */
        std::vector<SequenceOffset>& offsets = _index[code];

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
size_t DatabaseIndex::getOccurrenceNumber (const seed::ISeed* seed)
{
    SeedHashCode code = (seed->code != BAD_SEED_HASH_CODE ? seed->code : getHashCode (seed->kmer));

    if (code > _index.size())
    {
        printf ("ERROR!!! : BAD HASH CODE\n");
        return 0;
    }
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
        if (code > _index.size())  {  printf ("ERROR!!! : BAD HASH CODE\n");  return ;  }

        /** A little shortcut. */
        vector<SequenceOffset>& globalOccur = _index[code];

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
                vector<SequenceOffset>& childOccur =  child->_index [code];

                /** We have to align the offsets. */
                for (size_t j=0; j<childOccur.size(); j++)
                {
                    SequenceOffset& occur = childOccur[j];
                    occur += databasesSize;
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
    /** We get the offset of the occurrence in the database. */
    _item.offsetInDatabase  = (*_offsets)[_currentIdx];

    /** We retrieve other information: sequence and offset in sequence. */
    _database->getSequenceByOffset (_item.offsetInDatabase, _item.sequence, _item.offsetInSequence);

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
    size_t blockSize
)
    : _database(database),
      _span(span),
      _offsets(offsets),
      _neighbourSize(neighbourSize),
      _blockSize(blockSize),
      _occurrences (offsets->size()),
      _table (0), _neighbourhoods(0)
{
    if (offsets && offsets->size() > 0)
    {
        size_t nb = offsets->size();

        /** We create a table holding all the wanted occurrences. */
        _table = new ISeedOccurrence [nb];

        /** We compute the size of a complete neighbourhood (seed + left + right). */
        _neighbourTotalSize = (_span+2*_neighbourSize);

        /** We create a buffer holding all neighbourhoods for the occurrences. */
        _neighbourhoods = new LETTER [nb * _neighbourTotalSize];
        memset (_neighbourhoods, CODE_X, nb * _neighbourTotalSize);

        /** We need a cursor for iterating this buffer. */
        LETTER* cursor = _neighbourhoods;

        /** We fill the vector. */
        for (size_t currentIdx=0; currentIdx<nb; currentIdx++)
        {
            ISeedOccurrence* occur = & (_table[currentIdx]);

            /** We set it to current vector entry. */
            _occurrences.data[currentIdx] = occur;

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
#if 1
            _database->getActualInfoFromOffset (
                (*_offsets)[currentIdx],
                occur->database,
                occur->offsetInDatabase
            );
            if (occur->database == 0)  { /* should throw some exception */ }

            /** We retrieve other information: sequence and offset in sequence. */
            (occur->database)->getSequenceByOffset (occur->offsetInDatabase, occur->sequence, occur->offsetInSequence);
#else
            occur->database          = _database;
            occur->offsetInDatabase  = (*_offsets)[currentIdx];

            /** We retrieve other information: sequence and offset in sequence. */
            _database->getSequenceByOffset (occur->offsetInDatabase, occur->sequence, occur->offsetInSequence);
#endif

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

        /** We compute the number of iterations.  */
        _vectorsList = _occurrences.split (_blockSize);
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
DatabaseIndex::DatabaseOccurrenceBlockIterator::~DatabaseOccurrenceBlockIterator ()
{
    if (_table)             { delete[] _table;          }
    if (_neighbourhoods)    { delete[] _neighbourhoods; }
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
