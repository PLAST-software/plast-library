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

#include "AlgoUngapAlignment.hpp"

#include "UngapHitIterator.hpp"
#include "UngapHitIteratorOptim.hpp"
#include "UngapHitIteratorSSE8.hpp"
#include "UngapHitIteratorSSE16.hpp"

using namespace std;
using namespace os;
using namespace database;
using namespace seed;
using namespace indexation;
using namespace algo;

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
AlgoUngapAlignment::AlgoUngapAlignment ()
{
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AlgoUngapAlignment::~AlgoUngapAlignment ()
{
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
IHitIterator* AlgoUngapAlignment::createHitIterator (
    IHitIterator*    inputIterator,
    ISeedModel*      model,
    IScoreMatrix*    scoreMatrix,
    IParameters*     parameters
)
{
    //return new UngapHitIterator (inputIterator, model, scoreMatrix, neighbourLength, threshold);
    //return new UngapHitIteratorSSE8 (inputIterator, model, scoreMatrix, neighbourLength, threshold);
    return new UngapHitIteratorSSE16 (inputIterator, model, scoreMatrix, parameters, 0);
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
