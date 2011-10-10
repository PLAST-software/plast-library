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

/*****************************************************************************
 *   Implementation of an Iterator that loops over std::list.
 *****************************************************************************/

#ifndef _LIST_ITERATOR_HPP_
#define _LIST_ITERATOR_HPP_

#include "Iterator.hpp"
#include <list>

/********************************************************************************/
namespace dp {
/********************************************************************************/

/** */
template <class T1> class ListIterator : public Iterator<T1>
{
public:

    ListIterator (const std::list<T1>& l)  : _l(l) {}

    virtual ~ListIterator ()  {}

    void first()  {  _iter = _l.begin(); }

    dp::IteratorStatus next()   { _iter++;  return dp::ITER_UNKNOWN; }

    bool isDone() { return _iter == _l.end (); }

    T1 currentItem()  { return *_iter; }

private:

    std::list<T1> _l;
    typename std::list<T1>::iterator _iter;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _LIST_ITERATOR_HPP_ */
