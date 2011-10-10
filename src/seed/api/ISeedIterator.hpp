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

#ifndef _ISEED_ITERATOR_HPP_
#define _ISEED_ITERATOR_HPP_

/********************************************************************************/

#include "Iterator.hpp"
#include "ISeed.hpp"

#include <vector>

/********************************************************************************/
namespace seed {
/********************************************************************************/

class ISeedIterator : public dp::Iterator<const ISeed*>
{
public:

    /** Implementation of the Iterator interface. */
    virtual void first() = 0;
    virtual dp::IteratorStatus next()  = 0;
    virtual bool isDone()       = 0;
    virtual const ISeed* currentItem() = 0;

    /** Set the data holding the seeds to be iterated. */
    virtual void setData (const database::IWord& data) = 0;

    /** Create a new iterator that is a subset of the current one.
     *  The range is given by indexes of the first and last seeds. */
    virtual ISeedIterator* extract (size_t firstIdx, size_t lastIdx)  { return 0; }

    /** Keep only seeds whose idx is given in the argument vector.
     *  Returns new iterator if filtering is done, null otherwise. */
    virtual ISeedIterator* createFilteredIterator (const std::vector<size_t>& seedsIdx) = 0;

    /** Returns the number of items to be iterated. Implementations that don't know
     *  this number should return 0 by convention. */
    virtual u_int64_t getNbTotal () = 0;

    /** Another way for iterating: a method that assembles 'isDone', 'currentItem' and 'next'
     *  Note that the provided argument is affected by the current item of the iteration.
     */
    virtual bool retrieve (ISeed& item, u_int64_t& nbRetrieved) = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ISEED_ITERATOR_HPP_  */
