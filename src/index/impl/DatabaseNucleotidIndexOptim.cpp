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

#include <index/impl/DatabaseNucleotidIndexOptim.hpp>
#include <algo/core/api/IAlgoEvents.hpp>

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

using namespace algo::core;


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


/********************************************************************************/
inline static SeedHashCode revcomp (SeedHashCode x, size_t sizeKmer)
{
	u_int64_t res = x;

	res = ((res>> 2 & 0x3333333333333333) | (res & 0x3333333333333333) <<  2);
	res = ((res>> 4 & 0x0F0F0F0F0F0F0F0F) | (res & 0x0F0F0F0F0F0F0F0F) <<  4);
	res = ((res>> 8 & 0x00FF00FF00FF00FF) | (res & 0x00FF00FF00FF00FF) <<  8);
	res = ((res>>16 & 0x0000FFFF0000FFFF) | (res & 0x0000FFFF0000FFFF) << 16);
	res = ((res>>32 & 0x00000000FFFFFFFF) | (res & 0x00000000FFFFFFFF) << 32);
	res = res ^ 0xAAAAAAAAAAAAAAAA;

	return (res >> (2*(32-sizeKmer))) ;
/*    SeedHashCode res = 0;
    for (size_t i=0; i<sizeKmer; i++)
    {
        res = (res<<2) +  (((x & 3)+2) & 3);
        x >>= 2;
    }
    return res;*/
}

/********************************************************************************/

enum { WORD_SIZE = sizeof(DatabaseNucleotidIndexOptim::word_t) * 8 };

inline int bindex(int b)  { return b / WORD_SIZE; }
inline int boffset(int b) { return b % WORD_SIZE; }

#define GETMASK(data,b)  (data[bindex(b)] & (1LL << (boffset(b))))
#define SETMASK(data,b)  data[bindex(b)] |= 1LL << (boffset(b))

/*********************************************************************************/

template <typename Functor>
class SequenceSeedsCmd : public dp::impl::IteratorCommand<const database::ISequence*>
{
private:
    DatabaseNucleotidIndexOptim* _ref;
    Functor& _action;
    bool _calculateMmer;

public:
    SequenceSeedsCmd (
        dp::impl::IteratorGet<const database::ISequence*>* it,
        DatabaseNucleotidIndexOptim* ref,
        Functor& action,
        bool calculateMmer
    ) : dp::impl::IteratorCommand<const database::ISequence*> (it), _ref(ref), _action(action), _calculateMmer(calculateMmer)  {}

