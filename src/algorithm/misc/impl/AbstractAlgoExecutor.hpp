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

#ifndef _ABSTRACT_ALGO_EXECUTOR_HPP_
#define _ABSTRACT_ALGO_EXECUTOR_HPP_

/********************************************************************************/

#include "IAlgoExecutor.hpp"
#include "IAlgoInitializer.hpp"

/********************************************************************************/
namespace algo  {
/********************************************************************************/

class AbstractAlgoExecutor : public IAlgoExecutor
{
public:

    AbstractAlgoExecutor (IAlgoConfigurator* config);

    virtual ~AbstractAlgoExecutor();

protected:
    IAlgoConfigurator* _config;
    void setConfig (IAlgoConfigurator* config);
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ABSTRACT_ALGO_EXECUTOR_HPP_ */
