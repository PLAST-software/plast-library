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

#include <alignment/filter/impl/AlignmentFilterFactory.hpp>

#include <designpattern/impl/XmlReader.hpp>

#include <string>
#include <iostream>
#include <fstream>

#include <stdarg.h>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace dp;
using namespace dp::impl;

/********************************************************************************/
namespace alignment {
namespace filter    {
namespace impl      {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IAlignmentFilter* AlignmentFilterFactory::createFilter (const char* name, ...)
{
    IAlignmentFilter* result = 0;

    /** We build the arguments as a vector from the ellipsis. */
    std::vector<std::string> args;
    va_list ap;
    va_start(ap, name);
    for (const char* val=0; (val = va_arg(ap, const char*)) != 0 ; )  { args.push_back (val); }

    /** We get a new filter from the provided name and the arguments. */
    result = clone (name, args);

    /** We return the result. */
    return result;
}

/********************************************************************************/
}}}; /* end of namespaces. */
/********************************************************************************/
