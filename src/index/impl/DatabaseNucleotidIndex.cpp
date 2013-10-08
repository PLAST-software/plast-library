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
#include <misc/api/PlastStrings.hpp>

#include <designpattern/impl/CommandDispatcher.hpp>

#include <os/impl/DefaultOsFactory.hpp>
#include <os/impl/TimeTools.hpp>

using namespace std;

using namespace os;
using namespace os::impl;

using namespace dp;
using namespace dp::impl;

using namespace database;

using namespace seed;

/** Some macros. */
#define LETTER_NUC(l)    ((l) & 3)
#define LETTER_AA(l)     ((l) & 31)
#define LETTER_ASCII(l)  ((l) & 127)
//#define LETTER_ISBAD(l)  ((l) & 128)
#define LETTER_ISBAD(l)  ((l) & 4)

#include <stdio.h>
#define DEBUG(a)    //printf a
#define VERBOSE(a)  //printf a

/********************************************************************************/
namespace indexation { namespace impl {
/********************************************************************************/

typedef u_int64_t word_t;
enum { WORD_SIZE = sizeof(word_t) * 8 };

inline int bindex(int b)  { return b / WORD_SIZE; }
inline int boffset(int b) { return b % WORD_SIZE; }

#define GETMASK(data,b)  (data[bindex(b)] & (1 << (boffset(b))))
#define SETMASK(data,b)  data[bindex(b)] |= 1 << (boffset(b))

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
DatabaseNucleotidIndex::DatabaseNucleotidIndex (ISequenceDatabase* database, ISeedModel* model, IDatabaseIndex* otherIndex)
    : AbstractDatabaseIndex (database, model), _counter(0), _span(0), _bitshift(0), _maskIn(0), _maskOut(0)
{
    /** Shortcuts. */
    _span     = getModel()->getSpan();
    _bitshift = 2*(_span-1);

    size_t alphabetSize = getModel()->getAlphabet()->size;

    /** We compute the maximum number of seeds. This is for instance 4^11=4194304 for nucleotide 11-kmers. */
    _maxSeedsNumber = 1;
    for (size_t i=1; i<=_span; i++)  { _maxSeedsNumber *= alphabetSize; }

    /** We allocate the vector that counts the number or occurrences for each possible seeds. */
    _counter = (u_int32_t*) DefaultFactory::memory().calloc (_maxSeedsNumber, sizeof(u_int32_t));
    memset (_counter, 0, _maxSeedsNumber*sizeof(u_int32_t));

    /** We set the size of the index. Such index is a vector of vectors: the 'parent' vector size is the
     * number of possible seeds. For each seed, the 'child' vector holds locations (and possibly other
     * information) of the current seed occurrences in the database.
     * The total number of cells should be (at most) the size of the database since each letter of the database
     * is the beginning of a seed, seed that should be indexed. Note however that some seeds are not indexed if
     * they contain some bad letter (like 'N'), which can happen quite often when "dust" algorithm has been used
     * for tagging (with 'N') low informative regions in the database. */
    _index.resize (_maxSeedsNumber);

    size_t maskSize = _maxSeedsNumber / WORD_SIZE;

    _maskOut = new word_t [maskSize];  memset (_maskOut, 0, sizeof(word_t)*maskSize);
    _maskIn  = new word_t [maskSize];

    if (otherIndex != 0 && otherIndex->getMask())
    {
        memcpy (_maskIn, otherIndex->getMask(),  sizeof(word_t)*maskSize);
    }
    else
    {
        memset (_maskIn,  ~0, sizeof(word_t)*maskSize);
    }

    DEBUG (("DatabaseNucleotidIndex::DatabaseNucleotidIndex: _maxSeedsNumber=%ld  _alphabetSize=%ld\n",
		_maxSeedsNumber, alphabetSize
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
    /** We release resources. */
    DefaultFactory::memory().free (_counter);

    delete[] _maskIn;
    delete[] _maskOut;
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
    /** We need a command dispatcher. */
    ParallelCommandDispatcher dispatcher;

    /** Some time statistics. */
    TimeInfo timeInfo (DefaultFactory::time());

    DEBUG (("DatabaseNucleotidIndex::build : START ! \n"));

    size_t nbcpu = DefaultFactory::thread().getNbCores();

    /** The intent of this method is to fill the '_index' attribute; this attribute is designed to hold (for each possible seed
     *  of the seeds model) the vector of offset occurrences. These offsets are relative to the provided sequences iterator.
     *
     * The job is done in 3 phases:
     *   1) for each seed, count the occurrences number (ie. fill the '_counter' vector).
     *   2) for each seed, resize the vector of occurrences
     *   3) for each seed, fill the vector of occurrences (occurrence offset in database)
     *
     *   Note that phases 1 and 3 can be parallelized; therefore, these phases are done within a ICommand context, and
     *   can be executed on several threads thanks to a ParallelCommandDispatcher.
     */

    /** We create a sequence iterator that iterates the database. */
    ISequenceIterator* seqIter = getDatabase()->createSequenceIterator();
    LOCAL(seqIter);

    timeInfo.addEntry ("b");

    /************************************************************/
    /*********************** PHASE 1 ****************************/
    /************************************************************/

    timeInfo.addEntry ("1");

    /** We need a "get" iterator on the sequence iterator for parallel iteration. */
    IteratorGet<const database::ISequence*>* seqIterGetCount = new IteratorGet<const ISequence*> (seqIter);
    LOCAL (seqIterGetCount);

    /** We build a list of commands that will iterate our list, through the created iterator. */
    list<ICommand*> commands;
    for (size_t i=1; i <= nbcpu; i++)  {  commands.push_back (new CountSeedsCmd (seqIterGetCount, this));  }
    dispatcher.dispatchCommands (commands,0);

    timeInfo.stopEntry ("1");

	DEBUG (("DatabaseNucleotidIndex::build : COUNTING IS DONE...\n"));

    /************************************************************/
    /*********************** PHASE 2 ****************************/
    /************************************************************/

    timeInfo.addEntry ("2");

    u_int64_t nbOccurrences = 0;

	/** Now, we can allocate structures since we know the number of occurrences for each seed. */
    for (size_t currentCode=0; currentCode<_maxSeedsNumber; currentCode++)
    {
    	size_t len = _counter[currentCode];

    	nbOccurrences += len;
    	if (len > 0 && GETMASK (_maskIn, currentCode))
    	{
    		_index[currentCode].resize (len);
    		_counter[currentCode] = 0;

    		/** We setup the mask */
    		SETMASK (_maskOut, currentCode);
    	}
    }

    timeInfo.stopEntry ("2");

    /************************************************************/
    /*********************** PHASE 3 ****************************/
    /************************************************************/

    timeInfo.addEntry ("3");

    /** We need a "get" iterator on the sequence iterator for parallel iteration. */
    IteratorGet<const database::ISequence*>* seqIterGetFill = new IteratorGet<const ISequence*> (seqIter);
    LOCAL (seqIterGetFill);

    commands.clear();
    for (size_t i=1; i <= nbcpu; i++)  {  commands.push_back (new FillSeedsCmd (seqIterGetFill, this));  }
    dispatcher.dispatchCommands (commands,0);

    timeInfo.stopEntry ("3");

    timeInfo.stopEntry ("b");

    DEBUG (("DatabaseNucleotidIndex::build : END SEQUENCES LOOP\n"));

    DEBUG (("DatabaseNucleotidIndex::build: count in %d msec (%.1f), resize in %d msec (%.1f), fill in %d msec (%.1f)=> total %d msec\n",
        timeInfo.getEntryByKey("1"),  100.0 * (double) timeInfo.getEntryByKey("1") / (double)timeInfo.getEntryByKey("b"),
        timeInfo.getEntryByKey("2"),  100.0 * (double) timeInfo.getEntryByKey("2") / (double)timeInfo.getEntryByKey("b"),
        timeInfo.getEntryByKey("3"),  100.0 * (double) timeInfo.getEntryByKey("3") / (double)timeInfo.getEntryByKey("b"),
        timeInfo.getEntryByKey("b")
    ));

    DEBUG (("DatabaseNucleotidIndex::build : %ld sequences  %ld occurrences  dbSize=%ld => %ld letter(s) not used\n",
        getDatabase()->getSequencesNumber(), nbOccurrences, getDatabase()->getSize(),
        getDatabase()->getSize() - (_span-1)*getDatabase()->getSequencesNumber() - nbOccurrences
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
IDatabaseIndex::IndexEntry& DatabaseNucleotidIndex::getEntry (const seed::ISeed* seed)
{
    if (seed->code >= _index.size())   {  throw MSG_INDEXATION_MSG1; }

    return (IDatabaseIndex::IndexEntry&) _index[seed->code];
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
    if (seed->code >= _index.size())   {  throw MSG_INDEXATION_MSG1; }

    return _index[seed->code].size();
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

    for (size_t i=0; i<_index.size(); i++)   {  result += _index[i].size();  }

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
void DatabaseNucleotidIndex::countSeedsOccurrences (const ISequence*& sequence)
{
    /** This method takes a sequence as input and loops over all possible seeds
     *  on that sequence. For each seed (seen through its hash code), we increment
     *  its counter (ie. _counter attribute)
     *
     *  Note: this method can be called on the same instance concurrently by
     *  different threads . Since we modify an attribute of the instance, we must take
     *  care on concurrent access on '_counter' attribute. This is done with the
     *  intrinsic '__sync_fetch_and_add' command.
     *
     *  Note: When we deal with nucleotides, we can compute the next hash code
     *  knowing the current one and the next letter. This can be done efficiently
     *  with binary operators like >> and |. However the first hash code has to
     *  be computed in a slightly different way, so the method make the same treatment,
     *  first on the first hash code and after on all successive hash codes.
     *
     *  Note: We consider that we find a valid hash code when all the letters of the
     *  seeds are valid (cf usage of LETTER_ISBAD macro). This means that a seed holding
     *  one ore more 'N' character won't be kept in the index.
     */

    /** Shortcuts. */
    u_int32_t     length = sequence->getLength();
    const LETTER* data   = sequence->getData();

    if (length > _span)
    {
        size_t       nbMatch  = 0;
        SeedHashCode hashCode = 0;

        /** We compute the beginning hash code. */
        for (int i=_span-1; i>=0; i--)
        {
            LETTER l  = data[i];
            hashCode  = (hashCode << 2) | LETTER_NUC(l);
            nbMatch  += LETTER_ISBAD(l) ? 0 : 1;
        }

        /** We increment the counter for the current seed if that seed is valid.
         *  Note the usage of '__sync_fetch_and_add': we get the value (the number
         *  of occurrences for 'hashCode') and increment it in a single protected
         *  instruction.
         */
        if (nbMatch >= _span  && GETMASK(_maskIn,hashCode))  { __sync_fetch_and_add (_counter + hashCode,1 );  }

        /** We have computed the hash code for the first '_span' letters of the sequence.
         * Now, we will need only one letter for computing the next hash code, which explains
         * why we need to move the 'data' pointer '_span' letters further. */
        data    += _span;
        int imax = length - _span;

        /** We loop the remaining data. */
        for (int i=1; i<=imax; i++, data++)
        {
            LETTER s = *data;

            /** We update the hash code from the previous one:
             *      we get rid of the first letter:            (hashCode >> 2)
             *      we add the new one at the end of the seed: (LETTER_NUC(s) << _bitshift)  */
            hashCode = (hashCode >> 2) | (LETTER_NUC(s) << _bitshift);

            /** Note that we compute the hash code even if the letter is considered as bad.
             *  Actually, the hash code is updated with some valid letter, thanks to LETTER_NUC
             *  that returns any valid letter. Although the hash code is updated, this hash code
             *  won't be used in the index thanks to the LETTER_ISBAD macro. In such a case, we
             *  reset the 'nbMatch' value, and since we need 'nbMatch >= _span' for valid seed,
             *  this updated hash code occurrence won't be used by the index.
             */
            if (LETTER_ISBAD(s))   {  nbMatch = 0;  }
            else
            {
                nbMatch++;

                /** We increment the counter for the current seed if that seed is valid. */
                if (nbMatch >= _span && GETMASK(_maskIn,hashCode))  {  __sync_fetch_and_add (_counter+hashCode, 1);  }
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
void DatabaseNucleotidIndex::fillSeedsOccurrences  (const ISequence*& sequence)
{
    u_int32_t     length = sequence->getLength();
    const LETTER* data   = sequence->getData();
    u_int64_t     offset = sequence->offsetInDb;

    if (length > _span)
    {
        size_t       nbMatch  = 0;
        SeedHashCode hashCode = 0;

        /** We compute the beginning hash code. */
        for (int i=_span-1; i>=0; i--)
        {
            LETTER l  = data[i];
            hashCode  = (hashCode << 2) | LETTER_NUC(l);
            nbMatch  += LETTER_ISBAD(l) ? 0 : 1;
        }

        if (nbMatch >= _span  && GETMASK(_maskIn,hashCode))
        {
            /** We add the offset in the database for the current seed. */
            SeedOccurrence& occur = _index[hashCode] [ __sync_fetch_and_add (_counter+hashCode, 1)];

            occur.offsetInDatabase = offset + 0;
            occur.sequenceIdx      = sequence->index;
        }

        /** We loop the remaining data; we update the hash code from the previous one. */
        data    += _span;
        int imax = length - _span;
        for (int i=1; i<=imax; i++, data++)
        {
            LETTER s = *data;

            /** We update the hash code from the previous one. */
            hashCode = (hashCode >> 2)  | (LETTER_NUC(s) << _bitshift);

            if (LETTER_ISBAD(s))   {  nbMatch = 0; }
            else
            {
                nbMatch++;

                if (nbMatch >= _span && GETMASK(_maskIn,hashCode))
                {
                    /** We add the offset in the database for the current seed. */
                    SeedOccurrence& occur = _index[hashCode] [ __sync_fetch_and_add (_counter+hashCode, 1)];

                    occur.offsetInDatabase = offset + i;
                    occur.sequenceIdx      = sequence->index;
                }
            }
        }
    }
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
