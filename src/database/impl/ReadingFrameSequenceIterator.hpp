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

/** \file ReadingFrameSequenceIterator.hpp
 *  \brief Sequence iterator for one reading frame
 *  \date 07/11/2011
 *  \author edrezen
 *
 * Provides a way to iterate amino acid sequences from an iterator on nucleotid
 * sequences.
 */

#ifndef _READING_FRAME_SEQUENCE_ITERATOR_HPP_
#define _READING_FRAME_SEQUENCE_ITERATOR_HPP_

/********************************************************************************/

#include <database/impl/AbstractSequenceIterator.hpp>
#include <misc/api/types.hpp>

#include <string>
#include <vector>
#include <stdio.h>

/********************************************************************************/
namespace database {
/** \brief Implementation of concepts related to genomic databases. */
namespace impl {
/********************************************************************************/

/** \brief ISequenceIterator on amino acid sequences from an iterator on nucleotids
 *
 *  This class implements sequences iterator on amino acid sequences from an iterator on nucleotids
 *  for a given reading frame.
 *
 *  This is the kind of sequence iterator created by the ReadingFrameSequenceDatabase class.
 *
 *  \see ReadingFrameSequenceDatabase
 */
class ReadingFrameSequenceIterator : public AbstractSequenceIterator
{
public:

    /** Constructor.
     * \param[in] frame : opening reading frame used for translating the referenced nucleotid database
     * \param[in] nucleotidIter : iterator over the nucleotid sequences.
     */
    ReadingFrameSequenceIterator (misc::ReadingFrame_e frame, ISequenceIterator* nucleotidIter);

    /** Destructor. */
    virtual ~ReadingFrameSequenceIterator ();

    /** \copydoc AbstractSequenceIterator::first */
    void first();

    /** \copydoc AbstractSequenceIterator::next */
    dp::IteratorStatus next();

    /** \copydoc AbstractSequenceIterator::isDone */
    bool isDone()  { return (_nucleotidIter ? _nucleotidIter->isDone() : true); }

    /** \copydoc AbstractSequenceIterator::currentItem */
    ISequence* currentItem()  { return &_sequence;  }

    /** \copydoc AbstractSequenceIterator::clone */
    ISequenceIterator* clone () { return new ReadingFrameSequenceIterator (_frame, _nucleotidIter); }

private:

    /** The reading frame used for conversion. */
    misc::ReadingFrame_e _frame;

    /** Reference on the nucleotid sequences iterator. */
    ISequenceIterator* _nucleotidIter;

    /** Smart setter for the_nucleotidIter attribute. */
    void setNucleotidIterator (ISequenceIterator* nucleotidIter)  { SP_SETATTR (nucleotidIter); }

    /** Sequence to be filled and returned as current item of the iteration. */
    ISequence _sequence;

    /** Update the current item of the iteration. */
    void udpateItem ();

    /** Conversion. */
    void getCodes (int direction, LETTER l1, LETTER l2, LETTER l3, char& c1, char& c2, char& c3);

    static const char BAD_CODE = -1;

    /** Alphabet to be used. */
    IAlphabet* _alphabet;

    /** Define shortcuts for [A,C,G,T] according to the current encoding. */
    LETTER A, C, G, T;

    /** Data for storing the currently built protein from amino acids. */
    IWord _data;

    /** Returns the direction of the current frame.
     * \return 1 for strands 1,2,3 and -1 for strands 4,5,6
     */
    char getDirection()  { return (_frame==misc::FRAME_1 || _frame==misc::FRAME_2 ||_frame==misc::FRAME_3  ?  1 : -1); }
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _READING_FRAME_SEQUENCE_ITERATOR_HPP_ */
