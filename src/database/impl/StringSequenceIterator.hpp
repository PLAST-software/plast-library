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

#ifndef _STRING_SEQUENCE_ITERATOR_HPP_
#define _STRING_SEQUENCE_ITERATOR_HPP_

/********************************************************************************/

#include "AbstractSequenceIterator.hpp"
#include <string>
#include <vector>

/********************************************************************************/
namespace database {
/********************************************************************************/

class StringSequenceIterator : public AbstractSequenceIterator
{
public:

    StringSequenceIterator (size_t nb, ...);
    virtual ~StringSequenceIterator ();

    /** Implementation of Iterator interface. */
    void first()   { _currentIdx=0;  updateItem(); }
    dp::IteratorStatus next()    { _currentIdx++;  updateItem();  return dp::ITER_UNKNOWN; }
    bool isDone()  { return _currentIdx >= _strings.size() ;}
    ISequence* currentItem() { return getBuilder()->getSequence(); }

private:

    std::vector<std::string> _strings;
    size_t _currentIdx;

    void updateItem ();
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _STRING_SEQUENCE_ITERATOR_HPP_ */
