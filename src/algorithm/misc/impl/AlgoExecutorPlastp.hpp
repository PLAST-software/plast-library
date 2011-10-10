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

#ifndef _ALGO_EXECUTOR_PLASTP_HPP_
#define _ALGO_EXECUTOR_PLASTP_HPP_

/********************************************************************************/

#include "AbstractAlgoExecutor.hpp"

/********************************************************************************/
namespace algo  {
/********************************************************************************/

class AlgoExecutorPlastp : public AbstractAlgoExecutor
{
public:

    AlgoExecutorPlastp (IAlgoConfigurator* config);
    virtual ~AlgoExecutorPlastp();

protected:

    void process (void);
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _ALGO_EXECUTOR_PLASTP_HPP_ */
