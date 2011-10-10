/*
 * Database.hpp
 *
 *  Created on: 7 juin 2011
 *      Author: edrezen
 */

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
