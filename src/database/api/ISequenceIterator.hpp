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

/** \file ISequenceIterator.hpp
 *  \brief Iteration of ISequence instances.
 *  \date 07/11/2011
 *  \author edrezen
 *
 * We define here a subclass of Iterator that is able to iterate over ISequence
 * instances.
 *
 * Such iterators are likely to be created by ISequenceDatabase objects.
 */

#ifndef _ISEQUENCE_ITERATOR_HPP_
#define _ISEQUENCE_ITERATOR_HPP_

/********************************************************************************/

#include <designpattern/api/Iterator.hpp>
#include <database/api/ISequence.hpp>
#include <database/api/IAlphabet.hpp>

/********************************************************************************/
/** \brief Definition of concepts related to genomic databases. */
namespace database {
/********************************************************************************/

/* Forward references. */
class ISequenceBuilder;

/********************************************************************************/

/** \brief Iteration of ISequence objects
 *
 *  This is merely a subclass of our Iterator concept. Note that the iterated objects
 *  are of type const ISequence* and therefore can't be modified.
 *
 *  Such an iterator is able to clone itself.
 *
 *  Note the appearance of ISequenceBuilder; during iteration, an iterator can call
 *  such a builder for telling it that it has found some information. For instance,
 *  during a FASTA iteration, the iterator can fill a memory cache handled by a builder.
 */
class ISequenceIterator : public dp::Iterator<const ISequence*>
{
public:

    /** Set the builder used for creating ISequence instances during iteration.
     * \param builder : associated builder
     */
    virtual void setBuilder (ISequenceBuilder* builder) = 0;

    /** Method that clones the instance.
     * \return the cloned iterator
     */
    virtual ISequenceIterator* clone () = 0;

    /** Get some unique identifier for the iterator
     * \return the identifier.
     */
    virtual std::string getId () = 0;

protected:

    /** Get the builder used for creating ISequence instances during iteration.
     * \return the associated builder if any.
     */
    virtual ISequenceBuilder* getBuilder() const = 0;
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ISEQUENCE_ITERATOR_HPP_  */
