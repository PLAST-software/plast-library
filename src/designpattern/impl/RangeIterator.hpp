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

/** \brief
  */
template <class T> class RangeIterator : public SmartPointer
{
public:

    /** Constructors.
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

    /** */
    size_t getNbItems ()  {  return _nbItems;  }

    /** */
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
