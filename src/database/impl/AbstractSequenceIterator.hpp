/*
 * Database.hpp
 *
 *  Created on: 7 juin 2011
 *      Author: edrezen
 */

#ifndef _ABSTRACT_ITERATOR_HPP_
#define _ABSTRACT_ITERATOR_HPP_

/********************************************************************************/

#include "ISequenceIterator.hpp"
#include "ISequenceBuilder.hpp"

/********************************************************************************/
namespace database {
/********************************************************************************/

class AbstractSequenceIterator : public ISequenceIterator
{
public:

    AbstractSequenceIterator ();
    virtual ~AbstractSequenceIterator () { setBuilder(0); }

    void setBuilder (ISequenceBuilder* builder);

    void     setEncoding (Encoding encoding)    { _encoding = encoding; }
    Encoding getEncoding ()                     { return _encoding; }

    ISequenceIterator* clone ()  { return 0; }

protected:
    ISequenceBuilder* getBuilder ()  const  { return _sequenceBuilder; }

private:
    ISequenceBuilder* _sequenceBuilder;
    Encoding          _encoding;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ABSTRACT_ITERATOR_HPP_ */
