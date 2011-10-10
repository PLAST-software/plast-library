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

#include "AbstractAlgoExecutor.hpp"

#include <stdio.h>
#define DEBUG(a)  //printf a

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AbstractAlgoExecutor::AbstractAlgoExecutor (IAlgoConfigurator* config)
    : _config (0)
{
    setConfig (config);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AbstractAlgoExecutor::~AbstractAlgoExecutor()
{
    setConfig (0);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AbstractAlgoExecutor::setConfig (IAlgoConfigurator* config)
{
    if (_config != 0)  { _config->forget (); }
    _config = config;
    if (_config != 0)  { _config->use ();    }
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
