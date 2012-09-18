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

/** \file WrapperIterator.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief String wrapper as implementation of Iterator interface.
 */

#ifndef _WRAPPER_ITERATOR_HPP_
#define _WRAPPER_ITERATOR_HPP_

#include <designpattern/api/Iterator.hpp>
#include <string>

/********************************************************************************/
namespace dp {
/** \brief Implementation of Design Pattern tools (Observer, SmartPointer, Command...) */
namespace impl {
/********************************************************************************/

/** \brief  String wrapper as an Iterator.
 *
 *  Tool for wrapping a string in sub strings of N characters.
 *
 *  \code
 *  void foo ()
 *  {
 *      // We create an wrapper on a string, in order to split it in substrings of size 10
 *      WrapperIterator it ("EFLMDIFJDFMLSIHDFOAIEJFEOIFHMSLDIFJODUFHSLDKFUGMAEIF", 10);
 *
 *      // We loop over each sub strings
 *      for (it.first(); ! it.isDone(); it.next())
 *      {
 *          // We can retrieve the current sub string.
 *          const char* current = it.currentItem();
 *      }
 *  }
 *  \endcode
 */
class WrapperIterator : public Iterator<const char*>
{
public:

    /** Constructors.
     * \param text    : the string to be wrapped.
     * \param nbChars : number of characters of each split part
     */
    WrapperIterator (const std::string& text, size_t nbChars);

    /** Destructor. */
    virtual ~WrapperIterator ();

    /** \copydoc Iterator::first */
    void first();

    /** \copydoc Iterator::next */
    dp::IteratorStatus next();

    /** \copydoc Iterator::isDone */
    bool isDone()  { return _currentIndex >= _length;  }

    /** \copydoc Iterator::currentItem */
    const char* currentItem()  {  return _wrap.c_str() ;  }

private:

    /** String to be wrapped. */
    std::string _txt;

    /** Nb chars */
    size_t _nbChars;

    /** Text length */
    size_t _length;

    /** Current index in string. */
    size_t _currentIndex;

    /** Current wrapping. */
    std::string _wrap;

    /** */
    size_t getCharNb ();
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _WRAPPER_ITERATOR_HPP_ */
