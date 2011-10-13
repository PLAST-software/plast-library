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

#ifndef _READING_FRAME_SEQUENCE_ITERATOR_HPP_
#define _READING_FRAME_SEQUENCE_ITERATOR_HPP_

/********************************************************************************/

#include "AbstractSequenceIterator.hpp"
#include "types.hpp"
#include <string>
#include <vector>
#include <stdio.h>

/********************************************************************************/
namespace database {
/********************************************************************************/

class ReadingFrameSequenceIterator : public AbstractSequenceIterator
{
public:

    ReadingFrameSequenceIterator (types::ReadingFrame_e frame, ISequenceIterator* nucleotidIter);
    virtual ~ReadingFrameSequenceIterator ();

    /** Implementation of Iterator interface. */
    void first();
    dp::IteratorStatus next();
    bool isDone()  { return (_nucleotidIter ? _nucleotidIter->isDone() : true); }
    ISequence* currentItem()  { return &_sequence;  }

private:
    types::ReadingFrame_e _frame;

    ISequenceIterator* _nucleotidIter;
    void setNucleotidIterator (ISequenceIterator* nucleotidIter)  { SP_SETATTR (nucleotidIter); }

    ISequence _sequence;
    void udpateItem ();

    void getCodes (int direction, LETTER l1, LETTER l2, LETTER l3, char& c1, char& c2, char& c3);

    static const char BAD_CODE = -1;

    IAlphabet* _alphabet;

    /** Define shortcuts for [A,C,G,T] according to the current encoding. */
    LETTER A, C, G, T;

    /** We need a data for storing the currently built protein from amino acids. */
    IWord _data;

    char getDirection()  { return (_frame==types::FRAME_1 || _frame==types::FRAME_2 ||_frame==types::FRAME_3  ?  1 : -1); }
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _READING_FRAME_SEQUENCE_ITERATOR_HPP_ */
