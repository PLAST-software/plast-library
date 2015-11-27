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

/** \file TokenizerIterator.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief String tokenizer as implementation of Iterator interface.
 */

#ifndef _TOKENIZER_ITERATOR_HPP_
#define _TOKENIZER_ITERATOR_HPP_

#include <designpattern/api/Iterator.hpp>
#include <string.h>

/********************************************************************************/
namespace dp {
/** \brief Implementation of Design Pattern tools (Observer, SmartPointer, Command...) */
namespace impl {
/********************************************************************************/

/** \brief  String tokenizer as an Iterator.
 *
 *  Tool for tokenizing strings (like strtok) that follows our Iterator concept.
 *
 *  One should provide both the string to be tokenized and a string holding
 *  characters to be considered as separators.
 *
 *  Code sample:
 *  \code
 *  void sample ()
 *  {
 *      // We create a tokenizer with spaces as separators.
 *      TokenizerIterator it ("this is the text to tokenize", " ");
 *
 *      // We loop the iterator
 *      for (it.first(); !it.isDone(); it.next())
 *      {
 *          // We get the current token
 *          char* token = it.currentItem ();
 *      }
 *  }
 *  \endcode
 */
class TokenizerIterator : public Iterator<char*>
{
public:

    /** Constructors.
     * \param text : the string to be tokenized.
     * \param separator : the separator characters.
     */
    TokenizerIterator (const char* text, const char* separator);

    /** Destructor. */
    virtual ~TokenizerIterator ();

    /** \copydoc Iterator::first */
    void first();

    /** \copydoc Iterator::next */
    dp::IteratorStatus next();

    /** \copydoc Iterator::isDone */
    bool isDone()  { return _loop == 0;  }

    /** \copydoc Iterator::currentItem */
    char* currentItem()  {  return _loop;  }

private:

    /** String holding separator characters. */
    std::string _sep;

    /** Internal string. */
    char* _text;

    /** Internal string. */
    char* _str;

    /** Internal string. */
    char* _loop;

    /** Internal string. */
    char* _save;

    /** */
    char* tok_r (char* s, const char* delim, char** lasts);
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _TOKENIZER_ITERATOR_HPP_ */
