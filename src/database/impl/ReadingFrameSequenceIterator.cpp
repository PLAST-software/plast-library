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

#include <database/impl/ReadingFrameSequenceIterator.hpp>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace os;
using namespace misc;

/********************************************************************************/
namespace database { namespace impl  {
/********************************************************************************/

static LETTER nucleotid2acid [4][4][4] =
{
    {
        /** AA **/  {   CODE_K,     CODE_N,     CODE_K,     CODE_N, },
        /** AC **/  {   CODE_T,     CODE_T,     CODE_T,     CODE_T, },
        /** AG **/  {   CODE_R,     CODE_S,     CODE_R,     CODE_S, },
        /** AT **/  {   CODE_I,     CODE_I,     CODE_M,     CODE_I, },
    },
    {
        /** CA **/  {   CODE_Q,     CODE_H,     CODE_Q,     CODE_H, },
        /** CC **/  {   CODE_P,     CODE_P,     CODE_P,     CODE_P, },
        /** CG **/  {   CODE_R,     CODE_R,     CODE_R,     CODE_R, },
        /** CT **/  {   CODE_L,     CODE_L,     CODE_L,     CODE_L, },
    },
    {
        /** GA **/  {   CODE_E,     CODE_D,     CODE_E,     CODE_D, },
        /** GC **/  {   CODE_A,     CODE_A,     CODE_A,     CODE_A, },
        /** GG **/  {   CODE_G,     CODE_G,     CODE_G,     CODE_G, },
        /** GT **/  {   CODE_V,     CODE_V,     CODE_V,     CODE_V, },
    },
    {
        /** TA **/  {   CODE_STAR,  CODE_Y,     CODE_STAR,  CODE_Y, },
        /** TC **/  {   CODE_S,     CODE_S,     CODE_S,     CODE_S, },
        /** TG **/  {   CODE_STAR,  CODE_C,     CODE_W,     CODE_C, },
        /** TT **/  {   CODE_L,     CODE_F,     CODE_L,     CODE_F, },
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
ReadingFrameSequenceIterator::ReadingFrameSequenceIterator (ReadingFrame_e frame, ISequenceIterator* nucleotidIter)
    : _frame(frame), _nucleotidIter (0), _data (10*1024)
{
    DEBUG (("ReadingFrameSequenceIterator::ReadingFrameSequenceIterator  frame=%d  iter=%p\n", frame, nucleotidIter));

    setNucleotidIterator (nucleotidIter);

    _alphabet = EncodingManager::singleton().getAlphabet(SUBSEED);

    /** We set the shortcuts for letters. */
    A = _alphabet->letters[CODE_A];
    C = _alphabet->letters[CODE_C];
    G = _alphabet->letters[CODE_G];
    T = _alphabet->letters[CODE_T];

    _sequence.data.encoding = SUBSEED;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ReadingFrameSequenceIterator::~ReadingFrameSequenceIterator ()
{
    setNucleotidIterator (0);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void ReadingFrameSequenceIterator::first()
{
    if (_nucleotidIter)
    {
        _nucleotidIter->first ();
        udpateItem ();
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
dp::IteratorStatus ReadingFrameSequenceIterator::next()
{
    if (_nucleotidIter)
    {
        _nucleotidIter->next ();
        udpateItem ();
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
void ReadingFrameSequenceIterator::udpateItem ()
{
    /** A little check. */
    if (isDone ())  { return; }

    const ISequence* refSeq = _nucleotidIter->currentItem();
    DEBUG (("ReadingFrameSequenceIterator::udpateItem:  inputSeq='%s'\n", refSeq->data.toString().c_str() ));

    /** We set the comment. */
    _sequence.comment = refSeq->comment;

    /** We parse the current data of the reference sequence. */
    const IWord& refData = refSeq->data;

    char direction = getDirection();
    size_t i0, i1, offset;

    /** We determine parameters according to the frame. */
    if (direction == 1)
    {
        i0     = _frame;
        i1     = refData.letters.size - 3;
        offset = 0;
    }
    else
    {
        i0     = _frame - 3;
        i1     = refData.letters.size - 3;
        offset = refData.letters.size - 1;
    }

    size_t nbAminoAcids = 0;
    size_t i = 0;
    for (i=i0; i<=i1; i += 3)
    {
        char c1, c2, c3;

        /** We get the codes of the three amino acids. */
        getCodes (
            direction,
            refData.letters.data[offset + direction*(i+0)],
            refData.letters.data[offset + direction*(i+1)],
            refData.letters.data[offset + direction*(i+2)],
            c1,c2,c3
        );

        if (c1!=BAD_CODE && c2!=BAD_CODE && c3!=BAD_CODE)
        {
            /** We compute the acid from the codon. */
            LETTER acid = nucleotid2acid [(int)c1][(int)c2][(int)c3];

            //DEBUG (("(%d %d %d => acid=%d  (direction=%d)) \n", c1,c2,c3, acid, direction));

            if (_data.letters.size <= nbAminoAcids)
            {
                _data.letters.resize (2*_data.letters.size);
            }

            _data.letters.data [nbAminoAcids++] = acid;
        }
        else
        {
            /** We found a bad code. */
            //printf ("FOUND BAD CODE\n");
        }
    }

    /** We may have to add an extra 'any' letter because we had less than 3 nucleotids at the end of the loop. */
    // TO BE CONFIRMED...
    if (i < refData.letters.size)  {  _data.letters.data [nbAminoAcids++] = _alphabet->any;  }

    _sequence.data.letters.setReference (nbAminoAcids, _data.letters.data);

    /** We retrieve the builder => shortcut and optimization (avoid method call) */
    ISequenceBuilder* builder = getBuilder();
    if (builder != 0)
    {
        builder->setComment (_sequence.comment, strlen(_sequence.comment));

        builder->resetData ();
        builder->addData (
            _data.letters.data,
            nbAminoAcids,
            getEncoding()
        );
    }

    DEBUG (("ReadingFrameSequenceIterator::udpateItem: outputSeq='%s'\n", _sequence.data.toString().c_str() ));
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void ReadingFrameSequenceIterator::getCodes (int direction, LETTER l1, LETTER l2, LETTER l3, char& c1, char& c2, char& c3)
{
    /** We duplicate code for optimization => less calls to the method (we get the 3 codes in a single call). */

    if (direction > 0)
    {
         if (l1==A)  c1=0;  else if (l1==C)  c1=1;  else if (l1==G)  c1=2;  else if (l1==T)  c1=3;  else c1=BAD_CODE;
         if (l2==A)  c2=0;  else if (l2==C)  c2=1;  else if (l2==G)  c2=2;  else if (l2==T)  c2=3;  else c2=BAD_CODE;
         if (l3==A)  c3=0;  else if (l3==C)  c3=1;  else if (l3==G)  c3=2;  else if (l3==T)  c3=3;  else c3=BAD_CODE;
    }
    else
    {
        if (l1==A)   c1=3;  else if (l1==C)  c1=2;  else if (l1==G)  c1=1;  else if (l1==T)  c1=0;  else c1=BAD_CODE;
        if (l2==A)   c2=3;  else if (l2==C)  c2=2;  else if (l2==G)  c2=1;  else if (l2==T)  c2=0;  else c2=BAD_CODE;
        if (l3==A)   c3=3;  else if (l3==C)  c3=2;  else if (l3==G)  c3=1;  else if (l3==T)  c3=0;  else c3=BAD_CODE;
    }
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

