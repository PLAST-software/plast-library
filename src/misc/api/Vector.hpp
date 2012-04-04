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

/** \file Vector.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Vector management.
 */

#ifndef _VECTOR_HPP_
#define _VECTOR_HPP_

/********************************************************************************/

#include <os/impl/DefaultOsFactory.hpp>
#include <misc/api/types.hpp>
#include <string.h>
#include <vector>
#include <list>

/********************************************************************************/
/** \brief Miscellanous definitions */
namespace misc {
/********************************************************************************/

/** \class Vector
 *  \brief std::vector-like class.
 *
 *  The idea is to directly provide the data buffer instead of accessing data through
 *  the refine [] operator like in std::vector. This simple fact may speed up intensive
 *  memory access.
 *
 *  WARNING !!! Use only with basic types and not with class objects.
 */
template<typename T> class Vector
{
public:

    /** Constructor. */
    Vector () : size(0), data(0), _reference(false)  {}

    /** Constructor with initial size.
     * \param aSize : initial size of the vector. */
    Vector (Size aSize) : size(aSize), data(0), _reference(false)
    {
        if (size > 0) {  data = (T*) getAllocator().calloc (size, sizeof(T)); }
    }

    /** Copy constructor. Note that we have a 'ref' mode which means that we don't do a true copy
     *  but only keep reference on data of the provided vector.
     * \param v : vector to be copied. */
    Vector (const Vector<T>& v) : size(0), data(0), _reference(false)
    {
        if (v._reference == true)
        {
            setReference (v.size, v.data);
        }
        else
        {
            resize (v.size);
            memcpy (data, v.data, size);
        }
    }

    /** Destructor. */
    ~Vector ()
    {
        if (_reference==false)  {   getAllocator().free (data);  }
    }

    /** Resize the vector. May lead to some dynamic allocation management.
     * \param aSize : the new size of the vector.
     * \return a pointer to the data.
     */
    T* resize (Size aSize)
    {
        if (_reference==false)
        {
            size = aSize;
            data = (T*) getAllocator().realloc (data, size*sizeof(T));
            return data;
        }
        else
        {
            /** We used a reference, just forget this fact. */
            _reference = false;

            size = aSize;
            data = (T*) getAllocator().calloc (size, sizeof(T));
            return data;
        }
    }

    /** Affectation operator.
     * \param v : the source vector
     * \return the affected vector.
     */
    Vector<T>& operator= (const Vector<T>& v)
    {
        if (this != &v)
        {
            if( v._reference == true)
            {
                setReference (v.size, v.data);
            }
            else
            {
                resize (v.size);
                memcpy (data, v.data, size);
            }
        }

        return *this;
    }

    /** Comparison operator.
     * \param v : the vector to be compared to.
     * \return the comparison result as a boolean.
     */
    bool operator== (const Vector<T>& v) const
    {
        if (size != v.size)  { return false; }

        for (size_t i=0; i<size; i++)
        {
            if (data[i] != v.data[i])  { return false; }
        }
        return true;
    }

    /** Points to some external information.
     * \param aSize : size of the reference data
     * \param reference : data to be referenced.
     */
    void setReference (size_t aSize, T* reference)
    {
        /** We may get rid of previous data. */
        if (_reference==false)  {   getAllocator().free (data);  }

        /** We change the information. */
        size = aSize;
        data = reference;

        /** We keep in mind that we refer a buffer that doesn't belong to us. */
        _reference = true;
    }

    /** Split the vector into several parts.
     * \param sizeMax : number of parts we want to split the vector.
     * \return a list of split vectors.
     */
    std::list<Vector<T> > split (u_int32_t sizeMax)
    {
        std::list<Vector<T> > result;

        /** A little check. */
        if (sizeMax==0)  { sizeMax = this->size;  }

        size_t nmax = this->size / sizeMax;

        size_t total = 0;
        for (size_t n=0;  n < nmax; n++)
        {
            Vector<T>  item;
            item.setReference (sizeMax, this->data + total);
            result.push_back (item);

            total += sizeMax;
        }

        /** The remainder. */
        if (total < this->size)
        {
            Vector<T>  item;
            item.setReference (this->size - total, this->data + total);
            result.push_back (item);
        }

        return result;
    }

    /** Size of the data. */
    Size size;

    /** Pointer to the data. */
    T*   data;

private:
    bool _reference;

    os::IMemoryAllocator& getAllocator ()
    {
        return os::impl::DefaultFactory::memory();
    }
};

/********************************************************************************/
}  /* end of namespaces. */
/********************************************************************************/

#endif /* _VECTOR_HPP_ */
