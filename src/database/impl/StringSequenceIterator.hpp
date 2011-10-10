/*
 * Database.hpp
 *
 *  Created on: 7 juin 2011
 *      Author: edrezen
 */

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
