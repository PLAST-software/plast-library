/*
 * ICommand.hpp
 *
 *  Created on: Jun 20, 2011
 *      Author: edrezen
 */

#include "DatabaseIndex.hpp"
#include "macros.hpp"

using namespace std;
using namespace dp;
using namespace database;
using namespace seed;

#include <stdio.h>
#define DEBUG(a) // printf a

#define MIN(a,b)  ((a) < (b) ? (a) : (b))

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
    //printf ("DatabaseIndex::~DatabaseIndex:  this=%p \n", this);
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

    // dump ();

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

//    DEBUG (("DatabaseIndex::createOccurrenceIterator: this=%p  seed='%s'  nbOccurs=%ld\n", this,
//        seed->kmer.toString().c_str(),
//        offsets.size()
//    ));

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
void DatabaseIndex::dump (void)
{
    printf ("------------------------ DUMP ------------------------\n");

    /** We create an iterator that loops all seeds of the model. */
    ISeedIterator* itSeed = _model->createAllSeedsIterator();
    LOCAL (itSeed);

    /** We loop over all possible seeds for the given model. */
    for (itSeed->first(); !itSeed->isDone(); itSeed->next())
    {
        /** */
        const ISeed* seed = itSeed->currentItem();

        IOccurrenceIterator* it = createOccurrenceIterator (seed, 0);
        if (it != 0)
        {
            LOCAL (it);

            for (it->first(); ! it->isDone(); it->next())
            {
                printf ("SEED '%s' => offset=%ld\n", seed->kmer.toString().c_str(), it->currentItem()->offsetInDatabase);
            }
        }
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

//        if ((imin1 + imin2)  != (_span + 2*_neighbourSize))
//            printf ("m1=%ld  m2=%ld  t=%ld  offset=%ld  length=%ld  span=%ld  size=%ld\n",
//                imin1, imin2, (imin1+imin2), _item.offsetInSequence,
//                _item.sequence.data.letters.size, _span, _neighbourSize
//            );
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

            /** We complete the information. */
            occur->database          = _database;
            occur->offsetInDatabase  = (*_offsets)[currentIdx];

            /** We retrieve other information: sequence and offset in sequence. */
            _database->getSequenceByOffset (occur->offsetInDatabase, occur->sequence, occur->offsetInSequence);

#if 0
      printf ("+++ [%ld,%ld]  offset=%ld  seq '%s'\n", currentIdx, nb, occur->offsetInSequence, occur->sequence.toString().c_str() );
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

#if 0
    const LETTER* convert = EncodingManager::singleton().getEncodingConversion(SUBSEED, ASCII);
    printf ("***> %ld: ", _neighbourTotalSize);  for (size_t k=0; k<_neighbourTotalSize; k++)  { printf ("%c", convert[cursor[k]]); }  printf("\n\n");
#endif
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
