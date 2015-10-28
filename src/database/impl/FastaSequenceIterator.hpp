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
#include <algo/core/api/IAlgoParameters.hpp>
#include <database/impl/FastaSequencePureIterator.hpp>
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
 *
 *  \code
 *  void foo ()
 *  {
 *      // We create a FASTA iterator
 *      FastaSequenceIterator itSeq ("myDb");
 *      for (itSeq.first(); !itSeq.isDone(); itSeq.next())
 *      {
 *          // We can retrieve the current sequence
 *          const ISequence* seq = itSeq.currentItem();
 *      }
 *  }
 *  \endcode
 *
 */
class FastaSequenceIterator : public AbstractSequenceIterator
{
public:

    /** Constructor.
     * \param[in] filename : path of the FASTA file to be read.
     * \param[in] commentMaxSize : maximum size of sequences comments
     * \param[in] offset0 : starting offset in the file
     * \param[in] offset1 : ending offset in the file
     */
    FastaSequenceIterator (
        const char* filename,
        size_t commentMaxSize = 2*1024,
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

/** \brief Implementation of ISequenceIteratorFactory interface.
 *
 * This implementation creates FastaSequenceIterator instances.
 */
class FastaSequenceIteratorFactory  : public ISequenceIteratorFactory
{
public:
    /** \copydoc ISequenceIteratorFactory::createSequenceIterator */
    virtual ISequenceIterator* createSequenceIterator (const std::string& uri, const misc::Range64& range);
};

class FastaSequenceConditionalPureIteratorFactory : public ISequenceIteratorFactory
{
public:
    FastaSequenceConditionalPureIteratorFactory(algo::core::IParameters* params)
    : _uri(params->queryUri) {}

    /** \copydoc ISequenceIteratorFactory::createSequenceIterator */
    virtual ISequenceIterator* createSequenceIterator (const std::string& uri, const misc::Range64& range) {
        if (true && uri.compare(_uri) == 0) {
            return new FastaSequencePureIterator (uri.c_str(), SEQUENCE_MAX_COMMENT_SIZE, range.begin, range.end);
        }

        return new FastaSequenceIterator(uri.c_str(), SEQUENCE_MAX_COMMENT_SIZE, range.begin, range.end);
    }

private:
    const std::string& _uri;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _FASTA_ITERATOR_HPP_ */
