/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.3, released November 2015                                     *
 *   Copyright (c) 2009-2015 Inria-Cnrs-Ens                                  *
 *                                                                           *
 *   PLAST is free software; you can redistribute it and/or modify it under  *
 *   the Affero GPL ver 3 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   Affero GPL ver 3 License for more details.                              *
 *****************************************************************************/

/** \file ReverseStrandSequenceIterator.hpp
 *  \brief Sequence iterator of the reverse strand
 *  \date 07/11/2011
 *  \author edrezen
 *
 * Provides a way to iterate the reverse strand of nucleotides sequences.
 */

#ifndef _REVERSE_STRAND_SEQUENCE_ITERATOR_HPP_
#define _REVERSE_STRAND_SEQUENCE_ITERATOR_HPP_

/********************************************************************************/

#include <database/impl/AbstractSequenceIterator.hpp>
#include <database/impl/FastaSequenceIterator.hpp>
#include <database/impl/BlastdbSequenceIterator.hpp>
#include <database/impl/DatabaseUtility.hpp>


#include <misc/api/types.hpp>

#include <string>
#include <vector>
#include <stdio.h>

/********************************************************************************/
namespace database {
/** \brief Implementation of concepts related to genomic databases. */
namespace impl {
/********************************************************************************/

/** \brief ISequenceIterator on reverse strand of nucleotides sequences
 *
 * This iterator takes as input an ISequenceIterator (supposed to provide
 * nucleotides sequences), and provides as output the reverse strand of each
 * sequence of the input iterator.
 *
 * \code
 * void foo ()
 * {
 *      // we read a nucleotides FASTA database
 *      ISequenceIterator* nuclIt = new FastaSequenceIterator ("adn.fa");
 *
 *      // we create an iterator that reverse the incoming sequences
 *      ISequenceIterator* reverseIt = new ReverseStrandSequenceIterator (nuclIt);
 *
 *      // we loop our reversed sequences
 *      for (reverseIt->first(); !reverseIt->isDone(); reverseIt->next())
 *      {
 *          const ISequence* seq = reverseIt->currentItem();  // should be the reversed strand of the initial sequence
 *      }
 * }
 * \endcode
 *
 */
class ReverseStrandSequenceIterator : public AbstractSequenceIterator
{
public:

    /** Constructor.
     * \param[in] nucleotidIter : iterator over the nucleotide sequences.
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

/** \brief Implementation that returns instances of ReverseStrandSequenceIterator.
 */
class ReverseStrandSequenceIteratorFactory  : public ISequenceIteratorFactory
{
public:
    /** \copydoc ISequenceIteratorFactory::createSequenceIterator  */
    virtual ISequenceIterator* createSequenceIterator (const std::string& uri, const misc::Range64& range)
    {
    	DatabaseLookupType::QuickReaderType_e databaseType = DatabaseLookupType::ENUM_TYPE_UNKNOWN;

    	databaseType = DatabaseLookupType::quickReaderType(uri);
    	if ((databaseType==DatabaseLookupType::ENUM_BLAST_PIN)||(databaseType==DatabaseLookupType::ENUM_BLAST_NIN)
    		||(databaseType==DatabaseLookupType::ENUM_BLAST_PAL)||(databaseType==DatabaseLookupType::ENUM_BLAST_NAL))
    		return (new ReverseStrandSequenceIterator (new BlastdbSequenceIterator (uri.c_str(), SEQUENCE_MAX_COMMENT_SIZE, range.begin, range.end)));
    	else
    		return (new ReverseStrandSequenceIterator (new FastaSequenceIterator (uri.c_str(), SEQUENCE_MAX_COMMENT_SIZE, range.begin, range.end)));
    }
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _REVERSE_STRAND_SEQUENCE_ITERATOR_HPP_ */
