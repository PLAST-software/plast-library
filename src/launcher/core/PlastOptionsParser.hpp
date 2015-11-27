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

/** \file PlastOptionsParser.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Define an option parser for PLAST options
 */

#ifndef _PLAST_OPTIONS_HPP
#define _PLAST_OPTIONS_HPP

/********************************************************************************/

#include <misc/impl/OptionsParser.hpp>

/********************************************************************************/
/** \brief PLAST command line. */
namespace launcher {
/** \brief Tools (command line options management) */
namespace core     {
/********************************************************************************/

/** \brief Specific parser for PLAST command line options
 *
 * This implementation defines all the command line options PLAST should recognize.
 */
class PlastOptionsParser : public misc::impl::OptionsParser
{
public:

    /** Constructor. */
    PlastOptionsParser ();

private:
    void build ();
};

/********************************************************************************/
}}  /* end of namespaces. */
/********************************************************************************/

#endif /* _PLAST_OPTIONS_HPP */