    void execute (const database::ISequence*& sequence, size_t& nbGot)
    {
        if (sequence->getLength() > _ref->_span)
        {
            LETTER       l, l_R_first, l_R_next, l_L_first, l_L_next;
            size_t       nbMatch  = 0;
            SeedHashCode hashCode = 0;
            size_t       moduloSpan = 0;
            size_t       span     = _ref->_span;
            int32_t      bitshift = _ref->_bitshift;
            const u_int32_t    max_Mmer_Windows_size = _ref->_max_Mmer_Windows_size;
            u_int64_t    neighborBitsetR = 0;
            u_int64_t    neighborBitsetL = 0;
            u_int8_t     nbNeighbors = 0;
            const LETTER* data    = sequence->getData();
            DatabaseNucleotidIndexOptim::word_t*  maskIn   = _ref->_maskIn;

            u_int32_t code_Mmer_R = 0;
            u_int32_t code_Mmer_R_first = 0;
            u_int8_t count_code_Mmer_R[DatabaseNucleotidIndexOptim::NB_MAX_MMER];

            u_int32_t code_Mmer_L = 0;
            u_int32_t code_Mmer_L_first = 0;
            u_int8_t count_code_Mmer_L[DatabaseNucleotidIndexOptim::NB_MAX_MMER];

            int32_t bitshiftKmerMmer = _ref->_bitshiftKmerMmer;
            int32_t bitshiftMmer = _ref->_bitshiftMmer;

            /** We compute the beginning hash code. */
            int  imax = sequence->getLength() - span;
            moduloSpan = ((_ref->_extraSpan>_ref->_span)&&(_ref->_otherIndex!=0))?(_ref->_extraSpan - _ref->_span)+1:1;
            for (int i=span-1; i>=0; i--)
            {
                l = data[i];
                hashCode  = (hashCode << 2) | LETTER_NUC(l);
                nbMatch   = LETTER_ISBAD(l) ? 0 : nbMatch + 1;
            }


			if (_calculateMmer)
            {
	            memset(count_code_Mmer_R,0,DatabaseNucleotidIndexOptim::NB_MAX_MMER*sizeof(u_int8_t));
	            memset(count_code_Mmer_L,0,DatabaseNucleotidIndexOptim::NB_MAX_MMER*sizeof(u_int8_t));

	            for (int k=0; k<DatabaseNucleotidIndexOptim::INDEX_MMER_SIZE; k++)
				{
					l = data[k];
					code_Mmer_L = ( (code_Mmer_L >> 2)|(LETTER_NUC(l) << bitshiftMmer));
				}

				/** We loop the remaining data; we update the hash code from the previous one. */
				data += span;

				/** We loop on the data to calculate the Right Mmer code, we add these Mmer code in the FIFO **/
				code_Mmer_R = hashCode>>bitshiftKmerMmer;
				neighborBitsetR = 0;
				code_Mmer_R_first = (code_Mmer_R >> 2)|(LETTER_NUC(data[0]) << bitshiftMmer);
				for (int k=0; k<MIN(max_Mmer_Windows_size,imax); k++)
				{
					l = data[k];
					code_Mmer_R = (code_Mmer_R >> 2)|(LETTER_NUC(l) << bitshiftMmer);
					if (count_code_Mmer_R[code_Mmer_R]==0) nbNeighbors++;
					count_code_Mmer_R[code_Mmer_R]++;
					neighborBitsetR=neighborBitsetR|(((u_int64_t)1)<<code_Mmer_R);
				}
            }
			else
			{
				data += span;
			}

            /** We may to do some action if the current match is ok. */
            if (nbMatch >= span  && GETMASK(maskIn,hashCode))  {  _action (sequence, hashCode, 0,neighborBitsetR,0,nbNeighbors);  }

            for (int i=1; i<=imax; i++)
            {
                /** We retrieve the next letter. */
                l = *(data++);

                /** We update the hash code from the previous one. */
                hashCode = (hashCode >> 2)  | (LETTER_NUC(l) << bitshift);
                if (_calculateMmer)
                {
					/** set the Left Mmer **/
                	if (i>1)
                	{
						/** calculate the first Mmer for the left size **/
						if (i>max_Mmer_Windows_size)
						{
							l_L_first = *(data-span+2-max_Mmer_Windows_size);
							if (count_code_Mmer_L[code_Mmer_L_first]>0) count_code_Mmer_L[code_Mmer_L_first]--;
							if (count_code_Mmer_L[code_Mmer_L_first]==0) {neighborBitsetL=neighborBitsetL&(~(((u_int64_t)1)<<code_Mmer_L_first)); nbNeighbors--;}

							code_Mmer_L_first = (code_Mmer_L_first >> 2)|(LETTER_NUC(l_L_first) << bitshiftMmer);
						}
						l_L_next = *(data-span+1);
						code_Mmer_L = (code_Mmer_L >> 2)|(LETTER_NUC(l_L_next) << bitshiftMmer);
                	}
                	else
						code_Mmer_L_first = code_Mmer_L;
					if (count_code_Mmer_L[code_Mmer_L]==0) nbNeighbors++;
					count_code_Mmer_L[code_Mmer_L]++;
               		neighborBitsetL=neighborBitsetL|(((u_int64_t)1)<<code_Mmer_L);

					/** set the Right Mmer **/
                    l_R_first = *(data);
                	if (count_code_Mmer_R[code_Mmer_R_first]>0) count_code_Mmer_R[code_Mmer_R_first]--;
                	if (count_code_Mmer_R[code_Mmer_R_first]==0) { neighborBitsetR=neighborBitsetR&(~(((u_int64_t)1)<<code_Mmer_R_first)); nbNeighbors--;}
                	code_Mmer_R_first = ((code_Mmer_R_first >> 2)|(LETTER_NUC(l_R_first) << bitshiftMmer));
	                if ((i+max_Mmer_Windows_size)<=imax)
	                {
	                	l_R_next = *(data+max_Mmer_Windows_size-1);
						code_Mmer_R = (code_Mmer_R >> 2)|(LETTER_NUC(l_R_next) << bitshiftMmer);
		                if (count_code_Mmer_R[code_Mmer_R]==0) nbNeighbors++;
		                count_code_Mmer_R[code_Mmer_R]++;
                		neighborBitsetR=neighborBitsetR|(((u_int64_t)1)<<code_Mmer_R);
	                }
                }

                /** We update the number of matched letters that make a seed. */
                nbMatch   = LETTER_ISBAD(l) ? 0 : nbMatch + 1;

                /** We may to do some action if the current match is ok. */
                if (nbMatch >= span && GETMASK(maskIn,hashCode) && ((i%moduloSpan)==0))  {  _action (sequence, hashCode, i,neighborBitsetR,neighborBitsetL, nbNeighbors);  }
            }
        }
    }
};

