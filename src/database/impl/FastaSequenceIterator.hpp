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

/** \file FastaSequenceIterator.hpp
 *  \brief Sequence iterator that parses FASTA files
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _FASTA_ITERATOR_HPP_
#define _FASTA_ITERATOR_HPP_

/********************************************************************************/

#include <database/impl/AbstractSequenceIterator.hpp>
#include <designpattern/impl/FileLineIterator.hpp>
#include <misc/api/types.hpp>

/********************************************************************************/
namespace database {
/** \brief Implementation of concepts related to genomic databases. */
namespace impl {
/********************************************************************************/

/** \brief Sequence iterator that parses FASTA files
 *
 *  This class implements the ISequenceIterator interface by iterating sequences
 *  read from a FASTA file.
 *
 *  It is possible to set a maximum size for the comment associated to each sequence.
 *
 *  It is also possible to set a range to be read in the file. This range is given
 *  by two offsets (begin, end); such offsets may have been computed through the
 *  IDatabaseQuickReader::getOffsets method. If (0,0) is provided, the full file is read.
 */
class FastaSequenceIterator : public AbstractSequenceIterator
{
public:

    /** Constructor.
     * \param[in] filename : path of the FASTA file to be read.
     * \param[in] commentMaxSize : maximum size of sequences comments
     * \param[in] offset0 : starting offset
     * \param[in] offset1 : ending offset
     */
    FastaSequenceIterator (
        const char* filename,
        size_t commentMaxSize = 64*1024,
        u_int64_t offset0 = 0,
        u_int64_t offset1 = 0
    );

    /** Destructor. */
    virtual ~FastaSequenceIterator ();

    /** \copydoc AbstractSequenceIterator::first */
    void first();

    /** \copydoc AbstractSequenceIterator::next */
    dp::IteratorStatus next();

    /** \copydoc AbstractSequenceIterator::isDone */
    bool isDone()  { return _isDone;  }

    /** \copydoc AbstractSequenceIterator::currentItem */
    ISequence* currentItem() { return getBuilder()->getSequence(); }

    /** \copydoc AbstractSequenceIterator::clone */
    ISequenceIterator* clone () { return 0; }

private:

    /** Maximum size of a sequence comment. */
    size_t _commentMaxSize;

    /** File line iterator for reading the file line by line. */
    dp::impl::FileLineIterator _fileIterator;

    /** Tells whether the iteration is finished or not. */
    bool _isDone;
};

/********************************************************************************/

/** */
class FastaSequenceIteratorFactory  : public ISequenceIteratorFactory
{
public:
    /** Create a sequence iterator given an uri (and a range).
     */
    virtual ISequenceIterator* createSequenceIterator (const std::string& uri, const misc::Range64& range)
    {
        return new FastaSequenceIterator (uri.c_str(), 64*1024, range.begin, range.end);
    }
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _FASTA_ITERATOR_HPP_ */
