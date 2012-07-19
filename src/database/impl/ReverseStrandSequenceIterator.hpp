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

/** \file ReverseStrandSequenceIterator.hpp
 *  \brief Sequence iterator for one reading frame
 *  \date 07/11/2011
 *  \author edrezen
 *
 * Provides a way to iterate amino acid sequences from an iterator on nucleotid
 * sequences.
 */

#ifndef _REVERSE_STRAND_SEQUENCE_ITERATOR_HPP_
#define _REVERSE_STRAND_SEQUENCE_ITERATOR_HPP_

/********************************************************************************/

#include <database/impl/AbstractSequenceIterator.hpp>
#include <database/impl/FastaSequenceIterator.hpp>

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
 */
class ReverseStrandSequenceIterator : public AbstractSequenceIterator
{
public:

    /** Constructor.
     * \param[in] nucleotidIter : iterator over the nucleotid sequences.
     */
	ReverseStrandSequenceIterator (ISequenceIterator* nucleotidIter);

    /** Destructor. */
    virtual ~ReverseStrandSequenceIterator ();

    /** \copydoc AbstractSequenceIterator::first */
    void first();

    /** \copydoc AbstractSequenceIterator::next */
    dp::IteratorStatus next();

    /** \copydoc AbstractSequenceIterator::isDone */
    bool isDone()  { return (_nucleotidIter ? _nucleotidIter->isDone() : true); }

    /** \copydoc AbstractSequenceIterator::currentItem */
    ISequence* currentItem()  { return &_sequence;  }

    /** \copydoc AbstractSequenceIterator::clone */
    ISequenceIterator* clone () { return new ReverseStrandSequenceIterator (_nucleotidIter); }

private:

    /** Reference on the nucleotid sequences iterator. */
    ISequenceIterator* _nucleotidIter;

    /** Smart setter for the_nucleotidIter attribute. */
    void setNucleotidIterator (ISequenceIterator* nucleotidIter)  { SP_SETATTR (nucleotidIter); }

    /** Sequence to be filled and returned as current item of the iteration. */
    ISequence _sequence;

    /** Update the current item of the iteration. */
    void udpateItem ();

    /** Alphabet to be used. */
    IAlphabet* _alphabet;

    /** Define shortcuts for [A,C,G,T] according to the current encoding. */
    LETTER A, C, G, T;
};

/********************************************************************************/

/** */
class ReverseStrandSequenceIteratorFactory  : public ISequenceIteratorFactory
{
public:
    /** Create a sequence iterator given an uri (and a range).
     */
    virtual ISequenceIterator* createSequenceIterator (const std::string& uri, const misc::Range64& range)
    {
        return new ReverseStrandSequenceIterator (new FastaSequenceIterator (uri.c_str(), 64*1024, range.begin, range.end));
    }
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _REVERSE_STRAND_SEQUENCE_ITERATOR_HPP_ */
