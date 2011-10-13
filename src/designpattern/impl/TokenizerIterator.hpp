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

#ifndef _TOKENIZER_ITERATOR_HPP_
#define _TOKENIZER_ITERATOR_HPP_

#include "Iterator.hpp"
#include <string>
#include <stdio.h>
#include <string.h>

/********************************************************************************/
namespace dp {
/********************************************************************************/

class TokenizerIterator : public Iterator<char*>
{
public:

    TokenizerIterator (const char* text, const char* seperator);
    virtual ~TokenizerIterator ();

    /** */
    void first();

    /** */
    dp::IteratorStatus next();

    /** */
    bool isDone()  { return _loop == 0;  }

    /** */
    char* currentItem()  {  return _loop;  }

private:
    std::string _sep;
    char* _str;
    char* _loop;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _TOKENIZER_ITERATOR_HPP_ */
