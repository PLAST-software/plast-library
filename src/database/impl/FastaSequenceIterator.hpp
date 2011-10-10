/*
 * Database.hpp
 *
 *  Created on: 7 juin 2011
 *      Author: edrezen
 */

#ifndef _FASTA_ITERATOR_HPP_
#define _FASTA_ITERATOR_HPP_

/********************************************************************************/

#include "AbstractSequenceIterator.hpp"
#include "FileLineIterator.hpp"
#include "types.hpp"

/********************************************************************************/
namespace database {
/********************************************************************************/

class FastaSequenceIterator : public AbstractSequenceIterator
{
public:

    FastaSequenceIterator (
        const char* filename,
        size_t commentMaxSize=64,
        u_int64_t offset0 = 0,
        u_int64_t offset1 = 0
    );
    virtual ~FastaSequenceIterator ();

    /** Implementation of Iterator interface. */
    void first();
    dp::IteratorStatus next();
    bool isDone()  { return _isDone;  }
    ISequence* currentItem() { return getBuilder()->getSequence(); }

private:
    size_t _commentMaxSize;

    dp::FileLineIterator _fileIterator;
    bool _isDone;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _FASTA_ITERATOR_HPP_ */
