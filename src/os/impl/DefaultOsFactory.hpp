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

#ifndef DEFAULT_OS_FACTORY_HPP_
#define DEFAULT_OS_FACTORY_HPP_

/********************************************************************************/

#include "IOsFactory.hpp"

/********************************************************************************/
namespace os {
/********************************************************************************/

/** */
class DefaultFactory : public IFactory
{
public:

    virtual ~DefaultFactory ()  {}

    static DefaultFactory& singleton ()  { static DefaultFactory instance;  return instance; }

    IThreadFactory& getThreadFactory ();
    ITime&          getTimeFactory   ();
};


/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* I_OS_FACTORY_HPP_ */
