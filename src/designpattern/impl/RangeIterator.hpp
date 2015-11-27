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

/** \file RangeIterator.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Range iteration
 */

#ifndef _RANGE_ITERATOR_HPP_
#define _RANGE_ITERATOR_HPP_

#include <designpattern/api/Iterator.hpp>
#include <misc/api/types.hpp>
#include <misc/api/macros.hpp>

/********************************************************************************/
namespace dp {
/** \brief Implementation of Design Pattern tools (Observer, SmartPointer, Command...) */
namespace impl {
/********************************************************************************/

/** \brief Iterator that splits a range in several range of given size.
 *
 *  This class is useful for splitting some integer range into small parts of given size.
 *
 * \code
 * void foo ()
 * {
 *     // We create some range.
 *     Range<u_int32_t> r (1,1000);
 *
 *     // We create an iterator on this range with step 100
 *     RangeIterator it (r, 100, 0);
 *
 *     // We loop over the sub ranges
 *     size_t nbRetrieved = 0;
 *     Range<u_int32_t> loop;
 *
 *     while (it.retrieve (loop,nbRetrieved) == true)
 *     {
 *          // here we should get ranges [1,100], [101,200], ...
 *     }
 * }
 *
 * \endcode
 */
template <class T> class RangeIterator : public SmartPointer
{
public:

    /** Constructors.
     * \param[in] range : the range to be split
     * \param[in] step : size of the sub ranges to be iterated
     * \param[in] synchro : if provided, used for protecting from concurrent access
     */
    RangeIterator (const misc::Range<T>& range, T step, os::ISynchronizer* synchro)
        : _range(range),  _step(step), _synchro(synchro), _nbItems(0), _nbRetrieved(0)
    {
        _value = _range.begin;

        /** A little life insurance. */
        if (_step == 0)  { _step = 1; }

        /** We compute the number of ranges to be iterated. */
        _nbItems = _range.getLength() / _step + (_range.getLength() % _step != 0 ? 1 : 0);
    }

    /** Destructor. */
    virtual ~RangeIterator ()
    {
        /** WARNING ! we suppose that it is up to the client to delete the provided synchronizer. */
        if (_synchro)  { delete _synchro; }
    }

    /** Return the number of sub ranges to be iterated.
     * \return the number of ranges
     */
    size_t getNbItems ()  {  return _nbItems;  }

    /** Retrieve the current iterated range.
     * \param[in] range : the retrieved iterated range
     * \param[in] nbRetrieved : the number of ranges iterated so far
     * \return false if the iteration is finished, false otherwise.
     */
    bool retrieve (misc::Range<T>& range, size_t& nbRetrieved)
    {
        bool result = false;

        if (_synchro)  { _synchro->lock(); }

        if (_value <= _range.end)
        {
            T tmp = _value + _step - 1;

            range.begin = _value;
            range.end   = MIN (tmp, _range.end);
            _value      = tmp + 1;

            nbRetrieved = ++_nbRetrieved;

            result = true;
        }

        if (_synchro)  { _synchro->unlock(); }

        return result;
    }

private:

    misc::Range<T>     _range;
    T                  _step;
    T                  _value;
    os::ISynchronizer* _synchro;

    size_t _nbItems;
    size_t _nbRetrieved;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _RANGE_ITERATOR_HPP_ */
