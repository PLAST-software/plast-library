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

using namespace algo::core;

using namespace alignment::core;

#include <stdio.h>
#define DEBUG(a)    //printf a
#define VERBOSE(a)  //printf a

/********************************************************************************/
namespace algo {
namespace hits {
namespace hsp  {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
HSPGenerator::HSPGenerator (
    IIndexator*                 indexator,
    IHspContainer*              hspContainer,
    RangeIterator<u_int32_t>&   rangeIterator,
    int32_t threshold,
    int32_t match,
    int32_t mismatch,
    int32_t xdrop,
    dp::IObserver* observer
)
    : _indexator (0), _hspContainer(0), _rangeIterator(rangeIterator),
      _threshold(threshold), _match(match), _mismatch(mismatch), _xdrop(xdrop)
{
    DEBUG (("HSPGenerator::HSPGenerator:  threshold=%d  match=%d  mismatch=%d  xdrop=%d\n",
		threshold, match, mismatch, xdrop
	));

    setIndexator    (indexator);
    setHspContainer (hspContainer);

    /** Shortcut. */
    _span = _indexator->getQueryIndex()->getModel()->getSpan();

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

    size_t maxProd    = 0;
    size_t nbIterated = 0;
    size_t nbInserted = 0;

    if (maxNb1 < nb1)   {  maxNb1 = nb1;  seqs1 = (Entry*) allocator.calloc (maxNb1, sizeof(Entry));  }
    if (maxNb2 < nb2)   {  maxNb2 = nb2;  seqs2 = (Entry*) allocator.calloc (maxNb2, sizeof(Entry));  }

    size_t nbRetrieved = 0;
    size_t nbTotal     = _rangeIterator.getNbItems();

    DEBUG (("HSPGenerator::execute: LOOP BEGIN (this=%p) \n", this));

    while (_rangeIterator.retrieve (s, nbRetrieved) == true)
    {
        /** We send a notification about the % of execution. */
        this->notify (new IterationStatusEvent (
            ITER_ON_GOING,
            nbRetrieved,
            nbTotal,
            "iterating seeds codes",
            nbRetrieved, nbTotal
        ));

        for (u_int32_t g=s.begin; g<=s.end; g++)
        {
            seed.code = g;

            /** We retrieve all occurrences of the current seed for the subject database. */
            IDatabaseIndex::IndexEntry&  entries1 = idx1->getEntry (&seed);

            /** We retrieve all occurrences of the current seed for the query database. */
            IDatabaseIndex::IndexEntry&  entries2 = idx2->getEntry (&seed);

            /** We can skip seeds having no common occurrence in both databases. */
            if (entries1.empty() || entries2.empty())  { continue; }

            VERBOSE (("CURRENT CODE %d \n", seed.code));

            nb1 = entries1.size();
            if (maxNb1 < nb1)   {  maxNb1 = nb1;  seqs1 = (Entry*) allocator.realloc (seqs1, maxNb1*sizeof(Entry));  }

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
                    seq1->index
                );
            }

            nb2 = entries2.size();
            if (maxNb2 < nb2)   {  maxNb2 = nb2;  seqs2 = (Entry*) allocator.realloc (seqs2, maxNb2*sizeof(Entry));  }

            for (size_t i=0; i<nb2; i++)
            {
                IDatabaseIndex::SeedOccurrence& occur = entries2[i];

                const ISequence* seq2 = dbi2->getSequenceRefByIndex (occur.sequenceIdx);
                u_int32_t        off2 = (occur.offsetInDatabase - seq2->offsetInDb);

                seqs2[i].set (
                    seq2->getData() + off2,
                    occur.offsetInDatabase,
                    seq2->getLength() - off2,
                    off2,
                    seq2->index
                );
            }

            maxProd = MAX (maxProd, nb1*nb2);
            nbIterated += nb1*nb2;

            for (size_t i=0; i<nb1; i++)
            {
                Entry& e1 = seqs1[i];

                const LETTER*    s1        = e1.data;
                u_int32_t        rightLen1 = e1.rightLen;
                u_int32_t        leftLen1  = e1.leftLen;

                for (size_t j=0; j<nb2; j++)
                {
                    Entry& e2 = seqs2[j];

                    const LETTER*    s2        = e2.data;
                    u_int32_t        rightLen2 = e2.rightLen;
                    u_int32_t        leftLen2  = e2.leftLen;

                    bool alreadySeenRight = false;
                    bool alreadySeenLeft  = false;

                    size_t rightLen = MIN (rightLen1, rightLen2);
                    size_t leftLen  = MIN (leftLen1,  leftLen2);

                    int rightScore = computeExtensionRight (seed.code, s1, s2, rightLen, _span, alreadySeenRight);
                    if (alreadySeenRight)  {  continue;  }

                    int leftScore  = computeExtensionLeft  (seed.code, s1, s2, leftLen,  _span, alreadySeenLeft);
                    if (alreadySeenLeft)   {  continue;  }

                    int score = rightScore + leftScore - _span * _match;

                    if (score > _threshold)
                    {
                        /** We build the ranges to be used for adding a diagonal HSP. Note that we have to add back
                         *  the offset of the database. */
                        misc::Range64 qryRange (e2.offsetInDb-leftLen, e2.offsetInDb+rightLen-1);
                        misc::Range64 sbjRange (e1.offsetInDb-leftLen, e1.offsetInDb+rightLen-1);

                        /** We add the alignment. */
                        _hspContainer->insert (qryRange, sbjRange, e2.seqId, e1.seqId, score);

                        nbInserted ++;

#if 0
                        dump (seed.code, e1.seqId, e2.seqId, leftLen1, leftLen2);
#endif
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

    u_int32_t kmaxi=0;
    u_int32_t k=0;
    for (k=span;  ((maxi-score) < _xdrop) && k<length;  k++, ss1++, ss2++)
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

            if (nbMatch >= span)
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
    for (k=0;  (maxi-score) < _xdrop  &&  k<length;  k++, ss1--, ss2--)
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

            if (nbMatch >= span)
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
    ISequenceDatabase* dbi1 = _indexator->getSubjectDatabase();
    ISequenceDatabase* dbi2 = _indexator->getQueryDatabase();

    const ISequence* seq1 = dbi1->getSequenceRefByIndex (sbjIdx);
    const ISequence* seq2 = dbi2->getSequenceRefByIndex (qryIdx);

    printf ("\n");
    printf ("--------------------------------------------  code=%d '", code);
    for (size_t i=off1; i<off1+_span; i++)  {  printf ("%c", CONVERT(seq1->getData()[i]) ); }

    printf ("' ------------------------------------------------------------\n");

    printf ("SUBJECT (offset=%d  id=%d)   QUERY (offset=%d  id=%d)    diag=%d\n", off1, seq1->index, off2, seq2->index, off1-off2);

    size_t ll = MAX (seq1->getLength(), seq2->getLength());
    for (size_t i=0; i<ll; i++)  { printf ("%1ld", i/100); } printf ("\n");
    for (size_t i=0; i<ll; i++)  { printf ("%1ld", (i%100)/10); } printf ("\n");
    for (size_t i=0; i<ll; i++)  { printf ("%1ld", i%10); } printf ("\n");

    for (size_t i=0; i<seq2->getLength(); i++)  { printf ("%c", CONVERT(seq2->getData()[i])); } printf ("\n");
    for (size_t i=0; i<off2; i++)  {  printf (" "); }
    for (size_t i=0; i<_span; i++)  { printf ("X"); } printf ("\n");

    for (size_t i=0; i<seq1->getLength(); i++)  { printf ("%c", CONVERT(seq1->getData()[i])); } printf ("\n");
    for (size_t i=0; i<off1; i++)  {  printf (" "); }
    for (size_t i=0; i<_span; i++)  { printf ("X"); } printf ("\n");
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
