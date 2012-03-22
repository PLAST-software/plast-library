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

/** \file IAlignmentContainerModel.hpp
 *  \brief Interface for some alignments container.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _IALIGNMENT_CONTAINER_MODEL_HPP_
#define _IALIGNMENT_CONTAINER_MODEL_HPP_

/********************************************************************************/

#include <designpattern/api/SmartPointer.hpp>
#include <alignment/core/api/Alignment.hpp>

/** Forward declarations. */

/********************************************************************************/
namespace alignment {
namespace core      {
/********************************************************************************/

class ISubjectLevel : public dp::SmartIterator<Alignment*>
{
public:
    virtual const database::ISequence* getSequence () = 0;
};

/********************************************************************************/

class IQueryLevel : public dp::SmartIterator<ISubjectLevel*>
{
public:
    virtual const database::ISequence* getSequence () = 0;
};

/********************************************************************************/

class IRootLevel : public dp::SmartIterator<IQueryLevel*>
{
};

/********************************************************************************/
}}; /* end of namespaces. */
/********************************************************************************/

#endif /* _IALIGNMENT_CONTAINER_MODEL_HPP_ */
