/*
 * Database.hpp
 *
 *  Created on: 7 juin 2011
 *      Author: edrezen
 */

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
