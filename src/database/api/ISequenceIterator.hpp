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

#ifndef _ISEQUENCE_ITERATOR_HPP_
#define _ISEQUENCE_ITERATOR_HPP_

/********************************************************************************/

#include "Iterator.hpp"
#include "ISequence.hpp"
#include "IAlphabet.hpp"
#include "types.hpp"
#include <vector>

/********************************************************************************/
namespace database {
/********************************************************************************/

/** Forward references. */
class ISequenceBuilder;

/********************************************************************************/
class ISequenceIterator : public dp::Iterator<const ISequence*>
{
public:

    /** Set the builder used for creating ISequence instances during iteration. */
    virtual void setBuilder (ISequenceBuilder* mutator) = 0;

    /** Method that clones the instance. */
    virtual ISequenceIterator* clone () = 0;

protected:

    /** Get the builder used for creating ISequence instances during iteration. */
    virtual ISequenceBuilder* getBuilder() const = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ISEQUENCE_ITERATOR_HPP_  */
