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

/** \file SequenceTokenizer.hpp
 *  \brief Tokenizer that splits a sequence in valid sub sequences (with valid letters)
 *  \date 07/11/2011
 *  \author edrezen
 *
 * Implementation of the ITokenizer
 * holding letters (nucleotids or amino acids).
 */

#ifndef _SEQUENCE_TOKENIZER_HPP_
#define _SEQUENCE_TOKENIZER_HPP_

/********************************************************************************/

#include <designpattern/api/Iterator.hpp>
#include <database/api/ISequence.hpp>
#include <string>
#include <vector>

/********************************************************************************/
namespace database {
/** \brief Implementation of concepts related to genomic databases. */
namespace impl {
/********************************************************************************/

/** \brief Tokenizer that splits a sequence in valid sub sequences (with valid letters)
 *
 * It is also possible to retrieve the actual set of iterated items as a vector of indexes pairs.
 */
class SequenceTokenizer : public dp::Iterator<std::pair<size_t,size_t>& >
{
public:

    /** Constructor.
     */
    SequenceTokenizer (const database::IWord& sequence);

    /** Destructor. */
    virtual ~SequenceTokenizer ()  {}

    /** \copydoc Iterator::first */
    void first()  { _currentIdx = 0; }

    /** \copydoc Iterator::next */
    dp::IteratorStatus next()  { _currentIdx++;   return dp::ITER_UNKNOWN; }

    /** \copydoc Iterator::isDone */
    bool isDone()  { return _currentIdx >= _size; }

    /** \copydoc Iterator::currentItem */
    std::pair<size_t,size_t>& currentItem()  { return _pairs[_currentIdx]; }

    /** Retrieve the indexes couples.
     * \return the vector of couples
     */
    std::vector <std::pair<size_t,size_t> >& getItems ()  { return _pairs; }

private:

    const database::IWord& _sequence;

    std::vector <std::pair<size_t,size_t> > _pairs;

    size_t _currentIdx;
    size_t _size;

    /** Build the list of pairs. */
    void build ();
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _SEQUENCE_TOKENIZER_HPP_ */
