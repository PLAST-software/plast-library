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

#include <index/impl/DatabaseNucleotidIndex.hpp>
#include <misc/api/macros.hpp>

using namespace std;
using namespace dp;
using namespace database;
using namespace seed;

#include <stdio.h>
#define DEBUG(a)    //printf a
#define VERBOSE(a)  //printf a

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
DatabaseNucleotidIndex::DatabaseNucleotidIndex (ISequenceDatabase* database, ISeedModel* model)
    : AbstractDatabaseIndex (database, model), _span(0), _alphabetSize(0), _badLetter(0)
{
    _span           = getModel()->getSpan();
    _alphabetSize   = getModel()->getAlphabet()->size;
    _badLetter      = EncodingManager::singleton().getAlphabet(SUBSEED)->any;

    _maxSeedsNumber = 1;
    for (size_t i=1; i<=_span; i++)  { _maxSeedsNumber *= _alphabetSize; }

    /** We set the size of the index. */
    _index.resize (_maxSeedsNumber);

    DEBUG (("DatabaseNucleotidIndex::DatabaseNucleotidIndex: _maxSeedsNumber=%ld  _alphabetSize=%ld  _badLetter=%d \n",
		_maxSeedsNumber, _alphabetSize, _badLetter
	));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
DatabaseNucleotidIndex::~DatabaseNucleotidIndex ()
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
void DatabaseNucleotidIndex::build ()
{
    DEBUG (("DatabaseNucleotidIndex::build : START ! \n"));

    int32_t bitshift = 2*(_span-1);
    int32_t bitmask  = (1 << (2*_span)) - 1;

    /** The intent of this method is to fill the _index attribute; this attribute is designed to hold (for each possible seed
     *  of the seeds model) the vector of offset occurrences. These offsets are relative to the provided sequences iterator.
     */

    /** We create a sequence iterator that iterates the database. */
    ISequenceIterator* seqIter = getDatabase()->createSequenceIterator();
    LOCAL(seqIter);

    size_t    nbEntries      = 0;
    size_t    nbSequences    = 0;
    u_int64_t sequenceOffset = 0;

    DEBUG (("DatabaseNucleotidIndex::build : BEGIN SEQUENCES LOOP\n"));

    /** Current sequence parsed during index build. */
    const database::ISequence* currentSequence = 0;


    vector<size_t> counter (_maxSeedsNumber, 0);

#if 1
    /** We loop over all the sequences. */
    for (seqIter->first(); !seqIter->isDone(); seqIter->next())
    {
        /** A little shortcut for the currently iterated sequence. */
        currentSequence  = seqIter->currentItem();

        u_int32_t  length = currentSequence->getLength();

        if (length > _span)
        {
            const LETTER* data   = currentSequence->getData();

            size_t  nbMatch = _span;

            /** We compute the beginning hash code. */
            SeedHashCode hashCode = data[_span-1];
            for (int i=_span-2; i>=0; i--)  {  hashCode = (hashCode << 2) + data[i];  }
        	counter [hashCode] += 1;

            VERBOSE (("DatabaseNucleotidIndex::build : first code=%d \n", hashCode));

            /** We loop the remaining data; we update the hash code from the previous one. */
            data += _span;
            int imax = length - _span;
            for (int i=1; i<=imax; i++, data++)
            {
            	LETTER s = *data;

            	/** We update the hash code from the previous one. */
            	hashCode = ((hashCode >> 2)  +  (s << bitshift)) & bitmask;

                if (s < _badLetter)
                {
                    nbMatch++;

                    if (nbMatch >= _span)
                    {
                    	counter [hashCode] += 1;
						nbEntries++;
                    }
                }
                else
                {
                    nbMatch = 0;
                }
            }
        }
    }

#endif

	DEBUG (("DatabaseNucleotidIndex::build : COUNTING IS DONE...\n"));

    size_t nbFoundSeeds = 0;

    for (size_t currentCode=0; currentCode<_maxSeedsNumber; currentCode++)
    {
    	size_t len = counter[currentCode];

    	if (len > 0)
    	{
    		_index[currentCode].resize (len);
    		counter[currentCode] = 0;
    		nbFoundSeeds++;
    	}
    }
	DEBUG (("DatabaseNucleotidIndex::build : HERE WE GO  nbEntries=%ld  nbFoundSeeds=%ld ...\n", nbEntries, nbFoundSeeds));


    /** We loop over all the sequences. */
    for (seqIter->first(); !seqIter->isDone(); seqIter->next())
    {
        /** A little shortcut for the currently iterated sequence. */
        currentSequence  = seqIter->currentItem();

        VERBOSE (("DatabaseNucleotidIndex::build : current sequence (len=%d)  '%s'\n",
			currentSequence->getLength(),
			currentSequence->data.toString().c_str()
		));
        VERBOSE (("DatabaseNucleotidIndex::build : current sequence len=%d \n", currentSequence->getLength() ));

        u_int32_t  length = currentSequence->getLength();

        if (length > _span)
        {
            const LETTER* data   = currentSequence->getData();

            size_t  nbMatch = _span;

            /** We compute the beginning hash code. */
            SeedHashCode hashCode = data[_span-1];
            for (int i=_span-2; i>=0; i--)  {  hashCode = (hashCode << 2) + data[i];  }

            VERBOSE (("DatabaseNucleotidIndex::build : first code=%d \n", hashCode));

            /** We retrieve the index corresponding to the seed. */
            IndexEntry& entry = _index[hashCode];

            if (entry.empty())  {  DEBUG (("CAN'T HAPPEN   hash=%d !!!\n", hashCode));  continue;  }

            /** We add the offset in the database for the current seed. */
#if 0
            entry.push_back (SeedOccurrence (sequenceOffset + 0, currentSequence->index));
#else
            size_t& count = counter[hashCode];
            SeedOccurrence& occur = entry [count++];

            occur.offsetInDatabase = sequenceOffset + 0;
            occur.sequenceIdx      = currentSequence->index;
#endif
            nbEntries++;

            /** We loop the remaining data; we update the hash code from the previous one. */
            data += _span;
            int imax = length - _span;
            for (int i=1; i<=imax; i++, data++)
            {
            	LETTER s = *data;

            	/** We update the hash code from the previous one. */
            	hashCode = ((hashCode >> 2)  +  (s << bitshift)) & bitmask;

				VERBOSE (("DatabaseNucleotidIndex::build : s=%d  othercode=%d   (bitshift=%d  bitmask=%d) \n",
					s, hashCode, bitshift, bitmask
				));

                if (s < _badLetter)
                {
                    nbMatch++;

                    if (nbMatch >= _span)
                    {
						IndexEntry& entry = _index[hashCode];
#if 0
						entry.push_back (SeedOccurrence (sequenceOffset + i, currentSequence->index));
#else
			            size_t& count = counter[hashCode];
			            SeedOccurrence& occur = entry [count++];
			            occur.offsetInDatabase = sequenceOffset + i;
			            occur.sequenceIdx      = currentSequence->index;
#endif
						nbEntries++;
                    }
                }
                else
                {
                    nbMatch = 0;
                }
            }
        }

        /** We update the current sequence offset (ie offset in the whole database). */
        sequenceOffset += currentSequence->getLength();

        /** We increase the number of found sequences. */
        nbSequences++;
    }

    DEBUG (("DatabaseNucleotidIndex::build : END SEQUENCES LOOP\n"));

    DEBUG (("DatabaseNucleotidIndex::build : %ld sequences  %ld entries\n", nbSequences, nbEntries));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IDatabaseIndex::IndexEntry& DatabaseNucleotidIndex::getEntry (const seed::ISeed* seed)
{
    if (seed->code >= _index.size())
    {
        return _index[0];
    }
    else
    {
        return _index[seed->code];
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
size_t DatabaseNucleotidIndex::getOccurrenceNumber (const seed::ISeed* seed)
{
    if (seed->code >= _index.size())
    {
        return 0;
    }
    else
    {
        return _index[seed->code].size();
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
u_int64_t DatabaseNucleotidIndex::getTotalOccurrenceNumber ()
{
    u_int64_t result = 0;

    for (size_t i=0; i<_index.size(); i++)
    {
        result += _index[i].size();
    }

    return result;
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
