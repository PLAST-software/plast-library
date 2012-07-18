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

#include <database/impl/ReverseStrandSequenceIterator.hpp>
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

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ReverseStrandSequenceIterator::ReverseStrandSequenceIterator (ISequenceIterator* nucleotidIter)
    : _nucleotidIter (0)
{
    DEBUG (("ReverseStrandSequenceIterator::ReverseStrandSequenceIterator  frame=%d  iter=%p\n", frame, nucleotidIter));

    setNucleotidIterator (nucleotidIter);

    _alphabet = EncodingManager::singleton().getAlphabet(SUBSEED);

    /** We set the shortcuts for letters. */
    A = 0;
    C = 1;
    G = 2;
    T = 3;

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
ReverseStrandSequenceIterator::~ReverseStrandSequenceIterator ()
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
void ReverseStrandSequenceIterator::first()
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
dp::IteratorStatus ReverseStrandSequenceIterator::next()
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
void ReverseStrandSequenceIterator::udpateItem ()
{
	static LETTER convert[] = { 3, 2, 1, 0};

    /** A little check. */
    if (isDone ())  { return; }

    const ISequence* refSeq = _nucleotidIter->currentItem();
    DEBUG (("ReverseStrandSequenceIterator::udpateItem:  inputSeq='%s'\n", refSeq->data.toString().c_str() ));

    /** Shortcut. */
    size_t nbResidues = refSeq->getLength();

    /** We set the comment. */
    _sequence.comment = refSeq->comment;

    /** We resize the data. */
    _sequence.data.letters.resize (nbResidues);

    LETTER* dataIn  = refSeq->data.letters.data;
    LETTER* dataOut = _sequence.data.letters.data;

    for (size_t i=0; i<nbResidues; i++)
    {
    	LETTER l = dataIn [nbResidues - 1 - i];

    	dataOut[i] = l<4 ? convert[(int)l] : l;
    }

    /** We retrieve the builder => shortcut and optimization (avoid method call) */
    ISequenceBuilder* builder = getBuilder();
    if (builder != 0)
    {
        builder->setComment (_sequence.comment, strlen(_sequence.comment));
        builder->resetData ();
        builder->addData (dataOut, nbResidues, getEncoding());
    }

    DEBUG (("ReverseStrandSequenceIterator::udpateItem: outputSeq='%s'\n", _sequence.data.toString().c_str() ));
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

