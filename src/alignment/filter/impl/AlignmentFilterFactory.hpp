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

/** \file AlignmentFilterXML.hpp
 *  \brief Concepts about alignments filtering.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ALIGNMENT_FILTER_FACTORY_HPP_
#define _ALIGNMENT_FILTER_FACTORY_HPP_

/********************************************************************************/

#include <alignment/filter/impl/AbstractAlignmentFilterFactory.hpp>

/********************************************************************************/
namespace alignment {
namespace filter    {
namespace impl      {
/********************************************************************************/

/** \brief Definition of an alignment filtering manager
 */
class AlignmentFilterFactory : public AbstractAlignmentFilterFactory
{
public:

    /** */
    AlignmentFilterFactory () {}

    /** */
    virtual ~AlignmentFilterFactory () {}

    /** */
    IAlignmentFilter* createFilter (const char* name, ...);

private:

};

/********************************************************************************/
}}}; /* end of namespaces. */
/********************************************************************************/

#endif /* _ALIGNMENT_FILTER_FACTORY_HPP_ */