/*********************************************************************************/

struct SeedsFunctor
{
    DatabaseNucleotidIndexOptim* _ref;

    SeedsFunctor (DatabaseNucleotidIndexOptim* ref) : _ref(ref) {}
};

/*********************************************************************************/

struct CountFunctor : public SeedsFunctor
{
    CountFunctor (DatabaseNucleotidIndexOptim* ref) : SeedsFunctor (ref) {}

    void operator() (const database::ISequence* sequence, seed::SeedHashCode hashCode, size_t idx, u_int64_t neighborBitsetR, u_int64_t neighborBitsetL, u_int8_t nbNeighbor)
    {
        __sync_fetch_and_add (_ref->_counter + hashCode, 1);
    }
};

/*********************************************************************************/

struct FillFunctor : public SeedsFunctor
{
    FillFunctor (DatabaseNucleotidIndexOptim* ref) : SeedsFunctor (ref) {}

    void operator() (const database::ISequence* sequence, seed::SeedHashCode hashCode, size_t idx, u_int64_t neighborBitsetR, u_int64_t neighborBitsetL, u_int8_t nbNeighbor)
    {
        /** We add the offset in the database for the current seed. */
    	IDatabaseIndex::SeedOccurrence& occur = _ref->_index[hashCode] [ __sync_fetch_and_add (_ref->_counter+hashCode, 1)];

        occur.offsetInDatabase = sequence->offsetInDb + idx;
        occur.sequenceIdx      = sequence->index;
        occur.neighborBitsetR = neighborBitsetR;
        occur.neighborBitsetL = neighborBitsetL;
        occur.nbNeighbors	  = nbNeighbor;
    }
};

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
DatabaseNucleotidIndexOptim::DatabaseNucleotidIndexOptim (ISequenceDatabase* database, ISeedModel* model, IDatabaseIndex* otherIndex, dp::ICommandDispatcher* dispatcher)
    : AbstractDatabaseIndex (database, model), _counter(0), _span(0), _extraSpan(0), _bitshift(0), _maskIn(0),  _maskOut(0),
      _dispatcher(0), _occurrences(0),  _occurrencesSize(0), _index(0), _otherIndex(otherIndex), _isBuilt(false)
{
	/** Shortcuts. */
    _span     = getModel()->getSpan();
    _bitshift = 2*(_span-1);
    _bitshiftKmerMmer = 2*(_span-INDEX_MMER_SIZE);
    _bitshiftMmer = 2*(INDEX_MMER_SIZE-1);
    _bitmaskMmer  = (1 << (2*INDEX_MMER_SIZE)) - 1;

    _extraSpan     = getModel()->getExtraSpan();

    size_t alphabetSize = 4; //getModel()->getAlphabet()->size;

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
    _index = new IndexEntry[_maxSeedsNumber];

    size_t maskSize = _maxSeedsNumber / WORD_SIZE;

    _maskOut = new word_t [maskSize];  memset (_maskOut, 0, sizeof(word_t)*maskSize);
    _maskIn  = new word_t [maskSize];

    _max_Mmer_Windows_size = 10;

    if (otherIndex != 0 && otherIndex->getMask())  {  memcpy (_maskIn, otherIndex->getMask(),  sizeof(word_t)*maskSize);  }
    else                                           {  memset (_maskIn,  ~0,                    sizeof(word_t)*maskSize);  }

    /** We allocate the required number of seeds occurrences. */
    setDatabase (database);

    /** */
    //setDispatcher (new ParallelCommandDispatcher(dispatcher->getExecutionUnitsNumber()));
    setDispatcher (dispatcher);

    DEBUG (("DatabaseNucleotidIndexOptim::DatabaseNucleotidIndexOptim: _span=%d  _maxSeedsNumber=%ld  _alphabetSize=%ld\n",
        _span, _maxSeedsNumber, alphabetSize
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
DatabaseNucleotidIndexOptim::~DatabaseNucleotidIndexOptim ()
{
    /** We release resources. */
    DefaultFactory::memory().free (_counter);

    setDispatcher (0);

    delete[] _maskIn;
    delete[] _maskOut;

    delete[] _index;
    delete[] _occurrences;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void DatabaseNucleotidIndexOptim::setDatabase (database::ISequenceDatabase* database)
{
    if (database &&  _occurrencesSize < database->getSize())
    {
        if (_occurrences) {  delete[] _occurrences;  }

        DEBUG (("DatabaseNucleotidIndexOptim::setDatabase  this=%p  oldSize=%ld  newSize=%ld  '%s'\n",
            this, _occurrencesSize, database->getSize(), database->getId().c_str()
        ));

        /** We allocate a little bit more in order to avoid some further resizing with new databases
         *  that are only a little bit bigger than the previous one. */
        _occurrencesSize = (database->getSize()*10)/9;
        _occurrences = new SeedOccurrence [_occurrencesSize];
    }

    /** We set the new database. */
    SP_SETATTR(database);

    /** We reset the built flag. */
    _isBuilt = false;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void DatabaseNucleotidIndexOptim::build ()
{
    /** We may have already built the index. */
    if (_isBuilt == true)  { return; }

    /** Some time statistics. */
    TimeInfo timeInfo (DefaultFactory::time());

    DEBUG (("DatabaseNucleotidIndexOptim::build : START !  db='%s' \n", _database->getId().c_str() ));

    size_t nbcpu = _dispatcher->getExecutionUnitsNumber();

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

    /** We reset the distribution vector. */
    memset (_counter, 0, _maxSeedsNumber*sizeof(u_int32_t));

    /** We need a "get" iterator on the sequence iterator for parallel iteration. */
    IteratorGet<const database::ISequence*>* seqIterGetCount = new IteratorGet<const ISequence*> (seqIter);
    LOCAL (seqIterGetCount);

    /** We build a list of commands that will iterate our list, through the created iterator. */
    list<ICommand*> commands;
    CountFunctor count (this);
    for (size_t i=1; i <= nbcpu; i++)  {  commands.push_back (new SequenceSeedsCmd<CountFunctor> (seqIterGetCount, this, count, false));  }
    _dispatcher->dispatchCommands (commands,0);

    timeInfo.stopEntry ("1");

	DEBUG (("DatabaseNucleotidIndexOptim::build : COUNTING IS DONE...\n"));

    /************************************************************/
    /*********************** PHASE 2 ****************************/
    /************************************************************/

    timeInfo.addEntry ("2");

    u_int64_t nbOccurrences = 0;

    size_t nbSeedsFound = 0;

	/** Now, we can allocate structures since we know the number of occurrences for each seed. */
    for (size_t currentCode=0; currentCode<_maxSeedsNumber; currentCode++)
    {
        size_t len = _counter[currentCode];

        if (len > 0)
        {
            IndexEntry& entry = _index [currentCode];

            entry._occurs = _occurrences + nbOccurrences;
            entry._size   = len;

            _counter[currentCode] = 0;

            /** We setup the mask */
            SETMASK (_maskOut, currentCode);
            SETMASK (_maskOut, revcomp(currentCode, _span));

            nbSeedsFound++;

            nbOccurrences += len;
        }
    }

    DEBUG (("DatabaseNucleotidIndexOptim::build  nbOccurrences=%d  nbSeedsFound=%d  ratio=%.3f\n", nbOccurrences, nbSeedsFound, 100.0*(float)nbSeedsFound / (float)_maxSeedsNumber));

    timeInfo.stopEntry ("2");

    /************************************************************/
    /*********************** PHASE 3 ****************************/
    /************************************************************/

    timeInfo.addEntry ("3");

    /** We need a "get" iterator on the sequence iterator for parallel iteration. */
    IteratorGet<const database::ISequence*>* seqIterGetFill = new IteratorGet<const ISequence*> (seqIter);
    LOCAL (seqIterGetFill);

    commands.clear();
    FillFunctor fill (this);
    for (size_t i=1; i <= nbcpu; i++)  {  commands.push_back (new SequenceSeedsCmd<FillFunctor> (seqIterGetFill, this, fill, true));  }
    _dispatcher->dispatchCommands (commands,0);

    timeInfo.stopEntry ("3");

    timeInfo.stopEntry ("b");

    DEBUG (("DatabaseNucleotidIndexOptim::build : END SEQUENCES LOOP\n"));

    DEBUG (("DatabaseNucleotidIndexOptim::build: count in %d msec (%.1f), resize in %d msec (%.1f), fill in %d msec (%.1f)=> total %d msec\n",
        timeInfo.getEntryByKey("1"),  100.0 * (double) timeInfo.getEntryByKey("1") / (double)timeInfo.getEntryByKey("b"),
        timeInfo.getEntryByKey("2"),  100.0 * (double) timeInfo.getEntryByKey("2") / (double)timeInfo.getEntryByKey("b"),
        timeInfo.getEntryByKey("3"),  100.0 * (double) timeInfo.getEntryByKey("3") / (double)timeInfo.getEntryByKey("b"),
        timeInfo.getEntryByKey("b")
    ));

    DEBUG (("DatabaseNucleotidIndexOptim::build : %ld sequences  %ld occurrences  dbSize=%ld => %ld letter(s) not used,  ratio=%.1f\n",
        getDatabase()->getSequencesNumber(), nbOccurrences, getDatabase()->getSize(),
        getDatabase()->getSize() - (_span-1)*getDatabase()->getSequencesNumber() - nbOccurrences,
        100.0 - 100.0 *  (float) (getDatabase()->getSize() - (_span-1)*getDatabase()->getSequencesNumber() - nbOccurrences) / (float) getDatabase()->getSize()
    ));

    /** We change the inner built state. */
    _isBuilt = true;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IDatabaseIndex::IndexEntry& DatabaseNucleotidIndexOptim::getEntry (const seed::ISeed* seed)
{
    static IndexEntry instance;

    SeedHashCode code = (_database->getDirection() == ISequenceDatabase::PLUS ? seed->code : revcomp (seed->code, _span));

    if (code >= _maxSeedsNumber)   {  throw MSG_INDEXATION_MSG1; }

	return _counter[code] ? _index[code] : instance;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
size_t DatabaseNucleotidIndexOptim::getOccurrenceNumber (const seed::ISeed* seed)
{
    SeedHashCode code = seed->code;

    if (code >= _maxSeedsNumber)   {  throw MSG_INDEXATION_MSG1; }

    return _counter[code];
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
u_int64_t DatabaseNucleotidIndexOptim::getTotalOccurrenceNumber ()
{
    u_int64_t result = 0;

    for (size_t i=0; i<_maxSeedsNumber; i++)   {  result += _counter[i];  }

    return result;
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
