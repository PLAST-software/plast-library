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
