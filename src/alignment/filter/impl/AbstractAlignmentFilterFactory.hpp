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

/** \file AbstractAlignmentFilterFactory.hpp
 *  \brief Concepts about alignments filtering.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ABSTRACT_ALIGNMENT_FILTER_FACTORY_HPP_
#define _ABSTRACT_ALIGNMENT_FILTER_FACTORY_HPP_

/********************************************************************************/

#include <alignment/filter/api/IAlignmentFilter.hpp>
#include <map>
#include <list>

/********************************************************************************/
namespace alignment {
namespace filter    {
namespace impl      {
/********************************************************************************/

/** \brief Definition of an alignment filtering manager
 */
class AbstractAlignmentFilterFactory : public IAlignmentFilterFactory
{
public:

    /** */
    AbstractAlignmentFilterFactory ();

    /** */
    virtual ~AbstractAlignmentFilterFactory ();

protected:

    /** We need a map that gives a IAlignmentFilter given a name. */
    std::map<std::string,IAlignmentFilter*> _filtersMap;

    /** We need a map that gives a IAlignmentFilter given a name. */
    std::list<IAlignmentFilter*> _filtersList;

    /** */
    IAlignmentFilter* clone (const std::string& name, const std::vector<std::string>& args);
};

/********************************************************************************/
}}}; /* end of namespaces. */
/********************************************************************************/

#endif /* _ABSTRACT_ALIGNMENT_FILTER_FACTORY_HPP_ */
