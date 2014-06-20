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

/** \file AbstractSequenceIterator.hpp
 *  \brief Implementation of a part of ISequenceIterator interface
 *  \date 07/11/2011
 *  \author edrezen
 *
 *  The AbstractSequenceIterator implements some parts of the ISequenceIterator
 *  interface and can be therefore used as a common base for concrete implementation
 *  of ISequenceIterator interface.
 */

#ifndef _ABSTRACT_ITERATOR_HPP_
#define _ABSTRACT_ITERATOR_HPP_

/********************************************************************************/

#include <database/api/ISequenceIterator.hpp>
#include <database/api/ISequenceBuilder.hpp>

/********************************************************************************/
namespace database {
/** \brief Implementation of concepts related to genomic databases. */
namespace impl {
/********************************************************************************/

/** \brief Implementation of a part of ISequenceIterator interface
 *
 *   This class provides some parts of the ISequenceIterator interface and can be
 *   therefore be used as a common ancestor for actual ISequenceIterator implementors.
 *
 *   In particular, it implements the methods that belong to ISequenceIterator and not
 *   to Iterator. It is still an abstract class since none of the iterating methods are
 *   implemented.
 */
class AbstractSequenceIterator : public ISequenceIterator
{
public:

    /** Constructor. */
    AbstractSequenceIterator ();

    /** Destructor. */
    virtual ~AbstractSequenceIterator () { setBuilder(0); }

    /** \copydoc ISequenceIterator::setBuilder */
    void setBuilder (ISequenceBuilder* builder);

    /** Set the encoding scheme.
     * \param[in] encoding : the encoding scheme
     */
    void     setEncoding (Encoding encoding)    { _encoding = encoding; }

    /** Get the encoding scheme.
     * \return the encoding scheme
     */
    Encoding getEncoding ()                     { return _encoding; }

    /** \copydoc ISequenceIterator::getId */
    std::string getId () { return _id; }

    /** \copydoc ISequenceIterator::transformComment */
    std::string transformComment (const char* comment) {return comment; };

protected:

    /** \copydoc ISequenceIterator::getBuilder */
    ISequenceBuilder* getBuilder ()  const  { return _sequenceBuilder; }

    /** */
    void setId (const std::string& id)  { _id = id; }

private:
    /** Reference to the associated sequence builder (if any). */
    ISequenceBuilder* _sequenceBuilder;

    /** Encoding scheme. */
    Encoding          _encoding;

    /** The identifier string. */
    std::string _id;
};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _ABSTRACT_ITERATOR_HPP_ */
