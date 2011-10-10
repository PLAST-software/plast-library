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

/*****************************************************************************
 *   Operating System factory.
 *****************************************************************************/

#ifndef I_OS_FACTORY_HPP_
#define I_OS_FACTORY_HPP_

/********************************************************************************/

#include "IThread.hpp"
#include "ITime.hpp"

/********************************************************************************/
namespace os {
/********************************************************************************/

/** */
class IFactory
{
public:

    virtual IThreadFactory& getThreadFactory () = 0;
    virtual ITime&          getTimeFactory   () = 0;
};


/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* I_OS_FACTORY_HPP_ */
