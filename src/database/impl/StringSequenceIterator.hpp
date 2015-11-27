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

/** \file StringSequenceIterator.hpp
 *  \brief Sequence iterator parsing a simple string
 *  \date 07/11/2011
 *  \author edrezen
 *
 * Implementation of the ISequenceIterator interface by parsing a simple string
 * holding letters (nucleotids or amino acids).
 */

#ifndef _STRING_SEQUENCE_ITERATOR_HPP_
#define _STRING_SEQUENCE_ITERATOR_HPP_

/********************************************************************************/

#include <database/impl/AbstractSequenceIterator.hpp>
#include <string>
#include <vector>

/********************************************************************************/
namespace database {
/** \brief Implementation of concepts related to genomic databases. */
namespace impl {
/********************************************************************************/

/** \brief Sequence iterator parsing a simple string
 *
 * Implementation of the ISequenceIterator interface by parsing a simple string
 * holding letters (nucleotides or amino acids).
 *
 * Note that the comments of the iterated ISequence instances are fake.
 *
 * At construction, one can provide several strings, each string is then used as
 * a data sequence.
 *
 * This class is useful for unit testing.
 *
 * Code sample:
 * \code
 * void sample ()
 * {
 *      // we create a string sequence iterator with 3 sequences
 *      ISequenceIterator* it = new StringSequenceIterator (3,
 *          "KAKLAEQAERYDDMAAAMKAVTEQGHELSNEERNLL",
 *          "FDEAIAELDTLNEESYKDSTL",
 *          "VHFETHEAAQNAISTMNGMLLNDRKVFVGHFKPR"
 *      );
 *
 *      // we iterate the sequences
 *      for (it->first(); !it->isDone(); it->next())
 *      {
 *          // we retrieve a sequence
 *          ISequence* seq = it->currentItem ();
 *      }
 * }
 * \endcode
 */
class StringSequenceIterator : public AbstractSequenceIterator
{
public:

    /** Constructor. Note the ellipsis which allows to define several sequences in a single call.
     * \param[in] nb : number of sequences as strings.
     */
    StringSequenceIterator (size_t nb, ...);

    /** Constructor.
     * \param[in] strings : sequences as strings.
     */
    StringSequenceIterator (const std::vector<std::string>& strings);

    /** Destructor. */
    virtual ~StringSequenceIterator ();

    /** \copydoc AbstractSequenceIterator::first */
    void first()   { _currentIdx=0;  updateItem(); }

    /** \copydoc AbstractSequenceIterator::next */
    dp::IteratorStatus next()    { _currentIdx++;  updateItem();  return dp::ITER_UNKNOWN; }

    /** \copydoc AbstractSequenceIterator::isDone */
    bool isDone()  { return _currentIdx >= _strings.size() ;}

    /** \copydoc AbstractSequenceIterator::currentItem */
    ISequence* currentItem() { return getBuilder()->getSequence(); }

    /** \copydoc AbstractSequenceIterator::clone */
    ISequenceIterator* clone ()  { return new StringSequenceIterator (_strings); }

private:

    /** Container holding all sequences defined as strings. */
    std::vector<std::string> _strings;

    /** Current index in the iteration. */
    size_t _currentIdx;

    /** Update the current item of the iteration. */
    void updateItem ();
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _STRING_SEQUENCE_ITERATOR_HPP_ */
