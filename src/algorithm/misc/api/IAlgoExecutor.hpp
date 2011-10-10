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

#ifndef _IALGO_EXECUTOR_HPP_
#define _IALGO_EXECUTOR_HPP_

/********************************************************************************/

#include "SmartPointer.hpp"

/********************************************************************************/
namespace algo  {
/********************************************************************************/

class IAlgoExecutor : public dp::SmartPointer
{
public:

    /** Template Method. */
    void run (void)
    {
        preProcess  ();
        process     ();
        postProcess ();
    }

protected:

    virtual void preProcess  (void)  {};
    virtual void process     (void) = 0;
    virtual void postProcess (void)  {}
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _IALGO_EXECUTOR_HPP_ */
