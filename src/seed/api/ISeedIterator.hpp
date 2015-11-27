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

/** \file ISeedIterator.hpp
 *  \brief Concept of seeds iteration
 *  \date 07/11/2011
 *  \author edrezen
 *
 *  Iterator implementation that iterates over ISeed instances.
 *
 *  It may be used for iterating all the seeds of some seed model.
 *
 *  Seeds iterators are a central point in the PLAST algorithm because they provide
 *  the root item (seeds) of the algorithm from which all computations are done.
 */

#ifndef _ISEED_ITERATOR_HPP_
#define _ISEED_ITERATOR_HPP_

/********************************************************************************/

#include <designpattern/api/Iterator.hpp>
#include <seed/api/ISeed.hpp>

#include <vector>

/********************************************************************************/
/** \brief Seed based concepts. */
namespace seed {
/********************************************************************************/

/** \brief Iteration on seeds.
 *
 *  This interface inherits from the Iterator interface and iterates specifically
 *  const ISeed* items.
 *
 *  Seeds models are likely to be factories for implementors of this interface.
 */
class ISeedIterator : public dp::Iterator<const ISeed*>
{
public:

    /** \copydoc dp::Iterator<const ISeed*>::first */
    virtual void first() = 0;

    /** \copydoc dp::Iterator<const ISeed*>::next */
    virtual dp::IteratorStatus next()  = 0;

    /** \copydoc dp::Iterator<const ISeed*>::isDone */
    virtual bool isDone()       = 0;

    /** \copydoc dp::Iterator<const ISeed*>::currentItem */
    virtual const ISeed* currentItem() = 0;

    /** Set the data holding the seeds to be iterated.
     * \param[in] data : the data to be iterated
     */
    virtual void setData (const database::IWord& data) = 0;

    /** Create a new iterator that is a subset of the current one.
     *  The range is given by indexes of the first and last seeds.
     *  \param[in] firstIdx : first index to be iterated.
     *  \param[in] lastIdx  : last index to be iterated.
     *  \return a seed iterator.
     */
    virtual ISeedIterator* extract (size_t firstIdx, size_t lastIdx)  { return 0; }

    /** Keep only seeds whose idx is given in the argument vector.
     *  Returns new iterator if filtering is done, null otherwise.
     *  \param[in] seedsIdx : vector of indexes to be kept.
     *  \return a seed iterator.
     */
    virtual ISeedIterator* createFilteredIterator (const std::vector<size_t>& seedsIdx) = 0;

    /** Returns the number of items to be iterated. Implementations that don't know
     *  this number should return 0 by convention.
     *  \return number of iterated items
     */
    virtual u_int64_t getNbTotal () = 0;

    /** Another way for iterating: a method that assembles 'isDone', 'currentItem' and 'next'
     *  Note that the provided argument is affected by the current item of the iteration.
     *  \param[out] item : item to be filled
     *  \param[out] nbRetrieved : number of retrieved item in the iteration
     *  \return true if the iteration is finished, false otherwise.
     */
    virtual bool retrieve (ISeed& item, u_int64_t& nbRetrieved) = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ISEED_ITERATOR_HPP_  */
