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
#include <string>
#include <vector>
#include <stdio.h>

/********************************************************************************/
namespace database {
/********************************************************************************/

enum ReadingFrame_e { FRAME_1, FRAME_2, FRAME_3, FRAME_4, FRAME_5, FRAME_6};

/********************************************************************************/

class ReadingFrameSequenceIterator : public AbstractSequenceIterator
{
public:

    ReadingFrameSequenceIterator (ReadingFrame_e frame, ISequenceIterator* nucleotidIter);
    virtual ~ReadingFrameSequenceIterator ();

    /** Implementation of Iterator interface. */
    void first();
    dp::IteratorStatus next();
    bool isDone()  { return (_nucleotidIter ? _nucleotidIter->isDone() : true); }
    ISequence* currentItem()  { return &_sequence;  }

private:
    ReadingFrame_e _frame;

    ISequenceIterator* _nucleotidIter;
    void setNucleotidIterator (ISequenceIterator* iter);

    ISequence _sequence;
    void udpateItem ();

    void getCodes (int direction, LETTER l1, LETTER l2, LETTER l3, char& c1, char& c2, char& c3);

    static const char BAD_CODE = -1;

    IAlphabet* _alphabet;

    /** Define shortcuts for [A,C,G,T] according to the current encoding. */
    LETTER A, C, G, T;

    /** We need a data for storing the currently built protein from amino acids. */
    IWord _data;

    char getDirection()  { return (_frame==FRAME_1 || _frame==FRAME_2 ||_frame==FRAME_3  ?  1 : -1); }
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _READING_FRAME_SEQUENCE_ITERATOR_HPP_ */
