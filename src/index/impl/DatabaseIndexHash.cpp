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

#include <index/impl/DatabaseIndexHash.hpp>
#include <os/impl/DefaultOsFactory.hpp>
#include <misc/api/macros.hpp>

extern "C"
{
    #include "retrievall.h"
}

using namespace std;
using namespace os;
using namespace os::impl;
using namespace dp;
using namespace database;
using namespace seed;

#include <stdio.h>
#undef DEBUG
#define DEBUG(a)  //printf a

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
DatabaseIndexHash::DatabaseIndexHash (ISequenceDatabase* database, ISeedModel* model)
    : AbstractDatabaseIndex (database, model), _index(0), _hashTable(0)
{
    DEBUG (("DatabaseIndex::DatabaseIndex: _maxSeedsNumber=%ld\n", _maxSeedsNumber));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
DatabaseIndexHash::~DatabaseIndexHash ()
{
    DefaultFactory::memory().free (_hashTable);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void DatabaseIndexHash::initHashTable ()
{
    if (_hashTable == 0)
    {
        DEBUG (("DatabaseIndex::initHashTable : hashTable=%p  _maxSeedsNumber=%ld\n",
            _hashTable, _maxSeedsNumber
        ));

        DEBUG (("DatabaseIndex::initHashTable : sizeof(IndexEntry*)=%d  sizeof(unsigned int)=%d\n",
            sizeof(IndexEntry*), sizeof(unsigned int)
        ));

        /** We need an array that holds the keys. */
        const LETTER* keysTable = getModel()->getAllSeedsTable();

        //for (size_t i=0; i<getModel()->getSpan()*_maxSeedsNumber; i++)  {  printf ("%d ", keysTable[i]);  if (i%40==0)  { printf("\n"); }  }

        _indexVector.resize (_maxSeedsNumber);
        for (size_t i=0; i<_maxSeedsNumber; i++)  {  _indexVector[i] = new IndexEntry ();  }

        /** We need an array that holds the values. */
        _index =  (unsigned int*) DefaultFactory::memory().calloc (_maxSeedsNumber,sizeof(unsigned int));
        for (size_t i=0; i<_maxSeedsNumber; i++)  {  _index[i] = i;   }

        /** We create the hash structure. */
        _hashTable = (retr_t*) DefaultFactory::memory().malloc (sizeof(retr_t));
        if (_hashTable != 0)
        {
            _hashTable->length_in_bits = 0;
            _hashTable->nkeys          = _maxSeedsNumber;
            _hashTable->keys_length    = getModel()->getSpan() * sizeof(LETTER);
            _hashTable->keys_table     = (LETTER*) keysTable;
            _hashTable->values_table   = (unsigned int*)_index;
            _hashTable->is_packed      = 0;
            _hashTable->vals_length    = 16;   // size in bits of the values

            init_retr     (_hashTable);
            generate_retr (_hashTable);

            DEBUG (("DatabaseIndex::initHashTable : _hashTable=%p \n", _hashTable));
        }

        DEBUG (("DatabaseIndex::initHashTable : DONE \n"));
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
void DatabaseIndexHash::build ()
{
    DEBUG (("DatabaseIndex::build : START ! \n"));

    /** The intent of this method is to fill the _index attribute; this attribute is designed to hold (for each possible seed
     *  of the seeds model) the vector of offset occurrences. These offsets are relative to the provided sequences iterator.
     */

    /** We may have to initialize the hash table structure. */
    initHashTable();

    /** We create a sequence iterator that iterates the database. */
    ISequenceIterator* seqIter = getDatabase()->createSequenceIterator();
    LOCAL(seqIter);

    size_t nbSequences   = 0;
    size_t nbOccurrences = 0;
    u_int64_t sequenceOffset = 0;

    DEBUG (("DatabaseIndex::build : BEGIN SEQUENCES LOOP\n"));

    /** We loop over all the sequences. */
    for (seqIter->first(); !seqIter->isDone(); seqIter->next())
    {
        /** A little shortcut for the currently iterated sequence. */
        const ISequence* seq  = seqIter->currentItem();

        /** We create a seed iterator. */
        ISeedIterator* itSeed = getModel()->createSeedsIterator (seq->data);
        LOCAL (itSeed);

        /** We loop over all seeds for the current sequence. */
        for (itSeed->first(); !itSeed->isDone(); itSeed->next())
        {
            /** A little shortcut. */
            const ISeed* currentSeed = itSeed->currentItem();

            /** We retrieve the value from the kmer. */
            u_int16_t idx = query_retr ((char *) (currentSeed->kmer.letters.data), _hashTable);

            /** We add the offset in the database for the current seed. */
            IndexEntry* entry = _indexVector[idx];

            entry->push_back (sequenceOffset + currentSeed->offset);

            /** We count the number of occurrences (debug purpose). */
            nbOccurrences ++;
        }

        /** We update the current sequence offset. */
        sequenceOffset += seq->data.letters.size;

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
IOccurrenceIterator* DatabaseIndexHash::createOccurrenceIterator (const ISeed* seed, size_t neighbourhoodSize)
{
    IOccurrenceIterator* result = 0;
#if 0
    /** We retrieve the value from the kmer. */
    u_int16_t idx = query_retr ((char *) (seed.letters), _hashTable);
if (idx > _maxSeedsNumber)
{
        idx,
        seed.letters[0],
        seed.letters[1],
        seed.letters[2]
    );
}

    /** We add the offset in the database for the current seed. */
    IndexEntry* entry = _indexVector[idx];
    if (entry != 0)
    {
        result =  new DatabaseOccurrenceIterator (getDatabase(), entry, 0);
    }
#endif
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
IOccurrenceBlockIterator* DatabaseIndexHash::createOccurrenceBlockIterator (
    const seed::ISeed* seed,
    size_t neighbourhoodSize,
    size_t blockSize
)
{
    return 0;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void DatabaseIndexHash::merge (void)
{
    DEBUG (("DatabaseIndex::merge : BEGIN\n"));

#if 0
    /** We loop over all possible seeds. */
    for (size_t seedCode = 0; seedCode < _maxSeedsNumber; seedCode++)
    {
        /** A little shortcut. */
        vector<SequenceOffset>& globalOccur = _index[seedCode];

        /** We clear the occurrences vector of the 'this' instance for the current seed. */
        globalOccur.clear();

        size_t nbSequences = 0;

        /** We loop over children indexes. */
        for (std::list<IDatabaseIndex*>::iterator it=_children.begin(); it != _children.end(); it++)
        {
            /** A little shortcut. */
            DatabaseIndexHash* child = dynamic_cast<DatabaseIndexHash*> (*it);

            if (CHECKPTR(child))
            {
                vector<SequenceOffset>& childOccur =  child->_index [seedCode];

                /** We have to align the sequence number. */
                for (size_t j=0; j<childOccur.size(); j++)
                {
                    SequenceOffset& occur = childOccur[j];
                    occur.sequenceId += nbSequences;
                }

                /** We add the child index into the parent index. */
                globalOccur.insert (globalOccur.end(), childOccur.begin(), childOccur.end());
            }

            nbSequences += child->getDatabase()->getSequencesNumber();
        }
    }
#endif
    DEBUG (("DatabaseIndex::merge : END\n"));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void DatabaseIndexHash::dump (void)
{
#if 0
    DEBUG (("------------------------ DUMP ------------------------\n"));
    /** We loop over all possible seeds. */
    for (size_t seedCode = 0; seedCode < _maxSeedsNumber; seedCode++)
    {
        IOccurrenceIterator* it = createOccurrenceIterator (seedCode);
        if (it != 0)
        {
            LOCAL (it);

            size_t count = 0;
            for (it->first(); ! it->isDone(); it->next())  { count++; }

            DEBUG (("SEED %ld => %ld items\n", seedCode, count));
        }
    }
#endif
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
