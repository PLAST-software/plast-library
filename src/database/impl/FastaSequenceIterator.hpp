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
