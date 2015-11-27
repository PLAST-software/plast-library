/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.3, released November 2015                                     *
 *   Copyright (c) 2009-2015 Inria-Cnrs-Ens                                  *
 *                                                                           *
 *   PLAST is free software; you can redistribute it and/or modify it under  *
 *   the Affero GPL ver 3 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   Affero GPL ver 3 License for more details.                              *
 *****************************************************************************/

/** \file IAlignmentContainerModel.hpp
 *  \brief Interface for some alignments container.
 *  \date 07/11/2011
 *  \author edrezen
 *
 *  We define here some interface that represent a hierarchical view of
 *  the IAlignmentContainer interface
 */

#ifndef _IALIGNMENT_CONTAINER_MODEL_HPP_
#define _IALIGNMENT_CONTAINER_MODEL_HPP_

/********************************************************************************/

#include <designpattern/api/SmartPointer.hpp>
#include <alignment/core/api/Alignment.hpp>

/********************************************************************************/
namespace alignment {
namespace core      {
/********************************************************************************/

/** \brief Define an iterator that iterates alignments information
 */
class ISubjectLevel : public dp::SmartIterator<Alignment*>
{
public:

    /** \return the subject sequence */
    virtual const database::ISequence* getSequence () = 0;
};

/********************************************************************************/

/** \brief Define an iterator that iterates subject information
 */
class IQueryLevel : public dp::SmartIterator<ISubjectLevel*>
{
public:

    /** \return the query sequence */
    virtual const database::ISequence* getSequence () = 0;
};

/********************************************************************************/

/** \brief Define an iterator that iterates query information
 */
class IRootLevel : public dp::SmartIterator<IQueryLevel*>
{
};

/********************************************************************************/
}}; /* end of namespaces. */
/********************************************************************************/

#endif /* _IALIGNMENT_CONTAINER_MODEL_HPP_ */
