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

/** \file IResource.hpp
 *  \date 07/11/2011
 *  \author edrezen
 *  \brief Operating System common abstraction.
 */

#ifndef IRESOURCE_HPP_
#define IRESOURCE_HPP_

/********************************************************************************/

#include <designpattern/api/SmartPointer.hpp>

/********************************************************************************/
/** \brief Operating System abstraction layer */
 namespace os {
/********************************************************************************/

/** \brief Abstraction of what an operating resource is.
 *
 *  We define here just a name for grouping os-related interfaces.
 *
 *  Note: we could have inherited from dp::SmartPointer; the point here is to
 *  have no dependency from 'os' namespace to 'dp' namespace.
 *
 *  The consequence is that IResource instances have to be deallocated with
 *  traditional 'delete'.
 */
class IResource // :  public dp::SmartPointer
{
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* IRESOURCE_HPP_ */
