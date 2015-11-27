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

/** \file TruncatedSequenceIterator.hpp
 *  \brief Sequence iterator that controls the number of iterated items
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _TRUNCATED_SEQUENCES_ITERATOR_HPP_
#define _TRUNCATED_SEQUENCES_ITERATOR_HPP_

/********************************************************************************/

#include <database/impl/AbstractSequenceIterator.hpp>
#include <misc/api/types.hpp>

/********************************************************************************/
namespace database {
/** \brief Implementation of concepts related to genomic databases. */
namespace impl {
/********************************************************************************/

/** \brief Sequence iterator that controls the number of iterated items
 *
 *  This class is a Proxy (design pattern) since it controls the number of iterated
 *  items by a given sequences iterator.
 *
 *  It may be useful for truncating a database.
 */
class TruncatedSequenceIterator : public AbstractSequenceIterator
{
public:

    /** Constructor.
     * \param[in] ref : the actual sequences iterator to be iterated.
     * \param[in] nbTruncate : number of sequences to be iterated.
     */
    TruncatedSequenceIterator (ISequenceIterator* ref, size_t nbTruncate)
        : _ref(0), _nbTruncate(nbTruncate), _currentNb(0)  { setRef (ref); }

    /** Destructor. */
    virtual ~TruncatedSequenceIterator ()  { setRef(0); }

    /** \copydoc AbstractSequenceIterator::first */
    void first()   { _currentNb=0;  if (_ref)  { _ref->first (); } }

    /** \copydoc AbstractSequenceIterator::next */
    dp::IteratorStatus next()  {  _currentNb++; return (_ref ? _ref->next() : dp::ITER_UNKNOWN);   }

    /** \copydoc AbstractSequenceIterator::isDone */
    bool isDone()  { return (_ref ? _currentNb >=_nbTruncate ||  _ref->isDone() : true);  }

    /** \copydoc AbstractSequenceIterator::currentItem */
    const ISequence* currentItem() { return (_ref ? _ref->currentItem() : 0); }

    /** \copydoc AbstractSequenceIterator::clone */
    ISequenceIterator* clone () { return new TruncatedSequenceIterator(_ref,_nbTruncate); }

private:

    /** Referenced iterator. */
    ISequenceIterator* _ref;
    void setRef (ISequenceIterator* ref)  { SP_SETATTR(ref); }

    /** */
    size_t _nbTruncate;

    /** */
    size_t _currentNb;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _TRUNCATED_SEQUENCES_ITERATOR_HPP_ */
