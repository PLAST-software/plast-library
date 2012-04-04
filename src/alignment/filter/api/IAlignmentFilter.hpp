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

/** \file IAlignmentFilter.hpp
 *  \brief Concepts about alignments filtering.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _IALIGNMENT_FILTER_HPP_
#define _IALIGNMENT_FILTER_HPP_

/********************************************************************************/

#include <designpattern/api/SmartPointer.hpp>
#include <designpattern/api/IProperty.hpp>
#include <alignment/core/api/Alignment.hpp>

#include <string>
#include <vector>

/********************************************************************************/
namespace alignment {
namespace filter    {
/********************************************************************************/

/** \brief Definition of an alignment filtering
 */
class IAlignmentFilter : public dp::SmartPointer
{
public:

    virtual bool isOk (const core::Alignment& align) const = 0;

    virtual IAlignmentFilter* clone (const std::vector<std::string>& args) = 0;

    virtual dp::IProperties* getProperties () = 0;

    virtual std::string getName () = 0;

    virtual std::string toString () = 0;

    virtual std::string getTitle () = 0;
    virtual void setTitle (const std::string& title) = 0;
};

/********************************************************************************/
/** \brief Definition of an alignment filtering manager
 */
class IAlignmentFilterFactory : public dp::SmartPointer
{
public:

    virtual IAlignmentFilter* createFilter (const char* name, ...) = 0;
};

/********************************************************************************/
}}; /* end of namespaces. */
/********************************************************************************/

#endif /* _IALIGNMENT_FILTER_HPP_ */
