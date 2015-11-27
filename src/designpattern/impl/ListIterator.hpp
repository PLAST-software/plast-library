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

/** \file ListIterator.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Implementation of an Iterator that loops over std::list.
 */

#ifndef _LIST_ITERATOR_HPP_
#define _LIST_ITERATOR_HPP_

#include <designpattern/api/Iterator.hpp>
#include <list>

/********************************************************************************/
namespace dp {
/** \brief Implementation of Design Pattern tools (Observer, SmartPointer, Command...) */
namespace impl {
/********************************************************************************/

/** \brief Iterator that loops over std::list
 *
 *  This class is a wrapper between the STL list implementation and our own Iterator
 *  abstraction.
 *
 *  Note that the class is still a template one since we can iterate on list of anything.
 *
 *  \code
 *  void foo ()
 *  {
 *      list<int> l;
 *      l.push_back (1);
 *      l.push_back (2);
 *      l.push_back (4);
 *
 *      ListIterator<int> it (l);
 *      for (it.first(); !it.isDone(); it.next())       {   cout << it.currentItem() << endl;   }
 *  }
 *  \endcode
 */
template <class T1> class ListIterator : public Iterator<T1>
{
public:

    /** Constructor.
     * \param[in]  l : the list to be iterated
     */
    ListIterator (const std::list<T1>& l)  : _l(l) {}

    /** Destructor (here because of virtual methods). */
    virtual ~ListIterator ()  {}

    /** \copydoc Iterator<T1>::first */
    void first()  {  _iter = _l.begin(); }

    /** \copydoc Iterator<T1>::next */
    dp::IteratorStatus next()   { _iter++;  return dp::ITER_UNKNOWN; }

    /** \copydoc Iterator<T1>::isDone */
    bool isDone() { return _iter == _l.end (); }

    /** \copydoc Iterator<T1>::currentItem */
    T1 currentItem()  { return *_iter; }

private:

    /** List to be iterated. */
    std::list<T1> _l;

    /** STL iterator we are going to wrap. */
    typename std::list<T1>::iterator _iter;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _LIST_ITERATOR_HPP_ */
