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

/** \file Vectorterator.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Implementation of an Iterator that loops over std::vector
 */

#ifndef _VECTOR_ITERATOR_HPP_
#define _VECTOR_ITERATOR_HPP_

#include <designpattern/api/Iterator.hpp>
#include <vector>

/********************************************************************************/
namespace dp {
/** \brief Implementation of Design Pattern tools (Observer, SmartPointer, Command...) */
namespace impl {
/********************************************************************************/

/** \brief Iterator that loops over std::vector
 *
 *  This class is a wrapper between the STL vector implementation and our own Iterator
 *  abstraction.
 *
 *  Note that the class is still a template one since we can iterate on vector of anything.
 *
 *  \code
 *  void foo ()
 *  {
 *      // We create a std::vector holding some values.
 *      int table[] = {16,2,77,29};
 *      std::vector<int> vec (table, table + sizeof(table) / sizeof(int) );
 *
 *      // We create an iterator on this vector.
 *      VectorIterator it (vec);
 *
 *      // We loop over the items of the vector.
 *      for (it.first(); !it.isDone(); it.next())
 *      {
 *          // we can retrieve the current item with it.currentItem()
 *      }
 *  }
 *  \endcode
 */
template <class T1> class VectorIterator : public SmartIterator<T1>
{
public:

    /** Constructor.
     * \param[in]  l : the list to be iterated
     */
    VectorIterator (std::vector<T1>& l)  : _l(l) {}

    /** Destructor (here because of virtual methods). */
    virtual ~VectorIterator ()  {}

    /** \copydoc Iterator<T1>::first */
    void first()  {  _iter = _l.begin(); }

    /** \copydoc Iterator<T1>::next */
    dp::IteratorStatus next()   { _iter++;  return (_iter == _l.end () ? dp::ITER_DONE : dp::ITER_ON_GOING); }

    /** \copydoc Iterator<T1>::isDone */
    bool isDone() { return _iter == _l.end (); }

    /** \copydoc Iterator<T1>::currentItem */
    T1 currentItem()  { return *_iter; }

    /** \copydoc SmartIterator<T1>::size */
    u_int32_t size()  { return _l.size(); }

private:

    /** List to be iterated. */
    std::vector<T1>& _l;

    /** STL iterator we are going to wrap. */
    typename std::vector<T1>::iterator _iter;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _VECTOR_ITERATOR_HPP_ */
