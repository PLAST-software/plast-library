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

#include <algo/hits/hsp/HspGeneratorCmd.hpp>
#include <os/impl/DefaultOsFactory.hpp>

#include <designpattern/api/Iterator.hpp>

#include <misc/api/macros.hpp>
#include <misc/api/types.hpp>
#include <misc/api/PlastStrings.hpp>
#include <index/impl/DatabaseNucleotidIndexOptim.hpp>

#include <string.h>

using namespace std;

using namespace misc;

using namespace os;
using namespace os::impl;

using namespace dp;
using namespace dp::impl;

using namespace misc;

using namespace database;
using namespace seed;
using namespace indexation;
using namespace indexation::impl;

using namespace algo::core;

using namespace alignment::core;

#include <stdio.h>
#define DEBUG(a)    //printf a
#define VERBOSE(a)  //printf a
#define INFO(a)     printf a


/********************************************************************************/
namespace algo {
namespace hits {
namespace hsp  {

static const u_int32_t NB_MAX_MINUS_NEIGHBORS_TAB = 256;
static const u_int8_t reverse_neighbors_tab[DatabaseNucleotidIndexOptim::NB_MAX_MMER]={42,58,10,26,46,62,14,30,34,50,2,18,38,54,6,22,43,59,11,27,47,63,15,31,35,51,3,19,39,55,7,23,40,56,8,24,44,60,12,28,32,48,0,16,36,52,4,20,41,57,9,25,45,61,13,29,33,49,1,17,37,53,5,21};

static inline int popcount_wikipedia_3(u_int64_t bitset1, u_int64_t bitset2)
{
// pop count asm
	/*	u_int64_t x=bitset1&bitset2;
	__asm__ ("popcnt %1, %0" : "=r" (x) : "0" (x));
	return x;*/

// pop count SSE4.2 if msse4.2 otpion is activated
	//	return __builtin_popcountll((bitset1&bitset2));

// provide a parallel popcount is not very efficient because need to prepare and calculate the each 64 bits
// peraphs, the best solution is to parallelize the popcount and NI, alpha calculation
// Hamming weight popcount (most efficient and portable)
	u_int64_t x;
	x = bitset1&bitset2;
	x -= (x >> 1) & 0x5555555555555555UL;             //put count of each 2 bits into those 2 bits
	x = (x & 0x3333333333333333UL) + ((x >> 2) & 0x3333333333333333UL); //put count of each 4 bits into those 4 bits
	//put count of each 8 bits into those 8 bits

	return (((x + (x >> 4)) & 0x0f0f0f0f0f0f0f0fUL)* 0x0101010101010101UL)>>56;
}

static inline u_int64_t calculate_neighbors_minus_strand(u_int64_t bitset, const u_int64_t reverse_neighbors_tab_8bits[])
{
	u_int64_t neighborM;
	neighborM = reverse_neighbors_tab_8bits[(u_int8_t)((bitset)&0x00000000000000FF)];
	neighborM = neighborM|(reverse_neighbors_tab_8bits[(u_int8_t)((bitset>>8)&0x00000000000000FF)]>>8);
	neighborM = neighborM|(reverse_neighbors_tab_8bits[(u_int8_t)((bitset>>16)&0x00000000000000FF)]<<1);
	neighborM = neighborM|(reverse_neighbors_tab_8bits[(u_int8_t)((bitset>>24)&0x00000000000000FF)]>>7);
	neighborM = neighborM|(reverse_neighbors_tab_8bits[(u_int8_t)((bitset>>32)&0x00000000000000FF)]>>2);
	neighborM = neighborM|(reverse_neighbors_tab_8bits[(u_int8_t)((bitset>>40)&0x00000000000000FF)]>>10);
	neighborM = neighborM|(reverse_neighbors_tab_8bits[(u_int8_t)((bitset>>48)&0x00000000000000FF)]>>1);
	neighborM = neighborM|(reverse_neighbors_tab_8bits[(u_int8_t)((bitset>>56)&0x00000000000000FF)]>>9);
	return neighborM;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
HSPGenerator::HSPGenerator (
    IIndexator*                 	indexator,
    statistics::IQueryInformation*  queryInfo,
    IHspContainer*              	hspContainer,
    RangeIterator<u_int32_t>&   	rangeIterator,
    int32_t threshold,
    int32_t match,
    int32_t mismatch,
    int32_t xdrop,
    int32_t alpha_threshold,
    dp::IObserver* observer
)
    : _indexator (0), _queryInfo(0), _hspContainer(0), _rangeIterator(rangeIterator),
      _threshold(threshold), _match(match), _mismatch(mismatch), _xdrop(xdrop), _alpha_threshold(alpha_threshold)
{
    DEBUG (("HSPGenerator::HSPGenerator:  threshold=%d  match=%d  mismatch=%d  xdrop=%d index_threshold=%d\n",
		threshold, match, mismatch, xdrop, alpha_threshold
	));

    setIndexator    (indexator);
    setHspContainer (hspContainer);
    setQueryInfo    (queryInfo);

    /** Shortcut. */
    _span = _indexator->getQueryIndex()->getModel()->getSpan();
    _extraSpan = _indexator->getQueryIndex()->getModel()->getExtraSpan();

    _badLetter = EncodingManager::singleton().getAlphabet(SUBSEED)->any;

    /** We register the provided observer. */
    this->addObserver (observer);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
HSPGenerator::~HSPGenerator ()
{
    setIndexator    (0);
    setHspContainer (0);
    setQueryInfo    (0);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void HSPGenerator::execute ()
{
	/** Shortcuts. */
    ISequenceDatabase* dbi1 = _indexator->getSubjectDatabase();
    ISequenceDatabase* dbi2 = _indexator->getQueryDatabase();

    IDatabaseIndex* idx1 = _indexator->getSubjectIndex();
    IDatabaseIndex* idx2 = _indexator->getQueryIndex();

    IMemoryAllocator& allocator = DefaultFactory::singleton().memory();

    Entry* seqs1 = 0;
    Entry* seqs2 = 0;

    ISeed seed;
    Range<u_int32_t> s;

    size_t nb1 = 1000;
    size_t nb2 = 1000;

    size_t maxNb1 = 0;
    size_t maxNb2 = 0;

/*    size_t maxProd    = 0;
    size_t nbIterated = 0;
    size_t nbInserted = 0;*/
    int32_t alpha_int=0.0;
    
/*    double alpha_threshold = _alpha_threshold/100.0;
	double alpha=0.0;*/
	

	u_int64_t reverse_neighbors_tab_8bits[NB_MAX_MINUS_NEIGHBORS_TAB];

    if (maxNb1 < nb1)   {  maxNb1 = nb1;  seqs1 = (Entry*) allocator.calloc (maxNb1, sizeof(Entry));  }
    if (maxNb2 < nb2)   {  maxNb2 = nb2;  seqs2 = (Entry*) allocator.calloc (maxNb2, sizeof(Entry));  }

    size_t nbRetrieved = 0;
    size_t nbTotal     = _rangeIterator.getNbItems();

    ISequenceDatabase::StrandId_e direction = idx1->getDatabase()->getDirection();
	for (u_int64_t bits_8=0;bits_8<NB_MAX_MINUS_NEIGHBORS_TAB;bits_8++)
	{
		reverse_neighbors_tab_8bits[bits_8] = 0;
		for (u_int32_t bits=0;bits<DatabaseNucleotidIndexOptim::NB_MAX_MMER;bits++)
		{
			reverse_neighbors_tab_8bits[bits_8] = reverse_neighbors_tab_8bits[bits_8]|((u_int64_t)((bits_8>>bits)&0x0000000000000001)<<reverse_neighbors_tab[bits]);
		}
	}

	DEBUG (("HSPGenerator::execute: LOOP BEGIN (this=%p) \n", this));
    while (_rangeIterator.retrieve (s, nbRetrieved) == true)
    {
        /** We send a notification about the % of execution. */
        this->notify (new IterationStatusEvent (
            ITER_ON_GOING,
            nbRetrieved,
            nbTotal,
            MSG_HITS_MSG6,
            nbRetrieved, nbTotal
        ));

        for (u_int32_t g=s.begin; g<=s.end; g++)
        {
            seed.code = g;

            /** We retrieve all occurrences of the current seed for the query database. */
            IDatabaseIndex::IndexEntry&  entries2 = idx2->getEntry (&seed);
            if (entries2.empty())  { continue; }

            /** We retrieve all occurrences of the current seed for the subject database. */
            IDatabaseIndex::IndexEntry&  entries1 = idx1->getEntry (&seed);
            if (entries1.empty())  { continue; }

            VERBOSE (("CURRENT CODE %d \n", seed.code));

            nb1 = entries1.size();
            if (maxNb1 < nb1)   {  maxNb1 = nb1;  seqs1 = (Entry*) allocator.realloc (seqs1, maxNb1*sizeof(Entry));  }

            /** IMPORTANT: the subject database has to be analyzed according to the strand direction; offsets have to
             * be modified if we are in the minus strand.  */
            if (direction == ISequenceDatabase::PLUS)
            {
                for (size_t i=0; i<nb1; i++)
                {
                    IDatabaseIndex::SeedOccurrence& occur = entries1[i];

                    const ISequence* seq1 = dbi1->getSequenceRefByIndex (occur.sequenceIdx);
                    u_int32_t        off1 = (occur.offsetInDatabase - seq1->offsetInDb);

					seqs1[i].set (
							seq1->getData() + off1,
							occur.offsetInDatabase,
							seq1->getLength() - off1,
							off1,
							seq1->index,
							occur.neighborBitsetR,
							occur.neighborBitsetL,
							occur.nbNeighbors,
							0
					);
                }
            }
            else
            {
                for (size_t i=0; i<nb1; i++)
                {
                    IDatabaseIndex::SeedOccurrence& occur = entries1[i];

                    const ISequence* seq1 = dbi1->getSequenceRefByIndex (occur.sequenceIdx);
                    u_int32_t        off1 = (occur.offsetInDatabase - seq1->offsetInDb);
                    off1 = seq1->getLength() - (off1 + _span);
                    u_int64_t		 neighborR = calculate_neighbors_minus_strand(occur.neighborBitsetR,reverse_neighbors_tab_8bits);
                    u_int64_t		 neighborL = calculate_neighbors_minus_strand(occur.neighborBitsetL,reverse_neighbors_tab_8bits);

					seqs1[i].set (
							seq1->getData()   + off1,
							seq1->offsetInDb  + off1,
							seq1->getLength() - off1,
							off1,
							seq1->index,
							neighborL,
							neighborR,
							occur.nbNeighbors,
							0
					);
                }
            }

            nb2 = entries2.size();
            if (maxNb2 < nb2)   {  maxNb2 = nb2;  seqs2 = (Entry*) allocator.realloc (seqs2, maxNb2*sizeof(Entry));  }

            for (size_t i=0; i<nb2; i++)
            {
                IDatabaseIndex::SeedOccurrence& occur = entries2[i];

                const ISequence* seq2 = dbi2->getSequenceRefByIndex (occur.sequenceIdx);
                u_int32_t        off2 = (occur.offsetInDatabase - seq2->offsetInDb);

                statistics::IQueryInformation::SequenceInfo& info = _queryInfo->getSeqInfo (*seq2);
				int32_t  threshold = MAX(info.cut_offs,27);
				threshold = MIN(threshold,_threshold);

				seqs2[i].set (
					seq2->getData() + off2,
					occur.offsetInDatabase,
					seq2->getLength() - off2,
					off2,
					seq2->index,
					occur.neighborBitsetR,
					occur.neighborBitsetL,
					occur.nbNeighbors,
					threshold
				);
            }

/*            maxProd = MAX (maxProd, nb1*nb2);
            nbIterated += nb1*nb2;*/

            for (size_t i=0; i<nb1; i++)
            {
                Entry& e1 = seqs1[i];

                const LETTER*    s1        				= e1.data;
                u_int32_t        rightLen1 				= e1.rightLen;
                u_int32_t        leftLen1  				= e1.leftLen;
                u_int64_t 		 bitset_Mmer_R_subject 	= e1.neighborBitsetR;
                u_int64_t 		 bitset_Mmer_L_subject 	= e1.neighborBitsetL;
                int32_t 		 N_S 					= e1.nbNeighbors;


                for (size_t j=0; j<nb2; j++)
                {
                    Entry& e2 = seqs2[j];

                    const LETTER*    s2        				= e2.data;
                    u_int32_t        rightLen2 				= e2.rightLen;
                    u_int32_t        leftLen2  				= e2.leftLen;
                    u_int64_t 		 bitset_Mmer_R_query 	= e2.neighborBitsetR;
                    u_int64_t 		 bitset_Mmer_L_query 	= e2.neighborBitsetL;
                    int32_t 		 N_Q 					= e2.nbNeighbors;
                    int32_t          threshold 				= e2.threshold;


                    int32_t N_I = popcount_wikipedia_3(bitset_Mmer_R_subject,bitset_Mmer_R_query)+popcount_wikipedia_3(bitset_Mmer_L_subject,bitset_Mmer_L_query);
                    //int32_t N_I = __builtin_popcountll((bitset_Mmer_R_subject&bitset_Mmer_R_query)) +__builtin_popcountll((bitset_Mmer_L_subject&bitset_Mmer_L_query));
					double div=(N_Q+N_S-2*N_I);
					if (div!=0)
						alpha_int = (int32_t)(((double)N_I/div)*100.0);
					else
						alpha_int = 100;

/*					if (div!=0)
						alpha = ((double)N_I/div);
					else
						alpha = 1.1;

					if (alpha>alpha_threshold)*/
 					if (alpha_int>_alpha_threshold)
                	{

						bool alreadySeenRight = false;
						bool alreadySeenLeft  = false;

						size_t rightLen = MIN (rightLen1, rightLen2);
						size_t leftLen  = MIN (leftLen1,  leftLen2);

						int rightScore = computeExtensionRight (seed.code, s1, s2, rightLen, _span, alreadySeenRight);
						if (alreadySeenRight)  {  continue;  }

						int leftScore  = computeExtensionLeft  (seed.code, s1, s2, leftLen,  _span, alreadySeenLeft);
						if (alreadySeenLeft)   {  continue;  }

						int score = rightScore + leftScore - _span * _match;

						if (score > threshold)
						{
							/** We build the ranges to be used for adding a diagonal HSP. Note that we have to add back
							 *  the offset of the database. */
							misc::Range64 qryRange (e2.offsetInDb-leftLen, e2.offsetInDb+rightLen-1);
							misc::Range64 sbjRange (e1.offsetInDb-leftLen, e1.offsetInDb+rightLen-1);

							/** We add the alignment. */
							_hspContainer->insert (qryRange, sbjRange, e2.seqId, e1.seqId, score);

							//nbInserted ++;

	#if 0
							dump (seed.code, e1.seqId, e2.seqId, leftLen1, leftLen2);
	#endif
						}
                	}
                 }  /* end of for (size_t j=0... */
            } /* end of for (size_t i=0... */
        } /* end of for (int g=s.begin... */

    } /** end of while (getNextSeedCodes (s)... */
    DEBUG (("HSPGenerator::execute: LOOP END (this=%p)  maxNb1=%ld  maxNb2=%ld  nbIterated=%ld  maxProd=%ld  nbInserted=%ld\n",
		this, maxNb1, maxNb2, nbIterated, maxProd, nbInserted
	));


    allocator.free (seqs1);
    allocator.free (seqs2);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
int HSPGenerator::computeExtensionRight (int code, const LETTER* s1, const LETTER* s2, size_t& length, size_t span, bool& alreadySeen)
{
    int code_ss1 = code;

    alreadySeen = false;

    /** We reset the score. */
    size_t  nbMatch = span;
    int32_t score   = nbMatch * _match;
    int32_t maxi    = score;

    const LETTER* ss1 = s1 + span;
    const LETTER* ss2 = s2 + span;

    LETTER ss1Letter = 0;

    int32_t bitshift = 2*(span-1);
    int32_t bitmask  = (1 << (2*span)) - 1;

    u_int32_t kmaxi=span;
    u_int32_t k=0;
    for (k=span;  ((maxi-score) <= _xdrop) && k<length;  k++, ss1++, ss2++)
    {
        /** Shortcut. */
        ss1Letter = *ss1;

        /** We update the seed code for the current letter.
         *  Note that we use a bitmask: this will assure that we don't corrupt code update
         *  even if the ss1Letter is not valid (like 'N' letter whose value should be 4).
         */
        code_ss1 = ((code_ss1 >> 2)  +  (ss1Letter << bitshift)) & bitmask;

        if (ss1Letter == *ss2  &&  ss1Letter < _badLetter)
        {
            score += _match;
            nbMatch++;

            if (score > maxi)
            {
                maxi  = score;
                kmaxi = k;
            }

            if ((_extraSpan==0)&&(nbMatch >= span))
            {
                if (code_ss1 < code)
                {
                    score = -1;
                    alreadySeen = true;
                    break;
                }

                /** In case we find a common seed, we reset the max score which would allow to escape from some local minima. */
                // else if (code_ss1 != code)  {  maxi = score; }
            }
        }
        else
        {
            score += _mismatch;
            nbMatch = 0;
        }

    }  /* end of for (size_t k... */

    length = kmaxi + 1;

    return maxi;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
int HSPGenerator::computeExtensionLeft (int code, const LETTER* s1, const LETTER* s2, size_t& length, size_t span, bool& alreadySeen)
{
    int code_ss1 = code;

    alreadySeen = false;

    /** We reset the score. */
    size_t  nbMatch = span;
    int32_t score   = nbMatch * _match;
    int32_t maxi    = score;

    int32_t bitshift = (1 << (2*span)) - 1;

    const LETTER* ss1 = s1 - 1;
    const LETTER* ss2 = s2 - 1;

    LETTER ss1Letter = 0;

    u_int32_t kmaxi=0;
    u_int32_t k=0;
    for (k=0;  (maxi-score) <= _xdrop  &&  k<length;  k++, ss1--, ss2--)
    {
        /** Shortcut. */
        ss1Letter = *ss1;

        /** We update the seed code for the current letter.
         *  Note that we use (ss1Letter & 3): this will assure that we don't corrupt code update
         *  even if the ss1Letter is not valid (like 'N' letter whose value should be 4).
         */
        code_ss1 = ( (code_ss1 << 2) & bitshift) + (ss1Letter & 3);

        if (ss1Letter == *ss2  &&  ss1Letter < _badLetter)
        {
            score += _match;
            nbMatch++;

            if (score > maxi)
            {
                maxi  = score;
                kmaxi = k;
            }

            if ((_extraSpan==0)&&(nbMatch >= span))
            {
                if (code_ss1 < code)
                {
                    score = -1;
                    alreadySeen = true;
                    break;
                }

                /** In case we find a common seed, we reset the max score which would allow to escape from some local minima. */
                // else if (code_ss1 != code)  {  maxi = score; }
            }
        }
        else
        {
            score += _mismatch;
            nbMatch = 0;
        }

    }  /* end of for (size_t k... */

    length = kmaxi + 1;

    return maxi;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void HSPGenerator::dump (SeedHashCode code, u_int32_t qryIdx, u_int32_t sbjIdx, u_int32_t off1, u_int32_t off2)
{
#if 0
    ISequenceDatabase* dbi1 = _indexator->getSubjectDatabase();
    ISequenceDatabase* dbi2 = _indexator->getQueryDatabase();

    const ISequence* seq1 = dbi1->getSequenceRefByIndex (sbjIdx);
    const ISequence* seq2 = dbi2->getSequenceRefByIndex (qryIdx);

    INFO (("\n"));
    INFO (("--------------------------------------------  code=%d '", code));
    for (size_t i=off1; i<off1+_span; i++)  {  INFO (("%c", CONVERT(seq1->getData()[i]) )); }

    INFO (("' ------------------------------------------------------------\n"));

    INFO (("SUBJECT (offset=%d  id=%d)   QUERY (offset=%d  id=%d)    diag=%d\n", off1, seq1->index, off2, seq2->index, off1-off2));

    size_t ll = MAX (seq1->getLength(), seq2->getLength());
    for (size_t i=0; i<ll; i++)  { INFO (("%1ld", i/100)); }        INFO (("\n"));
    for (size_t i=0; i<ll; i++)  { INFO (("%1ld", (i%100)/10)); }   INFO (("\n"));
    for (size_t i=0; i<ll; i++)  { INFO (("%1ld", i%10)); }         INFO (("\n"));

    for (size_t i=0; i<seq2->getLength(); i++)  { INFO (("%c", CONVERT(seq2->getData()[i]))); }     INFO (("\n"));
    for (size_t i=0; i<off2; i++)  {  INFO ((" ")); }
    for (size_t i=0; i<_span; i++)  { INFO (("X")); }   INFO (("\n"));

    for (size_t i=0; i<seq1->getLength(); i++)  { INFO (("%c", CONVERT(seq1->getData()[i]))); }     INFO (("\n"));
    for (size_t i=0; i<off1; i++)  {  INFO ((" ")); }
    for (size_t i=0; i<_span; i++)  { INFO (("X")); }   INFO (("\n"));
#endif
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
char HSPGenerator::CONVERT (LETTER i)
{
    switch (i)
    {
        case 0:  return 'A';
        case 1:  return 'C';
        case 2:  return 'G';
        case 3:  return 'T';
        default: return '?';
    }
}

/********************************************************************************/
}}}; /* end of namespaces. */
/********************************************************************************/
